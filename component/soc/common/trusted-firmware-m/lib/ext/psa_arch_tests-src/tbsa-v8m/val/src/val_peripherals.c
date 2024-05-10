/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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
#include "pal_interfaces.h"

extern uint32_t g_test_binary_in_ram;

/*
    @brief    - Initialize UART
    @param    - addr : Address of peripheral
    @return   - tbsa_status_t
*/
tbsa_status_t val_uart_init(addr_t addr)
{
    return pal_uart_init(addr);
}

/*
    @brief     - Send data to UART TX FIFO
    @param     - addr : Address of the peripheral
                 data : Data to be written to TX FIFO
                 num  : Number of bytes
    @return    - tbsa_status_t
*/
tbsa_status_t val_uart_tx(addr_t addr, const void *data, uint32_t num)
{
    return pal_uart_tx(addr, data, num);
}

/**
    @brief    - This API will read from slave address via I2C
    @param    - addr : Slave address
                data : value read from Slave address
                len  : length of data to be read in bytes
    @return   - error status
**/
tbsa_status_t val_i2c_read(addr_t addr, uint8_t *data, uint32_t len)
{
  return pal_i2c_read(addr, data, len);
}

/**
    @brief    - This API will write to slave address via I2C
    @param    - addr : Slave address
                data : value to write to Slave address
                len  : length of data to be written in bytes
    @return   - error status
**/
tbsa_status_t val_i2c_write(addr_t addr, uint8_t *data, uint32_t len)
{
  return pal_i2c_write(addr, data, len);
}

/**
    @brief    - Initialize SPI
    @param    - void
    @return   - tbsa_status_t
**/
tbsa_status_t val_spi_init (void)
{
    tbsa_status_t         status;
    soc_peripheral_desc_t *spi_desc;
    uint32_t              instance = 0;

    do {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_SPI, instance),
                                       (uint8_t **)&spi_desc,
                                       (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (status != TBSA_STATUS_SUCCESS) {
             return status;
        }

        pal_spi_init(spi_desc->base);

        instance++;
    } while(instance);

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This API will read from slave address via SPI
    @param    - addr : Slave address
                data : value read from Slave address
                len  : length of data to be read in bytes
    @return   - error status
**/
tbsa_status_t val_spi_read(addr_t addr, uint8_t *data, uint32_t len)
{
    if (g_test_binary_in_ram) {
        memcpy((void*)data, (void *)addr, len);
        return TBSA_STATUS_SUCCESS;
    } else {
        return pal_spi_read(addr, data, len);
    }
}

/**
    @brief    - This API will write to slave address via SPI
    @param    - addr : Slave address
                data : value to write to Slave address
                len  : length of data to be written in bytes
    @return   - error status
**/
tbsa_status_t val_spi_write(addr_t addr, uint8_t *data, uint32_t len)
{
  return pal_spi_write(addr, data, len);
}

/**
    @brief    - Initializes the Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_timer_init (addr_t addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return pal_timer_init (addr, time_us, timer_tick_us);
}

/**
    @brief    - Enable Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_timer_enable (addr_t addr)
{
    return pal_timer_enable(addr);
}

/**
    @brief    - Disable Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_timer_disable (addr_t addr)
{
    return pal_timer_disable(addr);
}

/**
    @brief    - Clear the Timer interrupt
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_timer_interrupt_clear (addr_t addr)
{
    return pal_timer_interrupt_clear(addr);
}

/**
    @brief    - Initializes the WatchDog Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_wd_timer_init (addr_t addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return pal_wd_timer_init (addr, time_us, timer_tick_us);
}

/**
    @brief    - Enable WatchDog Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_wd_timer_enable (addr_t addr)
{
    return pal_wd_timer_enable(addr);
}

/**
    @brief    - Disable Watch Dog Timer instance
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_wd_timer_disable (addr_t addr)
{
    return pal_wd_timer_disable(addr);
}

/**
    @brief    - Checks if watchdog enabled
    @param    - Base address of the given timer instance
    @return   - Error status
**/
tbsa_status_t val_is_wd_timer_enabled (addr_t addr)
{
    if (pal_is_wd_timer_enabled(addr)) {
        return TBSA_STATUS_SUCCESS;
    } else {
        return TBSA_STATUS_ERROR;
    }
}

/**
    @brief    - Allow a memory region to be configured as Non-Secure/Secure via MPC
    @param    - mpc - MPC address
                start_addr : Start of memory address to be configured by MPC
                end_addr   : End of memory address to be configured by MPC
                sec_attr   : Map the address to either Secure or Non-Secure security attribute
    @return   - error status
**/
tbsa_status_t val_mpc_configure_security_attribute (addr_t mpc, addr_t start_addr, addr_t end_addr, mem_tgt_attr_t sec_attr)
{
    return pal_mpc_configure_mem_region(mpc, start_addr,end_addr, (uint32_t)sec_attr);
}

/*
    @brief     - Reads 'size' bytes from NVRAM at a given 'base + offset' into given buffer.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - tbsa_status_t
*/
tbsa_status_t val_nvram_read(addr_t base, uint32_t offset, void *buffer, int size)
{
    if(pal_nvram_read(base, offset, buffer, size)) {
        return TBSA_STATUS_SUCCESS;
    } else {
        return TBSA_STATUS_ERROR;
    }
}

/*
    @brief     - Writes 'size' bytes from buffer into NVRAM at a given 'base + offset'.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - tbsa_status_t
*/
tbsa_status_t val_nvram_write(addr_t base, uint32_t offset, void *buffer, int size)
{
    if(pal_nvram_write(base, offset, buffer, size)) {
        return TBSA_STATUS_SUCCESS;
    } else {
        return TBSA_STATUS_ERROR;
    }
}

/**
    @brief    - Initialize RTC
    @param    - void
    @return   - tbsa_status_t
**/
tbsa_status_t val_rtc_init (void)
{
    tbsa_status_t         status;
    soc_peripheral_desc_t *rtc_desc;
    uint32_t              instance = 0;

    do {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_RTC, instance),
                                       (uint8_t **)&rtc_desc,
                                       (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (status != TBSA_STATUS_SUCCESS) {
             return status;
        }

        pal_rtc_init(rtc_desc->base);

        instance++;
    } while(instance);

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - RTC validity mechanism to indicate RTC is Trusted/Non-trusted
    @param    - Base address of the given RTC instance
    @return   - 1 - Valid
                0 - Non-valid
**/
bool_t val_is_rtc_trustable (addr_t addr)
{
    return pal_is_rtc_trustable(addr);
}

/**
    @brief    - Returns whether RTC is synced to server or not.
    @param    - Base address of the given RTC instance
    @return   - 1 - Synced
                0 - Not-synced
**/
bool_t val_is_rtc_synced_to_server (addr_t addr)
{
    return pal_is_rtc_synced_to_server(addr);
}
