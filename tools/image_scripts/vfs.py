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

class BlockDevice:
    """Block device wrapper for littlefs-python that supports slice operations"""
    def __init__(self, data):
        self.data = bytearray(data)

    def __getitem__(self, key):
        return self.data[key]

    def __setitem__(self, key, value):
        self.data[key] = value

def unpack_littlefs_image(input_image, output_dir, block_size=None):
    with open(input_image, 'rb') as f:
        image_data = f.read()

    block_size = block_size or 4096
    block_count = len(image_data) // block_size

    # Create block device wrapper
    bd = BlockDevice(image_data)

    # Create LittleFS instance and mount
    lfs = LittleFS(block_size=block_size, block_count=block_count)
    lfs.context.buffer = bd

    try:
        lfs.mount()
    except Exception as e:
        print(f"Failed to mount littlefs image: {e}")
        raise

    # Read all files recursively
    def walk_lfs(path, prefix=''):
        try:
            entries = list(lfs.scandir(path))
        except Exception:
            return

        for entry in entries:
            # entry.name is the filename, entry.type is 1 for file, 2 for directory
            full_path = os.path.join(path, entry.name)
            try:
                # entry.type: 1 = file, 2 = directory
                if entry.type == 2:
                    os.makedirs(os.path.join(output_dir, prefix, entry.name), exist_ok=True)
                    walk_lfs(full_path, os.path.join(prefix, entry.name))
                else:
                    with lfs.open(full_path, 'rb') as f:
                        data = f.read()
                    rel_path = os.path.join(prefix, entry.name) if prefix else entry.name
                    out_file = os.path.join(output_dir, rel_path)
                    os.makedirs(os.path.dirname(out_file), exist_ok=True)
                    with open(out_file, 'wb') as of:
                        of.write(data)
            except Exception as e:
                print(f"Warning: Failed to read {full_path}: {e}")

    walk_lfs('/')
    print(f"Unpacked {input_image} -> {output_dir}")

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

def unpack_fatfs_image(input_image, output_dir):
    fat_fs = PyFatFS(input_image)

    # Use walk to traverse all files and directories
    # Note: dirs and files are lists of Info objects, not strings
    for root, dirs, files in fat_fs.walk('/'):
        # root is like '/', '/subdir', etc.
        # Convert to relative path for output
        rel_root = root.lstrip('/')

        # Create directories (dirs is list of Info objects)
        for d in dirs:
            dir_name = d.name
            out_dir = os.path.join(output_dir, rel_root, dir_name) if rel_root else os.path.join(output_dir, dir_name)
            os.makedirs(out_dir, exist_ok=True)

        # Copy files (files is list of Info objects)
        for f in files:
            file_name = f.name
            src_path = os.path.join(root, file_name).replace(os.sep, '/')
            rel_path = os.path.join(rel_root, file_name) if rel_root else file_name
            out_file = os.path.join(output_dir, rel_path)
            os.makedirs(os.path.dirname(out_file), exist_ok=True)

            try:
                data = fat_fs.getbytes(src_path)
                with open(out_file, 'wb') as of:
                    of.write(data)
            except Exception as e:
                print(f"Warning: Failed to read {src_path}: {e}")

    fat_fs.close()
    print(f"Unpacked {input_image} -> {output_dir}")

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
    parser.add_argument("-u", "--unpack",
                        type=str,
                        help="Unpack image to specified directory")

    args = parser.parse_args()

    # Handle unpack mode
    if args.unpack:
        if not os.path.exists(args.source_directory):
            print(f"Error: Input image '{args.source_directory}' does not exist.")
            exit(1)

        os.makedirs(args.unpack, exist_ok=True)

        if args.type == "LITTLEFS":
            try:
                unpack_littlefs_image(args.source_directory, args.unpack, args.block_size or 4096)
                print(f"Unpack completed: {args.source_directory} -> {args.unpack}")
                return
            except Exception as e:
                print(f"Failed to unpack LittleFS image: {e}")
                exit(1)
        else:  # FATFS
            try:
                unpack_fatfs_image(args.source_directory, args.unpack)
                print(f"Unpack completed: {args.source_directory} -> {args.unpack}")
                return
            except Exception as e:
                print(f"Failed to unpack FATFS image: {e}")
                exit(1)

    if not os.path.exists(args.source_directory):
        print(f"Source directory '{args.source_directory}' does not exist.")
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
    if args.type == "LITTLEFS":
        print(f"├─ block_size: {block_size}")
        print(f"├─ block_count: {block_count}")
    else:
        print(f"├─ sector_size: {block_size}")
        print(f"├─ sector_count: {block_count}")
    print(f"├─ image_size: {block_size * block_count}")
    print(f"├─ source_directory: {args.source_directory}")
    print(f"└─ output_image: {args.output_image}")


if __name__ == "__main__":
    main()
