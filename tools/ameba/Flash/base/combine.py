#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os

from .rtk_utils import RtkUtils


def combine_profile_images(profile_info, image_dir, logger, output_dir=None, padding_byte=0xFF):
    if not isinstance(padding_byte, int) or not 0 <= padding_byte <= 0xFF:
        raise ValueError("padding_byte must be in range 0x00-0xFF")
    padding_data = bytes([padding_byte])

    if output_dir is None:
        output_dir = RtkUtils.get_executable_root_path()
    output_dir = os.path.realpath(output_dir)
    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, "image_all.bin")
    temp_output_path = output_path + ".tmp"
    flash_start_address = profile_info.flash_start_address
    images = sorted(
        (image for image in profile_info.images if image.mandatory),
        key=lambda image: image.start_address)

    resolved_images = []
    for image in images:
        if (profile_info.is_ram_address(image.start_address)
                or profile_info.is_ram_address(image.end_address - 1)):
            raise ValueError(f"RAM image cannot be combined: {image.image_name}")

        image_name = image.image_name
        if ":" in image_name:
            image_name = image_name.split(":", 1)[1].split("(", 1)[0].strip()
        image_path = os.path.realpath(os.path.join(image_dir, image_name))
        if not os.path.isfile(image_path) and image_name.endswith(".dtb"):
            dtb_files = []
            for image_file_name in os.listdir(image_dir):
                candidate_path = os.path.join(image_dir, image_file_name)
                if image_file_name.endswith(".dtb") and os.path.isfile(candidate_path):
                    dtb_files.append((image_file_name, candidate_path))
            if dtb_files:
                image_name, image_path = dtb_files[0]
                image_path = os.path.realpath(image_path)
                if len(dtb_files) > 1:
                    logger.warning(
                        f"Multiple DTB files found; using {image_name} for image_all.bin")
        if not os.path.isfile(image_path):
            raise FileNotFoundError(f"Image file does not exist: {image_path}")
        if image.start_address < flash_start_address or image.end_address < image.start_address:
            raise ValueError(
                f"Invalid layout for {image_name}: "
                f"0x{image.start_address:08X}-0x{image.end_address:08X}")

        image_size = os.path.getsize(image_path)
        partition_size = image.end_address - image.start_address
        if image_size > partition_size:
            raise ValueError(
                f"Image {image_name} is too large: {image_size} bytes, "
                f"partition size is {partition_size} bytes")
        resolved_images.append((image, image_name, image_path))

    resolved_images.sort(key=lambda item: item[0].start_address)
    last_image = resolved_images[-1][0] if resolved_images else None

    logger.info("Start to combine images...")
    try:
        with open(temp_output_path, "wb") as output_file:
            for image, image_name, image_path in resolved_images:
                image_offset = image.start_address - flash_start_address
                current_offset = output_file.tell()
                if current_offset > image_offset:
                    raise ValueError(
                        f"Image layout overlaps at {image_name}: "
                        f"offset 0x{image_offset:X}, current size 0x{current_offset:X}")

                padding_size = image_offset - current_offset
                while padding_size > 0:
                    chunk_size = min(padding_size, 1024 * 1024)
                    output_file.write(padding_data * chunk_size)
                    padding_size -= chunk_size

                with open(image_path, "rb") as image_file:
                    while True:
                        data = image_file.read(1024 * 1024)
                        if not data:
                            break
                        output_file.write(data)

                # Keep the final layout image at its actual length instead of
                # padding the combined image to the end of its partition.
                if image is last_image:
                    continue

                padding_size = image.end_address - flash_start_address - output_file.tell()
                while padding_size > 0:
                    chunk_size = min(padding_size, 1024 * 1024)
                    output_file.write(padding_data * chunk_size)
                    padding_size -= chunk_size

        os.replace(temp_output_path, output_path)
    except Exception:
        if os.path.exists(temp_output_path):
            os.remove(temp_output_path)
        raise

    if resolved_images:
        logger.info(f"Combine images success, image_all.bin path: {output_path}")
    else:
        logger.warning(
            f"No mandatory images found; generated image_all.bin is empty: {output_path}")
    return output_path
