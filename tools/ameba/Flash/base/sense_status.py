#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from .errno import *


class SenseStatus:
    def __init__(self):
        self.op_code = None
        self.status = None
        self.data = None

    def parse(self, data, offset):
        ret = ErrType.SYS_OVERRANGE
        if len(data) >= offset + 6:
            self.op_code = data[offset]
            self.status = data[offset + 1]
            self.data = data[offset+2] + (data[offset+3]<<8) + (data[offset+4]<<16) + (data[offset+5]<<24)
            ret = ErrType.OK

        return ret