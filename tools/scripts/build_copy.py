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


def copy_app_folder(app_name, target_dir, submodule_json_path):
    app_path = None
    if os.path.exists(submodule_json_path):
        app_path = find_app_in_submodules(app_name, submodule_json_path)
    if not app_path:
        example_base = '../component/example'
        app_path = find_app_folder(app_name, example_base)

    if app_path:
        s = app_path
        d = os.path.join(target_dir, app_name)
        shutil.copytree(s, d)
        conf_file_path = os.path.join(d, 'prj.conf')
        if os.path.exists(conf_file_path):
            shutil.move(conf_file_path, target_dir)

        create_cmake_file(target_dir, app_name)
    else:
        print(f"Warning: No folder named '{app_name}' found under {example_base} or submodules")

def find_app_folder(app_name, source_base):
    for root, dirs, _ in os.walk(source_base):
        if app_name in dirs:
            return os.path.join(root, app_name)
    return None

def find_app_in_submodules(app_name, submodule_json_path):
    with open(submodule_json_path, 'r') as f:
        submodule_info = json.load(f)

    for submodule in submodule_info:
        for example in submodule.get("examples", []):
            if example.get("name") == app_name:
                app_path = example.get("path", "")
                if app_path:
                    return app_path

    return None

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

def list_available_apps(submodule_info_path: str):
    app_names = ["component/example/*/example_<app_name>.c"]

    if os.path.exists(submodule_info_path):
        with open(submodule_info_path, 'r') as f:
            data = json.load(f)
        for submodule in data:
            for example in submodule.get("examples", []):
                if "name" in example:
                    app_names.append(example["name"])

    if not app_names:
        print("No available example app found")
    else:
        print("-- List available apps:")
        for name in app_names:
            print(f"     - {name}")

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
        submodule_info_path = os.path.join(current_path,'build/submodule_info.json')
        if args.app == "list-apps":
            list_available_apps(submodule_info_path)
            shutil.rmtree(args.target)
            return
        else:
            copy_app_folder(args.app, args.target, submodule_info_path)
    else:
        create_cmake_file(args.target)
    creat_Kconfig_file(args.target)
    create_json_file(args.target, current_path)
    creat_ameba_script(args.target, os.path.dirname(current_path))

if __name__ == '__main__':
    main()



