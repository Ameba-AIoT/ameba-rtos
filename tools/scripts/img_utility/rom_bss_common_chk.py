#!/usr/bin/env python3
import sys
import os

def usage():
    print(f"Usage: {sys.argv[0]} [IMAGE_CHK_START] [IMAGE_CHK_END] [IMAGE_MAP_LITTLE] [IMAGE_MAP_BIGGER]")

def find_marker_line(lines, keyword):
    """
    Bash: CHK_COMMON_START="$(grep $IMAGE_CHK_START $IMAGE_MAP_LITTLE)"
    """
    for line in lines:
        if keyword in line:
            return line.strip()
    return None

def extract_section(lines, start_line_content, end_line_content):
    """
    Bash: sed -n ... | sed ...d | sed ...d
    """
    extracted = []
    in_section = False

    for line in lines:
        stripped_line = line.strip()

        if stripped_line == start_line_content:
            in_section = True
            continue

        if stripped_line == end_line_content:
            in_section = False
            break

        if in_section:
            extracted.append(stripped_line) # without start_line and end_line

    return extracted

def main():
    if len(sys.argv) < 5:
        usage()
        sys.exit(1)

    chk_start_keyword = sys.argv[1]
    chk_end_keyword = sys.argv[2]
    map_little_path = sys.argv[3]
    map_bigger_path = sys.argv[4]

    if not os.path.exists(map_little_path) or not os.path.exists(map_bigger_path):
        print("Error: One or more input files not found.")
        sys.exit(1)

    try:
        with open(map_little_path, 'r', encoding='utf-8', errors='ignore') as f:
            little_lines = f.readlines()

        with open(map_bigger_path, 'r', encoding='utf-8', errors='ignore') as f:
            bigger_lines_set = set(line.strip() for line in f)

        start_full_line = find_marker_line(little_lines, chk_start_keyword)
        end_full_line = find_marker_line(little_lines, chk_end_keyword)

        if not start_full_line or not end_full_line:
            print(f"Error: Markers '{chk_start_keyword}' or '{chk_end_keyword}' not found in {map_little_path}")
            sys.exit(1)

        extracted_content = extract_section(little_lines, start_full_line, end_full_line)

        if not extracted_content:
            print("Warning: Empty section found between markers.")
            sys.exit(0)

        # ---------------------------------------------------------
        # Bash: grep -Fxqf ...
        # ---------------------------------------------------------
        all_lines_exist = all(line in bigger_lines_set for line in extracted_content)

        if all_lines_exist:
            sys.exit(0)
        else:
            print(f"error: [{chk_start_keyword}, {chk_end_keyword}] not same in {map_little_path} and {map_bigger_path}")
            sys.exit(1)

    except Exception as e:
        print(f"Error processing files: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()