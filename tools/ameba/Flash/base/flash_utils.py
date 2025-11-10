#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from enum import Enum
from .sys_utils import *


class FlashBPS:
    def __init__(self):
        self.need_unlock = False
        self.protection = 0
        self.is_locked = False


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
    NorDefaultBlockSize = (NorDefaultPageSize * NorDefaultPagePerBlock)

    # NAND Flash default page/block size
    NandDefaultPageSize = 2048
    NandDefaultPagePerBlock = 64
    NandDefaultBlockSize = (NandDefaultPageSize * NandDefaultPagePerBlock)

    # Flash write padding data
    FlashWritePaddingData = 0xFF

    MinFlashProcessTimeoutInSecond = 1

    # NOR Flash program / read / erase timeout
    # Max page program time: typical 0.5ms for GD25Q128E, max 3ms for W25Q256JV
    # Take 0.5ms * 10 = 5ms
    NorPageProgramTimeoutInSeconds = 0.005
    # Max 4KB sector erase time: typical 70ms for GD25Q256D, max 400ms for GD25Q256D
    # Take(400ms * 2) > (70ms * 10 = 700ms)
    Nor4kSectorEraseTimeoutInSenonds = 0.8
    # Max 32KB block erase time: typical 0.16s for GD25Q128E, max 1.6s for W25Q256JV
    # Take 2s > 0.16s * 10 = 1.6s
    Nor32kBlockEraseTimeoutInSeconds = 2  # larger than max
    # Max 64KB block erase time: typical 0.25s for GD25Q128E, max 1.6s for GD25Q128E
    # Take 0.25s * 10 = 2.5s
    Nor64kBlockEraseTimeoutInSeconds = 2.5
    # Max chip erase time: typical 150s for W25Q256JV, max 400s for W25Q256JV
    # Take 1000s > 400s * 2 = 800s
    NorChipEraseTimeoutInSeconds = 1000
    # Read flash with 1IO @ 10MHz(10Mbps)
    NorRead1KiBTimeoutInSecond = 0.001
    # NOR calculate checksum with 1.5MB / s, test data(15.8MB / 8s)
    NorCalculate1KiBChecksumTimeoutInSeconds = 0.001

    # NAND Flash 4KB page size read / program / block erase timeout
    # Page size read from Array with ECC(900us) + page size(4KB) read from cache with 1IO @ 10MHz(4KB / 10Mbps=3.2ms)
    # Max read from array with ECC: 90us for MT29F4G01ABBF, max 170us for MT29F4G01ABBF
    # Take 90us * 10 = 900us + 3.2ms
    NandPageReadTimeoutInSeconds = 0.004
    # page program with ECC(4ms) + page size(4KB) load with 1IO @ 10MHz(4KB / 10Mbps=3.2ms)
    # Max page program with ECC: 400us for MX35LF4GE4AD, max 800us for MX35LF4GE4AD
    # Take 400us * 10 = 4ms + 3.2ms
    NandPageProgramTimeoutInSeconds = 0.007
    # Max block erase time: typical 4ms for MX35LFxG24AD, max 10ms for MT29F2G01ABAG
    # Take 4ms * 10 = 40ms
    NandBlockEraseTimeoutInSeconds = 0.04
    # NAND calculate checksum with 1.5MB / s, test data(2KB / 4KB:47MB / 23s)
    NandCalculate1KiBChecksumTimeoutInSeconds = 0.001


def nor_read_timeout_in_second(size_in_byte):
    return int(max(divide_then_round_up(size_in_byte, 1024) * FlashUtils.NorRead1KiBTimeoutInSecond.value,
                   FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nor_erase_timeout_in_second(size_in_kbyte):
    if size_in_kbyte == 0xFFFFFFFF:
        return int(max(FlashUtils.NorChipEraseTimeoutInSeconds.value,
                       FlashUtils.MinFlashProcessTimeoutInSecond.value))
    elif size_in_kbyte == 4:
        return int(max(FlashUtils.Nor4kSectorEraseTimeoutInSenonds.value,
                       FlashUtils.MinFlashProcessTimeoutInSecond.value))
    elif size_in_kbyte == 32:
        return int(max(FlashUtils.Nor32kBlockEraseTimeoutInSeconds.value,
                       FlashUtils.MinFlashProcessTimeoutInSecond.value))
    elif size_in_kbyte == 64:
        return int(max(FlashUtils.Nor64kBlockEraseTimeoutInSeconds.value,
                       FlashUtils.MinFlashProcessTimeoutInSecond.value))
    else:
        return int(
            max(divide_then_round_up(size_in_kbyte, 4) * FlashUtils.Nor4kSectorEraseTimeoutInSenonds.value,
                FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nor_checksum_timeout_in_second(size_in_byte):
    return int(max(divide_then_round_up(size_in_byte,
                                        1024) * FlashUtils.NorCalculate1KiBChecksumTimeoutInSeconds.value,
                   FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nand_read_timeout_in_second(size_in_byte, page_size):
    return int(max(
        divide_then_round_up(size_in_byte, page_size) * FlashUtils.NandPageReadTimeoutInSeconds.value,
        FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nand_program_timeout_in_second(size_in_byte, page_size):
    return int(max(
        divide_then_round_up(size_in_byte, page_size) * FlashUtils.NandPageProgramTimeoutInSeconds.value,
        FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nand_erase_timeout_in_second(size_in_byte, block_size):
    return int(max(
        divide_then_round_up(size_in_byte, block_size) * FlashUtils.NandBlockEraseTimeoutInSeconds.value,
        FlashUtils.MinFlashProcessTimeoutInSecond.value))


def nand_checksum_timeout_in_second(size_in_byte, page_size):
    return int(max(
        divide_then_round_up(size_in_byte, 1024) * FlashUtils.NandCalculate1KiBChecksumTimeoutInSeconds.value,
        FlashUtils.MinFlashProcessTimeoutInSecond.value))
