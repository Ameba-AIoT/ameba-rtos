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


class ConsoleReader(StoppableThread):
    """
    Read console input and push to the event queue, until stopped.
    """

    def __init__(self, console: Console, event_queue: Queue, cmd_queue: Queue, parser: ConsoleParser):
        super(ConsoleReader, self).__init__()
        self.console = console
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser

    def run(self):
        self.console.setup()
        if sys.platform == 'win32':
            # Miniterm is setting the code page to UTF-8,
            # but if we input multi-bytes characters, os will kill the terminal
            # so we set input cp to windows origin codepage
            import ctypes
            # output from scripts, we set utf-8
            # ctypes.windll.kernel32.SetConsoleOutputCP(self.console._saved_ocp)
            # input from human input, we use cmd ori setting
            ctypes.windll.kernel32.SetConsoleCP(self.console._saved_icp)
        else:
            # Use non-blocking busy read to avoid using unsecure TIOCSTI from console.cancel().
            # TIOCSTI is not supported on kernels newer than 6.2.
            import termios
            new = termios.tcgetattr(self.console.fd)
            # new[6] - 'cc': a list of the tty special characters
            new[6][termios.VMIN] = 0  # minimum bytes to read
            new[6][termios.VTIME] = 2  # timer of 0.1 second granularity
            termios.tcsetattr(self.console.fd, termios.TCSANOW, new)
        try:
            while self.alive:
                try:
                    if os.name == 'nt':
                        # console.getkey() will block, so we need to block before it and check self.alive.
                        # And then the program has time to handle commands.
                        import msvcrt
                        while not msvcrt.kbhit() and self.alive:
                            time.sleep(0.1)
                        if not self.alive:
                            break
                    c = self.console.getkey()
                except KeyboardInterrupt:
                    c = '\x03'
                if c:
                    ret = self.parser.parse(c)
                    if ret is not None:
                        (tag, cmd) = ret
                        # stop command should be executed last
                        if tag == TAG_CMD and cmd != CMD_STOP:
                            self.cmd_queue.put(ret)
                        else:
                            self.event_queue.put(ret)
        finally:
            self.console.cleanup()
