#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .memory_info import MemoryInfo


class BootFlashTypeOtpConfig():
    """Where and how an IC selects its boot flash type (NOR/NAND) in logical OTP."""

    def __init__(self, name, address, pos, width, values):
        self.name = name
        self.address = address
        self.pos = pos
        self.mask = ((1 << width) - 1) << pos
        # {MemoryInfo.MEMORY_TYPE_*: field value}
        self.values = values


# Boot flash type select field per DeviceID. When AutoFlashTypeDetect is enabled and the
# flashloader had to switch to the other flash type than the ROM selected, the field is
# programmed with the value of the flash type found by the flashloader.
# Field values not listed here (e.g. 0/3) leave the choice to the trap pin.
# To support a new IC, add its entry here.
BOOT_FLASH_TYPE_OTP_CONFIGS = {
    # AmebaSmart: SEC_OTP_SYSCFG0[21:20] SEC_MASK_BOOT_SELECT -> logical OTP 0x02[5:4]
    0x6678: BootFlashTypeOtpConfig("AmebaSmart", 0x02, 4, 2, {
        MemoryInfo.MEMORY_TYPE_NOR: 2,
        MemoryInfo.MEMORY_TYPE_NAND: 1,
    }),
    # AmebaGreen2: SEC_OTP_SYSCFG0[23:22] SEC_MASK_BOOT_NOR_FLASH -> logical OTP 0x02[7:6]
    0x7005: BootFlashTypeOtpConfig("AmebaGreen2", 0x02, 6, 2, {
        MemoryInfo.MEMORY_TYPE_NOR: 1,
        MemoryInfo.MEMORY_TYPE_NAND: 2,
    }),
}


def get_boot_flash_type_otp_config(device_id):
    return BOOT_FLASH_TYPE_OTP_CONFIGS.get(device_id, None)
