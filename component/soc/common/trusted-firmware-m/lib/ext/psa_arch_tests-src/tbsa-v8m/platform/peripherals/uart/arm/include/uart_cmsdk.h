/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _UART_CMSDK_H_
#define _UART_CMSDK_H_

#include "pal_common.h"

/* CMSDK_UART DATA Register Definitions */
#define CMSDK_UART_STATE_TXBF_Pos         0                                                  /* CMSDK_UART STATE: TXBF Position */
#define CMSDK_UART_STATE_TXBF_Msk        (0x1UL /*<< CMSDK_UART_STATE_TXBF_Pos*/)            /* CMSDK_UART STATE: TXBF Mask */

/* CMSDK_UART CTRL Register Definitions */
#define CMSDK_UART_CTRL_TXEN_Pos          0                                                  /* CMSDK_UART CTRL: TXEN Position */
#define CMSDK_UART_CTRL_TXEN_Msk         (0x01UL /*<< CMSDK_UART_CTRL_TXEN_Pos*/)            /* CMSDK_UART CTRL: TXEN Mask */

/* typedef's */
typedef struct
{
    uint32_t  DATA;                     /* Offset: 0x000 (R/W) Data Register */
    uint32_t  STATE;                    /* Offset: 0x004 (R/W) Status state */
    uint32_t  CTRL;                     /* Offset: 0x008 (R/W) Control Register */
    union {
        uint32_t INTSTATUS;             /* Offset: 0x00C (R/ ) Interrupt Status Register */
        uint32_t INTCLEAR;              /* Offset: 0x00C ( /W) Interrupt Clear Register */
    };
    uint32_t  BAUDDIV;                  /* Offset: 0x010 (R/W) Baud rate divider */
} uart_t;

/* function prototypes */
int32_t uart_init(uart_t *uart);
int32_t uart_putc(uart_t *uart, const void *data, uint32_t num);

#endif /* _UART_CMSDK_H_ */
