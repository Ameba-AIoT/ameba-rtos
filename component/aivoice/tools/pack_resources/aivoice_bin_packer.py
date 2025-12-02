'''
usage example:

* case 1: use config file
python aivoice_bin_packer.py \
    --config   <path_to_mcu>/component/aivoice/resource/aivoice_resources_example.config \
    --bins_dir <path_to_mcu>/component/aivoice/resource/bins \
    --out_dir  <output_directory>

* case 2: without config file
First, copy the bin files that need to be packaged into a new directory, such as "resources",
and then execute following command:
python aivoice_bin_packer.py \
    --no-config \
    --bins_dir <path_to_your_directory_need_to_packaged> \
    --out_dir <output_directory>
'''

import os
import re
import argparse
import struct

MODEL_TYPES = {
    "AFE": 1,
    "VAD": 2,
    "KWS": 3,
    "ASR": 4,
    "FST": 5,
    "NNNS": 6,
    "AGR": 7
}

MAGIC = b'RTAIBIN\x00'
FILE_VERSION = 1
FILE_ALIGNMENT = 1024

def parse_args():
    parser = argparse.ArgumentParser(
            description='Realtek AIVoice Model Packer Tool:'
                ' pack multiple AI model bins into a single file')

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--no-config', action='store_true',
                       help='Pack all valid models in bins_dir')
    group.add_argument('--config', type=str,
                       help='Use config file to select models')
    parser.add_argument('--bins_dir', required=True,
                        help='Directory containing model binaries')
    parser.add_argument('--out_dir',
                        help='Output directory (default: same as bins_dir)')
    parser.add_argument('--out_name', default='aivoice_models.bin',
                        help='Output binary name (default: aivoice_models.bin)')

    args = parser.parse_args()

    args.bins_dir = os.path.abspath(args.bins_dir)
    args.out_dir  = os.path.abspath(args.out_dir) if args.out_dir else args.bins_dir

    return args

def validate_paths(bins_dir, config_path=None):
    if not os.path.isdir(bins_dir):
        print(f"ERROR: Input directory not found: {bins_dir}")
        return False

    if config_path and not os.path.isfile(config_path):
        print(f"ERROR: Config file not found: {config_path}")
        return False

    return True

def parse_config(config_path):
    resources = []
    in_config_section = False

    with open(config_path, 'r') as f:
        for line in f:
            line = line.strip()

            if line == "# AI Config":
                in_config_section = True
                continue
            elif line == "# end of AI Config":
                break

            if not in_config_section:
                continue

            if line.startswith('#'):
                continue

            match = re.match(r'^\s*CONFIG_([\w.-]+)_MENU\s*=\s*y\s*$', line)
            if match:
                resource = match.group(1)
                if any(resource.startswith(t) for t in MODEL_TYPES):
                    resources.append(resource)

    return resources

def check_duplicate_models(matched_bins):
    has_error = False
    for model, bins in matched_bins.items():
        if len(bins) > 1:
            print(f"ERROR: Model {model} has multiple bins configured:")
            for bin_file in bins:
                print(f"  - {bin_file}")
            has_error = True
    return has_error

def find_bins_with_config(bins_dir, resources):
    matched = {}
    missed = False
    all_bins = [f for f in os.listdir(bins_dir) if f.lower().endswith('.bin')]

    for resource in resources:
        bin_name = f"{resource}.bin"
        found = False
        for file in all_bins:
            if file.lower() == bin_name.lower():
                model = resource.split('_')[0]
                matched.setdefault(model, []).append(file)
                found = True
                break
        if not found:
            missed = True
            print("ERROR: missing bins for {}".format(resource))

    if missed:
        exit(1)

    return matched

def scan_bins_directory_without_config(bins_dir):
    matched = {model: [] for model in MODEL_TYPES}
    invalid_files = []

    for file in os.listdir(bins_dir):
        if not file.lower().endswith('.bin'):
            continue

        file_upper = file.upper()

        for model in MODEL_TYPES:
            if file_upper.startswith(model + "_"):
                matched[model].append(file)
                break
        else:
            invalid_files.append(file)

    if invalid_files:
        print("WARNING: Ignored invalid bin files:")
        for f in invalid_files:
            print(f"  - {f}")

    return matched

def pack_models(matched_bins, bins_dir, out_path):
    models = []
    for model_type, bin_files in matched_bins.items():
        if bin_files:
            models.append((MODEL_TYPES[model_type], bin_files[0]))

    if not models:
        print("ERROR: No valid models to package")
        return False

    models.sort(key=lambda x: x[0])

    header = bytearray()
    header += MAGIC
    header += struct.pack('<I', FILE_VERSION)
    header += struct.pack('<I', 0)
    header += struct.pack('<I', len(models))

    model_headers = bytearray()
    model_contents = bytearray()

    current_offset = FILE_ALIGNMENT

    for model_id, bin_file in models:
        bin_path = os.path.join(bins_dir, bin_file)
        with open(bin_path, 'rb') as f:
            bin_data = f.read()

        bin_name = os.path.splitext(bin_file)[0]
        bin_name_bytes = bin_name.encode('ascii')[:31].ljust(32, b'\x00')

        model_headers += struct.pack('<I', model_id)
        model_headers += struct.pack('<I', len(bin_data))
        model_headers += struct.pack('<I', current_offset)
        model_headers += bin_name_bytes

        model_contents += bin_data
        current_offset += len(bin_data)

    header += model_headers
    header_padding = FILE_ALIGNMENT - len(header)
    assert(header_padding >= 0), "header is larger than 1024 bytes"
    if header_padding > 0:
        header += bytes([0] * header_padding)

    full_content = header + model_contents

    if len(full_content) % FILE_ALIGNMENT != 0:
        padding = FILE_ALIGNMENT - (len(full_content) % FILE_ALIGNMENT)
        full_content += bytes([0] * padding)

    total_length = len(full_content)
    full_content[12:16] = struct.pack('<I', total_length)

    with open(out_path, 'wb') as f:
        f.write(full_content)

    print(f"Successfully packed {len(models)} models")
    for m in models:
        print("  - {}".format(m[1]))
    print(f"Output file: {os.path.abspath(out_path)}")
    print(f"Total size: {total_length//1024}KB (0x{total_length:X} bytes)")
    return True

def main():
    args = parse_args()

    if not validate_paths(args.bins_dir, args.config):
        exit(1)

    if args.no_config:
        matched_bins = scan_bins_directory_without_config(args.bins_dir)
    else:
        resources = parse_config(args.config)
        matched_bins = find_bins_with_config(args.bins_dir, resources)

    if not matched_bins:
        print("ERROR: No valid models found to pack")
        exit(1)

    if check_duplicate_models(matched_bins):
        print("Please Check your configure.")
        exit(1)

    out_path = os.path.join(args.out_dir, args.out_name)
    if not pack_models(matched_bins, args.bins_dir, out_path):
        print("ERROR: pack aivoice model failed")
        exit(1)

if __name__ == '__main__':
    main()
