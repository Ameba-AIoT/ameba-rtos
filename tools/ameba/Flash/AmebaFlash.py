#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import argparse
import base64
import json
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
                        read_wifimac=False,
                        key_prog_specs=None,
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

        if memory_type == MemoryInfo.MEMORY_TYPE_NOR:
            ret, is_reburn = ameba.check_supported_flash_size()
            if ret != ErrType.OK:
                logger.error(f"Check supported flash size fail")
                sys_exit(logger, False, ret)

            if is_reburn:
                ameba.clean_up()
                # reset with remote params
                ameba = Ameba(profile_info, serial_port, serial_baudrate, image_dir, settings, logger,
                              download_img_info=images_info,
                              chip_erase=chip_erase,
                              memory_type=memory_type,
                              erase_info=memory_info,
                              remote_server=remote_server,
                              remote_port=remote_port,
                              remote_password=remote_password)

                logger.info(f"Re-prepare for reburn...")
                ret = ameba.prepare()
                if ret != ErrType.OK:
                    logger.error("Download prepare fail")
                    sys_exit(logger, False, ret)
            else:
                ret = ameba.show_device_info()
                if ret != ErrType.OK:
                    sys_exit(logger, False, ret)
        else:
            logger.info(f"Prepare for download...")
            ret = ameba.prepare()
            if ret != ErrType.OK:
                logger.error("Download prepare fail")
                sys_exit(logger, False, ret)

        # Merged single-session KEY_PROG: program the key(s) in the SAME floader
        # session as the encrypted download.  The floader caches each key in RAM
        # and encrypts/verifies with that copy (SW-AES), so no board reset is
        # needed between key programming and the encrypted write.
        if key_prog_specs:
            for idx, spec in enumerate(key_prog_specs):
                logger.info(f"Program key {idx + 1}/{len(key_prog_specs)} at "
                            f"{hex(spec['address'])} (same session as download)...")
                ret = ameba.burn_key(**spec)
                if ret != ErrType.OK:
                    logger.error(f"Program key at {hex(spec['address'])} fail")
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

        logger.info(f"Image download start...")  # customized, do not modify
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
    elif read_wifimac:
        # read wifi mac
        ret = ameba.prepare(show_device_info=False)
        if ret != ErrType.OK:
            logger.error("Prepare for read wifi-mac fail")
            sys_exit(logger, False, ret)

        logger.info(f'WiFiMAC: {ameba.device_info.get_wifi_mac_text()}')
    elif key_prog_specs is not None:
        # Standalone key-prog (no download in this invocation).
        ret = ameba.prepare(show_device_info=False)
        if ret != ErrType.OK:
            logger.error("Prepare for key program fail")
            sys_exit(logger, False, ret)

        for idx, spec in enumerate(key_prog_specs):
            logger.info(f"Program key {idx + 1}/{len(key_prog_specs)} at {hex(spec['address'])}...")
            ret = ameba.burn_key(**spec)
            if ret != ErrType.OK:
                logger.error(f"Program key at {hex(spec['address'])} fail")
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

    ameba.clean_up()

    sys_exit(logger, True, ret)


