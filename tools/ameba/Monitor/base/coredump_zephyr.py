#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import re
import subprocess
import sys
import tempfile
import time
from contextlib import contextmanager
from pathlib import Path
from queue import Queue
import binascii

from .constants import TAG_KEY, GDB_PRINTF_REGISTER_INFO_START, GDB_PRINTF_REGISTER_INFO_END, \
    GDB_PRINTF_BACKTRACE_START, GDB_PRINTF_BACKTRACE_END
from .log_handler import LogHandler
from .color_output import print_yellow, print_red, print_normal
from .gdbhelper import GDBHelper
from .stoppable_thread import StoppableThread

# coredump related messages
COREDUMP_START = r".*?#CD:BEGIN#"
COREDUMP_LINE = r".*?#CD:(.*)"
COREDUMP_END = r".*?#CD:END#"
COREDUMP_ERROR_STR = r".*?#CD:ERROR CANNOT DUMP#"

# coredump states
COREDUMP_IDLE = 0
COREDUMP_READING = 1
COREDUMP_DONE = 2


class CoreDump:
    def __init__(self, decode_coredumps: bool, event_queue: Queue, log_handler: LogHandler, elf_file: str,
                 rom_file: str, toolchain_path: str):
        self._coredump_buffer = b""
        self._decode_coredumps = decode_coredumps
        self.event_queue = event_queue
        self._reading_coredump = COREDUMP_IDLE
        self.log_handler = log_handler
        self.elf_file = elf_file
        self.rom_file = rom_file
        self.toolchain_dir = toolchain_path

    def _process_coredump(self):
        with tempfile.NamedTemporaryFile(mode='wb', delete=False, suffix=".bin") as coredump_file:
            coredump_file.write(self._coredump_buffer)
            coredump_file.flush()

        coredump = ZEPHYR_COREDUMP_Parser(coredump_file.name, self.elf_file, self.toolchain_dir)
        coredump.print_call_info()
        if coredump_file is not None:
            try:
                os.unlink(coredump_file.name)
            except Exception as e:
                print_yellow('Cannot remote temporary core dump file ({})'.format(e))

    def _check_coredump_trigger_before_print(self, line):
        if not self._decode_coredumps:
            return
        if not line.strip():
            return

        coredump_start_mo = re.search(COREDUMP_START, line.strip(), flags=re.I)
        coredump_end_mo = re.match(COREDUMP_END, line.strip(), flags=re.I)
        coredump_line = re.sub(COREDUMP_LINE, "\\1", line.strip(), flags=re.I)
        coredump_err_mo = re.match(COREDUMP_ERROR_STR, line.strip(), flags=re.I)
        coredump_line = re.sub(r"\033\[0m", "", coredump_line, flags=re.I)

        if coredump_err_mo:
            print_yellow('crash dump failed!')
            self._reading_coredump = COREDUMP_DONE
            self._coredump_buffer = b""
            self.log_handler.output_enabled = True
            return

        if coredump_start_mo:
            print_yellow('core dump started (further output muted)')
            self.event_queue.put((TAG_KEY, '\n'))
            self._reading_coredump = COREDUMP_READING
            self._coredump_buffer = b""
            self.log_handler.output_enabled = False
            return
        if coredump_end_mo:
            self._reading_coredump = COREDUMP_DONE
            print_yellow('\ncore dump finished!')
            self._process_coredump()
            return
        if self._reading_coredump == COREDUMP_READING:
            kb = 1024
            buffer_len_kb = len(self._coredump_buffer) // kb
            self._coredump_buffer += binascii.unhexlify(coredump_line)
            new_buffer_len_kb = len(self._coredump_buffer) // kb
            if new_buffer_len_kb > buffer_len_kb:
                print_yellow('received %3d kb...' % new_buffer_len_kb, end='\r')

    def _check_coredump_trigger_after_print(self):
        if not self._decode_coredumps:
            return

        # Re-enable output after the last line of crash dump has been consumed
        if not self.log_handler.output_enabled and self._reading_coredump == COREDUMP_DONE:
            self._reading_coredump = COREDUMP_IDLE
            self.log_handler.output_enabled = True
            self._coredump_buffer = b''

    @contextmanager
    def check(self, line):
        self._check_coredump_trigger_before_print(line)
        try:
            yield
        finally:
            self._check_coredump_trigger_after_print()


#########################################################
class ZEPHYR_COREDUMP_Parser():
    def __init__(self, dump_file, elf_file, toolchain_path):
        self.dump_file = Path(dump_file).resolve()
        self.tool_chain_path = toolchain_path
        self.elf_file = Path(elf_file)
        self.GDB_SERVER_PORT = "6666"

    def start_gdb_server(self):
        # Run the core dump serial log converter:
        coredump_gdb_server = Path(__file__).parent / "coredump_scripts_zephyr" / "coredump_gdbserver.py"
        cmd = [sys.executable, coredump_gdb_server, str(self.elf_file), str(self.dump_file), "--port",
               self.GDB_SERVER_PORT]
        try:
            self.gdb_server = SubprocessThread(cmd)
            self.gdb_server.start()
        except Exception as e:
            self.gdb_server_start = False
            print_yellow(f"Error: Can not start gdb server. {e}")

    def stop_gdb_server(self):
        try:
            if self.gdb_server:
                self.gdb_server.stop()
        finally:
            self.gdb_server = None

    def print_call_info(self):
        # Start the GDB server:
        self.start_gdb_server()
        # Connect to the GDB server:
        while not self.gdb_server.ready:
            time.sleep(0.1)
        gdb_helper = GDBHelper(self.tool_chain_path, self.elf_file, self.GDB_SERVER_PORT)
        gdb_info = gdb_helper.get_info()

        if self.gdb_server.alive:  # actually, gdb server will be killed by gdb_helper
            self.gdb_server.stop()

        if gdb_info:
            out, error = gdb_info

            registers_info_mo = re.search(rf"{GDB_PRINTF_REGISTER_INFO_START}(.*?){GDB_PRINTF_REGISTER_INFO_END}", out,
                                          flags=re.I | re.DOTALL)
            traceback_info_mo = re.search(rf"{GDB_PRINTF_BACKTRACE_START}(.*?){GDB_PRINTF_BACKTRACE_END}", out,
                                          flags=re.I | re.DOTALL)

            if registers_info_mo:
                print(f"\n{'=' * 20} Registers info {'=' * 20}")
                print_normal(registers_info_mo.group(1).strip() + "\n")

            if traceback_info_mo:
                print(f"\n{'=' * 20} Backtrace info {'=' * 20}")
                print_red(traceback_info_mo.group(1).strip() + "\n")
        else:
            return


class SubprocessThread(StoppableThread):
    def __init__(self, command):
        super().__init__()
        self.command = command
        self._need_stop = False
        self._process = None
        self.ready = False

    def run(self):
        self._process = subprocess.Popen(self.command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        while self._process.poll() is None:
            self.ready = True
            time.sleep(0.01)
            if self._need_stop:
                break

    def _cancel(self):
        self._need_stop = True
