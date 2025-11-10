#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


def divide_then_round_up(dividend, divisor):
    return int((dividend + divisor - 1) / divisor)
