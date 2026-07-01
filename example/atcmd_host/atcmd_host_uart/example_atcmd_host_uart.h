/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_ATCMD_HOST_UART_H
#define EXAMPLE_ATCMD_HOST_UART_H

/*
 * Two UART peripherals are required on the host board:
 *   HOST_UART : connects to the user's terminal (PC serial monitor)
 *   AT_UART   : connects to the AT command slave device
 *
 * Wire AT_UART_TX --> slave UART_RX
 *      AT_UART_RX <-- slave UART_TX
 *
 * The slave-side pin defaults (from at_intf_uart.c) are:
 *   AmebaSmart  : TX=PA_3,  RX=PA_2
 *   AmebaLite   : TX=PA_22, RX=PA_21
 *   AmebaDplus  : TX=PA_13, RX=PA_14
 *   AmebaGreen2 : TX=PA_26, RX=PA_25
 */

#if defined(CONFIG_AMEBASMART)

#define HOST_UART_IDX       0
#define HOST_UART_TX        PA_25
#define HOST_UART_RX        PA_26

#define AT_UART_IDX         1
#define AT_UART_TX          PA_3   /* --> slave UART_RX (PA_2 on slave) */
#define AT_UART_RX          PA_2   /* <-- slave UART_TX (PA_3 on slave) */

#elif defined(CONFIG_AMEBALITE)

#define HOST_UART_IDX       0
#define HOST_UART_TX        PA_28
#define HOST_UART_RX        PA_29

#define AT_UART_IDX         1
#define AT_UART_TX          PA_22  /* --> slave UART_RX (PA_21 on slave) */
#define AT_UART_RX          PA_21  /* <-- slave UART_TX (PA_22 on slave) */

#elif defined(CONFIG_AMEBADPLUS)

#define HOST_UART_IDX       0
#define HOST_UART_TX        PA_26
#define HOST_UART_RX        PA_27

#define AT_UART_IDX         1
#define AT_UART_TX          PB_18  /* --> slave UART_RX (PB_18 on slave) */
#define AT_UART_RX          PB_19  /* <-- slave UART_TX (PB_19 on slave) */

#else

#define HOST_UART_IDX       0
#define HOST_UART_TX        PA_26
#define HOST_UART_RX        PA_27

#define AT_UART_IDX         1
#define AT_UART_TX          PA_30
#define AT_UART_RX          PA_31

#endif

/* HOST_UART baud: user terminal.  AT_UART baud: must match slave UART_BAUD. */
#define HOST_UART_BAUDRATE  115200
#define AT_UART_BAUDRATE    38400

void example_atcmd_host_uart(void);

#endif /* EXAMPLE_ATCMD_HOST_UART_H */
