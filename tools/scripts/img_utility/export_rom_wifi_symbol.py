#!/usr/bin/env python
# amebagreen2 km4ns/km4tz

import sys
import subprocess
import os
import platform

if len(sys.argv) == 3:
	readelf = sys.argv[1]
	axf_file = sys.argv[2]
else:
	print("invalid parameter!!!  readelf and target_rom.axf path shall be passed in")
	sys.exit()

if os.path.exists('rom_wifi_symbol.txt'):
	os.remove('rom_wifi_symbol.txt')
f=open('rom_wifi_symbol.txt', 'w')

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
            #print "    " + symbol + " = " + hex(address) + ";";

print("}", file=f);

#flush() does not necessarily write the file’s data to disk. Use flush()followed by os.fsync() to ensure this behavior.
f.flush()
os.fsync(f.fileno())
f.close()
