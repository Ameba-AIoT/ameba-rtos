#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import re
import shutil
import glob
import subprocess
import argparse

# ===============initialize=====================#

# Parse command line arguments
def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='GDB script for flashing and debugging ameba SOC projects')
    parser.add_argument('project_dir', help='Project directory path (optional, defaults to SDK root)')
    parser.add_argument('action', nargs='?', choices=['debug', 'flash', 'loadns'], default='flash',
                        help='Action to perform: debug, flash, or loadns (default: flash)')
    return parser.parse_args()

args = parse_arguments()

PROJECT_DIR = os.path.abspath(args.project_dir)
SOC_TYPE = os.path.basename(PROJECT_DIR)[:-len('_gcc_project')]
action = args.action

# SOC-specific configurations
SOC_CONFIG = {
    'amebadplus': {
        'ASDK_PROJECT': 'km4',
        'BIN_MAP': [
            ['km0_km4_app', 'FlashFileSize'],
            ['km4_boot_all', 'XIPBootSize'],
            ['km4_image3_all', 'Img3FileSize'],
        ]
    },
    'amebalite': {
        'ASDK_PROJECT': 'km4',
        'BIN_MAP': [
            ['kr4_km4_app', 'FlashFileSize'],
            ['km4_boot_all', 'XIPBootSize'],
            ['km4_image3_all', 'Img3FileSize'],
            ['dsp', 'DSPFlashSize']
        ]
    },
    'amebasmart': {
        'ASDK_PROJECT': 'hp',
        'BIN_MAP': [
            ['km0_km4_ca32_app', 'FlashFileSize'],
            ['km4_boot_all', 'XIPBootSize'],
        ]
    },
    'amebagreen2': {
        'ASDK_PROJECT': 'km4tz',
        'BIN_MAP': [
            ['amebagreen2_app', 'FlashFileSize'],
            ['amebagreen2_boot', 'XIPBootSize'],
            ['km4tz_image3_all', 'Img3FileSize'],
        ]
    },
    'RTL8720F': {
        'ASDK_PROJECT': 'km4tz',
        'BIN_MAP': [
            ['RTL8720F_app', 'FlashFileSize'],
            ['RTL8720F_boot', 'XIPBootSize'],
            ['km4tz_image3_all', 'Img3FileSize'],
        ]
    },
}

CONFIG = SOC_CONFIG.get(SOC_TYPE)
if not CONFIG:
    print("GDB error: Unsupported SOC.")
    sys.exit(1)

target_prj = CONFIG['ASDK_PROJECT']
ASDK = os.path.join(PROJECT_DIR, f"project_{target_prj}/asdk")

GNU_SCRIPT =  os.path.join(ASDK, 'gnu_utility/gnu_script')
FLASH_SCRIPT = os.path.join(GNU_SCRIPT, 'rtl_gdb_flash_write.txt')
DEBUG_SCRIPT = os.path.join(GNU_SCRIPT, 'rtl_gdb_debug.txt')
ROM_SCRIPT = os.path.join(GNU_SCRIPT, 'rtl_gdb_jtag_load_rom.txt')

if SOC_TYPE == 'RTL8720F':
    ASDK_NS = os.path.join(PROJECT_DIR, 'project_km4ns/asdk')
    GNU_SCRIPT_NS =  os.path.join(ASDK_NS, 'gnu_utility/gnu_script')
    ROM_SCRIPT_NS = os.path.join(GNU_SCRIPT_NS, 'rtl_gdb_jtag_load_rom.txt')

GDB_FLOADER = os.path.join(ASDK, 'gnu_utility/gdb_floader')

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

CMAKE_CACHE = os.path.abspath(os.path.join(os.path.dirname(CONFIG_FILE_PATH), '../build/CMakeCache.txt'))
CMAKE_GDB = get_gdb_from_cache(CMAKE_CACHE)
if not CMAKE_GDB:
    print("GDB error: Please build the project first to match path/to/your/toolchain/gdb.exe.")
    sys.exit(1)

# ===============define utility functions=====================#
def replace_in_file(file_path, pattern, replacement):
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    content = re.sub(pattern, replacement, content, flags=re.M)

    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)

