#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import re
import shutil
import glob
import subprocess

# CMAKE_GDB IS AUTOSET BY CMAKE, DO NOT CHANGE IT MANUALLY.
CMAKE_GDB = '/opt/rtk-toolchain/asdk-10.3.1-4523/linux/newlib/bin/arm-none-eabi-gdb'
PROJECT_DIR = os.path.abspath(os.path.join(os.path.abspath(__file__), '../../..'))
ASDK = os.path.join(PROJECT_DIR, 'project_km4/asdk')
GNU_SCRIPT =  os.path.join(ASDK, 'gnu_utility/gnu_script')
FLASH_SCRIPT = os.path.join(GNU_SCRIPT, 'rtl_gdb_flash_write.txt')
DEBUG_SCRIPT = os.path.join(GNU_SCRIPT, 'rtl_gdb_debug.txt')
CONFIG_FILE = 'menuconfig/.config_km4'
CUT_TYPE = 'acut'
BIN_MAP = [
            ['kr4_km4_app', 'FlashFileSize'],
            ['km4_boot_all', 'XIPBootSize'],
            ['km4_image3_all', 'Img3FileSize'],
            ['dsp', 'DSPFlashSize']
          ]


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
print('ASDK : ', ASDK)
print(".............")
#=================================================================================================================#
if len(sys.argv) > 1 and sys.argv[1] == 'debug':
    print('\033[32m============== gdb debug ================\033[0m')
    cmd = 'cd ' + ASDK +  ' && ' + CMAKE_GDB + ' -x ' + DEBUG_SCRIPT
    print(' GDB CMD : ', cmd)
    os.system(cmd)
else:
    print('\033[32m========== gdb load app image ============\033[0m')

    copy_files_with_glob(os.path.join(GNU_SCRIPT, CUT_TYPE + '_loader'), GNU_SCRIPT, '*')

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
    os.system(cmd)
