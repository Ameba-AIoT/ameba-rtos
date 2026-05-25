#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from colorama import Fore
import sys


_DIAG_TO_STDERR = False


def set_diag_to_stderr(enabled: bool):
    global _DIAG_TO_STDERR
    _DIAG_TO_STDERR = enabled


def _diag_stream():
    return sys.stderr if _DIAG_TO_STDERR else sys.__stdout__


def color_print(text, color: Fore, end="\n"):
    print(f"{color}{text}{Fore.RESET}", end=end, file=_diag_stream())


def print_normal(text, end="\n"):
    print(text, end=end, file=_diag_stream())


def print_yellow(text, end="\n"):
    color_print(text, Fore.YELLOW, end)


def print_red(text, end="\n"):
    color_print(text, Fore.RED, end)


def print_green(text, end="\n"):
    color_print(text, Fore.GREEN, end)


def print_cyan(text, end="\n"):
    color_print(text, Fore.CYAN, end)
