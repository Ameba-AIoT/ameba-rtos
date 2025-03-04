#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import argparse
from littlefs import LittleFS

global_args = None


def add_dir_file_to_littlefs(lfs, parent_directory, rel_path=""):
    for item in os.listdir(parent_directory):
        item_full_path = os.path.join(parent_directory, item)
        item_lfs_path = os.path.join(rel_path, item).replace('\\', '/')

        if os.path.isdir(item_full_path):
            lfs.mkdir(item_lfs_path)
            add_dir_file_to_littlefs(lfs, item_full_path, item_lfs_path)
        else:
            with open(item_full_path, 'rb') as f:
                data = f.read()
                with lfs.open(item_lfs_path, 'wb') as lfs_file:
                    lfs_file.write(data)


def create_littlefs_image():
    if global_args.block_size is None:
        global_args.block_size = 4096
    if global_args.block_count is None:
        global_args.block_count = 32
    image_size = global_args.block_size * global_args.block_count

    output_image = "vfs.bin"
    if global_args.output_image is not None:
        output_image = global_args.output_image

    lfs = LittleFS(block_size=global_args.block_size, block_count=global_args.block_count, disk_version=0x00020000)
    add_dir_file_to_littlefs(lfs, global_args.source_directory)

    with open(output_image, 'wb') as image_file:
        image_file.write(lfs.context.buffer)
        print(f"{output_image} has been successfully generated.")
        print(f"block_size: {global_args.block_size}, block_count: {global_args.block_count}, image_size: {image_size}, source_directory: {global_args.source_directory}, output_image: {output_image}")


def main():
    global global_args

    parser = argparse.ArgumentParser(description="")

    parser.add_argument(
        "-t", "--type", choices=["LITTLEFS", "FAT"], required=True,
        help="Type Choice"
    )
    parser.add_argument(
        "-s", "--block_size", type=int,
        help="Block Size"
    )
    parser.add_argument(
        "-c", "--block_count", type=int,
        help="Block Count"
    )
    parser.add_argument(
        "-dir", "--source_directory", type=str, required=True,
        help="Source Directory"
    )
    parser.add_argument(
        "-out", "--output_image", type=str,
        help="Output Image"
    )

    global_args = parser.parse_args()

    if not os.path.exists(global_args.source_directory):
        print(f"Source directory '{global_args.source_directory}' does not exist.")
        exit(1)

    if global_args.type == "LITTLEFS":
        create_littlefs_image()
    elif global_args.type == "FAT":
        create_fat12_image()


if __name__ == "__main__":
    main()
