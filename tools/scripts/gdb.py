#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import re
import shutil
import glob
import subprocess
import argparse
import json5

# ===============initialize=====================#
GNU_SCRIPT = os.path.join(os.path.dirname(__file__), 'gnu_script')

# Parse command line arguments
def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='GDB script for flashing and debugging ameba SOC projects')
    parser.add_argument('project_dir', help='Project directory path (optional, defaults to SDK root)')
    parser.add_argument('action', nargs='?', choices=['debug', 'flash'], default='flash',
                        help='Action to perform: debug or flash (default: flash)')
    return parser.parse_args()

args = parse_arguments()

PROJECT_DIR = os.path.abspath(args.project_dir)
SOC_TYPE = os.path.basename(os.path.dirname(PROJECT_DIR))
action = args.action

# =============== Load SOC Configuration =====================#
class HexInt(int):
    """
    This class behaves like an int, but prints as a hex string.
    """
    def __repr__(self):
        return hex(self)

    def __str__(self):
        return hex(self)

def load_soc_config(json_file):
    if not os.path.exists(json_file):
        print(f"GDB Error: gdb config file not found: {json_file}")
        sys.exit(1)

    def convert_hex(obj):
        if isinstance(obj, dict):
            return {k: convert_hex(v) for k, v in obj.items()}
        elif isinstance(obj, list):
            return [convert_hex(i) for i in obj]
        elif isinstance(obj, str) and obj.startswith('0x'):
            try:
                return HexInt(int(obj, 16))
            except ValueError:
                return obj
        else:
            return obj

    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            raw_data = json5.load(f)
        return convert_hex(raw_data)
    except Exception as e:
        print(f"GDB Error: Failed to parse JSON: {e}")
        sys.exit(1)

SOC_CONFIG = load_soc_config(os.path.join(GNU_SCRIPT, 'ameba_gdb_config.json5'))

CONFIG = SOC_CONFIG.get(SOC_TYPE)
if not CONFIG:
    print("GDB error: Unsupported SOC.")
    sys.exit(1)

target_prj = CONFIG['IMG_PROJECT']
target_prj_dir = os.path.join(PROJECT_DIR, f"project_{target_prj}")
GDB_FLOADER = os.path.join(target_prj_dir, 'gnu_utility/gdb_floader')

CONFIG_FILE = 'menuconfig/.config'
BIN_MAP = CONFIG['BIN_MAP']

def check_config(file_path, config):
    with open(file_path, 'r', encoding='utf-8') as file:
        for line in file:
            if config in line:
                return True
    return False

if os.getcwd() == PROJECT_DIR:
    CONFIG_FILE_PATH = os.path.join(PROJECT_DIR, CONFIG_FILE)
else:
    CONFIG_FILE_PATH = os.path.join(os.environ.get('SOC_WORK_DIR'), CONFIG_FILE)
BUILD_TYPE = check_config(CONFIG_FILE_PATH, 'CONFIG_MP_INCLUDED=y')
FPGA = check_config(CONFIG_FILE_PATH, 'CONFIG_FPGA=y')

