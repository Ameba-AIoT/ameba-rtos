#! /usr/bin/env python
# -*- coding: utf-8 -*-

import select
import sys
from queue import Queue

from .console_parser import ConsoleParser
from .constants import CMD_STOP, TAG_CMD
from .stoppable_thread import StoppableThread


class StdinCmdReader(StoppableThread):
    """Read commands from stdin pipe and dispatch parsed events."""

    def __init__(self, event_queue: Queue, cmd_queue: Queue, parser: ConsoleParser):
        super(StdinCmdReader, self).__init__()
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            ready, _, _ = select.select([sys.stdin], [], [], 0.1)
            if not ready:
                continue
            line = sys.stdin.readline()
            if line == "":
                # EOF reached on stdin (pipe closed), stop reading
                break
            ret = self.parser.parse(line)
            if ret is None:
                continue
            tag, cmd = ret
            if tag == TAG_CMD and cmd != CMD_STOP:
                self.cmd_queue.put(ret)
            else:
                self.event_queue.put(ret)

    def _stop(self):
        self.running = False
        self.stop()
