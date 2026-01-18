#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import subprocess

project_dir = os.path.dirname(os.path.abspath(__file__))
script = os.path.join(project_dir, '../tools/scripts/menuconfig.py')

def main():
    print(f"\033[33m NOTICE: This script will be removed soon.\033[0m")
    print(f"\033[33m        Please use the new script by following steps:\033[0m")
    print(f"        1. $ cd .. && source env.sh")
    print(f"        2. $ ameba.py menuconfig\n")

    cmd = ['python', script] + sys.argv[1:] + ['-proj', project_dir]
    result = subprocess.run(cmd)
    sys.exit(result.returncode)


if __name__ == '__main__':
    main()