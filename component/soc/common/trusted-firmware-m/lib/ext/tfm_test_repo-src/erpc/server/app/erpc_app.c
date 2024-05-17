/*
 * Copyright (c) 2017-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_log.h"
#include "erpc_server_start.h"
#include "config_erpc_target.h"

#include "Driver_USART.h"
#ifdef ERPC_UART
extern ARM_DRIVER_USART ERPC_UART;
#else
#error "ERPC_UART is not provided!"
#endif

__attribute__((noreturn))
void test_app(void *argument)
{
    (void)(argument);

    erpc_transport_t transport;

    transport = erpc_transport_cmsis_uart_init((void *)&ERPC_UART);
    if (!transport) {
        LOG_MSG("eRPC transport init failed!\r\n");
    }

    erpc_server_start(transport);

    for (;;) {
    }
}
