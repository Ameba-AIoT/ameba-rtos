#!/usr/bin/env python3
"""
Extract ORIGIN or LENGTH (as END) value of a given segment from a linker script (.ld file).
Outputs the value in decimal.

Usage:
    python extract_ld_value.py <ldfile> <segment> <ORIGIN|END>

Example:
    python extract_ld_value.py memory.ld .text ORIGIN
    python extract_ld_value.py memory.ld .data END
"""

import sys
import re

def hex_to_dec(hex_str):
    """Convert a hexadecimal string like '0x1234' to a decimal integer."""
    try:
        return int(hex_str, 16)
    except ValueError:
        raise ValueError(f"Invalid hex string: {hex_str}")

def extract_value(ldfile, segment, variable):
    """
    Extract ORIGIN or LENGTH (treated as END) for the specified segment.
    Note: 'END' here refers to the LENGTH field, consistent with the original script.
    """
    # Match lines starting with the segment name (ignoring leading whitespace)
    segment_pattern = re.compile(rf'^\s*{re.escape(segment)}\s*[:\(].*$', re.IGNORECASE)
    origin_re = re.compile(r'ORIGIN\s*=\s*(0x[0-9A-Fa-f]+)', re.IGNORECASE)
    length_re = re.compile(r'LENGTH\s*=\s*(0x[0-9A-Fa-f]+)', re.IGNORECASE)

    try:
        with open(ldfile, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                stripped = line.strip()
                if not stripped or stripped.startswith('/*') or stripped.startswith('*'):
                    continue
                if segment_pattern.match(stripped):
                    if variable == "ORIGIN":
                        match = origin_re.search(stripped)
                        if match:
                            return hex_to_dec(match.group(1))
                    elif variable == "END":
                        match = length_re.search(stripped)
                        if match:
                            return hex_to_dec(match.group(1))
    except FileNotFoundError:
        print(f"Error: File '{ldfile}' not found.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)

    return None

def main():
    if len(sys.argv) != 4:
        print("Usage: python extract_ld_value.py <ldfile> <segment> <ORIGIN|END>", file=sys.stderr)
        sys.exit(1)

    ldfile = sys.argv[1]
    segment = sys.argv[2]
    variable = sys.argv[3].upper()

    if variable not in ("ORIGIN", "END"):
        print("Error: Third argument must be 'ORIGIN' or 'END'", file=sys.stderr)
        sys.exit(1)

    value = extract_value(ldfile, segment, variable)
    if value is not None:
        print(value)
    else:
        print(f"Value for '{variable}' not found for segment '{segment}' in '{ldfile}'.", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
