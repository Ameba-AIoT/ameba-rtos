#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


class MemoryInfo:
    MEMORY_TYPE_RAM = 0
    MEMORY_TYPE_NOR = 1
    MEMORY_TYPE_NAND = 2
    MEMORY_TYPE_VENDOR = 3
    MEMORY_TYPE_HYBRID = 4
    MAX_PARTITION_MEMORY_TYPE = MEMORY_TYPE_VENDOR
    MAX_DEVICE_MEMORY_TYPE = MEMORY_TYPE_HYBRID

    def __init__(self):
        self.start_address = 0
        self.end_address = 0
        self.memory_type = self.MEMORY_TYPE_RAM
        self.size_in_Kbyte = 0

    def size_in_byte(self):
        return self.size_in_Kbyte * 1024
