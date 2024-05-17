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

if (len(sys.argv) != 8):
	print("\nPlease provide following inputs")
	print("\narg1 : target name")
	print("\narg2 : val_target.h")
	print("\narg3 : tbsa_tgt.cfg")
	print("\narg4 : intermediate source file")
	print("\narg5 : output database file\n")
	print("\narg6 : name for table\n")
	print("\narg7 : section name for database table\n")
	sys.exit(1)

target       = sys.argv[1]
in_val_tgt   = sys.argv[2]
in_tbsa_tgt  = sys.argv[3]
out_source   = sys.argv[4]
out_database = sys.argv[5]
table_name   = sys.argv[6]
section_name = sys.argv[7]

minor_major_map = {}
unique_major_groups = []
unique_minor_components = []

if section_name == "NOSECTION":
	section_name = ""

def get_minor_major_map():
	""" This method populates the dictionary which maps between every available minor component to their major group """
	try:
		with open(in_val_tgt, mode="r") as f:
			for line in f:
				if "GROUP_" in line:
					temp_list = line.split()
					if "=" in temp_list[1] and "GROUP_" in temp_list[2]:
						minor_major_map.update({temp_list[0]:temp_list[2][6:temp_list[2].find(',')]})
						if minor_major_map[temp_list[0]] not in unique_major_groups:
							unique_major_groups.append(minor_major_map[temp_list[0]])
	except:
		print("Cannot open %s" %in_val_tgt)

