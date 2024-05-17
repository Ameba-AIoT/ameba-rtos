#!/usr/bin/python
#/** @file
# * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

if (len(sys.argv) != 8):
        print("\nScript requires following inputs")
        print("\narg1  : <INPUT  SUITE identifier>")
        print("\narg2  : <INPUT  testsuite.db file>")
        print("\narg3  : <OUTPUT testlist file>")
        print("\narg4  : <OUTPUT test_entry_list>")
        print("\narg5  : <OUTPUT test_entry_fn_declare_list>")
        print("\narg6  : <OUTPUT Suite test start number")
        print("\narg7  : <OUTPUT Suite test end number")
        sys.exit(1)

suite                      = sys.argv[1]
testsuite_db_file          = sys.argv[2]
testlist_file              = sys.argv[3]
test_entry_list            = sys.argv[4]
test_entry_fn_declare_list = sys.argv[5]
if sys.argv[6] != "None":
	suite_test_start_number    = int(sys.argv[6])
else:
	suite_test_start_number    = 0
if sys.argv[7] != "None":
	suite_test_end_number      = int(sys.argv[7])
else:
	suite_test_end_number      = sys.maxsize

# Dictionary to hold the mapping between suite and the base number
suite_with_base_dict = {"adac":4}

def gen_test_list():
	"""
	Read the input testsuite.db file and generates the output file with list of tests
	"""
	with open(testlist_file, mode='w') as o_f:
		with open(testsuite_db_file, mode='r') as i_f:
			for line in i_f:
				if (('test_' == line[0:5]) and
				    (int(line[6:9]) >= suite_test_start_number) and
				    (int(line[6:9]) <= suite_test_end_number)):
						o_f.write(line)

def gen_test_entry_info():
	"""
	Generate inc files
	"""
	test_num           = 0
	uniq_test_string   = ''
	max_test_per_suite = 200
	suite_base         = 0

	if (suite not in suite_with_base_dict.keys()):
		print("\nProvide a valid SUITE identifier")
		sys.exit()

	with open(test_entry_list, mode='w') as o_f1, \
             open(test_entry_fn_declare_list, mode='w') as o_f2,\
             open(testlist_file, mode='r') as i_f:
		for line in i_f:
			line = line.strip()
			test_num = int(line[6:9]) + (max_test_per_suite * suite_with_base_dict[suite])
			uniq_test_string = line[5:9]
			o_f1.write("\t{%d, &test_entry_%s},\n" %(test_num, uniq_test_string))
			o_f2.write("void test_entry_%s(val_api_t *val_api);\n" %(uniq_test_string))
	print("Test entry symbol list:\n\t%s,\n\t%s" %(test_entry_list, test_entry_fn_declare_list))

# Call routines
gen_test_list()
gen_test_entry_info()
