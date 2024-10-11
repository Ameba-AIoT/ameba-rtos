#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import shutil
import sys

DEFAULT_BUILD_DIR = 'build'


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-a', '--app', help='application path')
    parser.add_argument('-c', '--clean', action='store_true', help='clean')
    parser.add_argument('-d', '--build-dir', help='build directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build')
    parser.add_argument('-g', '--target',
                         help='custom target',
                         choices=['km0', 'km4', 'boot', 'gdb', 'flashloader', 'imgtool_flashloader', 'gen_imgtool_floader']
                        )
    parser.add_argument('--daily-build', help='daily build flag')
    parser.add_argument('-D', '--Defined', nargs='+', help='user defined variables')

    args = parser.parse_args()

    project_dir = os.getcwd()

    app = None
    if args.app == None:
        print('Note: No application specified, build default project')
    else:
        app = os.path.normcase(args.app)

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

    cmd = 'cd ' + build_dir + ' && '
    if app != None:
        cmd += 'cmake "' + project_dir + '" -DEXAMPLE=' + app
    else:
        cmd += 'cmake "' + project_dir + '"'
    
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
