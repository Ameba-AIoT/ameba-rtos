#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

class Version:
    def __init__(self, version_str):
        # parser version_str with pattern major.minor.micro
        parts = version_str.split(".")
        self.major = int(parts[0])
        self.minor = int(parts[1] if len(parts) > 1 else 0)
        self.micro = int(parts[2] if len(parts) > 2 else 0)

    def __repr__(self):
        return f"{self.major}.{self.minor}.{self.micro}"
