/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_PAL_INTERFACE_APIS_H_
#define _VAL_PAL_INTERFACE_APIS_H_


#include "val.h"

/* Peripherals APIs */

/*
    @brief     - This function initializes the uart
    @param     - uart_base_addr : Base address of the UART
    @return    -
*/
void pal_uart_init(addr_t uart_base_addr);

/*
    @brief     - This function parses the input string and writes byte by byte to print
                 the input string
    @param     - str            : Input String
               - data           : Value for Format specifier
    @return    - void
 */
void pal_print(const char *str, int32_t data);

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr     : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_enable(addr_t base_addr);

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr     : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_disable(addr_t base_addr);


/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr     : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int  pal_wd_timer_is_enabled(addr_t base_addr);

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given 'base + offset'
    @param     - base      : Base address of NV MEM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - error status 0:SUCCESS, 1:FAIL
*/
int  pal_nvmem_write(addr_t base, uint32_t offset, void *buffer, int size);

/*
    @brief     - Reads 'size' bytes from non-volatile memory at a given
                'base + offset' into given buffer
    @param     - base      : Base address of NV MEM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - error status 0:SUCCESS, 1:FAIL
*/
int  pal_nvmem_read(addr_t base, uint32_t offset, void *buffer, int size);

/**
    @brief   - Trigger interrupt for irq signal assigned to driver partition
               before return to caller.
    @param   - void
    @return  - void
**/
void pal_generate_interrupt(void);

/**
    @brief   - Disable interrupt that was generated using pal_generate_interrupt API.
    @param   - void
    @return  - void
**/
void pal_disable_interrupt(void);
#endif
