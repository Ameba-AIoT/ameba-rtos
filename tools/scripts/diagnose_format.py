import re
import json
import argparse
import hashlib
import sys
import os
from collections import OrderedDict

script_path = os.path.abspath(__file__)
script_dir = os.path.dirname(script_path)
component_path = os.path.join(os.path.dirname(os.path.dirname(script_dir)),'component')

target_path = [
   'wifi/api',
   'soc/common/include',
   ]

def _extract_enum_used_for_diag_from_file(enum_name, file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception:
        return {}

    # Remove line comments
    content_no_line_comments = re.sub(r'//.*?\n', '\n', content)
    enum_pattern = re.compile(r'enum\s+' + re.escape(enum_name) + r'\s*\{([^}]*)\}', re.DOTALL)
    m = enum_pattern.search(content_no_line_comments)
    if not m:
        return {}

    body = m.group(1)
    enum_dict = OrderedDict()
    variables = {}
    current_value = None
    is_first_item = True

    for line in body.split('\n'):
        raw = line.strip()
        if not raw:
            continue

        # Extract block comment
        block_comment_match = re.search(r'/\*(.*?)\*/', raw, flags=re.DOTALL)
        comment = block_comment_match.group(1).strip() if block_comment_match else ''

        # Remove block comments to parse name/value
        line_nocom = re.sub(r'/\*.*?\*/', '', raw).strip()
        item_match = re.match(r'(\w+)(?:\s*=\s*([^,]+))?,?', line_nocom)
        if not item_match:
            continue

        name = item_match.group(1)
        value_expr = item_match.group(2).strip() if item_match.group(2) else None

        # Handle the first item
        if is_first_item:
            if value_expr is None:
                current_value = 0
            is_first_item = False

        # Compute variable value
        if value_expr:
            try:
                # Replace previously defined variables
                for var_name, var_value in variables.items():
                    value_expr = re.sub(rf'\b{re.escape(var_name)}\b', str(var_value), value_expr)

                current_value = eval(value_expr)
            except Exception:
                try:
                    current_value = int(value_expr, 0)
                except Exception:
                    current_value = value_expr
        else:
            if current_value is None:
                current_value = 0
            elif isinstance(current_value, int):
                current_value += 1

        variables[name] = current_value

        # Check <!-- DIAG: --> in comment
        diag_content = ""
        if '<!-- DIAG: -->' in comment:
            diag_content = comment.split('<!-- DIAG: -->', 1)[1].strip()

        enum_dict[(name, current_value)] = diag_content

    # Build map: key is non-empty DIAG content, value is corresponding enum value
    diag_map = {}
    for (name, value), diag_content in enum_dict.items():
        if diag_content:
            diag_map[diag_content] = value

    return diag_map

def _parse_enums(content):

    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)

    enums = {}
    enum_pattern = re.compile(r'enum\s+(\w+)\s*\{([^}]*)\}', re.DOTALL)
    for enum_match in enum_pattern.finditer(content):
        enum_name = enum_match.group(1)
        enum_body = enum_match.group(2)

        enum_values = OrderedDict()
        current_value = 0
        for line in enum_body.split('\n'):
            line = line.strip()
            if not line:
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

    return enums

