#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


import argparse
import venv
import json
import os
import shutil
import subprocess
import sys


LOONG_SDK_VENV_DIR = '.venv'
LOONG_SDK_QUERY_CFG_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'query.json')
LOONG_SDK_DEFAULT_IMAGE_DIR = 'images'
LOONG_SDK_DEFAULT_BUILD_DIR = 'build'
LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_WINDOWS = 'C:\\msys64\\opt\\rtk-toolchain'
LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_LINUX = '/opt/rtk-toolchain'


def run_shell_cmd_with_output(cmd):
    return subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)


def check_venv():
    if os.path.exists(LOONG_SDK_VENV_DIR):
        pass
    else:
        # Create virtual environment if it does not exist
        try:
            venv.create(LOONG_SDK_VENV_DIR)
        except:
            print("Error: Fail to create Python virtual environment")
            sys.exit(2)


def get_abs_path(path):
    abs_path = os.path.normcase(path)
    sdkroot = os.getcwd()
    if not os.path.isabs(abs_path):
        abs_path = os.path.join(sdkroot, abs_path)
    else:
        pass
    return abs_path


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', help='application path')
    parser.add_argument('-b', '--build-dir', help='build directory')
    parser.add_argument('-c', '--clean', action='store_true', help='clean the build')
    parser.add_argument('-d', '--device', help='device name')
    parser.add_argument('-e', '--example', help='example name')
    parser.add_argument('-i', '--image-dir', help='image directory')
    parser.add_argument('-t', '--toolchain-dir', help='toolchain directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-v', '--venv', action='store_true', help='process in Python virtual enviroment')

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
        app = get_abs_path(args.app)
        if os.path.exists(app):
            pass
        else:
            print("Error: App directory '" + app + "' does not exist")
            sys.exit(1)
    else:
        pass

    example = None
    if args.example != None:
        example = os.path.normcase(args.example)
    else:
        pass

    if app == None and example == None and not args.clean:
        print('Note: No application or example specified, build default project')
    elif app != None and example != None:
        print('Error: Invalid arguments, application and example are not allowed to build at the same time')
        sys.exit(1)
    else:
        pass

    if args.toolchain_dir == None:
        if os.name == 'nt':
            toolchain_dir = LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_WINDOWS
        else:
            toolchain_dir = LOONG_SDK_TOOLCHAIN_DEFAULT_PATH_LINUX
        print('No toolchain specified, use default toolchain: ' + toolchain_dir)
    else:
        toolchain_dir = get_abs_path(args.toolchain_dir)

    if os.path.exists(toolchain_dir):
        pass
    else:
        print("Error: Toolchain '" + toolchain_dir + "' does not exist")
        sys.exit(1)

    venv = False
    if args.venv:
        venv = True
        check_venv()
    else:
        pass

    cfg = None
    if os.path.exists(LOONG_SDK_QUERY_CFG_FILE):
        try:
            with open(LOONG_SDK_QUERY_CFG_FILE, 'r') as f:
                cfg = json.load(f)
        except:
            print('Error: Fail to load query configuration file "' + LOONG_SDK_QUERY_CFG_FILE + '"')
            sys.exit(2)
    else:
        print('Error: Query configuration file "' + LOONG_SDK_QUERY_CFG_FILE + '" does not exist')
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
        build_dir = get_abs_path(args.build_dir)

    if args.image_dir == None:
        image_dir = device_dir
    else:
        image_dir = get_abs_path(args.image_dir)

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
    cmd_activate_venv = None
    cmd_deactivate_venv = None
    if os.name == 'nt':
        toolchain_path = os.path.join(toolchain_dir, toolchain, 'mingw32', 'newlib')
        cmd_activate_venv = os.path.join(LOONG_SDK_VENV_DIR, 'Scripts', 'activate.bat')
        cmd_deactivate_venv = os.path.join(LOONG_SDK_VENV_DIR, 'Scripts', 'deactivate.bat')
    else:
        toolchain_path = os.path.join(toolchain_dir, toolchain, 'linux', 'newlib')
        cmd_activate_venv = 'source ' + os.path.join(LOONG_SDK_VENV_DIR, 'bin', 'activate')
        cmd_deactivate_venv = 'deactivate'

    if os.path.exists(toolchain_path):
        pass
    else:
        print('Error: Toolchain "' + toolchain_path + '" does not exist')
        sys.exit(1)

    if os.path.exists(build_dir):
        if args.pristine or args.clean:
            cmd = 'cd ' + build_dir + ' && ninja clean'
            try:
                rc = os.system(cmd)
            except:
                rc = 1
            if rc != 0:
                print('Warning: Fail to clean project')
            else:
                pass
            if args.clean:
                if rc != 0:
                    # Return code will be truncated, e.g.: 256 => 0, so the raw return code will not be used
                    rc = 1
                else:
                    pass
                sys.exit(rc)
            else:
                pass
        else:
            pass
    else:
        if args.clean:
            print('No build need to clean')
            sys.exit(0)
        else:
            pass
        try:
            os.makedirs(build_dir)
        except:
            print('Error: Fail to create build directory "' + build_dir + '"')
            sys.exit(1)

    cmd = ''
    if venv:
        cmd += cmd_activate_venv + ' && '
    else:
        pass
    cmd += 'cd "' + build_dir + '" && '
    if app != None:
        cmd += 'cmake "' + device_dir + '" -DTOOLCHAIN_DIR="' + toolchain_dir + '" -DUSER_SRC_DIR="' + app + '" -DFINAL_IMAGE_DIR="' + image_dir + '" -G Ninja && ninja'
    elif example != None:
        cmd += 'cmake "' + device_dir + '" -DTOOLCHAIN_DIR="' + toolchain_dir + '" -DEXAMPLE="' + example + '" -DFINAL_IMAGE_DIR="' + image_dir + '" -G Ninja && ninja'
    else:
        cmd += 'cmake "' + device_dir + '" -DTOOLCHAIN_DIR="' + toolchain_dir + '" -DFINAL_IMAGE_DIR="' + image_dir + '" -G Ninja && ninja'

    try:
        rc = os.system(cmd)
    except:
        rc = 1

    if venv:
        run_shell_cmd_with_output(cmd_deactivate_venv)
    else:
        pass

    if rc != 0:
        print('Error: Fail to build application')
        # Return code will be truncated, e.g.: 256 => 0, so the raw return code will not be used
        sys.exit(1)
    else:
        pass

    print('Build done')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
