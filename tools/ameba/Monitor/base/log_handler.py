#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import datetime
import os
import queue
from typing import List, Optional, Union
from serial.tools.miniterm import Console

from .address_decoder import AddressDecoder
from serial.tools import miniterm
from .key_config import MENU_KEY, TOGGLE_OUTPUT_KEY
from .color_output import print_red, print_yellow
from .constants import TIME_FORMAT, EVENT_QUEUE_TIMEOUT
from .stoppable_thread import StoppableThread
from queue import Queue

key_description = miniterm.key_description



class LogHandler(StoppableThread):
    def __init__(self, elf_file: str, output_queue:Queue, timestamps: bool, enable_address_decoding: bool,
                 toolchain_path: str, log_enabled: bool, log_dir: str,  port: str, logAGG: Optional[List[str]], rom_elf_file: Union[str, None] = None):
        super(LogHandler, self).__init__()
        self.log_file = None
        self.log_dir = log_dir
        self.log_enabled = log_enabled
        self.log_date = None
        self.port = port
        self.output_enabled = True
        self.output_queue = output_queue
        self._start_of_line = True
        self.elf_file = elf_file
        self.timestamps = timestamps
        self.timestamp_format = TIME_FORMAT
        self._buf = bytearray()
        self.running = False
        self.logAGG_srcname = logAGG or []
        self.logAGG_src = 2**len(self.logAGG_srcname) - 1
        if enable_address_decoding:
            self.address_decoder = AddressDecoder(toolchain_path, elf_file, rom_elf_file)
        else:
            self.address_decoder = None
        if self.log_enabled:
            self.start_logging()

    def run(self):
        self.running = True
        while self.running:
            try:
                text = self.output_queue.get(timeout=EVENT_QUEUE_TIMEOUT)           
            except queue.Empty:
                continue

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

    def _stop(self):
        """Stop the log output and clean up resources"""
        print("Closing connection...")
        self.stop_logging()
        self.running = False
        self.stop()

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
            port_name = os.path.basename(self.port)
            name = os.path.join(self.log_dir, f"{port_name}_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.txt")
            if self.log_dir != "" and (not os.path.exists(self.log_dir)):
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

    def print(self, text, pathnum: int = 0):
        new_line_char = "\n"
        text = text.replace("\r\n", "\n")
        text = text.replace("\r", "")
        if text and self.timestamps and (self.output_enabled or self.log_file):
            t = datetime.datetime.now().strftime(self.timestamp_format)[:-3]
            # "text" is not guaranteed to be a full line. Timestamps should be only at the beginning of lines.

            if pathnum == 0:
                line_prefix = t + " "
            elif pathnum == 0x1:
               line_prefix = t + f" [{self.logAGG_srcname[0]}]"
            elif pathnum == 0x2:
                line_prefix = t + f" [{self.logAGG_srcname[1]}]"
            elif pathnum == 0x4:
                line_prefix = t + f" [{self.logAGG_srcname[2]}]"                     
            
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

        self.output_queue.put(text, False)   

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

    def logAGG_parse(self, data: bytes):
        """
        put raw bytes into buffer，logAGG header parse and return events
        Returns:
          [ ("raw", b"..."), ("frame", src, b"..."), ... ]
        """
        events = []
        self._buf.extend(data)

        while self._buf:
            "" " 1) find first 0xFF to sync " ""
            try:
                idx = self._buf.index(0xFF)
            except ValueError:
                "" "if 0xFF is not found，output as raw" ""
                events.append(("raw", bytes(self._buf)))
                self._buf.clear()
                break
            "" "if bytes before 0xFF，output as raw" ""
            if idx > 0:
                events.append(("raw", bytes(self._buf[:idx])))
                del self._buf[:idx]

            "" "make sure there are at lease header byte, otherwise wait for further input" ""
            if len(self._buf) < 2:
                break

            "" " 2) parse header " ""
            header = self._buf[1]
            src    = (header >> 5) & 0x07
            length = ((header >> 2) & 0x07) + 1
            csum   = header & 0x03  # bit1~0

            """ make sure the whole payload is here """
            if len(self._buf) < 2 + length:
                break

            "" " cut the whole payload " ""
            frame = bytes(self._buf[: 2 + length])
            del self._buf[: 2 + length]

            "" " 3) checksum compare：bit1 must be 0，bit0 == popcount(header>>2) % 2 " ""
            "" " header bit7~2 """
            high6 = header >> 2              
            odd_parity = bin(high6).count("1") & 1 
            bit1_ok = (csum & 0x2) == 0
            bit0_ok = (csum & 0x1) == odd_parity
            if not (bit1_ok and bit0_ok):
                "" " checksum fail, output frame as raw " ""
                events.append(("raw", frame))
                continue
            "" " 4) checksum pass and src matched, output frame as frame " ""
            if src in (0x1, 0x2, 0x4) and (src & self.logAGG_src) != 0:
                payload = frame[2:]
                events.append(("frame", src, payload))

        return events

