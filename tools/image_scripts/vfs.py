#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import argparse
import math
from littlefs import LittleFS
from pyfatfs.PyFat import PyFat
from pyfatfs.PyFatFS import PyFatFS

def calculate_directory_stats(source_dir):
    total_size = 0
    dir_count = 0
    file_count = 0

    for root, dirs, files in os.walk(source_dir):
        dir_count += len(dirs)
        file_count += len(files)
        for f in files:
            fp = os.path.join(root, f)
            if not os.path.islink(fp) and os.path.isfile(fp):
                total_size += os.path.getsize(fp)

    return total_size, dir_count, file_count

def process_directory(source_dir, mkdir_func, write_func):
    for root, _, files in os.walk(source_dir):
        rel_path = os.path.relpath(root, source_dir)
        fs_path = rel_path.replace(os.sep, '/') if rel_path != '.' else ''

        if fs_path:
            mkdir_func(fs_path)

        for filename in files:
            src_file = os.path.join(root, filename)
            if os.path.islink(src_file):
                continue

            dest_path = os.path.join(fs_path, filename).replace(os.sep, '/')

            with open(src_file, 'rb') as f:
                write_func(dest_path, f.read())

def create_littlefs_image(source_dir, output_image, block_size, block_count):
    lfs = LittleFS(block_size=block_size, block_count=block_count, disk_version=0x00020000)

    def mkdir(path):
        lfs.mkdir(path)

    def write_file(path, data):
        with lfs.open(path, 'wb') as f:
            f.write(data)

    process_directory(source_dir, mkdir, write_file)

    with open(output_image, 'wb') as f:
        f.write(lfs.context.buffer)

def create_fatfs_image(source_dir, output_image, sector_size, sector_count):
    total_bytes = sector_size * sector_count
    fat_type = 12 if total_bytes <= 32*1024*1024 else 16 if total_bytes <= 2*1024*1024*1024 else 32

    with open(output_image, 'wb') as f:
        f.truncate(sector_size * sector_count)

    pf = PyFat()
    pf.mkfs(filename=output_image, fat_type=fat_type, sector_size=sector_size)
    pf.close()

    fat_fs = PyFatFS(output_image)

    def mkdir(path):
        fat_fs.makedirs(path)

    def write_file(path, data):
        with fat_fs.open(path, 'wb') as f:
            f.write(data)

    process_directory(source_dir, mkdir, write_file)

    fat_fs.close()

def dynamic_safety_factor(fs_type, avg_file_size_kb):
    if fs_type == "FATFS":
        # FATFS：1.2~1.8
        if avg_file_size_kb <= 0:
            return 1.8
        base_factor = 1.2
        dynamic_term = 0.6 * (1 / (avg_file_size_kb + 10))
        return max(1.2, min(base_factor + dynamic_term, 1.8))
    else:
        # LittleFS：1.1~1.5
        if avg_file_size_kb <= 0:
            return 1.5
        base_factor = 1.1
        dynamic_term = 0.4 * (1 / (avg_file_size_kb + 20))
        return max(1.1, min(base_factor + dynamic_term, 1.5))

def main():
    parser = argparse.ArgumentParser(description="VFS Bin File Generation")
    parser.add_argument("-t", "--type",
                        choices=["LITTLEFS", "FATFS"],
                        default="LITTLEFS",
                        help="Filesystem type (default: LITTLEFS)")
    parser.add_argument("-s", "--block_size",
                        type=int,
                        help="Block/sector size (default: LITTLEFS=4096, FATFS=512)")
    parser.add_argument("-c", "--block_count",
                        type=int,
                        help="Block/sector count (default: LITTLEFS=32, FATFS=256)")
    parser.add_argument("-dir", "--source_directory",
                        type=str,
                        required=True,
                        help="Source directory path")
    parser.add_argument("-out", "--output_image",
                        type=str,
                        default="vfs.bin",
                        help="Output image file (default: vfs.bin)")

    args = parser.parse_args()

    if not os.path.exists(args.source_directory):
        print(f"Source directory '{global_args.source_directory}' does not exist.")
        exit(1)

    if args.type == "LITTLEFS":
        block_size = args.block_size or 4096
        block_count = args.block_count or 32
    else:
        block_size = args.block_size or 512
        block_count = args.block_count or 256

    # Calculate storage requirements
    total_size, dir_count, file_count = calculate_directory_stats(args.source_directory)
    avg_file_size_kb = total_size / file_count / 1024 if file_count > 0 else 0

    safety_factor = dynamic_safety_factor(args.type, avg_file_size_kb)

    # Metadata calculation
    if args.type == "LITTLEFS":
        meta_per_file = block_size
        meta_per_dir = block_size
    else:
        dir_entry_size = 32
        entries_per_block = block_size // dir_entry_size
        long_name_factor = 1.5  # for LFN
        meta_per_file = (block_size / entries_per_block) * long_name_factor
        meta_per_dir = block_size

    meta_size = (file_count * meta_per_file + dir_count * meta_per_dir)
    total_required = (total_size + meta_size) * safety_factor
    required_blocks = math.ceil(total_required / block_size)

    if required_blocks > block_count:
        print(f"**********************************************")
        print(f"Warning: Adjusting block count from {block_count} to {required_blocks}")
        print(f"**********************************************")
        block_count = required_blocks

    if args.type == "LITTLEFS":
        create_littlefs_image(args.source_directory, args.output_image,
                       block_size, block_count)
    else:
        create_fatfs_image(args.source_directory, args.output_image,
                    block_size, block_count)

    print(f"{args.output_image} has been successfully generated.")
    print("args:")
    print(f"├─ type: {args.type}")
    print(f"├─ block_size: {block_size}")
    print(f"├─ block_count: {block_count}")
    print(f"├─ image_size: {block_size * block_count}")
    print(f"├─ source_directory: {args.source_directory}")
    print(f"└─ output_image: {args.output_image}")


if __name__ == "__main__":
    main()
