#!/usr/bin/env python

import sys
import subprocess
import os
import platform

if len(sys.argv) == 3:
	readelf = sys.argv[1]
	axf_file = sys.argv[2]
else:
	print("invalid parameter!!!  readelf and target_rom_ns.axf path shall be passed in")
	sys.exit()

export_sections_start=(
    "AA__wifi_share_rom_start__",
    "AA__wifi_share_bss_start__",
)

export_sections_end=(
    "AA__wifi_share_rom_end__",
    "AA__wifi_share_bss_end__",
)

export_os_rom_sections_start=(
    "AA__os_share_rom_start__",
    "AA__os_share_bss_start__",
)

export_os_rom_sections_end=(
    "AA__os_share_rom_end__",
    "AA__os_share_bss_end__",
)

if os.path.exists('rom_symbol_ns.txt'):
	os.remove('rom_symbol_ns.txt')
f=open('rom_symbol_ns.txt', 'w')

if os.path.exists('rom_symbol_wifi.txt'):
	os.remove('rom_symbol_wifi.txt')
fwifi=open('rom_symbol_wifi.txt', 'w')

if os.path.exists('rom_symbol_os.txt'):
	os.remove('rom_symbol_os.txt')
fos=open('rom_symbol_os.txt', 'w')

command = readelf + " -W -s " + axf_file + " | sort -k 2";

proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)

flag = 0

print("SECTIONS", file=f)
print("{",  file=f)
print("SECTIONS", file=fwifi)
print("{",  file=fwifi)
print("SECTIONS", file=fos)
print("{",  file=fos)

for line in proc.stdout:
    # print(line, file=f)
    items = list(filter(None, line.decode().strip().split(' ')))

    num = len(items)
    if  num >= 8 :
        t = items[4]
        if t.lower() == 'global' or t.lower() == 'weak' :
            address = int(items[1], 16)
            symbol = items[7]

            for temp in export_sections_start :
                if temp == symbol :
                    print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
                    flag = 1

            for temp in export_os_rom_sections_start :
                if temp == symbol :
                    print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
                    flag = 2

            if flag == 0 :
                print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
            elif flag == 1 :
                print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=fwifi);
            else:
                print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=fos);

            for temp in export_sections_end :
                if temp == symbol :
                    print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
                    flag = 0

            for temp in export_os_rom_sections_end :
                if temp == symbol :
                    print("    PROVIDE(" + symbol + " = " + hex(address) + ");", file=f);
                    flag = 0

print("}", file=f)
print("}", file=fwifi)
print("}", file=fos)

#flush() does not necessarily write the file’s data to disk. Use flush()followed by os.fsync() to ensure this behavior.
f.flush()
os.fsync(f.fileno())
os.fsync(fwifi.fileno())
os.fsync(fos.fileno())
f.close()
