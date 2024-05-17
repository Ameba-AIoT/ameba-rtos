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

#include "val_peripherals.h"
#include "val_target.h"
#include "pal_interfaces_ns.h"
#include "val_framework.h"
#include "val_client_defs.h"

/* Global */
uint32_t   is_uart_init_done = 0;

/*
    @brief    - Initialize UART.
                This is client interface API of secure partition UART INIT API.
    @param    - None
    @return   - val_status_t
*/
val_status_t val_uart_init(void)
{
    soc_peripheral_desc_t   *uart_desc;
    val_status_t            status = VAL_STATUS_SUCCESS;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                    SOC_PERIPHERAL_UART, 0),
                                   (uint8_t **)&uart_desc,
                                   (uint32_t *)sizeof(soc_peripheral_desc_t));
    if (VAL_ERROR(status))
    {
         return status;
    }

    is_uart_init_done = 1;
    return pal_uart_init_ns(uart_desc->base);
}

/**
    @brief    - Print module. This is client interface API of secure partition
                val_print_sf API for nspe world
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - val_status_t
**/
val_status_t val_print(print_verbosity_t verbosity, const char *string, int32_t data)
{
    if ((is_uart_init_done == 0) || (verbosity < VERBOSE))
    {
       return VAL_STATUS_SUCCESS;
    }
    return pal_print_ns(string, data);
}

/* Watchdog APIs */
/**
    @brief    - Initializes the WatchDog Timer instance. This is client interface API of
                secure partition val_wd_timer_init_sf API for nspe world.
    @param    timeout: watchdog timeout value to be programmed
              Defines to be used are WD_LOW_TIMEOUT, WD_MEDIUM_TIMEOUT and WD_HIGH_TIMEOUT
    @return   - error status
**/
val_status_t val_wd_timer_init(wd_timeout_type_t timeout_type)
{
   soc_peripheral_desc_t   *soc_per_desc;
   uint32_t                time_us;
   val_status_t            status = VAL_STATUS_SUCCESS;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                   SOC_PERIPHERAL_WATCHDOG, 0),
                                   (uint8_t **)&soc_per_desc,
                                   (uint32_t *)sizeof(soc_peripheral_desc_t));
   if (VAL_ERROR(status))
   {
        return status;
   }

   if (timeout_type == WD_CRYPTO_TIMEOUT)
   {
       time_us =  soc_per_desc->timeout_in_micro_sec_crypto;
   }
   else if (timeout_type == WD_LOW_TIMEOUT)
   {
       time_us =  soc_per_desc->timeout_in_micro_sec_low;
   }
   else if (timeout_type == WD_MEDIUM_TIMEOUT)
   {
       time_us = soc_per_desc->timeout_in_micro_sec_medium;
   }
   else
   {
       time_us = soc_per_desc->timeout_in_micro_sec_high;
   }

   return pal_wd_timer_init_ns(soc_per_desc->base,
                               time_us,
                               soc_per_desc->num_of_tick_per_micro_sec);
}

/**
    @brief    - Enable WatchDog Timer instance. This is client interface API of
                secure partition val_wd_timer_enable_sf API for nspe world.
    @return   - error status
**/
val_status_t val_wd_timer_enable(void)
{
   soc_peripheral_desc_t   *soc_per_desc;
   val_status_t            status = VAL_STATUS_SUCCESS;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                   SOC_PERIPHERAL_WATCHDOG, 0),
                                   (uint8_t **)&soc_per_desc,
                                   (uint32_t *)sizeof(soc_peripheral_desc_t));
   if (VAL_ERROR(status))
   {
        return status;
   }

   return pal_wd_timer_enable_ns(soc_per_desc->base);
}

/**
    @brief    - Disable Watch Dog Timer instance. This is client interface API of
                secure partition val_wd_timer_disable_sf API for nspe world.
    @return   - error status
**/
val_status_t val_wd_timer_disable(void)
{
   soc_peripheral_desc_t   *soc_per_desc;
   val_status_t            status = VAL_STATUS_SUCCESS;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                   SOC_PERIPHERAL_WATCHDOG, 0),
                                   (uint8_t **)&soc_per_desc,
                                   (uint32_t *)sizeof(soc_peripheral_desc_t));
   if (VAL_ERROR(status))
   {
        return status;
   }

   return pal_wd_timer_disable_ns(soc_per_desc->base);
}

val_status_t val_wd_reprogram_timer(wd_timeout_type_t timeout_type)
{
    val_status_t    status = VAL_STATUS_SUCCESS;
#ifdef WATCHDOG_AVAILABLE
    /* Disable watchdog Timer */
    val_wd_timer_disable();

    /* Initialise watchdog */
    status = val_wd_timer_init(timeout_type);
    if (VAL_ERROR(status))
    {
        return status;
    }

    /* Enable watchdog Timer */
    status = val_wd_timer_enable();
    if (VAL_ERROR(status))
    {
        return status;
    }
#else
	(void)timeout_type; // Argument unused if WATCHDOG_AVAILABLE is not defined
#endif

    return status;
}


/*
    @brief     - Reads 'size' bytes from Non-volatile memory at a given. This is client interface
                API of secure partition val_nvmem_read_sf API for nspe world.
                'base + offset' into given buffer.
               - offset    : Offset from NV MEM base address
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_read(uint32_t offset, void *buffer, int size)
{
   memory_desc_t   *memory_desc;
   val_status_t    status = VAL_STATUS_SUCCESS;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVMEM, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));

   if (VAL_ERROR(status))
   {
        return status;
   }

   return pal_nvmem_read_ns(memory_desc->start, offset, buffer, size);
}

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given
                 'base + offset'.  This is client interface API of secure partition
                 val_nvmem_write_sf API for nspe world.
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_write(uint32_t offset, void *buffer, int size)
{
   memory_desc_t   *memory_desc;
   val_status_t    status = VAL_STATUS_SUCCESS;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVMEM, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));

   if (VAL_ERROR(status))
   {
        return status;
   }

   return pal_nvmem_write_ns(memory_desc->start, offset, buffer, size);
}
