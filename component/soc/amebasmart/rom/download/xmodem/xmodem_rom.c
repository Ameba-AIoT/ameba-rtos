/*
 *  xModem protocol
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "ameba_soc.h"
#include "xmodem_rom.h"
#include "xmodem_uart_rom.h"
#include "xmodem_update_rom.h"
#include "usbd_intf_rom.h"

//---------------------------------
#define XDEBUG_GPIO				_PA_6

/* common baud rate table*/
HAL_ROM_DATA_SECTION
const u32 BAUDRATE_TABLE[] = {
	/*baudrate*/
	110,
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	14400,
	19200,
	28800,
	38400,
	57600,
	76800,
	115200,
	128000,
	153600,
	230400,
	380400,
	460800,
	500000,
	921600,
	1000000,
	1382400,
	1444400,
	1500000,
	1843200,
	2000000,
	2100000,
	2764800,
	3000000,
	3250000,
	3692300,
	3750000,
	4000000,
	6000000,
	0xffffffff,
};

HAL_ROM_BSS_SECTION_BANK XMODEM_CTRL xmodem_ctrl;

/* Flag indicating whether USB download is enabled. */
HAL_ROM_BSS_SECTION_BANK u32 usbd_enabled;

/* Flag indicating whether USB download is deactivated.
 * Once Xmodem handshake done, USB download shall be deactivated.
 */
HAL_ROM_BSS_SECTION_BANK static u32 usbd_deactivated;

//---------------------------
HAL_ROM_TEXT_SECTION
static void xmodem_debugInit(void)
{
	if ((BKUP_Read(0) & BKUP_BIT_UARTBURN_DEBUG) == 0) {
		return;
	}

	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	Pinmux_Config(XDEBUG_GPIO, PINMUX_FUNCTION_GPIO);

	GPIO_InitTypeDef GPIO_InitStruct_Xmodem;

	GPIO_InitStruct_Xmodem.GPIO_Pin = XDEBUG_GPIO;
	GPIO_InitStruct_Xmodem.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct_Xmodem.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_Init(&GPIO_InitStruct_Xmodem);
}

HAL_ROM_TEXT_SECTION
static void xmodem_debug(u32 code, u32 need_delay)
{
	u32 tmp = code;
	int i;

	if ((BKUP_Read(0) & BKUP_BIT_UARTBURN_DEBUG) == 0) {
		return;
	}

	while (1) {
		i = 0;
		GPIO_WriteBit(XDEBUG_GPIO, 0);
		if (need_delay) {		/*add delay to distinguish debug messages and error messages*/
			while (i++ < 10) {
				asm("nop");
			}
		}
		GPIO_WriteBit(XDEBUG_GPIO, 1);

		tmp--;
		if (tmp == 0) {
			break;
		}
	}
}

HAL_ROM_TEXT_SECTION
static u32 xmodem_inquiry(u8 code)
{
	if (code != ACK) {
		DelayUs(1000);
	}

	//if (code == CAN) {
	//	return TRUE;	/* MUST clear buffer */
	//}

	xmodem_uart_putc((char)code);

	return TRUE;
}

HAL_ROM_TEXT_SECTION
static u32 xmodem_get_first(XMODEM_CTRL *pXMCtrl)
{
	char ch = 0;
	char tmp;
	u32 rStatus = XMODEM_OK;

	if (xmodem_uart_getc_to(&ch, WAIT_FRAME_TIME)) {
		pXMCtrl->rFinish = TRUE;
		rStatus =  XMODEM_TIMEOUT;
		xmodem_debug(3, 1);
		DBG_8195A("xModem: Wait Next Frame Start Timeout\r\n");
		return rStatus;
	}

	ch = ch & 0xff;
	switch (ch) {
	case CAN :
		xmodem_debug(4, 1);
		DBG_8195A("xModem: Get Cancel\r\n");
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_CANCEL;
		break;
	case EOT :
		pXMCtrl->rFinish = TRUE;
		xmodem_inquiry(ACK);
		rStatus = XMODEM_COMPLETE;
		xmodem_debug(5, 1);
		DBG_8195A("xModem: End of Transmit\r\n");
		break;
	case SOH :
		pXMCtrl->expected =  FRAME_SIZE;// - 4; /* we add 4byte load address */
		break;
	case STX :
		pXMCtrl->expected =  FRAME_SIZE_1K;// - 4; /* we add 4byte load address */
		break;
	case ESC :
		xmodem_debug(6, 1);
		DBG_8195A("xModem: Aborted!\r\n");
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_ABORT;
		break;
	default :
		while (xmodem_uart_getc_to(&tmp, WAIT_CHAR_TIME) == 0);
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_TIMEOUT;
		break;
	}

	return rStatus;
}

