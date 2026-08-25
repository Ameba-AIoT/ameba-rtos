#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import shutil
import sys
import subprocess
import time

script_dir = os.path.dirname(os.path.abspath(__file__))
copy_script_dir = os.path.join(script_dir, 'build_copy.py')

sys.path.append(script_dir)
from ameba_output import (
    BuildFilter,
    OutputMode,
    get_output_mode,
    is_quiet,
    print_quiet_summary,
    run_filtered,
)

# A single filter instance shared across the whole build.py invocation so
# quiet-mode summaries see every warning/error across configure + build steps.
_build_filter = BuildFilter(mode=get_output_mode())


def _read_kconfig(config_file):
    """Parse a .config file into a dict. 'y' values -> True, quoted strings kept
    unquoted, '# CONFIG_X is not set' -> absent."""
    cfg = {}
    try:
        with open(config_file, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#') or '=' not in line:
                    continue
                key, _, val = line.partition('=')
                val = val.strip()
                if len(val) >= 2 and val[0] == '"' and val[-1] == '"':
                    val = val[1:-1]
                cfg[key.strip()] = val
    except OSError:
        return None
    return cfg


def _resolve_dsp_sdk_root(cfg, project_dir):
    """Derive the DSP SDK root from CONFIG_DSP_SDK_IMAGE_DIR (the SDK image dir,
    e.g. <sdk>/project/image); the root is that dir two levels up. Absolute path
    used as-is; relative resolved against the SoC project dir (same rule as
    postbuild.cmake / dsp_layout_sync.py). Returns (sdk_root, image_dir), or
    (None, None) when CONFIG_DSP_SDK_IMAGE_DIR is unset."""
    image_dir = (cfg.get('CONFIG_DSP_SDK_IMAGE_DIR') or '').strip()
    if not image_dir:
        return None, None
    image_dir = os.path.expanduser(image_dir)
    if not os.path.isabs(image_dir):
        image_dir = os.path.join(project_dir, image_dir)
    image_dir = os.path.abspath(image_dir)
    sdk_root = os.path.dirname(os.path.dirname(image_dir))  # project/image -> root
    return sdk_root, image_dir


def build_dsp_from_source(build_dir, project_dir):
    """When CONFIG_DSP_BUILD_FROM_SOURCE=y, rebuild the DSP image from the DSP SDK
    (project/auto_build/auto_build.sh) so it stays in sync with the MCU layout.
    Runs before the MCU packaging step; returns 0 on success/skip, 1 on error."""
    config_file = os.path.join(build_dir, 'project_km4', '.config_km4')
    cfg = _read_kconfig(config_file)
    if cfg is None:
        # No km4 config yet (e.g. a target that does not run DSP): nothing to do.
        return 0
    if cfg.get('CONFIG_DSP_EN') != 'y' or cfg.get('CONFIG_DSP_BUILD_FROM_SOURCE') != 'y':
        return 0

    sdk_root, _ = _resolve_dsp_sdk_root(cfg, project_dir)
    if not sdk_root:
        print('\033[31mError: CONFIG_DSP_BUILD_FROM_SOURCE=y but CONFIG_DSP_SDK_IMAGE_DIR '
              'is not set.\033[0m')
        print('       Set it to the DSP SDK image output dir (holds dsp.bin / '
              'dsp_all.bin); the SDK root is derived from it, e.g.:')
        print('           CONFIG_DSP_SDK_IMAGE_DIR="/path/to/dsp_sdk/heap/source/project/image"')
        return 1
    auto_build_dir = os.path.join(sdk_root, 'project', 'auto_build')
    is_windows = (os.name == 'nt')
    script_name = 'auto_build.bat' if is_windows else 'auto_build.sh'
    script_path = os.path.join(auto_build_dir, script_name)
    if not os.path.isfile(script_path):
        print(f'\033[31mError: DSP SDK root derived from CONFIG_DSP_SDK_IMAGE_DIR is '
              f'invalid: {script_path} not found.\033[0m')
        print(f'       Derived SDK root: {sdk_root}')
        print('       Expected <sdk_root>/project/auto_build/'
              f'{script_name}. CONFIG_DSP_SDK_IMAGE_DIR must point at '
              '<sdk_root>/project/image.')
        return 1

    # auto_build.sh regenerates the bins in the SDK image dir (= CONFIG_DSP_SDK_IMAGE_DIR),
    # which flashing and within-app packaging read directly -- no extra copy (DSP_BIN_OUT).
    env = dict(os.environ)
    if not is_quiet():
        print(f'\033[36mBuilding DSP from source: {script_path}\033[0m')
    cmd = [script_path] if is_windows else ['sh', script_name]
    try:
        subprocess.check_call(cmd, cwd=auto_build_dir, env=env)
    except subprocess.CalledProcessError:
        print('\033[31mError: DSP build from source failed.\033[0m')
        return 1
    except KeyboardInterrupt:
        print('\n\033[31mKeyboardInterrupt: DSP build interrupted by user.\033[0m')
        return 1
    return 0


def run_command(cmd, shell=True):
    """Helper function: run shell command and print log."""
    if is_quiet():
        return run_filtered(cmd, _build_filter, shell=shell)

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
                         choices=['imgtool_flashloader', 'gen_imgtool_floader', 'gen_submodule_info', 'boot']
                        )
    parser.add_argument('-k', '--core', help='build the specified core')
    parser.add_argument('-G', '--generator', default='Ninja', help='CMake generator (e.g., Ninja, "Unix Makefiles")')
    parser.add_argument('--daily-build', help='daily build flag')
    parser.add_argument('-gdb', '--gdb', action='store_true', help='gdb')
    parser.add_argument('-debug', '--debug', action='store_true', help='debug')
    parser.add_argument('-D', '--Defined', nargs='+', help='user defined variables')
    parser.add_argument('--new',  nargs=1,
                         help='build.py --new <target_dir> [-a <APP>] (use [-a list-apps] to check available apps)')
    parser.add_argument('-q', '--quiet', action='store_true',
                        help='suppress verbose output; show only [INFOR] lines, errors, and final status')
    parser.add_argument('--remote-server', type=str, help=argparse.SUPPRESS)
    parser.add_argument('--image-dir', type=str, default='', help=argparse.SUPPRESS)

    args = parser.parse_args()

    # In quiet mode, skip the postbuild "Image analyze" step (code-size /
    # static analysis). This env var propagates down the whole process chain
    # (cmake --build -> ninja -> cmake -P postbuild.cmake), where each
    # postbuild.cmake gates the analyze block on `DEFINED ENV{...}`.
    if args.quiet:
        os.environ['AMEBA_SKIP_IMAGE_ANALYZE'] = '1'

    # Track start time and register quiet-mode summary (runs on any exit path).
    _build_start = time.monotonic()
    _build_state = {'success': False, 'final_image_dir': None, 'build_dir': None}

    def _emit_quiet_summary():
        if not is_quiet():
            return
        print_quiet_summary(
            filt=_build_filter,
            final_image_dir=_build_state['final_image_dir'],
            build_dir=_build_state['build_dir'],
            duration_s=time.monotonic() - _build_start,
            success=_build_state['success'],
        )

    import atexit
    atexit.register(_emit_quiet_summary)

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
        if not is_quiet():
            print(f"Removed {menuconfig_dir}")

    if os.path.exists(build_dir):
        if args.pristine:
            shutil.rmtree(build_dir)
            os.makedirs(build_dir)
            if not is_quiet():
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

    skip_current = False
    fetch_retry = 0
    if args.Defined:
        for defs in args.Defined:
            if defs.startswith("SKIP_CURRENT"):
                skip_current = True
                continue
            if defs.startswith("CODE_ANALYZE_RETRY"):
                fetch_retry = int(defs.split('=')[1])
                # continue  # TODO
            if defs.startswith('FINAL_IMAGE_DIR='):
                _build_state['final_image_dir'] = defs.split('=', 1)[1]
            cmake_config_cmd += f' -D{defs}'
    _build_state['build_dir'] = build_dir

    # --- 4. GDB / Debug ---
    gdb_script_dir = os.path.join(script_dir, 'gdb.py')
    if args.gdb or args.debug:
        if not os.path.exists(os.path.join(build_dir, 'CMakeCache.txt')):
            if run_command(cmake_config_cmd) != 0:
                sys.exit(1)

        cmd = [sys.executable, gdb_script_dir, project_dir]
        if args.debug:
            cmd.append('debug')
        if args.remote_server:
            cmd.extend(["--remote-server", args.remote_server])
        if args.image_dir:
            cmd.extend(["--image-dir", args.image_dir])
        try:
            subprocess.run(cmd, check=True)
        except KeyboardInterrupt:
            sys.exit(1)
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
        if not is_quiet():
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

    # --- 6b. Build DSP from source (CONFIG_DSP_BUILD_FROM_SOURCE=y) ---
    # Runs after configure (so .config_km4 exists) and before the MCU build, so a
    # freshly built dsp.bin is available for packaging / the postbuild layout check.
    if build_dsp_from_source(build_dir, project_dir) != 0:
        sys.exit(1)

    # --- 7. Main Build Command ---
    # Key: Use 'cmake --build' and '--parallel'
    # This enables automatic parallel compilation regardless of the underlying generator (Ninja/Make)
    build_cmd = f'cmake --build "{build_dir}" --parallel'
    if args.core:
        build_cmd += f' --target {args.core}'
    if args.target:
        build_cmd += f' --target {args.target}'

    # --- 8. Remove Old code_size.json Before Build ---
    if args.build_dir:
        old_file = os.path.join(os.path.dirname(args.build_dir), 'code_size.json')
        if os.path.exists(old_file):
            os.remove(old_file)
        fetch_code_size_script = os.path.join(os.path.dirname(args.build_dir), '../utils/code_analyze/fetch_parent_size.py')
        if os.path.exists(fetch_code_size_script):
            ic_name = args.project_dir.strip("/").split('/')[-2]
            fetch_cmd = f'python "{fetch_code_size_script}" --output "{os.path.dirname(args.build_dir)}" --ic {ic_name} --retry {fetch_retry}'
            if skip_current:
                fetch_cmd += ' --skip-current-as-parent'
            if not is_quiet():
                print(fetch_cmd)
            if run_command(fetch_cmd) != 0:
                print('\033[31mError: Fail to fetch parent code size data\033[0m')

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

    _build_state['success'] = True
    if not is_quiet():
        print('\033[32mBuild done\033[0m')


if __name__ == '__main__':
    main(len(sys.argv), sys.argv[1:])
