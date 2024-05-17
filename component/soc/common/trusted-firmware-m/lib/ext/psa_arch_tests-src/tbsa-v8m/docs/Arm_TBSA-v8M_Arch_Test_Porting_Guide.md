
# Porting Guide - TBSA-v8M Architecture test suite
-----------------------------------------------------

## Introduction
The TBSA-v8M Architecture test suite contains a platform abstraction layer (PAL) which abstracts platform specific information from the tests.
 - The PAL layer interface functions need to be implemented/ported to the target platform.
 - The target config file must be created/updated to match the details of the target platform.

This document provides details on the porting steps and the PAL APIs.

## Porting steps

### Target configuration

  You must populate your system configuration and provide it as an input to test suite.

This is captured in a single static input configuration file that is named as tbsa_tgt.cfg. This file is available at syscomp_tbsa_m/platform/board/<platform_name>/. <br />

An example of the input configuration file is as shown.

	//PERIPHERALS
	timer.num = 2;
	timer.0.vendor_id = 0x0;
	timer.0.device_id = 0x0;
	timer.0.base = 0x50000000;
	timer.0.intr_id = 0x8;
	timer.0.attribute = SECURE_PROGRAMMABLE;
	//MEMORY
	sram.num = 2;
	sram.1.start = 0x30000000;
	sram.1.end = 0x303FFFFF;
	sram.1.attribute = MEM_SECURE;
	sram.1.mem_type = TYPE_NORMAL_READ_WRITE;
	sram.1.dpm_index = 0;

  More details on the structure of the input can be obtained from val/include/val_target.h.


### Create a new target

  Since TBSA-v8M test suite is agnostic to various system targets, before building the tests, you must port the files mentioned in the following steps.

**Procedure**
----------------
  - Create a new directory in platform/board/<platform_name>. For reference, see the existing platform fvp.
  - The peripheral code exists inside platform/peripherals/<peripheral_name>. If <platform_name> is using the peripherals that already exist in platform/peripherals/<peripheral_name>,
    then this code can be reused. Otherwise, the code must be ported for platform specific peripherals.
  - Update platform/board/<platform_name>/Makefile with the appropriate path of the peripherals used.
  - Update platform/board/<platform_name>/src/pal_baremetal_intf.c or pal_cmsis_intf.c with the correct instance of the peripherals used.
  - Update the primary input for the TBSA-v8M tests, that is, target configuration file in platform/board/<platform_name>/tbsa_tgt.cfg. Use platform/boards/fvp/tbsa_tgt.cfg as reference.
  - Refer val/include/val_target.h for structure details.

**Note**:
  pal_nvram_read and pal_nvram_write of the reference FVP platform code simulate non-volatility of the data across resets by ensuring that the memory range is not initialized across warm boots.
  A partner board may choose to simulate the same or provide NVRAM using external storage or Internal Flash.

## PAL API list
  These functions will require implementation/porting to the target platform. <br />

**Note**:
The NVIC functions are CMSIS compliant. The CMSIS repository on Github is cloned during build. A partner need not port the NVIC functions if there are no platform specific changes.