HAL_ROM_TEXT_SECTION
static u32 xmodem_get_others(XMODEM_CTRL *pXMCtrl)
{
	char *xMFrameBuf = FRAME_BUF_ADDR;
	u8 frame_no;
	u8 frame_no_bar;
	u8 is_duplicate;
	char uch = 0;
	char uchk;
	u16	summation;
	u32	j;
	u32 wr_offset = 0;
	u32 rStatus = XMODEM_OK;

	if (xmodem_uart_getc_to((char *)(&frame_no), WAIT_CHAR_TIME)) {
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_TIMEOUT;
		xmodem_debug(7, 1);
		DBG_8195A("xmodem_get_others Timeout #1\r\n");
		return rStatus;
	}

	/* check invert number */
	if (xmodem_uart_getc_to((char *)(&frame_no_bar), WAIT_CHAR_TIME)) {
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_TIMEOUT;
		xmodem_debug(8, 1);
		DBG_8195A("xmodem_get_others Timeout #2\r\n");
		return rStatus;
	}

	rStatus = XMODEM_OK;

	if ((((~frame_no) & 0xff)^frame_no_bar) != 0) {
		xmodem_debug(9, 1);
		DBG_8195A("Incorrect FrameNo %x %x\r\n", frame_no, frame_no_bar);
		rStatus = XMODEM_NAK;
	}

	is_duplicate = 0;
	if (frame_no < pXMCtrl->currentFrame) {
		/* re-transmit packets, just send ACK and ignore it */
		is_duplicate = 1;
	} else if (frame_no != pXMCtrl->currentFrame) {
		rStatus = XMODEM_NAK;
		xmodem_debug(10, 1);
		DBG_8195A("Wrong FrameNo: expect for 0x%x but got 0x%x\r\n", pXMCtrl->currentFrame, frame_no);
	}

	/* get data */
	summation = 0;
	for (j = 0; j < pXMCtrl->expected; j++) {
		if (xmodem_uart_getc_to((char *)(xMFrameBuf + j), WAIT_CHAR_TIME)) {
			pXMCtrl->rFinish = TRUE;
			rStatus = XMODEM_TIMEOUT;
			xmodem_debug(11, 1);
			DBG_8195A("xmodem_get_others Timeout #3\r\n");
			return rStatus;
		}
		summation += *(xMFrameBuf + j);
	}

	/* CRC check */
	if (xmodem_uart_getc_to(&uch, WAIT_CHAR_TIME)) {
		pXMCtrl->rFinish = TRUE;
		rStatus = XMODEM_TIMEOUT;
		xmodem_debug(12, 1);
		DBG_8195A("xmodem_get_others Timeout #4\r\n");
		return rStatus;
	}

	uchk = summation & 0xff;
	if (uchk != uch) {
		rStatus = XMODEM_NAK;
		xmodem_debug(13, 1);
		DBG_8195A("Check-Sum Err(%x %x)!\r\n", uch, uchk);
	} else {
		if (!is_duplicate) {
			pXMCtrl->total_frame++;
			wr_offset = *((u32 *)xMFrameBuf); /* 4byte load address */

			xmodem_debug(1, 0);
			xmodem_img_write(xMFrameBuf + 4, wr_offset, pXMCtrl->expected - 4);
		}

		/* somehow the xmodem sender "TeraTerm" will send extra junk bytes if we start the xmodem */
		/* receiver before the xmodem sender start. So, we need to drop those extra data */
		xmodem_uart_clean_rx();
	}

	if (rStatus == XMODEM_OK) {
		pXMCtrl->rFinish = TRUE;
		xmodem_inquiry(ACK);
		if (is_duplicate) {
			/* duplicated frame */
			rStatus = XMODEM_NAK;
		}
	} else if (rStatus == XMODEM_NAK) {
		xmodem_inquiry(NAK);
		pXMCtrl->rFinish = TRUE;
	}

	return rStatus;
}

