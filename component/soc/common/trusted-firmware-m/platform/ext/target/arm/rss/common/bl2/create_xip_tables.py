#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import hashlib
from functools import reduce
from operator import add
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import struct
import secrets

sic_page_size = 1024

def struct_pack(objects, pad_to=0):
    defstring = "<"
    for obj in objects:
        defstring += str(len(obj)) + "s"

    size = struct.calcsize(defstring)
    if size < pad_to:
        defstring += str(pad_to - size) + "x"

    return (bytes(struct.pack(defstring, *objects)))

def chunk_bytes(x, n):
    return [x[i:i+n] for i in range(0, len(x), n)]

parser = argparse.ArgumentParser()
parser.add_argument("--input_image", help="the image to create table from", required=True)
parser.add_argument("--encrypt_key_file", help="Key to encrypt image with", required=False)
parser.add_argument("--image_version", help="Version of the image", required=True)
parser.add_argument("--table_output_file", help="table output file", required=True)
parser.add_argument("--encrypted_image_output_file", help="encrupted image output file", required=True)
args = parser.parse_args()

with open(args.input_image, "rb") as in_file:
    image = in_file.read()

# TODO derive a key from the main key based on the security counter / fw
# version, as we do for BL2. This requires tweaks to crypto in BL2 first.
if args.encrypt_key_file is not None:
    with open(args.encrypt_key, "rb") as in_file:
        encrypt_key = in_file.read()
else:
    encrypt_key = bytearray([0xfc, 0x57, 0x01, 0xdc, 0x61, 0x35, 0xe1, 0x32,
                             0x38, 0x47, 0xbd, 0xc4, 0x0f, 0x04, 0xd2, 0xe5,
                             0xbe, 0xe5, 0x83, 0x3b, 0x23, 0xc2, 0x9f, 0x93,
                             0x59, 0x3d, 0x00, 0x01, 0x8c, 0xfa, 0x99, 0x94,])


fw_version_bytes = int(args.image_version, 0).to_bytes(4, 'little')
nonce_bytes = secrets.token_bytes(8)

# Provided the image starts at the beginning of the DR, we can start the line
# index at 0.
counter_val = struct_pack([nonce_bytes, fw_version_bytes], pad_to=16)

cipher = Cipher(algorithms.AES(encrypt_key), modes.CTR(counter_val))
enc_image = cipher.encryptor().update(image)

htr = reduce(add, map(lambda x:hashlib.sha256(x).digest(), chunk_bytes(enc_image, sic_page_size)), b"")

table = struct_pack([
            fw_version_bytes,
            nonce_bytes,
            len(htr).to_bytes(4, 'little'),
            htr
            ])

with open(args.table_output_file, "wb") as out_file:
    out_file.write(table)

with open(args.encrypted_image_output_file, "wb") as out_file:
    out_file.write(enc_image)
