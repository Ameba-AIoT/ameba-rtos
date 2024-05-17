/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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

#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pal_common.h"

/* This stdc implementation doesn't support tests that involve resets of the
 * test process or the system, so we don't actually need non-volatile memory.
 * Just implement the "nvmem" as an array in memory.
 */

/* Using zero as NVMEM_BASE is a bit arbitrary - we don't actually need callers
 * to specify a base address but the nvmem function signatures have "base" params.
 * Zero is the value used in our target.cfg file so that's what we should receive.
 */
#define NVMEM_BASE 0

#define NVMEM_SIZE (1024)
static uint8_t g_nvmem[NVMEM_SIZE];

/**
    @brief    - Check that an nvmem access is within the bounds of the nvmem
    @param    - base    : Base address of nvmem (must be zero)
                offset  : Offset into nvmem
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
static int nvmem_check_bounds(addr_t base, uint32_t offset, int size)
{
    if (base != NVMEM_BASE)
        return PAL_STATUS_ERROR;
    if (offset > NVMEM_SIZE)
        return PAL_STATUS_ERROR;
    if (size < 0)
        return PAL_STATUS_ERROR;
    if (offset > (uint32_t)(INT_MAX - size))
        return PAL_STATUS_ERROR;
    if (offset + size > NVMEM_SIZE)
        return PAL_STATUS_ERROR;
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - Reads from given non-volatile address.
    @param    - base    : Base address of nvmem (must be zero)
                offset  : Offset
                buffer  : Pointer to dest address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (nvmem_check_bounds(base, offset, size) != PAL_STATUS_SUCCESS)
    {
        return PAL_STATUS_ERROR;
    }
    memcpy(buffer, g_nvmem + offset, size);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - Writes into given non-volatile address.
    @param    - base    : Base address of nvmem (must be zero)
                offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (nvmem_check_bounds(base, offset, size) != PAL_STATUS_SUCCESS)
    {
        return PAL_STATUS_ERROR;
    }
    memcpy(g_nvmem + offset, buffer, size);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function initializes the UART

    This implementation doesn't actually use a UART to print test output, we
    just send it to stdout. No init necessary.

    @param    - uart base addr (ignored)
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr)
{
    (void)uart_base_addr;
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO

    This implementation doesn't actually use a UART to print test output, we
    just send it to stdout.

    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/
int pal_print_ns(const char *str, int32_t data)
{
    if (printf(str, data) < 0)
    {
        return PAL_STATUS_ERROR;
    }
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Initializes an hardware watchdog timer

    This implementation doesn't support watchdogs so this is a noop.

    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    (void)base_addr;
    (void)time_us;
    (void)timer_tick_us;
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Enables a hardware watchdog timer

    This implementation doesn't support watchdogs so this is a noop.

    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr)
{
    (void)base_addr;
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Disables a hardware watchdog timer

    This implementation doesn't support watchdogs so this is a noop.

    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    (void)base_addr;
    return PAL_STATUS_SUCCESS;
}

/**
     @brief    - Terminates the simulation at the end of all tests completion.

     This implementation just calls exit.

     @param    - void
     @return   - void
**/
void pal_terminate_simulation(void)
{
    ;
}

/**
 *   @brief    - Resets the system.
 *   @param    - void
 *   @return   - SUCCESS/FAILURE
**/
int pal_system_reset(void)
{
    return PAL_STATUS_UNSUPPORTED_FUNC;
}
