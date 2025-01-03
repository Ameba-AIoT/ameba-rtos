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
    # print(f"start_marker: {start_index}, end_marker: {end_index}")
    if start_index is not None and end_index is not None:
        return lines[start_index:end_index]
    else:
        return []

# Helper function to process and write the extracted lines
def process_and_write_lines(extracted_lines, filename, skip_pattern, replace_pattern=None):
    processed_lines = []
    for line in extracted_lines:
        if not any(pattern in line for pattern in skip_pattern):
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

    if args.config_in is None:
        DEF_CONFIG = f"{FATHER_DIR}/menuconfig/.config"
    else:
        DEF_CONFIG = args.config_in

    if args.header_in is None:
        DEF_CONFIG_H = f"{FATHER_DIR}/menuconfig/autoconf.h"
    else:
        DEF_CONFIG_H = args.header_in

    CONFIG_TEMP = f"{FATHER_DIR}/tempconfig.txt"

    general_lines = extract_lines(DEF_CONFIG, "GeneralStart", "LPStart")

    sub_projects = ["LP", "HP", "AP"]
    for proj in sub_projects:
        lines = extract_lines(DEF_CONFIG, f"{proj}Start", f"{proj}End")
        with open(CONFIG_TEMP, 'w') as temp_file:
            temp_file.writelines(general_lines)
            temp_file.writelines(lines)
        skip_pattern = [f"_FOR_{o}" for o in sub_projects if o != proj]
        process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(),
                                f"{FATHER_DIR}/menuconfig/.config_{proj.lower()}",
                                skip_pattern,
                                f"_FOR_{proj}"
                                )

        with open(DEF_CONFIG_H, 'r') as file:
            all_lines_h = file.readlines()

        with open(CONFIG_TEMP, 'w') as temp_file:
            temp_file.writelines(all_lines_h)

        process_and_write_lines(open(CONFIG_TEMP, 'r').readlines(),
                                f"{FATHER_DIR}/project_{proj.lower()}/inc/platform_autoconf.h",
                                skip_pattern,
                                f"_FOR_{proj}"
                                )

    # Clean up
    os.remove(CONFIG_TEMP)
    os.remove(DEF_CONFIG_H)
