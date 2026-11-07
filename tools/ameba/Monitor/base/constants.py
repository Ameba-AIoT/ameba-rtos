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
CMD_HARD_RESET = 8

# Console input handled by the monitor itself instead of being sent to the IC: the whole line "!reset", or a line
# containing "__HARD_RESET__" (ameba_pytest dut.py sends it, possibly after a --cmd-prefix such as '@').
HARD_RESET_COMMAND = "!reset"
HARD_RESET_TOKEN = "__HARD_RESET__"

# DTR/RTS sequence that pulls CHIP_EN low on the Realtek auto-download circuit, same as Flash/Reset.cfg.
# DTR and RTS asserted together leave CHIP_EN high, so they must never be pulsed together.
# Split in two: while RESET_HOLD is in effect the IC is held in reset, so log read after it cannot be stale.
RESET_HOLD = (("dtr", 0), ("rts", 1), ("delay", 200))
RESET_RELEASE = (("rts", 0), ("dtr", 0))
RESET_SEQUENCE = RESET_HOLD + RESET_RELEASE
# Log markers that prove the IC rebooted. ROM_MARKER is printed by the mask ROM of every Ameba IC right after a reset.
ROM_MARKER = b'ROM:['
BOOT_MARKERS = (ROM_MARKER, b'BOOT-I')
HARD_RESET_BOOT_TIMEOUT = 2  # seconds to wait for BOOT_MARKERS after the DTR/RTS sequence
# seconds a remote hard reset waits for the server's reply to reset_device()
REMOTE_RESET_TIMEOUT = 10
# seconds each startup phase waits (AT+LIST probe, --reset soft window, --reset hard window)
STARTUP_PHASE_TIMEOUT = 2
# seconds a hard reset waits for startup to finish: AT+LIST probe, then --reset soft window, the hard fallback's
# reset sequence (up to a remote round trip) and hard window; +1 s for the small sleeps and scheduling
STARTUP_WAIT_TIMEOUT = 3 * STARTUP_PHASE_TIMEOUT + REMOTE_RESET_TIMEOUT + 1
# seconds shutdown waits for a hard reset in progress: its worst case is the startup wait, then the reset sequence
# (remote round trip, or local hold + drain), then the boot log; +1 s for thread scheduling
HARD_RESET_SHUTDOWN_TIMEOUT = STARTUP_WAIT_TIMEOUT + REMOTE_RESET_TIMEOUT + HARD_RESET_BOOT_TIMEOUT + 1

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
