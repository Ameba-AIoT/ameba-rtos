#!/usr/bin/python
#/** @file
# * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
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

if (len(sys.argv) != 14):
        print("\nScript requires following inputs")
        print("\narg1  : <INPUT  SUITE identifier>")
        print("\narg2  : <INPUT  testsuite.db file>")
        print("\narg3  : <INPUT  panic test>")
        print("\narg4  : <INPUT  tests coverage>")
        print("\narg5  : <OUTPUT testlist file>")
        print("\narg6  : <OUTPUT test_entry_list>")
        print("\narg7  : <OUTPUT test_entry_fn_declare_list>")
        print("\narg8  : <OUTPUT client_tests_list_declare>")
        print("\narg9  : <OUTPUT client_tests_list>")
        print("\narg10  : <OUTPUT server_tests_list_declare>")
        print("\narg11 : <OUTPUT server_tests_list>")
        print("\narg12 : <OUTPUT Suite test start number")
        print("\narg13 : <OUTPUT Suite test end number")
        sys.exit(1)

suite                      = sys.argv[1]
testsuite_db_file          = sys.argv[2]
panic_tests_included       = int(sys.argv[3])
tests_coverage             = sys.argv[4]
testlist_file              = sys.argv[5]
test_entry_list            = sys.argv[6]
test_entry_fn_declare_list = sys.argv[7]
client_tests_list_declare  = sys.argv[8]
client_tests_list          = sys.argv[9]
server_tests_list_declare  = sys.argv[10]
server_tests_list          = sys.argv[11]


if sys.argv[12] != "None":
	suite_test_start_number    = int(sys.argv[12])
else:
	suite_test_start_number    = 0
if sys.argv[13] != "None":
	suite_test_end_number      = int(sys.argv[13])
else:
	suite_test_end_number      = sys.maxsize

# Dictionary to hold the mapping between suite and the base number
suite_with_base_dict = {"ipc":0, "crypto":1, "internal_trusted_storage":2, "protected_storage":2, "storage":2, "initial_attestation":3}

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
					if (panic_tests_included == 0):
						if (tests_coverage == "ALL"):
							if (("panic" not in line) and ("failing" not in line)):
								o_f.write(line)
							if (("panic" not in line) and ("failing" in line)):
								o_f.write(line[0:line.find(',')]+'\n')
						if (tests_coverage == "PASS"):
							if (("panic" not in line) and ("failing" not in line)):
								o_f.write(line)
					if (panic_tests_included == 1):
						if (tests_coverage == "ALL"):
							if (("panic" not in line) and ("failing" not in line)):
								o_f.write(line)
							else:
								o_f.write(line[0:line.find(',')]+'\n')
						if (tests_coverage == "PASS"):
							if (("panic" not in line) and ("failing" not in line)):
								o_f.write(line)
							if (("panic" in line) and ("failing" not in line)):
								o_f.write(line[0:line.find(',')]+'\n')

def gen_test_entry_info():
	"""
	Generate Non-secure related inc files
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
			o_f2.write("void test_entry_%s(val_api_t *val_api, psa_api_t *psa_api);\n" %(uniq_test_string))
	print("Non-secure test entry symbol list:\n\t%s,\n\t%s" %(test_entry_list, test_entry_fn_declare_list))

def gen_secure_tests_list():
	"""
	Generate partition related inc files
	"""
	test_num = 0
	test_num_prev = 0

	with open(testlist_file, mode='r') as i_f, \
	     open(client_tests_list_declare, mode='w') as o_f1, \
	     open(client_tests_list, mode='w') as o_f2, \
	     open(server_tests_list_declare, mode='w') as o_f3, \
	     open(server_tests_list, mode='w') as o_f4:
		for line in i_f:
			line = line.strip()
			o_f1.write("extern client_test_t %s_client_tests_list[];\n" %(line))
			o_f3.write("extern server_test_t %s_server_tests_list[];\n" %(line))

			test_num = int(line[6:9])
			if ((test_num - test_num_prev) > 1):
				for num in range(test_num_prev, test_num-1):
					o_f2.write("\tNULL,\n")
					o_f4.write("\tNULL,\n")
			o_f2.write("\t%s_client_tests_list,\n" %(line[0:9]));
			o_f4.write("\t%s_server_tests_list,\n" %(line[0:9]));

			test_num_prev = test_num

	print("Secure test entry symbol list:\n\t%s,\n\t%s,\n\t%s,\n\t%s" \
               %(client_tests_list_declare, client_tests_list, \
                 server_tests_list_declare, server_tests_list))

# Call routines
gen_test_list()
gen_test_entry_info()
if (suite == "ipc"):
	gen_secure_tests_list()
