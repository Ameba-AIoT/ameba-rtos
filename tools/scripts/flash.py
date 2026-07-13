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

SCRIPT_DIR   = os.path.realpath(os.path.dirname(os.path.abspath(__file__)))
PROFILE_DIR  = os.path.realpath(os.path.join(SCRIPT_DIR, "../ameba/Flash/Devices/Profiles"))
FLASH_TOOL   = os.path.realpath(os.path.join(SCRIPT_DIR, "../ameba/Flash/AmebaFlash.py"))

# Flags that select a standalone operation in AmebaFlash.py (no firmware download).
# When any of these is present in the passthrough args, --download is NOT injected.
# Note: --key-prog is intentionally excluded — it must run in the same floader
# session as the firmware download, so --download is always injected alongside it.
_NON_DOWNLOAD_OPS = {'--erase', '-e', '--chip-erase', '--read-wifimac'}


class MemoryInfo:
    MEMORY_TYPE_RAM  = 0
    MEMORY_TYPE_NOR  = 1
    MEMORY_TYPE_NAND = 2


def _resolve_profile(device: str, mem_t: str) -> str:
    """Resolve a device name to an .rdev profile path."""
    path = os.path.join(PROFILE_DIR, device + ".rdev")
    if not os.path.exists(path):
        path = os.path.join(PROFILE_DIR, device + "_" + mem_t.upper() + ".rdev")
    return path


def _build_partition_table(images, mem_t: str, image_dir: str) -> str:
    """Convert flash.py's -i (name, start, end) triples to a base64 partition table."""
    if mem_t == "nand":
        memory_type = MemoryInfo.MEMORY_TYPE_NAND
    elif mem_t == "ram":
        memory_type = MemoryInfo.MEMORY_TYPE_RAM
    else:
        memory_type = MemoryInfo.MEMORY_TYPE_NOR

    partition_table = []
    for name, start_str, end_str in images:
        image_path = os.path.realpath(os.path.join(image_dir, name))
        try:
            start_addr = int(start_str, 16)
        except Exception as e:
            raise ValueError(f"Invalid start address '{start_str}': {e}")
        try:
            end_addr = int(end_str, 16)
        except Exception as e:
            raise ValueError(f"Invalid end address '{end_str}': {e}")

        partition_table.append({
            "ImageName":    image_path,
            "StartAddress": start_addr,
            "EndAddress":   end_addr,
            "FullErase":    False,
            "MemoryType":   memory_type,
            "Mandatory":    True,
            "Description":  os.path.basename(image_path),
        })

    json_str  = json.dumps(partition_table)
    b64_bytes = base64.b64encode(json_str.encode("utf-8"))
    return b64_bytes.decode("utf-8")


def run_flash(argv):
    result = subprocess.run([sys.executable, FLASH_TOOL] + argv)
    sys.exit(result.returncode)


def main():
    # flash.py owns only three concerns:
    #   1. --device   → auto-resolve to --profile (flash.py-exclusive shorthand)
    #   2. -i/--image → nargs=3 custom format, converted to --partition-table
    #   3. --image-dir → default image search path
    #   4. -m/--memory-type → needed for image-layout conversion; re-added to cmds
    #
    # Every other argument (port, baudrate, log-level, chip-erase, no-reset,
    # remote-server, key-prog and its sub-args, etc.) is captured in `passthrough`
    # and forwarded to AmebaFlash.py unchanged.  Adding new AmebaFlash.py features
    # therefore does NOT require touching this file.

    # flash.py exclusively handles four arguments; all others pass through to AmebaFlash.py.
    #
    # Why keep -p/--port and -b/--baudrate here (instead of pure passthrough)?
    #   - -b/--baudrate has a default of 1500000 in flash.py.  AmebaFlash.py has NO
    #     default; omitting -b causes AmebaFlash.py to fail with "no baudrate specified".
    #     Keeping it here preserves the long-standing default for all existing callers.
    #   - -p/--port is validated early so callers get the original error message
    #     ("Serial port is invalid") rather than a cryptic AmebaFlash.py failure.
    #
    # Every other argument (log-level, chip-erase, no-reset, remote-server,
    # key-prog and its sub-args, …) is in `passthrough` and forwarded unchanged.
    # Adding new AmebaFlash.py features never requires touching this file.

    parser = argparse.ArgumentParser(
        add_help=False,
        description="flash.py — thin wrapper around AmebaFlash.py; "
                    "use AmebaFlash.py --help for the full option list.")

    parser.add_argument('-dev', '--device', required=True, type=str,
                        help='device name (e.g. RTL8721F); auto-resolves to device profile')
    parser.add_argument('-p', '--port', nargs='+',
                        help='serial port(s)')
    parser.add_argument('-b', '--baudrate', type=int, default=1500000,
                        help='serial port baud rate (default: 1500000)')
    parser.add_argument('-m', '--memory-type',
                        choices=['nor', 'nand', 'ram'], default='nor',
                        help='memory type (default: nor)')
    parser.add_argument('-i', '--image', nargs=3, action='append',
                        metavar=('image-name', 'start-address', 'end-address'),
                        help='custom image: name start_addr(hex) end_addr(hex); '
                             'may be specified multiple times')
    parser.add_argument('--image-dir', type=str,
                        help='image directory (default: current working directory)')
    parser.add_argument('-h', '--help', action='store_true',
                        help='show this help and AmebaFlash.py help, then exit')

    args, passthrough = parser.parse_known_args()

    if args.help:
        parser.print_help()
        print()
        run_flash(['--help'])   # also shows AmebaFlash.py help (exits inside)

    if not args.port:
        raise ValueError("Serial port is invalid")

    mem_t     = args.memory_type
    image_dir = os.path.realpath(args.image_dir) if args.image_dir else os.getcwd()
    profile   = _resolve_profile(args.device, mem_t)

    # Inject --download only when no explicit non-download operation was requested
    is_download = not any(f in passthrough for f in _NON_DOWNLOAD_OPS)

    cmds = []
    if is_download:
        cmds.append('--download')

    cmds += ['--profile',      profile]
    cmds += ['--port']  + args.port
    cmds += ['--baudrate',     str(args.baudrate)]
    cmds += ['--memory-type',  mem_t]

    if not args.image:
        cmds += ['--image-dir', image_dir]
    else:
        cmds += ['--partition-table', _build_partition_table(args.image, mem_t, image_dir)]

    # Forward all remaining args (log-level, chip-erase, no-reset, remote-server,
    # key-prog and its sub-args, …) unchanged.
    cmds += passthrough

    run_flash(cmds)


if __name__ == "__main__":
    main()