HAL_ROM_TEXT_SECTION
static u32 xmodem_rx_frame(XMODEM_CTRL *pXMCtrl)
{
	u32 status = 0;

	pXMCtrl->expected = 1;
	pXMCtrl->rFinish = FALSE;

	/* get xmodem first byte */
	status = xmodem_get_first(pXMCtrl);

	xmodem_debug(1, 0);
	/* get following bytes */
	while (pXMCtrl->rFinish == FALSE) {
		status = xmodem_get_others(pXMCtrl);
	}

	xmodem_debug(1, 0);
	/* update rx index */
	if (status == XMODEM_OK) {
		pXMCtrl->currentFrame = (pXMCtrl->currentFrame + 1) & 0xff;
	}

	return status;
}

HAL_ROM_TEXT_SECTION
static u32 xmodem_handshake(void)
{
	char c;
	u8 baud_idx;

HS_again:
	while (1) {
		xmodem_uart_putc(NAK);
		if ((usbd_enabled == 1) && (usbd_deactivated == 0)) {
			/* Check the USB attached status before Xmodem handshake done.
			 * During the Xmodem handshake process, USB ISR will listen on the USB bus,
			 * once virtual serial port open command received form USB bus,
			 * Xmodem shall be deactivated while USB download shall be activated.
			 */
			if (USBD_IsAttached()) {
				/* Switch to USB download mode */
				return XM_RESULT_USB;
			}
		}
		DelayUs(50000);		//delay 50ms
		if (xmodem_uart_readable()) {
			break;
		}
	}

	while (1) {
		if (xmodem_uart_readable()) {
			if (xmodem_uart_getc_to(&c, WAIT_HANDSHAKE_TIME)) {
				goto HS_again;
			}

			switch (c) {
			case BAUDSET:
				xmodem_debug(1, 0);
				if (xmodem_uart_getc_to((char *)(&baud_idx), WAIT_HANDSHAKE_TIME)) {
					goto HS_again;
				}
				xmodem_uart_putc(ACK);
				xmodem_uart_waitbusy();   //wait for tx ACK done
				xmodem_uart_setbaud(BAUDRATE_TABLE[baud_idx]);
				xmodem_debug(1, 0);
				break;
			case BAUDCHK:
				xmodem_debug(2, 0);
				xmodem_uart_putc(ACK);
				xmodem_debug(2, 0);
				if (usbd_enabled == 1) {
					/* Xmodem handshake done, image will be downloaded via Xmodem,
					 * while USB download shall be deactivated.
					 */
					usbd_deactivated = 1;
					USBD_DeInit();
				}
				return XM_RESULT_OK;
				break;

			/* user key ESC to cancel */
			case ESC:
				return XM_RESULT_CAN;
				break;

			default:
				xmodem_uart_clean_rx();
				goto HS_again;
				break;
			}
		}
	}

	return XM_RESULT_OK;
}

HAL_ROM_TEXT_SECTION
u32 xmodem_img_rxbuffer(UNUSED_WARN_DIS u32 baud_rate)
{
	u32 status;
	u32 rxlen;
	u32 finished;
	u32 Temp = 0;

	xmodem_ctrl.currentFrame = 1;
	xmodem_ctrl.total_frame = 0;
	rxlen = 0;

	/* baudrate handshake */
	xmodem_debugInit();
	xmodem_uart_clean_rx();
	Temp = xmodem_handshake();

	/* Cancelled or switch to USB download mode */
	if (Temp != XM_RESULT_OK) {
		return Temp;
	}

	finished = FALSE;
	while (finished == FALSE) {

		xmodem_debug(2, 0);
		status = xmodem_rx_frame(&xmodem_ctrl);

		switch (status) {
		case XMODEM_OK :
			rxlen += xmodem_ctrl.expected;
			break;

		case XMODEM_TIMEOUT :
			/* Clear Rx FIFO and NAK, Xmodem will Try again */
			xmodem_uart_clean_rx();
			xmodem_inquiry(NAK);
			break;

		case XMODEM_COMPLETE :
			finished = TRUE;
			break;

		case XMODEM_NAK:
			break;

		case XMODEM_ABORT :
			rxlen = 0;
			xmodem_inquiry(CAN);
			finished = TRUE;
			rxlen = 0;
			break;
		default :
			xmodem_inquiry(CAN);
			finished = TRUE;
			rxlen = 0;
			break;
		}
	}

	/* wait for tx EOT done */
	xmodem_uart_waitbusy();

	return XM_RESULT_OK;
}
