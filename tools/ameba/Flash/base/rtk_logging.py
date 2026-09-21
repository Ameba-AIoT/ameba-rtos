#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import re
import sys
import uuid
import stat
import logging
from datetime import datetime

from colorama import Fore, Style, init

from .rtk_utils import RtkUtils

# init Colorama
init(autoreset=True)


def create_default_log_file(serial_ports=None):
    log_dir = os.path.join(RtkUtils.get_executable_root_path(), "log")
    os.makedirs(log_dir, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    port_suffix = ""
    if serial_ports is not None and len(serial_ports) == 1:
        safe_port = re.sub(r"[^A-Za-z0-9._-]+", "_", str(serial_ports[0])).strip("._-")
        port_suffix = f"_{safe_port or 'port'}"
    log_file = os.path.join(
        log_dir, f"{timestamp}_{uuid.uuid4().hex[:8]}{port_suffix}.log")
    with open(log_file, "x", encoding="utf-8"):
        pass
    return log_file


def prepare_output_log_file(output_log_file):
    output_path = os.path.abspath(output_log_file)
    output_dir = os.path.dirname(output_path)
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)

    if os.path.exists(output_path):
        if not os.path.isfile(output_path):
            raise OSError(f"Log file path is not a regular file: {output_path}")
        if not os.stat(output_path).st_mode & stat.S_IWRITE:
            raise PermissionError(f"Log file is read-only: {output_path}")

    with open(output_path, "ab"):
        pass
    return output_path


def create_logger(name, log_level="INFO", stream=sys.stdout, file=None,
                  additional_files=None):
    log_level = log_level.upper() if log_level else "INFO"
    if log_level == "DEBUG":
        level = logging.DEBUG
    elif log_level == "WARNING":
        level = logging.WARNING
    elif log_level == "ERROR":
        level = logging.ERROR
    elif log_level == "FATAL":
        level = logging.FATAL
    else:
        level = logging.INFO

    logger = logging.getLogger(name)

    formatter = logging.Formatter(
        fmt=f'[%(asctime)s.%(msecs)03d][%(levelname)s] [{name}]%(message)s',
        datefmt='%Y-%m-%d %H:%M:%S')

    # addLevelName is global and idempotent; safe to (re)set on every call.
    logging.addLevelName(logging.DEBUG, f"D")
    logging.addLevelName(logging.INFO, f"I")
    logging.addLevelName(logging.WARNING, f"{Fore.YELLOW}W{Style.RESET_ALL}")
    logging.addLevelName(logging.ERROR, f"{Fore.RED}E{Style.RESET_ALL}")
    logging.addLevelName(logging.FATAL, f"{Fore.RED}{Style.BRIGHT}F{Style.RESET_ALL}")

    # File handlers: reconcile all requested output files on every call so that
    # runtime Save Log changes take effect and --log-file receives live output.
    requested_files = []
    for requested_file in [file] + list(additional_files or []):
        if requested_file is None:
            continue
        requested_path = os.path.abspath(requested_file)
        if requested_path not in requested_files:
            requested_files.append(requested_path)

    existing_handlers = {
        os.path.abspath(h.baseFilename): h
        for h in logger.handlers if isinstance(h, logging.FileHandler)
    }
    for path, handler in existing_handlers.items():
        if path not in requested_files:
            logger.removeHandler(handler)
            handler.close()
    for path in requested_files:
        if path not in existing_handlers:
            file_handler = logging.FileHandler(path, mode='a')
            file_handler.setFormatter(formatter)
            logger.addHandler(file_handler)

    # Console handler: add once per logger (FileHandler is a StreamHandler
    # subclass, so exclude it when checking for an existing console handler).
    has_console = any(
        isinstance(h, logging.StreamHandler) and not isinstance(h, logging.FileHandler)
        for h in logger.handlers
    )
    if not has_console:
        consoleHandler = logging.StreamHandler(stream)
        consoleHandler.setFormatter(formatter)
        logger.addHandler(consoleHandler)

    # Reorder existing handlers as well. This is needed when the GUI enables
    # "Save Log" at runtime: the new file handler would otherwise be appended
    # after the console handler created by an earlier call.
    file_handlers = [h for h in logger.handlers if isinstance(h, logging.FileHandler)]
    console_handlers = [
        h for h in logger.handlers
        if isinstance(h, logging.StreamHandler) and not isinstance(h, logging.FileHandler)
    ]
    other_handlers = [
        h for h in logger.handlers
        if h not in file_handlers and h not in console_handlers
    ]
    ordered_handlers = file_handlers + console_handlers + other_handlers
    if logger.handlers != ordered_handlers:
        for h in logger.handlers[:]:
            logger.removeHandler(h)
        for h in ordered_handlers:
            logger.addHandler(h)

    logger.propagate = False  # Prevent logging from propagating to the root logger
    logger.setLevel(level)
    return logger
