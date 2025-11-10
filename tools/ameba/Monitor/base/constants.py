#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


# Control-key characters
CTRL_C = "\x03"
CTRL_H = "\x08"

# VT100 escape sequences
CONSOLE_STATUS_QUERY = "\x1b[5n"

# Command parsed from console inputs
CMD_STOP = 1
CMD_OUTPUT_TOGGLE = 5
CMD_TOGGLE_LOGGING = 6
CMD_TOGGLE_TIMESTAMPS = 7

# Tags for tuples in queues
TAG_KEY = 0
TAG_SERIAL = 1
TAG_SERIAL_FLUSH = 2
TAG_CMD = 3

EVENT_QUEUE_TIMEOUT = 0.03  # timeout before raising queue.Empty exception in case of empty event queue

GDB_UART_CONTINUE_COMMAND = "+$c#63"
GDB_EXIT_TIMEOUT = 0.3  # time delay between exit and writing GDB_UART_CONTINUE_COMMAND

# workaround for data sent without EOL
# if no data received during the time, last line is considered finished
LAST_LINE_THREAD_INTERVAL = 0.1
# closing wait timeout for serial port
ASYNC_CLOSING_WAIT_NONE = 0xFFFF  # don't wait at all

RECONNECT_DELAY = 0.5  # timeout between reconnect tries
CHECK_ALIVE_FLAG_TIMEOUT = 0.25  # timeout for checking alive flags (currently used by serial reader)

TIME_FORMAT = "%Y-%m-%d %H:%M:%S.%f"

GDB_PRINTF_REGISTER_INFO_START = "Register Infos Start"
GDB_PRINTF_REGISTER_INFO_END = "Register Infos END"

GDB_PRINTF_BACKTRACE_START = "BackTrace Start"
GDB_PRINTF_BACKTRACE_END = "BackTrace END"
