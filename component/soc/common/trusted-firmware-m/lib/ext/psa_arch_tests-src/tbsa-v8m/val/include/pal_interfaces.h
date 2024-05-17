/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _PAL_INTERFACES_H_
#define _PAL_INTERFACES_H_

#include "val_common.h"

/**
    @brief    - Enable Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_EnableIRQ(uint32_t intr_num);

/**
    @brief    - Disable Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_DisableIRQ(uint32_t intr_num);

/**
    @brief    - Clear Interrupt Target State
    @param    - intr_num: Interrupt number
    @return   - 0  if interrupt is assigned to Secure
                1  if interrupt is assigned to Non Secure
**/
uint32_t pal_NVIC_ClearTargetState(uint32_t intr_num);

/**
    @brief    - Set Interrupt Target Stat
    @param    - intr_num: Interrupt number
    @return   - 0  if interrupt is assigned to Secure
                1  if interrupt is assigned to Non Secure
**/
uint32_t pal_NVIC_SetTargetState(uint32_t intr_num);

/**
    @brief    - Set Interrupt Priority
    @param    - intr_num: Interrupt number
                priority: Priority to set
    @return   - void
**/
void pal_NVIC_SetPriority(uint32_t intr_num, uint32_t priority);

/**
    @brief    - Get Interrupt Priority
    @param    - intr_num: Interrupt number
    @return   - Interrupt Priority
**/
uint32_t pal_NVIC_GetPriority(uint32_t intr_num);

/**
    @brief    - Set Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_SetPendingIRQ(uint32_t intr_num);

/**
    @brief    - Clear Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_ClearPendingIRQ(uint32_t intr_num);

/**
    @brief    - Get Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - 0  Interrupt status is not pending
                1  Interrupt status is pending
**/
uint32_t pal_NVIC_GetPendingIRQ(uint32_t intr_num);

/**
    @brief    - Get Active Interrupt
    @param    - intr_num: Interrupt number
    @return   - 0  Interrupt status is not active
                1  Interrupt status is active
**/
uint32_t pal_NVIC_GetActive(uint32_t intr_num);

/**
    @brief    - Initialize I2C peripheral
    @param    - addr : Address of the peripheral
    @return   - 0 - Success, 0x80 - Failure
**/
int32_t pal_i2c_init(addr_t addr);

/**
    @brief    - Read peripheral using I2C
    @param    - slv_addr: Slave address
                rd_data : Pointer to buffer for data to receive from I2C Slave
                num     : Number of data bytes to receive
    @return   - error status
**/
int32_t  pal_i2c_read(addr_t slv_addr, uint8_t *rd_data, uint32_t num);

/**
    @brief    - Read peripheral using I2C
    @param    - slv_addr: Slave address
                wr_data : Pointer to buffer with data to transmit to I2C slave
                num     : Number of bytes to transfer
    @return   - error status
**/
int32_t  pal_i2c_write(addr_t slv_addr, uint8_t *wr_data, uint32_t num);

/**
    @brief    - Initialize SPI peripheral
    @param    - addr : Address of the peripheral
    @return   - 0 - Success, 0x80 - Failure
**/
int32_t pal_spi_init(addr_t addr);

/**
    @brief    - Read peripheral using SPI commands
    @param    - addr : Address of the peripheral
                data : Read buffer
                num  : Number of bytes to receive
    @return   - error status
**/
int32_t  pal_spi_read(addr_t addr, void *data, uint32_t num);

/**
    @brief    - Write peripheral using SPI commands
    @param    - addr : Address of the peripheral
                data : Write buffer
                num  : Number of bytes to transfer
    @return   - error status
**/
int32_t  pal_spi_write(addr_t addr, const void *data, uint32_t num);

/**
    @brief    - Initializes a hardware timer
    @param    - addr          : Address of the peripheral
              - time_us       : Time in micro seconds
              - timer_tick_us : Number of ticks per micro seconds
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_init(addr_t addr, uint32_t time_us, uint32_t timer_tick_us);

/**
    @brief    - Enables a hardware timer
    @param    - addr : Address of the peripheral
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_enable(addr_t addr);

/**
    @brief    - Disables a hardware timer
    @param    - addr : Address of the peripheral
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_disable(addr_t addr);

/**
    @brief    - Clears the interrupt status of timer
    @param    - addr : Address of the peripheral
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_interrupt_clear(addr_t addr);

/**
    @brief           - Initializes a hardware watchdog timer
    @param           - addr          : Address of the peripheral
                     - time_us       : Time in micro seconds
                     - timer_tick_us : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_init(addr_t addr, uint32_t time_us, uint32_t timer_tick_us);

/**
    @brief           - Enables a hardware watchdog timer
    @param           - addr : Address of the peripheral
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_enable(addr_t addr);

/**
    @brief           - Disables a hardware watchdog timer
    @param           - addr : Address of the peripheral
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_disable(addr_t addr);


/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - addr : Address of the peripheral
    @return          - Enabled : 1, Disabled : 0
**/
int  pal_is_wd_timer_enabled(addr_t addr);

