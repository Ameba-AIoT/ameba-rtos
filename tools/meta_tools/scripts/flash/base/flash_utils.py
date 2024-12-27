#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from enum import Enum


class FlashUtils(Enum):
    # NAND Flash MID
    NandMfgDosilicon = 0xE5
    NandMfgGigadevice = 0xC8
    NandMfgMacronix = 0xC2
    NandMfgMicron = 0x2C
    NandMfgWinbond = 0xEF

    # NAND Flash commands
    NandCmdGetFeatures = 0x0F
    NandCmdSetFeatures = 0x1F

    # NAND Flash registers
    NandRegProtection = 0xA0
    NandRegFeature = 0xB0
    NandRegStatus = 0xC0
    NandRegProtectionBpMask = 0x38
    NandRegProtectionBpMaskWinbondMicron = 0x78

    # NOR Flash command & registers
    NorCmdReadStatusReg1 = 0x05
    NorCmdWriteStatusReg1 = 0x01
    NorStatusReg1BpMask = 0x3C

    # NOR Flash default page/block size
    NorDefaultPageSize = 1024
    NorDefaultPagePerBlock = 4
    NoeDefaultBlockSize = (NorDefaultPageSize * NorDefaultPagePerBlock)

    # NAND Flash default page/block size
    NandDefaultPageSize = 2048
    NandDefaultPagePerBlock = 64
    NandDefaultBlockSize = (NandDefaultPageSize * NandDefaultPagePerBlock)

    # Flash write padding data
    FlashWritePaddingData = 0xFF