#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import argparse

from base import *

MinSupportedDeviceProfileMajorVersion = 1
MinSupportedDeviceProfileMinorVersion = 1
setting_file = "Setting.json"


def main(argc, argv):
    parser = argparse.ArgumentParser(description=None)
    parser.add_argument('-b', '--baudrate', type=int, help='serial port baud rate')
    parser.add_argument('-d', '--device-profile', help='device profile path')
    parser.add_argument('-i', '--image-dir', help='image directory')
    parser.add_argument('-p', '--port', help='serial port')
    parser.add_argument('-l', '--log-level', default="info", help='set log level')
    parser.add_argument('-f', '--log-file', help='set log file with path')

    args = parser.parse_args()

    device_profile = args.device_profile
    image_dir = args.image_dir
    serial_port = args.port
    serial_baudrate = args.baudrate
    log_level = args.log_level.upper()
    log_file = args.log_file

    if log_file is not None:
        log_path = os.path.dirname(log_file)
        if not os.path.exists(log_path):
            os.makedirs(log_path, exist_ok=True)
        log_f = log_file
    else:
        log_f = None
    logger = create_logger("flash", log_level=log_level, file=log_f)
    if log_file is not None:
        logger.info(f"Log file: {log_file}")

    if device_profile is None:
        logger.error('Invalid arguments, no device profile specified')
        parser.print_usage()
        sys.exit(1)

    if not os.path.exists(device_profile):
        logger.error("Device profile '" + device_profile + "' does not exist")
        sys.exit(1)

    if image_dir is None:
        logger.error('Invalid arguments, no image directory specified')
        parser.print_usage()
        sys.exit(1)

    if not os.path.exists(image_dir):
        logger.error("Image directory '" + image_dir + "' does not exist")
        sys.exit(1)

    if serial_port is None:
        logger.error('Invalid arguments, no serial port specified')
        parser.print_usage()
        sys.exit(1)

    if serial_baudrate is None:
        logger.error('Invalid arguments, no serial baudrate specified')
        parser.print_usage()
        sys.exit(1)

    logger.info(f'Device profile: {device_profile}')
    logger.info(f'Image directory: {image_dir}')
    logger.info(f'Serial port: {serial_port}')
    logger.info(f'Baudrate: {serial_baudrate}')

    # check device profile
    try:
        profile_json = JsonUtils.load_from_file(device_profile)
        if profile_json is None:
            logger.error(f"Fail to load device profile {device_profile}")
            sys.exit(1)
        profile = RtkDeviceProfile(**profile_json)
        ver = profile.get_version()
        if ver.major >= MinSupportedDeviceProfileMajorVersion and ver.minor >= MinSupportedDeviceProfileMinorVersion and profile.device_id != 0:
            logger.info(f"Device profile {device_profile} loaded")
        else:
            logger.error(f"Fail to load device profile {device_profile}, unsupported version {ver.__repr__()}")
            sys.exit(1)
    except Exception as err:
        logger.error(f"Load device profile {device_profile} exception: {err}")
        sys.exit(1)

    # check serial port
    try:
        port = serial.Serial(serial_port,
                             baudrate=profile.handshake_baudrate,
                             parity=serial.PARITY_NONE,
                             stopbits=serial.STOPBITS_ONE,
                             bytesize=serial.EIGHTBITS)
    except Exception as err:
        logger.error(f"Access serial port {serial_port} fail: {err}")
        sys.exit(1)

    # load settings
    setting_path = os.path.realpath(os.path.join(os.path.dirname(__file__), setting_file))
    try:
        if os.path.exists(setting_path):
            dt = JsonUtils.load_from_file(setting_path, need_decrypt=False)
            settings = RtSettings(**dt)
        else:
            logger.debug(f"Setting not exists!")
            settings = RtSettings(**{})
    except Exception as err:
        logger.error(f"Load settings exception: {err}")
        settings = RtSettings(**{})

    ameba = Ameba(profile, port, serial_baudrate, image_dir, settings, logger)

    if not ameba.check_protocol_for_download():
        sys.exit(1)

    logger.info(f"Image download start...")
    if ameba.prepare() != ErrType.OK:
        logger.error("Download prepare fail")
        sys.exit(-1)

    if ameba.post_verify_images() != ErrType.OK:
        sys.exit(-1)

    if ameba.download_images() != ErrType.OK:
        logger.error("Download image fail")
        sys.exit(-1)

    # save settings
    try:
        JsonUtils.save_to_file(setting_path, settings.__repr__())
    except Exception as err:
        logger.debug(f"save setting.json exception: {err}")

    if port.is_open:
        port.close()

    while port.is_open:
        pass

    if not port.is_open:
        logger.info(f"{port.port} closed.")

    sys.exit(0)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv[1:])
