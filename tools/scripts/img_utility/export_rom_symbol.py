#!/usr/bin/env python

import sys
import subprocess
import os
import platform

if len(sys.argv) == 4:
    outfile = sys.argv[1] # output file name, e.g., rom_symbol.txt
    readelf = sys.argv[2]
    axf_file = sys.argv[3]
else:
	print("invalid parameter!!!  outfile, readelf and target_rom.axf path shall be passed in")
	sys.exit()

# if os.path.exists('rom_symbol.txt'):
# 	os.remove('rom_symbol.txt')
# f=open('rom_symbol.txt', 'w')
if os.path.exists(outfile):
	os.remove(outfile)
f=open(outfile, 'w')

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
            symbol = items[7]
            print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);

print("}", file=f)

#flush() does not necessarily write the file’s data to disk. Use flush()followed by os.fsync() to ensure this behavior.
f.flush()
os.fsync(f.fileno())
f.close()
