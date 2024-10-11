#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess
import sys

CONFIG_SCRIPTS_DIR = 'menuconfig'


def run_command(script, argv=None):
    script_path = os.path.join(CONFIG_SCRIPTS_DIR, script)
    if argv != None:
        subprocess.run('python ' + script_path + ' ' + argv, shell=True)
    else:
        subprocess.run('python ' + script_path, shell=True)


def main(argc, argv):
    os.environ['KCONFIG_CONFIG']="menuconfig/.config"
    run_command('guiconfig.py')
    run_command('genconfig.py', '--header-path menuconfig/autoconf.h')
    run_command('splitconfig.py')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
