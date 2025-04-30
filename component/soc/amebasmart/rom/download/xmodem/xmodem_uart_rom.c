/*******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "ameba_soc.h"
#include "xmodem_uart_rom.h"

//#include "pinmap.h"

extern u32 ConfigDebugClose;

HAL_ROM_TEXT_SECTION
static void xmodem_log_close(void)
{
	ConfigDebugClose = 1;
}

HAL_ROM_TEXT_SECTION
static void xmodem_log_open(void)
{
	LOGUART_DiagInit(TRUE);

	ConfigDebugClose = 0;

	//DBG_8195A("xmodem_log_open \n");
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_init(UNUSED_WARN_DIS u8 uart_idx, UNUSED_WARN_DIS u8 pin_mux, u32 baud_rate)
{
	LOGUART_InitTypeDef LOGUART_InitStruct;

	/* UARTLOG is LogUART */
	RCC_PeriphClockCmd(APBPeriph_LOGUART, APBPeriph_LOGUART_CLOCK, ENABLE);
	Pinmux_UartLogCtrl(PINMUX_S0, ON);

	LOGUART_StructInit(&LOGUART_InitStruct);

	/* wait Log_UART tx done, otherwise redundant data will be tx when re-initialize Log_UART */
	LOGUART_WaitTxComplete();

	/* re-initialize Log_UART */
	_LOGUART_Init(LOGUART_DEV, &LOGUART_InitStruct);
	LOGUART_SetBaud(LOGUART_DEV, baud_rate);
	LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ERBI | LOGUART_BIT_ELSI, DISABLE);
	LOGUART_RxCmd(LOGUART_DEV, ENABLE);
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_port_init(u8 uart_idx, u8 pin_mux, u32 baud_rate)
{
	//DBG_8195A("Open xModem Transfer on Log UART...\r\n");

	/* close all log */
	xmodem_log_close();

	xmodem_uart_init(uart_idx, pin_mux, baud_rate);
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_port_deinit(UNUSED_WARN_DIS u8 uart_idx)
{
	xmodem_log_open();

	DBG_8195A("close xModem Transfer ...\r\n");
}

/******************************************************************************
 * READ/WRITE
 ******************************************************************************/
HAL_ROM_TEXT_SECTION
char xmodem_uart_readable(void)
{
	return LOGUART_Readable();
}

HAL_ROM_TEXT_SECTION
char xmodem_uart_writable(void)
{
	if (LOGUART_DEV->LOGUART_UART_LSR & LOGUART_BIT_TP1F_NOT_FULL) {
		return 1;
	} else {
		return 0;
	}
}

HAL_ROM_TEXT_SECTION
char xmodem_uart_getc(void)
{
	return (char)LOGUART_DEV->LOGUART_UART_RBR;
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_putc(char c)
{
	LOGUART_DEV->LOGUART_UART_THRx[0] = c;
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_putdata(u8 *buf, u32 cnt)
{
	u32 idx = 0;

	for (idx = 0; idx < cnt; idx++) {
		while (xmodem_uart_writable() == 0);

		xmodem_uart_putc(*buf++);
	}
}

HAL_ROM_TEXT_SECTION
char xmodem_uart_getc_to(char *pch, u32 timeout)
{
	do {
		if (xmodem_uart_readable()) {
			*pch = xmodem_uart_getc();
			return 0;
		}

		timeout--;
	} while (timeout != 0);

	//DBG_MISC_ERR("xModem: Wait Next Char Timeout\r\n");
	return 1;
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_clean_rx(void)
{
	while (xmodem_uart_readable()) {
		xmodem_uart_getc();
	}
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_waitbusy(void)
{
	LOGUART_WaitTxComplete();
	/* To avoid the corner case that data between path FIFO and TX FIFO */
	DelayMs(5);
}

HAL_ROM_TEXT_SECTION
void xmodem_uart_setbaud(u32 baud)
{
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, DISABLE);
	LOGUART_SetBaud(LOGUART_DEV, baud);
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, ENABLE);
}


