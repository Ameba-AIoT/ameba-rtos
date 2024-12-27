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
import subprocess
import sys
import argparse
import re

CONFIG_SCRIPTS_DIR = 'menuconfig'
config_file_km0 = os.path.join(CONFIG_SCRIPTS_DIR, '.config_km0')
config_file_km4 = os.path.join(CONFIG_SCRIPTS_DIR, '.config_km4')
header_file_km0 = os.path.join('project_km0/inc', 'platform_autoconf.h')
header_file_km4 = os.path.join('project_km4/inc', 'platform_autoconf.h')
marker = ['General config', 'KM0 config', 'KM4 config', 'Config end']


def run_command(script, argv=None):
    script_path = os.path.join(CONFIG_SCRIPTS_DIR, script)
    if argv != None:
        result = subprocess.run('python ' + script_path + ' ' + argv, shell=True)
    else:
        result = subprocess.run('python ' + script_path, shell=True)
    if result.returncode != 0:
        print("error:", result.stderr)
        sys.exit(1)

# Helper function to extract lines between markers
def extract_lines(filename, start_marker, end_marker):
    with open(filename, 'r') as file:
        lines = file.readlines()
    start_index = None
    end_index = None
    for i, line in enumerate(lines):
        if start_marker in line:
            start_index = i
        elif end_marker and end_marker in line:
            end_index = i
            break
    if start_index is not None and end_index is not None:
        return lines[start_index:end_index]
    elif start_index is not None and end_index is None:
        return lines[start_index:]
    else:
        return []

def check_invalid_config(config_in):
    lines = extract_lines(config_in, marker[-1], None)
    invalid_pattern = rf'^CONFIG_CORE_AS_(.)P=y$'
    for line in lines:
        stripped_line = line.strip()
        if not stripped_line:
            continue
        match = re.match(invalid_pattern, line)
        if match:
            print("FATAL ERROR! Please do not select the configs in last line which signs <DO NOT ENTER IN THIS BUTTON> ")
            sys.exit(1)

def extract_core_role(lines, suffix):
    pattern = rf'^CONFIG_CORE_AS_(.*)({suffix})=(.*)$'
    configs = ''
    for line in lines:
        stripped_line = line.strip()
        if not stripped_line:
            continue

        match = re.match(pattern, line)
        if match:
            middle_part = match.group(1)
            value_part = match.group(3)
            new_line = f"CORE_AS_{middle_part}={value_part}"
            configs = configs + new_line + ' '

    return configs

def process_and_write_lines(input_file, output_file, remove_pattern, replace_pattern=None):
    processed_lines = []
    with open(input_file, 'r') as file:
        input_lines = file.readlines()
    for line in input_lines:
        if remove_pattern not in line:
            line = re.sub(replace_pattern, '', line)
            processed_lines.append(line)
    with open(output_file, 'w') as file:
        file.writelines(processed_lines)

def main(argc, argv):
    os.environ['KCONFIG_CONFIG']=os.path.join(CONFIG_SCRIPTS_DIR, '.config')

    parser = argparse.ArgumentParser(description=None)
    parser.add_argument(
        "-f",
        "--file",
        nargs='+',
        help="extra config file"
    )
    args = parser.parse_args()
    content = ''
    if args.file is None:
        run_command('menuconfig.py')
        config_in = os.path.join(CONFIG_SCRIPTS_DIR, '.config')
    else:
        for file in args.file:
            if file.startswith('rls'):
                file_path = os.path.join(CONFIG_SCRIPTS_DIR,'confs_release_tool', file)
            else:
                file_path = os.path.join(CONFIG_SCRIPTS_DIR, 'confs_daily_build', file)
            if not os.path.isfile(file_path):
                raise TypeError(file_path + " does not exist")
            else:
                file_content = open(file_path).read()
            content = content + file_content.replace('\n', ' ') + ' '
        run_command('setconfig.py', f'--config-out {CONFIG_SCRIPTS_DIR}/.setconfig ' + content)
        config_in = os.path.join(CONFIG_SCRIPTS_DIR, '.setconfig')

    check_invalid_config(config_in)

    general_lines = extract_lines(config_in, marker[0], marker[1])
    config_km0 = extract_core_role(general_lines, '_FOR_KM0')
    print(config_km0)
    run_command('setconfig.py', f' --config-in {config_in}' + f' --config-out {config_file_km0} ' + config_km0)
    run_command('genconfig.py', f'--config-in {config_file_km0} --header-path {header_file_km0}')

    km0_lines = extract_lines(config_file_km0, marker[0], marker[1]) + extract_lines(config_file_km0, marker[1], marker[2])
    with open(config_file_km0, 'w') as file:
        file.writelines(km0_lines)

    process_and_write_lines(config_file_km0, config_file_km0, '_FOR_KM4', '_FOR_KM0')
    process_and_write_lines(header_file_km0, header_file_km0, '_FOR_KM4', '_FOR_KM0')


    config_km4 = extract_core_role(general_lines, '_FOR_KM4')
    print(config_km4)
    run_command('setconfig.py', f' --config-in {config_in}' + f' --config-out {config_file_km4} ' + config_km4)
    run_command('genconfig.py', f'--config-in {config_file_km4} --header-path {header_file_km4}')

    km4_lines = extract_lines(config_file_km4, marker[0], marker[1]) + extract_lines(config_file_km4, marker[2], marker[3])
    with open(config_file_km4, 'w') as file:
        file.writelines(km4_lines)

    process_and_write_lines(config_file_km4, config_file_km4, '_FOR_KM0', '_FOR_KM4')
    process_and_write_lines(header_file_km4, header_file_km4, '_FOR_KM0', '_FOR_KM4')

if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
