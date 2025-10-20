#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import argparse
import json

FILES_TO_COPY = ['manifest.json5', '../tools/scripts/build.py', '../tools/scripts/menuconfig.py', '../tools/scripts/monitor.py', '../tools/scripts/flash.py']

def copy_files(file_list, target_dir):
    for file in file_list:
        if os.path.exists(file):
            shutil.copy2(file, target_dir)
        else:
            print(f"Warning: {file} does not exist and will not be copied.")

def copy_or_append(src, dest_path):
    # Check if the destination path already exists
    if os.path.exists(dest_path):
        # Append the content of src to dest_path
        with open(src, 'rb') as src_file:
            with open(dest_path, 'ab') as dest_file:
                dest_file.write(src_file.read())
    else:
        # Copy the file normally
        shutil.copy2(src, dest_path)
        print(f"Copied {src} to {dest_path}")


def copy_app_folder(app_name, source_base, target_dir):
    app_path = None
    for root, dirs, _ in os.walk(source_base):
        if app_name in dirs:
            app_path = os.path.join(root, app_name)
            break

    if app_path:
        s = app_path
        d = os.path.join(target_dir, app_name)
        shutil.copytree(s, d)
        conf_file_path = os.path.join(d, 'prj.conf')
        if os.path.exists(conf_file_path):
            shutil.move(conf_file_path, target_dir)

        create_cmake_file(target_dir, app_name)


    else:
        print(f"Warning: No folder named '{app_name}' found under {source_base}")

def create_json_file(target_dir, current_path):
    config = {
        'Paths': {
            'gcc_project_dir': current_path,
        }
    }

    json_path = os.path.join(target_dir, 'info.json')

    with open(json_path, 'w') as jsonfile:
        json.dump(config, jsonfile, indent=4)


def create_cmake_file(target_dir, subdirectory_name=None):
    if subdirectory_name:
        cmake_content = f"add_subdirectory({subdirectory_name})\r\n"
    else:
        cmake_content="\r\n"
    cmake_path = os.path.join(target_dir, 'CMakeLists.txt')
    with open(cmake_path, 'w') as cmake_file:
        cmake_file.write(cmake_content)

def creat_ameba_script(target_dir, ameba_path):
    ameba_sh = f"source {ameba_path}/ameba.sh \r\n"
    ameba_sh = ameba_sh.replace("\\", "/")
    with open(os.path.join(target_dir, 'ameba.sh'), 'w') as file:
        file.write(ameba_sh)

    ameba_bat = f"call {ameba_path}\\ameba.bat \r\n"
    ameba_bat = ameba_bat.replace("/", "\\")
    with open(os.path.join(target_dir, 'ameba.bat'), 'w') as file:
        file.write(ameba_bat)

def creat_Kconfig_file(target_dir):
    Kconfig_name = os.path.join(target_dir, 'Kconfig')

    with open(Kconfig_name, 'w') as file:
        file.write("\r\n")

    conf_name = os.path.join(target_dir, 'prj.conf')
    if not os.path.exists(conf_name):
        with open(conf_name, 'w') as file:
            file.write("\r\n")


def main():
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('target', help='Target directory')
    parser.add_argument('--app', help='Example Template')

    args = parser.parse_args()

    if os.path.exists(args.target):
        shutil.rmtree(args.target)

    os.makedirs(args.target)

    current_path = os.getcwd()
    copy_files(FILES_TO_COPY, args.target)
    if args.app:
        copy_app_folder(args.app, '../component/example', args.target)
    else:
        create_cmake_file(args.target)
    creat_Kconfig_file(args.target)
    create_json_file(args.target, current_path)
    creat_ameba_script(args.target, os.path.dirname(current_path))

if __name__ == '__main__':
    main()



