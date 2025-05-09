#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


import argparse
import json
import os
import subprocess
import sys

import base.rtk_utils


LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_WINDOWS = 'C:\\msys64\\opt\\rtk-toolchain'
LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_LINUX = '/opt/rtk-toolchain'
LOONG_SDK_DEFAULT_BUILD_DIR = 'build'

def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', help='application path')
    parser.add_argument('-b', '--build-dir', help='build directory')
    parser.add_argument('-c', '--clean', action='store_true', help='clean the build')
    parser.add_argument('-d', '--device', help='device name')
    parser.add_argument('-i', '--image-dir', help='image directory')
    parser.add_argument('-t', '--toolchain-dir', help='toolchain directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')

    args = parser.parse_args()

    sdkroot = os.getcwd()

    device = None
    if args.device == None:
        print('Error: No device specified')
        sys.exit(1)
    else:
        device = args.device.lower()

    app = None
    if args.app != None:
        app = base.rtk_utils.get_abs_path(args.app)
        if os.path.exists(app):
            pass
        else:
            print("Error: App directory '" + app + "' does not exist")
            sys.exit(1)
    else:
        pass

    if app == None and not args.clean:
        print('Note: No application specified, build default project')

    if args.toolchain_dir == None:
        if os.name == 'nt':
            toolchain_dir = LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_WINDOWS
        else:
            toolchain_dir = LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_LINUX
        print('No toolchain specified, use default toolchain: ' + toolchain_dir)
    else:
        toolchain_dir = base.rtk_utils.get_abs_path(args.toolchain_dir)

    if os.path.exists(toolchain_dir):
        pass
    else:
        print("Error: Toolchain '" + toolchain_dir + "' does not exist")
        sys.exit(1)

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

    if args.build_dir == None:
        build_dir = os.path.join(device_dir, LOONG_SDK_DEFAULT_BUILD_DIR)
    else:
        build_dir = base.rtk_utils.get_abs_path(args.build_dir)

    if args.image_dir == None:
        image_dir = device_dir
    else:
        image_dir = base.rtk_utils.get_abs_path(args.image_dir)

    if not os.path.exists(image_dir):
        try:
            os.makedirs(image_dir)
        except:
            print('Error: Fail to create image directory "' + image_dir + '"')
            sys.exit(1)
    else:
        pass

    toolchain = cfg['chips'][chip]['toolchain']

    toolchain_path = None
    if os.name == 'nt':
        toolchain_path = os.path.join(toolchain_dir, toolchain, 'mingw32', 'newlib')
    else:
        toolchain_path = os.path.join(toolchain_dir, toolchain, 'linux', 'newlib')

    if os.path.exists(toolchain_path):
        pass
    else:
        print('Error: Toolchain "' + toolchain_path + '" does not exist')
        sys.exit(1)

    target_script = os.path.join(device_dir, "build.py")
    arguments = [
        '--app', args.app,
        '--build-dir', args.build_dir,
        '--Defined', f"FINAL_IMAGE_DIR={image_dir}", f"TOOLCHAIN_DIR={toolchain_dir}"
    ]
    if args.clean:
        arguments.append('--clean')
    if args.pristine:
        arguments.append('--pristine')
    try:
        result = subprocess.run(
            ["python", target_script] + arguments,
            stdout=None,
            stderr=None,
            text=True,
            check=True
        )
        print('Build done')
    except subprocess.CalledProcessError as e:
        print("Error:", e.stderr)
        print('Error: Fail to build application')
        sys.exit(1)


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
