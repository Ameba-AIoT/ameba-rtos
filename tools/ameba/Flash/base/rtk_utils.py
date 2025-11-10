#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys


class RtkUtils:
    @staticmethod
    def get_executable_root_path():
        if getattr(sys, 'frozen', False):  # judge if frozen as exe
            # get exe dir
            executable_root = os.path.dirname(os.path.abspath(sys.executable))
        else:
            # get py dir
            executable_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

        return executable_root