#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import shutil
import argparse
import json

sdk_root = os.environ.get('AMEBA_SDK')

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
        example_base = os.path.join(sdk_root, 'component/example')
        app_path = find_app_folder(app_name, example_base)

    if app_path:
        s = app_path
        d = os.path.join(target_dir, app_name)
        if os.path.exists(d): shutil.rmtree(d)
        shutil.copytree(s, d)
        conf_file_path = os.path.join(d, 'prj.conf')
        if os.path.exists(conf_file_path):
            f = os.path.join(target_dir, 'prj.conf')
            if os.path.exists(f): os.remove(f)
            shutil.move(conf_file_path, target_dir)

        create_cmake_file(target_dir, app_name)
    else:
        if os.path.exists(submodule_json_path):
            print(f"\nError: Failed to find '{app_name}' folder, or it is not enabled.")
            print(f"       Use '-a list-apps' option to check available names.")
        else:
            print(f"'{app_name}' folder was not found in '{example_base}'. Try to search submodules...")
        sys.exit(1)

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

def create_empty_app(target_dir):
    app_name = 'app_example'
    create_cmake_file(target_dir, app_name)

    app_dir = os.path.join(target_dir, app_name)
    os.makedirs(app_dir)
    src_file = "app_main.c"
    file_path = os.path.join(app_dir, src_file)
    c_code_content = """void app_example(void)
{

}
    """
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(c_code_content)

    cmake_file = os.path.join(app_dir, 'CMakeLists.txt')
    cmake_content = """# Please refer to path/to/your/ameba_sdk/cmake/CMakeLists-template.cmake
set(private_sources)
ameba_list_append(private_sources app_main.c)
ameba_add_internal_library(app_example
    p_SOURCES
	    ${private_sources}
)
    """
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.writelines(cmake_content)

def create_cmake_file(target_dir, subdirectory_name=None):
    if subdirectory_name:
        cmake_content = f"ameba_add_subdirectory({subdirectory_name})\r\n"
    else:
        cmake_content="\r\n"
    cmake_path = os.path.join(target_dir, 'CMakeLists.txt')
    with open(cmake_path, 'w') as cmake_file:
        cmake_file.write(cmake_content)

def creat_ameba_script(target_dir, ameba_path):
    env_sh = 'env.sh'
    env_bat = 'env.bat'
    ameba_sh = f"source {ameba_path}/{env_sh} \r\n"
    ameba_sh = ameba_sh.replace("\\", "/")
    with open(os.path.join(target_dir, env_sh), 'w') as file:
        file.write(ameba_sh)

    ameba_bat = f"call {ameba_path}\\{env_bat} \r\n"
    ameba_bat = ameba_bat.replace("/", "\\")
    with open(os.path.join(target_dir, env_bat), 'w') as file:
        file.write(ameba_bat)

def creat_Kconfig_file(target_dir):
    Kconfig_name = os.path.join(target_dir, 'Kconfig')

    with open(Kconfig_name, 'w') as file:
        file.write("# Please refer to https://aiot.realmcu.com/en/latest/rtos/sdk/sdk_config/index.html\r\n")

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

    current_path = os.getcwd()
    if args.app:
        submodule_info_path = os.path.join(current_path,'build/submodule_info.json')
        if args.app == "list-apps":
            list_available_apps(submodule_info_path)
            return

    args.target = os.path.abspath(args.target)
    if args.target != os.getcwd():
        if os.path.exists(args.target):
            shutil.rmtree(args.target)

        os.makedirs(args.target)
    else:
        for filename in os.listdir(args.target):
            file_path = os.path.join(args.target, filename)
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)

    if args.app:
        copy_app_folder(args.app, args.target, submodule_info_path)
    else:
        create_empty_app(args.target)

    creat_Kconfig_file(args.target)
    creat_ameba_script(args.target, sdk_root)

if __name__ == '__main__':
    main()