def set_file_size_in_script(file_path, pattern, script_file):
    if os.path.isfile(file_path):
        file_size = os.path.getsize(file_path)
        print(file_path, ' size is ', file_size)
    else:
        file_size = 0
        print(file_path, ' do not exist ')
    origin_pattern=pattern.replace('\\', '')
    replace_in_file(script_file, f'^{pattern}.*', f'{origin_pattern} {file_size}')


def copy_files_with_glob(src_folder, dst_folder, pattern):
    if not os.path.exists(dst_folder):
        os.makedirs(dst_folder)

    files_to_copy = glob.glob(os.path.join(src_folder, pattern))

    for file_path in files_to_copy:
        file_name = os.path.basename(file_path)
        dst_file_path = os.path.join(dst_folder, file_name)
        shutil.copy2(file_path, dst_file_path)
        print(f" copy done : {file_path} -> {dst_file_path}")


print(".............")
print('if MP_INCLUDED is set :', BUILD_TYPE)
print('if FPGA is set :', FPGA)
print('ASDK : ', ASDK)
print(".............")

#===================implement the specified action===================#
# debug
if action == 'debug':
    print('\033[32m============== gdb debug ================\033[0m')
    cmd = 'cd ' + ASDK +  ' && ' + CMAKE_GDB + ' -x ' + DEBUG_SCRIPT
    print(' GDB CMD : ', cmd)
    rc = os.system(cmd)
    sys.exit(rc)

# loadns
if SOC_TYPE == 'RTL8720F' and action == 'loadns':
    print('\033[32m============== load ns rom  ================\033[0m')
    cmd = 'cd ' + ASDK_NS +  ' && ' + CMAKE_GDB + ' -x ' + ROM_SCRIPT_NS
    print(' GDB CMD : ', cmd)
    rc = os.system(cmd)
    sys.exit(rc)

# flash
if FPGA == True:
    print(f"\033[32m============== gdb load rom: {target_prj} ================\033[0m")
    cmd = 'cd ' + ASDK +  ' && ' + CMAKE_GDB + ' -x ' + ROM_SCRIPT
    print(' GDB CMD : ', cmd)
    os.system(cmd)

    if SOC_TYPE == 'RTL8720F':
        print('\033[32m============== gdb load rom: km4ns  ================\033[0m')
        cmd = 'cd ' + ASDK_NS +  ' && ' + CMAKE_GDB + ' -x ' + ROM_SCRIPT_NS
        print(' GDB CMD : ', cmd)
        os.system(cmd)

print('\033[32m========== gdb load app image ============\033[0m')

copy_files_with_glob(GDB_FLOADER, GNU_SCRIPT, '*')

APP_BASE_NAME = BIN_MAP[0][0]
if BUILD_TYPE == False:
    IMAGE_TARGET_FOLDER = os.path.join(PROJECT_DIR, ASDK, 'image')
    APP_NAME = APP_BASE_NAME
    replace_in_file(FLASH_SCRIPT, '/image_mp/', '/image/')
    replace_in_file(FLASH_SCRIPT, f'{APP_BASE_NAME}_mp\.bin', f'{APP_NAME}.bin')
else:
    IMAGE_TARGET_FOLDER = os.path.join(PROJECT_DIR, ASDK, 'image_mp')
    APP_NAME = APP_BASE_NAME + '_mp'
    replace_in_file(FLASH_SCRIPT, '/image/', '/image_mp/')
    replace_in_file(FLASH_SCRIPT, f'{APP_BASE_NAME}\.bin', f'{APP_NAME}.bin')
    BIN_MAP[0][0] = APP_NAME

for map in BIN_MAP:
    set_file_size_in_script(os.path.join(IMAGE_TARGET_FOLDER, f'{map[0]}.bin'), f'set \${map[1]} =', FLASH_SCRIPT)

cmd = 'cd ' + ASDK +  ' && ' + CMAKE_GDB + ' -x ' + FLASH_SCRIPT

print(' GDB CMD : ', cmd)
rc = os.system(cmd)
sys.exit(rc)
