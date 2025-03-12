##@package    elf2tinf.py
# @brief   Create the binary and header file of the app in tiny binary format
#
#           Usage:
#           @verbatim
#		usage: elf2tinf.py [-h] [--major MAJOR] [--minor MINOR] elf_file app_name
#
#		positional arguments:
#		  elf_file       Elf file to convert
#		  app_name       Name of the app, should be less than 15 characters
#
#		optional arguments:
#		  -h, --help     show this help message and exit
#		  --major MAJOR  Major version of app. default:1
#		  --minor MINOR  Minor version of app. default:0
#           @endverbatim
# @attention
#
# LICENSE
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Rohit Gujarathi rgujju.github.io/
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import sys
import os
import argparse
import logging
import struct

from elftools.elf.elffile import ELFFile
from elftools import elf

text_section = '.text'
data_section = '.data'
bss_section = '.bss'
got_section = '.got'
symtab_section = '.symtab'
reloc_section = '.rel.text'
rel_data_section = '.data.rel'

## ConvertFile will convert the elf file into the tinf file and header file
#
def ConvertFile(elf_file, app_name, major_version=1, minor_version=0):
    logging.info("Coverting "+elf_file+" to TINF")

    # tinf_name = os.path.splitext(os.path.split(elf_file)[1])[0]
    tinf_name = os.path.splitext(elf_file)[0]
    print(tinf_name)

    if(len(app_name)<=18):
        logging.debug("Length of app_name is "+str(len(app_name)))
        tin = bytearray("TINF",encoding='utf8')
        tin.extend(app_name.encode('utf8'))
        # Pad 0s to make app_name length = 15(app_name)+1(0)
        # Last byte should be 0, for things like printf
        for i in range (18-len(app_name)):
            tin.append(0) # RGCHECK: check if 0 actually corresponds to NULL in C
    else:
        logging.error("App name is greater than 15 characters.")
        return -1

    # Since version fields are 8 bits log, value of it cannot be greater than 255
    if(major_version > 255 or minor_version > 255):
        tin.error("Version cannot be greater than 255")
        return -1
    # Add the major version of the app
    logging.debug("Setting major version to "+str(major_version))
    tin+=struct.pack('<B',major_version)
    # Add the minor version of the app
    logging.debug("Setting minor version to "+str(minor_version))
    tin+=struct.pack('<B',minor_version)

    # Open the input elf file
    with open(elf_file, 'rb') as f:
        # Read elf file
        elffile = ELFFile(f)

        #TODO: Check if elf class is ELF32 and type is EXEC and Machine is ARM


        # Fetch the .text section, this is the section which has all the code
        # It will be kept on the flash on the mcu
        # In our linker script we have put .rodata into text section itself
        # so dont need to handle that separately.
        text = elffile.get_section_by_name(text_section)
        if not isinstance(text, elf.sections.Section):
            logging.error("ELF file has no "+text_section+" section")
            return -1

        # Get the size of text section
        text_section_size = text['sh_size']
        logging.info(text_section+" section is of size "+str(text_section_size)+" bytes or "+str(text_section_size/4)+" 32 bit words")
        # Add size of text section to the header in words (since it is 32 bit system we divide by 4 to get number of words)
        if(text_section_size%4 != 0):
            text_section_size = (text_section_size +3)&-4
        tin+=struct.pack('<I',int(text_section_size/4))
        
        # Fetch the .data section, this section has content which will go into RAM
        reldata = elffile.get_section_by_name(rel_data_section)
        if not isinstance(reldata, elf.sections.Section):
            logging.error("ELF file has no "+rel_data_section+" section")
            return -1

        # Get the size of data section
        reldata_section_size = reldata['sh_size']
        logging.info(rel_data_section+" section is of size "+str(reldata_section_size)+" bytes or "+str(reldata_section_size/4)+" 32 bit words")
        # Add size of data section to the header in words (since it is 32 bit system we divide by 4 to get number of words)
        tin+=struct.pack('<I',int(reldata_section_size/4))

        # Fetch the .data section, this section has content which will go into RAM
        data = elffile.get_section_by_name(data_section)
        if not isinstance(data, elf.sections.Section):
            logging.error("ELF file has no "+data_section+" section")
            return -1

        # Get the size of data section
        data_section_size = data['sh_size']
        logging.info(data_section+" section is of size "+str(data_section_size)+" bytes or "+str(data_section_size/4)+" 32 bit words")
        # Add size of data section to the header in words (since it is 32 bit system we divide by 4 to get number of words)
        tin+=struct.pack('<I',int(data_section_size/4))

        # Fetch the .bss section, this section has content which is 0 and goes into RAM
        # like uninitialzed global variables
        bss = elffile.get_section_by_name(bss_section)
        if not isinstance(bss, elf.sections.Section):
            logging.error("ELF file has no "+bss_section+" section")
            return -1

        # Get the size of bss section
        bss_section_size = bss['sh_size']
        logging.info(bss_section+" section is of size "+str(bss_section_size)+" bytes or "+str(bss_section_size/4)+" 32 bit words")
        # Add size of bss section to the header in words (since it is 32 bit system we divide by 4 to get number of words)
        tin+=struct.pack('<I',int(bss_section_size/4))


        # Fetch the symbol table
        # The symbol table is used to check if a particular relocation
        # is of an object (variable) or a function.
        symtab = elffile.get_section_by_name(symtab_section)
        if not isinstance(symtab, elf.sections.Section):
            logging.error("ELF file has no "+symtab_section+" section")
            return -1

        # Fetch the text relocation section
        rel_text = elffile.get_section_by_name(reloc_section)
        if not isinstance(rel_text, elf.relocation.RelocationSection):
            logging.error("ELF file has no "+reloc_section+" section")
            return -1

        # for reloc in rel_text.iter_relocations():
        #     # TODO: handle different types of relocations?
        #     # Currently only R_ARM_GOT_BREL is handled
        #     if(reloc['r_info_type'] == elf.enums.ENUM_RELOC_TYPE_ARM['R_ARM_GOT_BREL']):
        #         symbol = symtab.get_symbol(reloc['r_info_sym'])
        #         if(symbol.entry['st_info']['type'] == 'STT_OBJECT'):
        #             logging.debug("Variable Reloc offset: " + str(reloc['r_offset']) + " Relocation symbol: "+symbol.name)
        #         elif(symbol.entry['st_info']['type'] == 'STT_FUNC'):
        #             logging.debug("Function Reloc offset: " + str(reloc['r_offset']) + " Relocation symbol: "+symbol.name)
        #         else:
        #             logging.warning("Invalid symbol type "+symbol.entry['st_info']['type']+ ". Only Objects and functions supported.")
        #     else:
        #         # TODO: Reverse map the number to relocation name.
        #         logging.warning("Invalid relocation "+str(reloc['r_info_type'])+". Only R_ARM_GOT_BREL supported.")

        # Fetch the .got section, this section has the GOT table
        got = elffile.get_section_by_name(got_section)
        if not isinstance(got, elf.sections.Section):
            logging.error("ELF file has no "+got_section+" section")
            return -1

        # Get the size of got section
        got_section_size = got['sh_size']
        logging.info(got_section+" section is of size "+str(got_section_size)+" bytes or "+str(got_section_size/4)+" 32 bit words")
        # Add size of got section to the header in words (since it is 32 bit system we divide by 4 to get number of words)
        tin+=struct.pack('<I',int(got_section_size/4))
	# This is nothing but the number of got entries

        # Add the text section to the img
        #print(text_data())
        tin+=text.data()
        
        # Add the rel data section to the img
        tin+=reldata.data()

        # Add the data section to the img
        tin+=data.data()

	# Add the GOT section to the img
        tin+=got.data()

        # TODO Perform sanity checks regarding size of text, data, etc

    tinf_file = tinf_name+".tinf"
    # Create binary file which can loaded over UART/BLE/ etc
    with open(tinf_file, "wb") as f:
        f.write(tin)

    # Create header file as well to test easily
    # header_file = os.path.join('..', 'app_loader', tinf_name+"_tinf.h")
    header_file = tinf_name+"_tinf.h"
    with open(header_file,"w") as f:
        f.write("const uint8_t %s[] = {"%(app_name))
        for i,hex_val in enumerate(tin):
            f.write(hex(hex_val))
            f.write(",")
            if(i%10==0):
                f.write("\n")
        f.write("};")
    
    bin_file = tinf_name+"_tinf.bin"
    with open(bin_file, 'wb') as f:
        f.write(tin)

    return 0

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("elf_file", help="Elf file to convert")
    parser.add_argument("app_name", help="Name of the app, should be less than 15 characters")
    parser.add_argument("--major", help="Major version of app. default:1",type=int)
    parser.add_argument("--minor", help="Minor version of app. default:0",type=int)
    args = parser.parse_args()
    logging.basicConfig(level=logging.DEBUG)

    if(ConvertFile(args.elf_file,args.app_name,args.major,args.minor)<0):
        logging.error("TINF creation failed")
