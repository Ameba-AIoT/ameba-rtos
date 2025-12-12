#!/usr/bin/env python

import sys
import subprocess
import os
import platform

if platform.system().lower() == 'linux':
    readelf = '"/opt/rtk-toolchain/asdk-10.2.0/linux/newlib/bin/arm-none-eabi-readelf"';
else:
     readelf = '"C:/msys64/opt/rtk-toolchain/asdk-10.2.0/mingw32/newlib/bin/arm-none-eabi-readelf"';

axf_file = 'lib/amebasmart_rom_acut/target_rom.axf'

if os.path.exists('rom_symbol.txt'):
	os.remove('rom_symbol.txt')
f=open('rom_symbol.txt', 'w')

#gRamStart_pos = 0x10000bc0  #rom v0
#rom v1
gRamStart_pos = 0x00082000
gFlashStart_pos = 0x08000000

command = readelf + " -W -s " + axf_file + " | sort -k 2";

proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)

print("SECTIONS", file=f)
print("{", file=f)
for line in proc.stdout:
#	print line
    items = list(filter(None, line.decode().strip().split(' ')))

    num = len(items)
    if  num >= 8  :
        t = items[4]
        if t.lower() == 'global' or t.lower() == 'weak' : 
            address = int(items[1], 16)
            #if address < gRamStart_pos and (address & gFlashStart_pos) == 0:
            symbol = items[7]
            print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
                #print "    " + symbol + " = " + hex(address) + ";";

print("}", file=f)

f.close()
