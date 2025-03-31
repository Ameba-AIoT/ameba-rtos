#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


import os
import sys
import argparse
import shutil
from configparser import ConfigParser


current_script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(f'{current_script_path}/../cmake')
from Kconfig.manager import Manager


def main():

    parser = argparse.ArgumentParser(description=None)
    parser.add_argument(
        "-f",
        "--file",
        nargs='+',
        help="extra config file"
    )

    parser.add_argument(
        "-r",
        "--reset",
        action='store_true',
        help="generate default config file"
    )

    parser.add_argument(
        "-c",
        "--clean",
        action='store_true',
        help="generate default config file"
    )

    parser.add_argument(
        "-d",
        "--menuconfig-dir",
        help=argparse.SUPPRESS # directory where menuconfig floader will be saved
    )

    parser.add_argument(
        "-s",
        "--save",
        help="save config"
    )

    parser.add_argument(
        "--check",
        action='store_true',
        help=argparse.SUPPRESS #check Kconfig update
    )

    parser.add_argument(
        "-e",
        "--external-kconfig",
        help=argparse.SUPPRESS #an external sub-kconfig under this path will be included
    )

    args = parser.parse_args()

    if args.external_kconfig:
        os.environ['RTK_APP_DIR'] = args.external_kconfig

    if args.menuconfig_dir:
        menuconfigdir = args.menuconfig_dir
    else:
        menuconfigdir = ''

    os.environ['KCONFIG_CONFIG']= os.path.join(menuconfigdir, 'menuconfig/.config')

    manager = Manager(
        {'km4': 'KM4', 'kr4': 'KR4'},
        section_begin_marker_prefix = 'MENUCONFIG FOR ', #full mark fmt: ${prefix}${core_name}${suffix}
        section_end_marker_prefix = 'end of MENUCONFIG FOR ',
        out_dir = menuconfigdir,
        top_kconfig = os.path.join(current_script_path, 'Kconfig')
    )

    if args.clean:
        manager.clean_all(current_script_path)
        return

    if args.file:
        code = manager.apply_files_config(args.file)
    elif args.reset:
        code = manager.apply_default_config()
    elif args.save:
        code = manager.save_confs(args.save)
    elif args.check:
        code = manager.apply_check_config()
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

