#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import queue
import textwrap
from serial.tools import miniterm

from . import __version__
from .constants import (CMD_OUTPUT_TOGGLE, CMD_STOP, CMD_TOGGLE_LOGGING, CMD_TOGGLE_TIMESTAMPS, CTRL_H, TAG_CMD,
                        TAG_KEY)
from .key_config import (COMMAND_KEYS, EXIT_KEY, EXIT_MENU_KEY, MENU_KEY, TOGGLE_LOG_KEY, TOGGLE_OUTPUT_KEY,
                         TOGGLE_TIMESTAMPS_KEY)
from .color_output import print_red

key_description = miniterm.key_description


class ConsoleParser(object):

    def __init__(self, eol='CRLF'):  # type: (str) -> None
        self.translate_eol = {
            'CRLF': lambda c: c.replace('\n', '\r\n'),
            'CR': lambda c: c.replace('\n', '\r'),
            'LF': lambda c: c.replace('\r', '\n'),
        }[eol]
        self._pressed_menu_key = False

    def parse(self, key):
        """
        Parse the key and return the tag and the key.
        :param key: the key to parse
        :return: the tag and the key
        """
        ret = None
        if self._pressed_menu_key or key in COMMAND_KEYS:
            ret = self._handle_menu_key(key)
        elif key == MENU_KEY:
            self._pressed_menu_key = True
        elif key == EXIT_KEY:
            ret = (TAG_CMD, CMD_STOP)
        else:
            key = self.translate_eol(key)
            ret = (TAG_KEY, key)
        return ret

    def _handle_menu_key(self, c):
        """
        Handle menu key.
        :param c: the key to handle
        :return: the tag and the key
        """
        ret = None
        if c in [EXIT_KEY, MENU_KEY]:  # send key to IC
            ret = (TAG_KEY, c)
        elif c in [CTRL_H, 'h', 'H']:  # get help text
            print_red(self.get_help_text())
        elif c == TOGGLE_OUTPUT_KEY:  # toggle output
            ret = (TAG_CMD, CMD_OUTPUT_TOGGLE)
        elif c == TOGGLE_LOG_KEY:  # toggle logging
            ret = (TAG_CMD, CMD_TOGGLE_LOGGING)
        elif c in [TOGGLE_TIMESTAMPS_KEY, 'i', 'I']:  # toggle timestamps
            ret = (TAG_CMD, CMD_TOGGLE_TIMESTAMPS)
        elif c in [EXIT_MENU_KEY, 'x', 'X']:  # exit program
            ret = (TAG_CMD, CMD_STOP)
        else:
            print_red(f'--- unknown menu character {key_description(c)} ---')
        self._pressed_menu_key = False
        return ret

    def get_help_text(self):
        """
        Get the help text.
        :return: the help text
        """
        text = f"""\
            --- monitor ({__version__}) - REAL CONNECT Monitor tool based on miniterm from pySerial
            ---
            --- {key_description(EXIT_KEY):8} Exit program
            --- {key_description(MENU_KEY):8} Menu escape key, followed by:
            --- Menu keys:
            ---    {key_description(MENU_KEY):14} Send the menu character to IC
            ---    {key_description(EXIT_KEY):14} Send the exit character to IC
            ---    {key_description(TOGGLE_OUTPUT_KEY):14} Toggle output display
            ---    {key_description(TOGGLE_LOG_KEY):14} Toggle saving output into file
            ---    {key_description(TOGGLE_TIMESTAMPS_KEY) + ' (or I)':14} Toggle printing timestamps
            ---    {key_description(EXIT_MENU_KEY) + ' (or X)':14} Exit program"""

        return textwrap.dedent(text)
