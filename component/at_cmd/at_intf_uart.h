/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define UART_DEV	UART2_DEV
#define UART_BAUD	38400
#define DMA_TX_BURST_SIZE	8

#define POLL_LEN_MAX		16

#if defined (CONFIG_AMEBASMART)
#define UART_TX		_PB_22 // UART2 TX
#define UART_RX		_PB_21 // UART2 RX
#elif defined (CONFIG_AMEBALITE)
/* fully programmable zone */
#define UART_TX		_PA_28 // UART TX
#define UART_RX		_PA_29 // UART RX
#elif defined (CONFIG_AMEBADPLUS)
/* fully programmable zone */
#define UART_TX		_PA_26 // UART TX
#define UART_RX		_PA_27 // UART RX
#endif

_LONG_CALL_ void atio_uart_output(char *buf, int len);
_LONG_CALL_ void atio_uart_init(void);

