#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .image_info import *
from .efuse_data import *
from .memory_info import *
from .version import *


class RtkDeviceProfile():
    DEFAULT_FLASH_START_ADDR = MemoryInfo.FLASH_BASE_ADDRESS
    DEFAULT_RAM_START_ADDR = 0x20000000
    DEVICE_ID_AMEBAD = 0x6548
    DEVICE_ID_AMEBAZ = 0x6547

    # Since 1.2.0 the device-level MemoryType is removed, the partition MemoryType holds
    # the partition type (RAM/FLASH/SECOND FLASH) and NAND partitions are addressed from
    # 0x08000000 like NOR. The NOR/NAND flash type is reported by the device.
    PARTITION_TYPE_VERSION = (1, 2, 0)
    VERSION = "1.2.0"

    def __init__(self, **kwargs):
        self.version = kwargs.get("Version", "1.1.1")
        self.device_name = kwargs.get("DeviceName", "")
        self.device_id = kwargs.get("DeviceID", 0)
        self.support_usb_download = kwargs.get("SupportUsbDownload", False)
        self.flash_start_address = kwargs.get("FlashStartAddress", self.DEFAULT_FLASH_START_ADDR)
        self.ram_start_address = kwargs.get("RamStartAddress", self.DEFAULT_RAM_START_ADDR)
        self.floader = kwargs.get("Floader", "")
        self.floader_address = kwargs.get("FloaderAddress", 0)
        self.handshake_baudrate = kwargs.get("HandshakeBaudrate", 0)
        self.log_baudrate = kwargs.get("LogBaudrate", 0)
        self.logical_efuse_len = kwargs.get("LogicalEfuseLen", 0)
        self.physical_efuse_len = kwargs.get("PhysicalEfuseLen", 0)
        self.images = []
        self.default_efuse_map = []
        for image_info in kwargs.get("Images", []):
            self.images.append(ImageInfo(**image_info))

        for efuse_data in kwargs.get("DefaultEfuseMap", []):
            self.default_efuse_map.append(EfuseData(**efuse_data))

        # Set when a profile older than 1.2.0 was converted on load
        self.upgraded = False
        self._upgrade_legacy(kwargs.get("MemoryType", MemoryInfo.MEMORY_TYPE_RAM))

    def _upgrade_legacy(self, legacy_device_memory_type):
        """Convert a profile older than 1.2.0 to the partition type layout, the same way
        as the Device Profile Editor: RAM -> RAM, NOR/NAND -> FLASH, Vendor -> SECOND FLASH,
        and NAND partitions are rebased from 0x00000000 to 0x08000000 to align with NOR."""
        if self.get_version().to_tuple() >= self.PARTITION_TYPE_VERSION:
            return

        is_nand = (legacy_device_memory_type == MemoryInfo.MEMORY_TYPE_NAND)
        for image in self.images:
            if MemoryInfo.upgrade_legacy_image(image):
                is_nand = True

        if is_nand:
            self.flash_start_address = self.DEFAULT_FLASH_START_ADDR

        self.version = self.VERSION
        self.upgraded = True

    def is_amebad(self):
        return (self.device_id == self.DEVICE_ID_AMEBAD)

    def is_amebaz(self):
        return (self.device_id == self.DEVICE_ID_AMEBAZ)

    def is_ram_address(self, address):
        return (address >= self.DEFAULT_RAM_START_ADDR)

    def is_flash_address(self, address):
        return (address >= self.DEFAULT_FLASH_START_ADDR)

    def get_version(self):
        if self.version:
            return Version(self.version)
        else:
            return Version("1.0.0")

    def __repr__(self):
        image_info_list = [ii.__repr__() for ii in self.images]
        efuse_data_list = [ed.__repr__() for ed in self.default_efuse_map]
        profile_dict = {
            "Images": image_info_list,
            "DefaultEfuseMap": efuse_data_list,
            "Version": f"{self.version}",
            "DeviceName": self.device_name,
            "DeviceID": self.device_id,
            "SupportUsbDownload": self.support_usb_download,
            "FlashStartAddress": self.flash_start_address,
            "RamStartAddress": self.ram_start_address,
            "Floader": self.floader,
            "FloaderAddress": self.floader_address,
            "HandshakeBaudrate": self.handshake_baudrate,
            "LogBaudrate": self.log_baudrate,
            "LogicalEfuseLen": self.logical_efuse_len,
            "PhysicalEfuseLen": self.physical_efuse_len
        }

        return profile_dict
