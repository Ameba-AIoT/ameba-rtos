#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys

try:
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
    from cryptography.hazmat.backends import default_backend
    _CRYPTO_AVAILABLE = True
except ImportError:
    _CRYPTO_AVAILABLE = False


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

    @staticmethod
    def aes_ecb_encrypt(key: bytes, plaintext: bytes) -> bytes:
        """Encrypt plaintext with AES-ECB.

        Used by model-unique (一型一密) key programming: host computes
        EncData = AES_ECB(KeyData, RawData) to let the device verify the written key.
        plaintext must be a multiple of 16 bytes; key must be 16 or 32 bytes.
        """
        if not _CRYPTO_AVAILABLE:
            raise RuntimeError("cryptography package is required for AES-ECB key verification. "
                               "Install with: pip install cryptography")
        if len(key) not in (16, 32):
            raise ValueError(f"AES key length must be 16 or 32 bytes, got {len(key)}")
        if len(plaintext) == 0 or len(plaintext) % 16 != 0:
            raise ValueError(f"Plaintext length must be a non-zero multiple of 16, got {len(plaintext)}")
        cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
        enc = cipher.encryptor()
        return enc.update(plaintext) + enc.finalize()