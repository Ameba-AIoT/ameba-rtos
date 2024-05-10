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
#ifndef _PAL_INTERFACES_NS_H_
#define _PAL_INTERFACES_NS_H_

#include "val.h"
#include "val_client_defs.h"
#include <stdarg.h>

/**
    @brief    - This function initializes the UART
    @param    - uart base addr
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr);

/**
 *   @brief    - This function parses the input string and writes bytes into UART TX FIFO
 *   @param    - str      : Input String
 *             - data     : Value for format specifier
 *   @return   - SUCCESS/FAILURE
**/

int pal_print_ns(const char *str, int32_t data);

/**
 *   @brief           - Initializes an hardware watchdog timer
 *   @param           - base_addr       : Base address of the watchdog module
 *                    - time_us         : Time in micro seconds
 *                    - timer_tick_us   : Number of ticks per micro second
 *   @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);

/**
 *   @brief           - Enables a hardware watchdog timer
 *   @param           - base_addr       : Base address of the watchdog module
 *   @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr);

/**
 *   @brief           - Disables a hardware watchdog timer
 *   @param           - base_addr  : Base address of the watchdog module
 *   @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr);

/**
 *   @brief    - Reads from given non-volatile address.
 *   @param    - base    : Base address of nvmem
 *               offset  : Offset
 *               buffer  : Pointer to source address
 *               size    : Number of bytes
 *   @return   - SUCCESS/FAILURE
**/
int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size);

/**
 *   @brief    - Writes into given non-volatile address.
 *   @param    - base    : Base address of nvmem
 *               offset  : Offset
 *               buffer  : Pointer to source address
 *               size    : Number of bytes
 *   @return   - SUCCESS/FAILURE
**/
int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size);

/**
 *   @brief    - This API will call the requested crypto function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
int32_t pal_crypto_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested internal trusted storage function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
uint32_t pal_its_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested protected storage function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
uint32_t pal_ps_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested attestation function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
int32_t pal_attestation_function(int type, va_list valist);

/**
 *   @brief    - Terminates the simulation at the end of all tests completion.
 *               By default, it put cpus into power down mode.
 *   @param    - void
 *   @return   - void
**/
void pal_terminate_simulation(void);

/**
 *   @brief    - Resets the system.
 *   @param    - void
 *   @return   - SUCCESS/FAILURE
**/
int pal_system_reset(void);
#endif
