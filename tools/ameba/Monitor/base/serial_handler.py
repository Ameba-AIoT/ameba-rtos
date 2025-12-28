#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from typing import List, Union
import serial

from .console_reader import ConsoleReader
from .serial_reader import SerialReader, LinuxReader
from .constants import CMD_OUTPUT_TOGGLE, CMD_STOP, CMD_TOGGLE_LOGGING, CMD_TOGGLE_TIMESTAMPS, CONSOLE_STATUS_QUERY
from .log_handler import LogHandler


class SerialHandler:
    """
    The class is responsible for buffering serial input and performing corresponding commands.
    """

    def __init__(self, last_line_part, logger, target_os, elf_file):
        # type: (str,  LogHandler, str,   serial.Serial, str) -> None
        self._last_line_part = last_line_part
        self._np_last_line_part = ""
        self._lp_last_line_part = ""
        self._ap_last_line_part = ""
        self.log_handler = logger
        self.target = target_os
        self.elf_file = elf_file

    def split_data(self, data: str, pathnum: int) -> List[str]:
        """
        Split data into lines, while keeping newlines, and move unfinished line for future processing
        """
        # if data is empty fallback to empty string for easier concatenation with last line
        sp = data.splitlines(keepends=True) or [""]
        if pathnum == 0:        
            if self._last_line_part != "":
                # add unprocessed part from previous "data" to the first line
                sp[0] = self._last_line_part + sp[0]
                self._last_line_part = ""
            if not sp[-1].endswith("\n"):
                # last part is not a full line
                self._last_line_part = sp.pop(-1)
        elif pathnum == 0x01:
            if self._np_last_line_part != "":
                # add unprocessed part from previous "data" to the first line
                sp[0] = self._np_last_line_part + sp[0]
                self._np_last_line_part = ""
            if not sp[-1].endswith("\n"):
                # last part is not a full line
                self._np_last_line_part = sp.pop(-1)
        elif pathnum == 0x02:
            if self._lp_last_line_part != "":
                # add unprocessed part from previous "data" to the first line
                sp[0] = self._lp_last_line_part + sp[0]
                self._lp_last_line_part = ""
            if not sp[-1].endswith("\n"):
                # last part is not a full line
                self._lp_last_line_part = sp.pop(-1)
        elif pathnum == 0x04:
            if self._ap_last_line_part != "":
                # add unprocessed part from previous "data" to the first line
                sp[0] = self._ap_last_line_part + sp[0]
                self._ap_last_line_part = ""
            if not sp[-1].endswith("\n"):
                # last part is not a full line
                self._ap_last_line_part = sp.pop(-1)
        return sp

    def handle_serial_input(self, data, coredump, pathnum: int = 0, finalize_line=False):
        
        sp = self.split_data(
            data, pathnum)  # confirm that sp is a list of entire lines and the left part will be stored in _last_line_part
        for line in sp:
            # line_strip = line.strip()
            line_strip = line
            if coredump:
                with coredump.check(line_strip):
                    self.log_handler.print(line, pathnum)
                    self.log_handler.handle_possible_address_in_line(line_strip)
            else:
                self.log_handler.print(line, pathnum)
                self.log_handler.handle_possible_address_in_line(line_strip)

        # we need to decide whether to handle the _last_line_part
        if self._last_line_part != "" and finalize_line:
            self.log_handler.print(self._last_line_part)
            self.log_handler.handle_possible_address_in_line(self._last_line_part)
            # It is possible that the incomplete line cuts in half the PC
            # address. A small buffer is kept and will be used the next time
            # handle_possible_pc_address_in_line is invoked to avoid this problem.
            # ADDRESS_RE matches 10 character long addresses. Therefore, we
            # keep the last 9 characters.
            self.log_handler.address_buffer = self._last_line_part[-9:]
            self._last_line_part = ""

    def handle_commands(self, cmd: int, console_reader: ConsoleReader, serial_reader: Union[SerialReader, LinuxReader]):
        if cmd == CMD_STOP:
            raise KeyboardInterrupt
        elif cmd == CMD_OUTPUT_TOGGLE:
            self.log_handler.output_toggle()
        elif cmd == CMD_TOGGLE_LOGGING:
            self.log_handler.toggle_logging()
        elif cmd == CMD_TOGGLE_TIMESTAMPS:
            self.log_handler.toggle_timestamps()
        else:
            raise RuntimeError('Bad command data %d' % cmd)  # type: ignore


class SerialStopException(Exception):
    """
    This exception is used for stopping the monitor in testing mode.
    """
    pass
