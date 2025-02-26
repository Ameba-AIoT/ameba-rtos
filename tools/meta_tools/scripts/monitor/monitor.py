#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import argparse
import codecs
import json
import os
import queue
import subprocess
import sys
import threading
import colorama
import serial
from serial.tools import list_ports, miniterm
from colorama.initialise import wrap_stream
from typing import Union

from base import __version__
from base.console_parser import ConsoleParser
from base.console_reader import ConsoleReader
from base.constants import CTRL_C, CTRL_H, EVENT_QUEUE_TIMEOUT, LAST_LINE_THREAD_INTERVAL, TAG_CMD, TAG_KEY, TAG_SERIAL, \
    TAG_SERIAL_FLUSH
from base.key_config import EXIT_KEY, EXIT_MENU_KEY, MENU_KEY
from base.log_handler import LogHandler
from base.color_output import print_normal, print_yellow, print_red
from base.serial_handler import SerialHandler, SerialStopException
from base.serial_reader import LinuxReader, SerialReader

key_description = miniterm.key_description

TOOLCHAIN_DEFAULT_PATH_WINDOWS = r'C:\msys64\opt\rtk-toolchain'
TOOLCHAIN_DEFAULT_PATH_LINUX = '/opt/rtk-toolchain'
SDK_QUERY_CFG_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'query.json')


