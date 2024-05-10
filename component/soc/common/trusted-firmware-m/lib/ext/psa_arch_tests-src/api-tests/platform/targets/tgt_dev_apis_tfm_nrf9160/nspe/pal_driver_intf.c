/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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

#include "pal_common.h"
#include "pal_nvmem.h"

#include "nrf_wdt.h"

/**
    @brief    - This function initializes the UART
    @param    - uart base addr
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr)
{
    (void)uart_base_addr;
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/

int pal_print_ns(const char *str, int32_t data)
{
    tfm_log_printf(str, data);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Initializes a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    (void)timer_tick_us;
    return nrf_wdt_init(base_addr, time_us);
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr)
{
    return nrf_wdt_enable(base_addr);
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    return nrf_wdt_disable(base_addr);
}

/**
    @brief    - Reads from given non-volatile address.
    @param    - base    : Base address of nvmem
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (base != 0) {
        /* Unexpected base address */
        return PAL_STATUS_ERROR;
    }
    base = pal_nvmem_get_addr();
    return nvmem_read(base, offset, buffer, size) ? PAL_STATUS_SUCCESS
                                                  : PAL_STATUS_ERROR;
}

/**
    @brief    - Writes into given non-volatile address.
    @param    - base    : Base address of nvmem
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (base != 0) {
        /* Unexpected base address */
        return PAL_STATUS_ERROR;
    }
    base = pal_nvmem_get_addr();
    return nvmem_write(base, offset, buffer, size) ? PAL_STATUS_SUCCESS
                                                   : PAL_STATUS_ERROR;
}

/**
 *   @brief    - Terminates the simulation at the end of all tests completion.
 *               By default, it put cpus into power down mode.
 *   @param    - void
 *   @return   - void
**/
void pal_terminate_simulation(void)
{
    /* Add logic to terminate the simluation */

    while (1)
    {
        __asm volatile("WFI");
    }
}

/**
 *   @brief    - Resets the system.
 *   @param    - void
 *   @return   - SUCCESS/FAILURE
**/
int pal_system_reset(void)
{
    return tfm_platform_system_reset();
}
