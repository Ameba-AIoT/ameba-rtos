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

#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX		_PA_18 // UART0 TX
#define UART_RX		_PA_19 // UART0 RX
#define UART_RTS	_PA_25 // UART0 RTS
#define UART_CTS	_PA_26 // UART0 CTS

#elif defined (CONFIG_RTL8720F)
/* fully programmable zone.
 * When wired pin-to-pin (PA21-PA21, PA22-PA22, ...) instead of cross-wired,
 * build the peer board firmware with -DUART_AUTOCTRL_PEER to swap TX/RX and
 * RTS/CTS so the signal directions come out correct over the straight cable. */
#ifndef PEER_BOARD
#define UART_TX		_PA_21 // UART0 TX
#define UART_RX		_PA_22 // UART0 RX
#define UART_RTS	_PA_23 // UART0 RTS
#define UART_CTS	_PA_24 // UART0 CTS
#else
#define UART_TX		_PA_22 // peer TX - dut RX
#define UART_RX		_PA_21 // peer RX - dut TX
#define UART_RTS	_PA_24 // peer RTS - dut CTS
#define UART_CTS	_PA_23 // peer CTS - dut RTS
#endif

#endif

extern int example_raw_uart_polling_auto_flow_ctrl(void);

#endif /* UART_EXT_H*/
