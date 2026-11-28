#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


class MemoryInfo:
    # Floader protocol memory types: the WRITE / READ / CHKSM / FS_ERASE type field,
    # and the memory type the device reports in QUERY (NOR or NAND)
    MEMORY_TYPE_RAM = 0
    MEMORY_TYPE_NOR = 1
    MEMORY_TYPE_NAND = 2
    MEMORY_TYPE_VENDOR = 3
    MEMORY_TYPE_HYBRID = 4

    # Device profile (>= 1.2.0) partition types, saved as the partition MemoryType.
    # The profile does not tell NOR from NAND: a FLASH partition is downloaded to the
    # flash type the device reports, see to_protocol_memory_type()
    PARTITION_TYPE_RAM = 0
    PARTITION_TYPE_FLASH = 1
    PARTITION_TYPE_SECOND_FLASH = 2
    MAX_PARTITION_TYPE = PARTITION_TYPE_SECOND_FLASH

    # Flash address base of the device profile, for both NOR and NAND. NOR is accessed
    # by its XIP address, so the floader takes it as is, while NAND is not memory-mapped
    # and the floader takes its physical byte offset from 0.
    FLASH_BASE_ADDRESS = 0x08000000
    NAND_PHYSICAL_BASE_ADDRESS = 0x00000000

    def __init__(self):
        self.start_address = 0
        self.end_address = 0
        self.memory_type = self.MEMORY_TYPE_RAM
        self.size_in_kbyte = 0

    def size_in_byte(self):
        return self.size_in_kbyte * 1024

    @classmethod
    def is_flash_partition(cls, partition_type):
        return partition_type in (cls.PARTITION_TYPE_FLASH, cls.PARTITION_TYPE_SECOND_FLASH)

    @classmethod
    def to_protocol_memory_type(cls, partition_type, device_memory_type):
        """Convert a device profile partition type to the floader protocol memory type:
        RAM -> RAM, FLASH -> the flash type the device reports (NOR/NAND), SECOND FLASH -> VENDOR."""
        if partition_type == cls.PARTITION_TYPE_RAM:
            return cls.MEMORY_TYPE_RAM
        if partition_type == cls.PARTITION_TYPE_SECOND_FLASH:
            return cls.MEMORY_TYPE_VENDOR
        return device_memory_type

    @classmethod
    def to_device_address(cls, address, protocol_memory_type):
        """Convert a device profile address to the address the floader takes: NAND is
        addressed by its physical offset from 0, NOR / RAM / VENDOR addresses are kept."""
        if protocol_memory_type != cls.MEMORY_TYPE_NAND:
            return address
        if address < cls.FLASH_BASE_ADDRESS:
            raise ValueError(f"NAND address 0x{address:08X} is below the flash base 0x{cls.FLASH_BASE_ADDRESS:08X}")
        return address - cls.FLASH_BASE_ADDRESS + cls.NAND_PHYSICAL_BASE_ADDRESS

    @classmethod
    def to_profile_address(cls, address, protocol_memory_type):
        """Inverse of to_device_address(), for logs and diagnostics."""
        if protocol_memory_type != cls.MEMORY_TYPE_NAND:
            return address
        return address - cls.NAND_PHYSICAL_BASE_ADDRESS + cls.FLASH_BASE_ADDRESS

    @classmethod
    def from_legacy_memory_type(cls, legacy_memory_type):
        """Partition type of a legacy partition MemoryType, i.e. the floader memory type used by
        device profiles older than 1.2.0 and the --partition-table of the previous versions.
        Same mapping as ImageTool: RAM -> RAM, NOR/NAND -> FLASH, VENDOR -> SECOND FLASH."""
        if legacy_memory_type == cls.MEMORY_TYPE_RAM:
            return cls.PARTITION_TYPE_RAM
        if legacy_memory_type == cls.MEMORY_TYPE_VENDOR:
            return cls.PARTITION_TYPE_SECOND_FLASH
        return cls.PARTITION_TYPE_FLASH

    @classmethod
    def upgrade_legacy_image(cls, image, rebase_nand=True):
        """Convert a legacy image in place: MemoryType to the partition type, and a NAND image
        rebased from the physical offset to 0x08000000 like NOR. A NAND address already at or
        above 0x08000000 is kept. Returns True if the image is a legacy NAND image."""
        is_nand = (image.memory_type == cls.MEMORY_TYPE_NAND)
        if is_nand and rebase_nand:
            if image.start_address < cls.FLASH_BASE_ADDRESS:
                image.start_address = cls.to_profile_address(image.start_address, cls.MEMORY_TYPE_NAND)
            if image.end_address < cls.FLASH_BASE_ADDRESS:
                image.end_address = cls.to_profile_address(image.end_address, cls.MEMORY_TYPE_NAND)
        image.memory_type = cls.from_legacy_memory_type(image.memory_type)
        return is_nand
