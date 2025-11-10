#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import sys
import os
import argparse
import base64
import json
import subprocess

PROJECT_ROOT_DIR = os.path.realpath(os.path.dirname(os.path.abspath(__file__)))
PROFILE = os.path.realpath(os.path.join(PROJECT_ROOT_DIR, "../tools/ameba/ImageTool/Devices/Profiles/AmebaDplus_FreeRTOS_NOR.rdev"))
FLASH_TOOL = os.path.realpath(os.path.join(PROJECT_ROOT_DIR, "../tools/ameba/Flash/flash.py"))


class MemoryInfo:
    MEMORY_TYPE_RAM = 0
    MEMORY_TYPE_NOR = 1
    MEMORY_TYPE_NAND = 2


def run_flash(argv):
    cmds = [sys.executable, FLASH_TOOL] + argv
    result = subprocess.run(cmds)
    if result.returncode != 0:
        sys.exit(1)
    else:
        sys.exit(0)


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-p', '--port', nargs="+", help='serial port')
    parser.add_argument('-b', '--baudrate', type=int, default=1500000, help='serial port baud rate')
    parser.add_argument('-m', '--memory-type', choices=['nor', 'nand', 'ram'], default="nor", help='specified memory type')
    parser.add_argument('-i', '--image', nargs=3, action='append', metavar=('image-name', 'start-address', 'end-address'),
                        help="user define image layout")
    parser.add_argument('-o', '--log-file', type=str, help='output log file with path')

    parser.add_argument('--remote-server', type=str, help='remote serial server IP address')
    parser.add_argument('--remote-password', type=str, help='remote serial server validation password')

    parser.add_argument('--chip-erase', action='store_true', help='chip erase')
    parser.add_argument('--log-level', default='info', help='log level')
    parser.add_argument('--no-reset', action='store_true', help='do not reset after flashing finished')

    args = parser.parse_args()
    ports = args.port
    serial_baudrate = args.baudrate
    mem_t = args.memory_type
    images = args.image

    log_file = args.log_file
    log_level = args.log_level.upper()

    remote_server = args.remote_server

    cmds = ["--download", "--profile", PROFILE]

    if log_file is not None:
        log_path = os.path.dirname(log_file)
        if log_path:
            if not os.path.exists(log_path):
                os.makedirs(log_path, exist_ok=True)
            log_f = log_file
        else:
            log_f = os.path.join(os.getcwd(), log_file)

        cmds.append("--log-file")
        cmds.append(log_f)
    else:
        log_f = None

    if not ports:
        raise ValueError("Serial port is invalid")

    cmds.append("--port")
    cmds.extend(ports)
    cmds.append(f"--baudrate")
    cmds.append(f"{serial_baudrate}")
    cmds.append(f"--memory-type")
    cmds.append(f"{mem_t}")
    cmds.append(f"--log-level")
    cmds.append(f"{log_level}")

    if remote_server:
        cmds.append("--remote-server")
        cmds.append(remote_server)
    if args.remote_password:
        cmds.append("--remote-password")
        cmds.append(str(args.remote_password))

    if args.chip_erase:
        cmds.append("--chip-erase")

    if args.no_reset:
        cmds.append("--no-reset")

    if not images:
        cmds.append(f"--image-dir")
        cmds.append(PROJECT_ROOT_DIR)
    else:
        partition_table = []

        if mem_t == "nand":
            memory_type = MemoryInfo.MEMORY_TYPE_NAND
        elif mem_t == "ram":
            memory_type = MemoryInfo.MEMORY_TYPE_RAM
        else:
            memory_type = MemoryInfo.MEMORY_TYPE_NOR

        # 1. Argparse.images format [[image-name, start-address, end-address], ...]
        for group in images:
            image_name_with_path = os.path.realpath(os.path.join(PROJECT_ROOT_DIR, group[0]))
            image_name = os.path.basename(image_name_with_path)
            try:
                start_addr = int(group[1], 16)
            except Exception as err:
                raise ValueError(f"Start addr in invalid: {err}")
                sys.exit(1)

            try:
                end_addr = int(group[2], 16)
            except Exception as err:
                raise ValueError(f"End addr in invalid: {err}")
                sys.exit(1)

            partition_table.append({
                "ImageName": image_name_with_path,
                "StartAddress": start_addr,
                "EndAddress": end_addr,
                "FullErase": False,
                "MemoryType": memory_type,
                "Mandatory": True,
                "Description": image_name
            })

        # 2. Convert list to json-str
        partition_table_json_string = json.dumps(partition_table)

        # 3. Encode json-str to bytes
        partition_table_bytes = partition_table_json_string.encode('utf-8')

        # 4. Base64 encode bytes
        partition_table_base64 = base64.b64encode(partition_table_bytes).decode('utf-8')

        cmds.append(f"--partition-table")
        cmds.append(f"{partition_table_base64}")

    run_flash(cmds)


if __name__ == "__main__":
    main()
