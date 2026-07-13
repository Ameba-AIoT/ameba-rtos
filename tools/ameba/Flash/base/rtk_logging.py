#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import os
import sys
import logging
from colorama import Fore, Style, init

# init Colorama
init(autoreset=True)


def create_logger(name, log_level="INFO", stream=sys.stdout, file=None):
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

    # File handler: reconcile with the requested `file` on every call so that
    # toggling "Save Log" at runtime takes effect. Drop stale file handlers
    # (different path or logging now disabled) and add one if needed.
    wanted = os.path.abspath(file) if file else None
    kept = None
    for h in [h for h in logger.handlers if isinstance(h, logging.FileHandler)]:
        if wanted is not None and os.path.abspath(h.baseFilename) == wanted:
            kept = h
        else:
            logger.removeHandler(h)
            h.close()
    if wanted is not None and kept is None:
        fileHandler = logging.FileHandler(file, mode='a')
        fileHandler.setFormatter(formatter)
        logger.addHandler(fileHandler)

    logger.propagate = False  # Prevent logging from propagating to the root logger
    logger.setLevel(level)
    return logger
