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

copy_script_dir = os.path.join(project_dir, '../tools/scripts/copy.py')


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', help='example name for core act as ap')
    parser.add_argument('--app-for-ca32', help='example name for core ca32')
    parser.add_argument('--app-for-km4', help='example name for core km4')
    # parser.add_argument('--app-for-km0', help='example name for core km0')
    parser.add_argument('-c', '--clean', action='store_true', help='clean')
    parser.add_argument('-d', '--build-dir', help='build directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-g', '--target',
                         help='custom target',
                         choices=['flashloader', 'imgtool_flashloader', 'gen_imgtool_floader']
                        )
    parser.add_argument('--daily-build', help='daily build flag')
    parser.add_argument('-gdb', '--gdb', action='store_true', help='gdb')
    parser.add_argument('-debug', '--debug', action='store_true', help='debug')
    parser.add_argument('-D', '--Defined', nargs='+', help='user defined variables')
    parser.add_argument('--new',  nargs='+', help='build.py --new-prj <target_dir> [-a <APP>]')

    args = parser.parse_args()

    app = None
    if args.app == None:
        print('Note: No application specified, choose default project')
    else:
        app = args.app

    app_for_ca32 = os.path.normcase(args.app_for_ca32) if args.app_for_ca32 else None
    app_for_km4 = os.path.normcase(args.app_for_km4) if args.app_for_km4 else None
    # app_for_km0 = os.path.normcase(args.app_for_km0) if args.app_for_km0 else None

    if args.new:
        cmd = 'python ' + copy_script_dir + ' ' + ' '.join(args.new)
        if app:
            cmd += ' --app ' + app
        print(cmd)
        os.system(cmd)
        return

    if args.build_dir == None:
        build_dir = DEFAULT_BUILD_DIR
    else:
        build_dir = args.build_dir


    if args.clean:
        cmd = 'cd ' + build_dir + ' && ninja clean'
        if os.path.exists(build_dir):
            os.system(cmd)
            shutil.rmtree(build_dir)
        return

    if os.path.exists(build_dir):
        if args.pristine:
            cmd = 'cd ' + build_dir + ' && ninja clean'
            try:
                rc = os.system(cmd)
            except:
                rc = 1
            if rc != 0:
                print('Warning: Fail to clean project')
            else:
                pass
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

    cmd = f'cd {build_dir} && cmake {project_dir}'
    if app: cmd += f' -DEXAMPLE={app}'
    if app_for_ca32: cmd += f' -DEXAMPLE_FOR_CA32={app_for_ca32}'
    if app_for_km4: cmd += f' -DEXAMPLE_FOR_KM4={app_for_km4}'


    if args.daily_build != None:
        cmd += ' -DDAILY_BUILD=' + args.daily_build

    if args.Defined !=None:
        for defs in args.Defined:
            cmd += ' -D' + defs

    cmd += ' -G Ninja && ninja'

    if args.target != None:
        cmd += ' ' + args.target

    try:
        rc = os.system(cmd)
    except:
        rc = 1

    if rc != 0:
        print('Error: Fail to build application')
        # Return code will be truncated, e.g.: 256 => 0, so the raw return code will not be used
        sys.exit(1)
    else:
        pass

    print('Build done')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
