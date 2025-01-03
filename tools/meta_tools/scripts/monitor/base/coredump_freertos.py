#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import io
import os
import re
import tempfile
from contextlib import contextmanager, redirect_stdout
from typing import Generator
from pathlib import Path
from queue import Queue

from .constants import TAG_KEY
from .log_handler import LogHandler
from .color_output import print_yellow, print_red
from .address_decoder import AddressDecoder

# coredump related messages
COREDUMP_START = '.*?(========== Crash Dump ==========.*)'
COREDUMP_END = '========== End of Crash Dump =========='

STACK_DUMP_PATTERN = re.compile('==========\w+ Stack Dump ==========', flags=re.I)
STACK_END_PATTERN = re.compile('StackEnd: (0x[\da-f]{8})', flags=re.I)
STACK_DEPTH_PATTERN = re.compile(r'.*?dump\s+stack\s+depth\s+=\s*(\d+)$', flags=re.I)
STACK_DATA_PATTERN = re.compile(r'\[([\da-f]+)]\s+((?:[\da-f]{8}\s*)+)', flags=re.I)

# coredump states
COREDUMP_IDLE = 0
COREDUMP_READING = 1
COREDUMP_DONE = 2


class CoreDump:
    def __init__(self, decode_coredumps: bool, event_queue: Queue, log_handler: LogHandler, elf_file: str,
                 rom_file: str, toolchain_path: str):
        self._coredump_buffer = ""
        self._decode_coredumps = decode_coredumps
        self.event_queue = event_queue
        self._reading_coredump = COREDUMP_IDLE
        self.log_handler = log_handler
        self.elf_file = elf_file
        self.rom_file = rom_file
        self.toolchain_path = toolchain_path

    def _process_coredump(self):
        with tempfile.NamedTemporaryFile(mode='w', delete=False) as coredump_file:
            coredump_file.write(self._coredump_buffer)
            coredump_file.flush()

        coredump = RTOS_COREDUMP_Parser(coredump_file.name, self.elf_file, self.rom_file, self.toolchain_path)
        coredump.print_call_info()

        if coredump_file is not None:
            try:
                os.unlink(coredump_file.name)
            except Exception as e:
                print_yellow('Cannot remote temporary core dump file ({})'.format(e))

    def _check_coredump_trigger_before_print(self, line):  # type: (str) -> None
        if not self._decode_coredumps:
            return
        if not line.strip():
            return

        coredump_start_mo = re.search(COREDUMP_START, line.strip(), flags=re.I)
        coredump_end_mo = re.match(COREDUMP_END, line.strip(), flags=re.I)

        if coredump_start_mo:
            print_yellow('core dump started (further output muted)')
            self.event_queue.put((TAG_KEY, '\n'))
            self._reading_coredump = COREDUMP_READING
            self._coredump_buffer = coredump_start_mo.group(1) + "\n"
            self.log_handler.output_enabled = False
            return
        if coredump_end_mo:
            self._reading_coredump = COREDUMP_DONE
            print_yellow('\ncore dump finished!')
            self._process_coredump()
            return
        if self._reading_coredump == COREDUMP_READING:
            kb = 1024
            buffer_len_kb = len(self._coredump_buffer.encode()) // kb
            self._coredump_buffer += line.strip() + "\n"
            new_buffer_len_kb = len(self._coredump_buffer) // kb
            if new_buffer_len_kb > buffer_len_kb:
                print_yellow('received %3d kb...' % new_buffer_len_kb, end='\r')

    def _check_coredump_trigger_after_print(self):  # type: () -> None
        if not self._decode_coredumps:
            return

        # Re-enable output after the last line of crash dump has been consumed
        if not self.log_handler.output_enabled and self._reading_coredump == COREDUMP_DONE:
            self._reading_coredump = COREDUMP_IDLE
            self.log_handler.output_enabled = True
            self._coredump_buffer = ""

    @contextmanager
    def check(self, line):  # type: (bytes) -> Generator
        self._check_coredump_trigger_before_print(line)
        try:
            yield
        finally:
            self._check_coredump_trigger_after_print()


#########################################################
class StackData():
    def __init__(self, addr, val):
        self.addr = addr
        self.val = int(val, 16)

    @property
    def valid(self):
        return self.val % 2 != 0

    @property
    def actual_val(self):
        return self.val - 1


class StackDump():
    def __init__(self, dump_file, stack_depth=None):
        self.dump_file = dump_file
        self.crash_info = ""
        self.stack_depth = stack_depth
        self.stack_end = None

    def parse(self):
        # [30009ee8] 00120107 05000000 0000000a 00000000
        dump_datas = []
        stack_start = False
        with open(self.dump_file, "r", encoding="utf-8") as f:
            for line in f:
                stack_end_mo = STACK_END_PATTERN.match(line)
                stack_depth_mo = STACK_DEPTH_PATTERN.match(line)
                stack_data_mo = STACK_DATA_PATTERN.match(line)
                stack_dump_mo = STACK_DUMP_PATTERN.match(line)
                if stack_end_mo:
                    self.stack_end = int(stack_end_mo.group(1), 16)
                if stack_depth_mo and not self.stack_depth:
                    self.stack_depth = int(stack_depth_mo.group(1).strip())
                if stack_dump_mo:
                    stack_start = True
                if stack_start == True and stack_data_mo:
                    start_addr = int(stack_data_mo.group(1), 16)
                    datas = [StackData(start_addr + i * 4, f"0x{x}") for i, x in
                             enumerate(stack_data_mo.group(2).split())]
                    dump_datas.extend(datas)
                else:
                    self.crash_info += line

        self.r0 = dump_datas[0]
        self.r1 = dump_datas[1]
        self.r2 = dump_datas[2]
        self.r3 = dump_datas[3]
        self.r12 = dump_datas[4]
        self.lr = dump_datas[5]
        self.pc = dump_datas[6]
        self.psr = dump_datas[7]

        all_stack_datas = dump_datas[8:]

        # filter stack datas according to stack depth
        if self.stack_depth is not None:
            all_stack_datas = all_stack_datas[:self.stack_depth - 8]
        # filter stack datas according to valid
        all_stack_datas = [x for x in all_stack_datas if x.valid]
        # filter stack datas according to stack end
        if self.stack_end is not None:
            all_stack_datas = [x for x in all_stack_datas if int(x.addr) <= self.stack_end]

        stack_datas = [self.pc]
        # start from lr
        got_lr = False
        for data in all_stack_datas:
            if not got_lr:
                if data.val != self.lr.val:
                    continue
                else:
                    got_lr = True
                    stack_datas.append(data)
            else:
                stack_datas.append(data)

        self.stack_datas = stack_datas


class RTOS_COREDUMP_Parser():
    def __init__(self, dump_file, elf_file, rom_file, toolchain_prefix):
        self.dump_file = Path(dump_file).resolve()
        self.elf_file = Path(elf_file)
        self.address_decoder = AddressDecoder(toolchain_prefix, elf_file, rom_file)

    def get_stack_dump(self):
        stack_dump = StackDump(self.dump_file)
        stack_dump.parse()
        return stack_dump

    def print_call_info(self):
        stack_dump = self.get_stack_dump()
        # print ori crash info from serial
        print(stack_dump.crash_info)
        # print backtrace info
        print(f"\n{'=' * 20} Backtrace info {'=' * 20}")
        for data in stack_dump.stack_datas:
            output = self.address_decoder.decode_address(f"0x{data.actual_val - 1:08x}")
            if output:
                print_red(output.strip())
