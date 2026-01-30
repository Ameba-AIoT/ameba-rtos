#!/usr/bin/env python3
import sys
import os

def usage():
    print(f"Usage: {sys.argv[0]} [ROM_COM_CHK_LITTLE] [ROM_COM_CHK_BIGGER]")

def main():
    if len(sys.argv) < 3:
        usage()
        sys.exit(1)

    file_little = sys.argv[1]
    file_bigger = sys.argv[2]

    if not os.path.exists(file_little):
        print(f"Error: File '{file_little}' not found.")
        sys.exit(1)
    if not os.path.exists(file_bigger):
        print(f"Error: File '{file_bigger}' not found.")
        sys.exit(1)

    try:
        little_size = os.path.getsize(file_little)

        with open(file_little, 'rb') as f_little, open(file_bigger, 'rb') as f_bigger:
            data_little = f_little.read()
            data_bigger_part = f_bigger.read(little_size)

            if data_little == data_bigger_part:
                sys.exit(0)
            else:
                print(f"error: {file_little} and {file_bigger} shall totally same!")
                sys.exit(1)

    except OSError as e:
        print(f"Error during file operation: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()