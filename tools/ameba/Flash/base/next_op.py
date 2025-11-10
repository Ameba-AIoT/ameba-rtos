#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from enum import Enum


class NextOpType(Enum):
    NONE = 0  # Do nothing, hold on in download mode
    INDICATION = 1  # Indicate download result via PWM/GPIO
    RESET = 2  # Exit download mode
    BOOT = 3  # Jump to Ram
    REBURN = 4  # Reset into download mode again