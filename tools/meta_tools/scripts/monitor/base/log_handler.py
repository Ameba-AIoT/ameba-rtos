#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import datetime
import os
from typing import Union
from serial.tools.miniterm import Console

from .address_decoder import AddressDecoder
from serial.tools import miniterm
from .key_config import MENU_KEY, TOGGLE_OUTPUT_KEY
from .color_output import print_red, print_yellow
from .constants import TIME_FORMAT

key_description = miniterm.key_description


class LogHandler:
    def __init__(self, elf_file: str, timestamps: bool, enable_address_decoding: bool,
                 toolchain_path: str, log_enabled: bool, log_dir: str, port: str, rom_elf_file: Union[str, None] = None):
        self.log_file = None
        self.log_dir = log_dir
        self.log_enabled = log_enabled
        self.log_date = None
        self.port = port
        self.output_enabled = True
        self._start_of_line = True
        self.elf_file = elf_file
        self.timestamps = timestamps
        self.timestamp_format = TIME_FORMAT
        if enable_address_decoding:
            self.address_decoder = AddressDecoder(toolchain_path, elf_file, rom_elf_file)
        else:
            self.address_decoder = None
        if self.log_enabled:
            self.start_logging()

    @property
    def address_buffer(self):
        return getattr(self.address_decoder, "address_buffer", "")

    @address_buffer.setter
    def address_buffer(self, value: str):
        if self.address_decoder:
            self.address_decoder.address_buffer = value

    def toggle_logging(self):
        if self.log_file:
            self.stop_logging()
        else:
            self.start_logging()

    def toggle_timestamps(self):
        self.timestamps = not self.timestamps

    def start_logging(self):
        if not self.log_file:
            name = os.path.join(self.log_dir, f"{self.port}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.txt")
            if not os.path.exists(self.log_dir):
                os.makedirs(self.log_dir, exist_ok=True)
            try:
                self.log_file = open(name, "w")
                self.log_date = datetime.datetime.now().date()
                print_yellow(f"Logging is enabled into file {name}")
            except Exception as e:
                print_red(f"\nLog file {name} cannot be created: {e}")

    def stop_logging(self):
        if self.log_file:
            try:
                name = self.log_file.name
                self.log_file.close()
                print_yellow(f"Logging is disabled and file {name} has been closed")
            except Exception as e:  #
                print_red(f"\nLog file cannot be closed: {e}")
            finally:
                self.log_file = None

    def print(self, text):
        new_line_char = "\n"
        text = text.replace("\r\n", "\n")
        text = text.replace("\r", "")
        if text and self.timestamps and (self.output_enabled or self.log_file):
            t = datetime.datetime.now().strftime(self.timestamp_format)[:-3]
            # "text" is not guaranteed to be a full line. Timestamps should be only at the beginning of lines.
            line_prefix = t + " "

            # If the output is at the start of a new line, prefix it with the timestamp text.
            if self._start_of_line:
                text = line_prefix + text

            # If the new output ends with a newline, remove it so that we don't add a trailing timestamp.
            self._start_of_line = text.endswith(new_line_char)
            if self._start_of_line:
                text = text[:-len(new_line_char)]

            text = text.replace(new_line_char, new_line_char + line_prefix)

            # If we're at the start of a new line again, restore the final newline.
            if self._start_of_line:
                text += new_line_char
        elif text:
            self._start_of_line = text.endswith(new_line_char)

        if self.output_enabled:
            print(text, end='')
        if self.log_file:
            if datetime.datetime.now().date() != self.log_date:
                self.stop_logging()
                self.start_logging()
            try:
                self.log_file.write(text)
                self.log_file.flush()
            except Exception as e:
                print_red(f"\nCannot write to file: {e}")
                self.stop_logging()

    def output_toggle(self):  # type: () -> None
        self.output_enabled = not self.output_enabled
        print_yellow(f"\nToggle output display: {self.output_enabled}, "
                     f"Type {key_description(MENU_KEY)} {key_description(TOGGLE_OUTPUT_KEY)} "
                     "to show/disable output again.")

    def handle_possible_address_in_line(self, line) -> None:
        if not self.address_decoder:
            return
        translation = self.address_decoder.decode_address(line)
        if translation:
            print_yellow(translation)
