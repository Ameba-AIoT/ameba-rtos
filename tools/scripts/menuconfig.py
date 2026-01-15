#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import argparse
import shutil
from configparser import ConfigParser

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_dir, '../../cmake'))
from Kconfig.manager import Manager

CHIP_CORE_MAPPING = {
    'amebad':      {'km0': 'KM0', 'km4': 'KM4'},
    'amebadplus':  {'km0': 'KM0', 'km4': 'KM4'},
    'amebagreen2': {'km4ns': 'KM4NS', 'km4tz': 'KM4TZ'},
    'amebalite':   {'km4': 'KM4', 'kr4': 'KR4'},
    'amebasmart':  {'lp': 'KM0', 'hp': 'KM4', 'ap': 'CA32'},
    'amebapro3':   {'fp': 'FP', 'np': 'NP', 'vp': 'VP', 'ap': 'AP'},
}

def main():

    parser = argparse.ArgumentParser(description=None)

    parser.add_argument(
        '-proj',
        '--project-dir',
        required=True,
        help='project directory'
    )

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

    project_dir = os.path.abspath(args.project_dir)
    if not project_dir or not os.path.isdir(project_dir):
        print(f"\033[31mError: '{project_dir}' is not a valid project.\033[0m")
        sys.exit(1)
    if not os.path.exists(os.path.join(project_dir, 'Kconfig')):
        print(f"\033[31mError: Kconfig file not found in {project_dir}\033[0m")
        sys.exit(1)

    if args.external_kconfig:
        os.environ['RTK_APP_DIR'] = args.external_kconfig

    if args.menuconfig_dir:
        menuconfigdir = args.menuconfig_dir
    else:
        menuconfigdir = project_dir

    os.environ['KCONFIG_CONFIG']= os.path.join(menuconfigdir, 'menuconfig/.config')


    manager = Manager(
        out_dir = menuconfigdir,
        top_kconfig = os.path.join(project_dir, 'Kconfig')
    )

    if args.clean:
        manager.clean_all(script_dir, project_dir)
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

