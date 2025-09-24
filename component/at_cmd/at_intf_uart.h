/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AT_INTF_UART_H
#define AT_INTF_UART_H

#include "ameba_soc.h"

/* note: Selected UART shall not conflict with UART_BT, */
/* e.g. UART3 in AmebaSmart/AmebaLite or UART2 in AmebaDplus */
#define UART_DEV	UART0_DEV
#define DMA_TX_BURST_SIZE	8

#define POLL_LEN_MAX		16

extern u32 UART_BAUD;
extern u8 UART_TX;
extern u8 UART_RX;
extern signed char UART_RTS;
extern signed char UART_CTS;

_LONG_CALL_ void atio_uart_output(char *buf, int len);
_LONG_CALL_ int atio_uart_init(void);

#endif /* AT_INTF_UART_H */