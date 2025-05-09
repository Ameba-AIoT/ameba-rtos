#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 


import argparse
import json
import os
import pathlib
import subprocess
import sys

import base.rtk_utils


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-b', '--build-dir', help='build directory')
    parser.add_argument('-d', '--device', help='device name')
    parser.add_argument('-g', '--gui', action='store_true', help='GUI config')
    parser.add_argument('-f', '--file', help='config file(s)', nargs='+')
    parser.add_argument('-c', '--clean', action='store_true', help='clean menuconfig')

    args = parser.parse_args()
    sdkroot = os.getcwd()

    os.environ['KCONFIG_CONFIG']="menuconfig/.config"

    device = None
    if args.device == None:
        print('Error: No device specified')
        sys.exit(1)
    else:
        device = args.device.lower()

    cfg = None
    if os.path.exists(base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE):
        try:
            with open(base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE, 'r') as f:
                cfg = json.load(f)
        except:
            print('Error: Fail to load query configuration file "' + base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE + '"')
            sys.exit(2)
    else:
        print('Error: Query configuration file "' + base.rtk_utils.LOONG_SDK_QUERY_CFG_FILE + '" does not exist')
        sys.exit(1)

    chip = None
    device_dir = None
    if device not in cfg['devices'].keys():
        print('Error: Unsupported device "' + device + '", valid values: ')
        [print(key) for key in cfg['devices'].keys()]
        sys.exit(1)
    else:
        chip = cfg['devices'][device]['chip']
        device_dir = os.path.join(sdkroot, os.path.normcase(cfg['devices'][device]['path']))

    menuconfig_dir = str(pathlib.Path(args.build_dir).parent)
    result = subprocess.run(
        ['python', 'menuconfig.py'] + (args.file if args.file else []) + (["-c", "-d", menuconfig_dir] if args.clean else []),
        cwd=device_dir,
        text=True
    )

    if result.returncode != 0:
        print('Error:', result.stderr.strip())
        sys.exit(1)


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
