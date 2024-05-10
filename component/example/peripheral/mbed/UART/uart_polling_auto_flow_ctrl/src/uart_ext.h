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

#if defined (CONFIG_AMEBASMART)
#define UART_TX		_PA_3 // UART0 TX
#define UART_RX		_PA_2 // UART0 RX
#define UART_RTS	_PA_5 // UART0 RTS
#define UART_CTS	_PA_4 // UART0 CTS

#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX		_PA_28 // UART0 TX
#define UART_RX		_PA_29 // UART0 RX
#define UART_RTS	_PA_30 // UART0 RTS
#define UART_CTS	_PA_31 // UART0 CTS

#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX		_PB_17 // UART0 TX
#define UART_RX		_PB_18 // UART0 RX
#define UART_RTS	_PB_19 // UART0 RTS
#define UART_CTS	_PB_20 // UART0 CTS

#endif

#endif /* UART_EXT_H*/
