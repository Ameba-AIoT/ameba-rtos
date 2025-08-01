#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
import os
import sys
import importlib.util

missing_modules = []
for module in ['json5']:
    if importlib.util.find_spec(module) is None:
        missing_modules.append(module)
if missing_modules:
    error_message = (
        "\n\033[1;31m ERROR \033[0m\n"
        f"\033[1;31m➜ Miss module: {', '.join(missing_modules)}\033[0m\n"
        f"\033[1;31m➜ Install by: pip install -r {os.path.dirname(os.path.dirname(__file__))}/requirements.txt\033[0m\n"
        f"\033[1;31m{'-'*50}\033[0m"
    )
    print(error_message, file=sys.stderr)
    sys.exit(1)

import argparse
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'image_process'))
from image_process.utility import default_logger
from image_process.context import Context

from image_process.op_compress import Compress as op_compress
from image_process.op_encrypt import Encrypt as op_encrypt
from image_process.op_firmware_package import FirmwarePackage as op_fw_pack
from image_process.op_ota_prepend_header import OtaPrependHeader as op_ota_prepend_header
from image_process.op_pad import Pad as op_pad
from image_process.op_pad_binary import PadBinary as op_pad_binary
from image_process.op_prepend_header import PrependHeader as op_prepend_header
from image_process.op_rdp import Rdp as op_rdp
from image_process.op_rsip import Rsip as op_rsip
from image_process.op_cut import Cut as op_cut

def parse_args():
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('--mp', choices=['y', 'n'], default='n', help='Mass production')
    parser.add_argument('--post-build-dir', help='Post Build dir')
    parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING'], default='WARNING', help='Logging level')
    parser.add_argument('--extern-dir', help='External Project Directory')

    subparsers = parser.add_subparsers(dest='operation', help='Available operations', required=True)

    for name, obj in globals().items():
        if not name.startswith('op_'): continue
        sub = subparsers.add_parser(
            name = obj.cmd_promote,
            help = obj.cmd_help_msg
        )
        obj.register_args(sub)

    args = parser.parse_args()
    return args

def main():
    args = parse_args()
    obj = globals()[f'op_{args.operation}']

    try:
        context = Context(args, obj)
    except Exception as e:
        default_logger.error(f"context init failed: {e}")
        sys.exit(1)

    operator = obj(context)
    res = operator.execute_all()
    if res:
        context.logger.fatal(f'operation execute failed: {args.operation}, {res}')
        sys.exit(1)
    else:
        sys.exit(0)

if __name__ == '__main__':
    main()
