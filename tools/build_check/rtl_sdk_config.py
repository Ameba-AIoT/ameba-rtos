#! /usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import re
import argparse
import json
import subprocess


class RTLMacroConfig(object):
    def __init__(self, file):
        self.file = file

    def file_lines_count(self):
        line_count = None
        with open(self.file, "r") as count_f:
            line_count = len(count_f.readlines())

        return line_count

    def find_macro(self, macro):
        macro_pattern = fr"\b{macro}\b"
        find = False
        with open(self.file, "r", encoding="utf-8") as find_f:
            content = find_f.read()
            find_ret = re.findall(macro_pattern, content)
            if find_ret:
                find = True

        return find

    def fine_flag_index(self, flag):
        macro_pattern = fr"\b{flag}\b"
        index = 0
        with open(self.file, "r", encoding="utf-8") as find_idx:
            for r_line in find_idx:
                index += 1
                if re.findall(macro_pattern, r_line):
                    break
            else:
                index = int(self.file_lines_count() / 2)

        return index

    def undef_macro(self, macro):
        """
        undef {macro}
        :param macro:
        :return:
        """
        if not self.find_macro(macro):
            # macro is not found, equal to undef macro
            print(f"{macro} is not found, equal to undef {macro}")
            return True

        macro_pattern = fr"#def.+?\s+{macro}\b"
        undef_set = False
        undef_line = f"#undef  {macro}\n"
        with open(self.file, "r", encoding="utf-8") as r_file:
            r_lines = r_file.readlines()
        with open(self.file, "w", encoding="utf-8") as w_file:
            for r_line in r_lines:
                if re.findall(macro_pattern, r_line):
                    w_file.write(undef_line)
                    undef_set = True
                    continue
                w_file.write(r_line)

        if undef_set:
            print(f"undef {macro} SUCCESS")
        else:
            print(f"undef {macro} FAIL")

    def define_macro(self, macro, value="1", insert_after_row_flag=None):
        """
        #define {macro} value
        :param macro:
        :param value:
        :param insert_row_idx:
        :return:
        """
        define_set = False
        macro_pattern = fr"#[def|un]\S*\s+\b{macro}\b"
        insert_idx = None
        define_line = f"#define {macro} {value}\n"
        if not self.find_macro(macro) or insert_after_row_flag:
            # if macro not exists, insert define_line in insert_idx
            insert_idx = self.fine_flag_index(flag=insert_after_row_flag)
        with open(self.file, "r", encoding="utf-8") as r_file:
            r_lines = r_file.readlines()
        with open(self.file, "w", encoding="utf-8") as w_file:
            if insert_idx:
                # not found macro
                idx = 0
                insert_flag = True
                for r_line in r_lines:
                    if r_line.strip().startswith("/*"):
                        insert_flag = False
                    if r_line.strip().startswith("*/"):
                        insert_flag = True
                    w_file.write(r_line)
                    idx += 1
                    if idx >= insert_idx and insert_flag and not define_set:
                        w_file.write(define_line)
                        define_set = True
            else:
                for r_line in r_lines:
                    if re.findall(macro_pattern, r_line):
                        w_file.write(define_line)
                        define_set = True
                        continue
                    w_file.write(r_line)
        if define_set:
            print(f"#define {macro} {value} SUCCESS")
        else:
            print(f"#define {macro} {value} FAIL")

    def not_set_macro(self, macro):
        """
        # {macro} is not set
        :param macro:
        :return:
        """
        not_set_flag = False
        macro_pattern = fr"\b{macro}\b"
        not_set_line = f"# {macro} is not set\n"
        if not self.find_macro(macro):
            # if macro not exists, equal to macro is not set
            print(f"{macro} is not found, equal to {macro} not set")
            return True
        with open(self.file, "r", encoding="utf-8") as r_file:
            r_lines = r_file.readlines()
        with open(self.file, "w", encoding="utf-8") as w_file:
            for r_line in r_lines:
                if re.findall(macro_pattern, r_line):
                    w_file.write(not_set_line)
                    not_set_flag = True
                    continue
                w_file.write(r_line)

        if not_set_flag:
            print(f"{macro} is not set.")
        else:
            print(f"{macro} set FAIL.")

    def set_macro(self, macro, value="y", insert_after_row_flag=None):
        """
        {macro}=value
        :param macro:
        :param value:
        :param insert_row_idx:
        :return:
        """
        set_flag = False
        insert_idx = None
        macro_pattern = fr"\b{macro}\b"
        set_line = f"{macro}={value}\n"
        if not self.find_macro(macro):
            # if macro not exists, insert define_line in insert_idx
            insert_idx = self.fine_flag_index(flag=insert_after_row_flag)
        with open(self.file, "r", encoding="utf-8") as r_file:
            r_lines = r_file.readlines()
        with open(self.file, "w", encoding="utf-8") as w_file:
            if insert_idx:
                # not found macro
                idx = 1
                for r_line in r_lines:
                    if idx == insert_idx:
                        w_file.write(set_line)
                        set_flag = True
                    w_file.write(r_line)
                    idx += 1
            else:
                for r_line in r_lines:
                    if re.findall(macro_pattern, r_line):
                        w_file.write(set_line)
                        set_flag = True
                        continue
                    w_file.write(r_line)
        if set_flag:
            print(f"Set {macro}={value} SUCCESS.")
        else:
            print(f"Set {macro}={value} FAIL.")

    def remove_macro(self, macro):
        """
        remove {macro}
        :param macro:
        :return:
        """
        if not self.find_macro(macro):
            return True

        macro_pattern = fr"#def.+?\s+{macro}\b|\b{macro}="
        remove = False
        with open(self.file, "r", encoding="utf-8") as r_file:
            r_lines = r_file.readlines()
        with open(self.file, "w", encoding="utf-8") as w_file:
            for r_line in r_lines:
                if re.findall(macro_pattern, r_line) and not remove:
                    remove = True
                    continue
                w_file.write(r_line)

        if remove:
            print(f"Remove {macro} SUCCESS.")
        else:
            print(f"Remove {macro} FAIL.")

