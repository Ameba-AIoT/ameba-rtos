#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import re
import sys

import serial.tools.list_ports

RTK_USB_VID = 0x0BDA

PORT_TYPE_USB = "USB"
PORT_TYPE_UART = "UART"


def natural_port_key(port_name):
    match = re.search(r"(\d+)$", port_name or "")
    return (int(match.group(1)), port_name) if match else (sys.maxsize, port_name or "")


def get_port_type(port_info):
    return PORT_TYPE_USB if port_info.vid == RTK_USB_VID else PORT_TYPE_UART


def format_port_label(port_info):
    desc = port_info.description or ""
    # Strip trailing " (COMxx)" that duplicates the device name.
    desc = re.sub(r'\s*\(COM\d+\)\s*$', '', desc, flags=re.IGNORECASE).strip()
    return (
        f"{port_info.device}  ({desc})"
        if desc and desc.lower() != "n/a"
        else port_info.device
    )


def scan_serial_ports(port_type=None):
    """Scan serial ports of current PC, sorted by natural port name.

    port_type: PORT_TYPE_USB / PORT_TYPE_UART to keep only that type, None for all.
    """
    ports = serial.tools.list_ports.comports()
    if port_type is not None:
        ports = [p for p in ports if get_port_type(p) == port_type]
    return sorted(ports, key=lambda p: natural_port_key(p.device))