def flash_thread_runner(result_holder, index, port_name, logger, entry_args):
    """Run flash_process_entry in a worker thread and record its exit status.

    flash_process_entry signals its result by calling sys.exit() (via sys_exit()
    or initial_serial_port). A sys.exit() raised inside a thread only terminates
    that thread, so the main process must collect each thread's status here and
    propagate a non-zero exit code itself; otherwise a failed flash would still
    report success (exit 0) to the caller/CI."""
    try:
        flash_process_entry(*entry_args)
        result_holder[index] = 0
    except SystemExit as se:
        code = se.code
        if code is None:
            result_holder[index] = 0
        elif isinstance(code, int):
            result_holder[index] = code
        else:
            result_holder[index] = 1
    except Exception as err:
        logger.error(f"[{port_name}] flash thread exception: {err}")
        result_holder[index] = 1


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
    parser.add_argument('-m', '--memory-type', choices=['nor', 'nand', 'ram'], default=None,
                        help='specified memory type (optional, derived from device profile when omitted)')
    parser.add_argument('-e', '--erase', action='store_true', help='erase flash')
    parser.add_argument('-o', '--log-file', type=str, help='output log file with path')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {version_info.version}')

    parser.add_argument('--chip-erase', action='store_true', help='chip erase')
    parser.add_argument('--log-level', default='info', help='log level')
    parser.add_argument('--partition-table', help="layout info, list")
    parser.add_argument('--read-wifimac', action='store_true', help="read wifi mac")
    parser.add_argument('--key-prog', action='store_true',
                        help='program key into OTP via KEY_PROG command')
    parser.add_argument('--key-type', type=lambda x: int(x, 0),
                        help='OTP storage type: 0=physical eFuse, 1=logical eFuse')
    parser.add_argument('--key-lock', type=lambda x: int(x, 0),
                        help='lock policy: 0=Unlock (no lock), 1=NoAccess (write+read lock). '
                             'Add --key-lock-write-only to lock write only (NoWrite).')
    parser.add_argument('--key-lock-write-only', action='store_true',
                        help='modifier for --key-lock 1: lock write only (NoWrite), not read. '
                             'Requires --key-lock 1.')
    parser.add_argument('--key-addr', type=str,
                        help='OTP key target start address (hex)')
    parser.add_argument('--key-data', type=str,
                        help='key data as hex string (model-unique / 一型一密); '
                             'omit for device-unique / 一机一密 (device generates key with TRNG)')
    parser.add_argument('--key-raw-data', type=str,
                        help='verification plaintext as hex string (model-unique only); '
                             'auto-generated (os.urandom 16B) when not specified')
    parser.add_argument('--key-enc-data', type=str,
                        help='expected ciphertext as hex string (model-unique only); '
                             'auto-computed as AES-ECB(KeyData, RawData) when not specified')

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
    read_wifimac = args.read_wifimac
    key_prog = args.key_prog

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

    logger.info(f"AmebaFlash Version: {version_info.version}")

    if remote_server:
        logger.info(f"Using remote serial server: {remote_server}:{remote_port}")

    # Load settings early so CLI args can fall back to remembered values.
    setting_path = os.path.realpath(os.path.join(RtkUtils.get_executable_root_path(), setting_file))
    logger.info(f"Settings path: {setting_path}")
    try:
        if os.path.exists(setting_path):
            dt = JsonUtils.load_from_file(setting_path, need_decrypt=False)
            settings = RtSettings(**(dt or {}))
        else:
            logger.debug(f"{setting_file} not exists!")
            settings = RtSettings(**{})
    except Exception as err:
        logger.error(f"Load settings exception: {err}")
        settings = RtSettings(**{})

    # CLI arguments are runtime-only — do not load defaults from Settings.json
    # to avoid stale state from a previous GUI or CLI run leaking into this one.

    if profile is None:
        logger.error('Invalid arguments, no device profile specified')
        parser.print_usage()
        sys.exit(1)

    if not os.path.exists(profile):
        logger.error("Device profile '" + profile + "' does not exist")
        sys.exit(1)
    logger.info(f'Device profile: {profile}')

    # Load the device profile early so the memory type can be derived from it.
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

    # Memory type is optional on the command line: when '--memory-type' is given
    # it takes precedence (backward compatible), otherwise it is derived from the
    # device profile's MemoryType field.
    if memory_type is None:
        memory_type = profile_info.memory_type
        logger.info(f"Memory type derived from device profile: {memory_type}")
    else:
        logger.info(f"Memory type from argument: {memory_type}")

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

    # key-prog may run standalone OR be combined with download (single floader
    # session: program key(s) then immediately do the encrypted download without
    # a board reset; the floader encrypts/verifies with the RAM copy of the key).
    # chip-erase may run standalone OR be combined with download (erase the whole
    # chip, then program in the same run — download_images() does the erase first).
    exclusive_ops = [erase, chip_erase, read_wifimac]
    if download and chip_erase:
        # Combined chip-erase + download: chip-erase is part of the download flow,
        # so don't count it as a separate exclusive operation.
        exclusive_ops = [erase, read_wifimac]
    op_count = sum(1 for op in exclusive_ops if op) + (1 if (download or key_prog) else 0)
    if op_count != 1:
        logger.error("Exactly one of {download[+key-prog][+chip-erase], erase, chip-erase, read-wifimac, key-prog} should be set")
        sys.exit(1)

    # key_prog_specs: list of dicts (XTS needs two keys: ECB + CTR slots).
    # --key-addr / --key-data accept comma-separated lists; key-type/key-lock
    # apply to all entries.  Empty key-data entry = device-unique (TRNG on device).
    key_prog_specs = None
    if key_prog:
        if args.key_type is None or args.key_lock is None or args.key_addr is None:
            logger.error("key-type, key-lock and key-addr are required for key-prog")
            sys.exit(1)

        # Translate --key-lock user value to floader lock bitmask.
        #   --key-lock 0                  → lock=0 (no lock)
        #   --key-lock 1                  → lock=3 (NOWRITE | NOREAD = NoAccess)
        #   --key-lock 1 --key-lock-write-only → lock=2 (NOWRITE only)
        _LOCK_NOREAD  = 0x01   # KEY_PROG_LOCK_NOREAD  in floader_intf.h
        _LOCK_NOWRITE = 0x02   # KEY_PROG_LOCK_NOWRITE in floader_intf.h
        if args.key_lock == 0:
            _resolved_lock = 0
        elif args.key_lock == 1:
            if getattr(args, 'key_lock_write_only', False):
                _resolved_lock = _LOCK_NOWRITE          # 2: write-only lock
            else:
                _resolved_lock = _LOCK_NOWRITE | _LOCK_NOREAD  # 3: full lock
        else:
            logger.error(f"--key-lock must be 0 or 1 (got {args.key_lock}).")
            sys.exit(1)
        args.key_lock = _resolved_lock

        def parse_hex_bytes(name, value):
            if value is None or value == "":
                return b""
            if len(value) % 2 != 0:
                logger.error(
                    f"{name} length error: got {len(value)} hex chars, "
                    f"must be even (2 hex chars per byte).")
                sys.exit(1)
            try:
                return bytes.fromhex(value)
            except ValueError as err:
                logger.error(f"{name} contains non-hex characters: {err}")
                sys.exit(1)

        addr_list = [a.strip() for a in args.key_addr.split(",") if a.strip()]
        data_list = (args.key_data.split(",") if args.key_data else [])
        raw_list  = (args.key_raw_data.split(",") if args.key_raw_data else [])
        enc_list  = (args.key_enc_data.split(",") if args.key_enc_data else [])

        key_prog_specs = []
        for i, addr_str in enumerate(addr_list):
            try:
                key_addr = int(addr_str, 16)
            except Exception as err:
                logger.error(f"key-addr '{addr_str}' is invalid: {err}")
                sys.exit(1)
            key_prog_specs.append({
                "key_type": args.key_type,
                "lock": args.key_lock,
                "address": key_addr,
                "key_data": parse_hex_bytes("key-data", data_list[i].strip() if i < len(data_list) else None),
                "raw_data": parse_hex_bytes("key-raw-data", raw_list[i].strip() if i < len(raw_list) else None),
                "enc_data": parse_hex_bytes("key-enc-data", enc_list[i].strip() if i < len(enc_list) else None),
            })

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
            # A single image may actually be a merged image (e.g. image_all.bin)
            # that spans several partitions of the device profile layout. Mark it so
            # the download handler can split it by layout and skip the all-FF padding
            # between/after partitions instead of programming it.
            download_img_info.split_by_layout = True
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
    elif read_wifimac:
        # profile, port, baudrate, memory-type
        pass
    elif key_prog:
        pass
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
                end_address = start_address + size * 1024
            memory_info.end_address = end_address

    if (not read_wifimac) and (not key_prog):
        if chip_erase:
            logger.info(f"Chip erase: {chip_erase}")
            if memory_type is None:
                logger.warning("Memory type is required for chip erase")
                sys.exit(1)
            if memory_type not in (MemoryInfo.MEMORY_TYPE_NOR, MemoryInfo.MEMORY_TYPE_NAND):
                logger.warning("Memory type should be 'nor' or 'nand' for chip erase")
        else:
            logger.info(f"Chip erase: False")

    try:
        # load settings
        setting_path = os.path.realpath(os.path.join(RtkUtils.get_executable_root_path(), setting_file))
        logger.info(f"Settings path: {setting_path}")

        # Save engine-level settings (timeouts, retry counts, DTR/RTS timing, etc.)
        # CLI arguments are runtime-only — do NOT overwrite profile, port, baudrate,
        # image, image_dir, address, chip_erase, or post_process in Settings.json
        # to avoid polluting GUI preferences with one-off CLI choices.
        try:
            JsonUtils.save_to_file(setting_path, settings.to_dict())
        except Exception as err:
            logger.debug(f"save {setting_file} exception: {err}")

        # --no-reset: override post_process at runtime without persisting to disk.
        # When not passed, post_process keeps whatever value is in Settings.json
        # (set by the GUI: NONE / RESET / INDICATION / BOOT).
        if no_reset:
            settings.post_process = "NONE"

        threads_list = []
        # Default each port to failure (1); flash_thread_runner overwrites with the
        # real exit status so a thread that dies unexpectedly is still treated as a
        # failure for the process exit code.
        thread_results = [1] * len(serial_ports)

        for idx, sp in enumerate(serial_ports):
            entry_args = (
                profile_info, sp, serial_baudrate, image_dir, settings, deepcopy(images_info), chip_erase,
                memory_type, memory_info, download, log_level, log_f, read_wifimac,
                key_prog_specs, remote_server, remote_port, remote_password)
            flash_thread = threading.Thread(target=flash_thread_runner,
                                            args=(thread_results, idx, sp, logger, entry_args))
            threads_list.append(flash_thread)
            flash_thread.start()

        for thred in threads_list:
            thred.join()

        logger.info(f"All flash threads have completed")

        # Propagate a non-zero exit code if any port failed; a sys.exit() inside a
        # worker thread does not affect the main process on its own.
        failed_ports = [serial_ports[i] for i, code in enumerate(thread_results) if code != 0]
        if failed_ports:
            logger.error(f"Flash failed on port(s): {', '.join(str(p) for p in failed_ports)}")
            sys.exit(1)
        sys.exit(0)
    except Exception as err:
        logger.error(f"Main process exception: {err}")
        sys_exit(logger, False, err)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv[1:])
