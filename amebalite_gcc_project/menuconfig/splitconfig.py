import re
import os
import sys
import argparse

# Helper function to extract lines between markers
def extract_lines(filename, start_marker, end_marker):
    with open(filename, 'r') as file:
        lines = file.readlines()
    start_index = None
    end_index = None
    for i, line in enumerate(lines):
        if start_marker in line:
            start_index = i
        elif end_marker in line:
            end_index = i
            break
    if start_index is not None and end_index is not None:
        return lines[start_index:end_index]
    else:
        return []

# Helper function to process and write the extracted lines
def process_and_write_lines(extracted_lines, filename, remove_pattern, replace_pattern=None):
    processed_lines = []
    for line in extracted_lines:
        if remove_pattern not in line:
            line = re.sub(replace_pattern, '', line)
            processed_lines.append(line)
    with open(filename, 'w') as file:
        file.writelines(processed_lines)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description=__doc__)

    parser.add_argument(
        "--config-in",
        metavar="CONFIG_IN",
        help="input config files")

    parser.add_argument(
        "--header-in",
        metavar="HEADER_IN",
        help="input header files")

    args = parser.parse_args()

    # Define paths
    FATHER_DIR = sys.path[0] + '/..'

    KM4_PROJECT_PATH = f"{FATHER_DIR}/project_km4"
    KR4_PROJECT_PATH = f"{FATHER_DIR}/project_kr4"

    if args.config_in is None:
        DEF_CONFIG = f"{FATHER_DIR}/.config"
    else:
        DEF_CONFIG = args.config_in

    if args.header_in is None:
        DEF_CONFIG_H = f"{FATHER_DIR}/menuconfig/autoconf.h"
    else:
        DEF_CONFIG_H = args.header_in

    CONFIG_TEMP = f"{FATHER_DIR}/tempconfig.txt"
    CONFIG_KM4 = f"{FATHER_DIR}/menuconfig/.config_km4"
    CONFIG_KR4 = f"{FATHER_DIR}/menuconfig/.config_kr4"
    CONFIG_KM4_H = f"{KM4_PROJECT_PATH}/inc/platform_autoconf.h"
    CONFIG_KR4_H = f"{KR4_PROJECT_PATH}/inc/platform_autoconf.h"

    # Process .config for KR4
    general_lines = extract_lines(DEF_CONFIG, "General config", "KR4 config")
    kr4_lines = extract_lines(DEF_CONFIG, "KR4 config", "KM4 config")
    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(general_lines)
        temp_file.writelines(kr4_lines)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KR4, "_FOR_KM4", "_FOR_KR4")

    # Process autoconf.h for KR4
    with open(DEF_CONFIG_H, 'r') as file:
        all_lines_h = file.readlines()

    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines_h)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KR4_H, "_FOR_KM4", "_FOR_KR4")

    # Process .config for KM4
    km4_lines = extract_lines(DEF_CONFIG, "KM4 config", "Config End")
    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(general_lines)
        temp_file.writelines(km4_lines)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM4, "_FOR_KR4", "_FOR_KM4")

    # Process autoconf.h for KM4
    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines_h)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM4_H, "_FOR_KR4", "_FOR_KM4")

    # Clean up
    os.remove(CONFIG_TEMP)
    os.remove(DEF_CONFIG_H)
