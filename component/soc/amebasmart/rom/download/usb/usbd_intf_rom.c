/**
  ******************************************************************************
  * @file    usbd_intf_rom.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides the functionalities of USB download interface
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include "usbd_hal_rom.h"
#include "usbd_intf_rom.h"
#include "usbd_cdc_rom.h"
#include "usbd_core_rom.h"

/* Private defines -----------------------------------------------------------*/

#define UBURN_LOOPBACK_TEST				0U

#define UBURN_STX						0x02U
#define UBURN_EOT						0x04U
#define UBURN_ACK						0x06U
#define UBURN_NAK						0x15U
#define UBURN_ESC						0x1BU
#define UBURN_BAUDSET					0x05U

#define UBURN_STX_PACKET_LEN			2056U
#define UBURN_STX_DATA_LEN				2048U
#define UBURN_STX_PACKET_NO_OFFSET		1U
#define UBURN_STX_ADDR_OFFSET			3U
#define UBURN_STX_DATA_OFFSET			7U
#define UBURN_STX_CHK_OFFSET			(UBURN_STX_PACKET_LEN - 1)

/* Private types -------------------------------------------------------------*/

typedef struct {
	__IO u8 Aborted;
	__IO u8 Attached;
	__IO u8 ReadyToStx;
	__IO u8 StxDone;
	__IO u8 CurrentPacketNo;
	USBD_CDC_LineCodingTypeDef LineCoding;
} USBD_CDC_CtrlTypeDef;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 CDC_Itf_Init(void);
static u8 CDC_Itf_DeInit(void);
static u8 CDC_Itf_Control(u8 cmd, u8 *pbuf, u16 length);
#if UBURN_LOOPBACK_TEST
static u8 CDC_Itf_Transmit(u8 *buf, u16 len);
#endif
static u8 CDC_Itf_TransmitByte(u8 data);
static u8 CDC_Itf_RxProcess(u8 *buf, u16 len);
static u8 CDC_Itf_Receive(u8 *pbuf, u32 *Len);

/* Private variables ---------------------------------------------------------*/

USBD_DATA_SECTION
static USBD_CDC_ItfTypeDef USBD_CDC_fops = {
	CDC_Itf_Init,
	CDC_Itf_DeInit,
	CDC_Itf_Control,
	CDC_Itf_Receive
};

USBD_DATA_SECTION
static u8 *USBD_RxBuffer = (u8 *)(CDC_RX_BUF_ADDR); /* Received data from USB are stored in this buffer */
USBD_DATA_SECTION
static u8 *USBD_TxBuffer = (u8 *)(CDC_TX_BUF_ADDR); /* Data to be transmitted over USB are stored in this buffer */