/**
    @brief    - Initializes the cryptographic functions
    @param    - crypto_base_addr : base address of the crypto module
    @return   - none
**/
void pal_crypto_init(addr_t crypto_base_addr);

/**
    @brief    - Generates AES key using various specified entropy sources
    @param    - key  : The buffer where the generated key is stored
                size : Size of the key to be generated. Valid options are:
                          - 128 bits
                          - 192 bits
                          - 256 bits
    @return   - 0 on success.
**/
int  pal_crypto_aes_generate_key(uint8_t *key, uint32_t size);

/**
    @brief    - This function calculates the SHA-224 or SHA-256 checksum of a buffer
    @param    - input   : The buffer holding the data
                ilen    : The length of the input data
                output  : The SHA-224 or SHA-256 checksum result
                algo    : Determines which function to use
                          0: Use SHA-256
                          1: Use SHA-224
    @return   - 0 on success.
**/
int  pal_crypto_compute_hash(unsigned char *input, uint32_t ilen, unsigned char *output, int algo);

/*
    @brief     - Gets the CMSIS structure address
    @param     - addr : Address of the peripheral
    @return    - address
*/
void *uart_get_cmsis_driver(addr_t addr);

/*
    @brief     - This function initializes the uart
    @param     - addr : Address of the peripheral
    @return    - 0 - Success, 0x80 - Failure
*/
int32_t pal_uart_init (addr_t addr);

/*
    @brief     - Send data to UART TX FIFO
    @param     - addr : Address of the peripheral
                 data : Data to be written to TX FIFO
                 num  : Number of bytes
    @return    - 0 - Success, 0x80 - Failure
*/
int32_t pal_uart_tx   (addr_t addr, const void *data, uint32_t num);

/**
    @brief    - Provides the database source location
    @param    - void
    @return   - Returns base address of database
**/
void *pal_get_target_cfg_start(void);

/*
    @brief     - Writes 'size' bytes from buffer into NVRAM at a given 'base + offset'
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - error status
*/
int  pal_nvram_write(addr_t base, uint32_t offset, void *buffer, int size);

/*
    @brief     - Reads 'size' bytes from NVRAM at a given 'base + offset' into given buffer
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - 1/0
*/
int  pal_nvram_read (addr_t base, uint32_t offset, void *buffer, int size);

/*
    @brief     - Generates system warm reset
    @param     - void
    @return    - void
*/
void  pal_system_warm_reset(void);

/*
    @brief     - Generates system cold reset
    @param     - void
    @return    - void
*/
void  pal_system_cold_reset(void);

/*
    @brief     - Reports whether the last reset is cold reset
    @param     - void
    @return    - YES(1)/NO(0)
*/
int  pal_is_cold_reset(void);

/*
    @brief     - Reports whether the last reset is warm reset
    @param     - void
    @return    - YES(1)/NO(0)
*/
int  pal_is_warm_reset(void);

/*
    @brief     - Reports whether the last reset is watch dog reset
    @param     - void
    @return    - YES(1)/NO(0)
*/
int pal_is_wdog_reset(void);

/**
    @brief    - This function will set the debug permission based on the input arg
    @param    - index          : DPM index
                access_ns      : TRUE  - allow debug access only for non-secure address
                                 FALSE - allow debug access to both secure and non-secure addresses
    @return   - error status
**/
int pal_dpm_set_access_ns_only(uint32_t index, bool_t access_ns);

/**
    @brief    - Allow a memory region to be configured as per attribute specified
    @param    - mpc        : MPC address
                start_addr : Start of memory address to be configured by MPC
                end_addr   : End of memory address to be configured by MPC
                attr       : 0 - Secure, 1 - Non-secure, 2 - NSC, 3 - Configurable
    @return   - error status
**/
int pal_mpc_configure_mem_region(addr_t mpc, addr_t start_addr,addr_t end_addr, uint32_t attr);

