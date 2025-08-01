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
        print('WARNING: -d option is not supported in a user project, it will be ignored')

    if '--new' in sys.argv:
        print('ERROR: --new option is not supported in a user project, you are already in a new project')
        return


    cmd = 'python ' + os.path.join(project_dir, 'build.py ') + ' '.join(sys.argv[1:]) + ' -a ' + os.getcwd() + ' -d ' + os.path.join(os.getcwd(), 'build') + ' -D EXTERN_DIR:STRING=' + os.getcwd()
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()
