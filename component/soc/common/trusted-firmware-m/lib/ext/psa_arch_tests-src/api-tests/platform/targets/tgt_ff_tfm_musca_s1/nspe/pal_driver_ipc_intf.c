/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

/**
    @brief    - This function initializes the UART
    @param    - uart base addr
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr)
{
    psa_status_t            status_of_call = PSA_SUCCESS;
    uart_fn_type_t          uart_fn = UART_INIT;

    psa_invec data[3] = {{&uart_fn, sizeof(uart_fn)},
                         {&uart_base_addr, sizeof(uart_base_addr)},
                         {NULL, 0} };

#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_UART_HANDLE, 0, data, 3, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t            print_handle = 0;
    print_handle = psa_connect(DRIVER_UART_SID, DRIVER_UART_VERSION);
    if (PSA_HANDLE_IS_VALID(print_handle))
    {
        status_of_call = psa_call(print_handle, 0, data, 3, NULL, 0);
        psa_close(print_handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/

int pal_print_ns(const char *str, int32_t data)
{
    int             string_len = 0;
    const char      *p = str;
    psa_status_t    status_of_call = PSA_SUCCESS;
    uart_fn_type_t  uart_fn = UART_PRINT;

    while (*p != '\0')
    {
        string_len++;
        p++;
    }

    psa_invec data1[3] = {{&uart_fn, sizeof(uart_fn)},
                          {str, string_len+1},
                          {&data, sizeof(data)} };

#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_UART_HANDLE, 0, data1, 3, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t    print_handle = 0;
    print_handle = psa_connect(DRIVER_UART_SID, DRIVER_UART_VERSION);
    if (PSA_HANDLE_IS_VALID(print_handle))
    {
        status_of_call = psa_call(print_handle, 0, data1, 3, NULL, 0);
        psa_close(print_handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
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
    wd_param_t              wd_param;
    psa_status_t            status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_INIT_SEQ;
    wd_param.wd_base_addr = base_addr;
    wd_param.wd_time_us = time_us;
    wd_param.wd_timer_tick_us = timer_tick_us;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)} };

#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_WATCHDOG_HANDLE, 0, invec, 1, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else

    psa_handle_t            handle = 0;
    handle = psa_connect(DRIVER_WATCHDOG_SID, DRIVER_WATCHDOG_VERSION);
    if (PSA_HANDLE_IS_VALID(handle))
    {
        status_of_call = psa_call(handle, 0, invec, 1, NULL, 0);
        psa_close(handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr)
{
    wd_param_t              wd_param;
    psa_status_t            status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_ENABLE_SEQ;
    wd_param.wd_base_addr = base_addr;
    wd_param.wd_time_us = 0;
    wd_param.wd_timer_tick_us = 0;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)} };

#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_WATCHDOG_HANDLE, 0, invec, 1, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t            handle = 0;
    handle = psa_connect(DRIVER_WATCHDOG_SID, DRIVER_WATCHDOG_VERSION);
    if (PSA_HANDLE_IS_VALID(handle))
    {
        status_of_call = psa_call(handle, 0, invec, 1, NULL, 0);
        psa_close(handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    wd_param_t              wd_param;
    psa_status_t            status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_DISABLE_SEQ;
    wd_param.wd_base_addr = base_addr;
    wd_param.wd_time_us = 0;
    wd_param.wd_timer_tick_us = 0;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)} };

#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_WATCHDOG_HANDLE, 0, invec, 1, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t            handle = 0;
    handle = psa_connect(DRIVER_WATCHDOG_SID, DRIVER_WATCHDOG_VERSION);
    if (PSA_HANDLE_IS_VALID(handle))
    {
        status_of_call = psa_call(handle, 0, invec, 1, NULL, 0);
        psa_close(handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
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
    nvmem_param_t   nvmem_param;
    psa_status_t    status_of_call = PSA_SUCCESS;

    nvmem_param.nvmem_fn_type = NVMEM_READ;
    nvmem_param.base = base;
    nvmem_param.offset = offset;
    nvmem_param.size = size;
    psa_invec invec[1] = {{&nvmem_param, sizeof(nvmem_param)} };
    psa_outvec outvec[1] = {{buffer, size} };
#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_NVMEM_HANDLE, 0, invec, 1, outvec, 1);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t    handle = 0;
    handle = psa_connect(DRIVER_NVMEM_SID, DRIVER_NVMEM_VERSION);
    if (PSA_HANDLE_IS_VALID(handle))
    {
        status_of_call = psa_call(handle, 0, invec, 1, outvec, 1);
        psa_close(handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
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
    nvmem_param_t   nvmem_param;
    psa_status_t    status_of_call = PSA_SUCCESS;

    nvmem_param.nvmem_fn_type = NVMEM_WRITE;
    nvmem_param.base = base;
    nvmem_param.offset = offset;
    nvmem_param.size = size;
    psa_invec invec[2] = {{&nvmem_param, sizeof(nvmem_param)}, {buffer, size} };
#if STATELESS_ROT == 1
    status_of_call = psa_call(DRIVER_NVMEM_HANDLE, 0, invec, 2, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    	return PAL_STATUS_ERROR;

    return PAL_STATUS_SUCCESS;
#else
    psa_handle_t    handle = 0;
    handle = psa_connect(DRIVER_NVMEM_SID, DRIVER_NVMEM_VERSION);
    if (PSA_HANDLE_IS_VALID(handle))
    {
        status_of_call = psa_call(handle, 0, invec, 2, NULL, 0);
        psa_close(handle);
        if (status_of_call != PSA_SUCCESS)
            return PAL_STATUS_ERROR;

        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
#endif
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
