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
    for i in range(len(sys.argv)):
        if sys.argv[i] == "--image":
            sys.argv[i + 1] = os.path.join(os.getcwd(), sys.argv[i + 1])
    cmd = 'python ' + os.path.join(project_dir, 'flash.py ') + ' '.join(sys.argv[1:])
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()
