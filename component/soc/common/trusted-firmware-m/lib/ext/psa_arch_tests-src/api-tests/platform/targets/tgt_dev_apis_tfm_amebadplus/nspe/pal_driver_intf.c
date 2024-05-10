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

#include "pal_common.h"
#include "pal_uart.h"
#include "pal_wd_rtk.h"

int32_t tfm_platform_system_reset(void);

/**
    @brief    - This function initializes the UART
    @param    - uart base addr
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(uint32_t uart_base_addr)
{
    pal_uart_rtk_init(uart_base_addr);
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
    pal_rtk_print(str, data);
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
    return(pal_wd_rtk_init(base_addr,time_us, timer_tick_us));
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_enable_ns(addr_t base_addr)
{
    return(pal_wd_rtk_enable(base_addr));
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr  : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_disable_ns(addr_t base_addr)
{
    return (pal_wd_rtk_disable(base_addr));
}

/**
  * @brief  Read a stream of data from specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to read from.
  * @param  len: Length of the data to read.
  * @param  data: Address to save the readback data.
  * @return Read result.
  * @retval 1: Success.
  * @retval Others: Error.
  * @note It is ok to read under auto mode due to flash cache.
  */
int  flash_stream_read(void *obj, uint32_t address, uint32_t len, uint8_t *data);

/**
  * @brief  Write a stream of data to specified address.
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to write to.
  * @param  len: Length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
  * @return Write result.
  * @retval 1: Success.
  * @retval Others: Error.
  */
int  flash_stream_write(void *obj, uint32_t address, uint32_t len, uint8_t *data);
/**
  * @brief  Erase flash sector(4KB).
  * @param  obj: Flash object defined in application software.
  * @param  address: Starting address to be erased.
  * @retval none
  * @note LSB 12bits will be masked.
  */
void flash_erase_sector(void *obj, uint32_t address);

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
	flash_stream_read(NULL, base + offset, size, buffer);

    return PAL_STATUS_SUCCESS;
}

void *pvPortMalloc(size_t xWantedSize);
void vPortFree(void *pv);
void *_memcpy(void *dst0, const void *src0, size_t n);
#define FLASH_AREA_NVMEM_OP_SIZE    (0x400)     /* 1 KB */

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
	if(((base + offset) & (FLASH_AREA_NVMEM_OP_SIZE - 1)) + size > FLASH_AREA_NVMEM_OP_SIZE) {
		pal_print_ns("error, pal_nvmem_write_ns is overflow.\n", 0);
		while(1);
	}
	
	/* Allocate the memory that will store previous data. */
	void *preDataBuf = pvPortMalloc(FLASH_AREA_NVMEM_OP_SIZE);
	flash_stream_read(NULL, base, FLASH_AREA_NVMEM_OP_SIZE, preDataBuf);
	_memcpy((void *)((uint32_t)preDataBuf + offset), buffer, size);

	/*discard older data after sector erase*/
	flash_erase_sector(NULL, base);
	flash_stream_write(NULL, base, FLASH_AREA_NVMEM_OP_SIZE, preDataBuf);
	
	vPortFree(preDataBuf);
    return PAL_STATUS_SUCCESS;
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

    while(1)
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