USBD_BSS_SECTION
static USBD_CDC_CtrlTypeDef USBD_Ctrl;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_Init(void)
{
	USBD_CDC_CtrlTypeDef *ctrl = &USBD_Ctrl;
	USBD_CDC_LineCodingTypeDef *coding = &ctrl->LineCoding;

	ctrl->CurrentPacketNo = 1;

	coding->bitrate = 150000;
	coding->format = 0x00;
	coding->paritytype = 0x00;
	coding->datatype = 0x08;

	USBD_CDC_SetTxBuffer(USBD_TxBuffer, 0);
	USBD_CDC_SetRxBuffer(USBD_RxBuffer);

	return (HAL_OK);
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_DeInit(void)
{
	USBD_CDC_CtrlTypeDef *ctrl = &USBD_Ctrl;

	ctrl->Attached = 0;
	ctrl->Aborted = 0;
	ctrl->CurrentPacketNo = 1;
	ctrl->ReadyToStx = 0;
	ctrl->StxDone = 0;

	return (HAL_OK);
}

#if UBURN_LOOPBACK_TEST

/**
  * @brief  Transmit data to CDC host
  * @param  buf: Data buffer
  * @param  lem: Data length to be transmitted
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW or HAL_BUSY
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_Transmit(u8 *buf, u16 len)
{
	USBD_CDC_SetTxBuffer(buf, len);
	return USBD_CDC_TransmitPacket();
}

#endif

/**
  * @brief  Transmit a byte to CDC host
  * @param  data: Data to be transmitted
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW or HAL_BUSY
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_TransmitByte(u8 data)
{
	USBD_TxBuffer[0] = data;
	USBD_CDC_SetTxBuffer(USBD_TxBuffer, 1);
	return USBD_CDC_TransmitPacket();
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_Receive(u8 *buf, u32 *len)
{
	u32 bytes = *len;

	//DBG_PRINTF(MODULE_USB_OTG, LEVEL_TRACE, "CDC_Itf_Receive %d\n", *len);

#if UBURN_LOOPBACK_TEST
	CDC_Itf_Transmit(buf, *len);
#else
	CDC_Itf_RxProcess(buf, bytes);
#endif

	return USBD_CDC_ReceivePacket();
}

/**
  * @brief  Handle the CDC class control requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_Control(u8 cmd, u8 *pbuf, u16 length)
{
	UNUSED(length);
	USBD_CDC_CtrlTypeDef *ctrl = &USBD_Ctrl;
	USBD_CDC_LineCodingTypeDef *coding = &ctrl->LineCoding;

	switch (cmd) {
	case CDC_SEND_ENCAPSULATED_COMMAND:
		/* Do nothing */
		break;

	case CDC_GET_ENCAPSULATED_RESPONSE:
		/* Do nothing */
		break;

	case CDC_SET_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_GET_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_CLEAR_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_SET_LINE_CODING:
		coding->bitrate = (u32)(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
		coding->format = pbuf[4];
		coding->paritytype = pbuf[5];
		coding->datatype = pbuf[6];
		break;

	case CDC_GET_LINE_CODING:
		pbuf[0] = (u8)(coding->bitrate & 0xFF);
		pbuf[1] = (u8)((coding->bitrate >> 8) & 0xFF);
		pbuf[2] = (u8)((coding->bitrate >> 16) & 0xFF);
		pbuf[3] = (u8)((coding->bitrate >> 24) & 0xFF);
		pbuf[4] = coding->format;
		pbuf[5] = coding->paritytype;
		pbuf[6] = coding->datatype;
		break;

	case CDC_SET_CONTROL_LINE_STATE:
		/*
		B0:	bmRequestType, 0x21
		B1:	bRequestCode, 0x22
		B2-B3:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		B4-B5:	wIndex, Interface
		B6-B7:	WLength, 0
		*/
		if ((pbuf[2] & 0x02) == 0x02) {
			/* Open VCOM port: RTS = Activate */
			ctrl->Attached = 1U;
		}
		break;

	case CDC_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		break;
	}

	return (HAL_OK);
}

/**
  * @brief  Write image frame
  * @param  dest : Destination
  * @param  src  : Frame buffer
  * @param  len  : Frame length
  * @retval None
  */
USBD_TEXT_SECTION
static void CDC_Itf_WriteImage(u32 dest, const u8 *src,  u32 len)
{
	u32 index = 0;

	/* Only allow to write hs sram, to avoid attack */
	if ((!IS_HS_SRAM_S_ADDR(dest)) || (!IS_HS_SRAM_S_ADDR(dest + len - 1))) {
		return;
	}

	for (index = 0; index < (len / 4); index++) {
		*((u32 *)(dest + index * 4)) = (*((u32 *)(src + index * 4)));
	}

	DCache_CleanInvalidate(dest, len);
}

