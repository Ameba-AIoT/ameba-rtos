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
from prompt_toolkit import PromptSession
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit import Application
from prompt_toolkit.layout import Layout, HSplit
from prompt_toolkit.widgets import TextArea
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.patch_stdout import patch_stdout

command_list = ['exit']
completer = WordCompleter(command_list, ignore_case=True, sentence=True, match_middle=False)
session = PromptSession(completer=completer)

def console_update_session(commands):
    command_list.extend(commands)
    completer = WordCompleter(command_list, ignore_case=True, sentence=True, match_middle=False)
    session.completer = completer

class ConsoleReader(StoppableThread):
    """
    Read console input and push to the event queue, until stopped.
    """

    def __init__(self, event_queue: Queue, cmd_queue: Queue, parser: ConsoleParser):
        super(ConsoleReader, self).__init__()
        self.event_queue = event_queue
        self.cmd_queue = cmd_queue
        self.parser = parser
        self.running = False
        self.command_history = []
        self.history_index = [0]

        self.input_field = TextArea(height=1, prompt='> ', style='class:input-field', completer=completer, multiline=False)
        self.input_field.accept_handler = self._handle_command

        self.layout = Layout(HSplit([self.input_field]), focused_element=self.input_field)

        self.kb = KeyBindings()
        @self.kb.add('c-c')
        def _(event):
            self.running = False
            event.app.exit()

        @self.kb.add('up')
        def _(event):
            if self.command_history and self.history_index[0] > 0:
                self.history_index[0] -= 1
                self.input_field.text = self.command_history[self.history_index[0]]
                self.input_field.buffer.cursor_position = len(self.input_field.text)

        @self.kb.add('down')
        def _(event):
            if self.command_history and self.history_index[0] < len(self.command_history)-1:
                self.history_index[0] += 1
                self.input_field.text = self.command_history[self.history_index[0]]
                self.input_field.buffer.cursor_position = len(self.input_field.text)
            else:
                self.history_index[0] = len(self.command_history)
                self.input_field.text = ''
                self.input_field.buffer.cursor_position = 0

        self.app = Application(
            layout=self.layout,
            key_bindings=self.kb,
            full_screen=False
        )

    def _handle_command(self, buff):
        c = self.input_field.text.strip()
        self.input_field.text = ""

        if c == 'exit':
            self.running = False
            self.app.exit()
            return
        else:
            if c != "":
                if c in self.command_history:
                    self.command_history.remove(c)
                self.command_history.append(c)
                self.history_index[0] = len(self.command_history)
            c = c + "\r\n"
            ret = self.parser.parse(c)
            if ret is not None:
                (tag, cmd) = ret
                # stop command should be executed last
                if tag == TAG_CMD and cmd != CMD_STOP:
                    self.cmd_queue.put(ret)
                else:
                    self.event_queue.put(ret)

    def run(self):
        self.running = True
        with patch_stdout():
            try:
                self.app.run()
            except (EOFError, KeyboardInterrupt):
                self.event_queue.put((TAG_CMD, CMD_STOP))
            finally:
                print('Exit command session2')

    def _stop(self):
        print('Exit command session1')
        self.running = False
        if self.app.is_running:
            self.app.exit()
            time.sleep(0.5)
        self.stop()
