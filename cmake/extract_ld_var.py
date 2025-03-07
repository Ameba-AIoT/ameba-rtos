import sys
import re

def extract_value(operation, file_path, segment_name):
    """Extract a specific value from a file based on segment and operation."""
    with open(file_path, 'r') as file:
        for line in file:
            # Check if the line starts with the segment name
            if line.lstrip().startswith(segment_name):
                if operation == "ORIGIN":
                    match = re.search(r'ORIGIN = 0x[0-9A-Fa-f]+', line)
                    if match:
                        hex_value = match.group(0).split('=')[1].strip()
                        return int(hex_value, 16)
                elif operation == "END":
                    match = re.search(r'LENGTH = 0x[0-9A-Fa-f]+', line)
                    if match:
                        hex_value = match.group(0).split('=')[1].strip()
                        return int(hex_value, 16)
                elif operation == "SIZE":
                    match = re.search(r'ORIGIN = 0x[0-9A-Fa-f]+', line)
                    if match:
                        hex_value = match.group(0).split('=')[1].strip()
                        start = int(hex_value, 16)
                    match = re.search(r'LENGTH = 0x[0-9A-Fa-f]+', line)
                    if match:
                        hex_value = match.group(0).split('=')[1].strip()
                        end = int(hex_value, 16)
                        length = end - start
                        return length
    return None  # Return None if no value is found

if __name__ == "__main__":
    # Get command line arguments
    operation = sys.argv[1]
    ldfile = sys.argv[2]
    segment = sys.argv[3]

    # Execute the function and store the result
    result = extract_value(operation, ldfile, segment)

    # Check if result is not None, then print or handle otherwise
    if result is not None:
        print(result)
    else:
        print("NOTHING found! Please check LD file and segment.")