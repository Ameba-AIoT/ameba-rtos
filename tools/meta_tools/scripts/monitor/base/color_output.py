#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from colorama import Fore


def color_print(text, color: Fore, end="\n"):
    print(f"{color}{text}{Fore.RESET}", end=end)


def print_normal(text, end="\n"):
    print(text, end=end)


def print_yellow(text, end="\n"):
    color_print(text, Fore.YELLOW, end)


def print_red(text, end="\n"):
    color_print(text, Fore.RED, end)


def print_green(text, end="\n"):
    color_print(text, Fore.GREEN, end)


def print_cyan(text, end="\n"):
    color_print(text, Fore.CYAN, end)