def _parse_structs(content):

    structs = {}
    enum_ref_types = {}
    struct_pattern = re.compile(r'struct\s+(?:tag)?(\w+)\s*\{([^}]*)\}\s*(?:__PACKED)?\s*;', re.DOTALL)
    for struct_match in struct_pattern.finditer(content):
        struct_name = struct_match.group(1)
        struct_body = struct_match.group(2)

        fields = OrderedDict()

        for raw_line in struct_body.split('\n'):
            # Extract block comment (from raw line)
            comment = None
            comment_match = re.search(r'/\*(.*?)\*/', raw_line, flags=re.DOTALL)
            line = re.sub(r'/\*.*?\*/', '', raw_line).strip()
            if comment_match:
                comment = comment_match.group(1).strip()

            # Skip empty lines
            if not line:
                continue

            # Handle bit fields
            bit_field_match = re.match(r'(\w+)\s+(\w+)\s*:\s*(\d+);', line)
            if bit_field_match:
                type_name = bit_field_match.group(1)
                field_name = bit_field_match.group(2)
                bits = bit_field_match.group(3)
                fields[field_name] = {
                    'type': f"{type_name}:{bits}",
                    'comment': comment or ''
                }
                continue

            # Handle normal fields, arrays, and nested structs
            field_match = re.match(r'(struct\s+)?\s*(\w+)\s+(\w+)(?:\s*\[(\d+)\])?;', line)
            """
            r'(struct\s+)?'          # struct marker
            r'\s*(\w+)\s+'           # type name
            r'(\w+)'                 # field name
            r'(?:\s*\[(\d+)\])?'     # array length
            """
            if field_match:
                is_struct = field_match.group(1) is not None
                type_name = field_match.group(2)
                field_name = field_match.group(3)
                array_size = field_match.group(4)

                # Build field info (with comment handling)
                field_info = {
                    'type': f"{type_name}[{array_size}]" if array_size
                            else (f"struct {type_name}" if is_struct else type_name),
                    'comment': comment or ''
                }

                # Explicit comment declaration: "enum rtw_disconn_reason in wifi_api_types.h"
                if comment:
                    enum_ref_match = re.search(r'enum\s+(\w+)\s+in\s+([\w\.]+)', comment, re.IGNORECASE)
                    if enum_ref_match:
                        field_info['enum_ref'] = enum_ref_match.group(1)
                        enum_file = enum_ref_match.group(2)
                        if enum_file.endswith('.h'):
                            for path_ in target_path:
                                candidate = os.path.join(component_path, path_, enum_file)
                                if os.path.exists(candidate):
                                    diag_map = _extract_enum_used_for_diag_from_file(field_info['enum_ref'], candidate)
                                    if diag_map:
                                        enum_ref_types[field_info['enum_ref']] = diag_map
                                    break

                fields[field_name] = field_info

        structs[struct_name] = dict(fields)

    return structs, enum_ref_types

def parse_header_file(input_file):

    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    content = re.sub(r'//.*?\n', '\n', content)
    enums = _parse_enums(content)
    structs, enum_ref_types = _parse_structs(content)

    # Handle typedefs
    typedef_pattern = re.compile(r'typedef\s+struct\s+(?:tag)?(\w+)\s+(\w+)_t\s*;')
    for typedef_match in typedef_pattern.finditer(content):
        original_name = typedef_match.group(1)
        new_name = typedef_match.group(2) + "_t"
        if original_name in structs:
            structs[new_name] = structs[original_name]

    return {
        'enums': enums,
        'structs': structs,
        'enum_ref_types': enum_ref_types
    }


def convert_to_output_format(parsed_data):
    output = {}

    # Add enums
    for enum_name, enum_values in parsed_data['enums'].items():
        output[enum_name] = enum_values

    # Add structs
    for struct_name, struct_fields in parsed_data['structs'].items():
        converted_fields = OrderedDict()
        for field_name, field_info in struct_fields.items():
            # field_info is expected to be a dict with at least 'type'
            if isinstance(field_info, dict):
                entry = {'type': field_info.get('type')}
                # include enum_ref if present
                if 'enum_ref' in field_info:
                    entry['enum_ref'] = field_info['enum_ref']
                # only include comment if non-empty
                comment = field_info.get('comment')
                if comment:
                    entry['comment'] = comment
                converted_fields[field_name] = entry
            else:
                # legacy: if it's a simple type string, keep as type
                converted_fields[field_name] = {'type': field_info}

        output[struct_name] = converted_fields

    # Add diag enums (extracted from referenced .h files)
    if 'enum_ref_types' in parsed_data:
        for enum_name, diag_map in parsed_data['enum_ref_types'].items():
            # ensure keys are human-friendly (already normalized in extraction)
            output[enum_name] = diag_map

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

    # Initialize or load the summary json file
    if os.path.exists(summary_json_path):
        with open(summary_json_path, 'r', encoding='utf-8') as f:
            all_data = json.load(f)
    else:
        all_data = {
            "version": "1.0",
            "hash": {},
            "latest_hash": ""
        }

    # Check if the current hash already exists
    if current_hash in all_data["hash"]:
        print(f"Hash {current_hash} already exists in summary. No update needed.")
        return

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
                return new == old

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

    print(f"Updated summary with new hash {current_hash}")


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