#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .image_info import *
from .efuse_data import *
from .version import *


class RtkDeviceProfile():
    DEFAULT_FLASH_START_ADDR = 0x08000000
    DEFAULT_RAM_START_ADDR = 0x20000000
    DEVICE_ID_AMEBAD = 0x6548
    DEVICE_ID_AMEBAZ = 0x6547

    def __init__(self, **kwargs):
        self.version = kwargs.get("Version", "1.0.0")
        self.device_name = kwargs.get("DeviceName", "")
        self.device_id = kwargs.get("DeviceID", 0)
        self.memory_type = kwargs.get("MemoryType", 0)
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
            "MemoryType": self.memory_type.value,
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
