#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import time
from serial.tools.miniterm import Console
from queue import Queue

from .console_parser import ConsoleParser
from .constants import CMD_STOP, TAG_CMD
from .stoppable_thread import StoppableThread

# Import different terminal control modules for different operating systems
try:
    import termios
    is_posix = True
except ImportError:
    import msvcrt
    is_posix = False

class ConsoleReader(StoppableThread):
    """
    Read console input and push to the event queue, until stopped.
    """

    def __init__(self, event_queue: Queue, cmd_queue: Queue, parser: ConsoleParser):
        super(ConsoleReader, self).__init__()
        if is_posix:
            self.console = sys.stdin
            self.fd = self.console.fileno()
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser
        self.running = False

    def _disable_echo(self):
        """Disable terminal echo"""
        if is_posix:
            # Save current terminal settings
            self.old_settings = termios.tcgetattr(self.fd)
            new_settings = termios.tcgetattr(self.fd)
            # disable echo
            new_settings[3] &= ~(termios.ICANON | termios.ECHO)
            # tty.setraw(self.fd)
            termios.tcsetattr(self.fd, termios.TCSADRAIN, new_settings)
        # msvcrt.getch() in Windows doesn't echo by default, no extra settings needed

    def _restore_terminal(self):
        """Restore original terminal settings"""
        if is_posix and self.old_settings:
            termios.tcsetattr(self.fd, termios.TCSADRAIN, self.old_settings)

    def run(self):
        self._disable_echo()
        self.running = True
        try:
            while self.running:
                if is_posix:
                    c = self.console.read(1)
                else:
                    c = msvcrt.getch().decode('utf-8', errors='ignore')

                ret = self.parser.parse(c)
                if ret is not None:
                    (tag, cmd) = ret
                    # stop command should be executed last
                    if tag == TAG_CMD and cmd != CMD_STOP:
                        self.cmd_queue.put(ret)
                    else:
                        self.event_queue.put(ret)
        finally:
            # Ensure terminal settings are restored even if exception occurs
            self._restore_terminal()
            pass

    def _stop(self):
        print("Restore terminal...")
        self._restore_terminal()
        self.running = False
        self.stop()