def generate_source():
	"""" This method generates the source file which gets compiled on host machine.
		The compiled output then generates the database file for a given target """
	try:
		with open(out_source, mode="a") as o_f:
			o_f.write("#include <stdio.h>\n")
			o_f.write("#include \"val_target.h\"\n\n")
			o_f.write("int main (void)\n")
			o_f.write("{\n")
			try:
				with open(in_tbsa_tgt, mode="r") as i_f:
					num = ''
					for line in i_f:
						if "/" in line[0:1]:
							""" Ignoring the commented lines """
							pass
						elif (".num" in line) and (line[line.find('.')+1:line.find('.')+4] == "num"):
							""" Pick the lines which tells the total instances of a component """
							minor_comp = line[:line.find('.')]
							num        = "".join(line[line.find('=')+1:line.find(';')].replace(" ",""))
							if (int(num)) != 0:
								o_f.write("\t%s_desc_t %s[%s] = {0};\n" %(minor_major_map[minor_comp.upper()].lower(), minor_comp, num))
								o_f.write("\tint %s_num_instances = %s;\n" %(minor_comp, num))
								num = int(num)
								for instance in range(num):
									o_f.write("\t%s[%d].cfg_type.cfg_id = ((GROUP_%s << 24) + (%s_%s << 16) + %d);\n" \
										%(minor_comp, instance, minor_major_map[minor_comp.upper()],\
											minor_major_map[minor_comp.upper()], minor_comp.upper(), instance))
									o_f.write("\t%s[%d].cfg_type.size   = sizeof(%s)/%s_num_instances;\n" %(minor_comp, instance, minor_comp, minor_comp))
									o_f.write("\t%s[%d].cfg_type.size  |= (%s_num_instances << 24);\n" %(minor_comp, instance, minor_comp))
								unique_minor_components.append(minor_comp.upper())
						elif ("=" in line) and (";" in line) and (int(num) != 0):
							""" Pick the lines which tells the component details """
							period_1 = line.find('.')
							period_2 = period_1 + line[period_1+1:].find('.') + 1
							o_f.write("\t%s[%s].%s;\n" %(line[:period_1], line[period_1+1:period_2], line[period_2+1:line.find(';')]))
						else:
							o_f.write("\n")
					o_f.write("\n")
			except:
				print("Cannot open input file %s" %in_tbsa_tgt)

			""" Populate major structure details """
			for group in unique_major_groups:
				o_f.write("\t%s_hdr_t group_%s = {0};\n" %(group.lower(), group.lower()))
				o_f.write("\tgroup_%s.cfg_type.cfg_id = (GROUP_%s << 24);\n" %(group.lower(), group))
				o_f.write("\tgroup_%s.cfg_type.size  += sizeof(group_%s);\n" %(group.lower(), group.lower()))
				for minor in unique_minor_components:
					if group == minor_major_map[minor]:
						o_f.write("\tgroup_%s.cfg_type.size  += sizeof(%s);\n" %(group.lower(), minor.lower()))
						o_f.write("\tgroup_%s.num            += %s_num_instances;\n" %(group.lower(), minor.lower()))
				o_f.write("\n")

			""" Start pushing fprintf into the source resposible for generating the database table """
			o_f.write("\tuint32_t *word_ptr;\n")
			o_f.write("\tint      byte_no  = 0;\n")
			o_f.write("\tFILE     *fp;\n\n")
			o_f.write("\tfp = fopen(\"%s\", \"w\");\n\n" %(out_database))
			temp_out_file = out_database
			while '/' in temp_out_file:
				temp_out_file = "".join(temp_out_file[temp_out_file.find('/')+1:])
			o_f.write("\tfprintf(fp, \"#ifndef _%s_H_\\n\");\n" %(temp_out_file[:temp_out_file.find('.')].upper()))
			o_f.write("\tfprintf(fp, \"#define _%s_H_\\n\");\n\n" %(temp_out_file[:temp_out_file.find('.')].upper()))
			if section_name:
				o_f.write("\tfprintf(fp, \"__attribute__((section(\\\"%s\\\")))\\n\");\n" %(section_name))
			o_f.write("\tfprintf(fp, \"static const uint32_t %s[] = {\\n\");\n" %(table_name))
			o_f.write("\tfprintf(fp, \"0x%x\", \'T\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'B\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'S\');\n")
			o_f.write("\tfprintf(fp, \"%x,\\n\", \'A\');\n")
			o_f.write("\tfprintf(fp, \"0x%x\", \'_\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'C\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'F\');\n")
			o_f.write("\tfprintf(fp, \"%x,\\n\", \'G\');\n")
			required_target_name_len = 4
			new_target = target
			if (len(target) < required_target_name_len):
				while(required_target_name_len - len(target)):
					new_target += " "
					required_target_name_len -= 1
			o_f.write("\tfprintf(fp, \"0x%x\", ")
			o_f.write("\'%c\');\n" %(new_target[0:1]))
			o_f.write("\tfprintf(fp, \"%x\", ")
			o_f.write("\'%c\');\n" %(new_target[1:2]))
			o_f.write("\tfprintf(fp, \"%x\", ")
			o_f.write("\'%c\');\n" %(new_target[2:3]))
			o_f.write("\tfprintf(fp, \"%x,\\n\", ")
			o_f.write("\'%c\');\n" %(new_target[3:4]))
			o_f.write("\tfprintf(fp, \"0x%x\", \'_\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'C\');\n")
			o_f.write("\tfprintf(fp, \"%x\", \'F\');\n")
			o_f.write("\tfprintf(fp, \"%x,\\n\", \'G\');\n")
			o_f.write("\tuint32_t version = 1;\n")
			o_f.write("\tfprintf(fp, \"0x%08x,\\n\", version);\n")
			o_f.write("\tuint32_t total_size = 0;\n")
			for group in unique_major_groups:
				o_f.write("\ttotal_size += group_%s.cfg_type.size;\n" %(group.lower()))
			o_f.write("\ttotal_size += (8 + 8 + 4 + 4);\n")
			o_f.write("\tfprintf(fp, \"0x%08x,\\n\", total_size);\n")

			""" Start writing component values to database file """
			for group in unique_major_groups:
				o_f.write("\t/* Writing major group details to the file */\n")
				o_f.write("\tword_ptr = (uint32_t *)&group_%s;\n" %(group.lower()))
				o_f.write("\tfor(byte_no=0; byte_no<sizeof(group_%s); byte_no += 4) {\n" %(group.lower()))
				o_f.write("\t\tfprintf(fp, \"0x%08x,\\n\", *word_ptr);\n")
				o_f.write("\t\tword_ptr++;\n")
				o_f.write("\t}\n")
				o_f.write("\t/* Writing minor group details to the file */\n")
				for minor in unique_minor_components:
					if group == minor_major_map[minor]:
						o_f.write("\tword_ptr = (uint32_t *)&%s[0];\n" %(minor.lower()))
						o_f.write("\tfor(byte_no=0; byte_no<sizeof(%s); byte_no += 4) {\n" %(minor.lower()))
						o_f.write("\t\tfprintf(fp, \"0x%08x,\\n\", *word_ptr);\n")
						o_f.write("\t\tword_ptr++;\n")
						o_f.write("\t}\n")
			o_f.write("\tfprintf(fp, \"0x%08x\\n\", 0xffffffff);\n")
			o_f.write("\tfprintf(fp, \"};\\n\\n\");\n")
			o_f.write("\tfprintf(fp, \"#endif\\n\");\n")
			o_f.write("\treturn 0;\n")
			o_f.write("}\n")
	except:
		print("Cannot open output file %s" %out_source)

get_minor_major_map()
generate_source()
