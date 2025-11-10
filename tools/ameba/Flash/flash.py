#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import argparse
import base64
import re
import threading
from copy import deepcopy

from base import *
import version_info

MinSupportedDeviceProfileMajorVersion = 1
MinSupportedDeviceProfileMinorVersion = 1
setting_file = "Settings.json"


def convert_mingw_path_to_windows(mingw_path):
    drive_match = re.match(r'^/([a-zA-Z])/', mingw_path)
    if drive_match:
        drive_letter = drive_match.group(1).upper() + ":\\"
        windows_path_tail = mingw_path[3:]
    else:
        drive_letter = ""
        windows_path_tail = mingw_path

    windows_path_tail = windows_path_tail.replace('/', '\\')

    windows_path = drive_letter + windows_path_tail

    return windows_path


def sys_exit(logger, status, ret):
    if status:
        logger.info(f"Finished PASS")  # customized, do not modify
        sys.exit(0)
    else:
        logger.error(f"Finished FAIL: {ret}")  # customized, do not modify
        sys.exit(1)


def decoder_partition_string(partition_table_base64):
    try:
        if partition_table_base64 is None:
            return None
        partition_value = base64.b64decode(partition_table_base64).decode("utf-8")
        partition_list = json.loads(partition_value)
        return partition_list
    except Exception as err:
        raise argparse.ArgumentTypeError("Invalid partition table format with base64") from err


