#!/usr/bin/env python3
"""
This script is used to modify certain files in projects to enable custom command 
functionality. It automatically replaces content in specified files based on 
predefined modification rules for different chip models.

Usage:
    python patch_custom_cmd.py <chip>

Supported Chip Models:
    - RTL8713E
    - RTL8726E
    - RTL8730E

Example:
    python patch_custom_cmd.py RTL8713E
"""

import os
import re
import sys

CHIP_CHANGES = {
    'RTL8713E': [
        {
            'file': 'component/soc/usrcfg/amebalite/ameba_flashcfg.c',
            'changes': [
                (r'(\{VFS1,\s+0x083E0000,\s+)0x083FFFFF(\})', 
                 '{VFS1,          0x083E0000, 0x087DFFFF}'),
                (r'(\{IMG_DSP,\s+0x08400000,\s+)0x086FFFFF(\})', 
                 '{IMG_DSP,       0x087E0000, 0x089FFFFF}'),
                # change USER after IMG_DSP
                (r'({IMG_DSP.*\n.*\n)(\t{USER,\s+)0x[0-9A-F]+(,\s+0xFFFFFFFF\})',
                 r'\g<1>\g<2>0x08A00000\g<3>')
            ]
        },
        {
            'file': 'component/application/speechmind/src/playlist_parser.c',
            'changes': [
                (r'(#define PLAY_MUSIC_INTENT_ID\s+)"29"', r'\g<1>"-1"'),
                (r'(#define STOP_MUSIC_INTENT_ID\s+)"30"', r'\g<1>"-1"'),
            ]
        }
    ],
    'RTL8730E': [
        {
            'file': 'amebasmart_gcc_project/ameba_layout.ld',
            'changes': [
                (r'(#define PSRAM_END)(\s+)\(0x60800000\)', r'\g<1>\g<2>(0x61500000)'),
                (r'(CA32_BL3_DRAM_NS.*LENGTH = )0x60700000 - 0x60300000', 
                 r'\g<1>0x61400000 - 0x60300000'),
                (r'(KM4_DRAM_HEAP_EXT.*ORIGIN = )0x60700000(.*LENGTH = PSRAM_END - )0x60700000', 
                 r'\g<1>0x61400000\g<2>0x61400000')
            ]
        },
        {
            'file': 'component/soc/usrcfg/amebasmart/ameba_flashcfg.c',
            'changes': [
                (r'(\{VFS1,\s+0x08640000,\s+)0x086BFFFF(\})', 
                 '{VFS1,          0x08640000, 0x08A3FFFF}')
            ]
        },
        {
            'file': 'component/aivoice/CMakeLists.txt',
            'changes': [
                (r'(cn_cmd_ac40)', r'user_cmd'),
            ]
        },
        {
            'file': 'component/application/speechmind/src/playlist_parser.c',
            'changes': [
                (r'(#define PLAY_MUSIC_INTENT_ID\s+)"29"', r'\g<1>"-1"'),
                (r'(#define STOP_MUSIC_INTENT_ID\s+)"30"', r'\g<1>"-1"'),
            ]
        }
    ]
}

def apply_patches(chip):
    if chip not in CHIP_CHANGES:
        print(f"Error: Unsupported chip model '{chip}'. Supported models: {list(CHIP_CHANGES.keys())}")
        return False

    changes = CHIP_CHANGES[chip]

    for item in changes:
        filepath = item['file']

        if not os.path.exists(filepath):
            print(f"Warning: File '{filepath}' does not exist. Skipping...")
            continue

        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()

            modified = False
            for pattern, replacement in item['changes']:
                if re.search(pattern, content, re.MULTILINE):
                    content = re.sub(pattern, replacement, content, flags=re.MULTILINE)
                    modified = True
                else:
                    print(f"Warning: Pattern '{pattern}' not found in file '{filepath}'.")

            if modified:
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"Successfully patched file: {filepath}")
            else:
                print(f"No changes made to file: {filepath}")

        except Exception as e:
            print(f"Error processing file '{filepath}': {str(e)}")
            return False

    print(f"Patch Finished for {chip}!")
    return True

def print_usage():
    print("Usage: python patch_script.py <chip>")
    print("Supported chips:")
    for chip in CHIP_CHANGES.keys():
        print(f"  - {chip}")

def main():
    if len(sys.argv) != 2:
        print_usage()
        sys.exit(1)

    chip = sys.argv[1].upper()
    # Replace RTL8726E with RTL8713E
    if chip == "RTL8726E":
        chip = "RTL8713E"

    apply_patches(chip)

if __name__ == '__main__':
    main()
