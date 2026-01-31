#!/usr/bin/env python3
import sys
import os

def usage():
    print(f"Usage: {sys.argv[0]} [Image Map] [Image Asm]")

def get_address_from_map(map_file, keyword):
    try:
        with open(map_file, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                if keyword in line:
                    return line.split()[0]
    except FileNotFoundError:
        print(f"Error: Map file '{map_file}' not found.")
        sys.exit(1)
    return None

def main():
    if len(sys.argv) < 3:
        usage()
        sys.exit(1)

    image_map_path = sys.argv[1]
    image_asm_path = sys.argv[2]

    xo_addr_start = get_address_from_map(image_map_path, "__rom_xo_start__")
    xo_addr_end = get_address_from_map(image_map_path, "__rom_xo_end__")

    if not xo_addr_start or not xo_addr_end:
        print("Error: Could not find start or end addresses in map file.")
        sys.exit(1)

    # ---------------------------------------------------------
    # Bash: while read -r line...
    # ---------------------------------------------------------
    count = 0

    try:
        with open(image_asm_path, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                line = line.strip()

                if ".word" in line:
                    count += 1

                if xo_addr_start in line:
                    count = 0

                if xo_addr_end in line:
                    break

    except FileNotFoundError:
        print(f"Error: Asm file '{image_asm_path}' not found.")
        sys.exit(1)

    if count == 0:
        sys.exit(0)
    else:
        print(f"error: XO range shall no .word in {image_asm_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()