def __bytes2str(bytes_data):
    """
    bytes to str
    :param bytes_data:
    :return:
    """
    try:
        return bytes_data.decode("gbk")
    except:
        return bytes_data.decode("utf-8", "ignore")

def run_command(command, work_dir):
    print(f"Run command: [{command}].")
    try:
        p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                             universal_newlines=False, cwd=work_dir)
        out, err = p.communicate()
        out_msg = "\n".join([x.strip("\r").strip() for x in __bytes2str(out).split('\n')]) if out is not None else ""

        if p.returncode == 0:
            print(out_msg)
            return True, out_msg
        else:
            print(out_msg)
            return False, out_msg
    except Exception as ex:
        print('Unexpected run command error from dut. ' + str(ex))
        return False, str(ex)

def get_sdk_cfg(chip_type):
    cfg_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'sdk_cfg.json')
    with open(cfg_path, 'r') as file:
        data = json.load(file)
    
    return data[chip_type]

def set_config(sdk_path, chip_type="AmebaLite", config_type="BT", is_release=False, branch="master"):
    chip_cfg = get_sdk_cfg(chip_type.upper())[config_type.upper()]
    print(f"Set {chip_type} {config_type}...")
    try:
        # set kr4+np/km4+ap
        for cfg_file in chip_cfg.keys():
            file_path_tmp = chip_cfg[cfg_file]["path"]
            if file_path_tmp.startswith("source/") and is_release:
                file_path_tmp = file_path_tmp.split("/", 1)[1]
            file_path = os.path.realpath(os.path.join(sdk_path, file_path_tmp))
            print(f"Start set {file_path_tmp}...")
            config = RTLMacroConfig(file_path)
            for cfg in chip_cfg[cfg_file].keys():
                value = chip_cfg[cfg_file][cfg]
                if cfg == "path" or (not value):
                    continue
                for macro in value:
                    if cfg == "set_y":
                        config.set_macro(macro)
                    if cfg == "set_n":
                        config.set_macro(macro, value="n")
                    if cfg == "remove":
                        config.remove_macro(macro)
                    if cfg == "not_set":
                        config.not_set_macro(macro)
                    if cfg == "undef":
                        config.undef_macro(macro)
                    if cfg == "define_1":
                        config.define_macro(macro)
                    if cfg == "define_0":
                        config.define_macro(macro, value="0")
                    if cfg == "def_insert":
                        for line_flag in value.keys():
                            for insert_macro in value[line_flag]:
                                config.define_macro(insert_macro, insert_after_row_flag=line_flag)
                    if cfg == "set_insert":
                        for li_flag in value.keys():
                            for insert_m in value[li_flag]:
                                config.set_macro(insert_m, insert_after_row_flag=li_flag)
        return True
    except Exception as set_bt_cfg_err:
        print(set_bt_cfg_err)

    return False