def get_gdb_from_cache(cache_file_path):
    cmake_gdb_path = None
    if not os.path.exists(cache_file_path):
        return None
    with open(cache_file_path, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.strip()
            if line.startswith("CMAKE_GDB:FILEPATH="):
                cmake_gdb_path = line.split("=", 1)[1]
                break
    if cmake_gdb_path and os.path.exists(cmake_gdb_path):
        return cmake_gdb_path
    return None

BUILD_DIR = os.path.abspath(os.path.join(os.path.dirname(CONFIG_FILE_PATH), '../build'))
CMAKE_CACHE = os.path.join(BUILD_DIR, 'CMakeCache.txt')
CMAKE_GDB = get_gdb_from_cache(CMAKE_CACHE)
if not CMAKE_GDB:
    print("GDB error: Please build the project first to match path/to/your/toolchain/gdb.exe.")
    sys.exit(1)

print(".............")
print('if MP_INCLUDED is set :', BUILD_TYPE)
print('if FPGA is set :', FPGA)
print(".............")

# ===============define utility functions=====================#
def copy_files_with_glob(src_folder, dst_folder, pattern):
    if not os.path.exists(dst_folder):
        os.makedirs(dst_folder)

    files_to_copy = glob.glob(os.path.join(src_folder, pattern))

    for file_path in files_to_copy:
        file_name = os.path.basename(file_path)
        dst_file_path = os.path.join(dst_folder, file_name)
        shutil.copy2(file_path, dst_file_path)
        print(f" copy done : {file_path} -> {dst_file_path}")


def get_bin_paths(bin_map, build_type):
    base_dir = './image_mp' if build_type else './image'
    paths = {}
    APP_BASE_NAME = bin_map[0][0] if bin_map else None

    for name, size_key in bin_map:
        is_app = (name == APP_BASE_NAME) or (size_key == 'FlashFileSize')
        need_mp_suffix = build_type and is_app
        fname = name + ('_mp' if need_mp_suffix else '') + '.bin'
        if name == 'dsp':
            rel_path = '../project_dsp/image/dsp.bin'
        else:
            rel_path = f'{base_dir}/{fname}'
        paths[size_key] = rel_path

    return paths

def get_bin_size(parent_dir, rel_path):
    if not rel_path:
        return 0
    abs_path = os.path.normpath(os.path.join(parent_dir, rel_path))
    return os.path.getsize(abs_path) if os.path.isfile(abs_path) else 0

def fmt_path(path):
    '''
    Format windows path to gdb friendly path,
    e.g. change "C:\path\to\file" to "C:/path/to/file"
    '''
    if path and isinstance(path, str):
        return path.replace('\\', '/')
    return path

def fmt_gdb_arg(var_name, value):
    '''
    Format a gdb -ex argument to set a variable, for different OS,
    e.g. on Windows: -ex "set $VAR=\"value\"", on Linux: -ex 'set $VAR="value"'
    '''
    if isinstance(value, str):
        gdb_val = f'"{fmt_path(value)}"'
    else:
        gdb_val = str(value)

    gdb_cmd = f'set ${var_name}={gdb_val}'
    if os.name == 'nt': # Windows
        escaped_cmd = gdb_cmd.replace('"', '\\"')
        return f' -ex "{escaped_cmd}"'
    else:
        return f" -ex '{gdb_cmd}'"

#===================implement the specified action===================#
FLASH_SCRIPT   = fmt_path(os.path.join(GNU_SCRIPT, 'rtl_gdb_flash_write.txt'))
DEBUG_SCRIPT   = fmt_path(os.path.join(GNU_SCRIPT, 'rtl_gdb_debug.txt'))
ROM_SCRIPT     = fmt_path(os.path.join(GNU_SCRIPT, 'rtl_gdb_jtag_load_rom.txt'))
CMAKE_GDB      = fmt_path(CMAKE_GDB)

# debug
if action == 'debug':
    print('\033[32m============== gdb debug ================\033[0m')

    gdb_port = CONFIG.get('GDB_PORTS', {}).get(target_prj, 2335)
    reg_vars = CONFIG.get('REG_VARS', {})

    ex_list = []
    ex_list.append(fmt_gdb_arg('GDB_PORT', gdb_port))
    for var_name, var_value in reg_vars.items():
        ex_list.append(fmt_gdb_arg(var_name, var_value))
    ex_commands = "".join(ex_list)

    IMAGE_PARENT = fmt_path(os.path.join(BUILD_DIR, f'project_{target_prj}'))     # parent dir of /image
    cmd = 'cd ' + IMAGE_PARENT + ' && ' + CMAKE_GDB + ex_commands + ' -x ' + DEBUG_SCRIPT
    print(' GDB CMD : ', cmd)
    rc = os.system(cmd)
    sys.exit(rc)

# flash: 1- load rom, 2- load app image
if FPGA == True:
    rom_section = CONFIG.get('ROM', {})
    if not rom_section:
        print("No rom config, skip load rom")
    for core, file_list in rom_section.items():
        print(f'\033[32m============== gdb load rom: {core}  ================\033[0m')
        LIB_PARENT = fmt_path(os.path.join(PROJECT_DIR, f'project_{core}'))       # parent dir of /lib

        ex_list = []
        ex_list.append(fmt_gdb_arg('SCRIPT_DIR', GNU_SCRIPT))
        ex_list.append(fmt_gdb_arg('GDB_PORT', CONFIG.get('GDB_PORTS', {}).get(core, 2335)))
        ex_list.append(fmt_gdb_arg('ROM_BIN', file_list[0]))
        ex_list.append(fmt_gdb_arg('ROM_ADDR', file_list[1]))
        ex_commands = "".join(ex_list)

        cmd = 'cd ' + LIB_PARENT +  ' && ' + CMAKE_GDB + ex_commands + ' -x ' + ROM_SCRIPT
        print(' GDB CMD : ', cmd)
        os.system(cmd)

print('\033[32m========== gdb load app image ============\033[0m')

copy_files_with_glob(GDB_FLOADER, GNU_SCRIPT, '*')

IMAGE_PARENT = fmt_path(os.path.join(BUILD_DIR, f'project_{target_prj}'))        # parent dir of /image or /image_mp
BIN_PATHS = get_bin_paths(BIN_MAP, BUILD_TYPE)
BIN_SIZES = {k: get_bin_size(IMAGE_PARENT, v) for k, v in BIN_PATHS.items()}
for k, v in BIN_PATHS.items():
    print(f'image file: {v}, size: {BIN_SIZES[k]}')

gdb_port = CONFIG.get('GDB_PORTS', {}).get(target_prj, 2335)

ex_list = []
ex_list.append(fmt_gdb_arg('SCRIPT_DIR', GNU_SCRIPT))
ex_list.append(fmt_gdb_arg('GDB_PORT',   gdb_port))
ex_list.append(fmt_gdb_arg('FlashFileSize', BIN_SIZES.get("FlashFileSize", 0)))
ex_list.append(fmt_gdb_arg('XIPBootSize',   BIN_SIZES.get("XIPBootSize", 0)))
ex_list.append(fmt_gdb_arg('Img3FileSize',  BIN_SIZES.get("Img3FileSize", 0)))
ex_list.append(fmt_gdb_arg('DSPFlashSize',  BIN_SIZES.get("DSPFlashSize", 0)))

reg_vars = CONFIG.get('REG_VARS', {})
for var_name, var_value in reg_vars.items():
    ex_list.append(fmt_gdb_arg(var_name, var_value))

flash_params = CONFIG.get('FLASH_PARAMS', {})
for param_name, param_value in flash_params.items():
    ex_list.append(fmt_gdb_arg(param_name, param_value))

if BIN_PATHS.get('FlashFileSize'):
    ex_list.append(fmt_gdb_arg('BIN_IMG2_ALL', BIN_PATHS["FlashFileSize"]))
if BIN_PATHS.get('XIPBootSize'):
    ex_list.append(fmt_gdb_arg('BIN_IMG1_BOOT', BIN_PATHS["XIPBootSize"]))
if BIN_PATHS.get('Img3FileSize'):
    ex_list.append(fmt_gdb_arg('BIN_KM4_IMG3', BIN_PATHS["Img3FileSize"]))
if BIN_PATHS.get('DSPFlashSize'):
    ex_list.append(fmt_gdb_arg('BIN_DSP_ALL', BIN_PATHS["DSPFlashSize"]))

ex_cmds = ' '.join(ex_list)
cmd = 'cd ' + IMAGE_PARENT +  ' && ' + CMAKE_GDB + ' ' + ex_cmds + ' -x ' + FLASH_SCRIPT

print(' GDB CMD : ', cmd)
rc = os.system(cmd)
sys.exit(rc)