# --- add remote server params ---
def flash_process_entry(profile_info, serial_port, serial_baudrate, image_dir, settings, images_info,
                        chip_erase,
                        memory_type, memory_info, download,
                        log_level, log_f,
                        remote_server=None, remote_port=None, remote_password=None):
    logger = create_logger(serial_port, log_level=log_level, file=log_f)

    ameba = Ameba(profile_info, serial_port, serial_baudrate, image_dir, settings, logger,
                  download_img_info=images_info,
                  chip_erase=chip_erase,
                  memory_type=memory_type,
                  erase_info=memory_info,
                  remote_server=remote_server,
                  remote_port=remote_port,
                  remote_password=remote_password)
    if download:
        # download
        if not ameba.check_protocol_for_download():
            ret = ErrType.SYS_PROTO
            sys_exit(logger, False, ret)

        ret, is_reburn = ameba.check_supported_flash_size(memory_type)
        if ret != ErrType.OK:
            logger.error(f"Check supported flash size fail")
            sys_exit(logger, False, ret)

        if is_reburn:
            ameba.__del__()
            # reset with remote params
            ameba = Ameba(profile_info, serial_port, serial_baudrate, image_dir, settings, logger,
                          download_img_info=images_info,
                          chip_erase=chip_erase,
                          memory_type=memory_type,
                          erase_info=memory_info,
                          remote_server=remote_server,
                          remote_port=remote_port,
                          remote_password=remote_password)

        logger.info(f"Image download start...")  # customized, do not modify
        ret = ameba.prepare()
        if ret != ErrType.OK:
            logger.error("Download prepare fail")
            sys_exit(logger, False, ret)

        ret = ameba.verify_images()
        if ret != ErrType.OK:
            sys_exit(logger, False, ret)

        if not ameba.is_all_ram:
            ret = ameba.post_verify_images()
            if ret != ErrType.OK:
                sys_exit(logger, False, ret)

        if not ameba.is_all_ram:
            flash_status = FlashBPS()
            ret = ameba.check_and_process_flash_lock(flash_status)
            if ret != ErrType.OK:
                logger.error("Download image fail")
                sys_exit(logger, False, ret)

        ret = ameba.download_images()
        if ret != ErrType.OK:
            logger.error("Download image fail")
            sys_exit(logger, False, ret)

        if (not ameba.is_all_ram) and flash_status.need_unlock:
            logger.info("Restore the flash block protection...")
            ret = ameba.lock_flash(flash_status.protection)
            if ret != ErrType.OK:
                logger.error(f"Fail to restore the flash block protection")
                sys_exit(logger, False, ret)

        ret = ameba.post_process()
        if ret != ErrType.OK:
            logger.error("Post process fail")
            sys_exit(logger, False, ret)
    else:
        # erase
        ret = ameba.prepare()
        if ret != ErrType.OK:
            logger.error("Erase prepare fail")
            sys_exit(logger, False, ret)

        if chip_erase:
            ret = ameba.erase_flash_chip()
            if ret != ErrType.OK:
                logger.error("Chip erase fail")
                sys_exit(logger, False, ret)
            sys_exit(logger, True, ret)

        ret = ameba.validate_config_for_erase()
        if ret != ErrType.OK:
            sys_exit(logger, False, ret)

        ret = ameba.post_validate_config_for_erase()
        if ret != ErrType.OK:
            sys_exit(logger, False, ret)

        if (not profile_info.is_ram_address(memory_info.start_address)):
            flash_status = FlashBPS()
            ret = ameba.check_and_process_flash_lock(flash_status)
            if ret != ErrType.OK:
                logger.error("Erase fail")
                sys_exit(logger, False, ret)

        ret = ameba.erase_flash()
        if ret != ErrType.OK:
            logger.error(f"Erase {memory_type} failed")
            sys_exit(logger, False, ret)

        if (not profile_info.is_ram_address(memory_info.start_address)) and flash_status.need_unlock:
            logger.info("Restore the flash block protection...")
            ret = ameba.lock_flash(flash_status.protection)
            if ret != ErrType.OK:
                logger.error(f"Fail to restore the flash block protection")
                sys_exit(logger, False, ret)

    sys_exit(logger, True, ret)


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-d', '--download', action='store_true', help='download images')
    parser.add_argument('-f', '--profile', type=str, help='device profile')
    parser.add_argument('-p', '--port', nargs="+", help='serial port')
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

    parser.add_argument('--chip-erase', action='store_true', help='chip erase')
    parser.add_argument('--log-level', default='info', help='log level')
    parser.add_argument('--partition-table', help="layout info, list")

    parser.add_argument('--remote-server', type=str, help='remote serial server IP address')
    parser.add_argument('--remote-password', type=str, help='remote serial server validation password')
    parser.add_argument('--no-reset', action='store_true', help='do not reset after flashing finished')

    args = parser.parse_args()
    download = args.download
    profile = args.profile
    image = args.image
    image_dir = args.image_dir
    chip_erase = args.chip_erase
    serial_ports = args.port
    serial_baudrate = args.baudrate
    log_level = args.log_level.upper()
    log_file = args.log_file
    erase = args.erase
    start_addr = args.start_address
    end_addr = args.end_address
    size = args.size
    mem_t = args.memory_type
    partition_table = decoder_partition_string(args.partition_table)

    remote_server = args.remote_server
    remote_port = 58916
    remote_password = args.remote_password
    no_reset = args.no_reset

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
    logger = create_logger("main", log_level=log_level, file=log_f)
    if log_file is not None:
        logger.info(f"Log file: {log_file}")

    logger.info(f"Flash Version: {version_info.version}")

    if remote_server:
        logger.info(f"Using remote serial server: {remote_server}:{remote_port}")

    if profile is None:
        logger.error('Invalid arguments, no device profile specified')
        parser.print_usage()
        sys.exit(1)

    if not os.path.exists(profile):
        logger.error("Device profile '" + profile + "' does not exist")
        sys.exit(1)
    logger.info(f'Device profile: {profile}')

    if serial_ports is None:
        logger.error('Invalid arguments, no serial port specified')
        parser.print_usage()
        sys.exit(1)
    logger.info(f'Serial port: {serial_ports}')

    if serial_baudrate is None:
        logger.error('Invalid arguments, no serial baudrate specified')
        parser.print_usage()
        sys.exit(1)
    logger.info(f'Baudrate: {serial_baudrate}')

    if all([download, erase]):
        logger.warning("Download and erase are set true, only do image download ")
    elif not (download or erase or chip_erase):
        logger.error("Download or erase or chip-erase should be set")
        sys.exit(1)

    memory_info = None
    images_info = None
    if download:
        # download
        if (image is None) and (image_dir is None) and (partition_table is None):
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
                sys.exit(1)
            download_img_info.start_address = start_address

            if memory_type == MemoryInfo.MEMORY_TYPE_NAND:
                if end_addr is None:
                    logger.error(f"End address is required for nand flash download")
                    sys.exit(1)

                try:
                    end_address = int(end_addr, 16)
                except Exception as err:
                    logger.error(f"End address is invalid: {err}")
                    sys.exit(1)
            else:
                end_address = start_address + os.path.getsize(image)

            download_img_info.end_address = end_address
            download_img_info.memory_type = memory_type
            download_img_info.mandatory = True
            images_info = [download_img_info]
        elif partition_table is not None:
            images_info = []
            for img_info in partition_table:
                img_json = ImageInfo(** img_info)
                if sys.platform == "win32":
                    img_p = convert_mingw_path_to_windows(img_json.image_name)
                    img_json.image_name = img_p
                img_json.description = os.path.basename(img_json.image_name)
                images_info.append(img_json)
        else:
            images_info = None
            if not os.path.exists(image_dir):
                logger.error(f"Image directory {image_dir} does not exist")
                sys.exit(1)

            logger.info(f'Image dir: {image_dir}')
        if images_info:
            logger.info(f'Image info:')
            for img_info in images_info:
                for key, value in img_info.__repr__().items():
                    if key == "ImageName":
                        key = "Image"
                    logger.info(f'> {key}: {value}')
    else:
        # erase
        if all([chip_erase, erase]):
            logger.warning(f"Both chip erase and erase are enabled, do chip erase only")
        if not chip_erase:
            memory_info = MemoryInfo()
            if start_addr is None:
                logger.error(f"Start address is required for erase flash")
                sys.exit(1)

            try:
                start_address = int(start_addr, 16)
            except Exception as err:
                logger.error(f"Start address is invalid: {err}")
                sys.exit(1)
            memory_info.start_address = start_address

            if memory_type is None:
                logger.error("Memory type is required for erase")
                sys.exit(1)

            memory_info.memory_type = memory_type

            if memory_type == MemoryInfo.MEMORY_TYPE_NAND:
                if end_addr is None:
                    logger.error(f"End address is required for nand flash download")
                    sys.exit(1)
            else:
                if size is None:
                    logger.error(f"Erase size is required")
                    sys.exit(1)

            if end_addr:
                try:
                    end_address = int(end_addr, 16)
                except Exception as err:
                    logger.error(f"End address is invalid: {err}")
                    sys.exit(1)
            else:
                end_address = 0

            memory_info.size_in_kbyte = size

            if end_address == 0:
                end_address = start_address + size
            memory_info.end_address = end_address

    if chip_erase:
        logger.info(f"Chip erase: {chip_erase}")
        if memory_type is None:
            logger.warning("Memory type is required for chip erase")
            sys.exit(1)
        if memory_type != MemoryInfo.MEMORY_TYPE_NOR:
            logger.warning("Memory type should be 'nor' for chip erase")
    else:
        logger.info(f"Chip erase: False")

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

    # load settings
    setting_path = os.path.realpath(os.path.join(RtkUtils.get_executable_root_path(), setting_file))
    logger.info(f"Settings path: {setting_path}")
    try:
        if os.path.exists(setting_path):
            dt = JsonUtils.load_from_file(setting_path, need_decrypt=False)
            settings = RtSettings(** dt)
        else:
            logger.debug(f"{setting_file} not exists!")
            settings = RtSettings(**{})
    except Exception as err:
        logger.error(f"Load settings exception: {err}")
        settings = RtSettings(** {})
    # save Setting.json
    try:
        if no_reset:
            settings.post_process = "NONE"
        else:
            settings.post_process = "RESET"
        JsonUtils.save_to_file(setting_path, settings.__repr__())
    except Exception as err:
        logger.debug(f"save {setting_file} exception: {err}")

    threads_list = []

    for sp in serial_ports:
        flash_thread = threading.Thread(target=flash_process_entry, args=(
        profile_info, sp, serial_baudrate, image_dir, settings, deepcopy(images_info), chip_erase,
        memory_type, memory_info, download, log_level, log_f,
        remote_server, remote_port, remote_password))
        threads_list.append(flash_thread)
        flash_thread.start()

    for thred in threads_list:
        thred.join()

    logger.info(f"All flash threads have completed")


if __name__ == "__main__":
    main(len(sys.argv), sys.argv[1:])
