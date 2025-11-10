#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

class ImageInfo():
    def __init__(self, **kwargs):
        self.image_name = kwargs.get("ImageName", "")
        self.start_address = kwargs.get("StartAddress", 0)
        self.end_address = kwargs.get("EndAddress", 0)
        self.memory_type = kwargs.get("MemoryType", 0)
        self.full_erase = kwargs.get("FullErase", False)
        self.mandatory = kwargs.get("Mandatory", False)
        self.description = kwargs.get("Description", "")

    def __repr__(self):
        image_info_dict = {
            "ImageName": self.image_name,
            "StartAddress": self.start_address,
            "EndAddress": self.end_address,
            "MemoryType": self.memory_type,
            "FullErase": self.full_erase,
            "Mandatory": self.mandatory,
            "Description": self.description
        }

        return image_info_dict