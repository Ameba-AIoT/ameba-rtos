#!/rtkoss/python/3.6.7/rhel7/x86_64/bin/python3
# coding=utf-8
"""
@Project : walk_c
@File    : gen_noused_c.py
@Time    : 2024/4/23 17:46
@Author  : terra_cai
@Email   : terra_cai@realsil.com.cn
@Software: PyCharm
"""

import os
import re
import sys
from pathlib import Path
import argparse

c_language_keywords = [
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float",
    "for", "goto", "if", "elif", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof",
    "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local", "defined"
]
func_pattern = re.compile(r"^(\w+\s+)+(\w+)\s*\(.*?")
non_func_prefix = ("typedef", "enum")


class Function():
    def __init__(self, name):
        self.name = name
        self.define = []
        self.is_func = True

    def parser(self):
        define_str = "\n".join(self.define)
        self.body = re.sub(r".*?(\{.*}).*", "\\1", define_str, flags=re.DOTALL)
        self.header = define_str.replace(self.body, "")
        if ";" in self.header:
            self.is_func = False
            return

    def __repr__(self):
        # return self.name
        return f"Function({self.name})"


def get_asms_funcs(asms: [str]) -> [str]:
    func_names = []
    for asm in asms:
        asm_file = Path(asm).resolve()
        if asm_file.exists():
            for line in asm_file.read_text().splitlines():
                func_name_mo = re.match(r"^[\da-f]+\s+<(.+)>:$", line.strip())
                if func_name_mo:
                    func_name = func_name_mo.group(1)
                    func_names.append(func_name)
        else:
            print(f"Not exists {asm_file}!")
    return func_names


def get_c_funcs(cfile) -> [Function]:
    c_file = Path(cfile).resolve()
    funcs = {}
    actual_funcs = {}
    if c_file.exists():
        file_contents = strip_comments(c_file.read_text(encoding="utf-8", errors="replace"))
        left_brace = 0
        right_brace = 0
        body_start = False
        func = None
        for line in file_contents.splitlines():
            if line.strip().startswith("extern "):
                continue
            func_mo = func_pattern.match(line)
            # 判断是否是函数
            if func_mo:
                func_prefix = func_mo.group(1).strip()
                if func_prefix.startswith(non_func_prefix):
                    continue
                func_name = func_mo.group(2)
                if func_name not in c_language_keywords:
                    body_start = True
                    func = Function(func_name)
                    funcs.update({func_name: func})
                    left_brace = 0
                    right_brace = 0
            # 检测左右大括号数量
            left_brace += line.count("{")
            right_brace += line.count("}")
            # 如果左右大括号数量一致，并且大于1说明函数定义结束
            if body_start and left_brace == right_brace and left_brace > 0:
                func.define.append(line)
                body_start = False
            # 函数定义
            if body_start:
                func.define.append(line)
        for key, func in funcs.items():
            func.parser()
            if func.is_func:
                actual_funcs.update({key: func})
            else:
                print("Is Declare!")
    else:
        print(f"Not exists {c_file}!")

    return actual_funcs


def strip_comments(file_contents):
    file_contents = re.sub(r'/\*.*?\*/', '', file_contents, flags=re.DOTALL)
    file_contents = re.sub(r'//.*', '', file_contents)
    return file_contents


def gen_new_file(asms, cfile, result_file):
    cfile = Path(cfile).resolve()
    asm_funcs = get_asms_funcs(asms)
    new_file_lines = []
    write_into = False
    result_file = Path(result_file)

    if asm_funcs and cfile.exists():
        c_funcs = get_c_funcs(cfile)
        for line in cfile.read_text().splitlines():
            if line.strip().startswith("#include"):
                new_file_lines.append(line)
        new_file_lines.append("\n")

        for func_name, func in c_funcs.items():
            if func_name not in asm_funcs:
                new_file_lines.extend(func.define)
                new_file_lines.append("\n")
            else:
                # 默认不写入，如果有一个函数找到了，就可以写入结果，如果所有都找不到，则不写入结果
                write_into = True
    if not write_into:
        result_file.write_text("")
        print(f"Gen empty file: {result_file}!")
    else:
        result_file.write_text("\n".join(new_file_lines))
        print(f"Gen file: {result_file}!")


if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('-a', "--asms", nargs="+", help="Set asm files.")
    arg_parser.add_argument('-c', "--cfile", help="Set cfile path.")
    arg_parser.add_argument('-r', '--result', help="Set result file path!")
    args = arg_parser.parse_args()
    if sys.gettrace():
        args.asms = [r"C:\Users\terra_cai\Desktop\wifi\target_img2_otrcores.asm"]
        args.cfile = r"C:\Users\terra_cai\Desktop\wifi\wifi_intf_drv_to_app_ext_strong.c"
        args.result = r"C:\Users\terra_cai\Desktop\wifi\wifi_intf_drv_to_app_ext_noused.c"
    gen_new_file(args.asms, args.cfile, args.result)
