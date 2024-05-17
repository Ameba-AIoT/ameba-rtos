#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import ecdsa

parser = argparse.ArgumentParser()
parser.add_argument("--input_key_file", help="the input key in binary format", required=True)
parser.add_argument("--output_key_file", help="the file to output the pem key to", required=True)
args = parser.parse_args()

with open(args.input_key_file, "rb") as in_file:
    input_key = in_file.read()

# Remove fixed 0x04 byte from the start of the key before processing
key = ecdsa.VerifyingKey.from_string(input_key[1:], curve=ecdsa.NIST384p)
output_key = key.to_pem()

with open(args.output_key_file, "wb") as out_file:
    out_file.write(output_key)
