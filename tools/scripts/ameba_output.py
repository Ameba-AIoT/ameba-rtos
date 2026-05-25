#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import re
import subprocess
import sys
from enum import IntEnum
from typing import List, Optional


__all__ = [
    'BuildFilter',
    'OutputMode',
    'get_output_mode',
    'is_quiet',
    'print_quiet_summary',
    'run_filtered',
]


class OutputMode(IntEnum):
    NORMAL = 0
    QUIET  = 1


def get_output_mode() -> OutputMode:
    if '-q' in sys.argv or '--quiet' in sys.argv:
        return OutputMode.QUIET
    return OutputMode.NORMAL


def is_quiet() -> bool:
    return get_output_mode() == OutputMode.QUIET


_NINJA_PROGRESS_RE = re.compile(r'^\[\d+/\d+\]')
_ANSI_RE = re.compile(r'\x1b\[[0-9;]*[mK]')


class BuildFilter:
    """
    Stateful line filter for quiet-mode builds.

    Keeps:
      - [INFOR] lines (CMake structural info)
      - Any compile step that emits a warning (the [N/M] progress line plus
        every line up to the next progress / FAILED / build-stop sentinel)
      - The [N/M] progress line that preceded a FAILED: target
      - Everything from FAILED: until the next [N/M] (compiler cmd + error output)
      - "ninja: build stopped:" terminator line

    Suppresses everything else (configure boilerplate, clean compile lines,
    size tables, image analysis, etc.).
    """

    def __init__(self, mode: OutputMode = OutputMode.NORMAL):
        self._mode = mode
        self._in_failed: bool = False
        self._pending_progress: Optional[str] = None
        self._pending_buffer: List[str] = []
        self._pending_has_warning: bool = False
        self.errors: List[str] = []
        self.warnings: List[str] = []

    def _flush_pending_if_warning(self) -> None:
        """Emit the saved progress line + buffered output iff a warning was seen."""
        if self._pending_progress is not None and self._pending_has_warning:
            print(self._pending_progress, flush=True)
            for buf_line in self._pending_buffer:
                print(buf_line, flush=True)
        self._pending_progress = None
        self._pending_buffer = []
        self._pending_has_warning = False

    def feed(self, line: str) -> None:
        if self._mode != OutputMode.QUIET:
            print(line, flush=True)
            return

        # Strip ANSI escapes for pattern matching; keep originals for output
        # so colored [INFOR]/FAILED lines stay colored on the terminal.
        plain = _ANSI_RE.sub('', line)

        # Always show CMake [INFOR] lines.
        if plain.startswith('[INFOR]'):
            print(line, flush=True)
            return

        # Ninja progress: [N/M] task_description
        # The previous step is now finished — flush its buffer if it warned.
        if _NINJA_PROGRESS_RE.match(plain):
            self._flush_pending_if_warning()
            self._in_failed = False
            self._pending_progress = line
            return

        # FAILED: target — start of a failure block.
        if plain.startswith('FAILED:'):
            self._in_failed = True
            if self._pending_progress is not None:
                print(self._pending_progress, flush=True)
                self._pending_progress = None
            # Lines buffered between progress and FAILED also belong to the failure.
            for buf_line in self._pending_buffer:
                print(buf_line, flush=True)
            self._pending_buffer = []
            self._pending_has_warning = False
            print(line, flush=True)
            return

        # Inside a failure block: print everything and collect diagnostics.
        if self._in_failed:
            print(line, flush=True)
            if ': error:' in plain:
                self.errors.append(plain)
            elif ': warning:' in plain:
                self.warnings.append(plain)
            return

        # Build-stop sentinel.
        if 'ninja: build stopped:' in plain:
            self._flush_pending_if_warning()
            print(line, flush=True)
            return

        # Anything else: buffer against the pending progress step. If this step
        # later turns out to have warned, the whole buffer is flushed; otherwise
        # it's dropped at the next progress line.
        if self._pending_progress is not None:
            self._pending_buffer.append(line)
            if ': warning:' in plain:
                self._pending_has_warning = True
                self.warnings.append(plain)

    def flush(self) -> None:
        """Flush any pending buffered step (call at end of stream)."""
        self._flush_pending_if_warning()


def run_filtered(cmd, filt: BuildFilter, shell: bool = True) -> int:
    """Run *cmd* and pipe its combined stdout+stderr through *filt*."""
    proc = subprocess.Popen(
        cmd, shell=shell,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    )
    for raw in iter(proc.stdout.readline, b''):
        text = raw.decode('utf-8', errors='replace').rstrip('\n')
        # Ninja overwrites the progress line using \r; split to process each segment.
        for seg in text.split('\r'):
            if seg:
                filt.feed(seg)
    proc.wait()
    filt.flush()
    return proc.returncode


def print_quiet_summary(
    filt: BuildFilter,
    final_image_dir: Optional[str],
    build_dir: Optional[str],
    duration_s: float,
    success: bool,
) -> None:
    """Print the one-line build outcome in quiet mode (called via atexit)."""
    if not success:
        return  # Inline error output was already shown by run_filtered.

    if filt.warnings:
        print('\033[32mBuild done\033[0m\033[33m (with warnings)\033[0m', flush=True)
    else:
        print('\033[32mBuild done\033[0m', flush=True)
