#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import subprocess

project_dir = os.path.dirname(os.path.abspath(__file__))
script = os.path.join(project_dir, '../tools/scripts/flash.py')

def main():
    print("\033[33m NOTICE: This script will be removed soon.\033[0m")
    print("\033[33m        Please use the new script by following steps:\033[0m")
    print("        1. $ cd .. && source env.sh")
    print("        2. $ ameba.py flash\n")

    args = sys.argv[1:]
    if not "-dev" in args and not "--device" in args:
        print("\033[33mWarning: -dev/--device argument is required but not given. Use default: RTL8721Dx\033[0m")
        args += ['-dev', 'RTL8721Dx']

    cmd = ['python', script] + args
    result = subprocess.run(cmd)
    sys.exit(result.returncode)


if __name__ == '__main__':
    main()