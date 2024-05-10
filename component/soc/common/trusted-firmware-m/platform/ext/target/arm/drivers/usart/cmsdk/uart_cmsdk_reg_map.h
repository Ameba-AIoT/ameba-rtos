/*
 * Copyright (c) 2021 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * \file uart_cmsdk_reg_map.h
 * \brief Register map for ARM CMSDK UART
 */

#ifndef __UART_CMSDK_REG_MAP_H__
#define __UART_CMSDK_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UART register map structure */
struct uart_cmsdk_reg_map_t {
    volatile uint32_t data;   /* Offset: 0x000 (R/W) data register    */
    volatile uint32_t state;  /* Offset: 0x004 (R/W) status register  */
    volatile uint32_t ctrl;   /* Offset: 0x008 (R/W) control register */
    union {
        volatile uint32_t intrstatus;  /* Offset: 0x00c (R/ ) interrupt status
                                        *                     register
                                        */
        volatile uint32_t intrclear;   /* Offset: 0x00c ( /W) interrupt clear
                                        *                     register
                                        */
    }intr_reg;
    volatile uint32_t bauddiv;        /* Offset: 0x010 (R/W) Baudrate divider
                                       *                     register
                                       */
};

#ifdef __cplusplus
}
#endif

#endif /* __UART_CMSDK_REG_MAP_H__ */
