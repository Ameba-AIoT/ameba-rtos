#!/usr/bin/python
#/** @file
# * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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
import re
import shutil
from argparse import ArgumentParser

def update_manifest_file(manifest_dir_path):
	"""
	- Remove heap_size field from manifest if dynamic memory function is not supported.
	"""
	file_list =	[manifest_dir_path+"/driver_partition_psa.json",
				manifest_dir_path+"/client_partition_psa.json",
				manifest_dir_path+"/server_partition_psa.json"]

	for file in file_list:
		print("Updating "+file+" manifest file")
		f_i = open(file, 'r')
		f_o = open(file+".update", 'w')
		f1 = f_i.readlines()
		for line in f1:
				if (re.findall('heap_size', line)):
					continue
				f_o.write(line)
		f_i.close()
		f_o.close()
		shutil.move(file+".update", file)

def argparse():
	"""
	Parse the command line argument
	"""
	parser = ArgumentParser(description='Utility to remove heap_size field from manifest files. \
							Heap_size field is optional feature and use this script to remove it\
							when dynamic memory function are not available to secure partition.')
	options = parser.parse_args()

	return options

#main code starts here
if __name__ == "__main__":
	manifest_dir_path =	"platform/manifests"
	options = argparse()
	update_manifest_file(manifest_dir_path)
