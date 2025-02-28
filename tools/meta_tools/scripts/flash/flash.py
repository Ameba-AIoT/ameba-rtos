#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import argparse
import ast

from base import *
import version_info

MinSupportedDeviceProfileMajorVersion = 1
MinSupportedDeviceProfileMinorVersion = 1
setting_file = "Setting.json"


def parse_string_as_dict_list(value):
    dict_list = []
    try:
        parsed_value = ast.literal_eval(value)
        if isinstance(parsed_value, list):
            for parsed_dict in parsed_value:
                if isinstance(parsed_dict, dict):
                    dict_list.append(parsed_dict)
                else:
                    raise argparse.ArgumentTypeError(f"Expected dict element in list, got {type(parsed_dict).__name__}")
        else:
            raise argparse.ArgumentTypeError(f"Expected a list, got {type(parsed_value).__name__}")
    except (ValueError, SyntaxError) as err:
        raise argparse.ArgumentTypeError("Invalid list format") from err

    return dict_list


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-d', '--download', action='store_true', help='start address, hex')
    parser.add_argument('-f', '--profile', type=str, help='device profile')
    parser.add_argument('-p', '--port', help='serial port')
    parser.add_argument('-b', '--baudrate', type=int, help='serial port baud rate')
    parser.add_argument('-i', '--image', type=str, help='single image')
    parser.add_argument('-r', '--image-dir', type=str, help='image directory')
    parser.add_argument('-a', '--start-address', type=str, help='start address, hex')
    parser.add_argument('-n', '--end-address', type=str, help='end address, hex')
    parser.add_argument('-z', '--size', type=int, help='size in KB')
    parser.add_argument('-m', '--memory-type', choices=['nor', 'nand', 'ram'], default="nor",
                        help='specified memory type')
    parser.add_argument('-e', '--erase', action='store_true', help='erase flash')
    parser.add_argument('-o', '--log-file', type=str, help='output log file with path')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {version_info.version}')

    parser.add_argument('--chip-erase', action='store_true', help='chip erase option for download')
    parser.add_argument('--log-level', default='info', help='log level')
    parser.add_argument('--partion-table', type=parse_string_as_dict_list, help="layout info, list")

    args = parser.parse_args()
    download = args.download
    profile = args.profile
    image = args.image
    image_dir = args.image_dir
    chip_erase = args.chip_erase
    serial_port = args.port
    serial_baudrate = args.baudrate
    log_level = args.log_level.upper()
    log_file = args.log_file
    erase = args.erase
    start_addr = args.start_address
    end_addr = args.end_address
    size = args.size
    mem_t = args.memory_type
    partion_table = args.partion_table

    if mem_t is not None:
        if mem_t == "nand":
            memory_type = MemoryInfo.MEMORY_TYPE_NAND
        elif mem_t == "ram":
            memory_type = MemoryInfo.MEMORY_TYPE_RAM
        else:
            memory_type = MemoryInfo.MEMORY_TYPE_NOR
    else:
        memory_type = None

    if log_file is not None:
        log_path = os.path.dirname(log_file)
        if log_path:
            if not os.path.exists(log_path):
                os.makedirs(log_path, exist_ok=True)
            log_f = log_file
        else:
            log_f = os.path.join(os.getcwd(), log_file)
    else:
        log_f = None
    logger = create_logger("flash", log_level=log_level, file=log_f)
    if log_file is not None:
        logger.info(f"Log file: {log_file}")

    logger.info(f"Flash Version: {version_info.version}")

    if profile is None:
        logger.error('Invalid arguments, no device profile specified')
        parser.print_usage()
        sys.exit(1)

    if not os.path.exists(profile):
        logger.error("Device profile '" + profile + "' does not exist")
        sys.exit(1)
    logger.info(f'Device profile: {profile}')

    if serial_port is None:
        logger.error('Invalid arguments, no serial port specified')
        parser.print_usage()
        sys.exit(1)
    logger.info(f'Serial port: {serial_port}')

    if serial_baudrate is None:
        logger.error('Invalid arguments, no serial baudrate specified')
        parser.print_usage()
        sys.exit(1)
    logger.info(f'Baudrate: {serial_baudrate}')

    if all([download, erase]):
        logger.warning("Download and erase are set true, only do image download ")
    elif not (download or erase):
        logger.error("Download or erase should be set")
        sys.exit(1)

    memory_info = None
    images_info = None
    if download:
        # download
        if (image is None) and (image_dir is None):
            logger.error('Invalid arguments, no image or image_dir input')
            parser.print_usage()
            sys.exit(1)

        if image is not None:
            download_img_info = ImageInfo()
            if not os.path.exists(image):
                logger.error(f"Image {image} does not exist")
                sys.exit(1)
            download_img_info.image_name = image
            download_img_info.description = os.path.basename(image)

            if memory_type is None:
                logger.error(f"Memory type is required for single image download")
                sys.exit(1)

            if start_addr is None:
                logger.error(f"Start address is required for single image download")
                sys.exit(1)

            try:
                start_address = int(start_addr, 16)
            except Exception as err:
                logger.error(f"Start address is invalid: {err}")
                sys.exit(-1)
            download_img_info.start_address = start_address

            if memory_type == MemoryInfo.MEMORY_TYPE_NAND:
                if end_addr is None:
                    logger.error(f"End address is required for nand flash download")
                    sys.exit(1)

                try:
                    end_address = int(end_addr, 16)
                except Exception as err:
                    logger.error(f"End address is invalid: {err}")
                    sys.exit(-1)
            else:
                end_address = start_address + os.path.getsize(image)

            download_img_info.end_address = start_address + os.path.getsize(image)
            download_img_info.memory_type = memory_type
            images_info = [download_img_info]

            logger.info(f'Image info:')
            for img_info in images_info:
                for key, value in img_info.__repr__().items():
                    if key == "ImageName":
                        key = "Image"
                    logger.info(f'> {key}: {value}')
        else:
            images_info = None
            if not os.path.exists(image_dir):
                logger.error(f"Image directory {image_dir} does not exist")
                sys.exit(1)

            logger.info(f'Image dir: {image_dir}')

        if chip_erase:
            logger.info(f"Chip erase: {chip_erase}")
            if memory_type is None:
                logger.warning("Memory type is required for chip erase")
                sys.exit(1)
            if memory_type != MemoryInfo.MEMORY_TYPE_NOR:
                logger.warning("Memory type should be 'nor' for chip erase")
        else:
            logger.info(f"Chip erase: False")
    else:
        # erase
        memory_info = MemoryInfo()
        if start_addr is None:
            logger.error(f"Start address is required for erase flash")
            sys.exit(1)

        try:
            start_address = int(start_addr, 16)
        except Exception as err:
            logger.error(f"Start address is invalid: {err}")
            sys.exit(-1)
        memory_info.start_address = start_address

        if memory_type is None:
            logger.error("Memory type is required for erase")
            sys.exit(1)

        memory_info.memory_type = memory_type

        if memory_type == MemoryInfo.MEMORY_TYPE_NAND:
            if end_addr is None:
                logger.error(f"End address is required for nand flash download")
                sys.exit(1)

        if end_addr:
            try:
                end_address = int(end_addr, 16)
            except Exception as err:
                logger.error(f"End address is invalid: {err}")
                sys.exit(-1)
        else:
            end_address = 0

        if size is None:
            logger.error(f"Erase size is required")
            sys.exit(-1)
        memory_info.size_in_Kbyte = size

        if end_address == 0:
            end_address = start_address + size
        memory_info.end_address = end_address

    # check device profile
    try:
        profile_json = JsonUtils.load_from_file(profile)
        if profile_json is None:
            logger.error(f"Fail to load device profile {profile}")
            sys.exit(1)
        profile_info = RtkDeviceProfile(**profile_json)
        ver = profile_info.get_version()
        if ver.major >= MinSupportedDeviceProfileMajorVersion and ver.minor >= MinSupportedDeviceProfileMinorVersion and profile_info.device_id != 0:
            logger.info(f"Device profile {profile} loaded")
        else:
            logger.error(f"Fail to load device profile {profile}, unsupported version {ver.__repr__()}")
            sys.exit(1)
    except Exception as err:
        logger.error(f"Load device profile {profile} exception: {err}")
        sys.exit(1)

    # check serial port
    try:
        port = serial.Serial(serial_port,
                             baudrate=profile_info.handshake_baudrate,
                             parity=serial.PARITY_NONE,
                             stopbits=serial.STOPBITS_ONE,
                             bytesize=serial.EIGHTBITS)
    except Exception as err:
        logger.error(f"Access serial port {serial_port} fail: {err}")
        sys.exit(1)

    # load settings
    if getattr(sys, "frozen", False):  # judge if frozen as exe
        # get exe dir
        setting_dir = os.path.dirname(os.path.abspath(sys.executable))
    else:
        # get py dir
        setting_dir = os.path.dirname(__file__)
    setting_path = os.path.realpath(os.path.join(setting_dir, setting_file))
    logger.info(f"Setting path: {setting_path}")
    try:
        if os.path.exists(setting_path):
            dt = JsonUtils.load_from_file(setting_path, need_decrypt=False)
            settings = RtSettings(**dt)
        else:
            logger.debug(f"Setting.json not exists!")
            settings = RtSettings(**{})
    except Exception as err:
        logger.error(f"Load settings exception: {err}")
        settings = RtSettings(**{})
    # save Setting.json
    try:
        JsonUtils.save_to_file(setting_path, settings.__repr__())
    except Exception as err:
        logger.debug(f"save setting.json exception: {err}")

    ameba = Ameba(profile_info, port, serial_baudrate, image_dir, settings, logger,
                  download_img_info=images_info,
                  chip_erase=chip_erase,
                  memory_type=memory_type,
                  erase_info=memory_info)
    if download:
        # download
        if not ameba.check_protocol_for_download():
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            sys.exit(-1)

        logger.info(f"Image download start...")
        ret = ameba.prepare()
        if ret != ErrType.OK:
            logger.error("Download prepare fail")
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)

        ret = ameba.post_verify_images()
        if ret != ErrType.OK:
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)

        ret = ameba.download_images()
        if ret != ErrType.OK:
            logger.error("Download image fail")
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)
    else:
        # erase
        logger.info(f"Erase {mem_t} start...")
        ret = ameba.prepare()
        if ret != ErrType.OK:
            logger.error("Erase prepare fail")
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)

        ret = ameba.post_validate_config_for_erase()
        if ret != ErrType.OK:
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)

        ret = ameba.erase_flash()
        if ret != ErrType.OK:
            if port.is_open:
                port.close()
                logger.info(f"{port.port} closed.")
            logger.error(f"Erase {mem_t} failed")
            logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
            sys.exit(-1)

    if port.is_open:
        port.close()
    while port.is_open:
        pass
    if not port.is_open:
        logger.info(f"{port.port} closed.")

    logger.info(f"Finished PASS")  # customized, do not modify
    sys.exit(0)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv[1:])