/**
    @brief    - Read the value of given fuse address
    @param    - addr     : Address of the fuse
              - data     : Buffer to store the data
              - size     : Number of words to be read
    @return   - error status
**/
int pal_fuse_read(addr_t addr, uint32_t *data, uint32_t size);

/**
    @brief    - Write the value in given fuse address
    @param    - addr     : Address of the fuse
              - data     : Data to be written
              - size     : Number of words to write
    @return   - error status
**/
int pal_fuse_write(addr_t addr, uint32_t *data, uint32_t size);

/**
    @brief    - Count the number of Zeros in ROTPK
    @param    - zero_cnt : Buffer to store the zero count
    @return   - error status
**/
int pal_fuse_count_zeros_in_rotpk(uint32_t *zero_cnt);

/**
    @brief    - Count the number of Zeros in the given value
    @param    - value    : Number of zeros to be determined
                zero_cnt : Buffer to store the zero count
    @return   -
**/
void pal_fuse_count_zeros(uint32_t value, uint32_t *zero_cnt);

/**
    @brief    - Reads the LCS register
    @param    - pLcs : Buffer to store the LCS value
    @return   - error status
**/
int pal_fuse_get_lcs(uint32_t *pLcs);

/**
    @brief    - Validates the certificate using public key
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - 0 on success.
**/
int pal_crypto_validate_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);

/**
    @brief    - Get unique ID from valid certificate using public key
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - unique ID of the certificate
**/
int pal_crypto_get_uniqueID_from_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);

/**
    @brief    - Initialize RTC
    @param    - addr : Address of peripheral
    @return   - error status
**/
int pal_rtc_init(addr_t addr);

/**
    @brief    - RTC validity mechanism to indicate RTC is Trusted/Non-trusted
    @param    - addr : Address of peripheral
    @return   - error status
**/
int pal_is_rtc_trustable(addr_t addr);

/**
    @brief    - RTC validity mechanism to indicate RTC is synced with server or not
    @param    - addr : Address of peripheral
    @return   - error status
**/
int pal_is_rtc_synced_to_server(addr_t addr);

/**
    @brief    - Get DPM field from public key
    @param    - public_key_addr : Base address of the public key where it is stored in memory
                public_key_size : Public key memory size
                dpm_field       : Buffer to store DPM number
    @return   - error status
**/
int pal_crypto_get_dpm_from_key(addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);

/**
    @brief    - Get DPM field from certificate
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                certificate_size      : certificate memory size
                dpm_field             : Buffer to store DPM number
    @return   - error status
**/
int pal_crypto_get_dpm_from_certificate(addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);

/**
    @brief    - Update the firmware version
    @param    - instance              : Instance of the firmware
                firmware_version_type : 0 - Trusted, 1 - Non-trusted
                fw_ver_cnt            : Version of the firmware
    @return   - error status
**/
int pal_firmware_version_update(uint32_t instance, uint32_t firmware_version_type, uint32_t fw_ver_cnt);

/**
    @brief    - Read the firmware version
    @param    - instance              : Instance wf the firmware
                firmware_version_type : 0 - Trusted, 1 - Non-trusted
    @return   - error status
**/
int pal_firmware_version_read(uint32_t instance, uint32_t firmware_version_type);

/**
    @brief    - Revoke the given key
    @param    - index   : Index of the key
                addr    : Address of the key
                size    : Size of the key
    @return   - error status
**/
int pal_crypto_revoke_key(uint32_t index, addr_t addr, uint32_t size);

/**
    @brief    - This function will set the DPM state
    @param    - index          : DPM index
                dpm_status     : DPM status as per TBSA spec, can returned as logical OR of in relevant cases
                                 DPM_LOCK_IMPLEMENTED/DPM_LOCK_VALUE
                                 DPM_EN_IMPLEMENTED/DPM_EN_VALUE
                unlock_token   : unlock token in case of unlocking the DPM state
                token          : Pointer to passowrd or certificate or challenge response
    @return   - error status
**/
int pal_dpm_set_state(uint32_t index, uint32_t dpm_status, uint32_t unlock_token, void *token);

/**
    @brief    - This function will get the DPM state
    @param    - index          : DPM index
              - dpm_status     : DPM status as per TBSA spec, defined in dpm_status_t
    @return   - error status
**/
int pal_dpm_get_state(uint32_t index, uint32_t *dpm_status);

#endif /* PAL_INTERFACES_H */