class Monitor():
    """
    Monitor application base class.

    This was originally derived from miniterm.Miniterm, but it turned out to be easier to write from scratch for this
    purpose.

    Main difference is that all event processing happens in the main thread, not the worker threads.
    """

    def __init__(
            self,
            serial_instance: serial.Serial,
            elf_file: str,
            toolchain_path: str = '',
            eol: str = 'CRLF',
            decode_coredumps: bool = False,
            target_os: Union[str, None] = None,
            enable_address_decoding: bool = True,
            timestamps: bool = False,
            rom_file: Union[str, None] = None,
    ):
        self.event_queue = queue.Queue()
        self.cmd_queue = queue.Queue()
        self.console = miniterm.Console()
        # wrap for windows support ANSI
        self.console.output = wrap_stream(self.console.output, autoreset=True, convert=None, strip=None, wrap=True)

        self.target_os = target_os
        self.elf_file = elf_file or ""
        self.rom_file = rom_file or ""
        self.elf_exists = os.path.exists(self.elf_file)
        self.log_handler = LogHandler(self.elf_file, self.console, timestamps, enable_address_decoding, toolchain_path,
                                      rom_elf_file=rom_file)
        if not self.elf_exists:
            print_yellow("Not set elf or elf not existed, will not handle core dump!")

        if self.target_os == "freertos":
            from base.coredump_freertos import CoreDump
            self.coredump = CoreDump(decode_coredumps, self.event_queue, self.log_handler, self.elf_file, self.rom_file,
                                     toolchain_path) if self.elf_exists else None
        elif self.target_os == "zephyr":
            from base.coredump_zephyr import CoreDump
            self.coredump = CoreDump(decode_coredumps, self.event_queue, self.log_handler, self.elf_file, self.rom_file,
                                     toolchain_path, ) if self.elf_exists else None
        else:
            print_yellow(f"Core dump not support os [{target_os}]!")
            self.coredump = None
            pass

        self.timeout_cnt = 0

        if isinstance(self, SerialMonitor):
            self.serial = serial_instance
            self.serial_reader = SerialReader(self.serial, self.event_queue)

        else:
            self.serial = subprocess.Popen([self.elf_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT, bufsize=0)
            self.serial_reader = LinuxReader(self.serial, self.event_queue)

        self.serial_handler = SerialHandler("", self.log_handler, target_os, self.serial, self.elf_file)

        self.console_parser = ConsoleParser(eol)
        self.console_reader = ConsoleReader(self.console, self.event_queue, self.cmd_queue, self.console_parser)

        # internal state
        self._invoke_processing_last_line_timer = None

    def __enter__(self):
        """ Use "with self" to temporarily disable monitoring behaviour """
        self.serial_reader.stop()
        self.console_reader.stop()

    def __exit__(self, exc_type, exc_val, exc_tb):  # type: ignore
        raise NotImplementedError

    def _pre_start(self):
        self.console_reader.start()
        self.serial_reader.start()

    def main_loop(self):
        self._pre_start()
        try:
            while self.console_reader.alive and self.serial_reader.alive:
                try:
                    self._main_loop()
                except KeyboardInterrupt:
                    print_yellow(
                        f"To exit from monitor please use '{key_description(EXIT_KEY)}'. "
                        f"Alternatively, you can use {key_description(MENU_KEY)} {key_description(EXIT_MENU_KEY)} to exit."
                    )
                    self.serial_write(codecs.encode(CTRL_C))
        except SerialStopException:
            print_normal("Stopping condition has been received\n")
        except KeyboardInterrupt:
            pass
        finally:
            try:
                self.console_reader.stop()
                self.serial_reader.stop()
                self.log_handler.stop_logging()
                # Cancelling _invoke_processing_last_line_timer is not
                # important here because receiving empty data doesn"t matter.
                self._invoke_processing_last_line_timer = None
            except Exception:  # noqa
                pass
            print_normal("\n")

    def serial_write(self, *args, **kwargs):
        raise NotImplementedError

    def invoke_processing_last_line(self):
        self.event_queue.put((TAG_SERIAL_FLUSH, b""), False)

    def _main_loop(self):
        try:
            item = self.cmd_queue.get_nowait()
        except queue.Empty:
            try:
                item = self.event_queue.get(timeout=EVENT_QUEUE_TIMEOUT)
            except queue.Empty:
                return

        event_tag, data = item
        if event_tag == TAG_CMD:
            self.serial_handler.handle_commands(data, self.console_reader, self.serial_reader)
        elif event_tag == TAG_KEY:
            self.serial_write(codecs.encode(data))
        elif event_tag == TAG_SERIAL:
            self.serial_handler.handle_serial_input(data, self.coredump)
            if self._invoke_processing_last_line_timer is not None:
                self._invoke_processing_last_line_timer.cancel()
            self._invoke_processing_last_line_timer = threading.Timer(LAST_LINE_THREAD_INTERVAL,
                                                                      self.invoke_processing_last_line)
            self._invoke_processing_last_line_timer.start()
            # If no further data is received in the next short period
            # of time then the _invoke_processing_last_line_timer
            # generates an event which will result in the finishing of
            # the last line. This is fix for handling lines sent
            # without EOL.
            # finalizing the line when coredump is in progress causes decoding issues
            # the coredump loader uses empty line as a sign for end-of-coredump
            # line is finalized only for non coredump data
        elif event_tag == TAG_SERIAL_FLUSH:
            self.serial_handler.handle_serial_input(data, self.coredump, True)
        else:
            raise RuntimeError("Bad event data %r" % ((event_tag, data),))


class SerialMonitor(Monitor):
    def __exit__(self, exc_type, exc_val, exc_tb):  # type: ignore
        """ Use "with self" to temporarily disable monitoring behaviour """
        self.console_reader.start()
        if self.elf_exists:
            self.serial_reader.gdb_exit = self.gdb_helper.gdb_exit  # type: ignore # write gdb_exit flag
        self.serial_reader.start()

    def _pre_start(self):
        super()._pre_start()
        self.serial_handler.start_cmd_sent = False

    def serial_write(self, *args, **kwargs):
        self.serial: serial.Serial
        try:
            self.serial.write(*args, **kwargs)
            self.timeout_cnt = 0
        except serial.SerialTimeoutException:
            if not self.timeout_cnt:
                print_yellow("Writing to serial is timing out. Please make sure that your application supports "
                             "an interactive console and that you have picked the correct console for serial communication.")
            self.timeout_cnt += 1
            self.timeout_cnt %= 3
        except serial.SerialException:
            pass  # this shouldn't happen, but sometimes port has closed in serial thread
        except UnicodeEncodeError:
            pass  # this can happen if a non-ascii character was passed, ignoring

    def _main_loop(self):
        super()._main_loop()


class LinuxMonitor(Monitor):
    def __exit__(self, exc_type, exc_val, exc_tb):  # type: ignore
        """ Use "with self" to temporarily disable monitoring behaviour """
        self.console_reader.start()
        self.serial_reader.start()

    def serial_write(self, *args, **kwargs):
        self.serial.stdin.write(*args, **kwargs)
        self.serial.stdin.flush()

    def check_gdb_stub_and_run(self, line):
        return  # fake function for linux target_os


def detect_port():
    """Detect connected ports and return the last one"""
    try:
        port_list = list_ports.comports()
        port: str = port_list[-1].device
        # keep the `/dev/ttyUSB0` default port on linux if connected
        if sys.platform == "linux":
            for p in port_list:
                if p.device == "/dev/ttyUSB0":
                    port = p.device
                    break
        print_yellow(f"--- Using autodetected port {port}")
        return port
    except IndexError:
        sys.exit("No serial ports detected.")


def main():
    colorama.init()
    parser = get_parser()
    args = parser.parse_args()

    if os.path.exists(SDK_QUERY_CFG_FILE):
        try:
            with open(SDK_QUERY_CFG_FILE, "r") as f:
                cfg = json.load(f)
        except:
            print_red('Fail to load query configuration file "' + SDK_QUERY_CFG_FILE + '"')
            sys.exit(2)
    else:
        print_red('Query configuration file "' + SDK_QUERY_CFG_FILE + '" does not exist')
        sys.exit(1)

    if args.device is None:
        print("Note: No device specified, monitor starts failed!")
        sys.exit(1)
    else:
        device = args.device.lower()

    if args.toolchain_dir is None:
        if os.name == "nt":
            toolchain_dir = TOOLCHAIN_DEFAULT_PATH_WINDOWS
        else:
            toolchain_dir = TOOLCHAIN_DEFAULT_PATH_LINUX
        print("No toolchain specified, use default toolchain: " + toolchain_dir)
    else:
        toolchain_dir = args.toolchain_dir

    if not os.path.exists(toolchain_dir):
        print("Error: Toolchain '" + toolchain_dir + "' does not exist")
        sys.exit(1)

    if device not in cfg["devices"].keys():
        print('Error: Unsupported device "' + device + '", valid values: ')
        [print(key) for key in cfg["devices"].keys()]
        sys.exit(1)
    else:
        chip = cfg["devices"][device]["chip"]

    toolchain = cfg["chips"][chip]["toolchain"]
    if os.name == "nt":
        toolchain_path = os.path.join(toolchain_dir, toolchain, "mingw32", "newlib")
    else:
        toolchain_path = os.path.join(toolchain_dir, toolchain, "linux", "newlib")
    toolchain_path = str(toolchain_path)
    if not os.path.exists(toolchain_path):
        print(f"Error: Toolchain '{toolchain_path}' does not exist")
        sys.exit(1)

    target_os = args.target_os or cfg["info"]["os"].lower()

    args.eol = args.eol or ("LF" if sys.platform == "linux" else "CRLF")

    elf_file = args.elf_file

    try:
        if target_os == "linux":
            serial_instance = None
            cls = LinuxMonitor
            print_yellow(f"--- monitor {__version__} on linux ---")
        else:
            # The port name is changed in cases described in the following lines.
            # Use a local argument and avoid the modification of args.port.
            port = args.port

            # if no port was set, detect connected ports and use one of them
            if port is None:
                port = detect_port()
            # GDB uses CreateFile to open COM port, which requires the COM name to be r"\\.\COMx" if the COM
            # number is larger than 10
            if os.name == "nt" and port.startswith("COM"):
                port = port.replace("COM", r"\\.\COM")
                print_yellow("--- WARNING: GDB cannot open serial ports accessed as COMx")
                print_yellow(f"--- Using {port} instead...")
            elif port.startswith("/dev/tty.") and sys.platform == "darwin":
                port = port.replace("/dev/tty.", "/dev/cu.")
                print_yellow("--- WARNING: Serial ports accessed as /dev/tty.* will hang gdb if launched.")
                print_yellow(f"--- Using {port} instead...")

            serial_instance = serial.serial_for_url(port, int(args.baud), do_not_open=True, exclusive=True)
            # setting write timeout is not supported for RFC2217 in pyserial
            if not port.startswith("rfc2217://"):
                serial_instance.write_timeout = 0.3

            cls = SerialMonitor
            print_yellow(
                "--- monitor {v} on {p.name} {p.baudrate} ---".format(v=__version__, p=serial_instance))
        rom_file = ""
        monitor = cls(serial_instance, elf_file, toolchain_path, args.eol, args.decode_coredumps,
                      target_os, args.enable_address_decoding, rom_file=rom_file)

        print_yellow("--- Quit: {q} | Menu: {m} | Help: {m} followed by {h} ---".format(
            q=key_description(EXIT_KEY),
            m=key_description(MENU_KEY),
            h=key_description(CTRL_H)))

        monitor.main_loop()
    except KeyboardInterrupt:
        pass


def get_parser():
    parser = argparse.ArgumentParser("monitor - a serial output monitor.")

    parser.add_argument("-b", "--baud", type=int, default=1500000, help="Serial port baud rate, default is 1500000")
    parser.add_argument("--decode_coredumps", action="store_true",
                        help=f"If set will handle core dumps found in serial output. Default is False")
    parser.add_argument("--device", help="Set device name.")
    parser.add_argument("--enable_address_decoding", action="store_true",
                        help="Print lines about decoded addresses from the application ELF file, default is False")
    parser.add_argument("--elf_file", nargs="?", help="ELF or AXF file of application")
    parser.add_argument("--eol", choices=["CR", "LF", "CRLF"],
                        help="End of line to use when sending to the serial port. Defaults to LF for Linux targets and CR otherwise.")
    parser.add_argument("-p", "--port", help="Serial port device. If not set, a connected port will be used."
                                             "Default: `/dev/ttyUSB0` if connected." if sys.platform == "linux" else "")
    parser.add_argument("--rom_file", nargs="?", help="Rom of application")
    parser.add_argument("--target_os", help="Target os name (used when core dump decoding is enabled)")
    parser.add_argument("--timestamps", default=False, action="store_true",
                        help="Add timestamp for each line. Default is False")
    parser.add_argument("--toolchain_dir", help="Set toolchain dir. If not set, will get from config.")

    return parser


if __name__ == "__main__":
    main()