def set_dot_config(sdk_path, chip_type="AmebaLite", config_type="BT1", is_release=False, branch="master"):
    chip_cfg = get_sdk_cfg(chip_type.upper())[config_type.upper()]
    print(f"Set {chip_type} {config_type}...")
    print(sdk_path)
    try:
        # set .config
        file_path_tmp = chip_cfg["auto_config"]["path"]
        if file_path_tmp.startswith("source/") and is_release:
            file_path_tmp = file_path_tmp.split("/", 1)[1]
        file_path = os.path.realpath(os.path.join(sdk_path, file_path_tmp))
        print(f"Start set {file_path_tmp}...")
        config = RTLMacroConfig(file_path)
        for cfg in chip_cfg["auto_config"].keys():
            value = chip_cfg["auto_config"][cfg]
            if cfg == "path" or (not value):
                continue
            for macro in value:
                if cfg == "set_y":
                    config.set_macro(macro)
                if cfg == "set_n":
                    config.set_macro(macro, value="n")
                if cfg == "remove":
                    config.remove_macro(macro)
                if cfg == "not_set":
                    config.not_set_macro(macro)
                if cfg == "undef":
                    config.undef_macro(macro)
                if cfg == "define_1":
                    config.define_macro(macro)
                if cfg == "define_0":
                    config.define_macro(macro, value="0")
                if cfg == "def_insert":
                    for line_flag in value.keys():
                        for insert_macro in value[line_flag]:
                            config.define_macro(insert_macro, insert_after_row_flag=line_flag)
                if cfg == "set_insert":
                    for li_flag in value.keys():
                        for insert_m in value[li_flag]:
                            config.set_macro(insert_m, insert_after_row_flag=li_flag)
        return deploy_config(sdk_path, chip_type=chip_type, branch=branch,is_release=is_release)
    except Exception as set_bt_cfg_err:
        print(set_bt_cfg_err)

    return False


def deploy_config(repo_path, chip_type="AmebaSmart", branch="master",is_release=False):
    menuconfig_dir = get_sdk_cfg(chip_type.upper())["build_dir"]
    if menuconfig_dir.startswith("source/") and is_release:
        menuconfig_dir = menuconfig_dir.split("/", 1)[1]
    cmd_dir = os.path.realpath(os.path.join(repo_path, menuconfig_dir))
    save_config_cmd = "make menuconfig CONFIG_DISPLAY=0"
    ret_code, ret_mess = run_command(save_config_cmd, cmd_dir)

    if ret_code == True:
        print("deploy menuconfig SUCCESS")
        return True
    else:
        print("deploy menuconfig FAIL")
        return False


def set_project_platform(sdk_path, chip_type="AmebaLite", core_type="kr4", platform="8726EA"):
    platform = platform.upper()
    chip_platform = get_sdk_cfg(chip_type.upper())["PLATFORM"][core_type.upper()]
    if platform not in chip_platform.keys():
        print(f"No Configuration for setting {chip_type} {core_type} {platform}...")
        return False
    chip_cfg = chip_platform[platform.upper()]
    print(f"Start set {chip_type} {core_type} {platform}...")
    try:
        # set kr4+np/km4+ap
        for cfg_file in chip_cfg.keys():
            file_path_tmp = chip_cfg[cfg_file]["path"]
            file_path = os.path.realpath(os.path.join(sdk_path, file_path_tmp))
            print(f"Start set {file_path_tmp}...")
            config = RTLMacroConfig(file_path)
            for cfg in chip_cfg[cfg_file].keys():
                value = chip_cfg[cfg_file][cfg]
                if cfg == "path" or (not value):
                    continue
                for macro in value:
                    if cfg == "set_y":
                        config.set_macro(macro)
                    if cfg == "set_n":
                        config.set_macro(macro, value="n")
                    if cfg == "remove":
                        config.remove_macro(macro)
                    if cfg == "not_set":
                        config.not_set_macro(macro)
                    if cfg == "undef":
                        config.undef_macro(macro)
                    if cfg == "define_1":
                        config.define_macro(macro)
                    if cfg == "define_0":
                        config.define_macro(macro, value="0")
                    if cfg == "def_insert":
                        for line_flag in value.keys():
                            for insert_macro in value[line_flag]:
                                config.define_macro(insert_macro, insert_after_row_flag=line_flag)
                    if cfg == "set_insert":
                        for li_flag in value.keys():
                            for insert_m in value[li_flag]:
                                config.set_macro(insert_m, insert_after_row_flag=li_flag)
        return True
    except Exception as set_bt_cfg_err:
        print(set_bt_cfg_err)

    return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--sdk_path", default=os.getcwd(),help="specified sdk path to config")
    parser.add_argument("--config", help="specified config type")
    parser.add_argument("--chip", default="AMEBADPLUS", help="specified sdk path to config")
    parser.add_argument("--branch", default="master", help="the current branch")
    parser.add_argument('--is_release', action='store_true', default=False, help="github/release package check or not")
    args = parser.parse_args()

    sdk_path = args.sdk_path
    chip_type = args.chip
    config_type = args.config
    branch = args.branch
    is_release = args.is_release
    print(f"is_release:{is_release}")

    try:
        # set_config(sdk_path, chip_type=chip_type, config_type=config_type)
        set_dot_config(sdk_path, chip_type=chip_type, config_type=config_type, is_release=is_release,branch=branch)
    except Exception as err:
        print(err)

    sys.exit(0)
