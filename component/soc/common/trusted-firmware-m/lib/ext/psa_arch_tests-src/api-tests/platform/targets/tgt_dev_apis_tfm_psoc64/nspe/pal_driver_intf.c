/** @file
 * Copyright (c) 2020, Arm Limited or its affiliates. All rights reserved.
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

extern int tfm_log_printf(const char *, ...);
int32_t tfm_platform_system_reset(void);

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

int pal_print_ns(char *str, int32_t data)
{
    tfm_log_printf(str, data);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Initializes an hardware watchdog timer
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
    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    (void)base_addr;
    return PAL_STATUS_SUCCESS;
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
    memcpy(buffer, (void *)(base+offset), size);
    return PAL_STATUS_SUCCESS;
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
    memcpy((void *)(base+offset), buffer, size);
    return PAL_STATUS_SUCCESS;
}

/**
 *   @brief    - This function will read peripherals using SPI commands
 *   @param    - addr : address of the peripheral
 *               data : read buffer
 *               len  : length of the read buffer in bytes
 *   @return   - error status
**/
int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len)
{
    (void)addr;
    (void)data;
    (void)len;

    return 0xFF;
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
