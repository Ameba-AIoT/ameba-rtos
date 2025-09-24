import json
import sys
import argparse

def parse_submodule_info(input_string, output_file):
    if not input_string:
        return

    result = []

    if input_string.endswith('\\'):
        input_string = input_string[:-1]
    submodule_chunks = input_string.split('submodule_name=')[1:]

    for chunk in submodule_chunks:
        parts = chunk.strip().split()

        submodule_name = parts[0]
        submodule_path = parts[1].split('=', 1)[1]

        examples = []
        for i in range(2, len(parts), 2):
            example_name = parts[i].split('=', 1)[1]
            example_path = parts[i+1].split('=', 1)[1]
            examples.append({
                "name": example_name,
                "path": example_path
            })

        result.append({
            "name": submodule_name,
            "path": submodule_path,
            "examples": examples
        })

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=2)

    print(f"Generate submodule info file success: {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('output_file', help='Output json file path')
    parser.add_argument('submodule_info', nargs='?', default=None, help='Submodule information string')

    args = parser.parse_args()

    if args.submodule_info is None:
        print("No submodule, submodule_info.json will not be generated.")
        sys.exit(0)

    # print(f'info: {args.submodule_info}')
    parse_submodule_info(args.submodule_info, args.output_file)