#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import shutil
import sys

DEFAULT_BUILD_DIR = 'build'

project_dir = os.path.dirname(os.path.abspath(__file__))

gdb_script_dir = os.path.join(project_dir, 'utils/jlink_script/gdb.py')

copy_script_dir = os.path.join(project_dir, '../tools/scripts/build_copy.py')


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', help='example name for core act as ap')
    parser.add_argument('-c', '--clean', action='store_true', help='clean')
    parser.add_argument('-d', '--build-dir', help='build directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-g', '--target',
                         help='custom target',
                         choices=['flashloader', 'imgtool_flashloader', 'gen_imgtool_floader', 'gen_submodule_info']
                        )
    parser.add_argument('--daily-build', help='daily build flag')
    parser.add_argument('-gdb', '--gdb', action='store_true', help='gdb')
    parser.add_argument('-debug', '--debug', action='store_true', help='debug')
    parser.add_argument('-D', '--Defined', nargs='+', help='user defined variables')
    parser.add_argument('--new',  nargs='+',
                         help='build.py --new <target_dir> [-a <APP>] (use [-a list-apps] to check available apps)')

    args = parser.parse_args()

    app = None
    if args.app == None:
        print('Note: No application specified, choose default project')
    else:
        app = args.app

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

    #TODO: For temporary compatibility, remove when wifi fully support new cmake
    if args.Defined:
        if not any(s.startswith("CMAKE_REFACTOR=") for s in args.Defined):
            cmd += f' -D CMAKE_REFACTOR="TRUE"'
    else:
        cmd += f' -D CMAKE_REFACTOR="TRUE"'

    cmd += ' -G Ninja'

    if app != None:  # app maybe in submodule directory, get submodule info first
        cmd_pre = cmd + ' && ninja gen_submodule_info'
        print(cmd_pre)
        os.system(cmd_pre)
        if args.pristine:
            shutil.rmtree(menuconfig_dir)
        cmd += ' -DEXAMPLE=' + app

    if args.new:
        cmd_new = 'python ' + copy_script_dir + ' ' + ' '.join(args.new)
        if app:
            cmd_new += ' --app ' + app
        print(cmd_new)
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
