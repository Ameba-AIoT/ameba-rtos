#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .rtk_flash_type import *
from .memory_info import *


class DeviceInfo(object):
    def __init__(self):
        self.did = 0
        self.image_type = 0
        self.cmd_set_version = 0
        self.wifi_mac = None
        self.memory_type = None
        self.flash_mid = None
        self.flash_did = None
        self.flash_mfg = ""
        self.flash_model = ""
        self.flash_page_size = 0
        self.flash_oob_size = 0
        self.flash_pages_per_block = 0
        self.flash_blocks_per_lun = 0
        self.flash_luns_per_target = None
        self.flash_max_bad_block_per_lun = 0
        self.flash_req_host_ecc_level = None
        self.flash_targets = None
        self.flash_capacity = 0

    def get_wifi_mac_text(self):
        mac_list = []
        for chr in self.wifi_mac:
            mac_list.append(hex(chr)[2:].zfill(2).upper())
        return ":".join(mac_list)

    def flash_block_size(self):
        return self.flash_page_size * self.flash_pages_per_block

    def is_boot_from_nand(self):
        return self.memory_type == MemoryInfo.MEMORY_TYPE_NAND
