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


#include "val_driver_service_apis.h"
#include "val_target.c"

print_verbosity_t  g_print_level = PRINT_INFO;
static int is_uart_init_done = 0;

/* UART APIs */
/*
    @brief    - Initialise the UART
    @param    - uart_base_addr: address of the uart base
    @param    - print_level: g_print_level verbosity
    @return   - val_status_t
*/
val_status_t val_uart_init_sf(addr_t uart_base_addr)
{
    pal_uart_init(uart_base_addr);
    is_uart_init_done = 1;
    return VAL_STATUS_SUCCESS;
}
/*
    @brief     - This function parses the input string and writes byte by byte to
                 print the input string
    @param     - pointer         : Input String
               - data            : Value for Format specifier
    @return    - error status
 */
val_status_t val_print_sf(const char *string, int32_t data)
{
    if (is_uart_init_done == 1)
    {
        /* Do not print until uart_init is complete */
        pal_print(string, data);
    }
    return VAL_STATUS_SUCCESS;
}

/* Watchdog APIs */
/**
    @brief    - Initializes the WatchDog Timer instance
    @param    - base address of the given timer instance
              - time_us         : Time in micro seconds
              - timer_tick_us   : Number of ticks per micro second
    @return   - error status
**/
val_status_t val_wd_timer_init_sf(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return pal_wd_timer_init(base_addr, time_us, timer_tick_us);
}

/**
    @brief    - Enable WatchDog Timer instance
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_wd_timer_enable_sf(addr_t base_addr)
{
    return pal_wd_timer_enable(base_addr);
}

/**
    @brief    - Disable Watch Dog Timer instance
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_wd_timer_disable_sf(addr_t base_addr)
{
    return pal_wd_timer_disable(base_addr);
}

/**
    @brief    - Checks if watchdog enabled
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_is_wd_timer_enabled_sf(addr_t base_addr)
{
    if (pal_wd_timer_is_enabled(base_addr))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}

/*
    @brief     - Reads 'size' bytes from Non-volatile memory at a given 'base + offset'
                 into given buffer.
               - offset    : Offset from NV MEM base address
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_read_sf(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (pal_nvmem_read(base, offset, buffer, size))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given 'base + offset'
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_write_sf(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (pal_nvmem_write(base, offset, buffer, size))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}

/**
    @brief    - This function sets the given boot.state value to corresponding
                boot NVMEM location
    @param    - state: boot_state_t
    @return   - val_status_t
**/
val_status_t val_driver_private_set_boot_flag_fn(boot_state_t state)
{
   boot_t           boot;
   val_status_t     status;
   memory_desc_t   *memory_desc;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVMEM, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));

   if (VAL_ERROR(status))
   {
        return status;
   }

   boot.state = state;
   status = val_nvmem_write_sf(memory_desc->start,
                               VAL_NVMEM_OFFSET(NV_BOOT),
                               &boot,
                               sizeof(boot_t));
   if (VAL_ERROR(status))
   {
       val_print_sf("val_nvmem_write_sf failed Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function initialises the driver reserved mmio region
    @param    - void
    @return   - val_status_t
**/
val_status_t val_init_driver_memory(void)
{
   val_status_t     status;
   memory_desc_t   *memory_desc;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                  MEMORY_DRIVER_PARTITION_MMIO, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));
   if (VAL_ERROR(status))
   {
        return status;
   }

   /* Init driver mmio space to 0 to avoid uninit access */
   memset((uint32_t *)memory_desc->start, 0, (memory_desc->end - memory_desc->start + 1));

   return VAL_STATUS_SUCCESS;
}

/**
    @brief    - This function returns the driver reserved mmio region base
    @param    - base pointer
    @return   - val_status_t
**/
val_status_t val_get_driver_mmio_addr(addr_t *base_addr)
{
   val_status_t     status;
   memory_desc_t   *memory_desc;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                  MEMORY_DRIVER_PARTITION_MMIO, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));
   if (VAL_ERROR(status))
   {
        return status;
   }

   *base_addr = memory_desc->start;
   return VAL_STATUS_SUCCESS;
}

/**
    @brief   - Trigger interrupt for irq signal assigned to driver partition
               before return to caller.
    @param   - void
    @return  - void
**/
void val_generate_interrupt(void)
{
    pal_generate_interrupt();
}

/**
    @brief   - Disable interrupt that was generated using val_generate_interrupt API.
    @param   - void
    @return  - void
**/
void val_disable_interrupt(void)
{
    pal_disable_interrupt();
}
