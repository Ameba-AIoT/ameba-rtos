#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import enum

_DEV_ERR_BASE = 0X00E0
_SYS_ERR_BASE = 0x0100


class ErrType(enum.Enum):
    OK = 0X00

    DEV_ERR_BASE = _DEV_ERR_BASE

    DEV_BUSY = _DEV_ERR_BASE + 0X1  # Busy
    DEV_TIMEOUT = _DEV_ERR_BASE + 0X2  # Operation timeout
    DEV_FULL = _DEV_ERR_BASE + 0X4  # Operation timeout
    DEV_INVALID = _DEV_ERR_BASE + 0x5  # device invalid
    DEV_LENGTH = _DEV_ERR_BASE + 0x6  # device length
    DEV_CHECKSUM = _DEV_ERR_BASE + 0x7  # device checksum
    DEV_ADDRESS = _DEV_ERR_BASE + 0x8  # device address
    DEV_NAND_BAD_BLOCK = _DEV_ERR_BASE + 0x9  # device nand bad block
    DEV_NAND_WORN_BLOCK = _DEV_ERR_BASE + 0xA   # device nand worn block
    DEV_NAND_BIT_FLIP_WARNING = _DEV_ERR_BASE + 0xB  # device nand bit flip warning
    DEV_NAND_BIT_FLIP_ERROR = _DEV_ERR_BASE + 0xC  # device nand bit flip error
    DEV_NAND_BIT_FLIP_FATAL = _DEV_ERR_BASE + 0xD  # device nand bit flip fatal

    SYS_ERR_BASE = _SYS_ERR_BASE
    SYS_TIMEOUT = _SYS_ERR_BASE + 0x02  # Operation timeout
    SYS_PARAMETER = _SYS_ERR_BASE + 0X3  # Invalid parameter
    SYS_IO = _SYS_ERR_BASE + 0x05  # IO error
    SYS_NAK = _SYS_ERR_BASE + 0x15  # Device NAK
    SYS_PROTO = _SYS_ERR_BASE + 0x22  # Protocol error
    SYS_CHECKSUM = _SYS_ERR_BASE + 0x23  # checksum error
    SYS_OVERRANGE = _SYS_ERR_BASE + 0x24  # operation overrange
    SYS_CANCEL = _SYS_ERR_BASE + 0x30  # Operation cancelled
    SYS_UNKNOWN = _SYS_ERR_BASE + 0xEE  # Unknown error