| No | Prototype                                                                                                                                              | Description                                                              | Parameters                                                                              |
|----|--------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------|-----------------------------------------------------------------------------------------|
| 01 | void pal_NVIC_EnableIRQ(uint32_t intr_num);                                                                                                            | Enable Interrupt                                                         | intr_num: Interrupt number<br/>                                                         |
| 02 | void pal_NVIC_DisableIRQ(uint32_t intr_num);                                                                                                           | Disable Interrupt                                                        | intr_num: Interrupt number<br/>                                                         |
| 03 | uint32_t pal_NVIC_ClearTargetState(uint32_t intr_num);                                                                                                 | Clear Interrupt Target State                                             | intr_num: Interrupt number<br/>                                                         |
| 04 | uint32_t pal_NVIC_SetTargetState(uint32_t intr_num);                                                                                                   | Set Interrupt Target Stat                                                | intr_num: Interrupt number<br/>                                                         |
| 05 | void pal_NVIC_SetPriority(uint32_t intr_num, uint32_t priority);                                                                                       | Set Interrupt Priority                                                   | intr_num: Interrupt number<br/>priority: Priority to set<br/>                                                         |
| 06 | uint32_t pal_NVIC_GetPriority(uint32_t intr_num);                                                                                                      | Get Interrupt Priority                                                   | intr_num: Interrupt number<br/>                                                         |
| 07 | void pal_NVIC_SetPendingIRQ(uint32_t intr_num);                                                                                                        | Set Pending Interrupt                                                    | intr_num: Interrupt number<br/>                                                         |
| 08 | void pal_NVIC_ClearPendingIRQ(uint32_t intr_num);                                                                                                      | Clear Pending Interrupt                                                  | intr_num: Interrupt number<br/>                                                         |
| 09 | uint32_t pal_NVIC_GetPendingIRQ(uint32_t intr_num);                                                                                                    | Get Pending Interrupt                                                    | intr_num: Interrupt number<br/>                                                         |
| 10 | uint32_t pal_NVIC_GetActive(uint32_t intr_num);                                                                                                        | Get Active Interrupt                                                     | intr_num: Interrupt number<br/>                                                         |
| 11 | int32_t pal_i2c_init(addr_t addr);                                                                                                                     | Initialize I2C peripheral                                                | addr : Address of the peripheral<br/>                                                   |
| 12 | int32_t  pal_i2c_read(addr_t slv_addr, uint8_t *rd_data, uint32_t num);                                                                                | Read peripheral using I2C                                                | slv_addr: Slave address<br/>rd_data : Pointer to buffer for data to receive from I2C Slave<br/>num     : Number of data bytes to receive<br/>                                                            |
| 13 | int32_t  pal_i2c_write(addr_t slv_addr, uint8_t *wr_data, uint32_t num);                                                                               | Read peripheral using I2C                                                | slv_addr: Slave address<br/>wr_data : Pointer to buffer with data to transmit to I2C slave<br/>num     : Number of bytes to transfer<br/>                                                            |
| 14 | int32_t pal_spi_init(addr_t addr);                                                                                                                     | Initialize SPI peripheral                                                | addr : Address of the peripheral<br/>                                                   |
| 15 | int32_t  pal_spi_read(addr_t addr, void *data, uint32_t num);                                                                                          | Read peripheral using SPI commands                                       | addr : Address of the peripheral<br/>data : Read buffer<br/>num  : Number of bytes to receive<br/>                                                   |
| 16 | int32_t  pal_spi_write(addr_t addr, const void *data, uint32_t num);                                                                                   | Write peripheral using SPI commands                                      | addr : Address of the peripheral<br/>data : Write buffer<br/>num  : Number of bytes to transfer<br/>                                                   |
| 17 | int  pal_timer_init(addr_t addr, uint32_t time_us, uint32_t timer_tick_us);                                                                            | Initializes a hardware timer                                             | addr          : Address of the peripheral<br/>time_us       : Time in micro seconds<br/>timer_tick_us : Number of ticks per micro seconds<br/>                                          |
| 18 | int  pal_timer_enable(addr_t addr);                                                                                                                    | Enables a hardware timer                                                 | addr : Address of the peripheral<br/>                                                   |
| 19 | int  pal_timer_disable(addr_t addr);                                                                                                                   | Disables a hardware timer                                                | addr : Address of the peripheral<br/>                                                   |
| 20 | int  pal_timer_interrupt_clear(addr_t addr);                                                                                                           | Clears the interrupt status of timer                                     | addr : Address of the peripheral<br/>                                                   |
| 21 | int  pal_wd_timer_init(addr_t addr, uint32_t time_us, uint32_t timer_tick_us);                                                                         | Initializes a hardware watchdog timer                                    | addr          : Address of the peripheral<br/>time_us       : Time in micro seconds<br/>timer_tick_us : Number of ticks per micro second<br/>                                          |
| 22 | int  pal_wd_timer_enable(addr_t addr);                                                                                                                 | Enables a hardware watchdog timer                                        | addr : Address of the peripheral<br/>                                                   |
| 23 | int  pal_wd_timer_disable(addr_t addr);                                                                                                                | Disables a hardware watchdog timer                                       | addr : Address of the peripheral<br/>                                                   |
| 24 | int  pal_is_wd_timer_enabled(addr_t addr);                                                                                                             | Checks whether hardware watchdog timer is enabled                        | addr : Address of the peripheral<br/>                                                   |
| 25 | void pal_crypto_init(addr_t crypto_base_addr);                                                                                                         | Initializes the cryptographic functions                                  | crypto_base_addr : base address of the crypto module<br/>                               |
| 26 | int  pal_crypto_aes_generate_key(uint8_t *key, uint32_t size);                                                                                         | Generates AES key using various specified entropy sources                | key  : The buffer where the generated key is stored<br/>size : Size of the key to be generated. Valid options are:<br/>          - 128 bits<br/>          - 192 bits<br/>          - 256 bits<br/>                                |
| 27 | int  pal_crypto_compute_hash(unsigned char *input, uint32_t ilen, unsigned char *output, int algo);                                                    | This function calculates the SHA-224 or SHA-256 checksum of a buffer     | input   : The buffer holding the data<br/>ilen    : The length of the input data<br/>output  : The SHA-224 or SHA-256 checksum result<br/>algo    : Determines which function to use<br/>          0: Use SHA-256<br/>          1: Use SHA-224<br/>                                              |
| 28 | void *uart_get_cmsis_driver(addr_t addr);                                                                                                              | Gets the CMSIS structure address                                         | addr : Address of the peripheral<br/>                                                   |
| 29 | int32_t pal_uart_init (addr_t addr);                                                                                                                   | This function initializes the uart                                       | addr : Address of the peripheral<br/>                                                   |
| 30 | int32_t pal_uart_tx   (addr_t addr, const void *data, uint32_t num);                                                                                   | Send data to UART TX FIFO                                                | addr : Address of the peripheral<br/>data : Data to be written to TX FIFO<br/>num  : Number of bytes<br/>                                                   |
| 31 | void *pal_get_target_cfg_start(void);                                                                                                                  | Provides the database source location                                    | void<br/>                                                                               |
| 32 | int  pal_nvram_write(addr_t base, uint32_t offset, void *buffer, int size);                                                                            | Writes 'size' bytes from buffer into NVRAM at a given 'base + offset'    | base      : Base address of NVRAM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                                                  |
| 33 | int  pal_nvram_read (addr_t base, uint32_t offset, void *buffer, int size);                                                                            | Reads 'size' bytes from NVRAM at a given 'base + offset' into given buffer| base      : Base address of NVRAM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                                                  |
| 34 | void  pal_system_warm_reset(void);                                                                                                                     | Generates system warm reset                                              | void<br/>                                                                               |
| 35 | void  pal_system_cold_reset(void);                                                                                                                     | Generates system cold reset                                              | void<br/>                                                                               |
| 36 | int  pal_is_cold_reset(void);                                                                                                                          | Reports whether the last reset is cold reset                             | void<br/>                                                                               |
| 37 | int  pal_is_warm_reset(void);                                                                                                                          | Reports whether the last reset is warm reset                             | void<br/>                                                                               |
| 38 | int pal_is_wdog_reset(void);                                                                                                                           | Reports whether the last reset is watch dog reset                        | void<br/>                                                                               |
| 39 | int pal_dpm_set_access_ns_only(uint32_t index, bool_t access_ns);                                                                                      | This function will set the debug permission based on the input arg       | index          : DPM index<br/>access_ns      : TRUE  - allow debug access only for non-secure address<br/>                 FALSE - allow debug access to both secure and non-secure addresses<br/>                                                         |
| 40 | int pal_mpc_configure_mem_region(addr_t mpc, addr_t start_addr,addr_t end_addr, uint32_t attr);                                                        | Allow a memory region to be configured as per attribute specified        | mpc        : MPC address<br/>start_addr : Start of memory address to be configured by MPC<br/>end_addr   : End of memory address to be configured by MPC<br/>attr       : 0 - Secure, 1 - Non-secure, 2 - NSC, 3 - Configurable<br/>                                                           |
| 41 | int pal_fuse_read(addr_t addr, uint32_t *data, uint32_t size);                                                                                         | Read the value of given fuse address                                     | addr     : Address of the fuse<br/>data     : Buffer to store the data<br/>size     : Number of words to be read<br/>                                                     |
| 42 | int pal_fuse_write(addr_t addr, uint32_t *data, uint32_t size);                                                                                        | Write the value in given fuse address                                    | addr     : Address of the fuse<br/>data     : Data to be written<br/>size     : Number of words to write<br/>                                                     |
| 43 | int pal_fuse_count_zeros_in_rotpk(uint32_t *zero_cnt);                                                                                                 | Count the number of Zeros in ROTPK                                       | zero_cnt : Buffer to store the zero count<br/>                                          |
| 44 | void pal_fuse_count_zeros(uint32_t value, uint32_t *zero_cnt);                                                                                         | Count the number of Zeros in the given value                             | value    : Number of zeros to be determined<br/>zero_cnt : Buffer to store the zero count<br/>                                        |
| 45 | int pal_fuse_get_lcs(uint32_t *pLcs);                                                                                                                  | Reads the LCS register                                                   | pLcs : Buffer to store the LCS value<br/>                                               |
| 46 | int pal_crypto_validate_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);        | Validates the certificate using public key                               | certificate_base_addr : Base address of the certificate where it is stored in memory<br/>public_key_addr       : Base address of the public key where it is stored in memory<br/>certificate_size      : Certificate memory size<br/>public_key_size       : Public key memory size<br/>|
| 47 | int pal_crypto_get_uniqueID_from_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);| Get unique ID from valid certificate using public key                    | certificate_base_addr : Base address of the certificate where it is stored in memory<br/>public_key_addr       : Base address of the public key where it is stored in memory<br/>certificate_size      : Certificate memory size<br/>public_key_size       : Public key memory size<br/>|
| 48 | int pal_rtc_init(addr_t addr);                                                                                                                         | Initialize RTC                                                           | addr : Address of peripheral<br/>                                                       |
| 49 | int pal_is_rtc_trustable(addr_t addr);                                                                                                                 | RTC validity mechanism to indicate RTC is Trusted/Non-trusted            | addr : Address of peripheral<br/>                                                       |
| 50 | int pal_is_rtc_synced_to_server(addr_t addr);                                                                                                          | RTC validity mechanism to indicate RTC is synced with server or not      | addr : Address of peripheral<br/>                                                       |
| 51 | int pal_crypto_get_dpm_from_key(addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);                                                | Get DPM field from public key                                            | public_key_addr : Base address of the public key where it is stored in memory<br/>public_key_size : Public key memory size<br/>dpm_field       : Buffer to store DPM number<br/>      |
| 52 | int pal_crypto_get_dpm_from_certificate(addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);                                 | Get DPM field from certificate                                           | certificate_base_addr : Base address of the certificate where it is stored in memory<br/>certificate_size      : certificate memory size<br/>dpm_field             : Buffer to store DPM number<br/>|
| 53 | int pal_firmware_version_update(uint32_t instance, uint32_t firmware_version_type, uint32_t fw_ver_cnt);                                               | Update the firmware version                                              | instance              : Instance of the firmware<br/>firmware_version_type : 0 - Trusted, 1 - Non-trusted<br/>fw_ver_cnt            : Version of the firmware<br/>                                   |
| 54 | int pal_firmware_version_read(uint32_t instance, uint32_t firmware_version_type);                                                                      | Read the firmware version                                                | instance              : Instance wf the firmware<br/>firmware_version_type : 0 - Trusted, 1 - Non-trusted<br/>                                   |
| 55 | int pal_crypto_revoke_key(uint32_t index, addr_t addr, uint32_t size);                                                                                 | Revoke the given key                                                     | index   : Index of the key<br/>addr    : Address of the key<br/>size    : Size of the key<br/>                                                         |
| 56 | int pal_dpm_set_state(uint32_t index, uint32_t dpm_status, uint32_t unlock_token, void *token);                                                        | This function will set the DPM state                                     | index          : DPM index<br/>dpm_status     : DPM status as per TBSA spec, can returned as logical OR of in relevant cases<br/>                 DPM_LOCK_IMPLEMENTED/DPM_LOCK_VALUE<br/>                 DPM_EN_IMPLEMENTED/DPM_EN_VALUE<br/>unlock_token   : unlock token in case of unlocking the DPM state<br/>token          : Pointer to passowrd or certificate or challenge response<br/>                                                         |
| 57 | int pal_dpm_get_state(uint32_t index, uint32_t *dpm_status);                                                                                           | This function will get the DPM state                                     | index          : DPM index<br/>dpm_status     : DPM status as per TBSA spec, defined in dpm_status_t<br/>                                                         |

## License
Arm TBSA-v8M Architecture test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
