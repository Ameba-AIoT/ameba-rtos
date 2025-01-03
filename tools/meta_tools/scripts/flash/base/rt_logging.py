#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import sys
import logging
from colorama import Fore, Style, init

# init Colorama
init(autoreset=True)


def create_logger(name, log_level="INFO", stream=sys.stdout, file=None):
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
    if not logger.handlers:
        formatter = logging.Formatter(
            fmt='[%(asctime)s.%(msecs)03d][%(filename)s][%(lineno)s][%(levelname)s] %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S')

        logging.addLevelName(logging.DEBUG, f"D")
        logging.addLevelName(logging.INFO, f"I")
        logging.addLevelName(logging.WARNING, f"{Fore.YELLOW}W{Style.RESET_ALL}")
        logging.addLevelName(logging.ERROR, f"{Fore.RED}E{Style.RESET_ALL}")
        logging.addLevelName(logging.FATAL, f"{Fore.RED}{Style.BRIGHT}F{Style.RESET_ALL}")
        consoleHandler = logging.StreamHandler(stream)
        consoleHandler.setFormatter(formatter)
        logger.addHandler(consoleHandler)

        if file is not None:
            fileHandler = logging.FileHandler(file, mode='a')
            fileHandler.setFormatter(formatter)
            logger.addHandler(fileHandler)

        logger.propagate = False  # Prevent logging from propagating to the root logger
        logger.setLevel(level)
    return logger
