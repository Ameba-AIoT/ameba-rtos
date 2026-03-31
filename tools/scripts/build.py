#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import shutil
import sys
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
copy_script_dir = os.path.join(script_dir, 'build_copy.py')


def run_command(cmd, shell=True):
    """Helper function: run shell command and print log."""
    print(f"\033[36mRunning: {cmd}\033[0m")
    try:
        # Use subprocess.check_call. It raises an exception on failure,
        # which is safer than os.system and captures errors better.
        subprocess.check_call(cmd, shell=shell)
        return 0
    except subprocess.CalledProcessError:
        print(f'\033[31mError: Command failed: {cmd}\033[0m')
        return 1
    except KeyboardInterrupt:
        print('\n\033[31mKeyboardInterrupt: build interrupted by user.\033[0m')
        return 1


def main(argc, argv):
    parser = argparse.ArgumentParser(description='Realtek Build Script')
    parser.add_argument('-proj', '--project-dir', required=True, help='project directory')
    parser.add_argument('-a', '--app', help='application path')
    parser.add_argument('-c', '--clean', action='store_true', help='clean')
    parser.add_argument('-d', '--build-dir', help='build directory')
    parser.add_argument('-p', '--pristine', action='store_true', help='pristine build (remove build dir)')
    parser.add_argument('-g', '--target',
                         help='custom target',
                         choices=['imgtool_flashloader', 'gen_imgtool_floader', 'gen_submodule_info']
                        )
    parser.add_argument('-G', '--generator', default='Ninja', help='CMake generator (e.g., Ninja, "Unix Makefiles")')
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

    DEFAULT_BUILD_DIR = os.path.join(project_dir, 'build')
    build_dir = args.build_dir if args.build_dir else DEFAULT_BUILD_DIR
    menuconfig_dir = os.path.join(os.path.dirname(build_dir), 'menuconfig')

    # --- 1. Clean ---
    if args.clean:
        if os.path.exists(build_dir):
            # Use 'cmake --build --target clean' for generic clean command
            cmd = f'cmake --build "{build_dir}" --target clean'
            sys.exit(run_command(cmd))
        return

    # --- 2. Pristine Build (Deep Clean) ---
    if os.path.exists(menuconfig_dir) and args.pristine:
        shutil.rmtree(menuconfig_dir)
        print(f"Removed {menuconfig_dir}")

    if os.path.exists(build_dir):
        if args.pristine:
            shutil.rmtree(build_dir)
            os.makedirs(build_dir)
            print(f"Re-created {build_dir}")
        else:
            pass
    else:
        os.makedirs(build_dir)

    # --- 3. Base Configuration Command (CMake Configure) ---
    # Using -S (source dir) and -B (build dir) avoids directory switching (cd)
    cmake_config_cmd = f'cmake -S "{project_dir}" -B "{build_dir}" -G "{args.generator}"'

    if args.daily_build:
        cmake_config_cmd += f' -DDAILY_BUILD={args.daily_build}'

    if args.Defined:
        for defs in args.Defined:
            cmake_config_cmd += f' -D{defs}'

    # --- 4. GDB / Debug ---
    gdb_script_dir = os.path.join(script_dir, 'gdb.py')
    if args.gdb or args.debug:
        if not os.path.exists(os.path.join(build_dir, 'CMakeCache.txt')):
            if run_command(cmake_config_cmd) != 0:
                sys.exit(1)

        mode = 'debug' if args.debug else ''
        os.system(f'python {gdb_script_dir} {project_dir} {mode}')
        return

    # --- 5. App Logic (Submodules & Re-configure) ---
    if args.app:
        if not os.path.exists(os.path.join(menuconfig_dir, '.config')):
            args.pristine = True   # prevent prj.conf not working due to pre_build .config

        # Step A: Run basic configure first
        if run_command(cmake_config_cmd) != 0:
            sys.exit(1)

        # Step B: Execute gen_submodule_info target
        # Use cmake --build to invoke it
        pre_build_cmd = f'cmake --build "{build_dir}" --target gen_submodule_info'
        if run_command(pre_build_cmd) != 0:
            sys.exit(1)

        if args.pristine and os.path.exists(menuconfig_dir):
            shutil.rmtree(menuconfig_dir)

        # Step C: Re-configure with APP definition
        cmake_config_cmd += f' -DEXAMPLE={args.app}'
        if run_command(cmake_config_cmd) != 0:
            sys.exit(1)
    else:
        print('Note: No application specified, choose default project')
        # If no app specified, run standard configure once
        if run_command(cmake_config_cmd) != 0:
            sys.exit(1)

    # --- 6. Handle --new command ---
    if args.new:
        cmd_new = f'python {copy_script_dir} {" ".join(args.new)}'
        if args.app:
            cmd_new += f' --app {args.app}'
        sys.exit(run_command(cmd_new))

    # --- 7. Main Build Command ---
    # Key: Use 'cmake --build' and '--parallel'
    # This enables automatic parallel compilation regardless of the underlying generator (Ninja/Make)
    build_cmd = f'cmake --build "{build_dir}" --parallel'

    if args.target:
        build_cmd += f' --target {args.target}'

    # --- 8. Remove Old code_size.json Before Build ---
    if args.build_dir:
        old_file = os.path.join(os.path.dirname(args.build_dir), 'code_size.json')
        if os.path.exists(old_file):
            os.remove(old_file)

    # Execute Final Build
    if run_command(build_cmd) != 0:
        print('\033[31mError: Fail to build application\033[0m')
        # Return code will be truncated, e.g.: 256 => 0, so the raw return code will not be used
        sys.exit(1)

    # --- Summary Code Size Result ---
    if args.build_dir:
        summary_script = os.path.join(os.path.dirname(args.build_dir), '../utils/code_analyze/code_analyze_summary.py')
        if os.path.exists(summary_script):
            code_size_json = os.path.join(os.path.dirname(args.build_dir), 'code_size.json')
            run_summary_cmd = f'python "{summary_script}" "{code_size_json}"'
            if run_command(run_summary_cmd) != 0:
                print('\033[31mError: Fail to summary code size\033[0m')

    print('\033[32mBuild done\033[0m')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
