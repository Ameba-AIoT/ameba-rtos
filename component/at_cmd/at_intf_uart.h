/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* note: Selected UART shall not conflict with UART_BT, */
/* e.g. UART3 in AmebaSmart/AmebaLite or UART2 in AmebaDplus */
#define UART_DEV	UART0_DEV
#define DMA_TX_BURST_SIZE	8

#define POLL_LEN_MAX		16

_LONG_CALL_ void atio_uart_output(char *buf, int len);
_LONG_CALL_ void atio_uart_init(void);

