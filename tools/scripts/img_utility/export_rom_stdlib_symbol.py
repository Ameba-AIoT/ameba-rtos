#!/usr/bin/env python
# amebadplus km0/km4, amebalite km4/kr4, amebasmart km4

import sys
import subprocess
import os

if len(sys.argv) == 3:
    readelf = sys.argv[1]
    axf_file = sys.argv[2]
else:
    print("invalid parameter!!!  readelf and target_rom.axf path shall be passed in")
    sys.exit()

export_sections = (
    "__rom_stdlib_data_start__",
    "__rom_stdlib_data_end__",
    "__rom_stdlib_bss_start__",
    "__rom_stdlib_bss_end__",
    "__rom_stdlib_heap_start__",
    "__rom_stdlib_text_start__",
    "__rom_stdlib_text_end__",
    "rom_customer_log",
    "stdlib_loguart_relocation",
)

export_symbols = (
    "strchr",
    "strcpy",
    "strsep",
    "strstr",
    "strtok",
    "strcat",
    "strncmp",
    "strncpy",
    "strncat",
    "strtol",
    "strpbrk",
    "printf",
    "sprintf",
    "snprintf",
    "vsnprintf",
    "sscanf",
    "puts",
    "putchar",
    "acos",
    "asin",
    "atan",
    "atan2",
    "cos",
    "cosh",
    "sin",
    "sinh",
    "tan",
    "tanh",
    "exp",
    "frexp",
    "ldexp",
    "log",
    "log10",
    "modf",
    "pow",
    "sqrt",
    "ceil",
    "fabs",
    "floor",
    "fmod",
    "fadd",
    "fsub",
    "fmul",
    "fdiv",
    "__aeabi_drsub",
    "__aeabi_dsub",
    "__adddf3",
    "__aeabi_ui2d",
    "__aeabi_i2d",
    "__aeabi_f2d",
    "__aeabi_ul2d",
    "__aeabi_l2d",
    "__aeabi_dmul",
    "__aeabi_ddiv",
)

export_symbols1 = (
    "atoi",
)

output_file = "rom_symbol_stdlib.txt"

if os.path.exists(output_file):
    os.remove(output_file)
f = open(output_file, "w")

command = readelf + " -W -s " + axf_file + " | sort -k 2";

proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)

print("SECTIONS", file=f)
print("{", file=f)
for line in proc.stdout:
    items = list(filter(None, line.decode().strip().split(" ")))
    if len(items) >= 8:
        t = items[4]
        if t.lower() == "global" or t.lower() == "weak":
            address = int(items[1], 16)
            symbol = items[7]

            if symbol in export_sections:
                print(f"    PROVIDE({symbol} = {hex(address)});", file=f)

            if symbol in export_symbols:
                print(f"    PROVIDE(__wrap_{symbol} = {hex(address)});", file=f)
                print(f"    PROVIDE(_{symbol} = {hex(address)});", file=f)

            if symbol in export_symbols1:
                print(f"    PROVIDE(_{symbol} = {hex(address)});", file=f)
                print(f"    PROVIDE(_str{symbol} = {hex(address)});", file=f)
                print(f"    PROVIDE(__wrap_{symbol} = {hex(address)});", file=f)
                print(f"    PROVIDE(__wrap_str{symbol} = {hex(address)});", file=f)


print("}", file=f)

#flush() does not necessarily write the file’s data to disk. Use flush()followed by os.fsync() to ensure this behavior.
f.flush()
os.fsync(f.fileno())
f.close()
