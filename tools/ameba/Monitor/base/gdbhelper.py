#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import fnmatch
import subprocess
from pathlib import Path
from typing import Union

from .color_output import print_normal, print_red, print_yellow
from .constants import GDB_PRINTF_REGISTER_INFO_START, GDB_PRINTF_REGISTER_INFO_END, GDB_PRINTF_BACKTRACE_START, \
    GDB_PRINTF_BACKTRACE_END


class GDBHelper:
    def __init__(self, toolchain_path: Union[str, Path], elf_file: Union[str, Path], port: Union[str, int]):
        self.toolchain_path = toolchain_path
        self.elf_file = elf_file
        self.port = port
        self.gdb = None

    def _get_gdb(self):
        if self.gdb is None:
            if os.path.exists(self.toolchain_path):
                bin_dir = os.path.join(self.toolchain_path, "bin")
                if os.path.exists(bin_dir):
                    if os.name == "nt":
                        gdb_name_pattern = "*gdb.exe"
                    else:
                        gdb_name_pattern = "*gdb"
                    for file in os.listdir(bin_dir):
                        if fnmatch.fnmatch(file, gdb_name_pattern):
                            self.gdb = os.path.join(bin_dir, file)
                            return
                else:
                    print_yellow(f"[Warning]Not exists {bin_dir}!")
            print_yellow(f"[Warning]Can not find gdb tool!")

    def get_info(self):
        try:
            self._get_gdb()
            cmd = [self.gdb, str(self.elf_file), "-ex", "target remote localhost:%s" % self.port,
                   "-ex", f"printf \"{GDB_PRINTF_REGISTER_INFO_START}\\n\"",
                   "-ex", "info registers",
                   "-ex", f"printf \"{GDB_PRINTF_REGISTER_INFO_END}\\n\"",
                   "-ex", f"printf \"{GDB_PRINTF_BACKTRACE_START}\\n\"",
                   "-ex", "bt",
                   "-ex", f"printf \"{GDB_PRINTF_BACKTRACE_END}\\n\""
                   ]
            # this cmd will auto get register infos and traceback infos and auto kill gdb server
            process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = process.communicate()

            if process.returncode == 0:
                return out.decode(), err.decode()
            else:
                print_red(f"Can not start gdb! {err.decode()}")
        except Exception as e:
            print_red(f"Can not start gdb! {e}")

        return None
