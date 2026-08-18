/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef UART_EXT_H
#define UART_EXT_H

#if defined (CONFIG_AMEBAGREEN2)
#define UART_TX		_PA_19 // UART0 TX
#define UART_RX		_PA_18 // UART0 RX
#define UART_RTS	_PA_26 // UART0 RTS
#define UART_CTS	_PA_25 // UART0 CTS

#elif defined (CONFIG_RTL8720F)
#define UART_TX		_PA_22 // UART0 TX
#define UART_RX		_PA_21 // UART0 RX
#define UART_RTS	_PA_24 // UART0 RTS
#define UART_CTS	_PA_23 // UART0 CTS

#endif

extern int example_uart_tool(void);

#endif /* UART_EXT_H*/
