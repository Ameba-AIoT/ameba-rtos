#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import json
import base64
import tempfile
from pyDes import *

_DES_KEY = "574C414E"  # 0x574C414E, WLAN
_DES_IV = [0x40, 0x52, 0x65, 0x61, 0x6C, 0x73, 0x69, 0x6C]  # @Realsil


class JsonUtils:
    @staticmethod
    def load_from_file(file_path, need_decrypt=True):
        result = None
        if os.path.exists(file_path):
            # utf-8-sig: also accept a file saved with a BOM (e.g. by Windows Notepad)
            with open(file_path, 'r', encoding='utf-8-sig') as f:
                f_content = f.read()

            if need_decrypt:
                k = des(_DES_KEY, CBC, _DES_IV, padmode=PAD_PKCS5)
                des_k = k.decrypt(base64.b64decode(f_content))
                profile_str = des_k.decode("utf-8")
                result = json.loads(profile_str)
            else:
                result = json.loads(f_content)

        return result

    @staticmethod
    def save_to_file(file_path, data, need_encrypt=False):
        path_dir = os.path.dirname(file_path)
        os.makedirs(path_dir, exist_ok=True)
        if need_encrypt:
            ek = des(_DES_KEY.encode("utf-8"), CBC, _DES_IV, pad=None, padmode=PAD_PKCS5)
            en_bytes = ek.encrypt(json.dumps(data).encode("utf-8"))
            save_bytes = base64.b64encode(en_bytes)
        else:
            save_bytes = json.dumps(data, indent=2).encode("utf-8")

        # Write a temp file and rename it over the target: another process (e.g. a
        # parallel CLI run) never reads a truncated or half written file
        fd, tmp_path = tempfile.mkstemp(dir=path_dir, prefix=os.path.basename(file_path) + ".", suffix=".tmp")
        try:
            with os.fdopen(fd, "wb") as json_file:
                json_file.write(save_bytes)
            os.replace(tmp_path, file_path)
        except BaseException:
            try:
                os.remove(tmp_path)
            except OSError:
                pass
            raise
