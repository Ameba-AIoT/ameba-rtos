#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import sys


with open('info.json', 'r') as jsonfile:
    config = json.load(jsonfile)

project_dir = config.get('Paths', {}).get('gcc_project_dir')

def main():
    if '-d' in sys.argv:
        print('WARNING: -d option is not support in a user project, it will be ignored')

    cmd = 'python ' + os.path.join(project_dir, 'menuconfig.py ') + ' '.join(sys.argv[1:]) + ' -d ' + os.getcwd() + ' -e ' + os.getcwd()
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()

