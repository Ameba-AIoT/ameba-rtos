#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0 


import argparse
import json
import os
import subprocess
import sys


LOONG_CONFIG_SCRIPTS_DIR = 'menuconfig'
LOONG_SDK_QUERY_CFG_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'query.json')


def run_command(workspace, script, argv=None):
    if argv != None:
        result = subprocess.run('cd "' +  workspace + '" && python ' + os.path.join(LOONG_CONFIG_SCRIPTS_DIR, script) + ' ' + argv, shell=True)
    else:
        result = subprocess.run('cd "' +  workspace + '" && python ' + os.path.join(LOONG_CONFIG_SCRIPTS_DIR, script), shell=True)
    if result.returncode != 0:
        print('Error:', result.stderr)
        sys.exit(1)


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
    parser.add_argument('-b', '--build-dir', help='build directory')
    parser.add_argument('-d', '--device', help='device name')
    parser.add_argument('-g', '--gui', action='store_true', help='GUI config')
    parser.add_argument('-f', '--file', help='config file(s)')

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

    setconfig = os.path.join(LOONG_CONFIG_SCRIPTS_DIR, '.setconfig')
    autoconf_h = os.path.join(LOONG_CONFIG_SCRIPTS_DIR, 'autoconf.h')

    if args.file is None:
        if args.gui:
            run_command(device_dir, 'guiconfig.py')
        else:
            run_command(device_dir, 'menuconfig.py')
        run_command(device_dir, 'genconfig.py', '--header-path "' + autoconf_h + '"')
        run_command(device_dir, 'splitconfig.py')
    else:
        configs = ''
        for f in args.file:
            file_abs_path = get_abs_path(f)
            if not os.path.isfile(file_abs_path):
                print('Error: file "' + file_abs_path + '" does not exist')
                sys.exit(1)
            else:
                file_content = open(file_abs_path).read()
                configs += file_content.replace('\n', ' ') + ' '
        run_command(device_dir, 'setconfig.py', '--config-out "' + setconfig + '" '  + content)
        run_command(device_dir, 'genconfig.py', '--config-in  "' + setconfig + '"--header-path "' + autoconf_h + '"')
        run_command(device_dir, 'splitconfig.py', '--config-in "' + setconfig + '" --header-in "' + autoconf_h + '"')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
