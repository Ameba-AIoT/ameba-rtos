#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from copy import deepcopy

from .download_handler import Ameba
from .errno import ErrType
from .flash_utils import nor_read_timeout_in_second
from .memory_info import MemoryInfo
from .rtk_logging import create_logger


GOLDEN_BOARD_IDS = tuple(range(1, 21))


def query_golden_board(port_name, profile_info, settings, id_address, baudrate):
    """Read a golden-board ID through the normal Ameba floader protocol.

    Golden-board images contain the one-based production slot number in the
    first byte and 0xFF in the rest of their 4-KiB image. Return
    ``(slot_id, error)`` using the same one-based slot number.
    """
    logger = create_logger(port_name, log_level="ERROR")
    ameba = None
    try:
        ameba = Ameba(
            deepcopy(profile_info),
            port_name,
            int(baudrate),
            "",
            deepcopy(settings),
            logger,
            memory_type=profile_info.memory_type,
        )
        ret = ameba.prepare(show_device_info=True)
        if ret != ErrType.OK:
            return None, f"prepare failed: {ret}"

        memory_type = ameba.device_info.memory_type
        if memory_type != MemoryInfo.MEMORY_TYPE_NOR:
            return None, "golden-board layout currently supports NOR flash only"

        read_size = max(int(ameba.device_info.flash_page_size), 1)
        timeout = nor_read_timeout_in_second(read_size)
        ret, data = ameba.floader_handler.read(
            memory_type, int(id_address), read_size, timeout
        )
        if ret != ErrType.OK:
            return None, f"read {hex(int(id_address))} failed: {ret}"
        if not data:
            return None, "empty golden-board ID response"

        board_id = data[0]
        if board_id not in GOLDEN_BOARD_IDS:
            return None, f"unknown golden-board ID 0x{board_id:02X}"
        return board_id, ""
    except SystemExit as err:
        return None, f"failed to open or prepare the serial port (exit {err.code})"
    except Exception as err:
        return None, str(err)
    finally:
        if ameba is not None:
            try:
                ameba.clean_up()
            except Exception:
                pass
