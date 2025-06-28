import re
import json
import argparse
import hashlib
import sys
import os
from collections import OrderedDict


def parse_header_file(input_file):
    with open(input_file, 'r') as f:
        content = f.read()

    # Remove comments
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    content = re.sub(r'//.*?\n', '\n', content)

    # Parse enums
    enums = {}
    enum_pattern = re.compile(r'enum\s+(\w+)\s*\{([^}]*)\}', re.DOTALL)
    for enum_match in enum_pattern.finditer(content):
        enum_name = enum_match.group(1)
        enum_body = enum_match.group(2)

        enum_values = OrderedDict()
        current_value = 0
        for line in enum_body.split('\n'):
            line = line.strip()
            if not line or line.startswith('//'):
                continue

            item_match = re.match(r'(\w+)(?:\s*=\s*([^,]+))?,?', line)
            if item_match:
                name = item_match.group(1)
                if item_match.group(2):
                    value_expr = item_match.group(2).strip()
                    try:
                        current_value = eval(value_expr)
                    except:
                        current_value = value_expr
                enum_values[name] = current_value
                if isinstance(current_value, int):
                    current_value += 1

        enums[enum_name] = dict(enum_values)

    # Parse structs with improved array handling
    structs = {}
    struct_pattern = re.compile(r'struct\s+(?:tag)?(\w+)\s*\{([^}]*)\}\s*(?:__PACKED)?\s*;', re.DOTALL)
    for struct_match in struct_pattern.finditer(content):
        struct_name = struct_match.group(1)
        struct_body = struct_match.group(2)

        fields = OrderedDict()
        for line in struct_body.split('\n'):
            line = line.strip()
            if not line or line.startswith('//'):
                continue

            # Handle bit fields
            bit_field_match = re.match(r'(\w+)\s+(\w+)\s*:\s*(\d+);', line)
            if bit_field_match:
                type_name = bit_field_match.group(1)
                field_name = bit_field_match.group(2)
                bits = bit_field_match.group(3)
                fields[field_name] = f"{type_name}:{bits}"
                continue

            # Handle normal fields and arrays
            field_match = re.match(r'(\w+)\s+(\w+)(?:\s*$$(\d*)$$)?;', line)
            if field_match:
                type_name = field_match.group(1)
                field_name = field_match.group(2)
                array_size = field_match.group(3)

                if array_size is not None:
                    if array_size == '0':  # Flexible array member
                        fields[field_name] = f"{type_name}[]"
                    elif array_size == '':  # Incomplete array declaration
                        fields[field_name] = f"{type_name}[]"
                    else:  # Fixed size array
                        try:
                            size = int(array_size)
                            fields[field_name] = [type_name] * size
                        except ValueError:
                            fields[field_name] = f"{type_name}[{array_size}]"
                else:
                    fields[field_name] = type_name

        structs[struct_name] = dict(fields)

    # Handle typedefs
    typedef_pattern = re.compile(r'typedef\s+struct\s+(?:tag)?(\w+)\s+(\w+)_t\s*;')
    for typedef_match in typedef_pattern.finditer(content):
        original_name = typedef_match.group(1)
        new_name = typedef_match.group(2) + "_t"
        if original_name in structs:
            structs[new_name] = structs[original_name]

    return {
        'enums': enums,
        'structs': structs
    }


def convert_to_output_format(parsed_data):
    output = {}

    # Add enums
    for enum_name, enum_values in parsed_data['enums'].items():
        output[enum_name] = enum_values

    # Add structs
    for struct_name, struct_fields in parsed_data['structs'].items():
        output[struct_name] = struct_fields

    return output


def normalize_json(data):

    if isinstance(data, dict):
        return {k: normalize_json(v) for k, v in sorted(data.items())}
    elif isinstance(data, list):
        return [normalize_json(item) for item in data]
    else:
        return data


def calculate_json_md5(file_path):

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            json_data = json.load(f)

        normalized_data = normalize_json(json_data)
        normalized_str = json.dumps(normalized_data, separators=(',', ':'), ensure_ascii=False)

        md5_hash = hashlib.md5(normalized_str.encode('utf-8')).hexdigest()[:8]
        return md5_hash
    except Exception as e:
        print(f"Error processing file: {e}", file=sys.stderr)
        sys.exit(1)


def update_summary_json(json_file_path, summary_json_path):

    current_hash = calculate_json_md5(json_file_path)

    with open(json_file_path, 'r', encoding='utf-8') as f:
        current_content = json.load(f)

    # Initialize or load the all_json file
    if os.path.exists(summary_json_path):
        with open(summary_json_path, 'r', encoding='utf-8') as f:
            all_data = json.load(f)
    else:
        all_data = {
            "version":"1.0",
            "hash": {},
            "latest_hash": ""
        }

    compatible = None
    if all_data["hash"]:
        latest_hash = all_data["latest_hash"]
        latest_content = all_data["hash"][latest_hash]["format"]

        # Check if current content is a superset of latest content
        def is_superset(new, old):
            if isinstance(new, dict) and isinstance(old, dict):
                return all(k in new and is_superset(new[k], old[k]) for k in old)
            elif isinstance(new, list) and isinstance(old, list):
                return len(new) >= len(old) and all(is_superset(new[i], old[i]) for i in range(len(old)))
            else:
                return True

        if is_superset(current_content, latest_content):
            compatible = latest_hash

    new_entry = {
        "compatible": [compatible] if compatible else [],
        "format": current_content
    }

    all_data["hash"][current_hash] = new_entry
    all_data["latest_hash"] = current_hash

    if compatible:
        all_data["hash"][compatible]["format"] = {}

    with open(summary_json_path, 'w', encoding='utf-8') as f:
        json.dump(all_data, f, indent=2, ensure_ascii=False)


def main():
    parser = argparse.ArgumentParser(description='Process diagnose format files')
    subparsers = parser.add_subparsers(dest='command', required=True)

    # Parser for header to json conversion
    h2j_parser = subparsers.add_parser('h2j', help='Convert header file to JSON')
    h2j_parser.add_argument('input_header', help='Input header file path')
    h2j_parser.add_argument('output_json', help='Output JSON file path')

    # Add the json file to the summary json file
    vc_parser = subparsers.add_parser('sum', help='Add to the summary JSON')
    vc_parser.add_argument('input_json', help='Input JSON file path')
    vc_parser.add_argument('summary_json', help='The summary JSON path')

    # Calculate hash
    cal_hash_parser = subparsers.add_parser('hash', help='Calculate normalized MD5 hash of a JSON file')
    cal_hash_parser.add_argument('input_json', help='Path to the input JSON file')

    args = parser.parse_args()

    if args.command == 'h2j':
        # Convert header to JSON
        parsed_data = parse_header_file(args.input_header)
        output_data = convert_to_output_format(parsed_data)

        with open(args.output_json, 'w') as f:
            json.dump(output_data, f, indent=2, ensure_ascii=False)

    elif args.command == 'sum':
        # Add to the summary JSON
        update_summary_json(args.input_json, args.summary_json)

    elif args.command == 'hash':
        hash_result = calculate_json_md5(args.input_json)
        print(hash_result)

if __name__ == '__main__':
    main()