/**
  * @brief  RX data process
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
USBD_TEXT_SECTION
static u8 CDC_Itf_RxProcess(u8 *buf, u16 len)
{
	u8 ret = HAL_ERR_HW;
	u8 opcode;
	u8 packet_no;
	u8 packet_no_inv;
	u8 checksum = 0;
	u8 response = UBURN_NAK;
	u8 skip_response = 0;
	u32 i;
	u32 addr;
	USBD_CDC_CtrlTypeDef *ctrl = &USBD_Ctrl;

	if (len == 0) {
		return HAL_ERR_HW;
	}

	opcode = buf[0];

	switch (opcode) {
	case UBURN_BAUDSET:
		if (len == 2) {
			ctrl->ReadyToStx = 1;
			ret = HAL_OK;
			response = UBURN_ACK;
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "BAUDSET");
			USBD_Debug(2, 0);
		}
		break;
	case UBURN_STX:
		if (ctrl->ReadyToStx == 1) {
			if (len == UBURN_STX_PACKET_LEN) {
				packet_no = buf[UBURN_STX_PACKET_NO_OFFSET];
				packet_no_inv = buf[UBURN_STX_PACKET_NO_OFFSET + 1];
				if ((((~packet_no) & 0xFF) ^ packet_no_inv) == 0) {
					for (i = UBURN_STX_ADDR_OFFSET; i < UBURN_STX_CHK_OFFSET; i++) {
						checksum += buf[i];
					}
					if (checksum == buf[UBURN_STX_CHK_OFFSET]) {
						if (packet_no == ctrl->CurrentPacketNo) {
							ctrl->CurrentPacketNo = (ctrl->CurrentPacketNo + 1) & 0xFF;
							addr = buf[UBURN_STX_ADDR_OFFSET] + (buf[UBURN_STX_ADDR_OFFSET + 1] << 8) + (buf[UBURN_STX_ADDR_OFFSET + 2] << 16) + (buf[UBURN_STX_ADDR_OFFSET + 3] << 24);
							USBD_Debug(1, 0);
							CDC_Itf_WriteImage(addr, buf + UBURN_STX_DATA_OFFSET, UBURN_STX_DATA_LEN);
							USBD_Debug(1, 0);
							ret = HAL_OK;
							response = UBURN_ACK;
							DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "STX: packet_no=0x%02X addr=0x%08X size=%d", packet_no, addr, UBURN_STX_PACKET_LEN);
						} else if (packet_no < ctrl->CurrentPacketNo) {
							// Retry
							response = UBURN_ACK;
						} else {
							DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "STX: invalid packet no %d, exepcted %d", packet_no, ctrl->CurrentPacketNo);
						}
					} else {
						DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "STX: checksum error %d, exepcted %d", checksum, buf[UBURN_STX_CHK_OFFSET]);
					}
				} else {
					DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "STX: invalid packet no %d - %d", packet_no, packet_no_inv);
				}
			} else {
				DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "STX: invalid payload length %d for STX opcode", len);
			}
		} else {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "STX: not ready for STX yet");
		}
		break;
	case UBURN_EOT:
		if (len == 1) {
			ret = HAL_OK;
			response = UBURN_ACK;
			ctrl->StxDone = 1;
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "EOT");
			USBD_Debug(5, 1);
		} else {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "EOT: invalid payload length %d", len);
		}
		break;
	case UBURN_ESC:
		if (len == 1) {
			ret = HAL_OK;
			response = UBURN_ACK;
			ctrl->Aborted = 1U;
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "ESC");
			USBD_Debug(6, 1);
		} else {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_WARN, "ESC: invalid payload length %d", len);
		}
		break;
	default:
		break;
	}

	if (!skip_response) {
		CDC_Itf_TransmitByte(response);
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Return the USB attached status
  * @param  None
  * @retval Result of the operation: 1 if attached else 0
  */
USBD_WEEK USBD_TEXT_SECTION
int USBD_IsAttached(void)
{
	return USBD_Ctrl.Attached;
}

/**
  * @brief  USB download process
  * @param  None
  * @retval Result of the operation: 1 if Flashloader STX done, 0 if aborted
  */
USBD_WEEK USBD_TEXT_SECTION
int USBD_Process(void)
{
	int ret = 0;

	USBD_Debug_Config(ENABLE);

	while (1) {
		if (USBD_Ctrl.StxDone) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "Done\n");
			USB_DELAY_US(10000);	// Wait transfer done
			ret = 0;
			break;
		}
		if (USBD_Ctrl.Aborted) {
			DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "Aborted\n");
			USB_DELAY_US(10000);	// Wait transfer done
			ret = 1;
			break;
		}
		USB_DELAY_US(50000);
	}

	return ret;
}

/**
  * @brief  USB download initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
USBD_WEEK USBD_TEXT_SECTION
int USBD_Init(void)
{
	int ret;
	u32 is_full_speed;
	USBD_SpeedTypeDef speed = USBD_SPEED_HIGH;

	USBD_Debug_Config(DISABLE);

	DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "Init\n");

	USBD_DebugInit();

	is_full_speed = SYSCFG_OTP_USBLoadFullSpeed();
	if (is_full_speed) {
		speed = USBD_SPEED_FULL;
	}

	ret = USBD_Core_Init(&USBD_CDC_fops, speed);
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Init fail: %d\n", ret);
	}

	return ret;
}

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
USBD_WEEK USBD_TEXT_SECTION
int USBD_DeInit(void)
{
	int ret;

	DBG_PRINTF(MODULE_USB_OTG, LEVEL_INFO, "Deinit\n");

	ret = USBD_Core_DeInit();
	if (ret != HAL_OK) {
		DBG_PRINTF(MODULE_USB_OTG, LEVEL_ERROR, "Deinit fail: %d\n", ret);
	}

	return ret;
}

