import re
import os
import sys

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
        return lines[start_index + 1:end_index]
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
    # Define paths
    FATHER_DIR = sys.argv[1]
    KM4_PROJECT_PATH = f"{FATHER_DIR}/project_km4"
    KM0_PROJECT_PATH = f"{FATHER_DIR}/project_km0"

    DEF_CONFIG = f"{FATHER_DIR}/.config"
    DEF_CONFIG_H = f"{FATHER_DIR}/autoconf.h"
    CONFIG_TEMP = f"{FATHER_DIR}/tempconfig.txt"
    CONFIG_KM4 = f"{FATHER_DIR}/menuconfig/.config_km4"
    CONFIG_KM0 = f"{FATHER_DIR}/menuconfig/.config_km0"
    CONFIG_KM4_H = f"{KM4_PROJECT_PATH}/inc/platform_autoconf.h"
    CONFIG_KM0_H = f"{KM0_PROJECT_PATH}/inc/platform_autoconf.h"

    # Process .config for KM0

    # general_lines = extract_lines(DEF_CONFIG, "#GeneralStart", "#GeneralEnd")
    # km0_lines = extract_lines(DEF_CONFIG, "#KM0Start", "#KM0End")
    # with open(CONFIG_TEMP, 'w') as temp_file:
    #     temp_file.writelines(general_lines)
    #     temp_file.writelines(km0_lines)

    with open(DEF_CONFIG, 'r') as file:
        all_lines = file.readlines()

    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM0, "_FOR_KM4", "_FOR_KM0")

    # Process autoconf.h for KM0

    # general_lines_h = extract_lines(DEF_CONFIG_H, "#GeneralStart", "#GeneralEnd")
    # km0_lines_h = extract_lines(DEF_CONFIG_H, "#KM0Start", "#KM0End")
    # with open(CONFIG_TEMP, 'w') as temp_file:
    #     temp_file.writelines(general_lines_h)
    #     temp_file.writelines(km0_lines_h)

    with open(DEF_CONFIG_H, 'r') as file:
        all_lines_h = file.readlines()

    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines_h)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM0_H, "_FOR_KM4", "_FOR_KM0")

    # Process .config for KM4

    # km4_lines = extract_lines(DEF_CONFIG, "#KM4Start", "#KM4End")
    # with open(CONFIG_TEMP, 'w') as temp_file:
    #     temp_file.writelines(general_lines)
    #     temp_file.writelines(km4_lines)

    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM4, "_FOR_KM0", "_FOR_KM4")

    # Process autoconf.h for KM4

    # km4_lines_h = extract_lines(DEF_CONFIG_H, "#KM4Start", "#KM4End")
    # with open(CONFIG_TEMP, 'w') as temp_file:
    #     temp_file.writelines(general_lines_h)
    #     temp_file.writelines(km4_lines_h)
    with open(CONFIG_TEMP, 'w') as temp_file:
        temp_file.writelines(all_lines_h)

    process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(), CONFIG_KM4_H, "_FOR_KM0", "_FOR_KM4")

    # Clean up
    os.remove(CONFIG_TEMP)
    os.remove(DEF_CONFIG_H)