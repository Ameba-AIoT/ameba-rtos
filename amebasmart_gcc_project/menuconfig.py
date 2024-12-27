#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0
"""
Simple utility for setting configuration values from the xxx.conf.

xxx.conf is loacated at menuconfig/confs_daily_build and menuconfig/confs_release_tool

Usage:

  $ ./menuconfig.py [-f FILE1 FILE2 ...]

Sample usage:

  $ ./menuconfig.py
  $ ./menuconfig.py -f usb.conf
  $ ./menuconfig.py -f bt1.conf audio_passthrough.conf

Note: In xxx.conf files, symbol names should not be prefixed with 'CONFIG_'.

There are two modes for menuconfig: cmd-ui mode and file-input mode.

[CMD-UI]
The initial configuration file is '.config', which is a default config file
The output configuration file is '.config'.
Each set-config action will change the default config file (i.e., '.config')
When overwriting a configuration file, the old version is saved to .config.old

[FILE-INPUT]
The initial configuration file is '.config', which is a default config file
The input configuration file is 'xxx.conf', which is a list of extra configurations
The output configuration file is '.setconfig'.
Each set-config action will not change the default config file (i.e., '.config')
When overwriting a configuration file, the old version is saved to .setconfig.old
"""

import os
import sys
import argparse
from menuconfig.manager import Manager

def main():
    os.environ['KCONFIG_CONFIG']="menuconfig/.config"

    parser = argparse.ArgumentParser(description=None)
    parser.add_argument(
        "-f",
        "--file",
        nargs='+',
        help="extra config file"
    )
    args = parser.parse_args()
    manager = Manager(
        {'lp': 'KM0', 'hp': 'KM4', 'ap': 'CA32'},
        section_begin_marker_suffix = ' Begin', #full mark fmt: ${prefix}${core_name}${suffix}
        section_end_marker_suffix = ' End'
    )
    if args.file:
        code = manager.apply_files_config(args.file)
    else:
        code = manager.apply_manual_config()

    if code != 0:
        print("apply config failed")
        sys.exit(1)

    if manager.check_invalid_config():
        sys.exit(1)
    manager.parse_general_config()
    manager.process_projects()

if __name__ == '__main__':
    main()

