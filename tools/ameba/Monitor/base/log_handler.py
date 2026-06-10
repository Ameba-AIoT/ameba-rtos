#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import datetime
import os
import queue
import re
from typing import List, Optional, Union
from serial.tools.miniterm import Console

from .address_decoder import AddressDecoder
from serial.tools import miniterm
from .key_config import MENU_KEY, TOGGLE_OUTPUT_KEY
from .color_output import print_red, print_yellow
from .constants import TIME_FORMAT, EVENT_QUEUE_TIMEOUT
from .stoppable_thread import StoppableThread
from .chip_info import AggKind, resolve_path, get_core_tag, bin_suffix
from queue import Queue

key_description = miniterm.key_description

STACKTRACE_START = '.*?(========== Stack Trace ==========.*)'
STACKTRACE_END = '========== End of Stack Trace =========='


class LogHandler(StoppableThread):
    def __init__(self, elf_file: str, output_queue:Queue, timestamps: bool, enable_address_decoding: bool,
                 toolchain_path: str, log_enabled: bool, log_dir: str,  port: str, logAGG: Optional[List[str]],
                 rom_elf_file: Union[str, None] = None, chip: Optional[str] = None):
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
        # Bitmask filter for text paths in --logAGG mode only. The chip-mode
        # branch of logAGG_parse does not consult this field — dispatch there
        # is fully driven by CHIP_TABLE via resolve_path_kind. Left at 0 when
        # --logAGG is absent because nothing reads it in that case.
        self.logAGG_src = 2**len(self.logAGG_srcname) - 1 if self.logAGG_srcname else 0
        self._stacktrace = False
        # IC chip for table-driven path dispatch and tag labeling.
        # Only used when --logAGG is not supplied; --logAGG wins everywhere when present.
        self.chip = chip
        # Lazy-opened binary sinks for AGG binary paths, keyed by path number.
        # The label on each path's CHIP_TABLE entry supplies the filename
        # prefix; the kind (BIN_FILE/LOG_FILE) supplies the suffix.
        # Per-path state: missing/None = not yet opened, False = open failed
        # (don't retry), file = open. The False sentinel prevents per-packet
        # error-message floods when the destination directory is unwritable.
        self._agg_bin_files = {}
        self._agg_cwd_warned = False
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
                print(text, end='', flush=True)
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
        self._close_agg_binary_files()
        self.running = False
        self.stop()

    def _close_agg_binary_files(self):
        """Close any lazy-opened AGG binary capture files."""
        for f in self._agg_bin_files.values():
            # Skip the sentinels explicitly. Don't use `not f`: a real file
            # object whose __bool__ returns False (rare, but possible for some
            # IO subclasses) would be incorrectly skipped instead of closed.
            if f is None or f is False:
                continue
            try:
                name = f.name
                f.close()
                print_yellow(f"AGG capture file closed: {name}")
            except Exception as e:
                print_red(f"\nAGG capture file cannot be closed: {e}")
        self._agg_bin_files = {}

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

    def _get_path_tag(self, pathnum: int) -> str:
        """Return the bracketed core-name suffix for `pathnum`, or empty string.

        Lookup order:
          1. If --logAGG names are supplied, use positional index (0x1→0, 0x2→1, 0x4→2).
          2. If --chip is set, use CHIP_TABLE[chip] via get_core_tag, with Generic
             fallback (Core0/Core1/Core2) for legacy/unknown chips or unassigned paths.
        Returns "" when no label is mapped so callers emit a plain timestamp prefix.
        """
        if self.logAGG_srcname:
            idx = {0x1: 0, 0x2: 1, 0x4: 2}.get(pathnum)
            if idx is not None and idx < len(self.logAGG_srcname):
                return f" [{self.logAGG_srcname[idx]}]"
            return ""
        if self.chip:
            return get_core_tag(self.chip, pathnum)
        return ""

    def print(self, text, pathnum: int = 0):
        new_line_char = "\n"
        text = text.replace("\r\n", "\n")
        text = text.replace("\r", "")
        if text and self.timestamps and (self.output_enabled or self.log_file):
            t = datetime.datetime.now().strftime(self.timestamp_format)[:-3]
            # "text" is not guaranteed to be a full line. Timestamps should be only at the beginning of lines.

            if pathnum == 0:
                line_prefix = t + " "
            else:
                line_prefix = t + self._get_path_tag(pathnum)

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
        stacktrace_start_mo = re.search(STACKTRACE_START, line.strip(), flags=re.I)
        stcaktrace_end_mo = re.search(STACKTRACE_END, line.strip(), flags=re.I)
        if stacktrace_start_mo:
            self._stacktrace = True
            return
        if stcaktrace_end_mo:
            self._stacktrace = False
            return

        if self._stacktrace:
        #if True:
            translation = self.address_decoder.decode_address(line)
            if translation:
                self.output_queue.put(translation, False)
                #print_yellow(translation)

    def _open_agg_binary_log(self, prefix: str, suffix: str):
        """Lazy-create a binary-mode capture file in self.log_dir for AGG binary paths."""
        ts = datetime.datetime.now().strftime('%Y_%m_%d_%H%M%S')
        directory = self.log_dir or ""
        # AGG binary capture has no separate enable flag, so when --log-dir is
        # not specified, files land in the CWD. Warn once so users notice.
        if not directory and not self._agg_cwd_warned:
            print_yellow("AGG binary capture: no --log-dir specified, writing to current working directory")
            self._agg_cwd_warned = True
        if directory and not os.path.exists(directory):
            try:
                os.makedirs(directory, exist_ok=True)
            except Exception as e:
                print_red(f"\nCannot create directory {directory}: {e}")
                return None
        name = os.path.join(directory, f"{prefix}_{ts}.{suffix}")
        try:
            f = open(name, "wb")
            print_yellow(f"AGG capture file opened: {name}")
            return f
        except Exception as e:
            print_red(f"\nLog file {name} cannot be created: {e}")
            return None

    def _save_agg_binary(self, path_num: int, prefix: str, suffix: str, payload: bytes):
        """Append an AGG binary path's payload to <prefix>_*.<suffix>.

        One file per path number, lazily opened on first payload. A write/open
        failure latches a False sentinel for that path so subsequent packets
        don't re-error.
        """
        f = self._agg_bin_files.get(path_num)
        if f is None:
            f = self._open_agg_binary_log(prefix, suffix) or False
            self._agg_bin_files[path_num] = f
        if f:
            try:
                f.write(payload)
                f.flush()
            except Exception as e:
                print_red(f"\nCannot write {prefix} log: {e}")
                try:
                    f.close()
                except Exception:
                    pass
                self._agg_bin_files[path_num] = False

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

            # checksum passed -- extract payload once
            payload = frame[2:]

            # 4) Table-driven dispatch via CHIP_TABLE — the single source of
            #    truth for path layout, including binary filename prefixes
            #    (the resolved label) and suffixes (the kind, via bin_suffix).
            #    --chip mode resolves against the selected IC; --logAGG mode has
            #    no chip and resolves against Generic, which defines the same
            #    BT/BT_FW binary paths and Core0/1/2 text paths. NONE paths fall
            #    back to Generic so wrong-IC selections still route data.
            chip = 'Generic' if self.logAGG_srcname else self.chip
            entry = resolve_path(chip, src)
            kind = entry.kind if entry else AggKind.NONE
            if kind == AggKind.TEXT_LOG:
                # In --logAGG mode, text paths are filtered by the bitmask
                # derived from the number of names supplied; the tag itself
                # comes from those positional names (see _get_path_tag).
                if self.logAGG_srcname and (src & self.logAGG_src) == 0:
                    continue
                events.append(("frame", src, payload))
            elif kind in (AggKind.BIN_FILE, AggKind.LOG_FILE):
                self._save_agg_binary(src, entry.label, bin_suffix(kind), payload)
            # AggKind.NONE cannot occur for valid AGG src values 1-5 (Generic
            # defines a kind for each). The 3-bit header src field also admits
            # 0/6/7, which resolve to NONE; such frames are silently dropped
            # because the AGG protocol never emits them in practice.

        return events
