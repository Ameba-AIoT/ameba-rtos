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

CONFIG_SCRIPTS_DIR = 'menuconfig'


def run_command(script, argv=None):
    script_path = os.path.join(CONFIG_SCRIPTS_DIR, script)
    if argv != None:
        result = subprocess.run('python ' + script_path + ' ' + argv, shell=True)
    else:
        result = subprocess.run('python ' + script_path, shell=True)
    if result.returncode != 0:
        print("error:", result.stderr) 
        sys.exit(1)



def main(argc, argv):
    os.environ['KCONFIG_CONFIG']="menuconfig/.config"

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
        run_command('genconfig.py', '--header-path menuconfig/autoconf.h')
        run_command('splitconfig.py')
    else:
        for file in args.file:
            if file.startswith('rls'):
                file_path = os.path.join('menuconfig/confs_release_tool', file)
            else:
                file_path = os.path.join('menuconfig/confs_daily_build', file)
            if not os.path.isfile(file_path):
                raise TypeError(file_path + " does not exist")
            else:
                file_content = open(file_path).read()
            content = content + file_content.replace('\n', ' ') + ' '
    
        run_command('setconfig.py', '--config-out menuconfig/.setconfig ' + content)
        run_command('genconfig.py', '--config-in menuconfig/.setconfig --header-path menuconfig/autoconf.h')
        run_command('splitconfig.py', '--config-in menuconfig/.setconfig --header-in menuconfig/autoconf.h')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
