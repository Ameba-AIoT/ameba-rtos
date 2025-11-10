#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

class EfuseData():
    def __init__(self, **kwargs):
        self.length = 16

        self.offset = kwargs.get("Offset", 0)
        self.value = kwargs.get("Value", [0] * self.length)

    def __repr__(self):
        efuse_data_dict = {
            "Value": self.value,
            "Offset": self.offset
        }

        return efuse_data_dict