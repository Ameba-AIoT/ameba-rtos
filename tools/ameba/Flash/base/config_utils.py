#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os


class ConfigUtils:
    @staticmethod
    def get_key_value_pairs(logger, file_path):
        result = []
        with open(file_path, 'r') as file:
            for line in file:
                line = line.strip()
                if not line:
                    continue

                parts = line.split("=", 1)
                if len(parts) == 2:
                    key, value = parts
                    try:
                        result.append({key: int(value)})
                    except ValueError:
                        logger.warning(f"Skipping line with non-integer value: {line}")
                else:
                    logger.warning(f"Skipping improperly formatted line: {line}")

        return result