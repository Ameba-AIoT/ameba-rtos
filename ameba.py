#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (c) 2025 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
#
# Ameba SDK CLI development tool

import sys
import os

current_script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(f'{current_script_path}/tools/scripts')
from ameba_manager import AmebaManager

def main():

    manager = AmebaManager()
    manager.cmd_preprocess()

    args = sys.argv[1:]

    if not args:
        command = "build"
    else:
        command = args[0].lower()
        args = args[1:] if len(args) > 1 else []

    success = False

    handler = manager.get_command_handler(command)
    if handler:
        success = handler(args)

    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()