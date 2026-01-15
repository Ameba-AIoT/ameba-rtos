#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import shutil
import sys

script_dir = os.path.dirname(os.path.abspath(__file__))
copy_script_dir = os.path.join(script_dir, 'build_copy.py')


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-proj', '--project-dir', required=True, help='project directory')
    parser.add_argument('-a', '--app', help='application path')
    parser.add_argument('-c', '--clean', action='store_true', help='clean')
    parser.add_argument('-d', '--build-dir', help='build directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-g', '--target',
                         help='custom target',
                         choices=['imgtool_flashloader', 'gen_imgtool_floader', 'gen_submodule_info']
                        )
    parser.add_argument('--daily-build', help='daily build flag')
    parser.add_argument('-gdb', '--gdb', action='store_true', help='gdb')
    parser.add_argument('-debug', '--debug', action='store_true', help='debug')
    parser.add_argument('-D', '--Defined', nargs='+', help='user defined variables')
    parser.add_argument('--new',  nargs=1,
                         help='build.py --new <target_dir> [-a <APP>] (use [-a list-apps] to check available apps)')

    args = parser.parse_args()

    project_dir = os.path.abspath(args.project_dir)
    if not project_dir or not os.path.isdir(project_dir):
        print(f"\033[31mError: '{project_dir}' is not a valid project.\033[0m")
        sys.exit(1)
    if not os.path.exists(os.path.join(project_dir, 'CMakeLists.txt')):
        print(f"\033[31mError: CMakeLists.txt not found in '{project_dir}'\033[0m")
        sys.exit(1)

    gdb_script_dir = os.path.join(project_dir, 'utils/jlink_script/gdb.py')
    DEFAULT_BUILD_DIR = os.path.join(project_dir, 'build')

    if args.build_dir == None:
        build_dir = DEFAULT_BUILD_DIR
    else:
        build_dir = args.build_dir

    menuconfig_dir = os.path.join(os.path.dirname(build_dir), 'menuconfig')

    if args.clean:
        cmd = 'cd ' + build_dir + ' && ninja clean'
        if os.path.exists(build_dir):
            os.system(cmd)
        return

    if os.path.exists(menuconfig_dir):
        if args.pristine:
            shutil.rmtree(menuconfig_dir)

    if os.path.exists(build_dir):
        if args.pristine:
            shutil.rmtree(build_dir)
            os.makedirs(build_dir)
        else:
            pass
    else:
        os.makedirs(build_dir)

    if args.gdb:
        os.system(f'python {gdb_script_dir}')
        return

    if args.debug:
        os.system(f'python {gdb_script_dir} debug')
        return

    cmd = 'cd ' + build_dir + ' && ' + 'cmake "' + project_dir + '"'

    if args.daily_build != None:
        cmd += ' -DDAILY_BUILD=' + args.daily_build

    if args.Defined !=None:
        for defs in args.Defined:
            cmd += ' -D' + defs

    cmd += ' -G Ninja'

    if args.app != None:  # app maybe in submodule directory, get submodule info first
        cmd_pre = cmd + ' && ninja gen_submodule_info'
        os.system(cmd_pre)
        if args.pristine:
            shutil.rmtree(menuconfig_dir)
        cmd += ' -DEXAMPLE=' + args.app
    else:
        print('Note: No application specified, choose default project')

    if args.new:
        cmd_new = 'python ' + copy_script_dir + ' ' + ' '.join(args.new)
        if args.app:
            cmd_new += ' --app ' + args.app
        os.system(cmd_new)
        return

    if args.pristine:
        cmd += ' && ninja clean && ninja'
    else:
        cmd += ' && ninja'

    if args.target != None:
        cmd += ' ' + args.target


    try:
        rc = os.system(cmd)
    except:
        rc = 1

    if rc != 0:
        print('\033[31mError: Fail to build application')
        print('Error CMD : ', cmd, '\033[0m')
        # Return code will be truncated, e.g.: 256 => 0, so the raw return code will not be used
        sys.exit(1)
    else:
        pass

    print('Build done')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
