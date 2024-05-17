#!/usr/bin/python
#/** @file
# * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

import sys
import os
from fnmatch import fnmatch
import shutil
import binascii

if (len(sys.argv) != 3):
	print("\nPlease provide following inputs")
	print("\narg1 : <Starting location to find test elfs>")
	print("\narg2 : <Output filename of test combined binary>")
	sys.exit(1)

test_elfs_path = sys.argv[1]
output_file    = sys.argv[2]
file_extension = "*secure.elf"
test_elfs      = []
offset         = 1024

# Gell the list of all test elfs
for path, subdirs, files in os.walk(test_elfs_path):
	for name in files:
		if fnmatch(name, file_extension):
			test_elfs.append(os.path.join(path, name))

# Sort the list as per the components in alphabetical order
if (len(test_elfs) == 0):
	print("[TBSA] Error: No test ELFs found at %s" % (test_elfs_path))
	sys.exit(1)

test_elfs = sorted(test_elfs)

def test_elf_combine():
	"""
	This function goes through the list of available test ELFs
	Creates output binary file

	"""
	elf_num = 0
	while elf_num < len(test_elfs):
		test_id_s  = ''
		test_id_ns = ''

		cur_elf = test_elfs[elf_num]
		# extract test_id from Non-secure file
		with open(cur_elf, mode='rb') as f:
			f.seek(offset)
			test_id_ns = ord(f.read(1))

		cur_elf = test_elfs[elf_num+1]
		# extract test_id from Secure file
		with open(cur_elf, mode='rb') as f:
			f.seek(offset)
			test_id_s = ord(f.read(1))

		# Check for test_id mismatch between Secure and Non-secure file
		if (test_id_ns != test_id_s):
			print("Mismatch in test_id configuration between Secure and Non-secure file respectively for %s and %s" % (test_elfs[elf_num+1], test_elfs[elf_num]))
			sys.exit(1)

		"""
		Create output binary file with the below format

		+---------------+             +----------------+
		| TEST#0 header |-----------> |  START MARKER  |
		|               |             |----------------|
		| Test#0 S ELF  |             |   Test#n ID    |
		|               |             |----------------|
		| Test#0 NS ELF |             | Test#n S size  |
		+---------------+             |----------------|
		| TEST#1 header |             | Test#n NS size |
		|               |             +----------------+
		| Test#1 S ELF  |
		|               |
		| Test#1 NS ELF |
		+---------------+
			|
			.
			|
			.
		+---------------+
		| TEST#n header |
		|               |
		| Test#n S ELF  |
		|               |
		| Test#n NS ELF |
		+---------------+
		|  END MARKER   |
		+---------------+

		"""
		with open(output_file, "ab") as write_file:
			# Print details to console
			print("test_id %04d : %s\t%05d : %s\t%05d" \
                               %(test_id_s, test_elfs[elf_num+1], os.path.getsize(test_elfs[elf_num+1]), test_elfs[elf_num], os.path.getsize(test_elfs[elf_num])))
			# Writing START_MARKER
			start_marker = "FACEFACE"
			start_marker = "".join(reversed([start_marker[i:i+2] for i in range(0, len(start_marker), 2)]))
			write_file.write(binascii.a2b_hex(start_marker))
			# Writing TEST ID
			test_id      = "{:08x}".format(test_id_s)
			test_id      = "".join(reversed([test_id[i:i+2] for i in range(0, len(test_id), 2)]))
			write_file.write(binascii.a2b_hex(test_id))
			# Writing Secure Test ELF Size
			s_file_size  = "{:08x}".format(os.path.getsize(test_elfs[elf_num+1]))
			s_file_size  = "".join(reversed([s_file_size[i:i+2] for i in range(0, len(s_file_size), 2)]))
			write_file.write(binascii.a2b_hex(s_file_size))
			# Writing Non-secure Test ELF Size
			ns_file_size = "{:08x}".format(os.path.getsize(test_elfs[elf_num]))
			ns_file_size = "".join(reversed([ns_file_size[i:i+2] for i in range(0, len(ns_file_size), 2)]))
			write_file.write(binascii.a2b_hex(ns_file_size))
			with open(test_elfs[elf_num+1], "rb") as read_file:
				shutil.copyfileobj(read_file, write_file)
			with open(test_elfs[elf_num], "rb") as read_file:
				shutil.copyfileobj(read_file, write_file)

		elf_num = elf_num + 2

	with open(output_file, "ab") as write_file:
		end_marker = "C3C3C3C3"
		write_file.write(binascii.a2b_hex(end_marker))

# Calling elf_combine method to generate final combined binary of all the available test ELFs
test_elf_combine()
