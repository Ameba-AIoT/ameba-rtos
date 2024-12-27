#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .color_output import print_red


def key_to_hex(key: str) -> str:
    ascii_code = ord(key.upper())
    # convert key from string to C0 escape code
    C0_code = ascii_code - ord('@')
    if C0_code < 1 or C0_code > 32:
        print_red(f"--- Error: Unsupported configuration for key: '{key}', please use just the English alphabet "
                  f"characters (A-Z) and [,],\\,^,_. Using the default option '{key}'.")
        C0_code = ord(key) - ord('@')
    return chr(C0_code)


MENU_KEY = key_to_hex('T')
EXIT_KEY = key_to_hex(']')
TOGGLE_OUTPUT_KEY = key_to_hex('Y')
TOGGLE_LOG_KEY = key_to_hex('L')
TOGGLE_TIMESTAMPS_KEY = key_to_hex('I')
EXIT_MENU_KEY = key_to_hex('X')

COMMAND_KEYS = [
    TOGGLE_OUTPUT_KEY,
    TOGGLE_LOG_KEY,
    TOGGLE_TIMESTAMPS_KEY,
    EXIT_MENU_KEY
]
