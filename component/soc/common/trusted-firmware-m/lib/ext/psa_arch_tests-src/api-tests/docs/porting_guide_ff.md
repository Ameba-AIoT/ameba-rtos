
# Porting Guide: PSA-FF Architecture Test Suite
-----------------------------------------------------

## Introduction
The architecture test suite contains a *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your target platform. Create and update the target configuration file to match the details of this target platform.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Target configuration

You must populate your system configuration and provide it as an input to test suite. The system configuration is captured in a single static input configuration file called **target.cfg**, available at **api-tests/platform/targets/<platform_name>/**. <br />

An example input configuration file is as shown.

    // UART device info
    uart.num=1;
    uart.0.base = 0x40004000;
    uart.0.size = 0xFFF;
    uart.0.intr_id = 0xFF;
    uart.0.permission = TYPE_READ_WRITE;

    // Watchdog device info
    watchdog.num = 1;
    watchdog.0.base = 0x40008000;
    watchdog.0.size = 0xFFF;
    watchdog.0.intr_id = 0xFF;
    watchdog.0.permission = TYPE_READ_WRITE;

  For details on the structure of the input, refer to **val/common/val_target.h**.

### Adding a new target

  1. Create a new directory in **platform/targets/<platform_name>**. For reference, see the existing platform tgt_ff_tfm_an521 directory.
  2. Execute `cp -rf platform/targets/tgt_ff_tfm_an521/ platform/targets/<platform_name>/`.
  3. Update **platform/targets/<platform_name>/target.cfg** with your target platform details. Refer to **val/common/val_target.h** for structure details.
  4. Update the platform information available in manifest files located in  **platform/manifests/** directory with your platform information. The platform details must match the device details provided in the target.cfg file.
  5. Update **platform/targets/<platform_name>/target.cmake** appropriately to select the correct instances of PAL files for compilation.
  6. Refer to the **List of PAL APIs** section to view the list of PAL APIs that must be ported for your target platform. These API definitions are available in **nspe/<suite_name>/pal_\*\_intf.c** and **spe/pal_\*\_intf.c** files. These APIs are written for **tgt_ff_tfm_an521** platform. You can reuse the code if it works for your platform. Otherwise, you must port them for your platform-specific peripherals.

**Note**:
- The test suite requires access to the peripherals mentioned below. For IPC suite, driver functionalities are implemented as RoT-services in driver partition. Other Secure partitions and Non-secure code calls to these RoT-services to get appropriate driver services.
  - One UART to print Test NSPE or SPE messages and to cover secure partition interrupt handling scenarios
  - One Watchdog timer to help recover from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset. Each byte of this region must be initialised to FF at power on reset.


## List of PAL APIs

Since the test suite is agnostic to various system targets, you must port the following PAL NSPE APIs before building the tests. Implement these functions for your target platform. <br />

The following is the list of PAL APIs used in SPE: <br />

| No | Prototype                                                                         | Description                                                                       | Parameters                                               |
|----|-----------------------------------------------------------------------------------|-----------------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | void pal_uart_init(addr_t uart_base_addr);                                        | Initializes the UART                                                | uart_base_addr : Base address of the UART<br/>           |
| 02 | void pal_print(char *str, int32_t data);                                         | Parses the input string and writes byte by byte to print            | str            : Input String<br/>data           : Value for Format specifier<br/>                       |
| 03 | int  pal_wd_timer_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);| Initializes a hardware watchdog timer                                            | base_addr       : Base address of the watchdog module<br/>time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 04 | int  pal_wd_timer_enable(addr_t base_addr);                                       | Enables a hardware watchdog timer                                                 | base_addr     : Base address of the watchdog module<br/> |
| 05 | int  pal_wd_timer_disable(addr_t base_addr);                                      | Disables a hardware watchdog timer                                                | base_addr     : Base address of the watchdog module<br/> |
| 06 | int  pal_wd_timer_is_enabled(addr_t base_addr);                                   | Checks whether hardware watchdog timer is enabled                                 | base_addr     : Base address of the watchdog module<br/> |
| 07 | int  pal_nvmem_write(addr_t base, uint32_t offset, void *buffer, int size);       | Writes 'size bytes from buffer into non-volatile memory at a given 'base + offset'| base      : Base address of NV MEM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                  |
| 08 | int  pal_nvmem_read(addr_t base, uint32_t offset, void *buffer, int size);       | Reads 'size' bytes from non-volatile memory at a given                            | base      : Base address of NV MEM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                  |
| 09 | void pal_generate_interrupt(void);                                               | Trigger interrupt for IRQ signal assigned to driver partition                      | None |
| 10 | void pal_disable_interrupt(void);                                                | Disable the interrupt that was generated using pal_generate_interrupt API.              | None |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2020, Arm Limited and Contributors. All rights reserved.*
