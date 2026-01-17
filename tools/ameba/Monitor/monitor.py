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
    TAG_SERIAL_FLUSH, CMD_STOP
from base.key_config import EXIT_KEY, EXIT_MENU_KEY, MENU_KEY
from base.log_handler import LogHandler
from base.color_output import print_normal, print_yellow, print_red
from base.serial_handler import SerialHandler, SerialStopException
from base.serial_reader import LinuxReader, SerialReader
from typing import Optional, Dict, Any, List
import os

key_description = miniterm.key_description

TOOLCHAIN_DEFAULT_PATH_WINDOWS = r'C:\rtk-toolchain'
TOOLCHAIN_DEFAULT_PATH_LINUX = '/opt/rtk-toolchain'
REMOTE_PORT = 58916

class Monitor():
    """
    Monitor application base class.

    This was originally derived from miniterm.Miniterm, but it turned out to be easier to write from scratch for this
    purpose.

    Main difference is that all event processing happens in the main thread, not the worker threads.
    """

    def __init__(
            self,
            port,
            baudrate,
            elf_file: str = '',
            toolchain_path: str = '',
            eol: str = 'CRLF',
            decode_coredumps: bool = False,
            target_os: Union[str, None] = None,
            is_ca32: bool = False,
            enable_address_decoding: bool = True,
            timestamps: bool = False,
            rom_file: Union[str, None] = None,
            reset_mode: bool = False,
            debug: bool = False,
            remote_server: Optional[str] = None,
            remote_port: Optional[int] = None,
            remote_password: Optional[str] = None,
            log_enabled: bool = False,
            log_dir: Optional[List[str]] = None,
            logAGG: Optional[List[str]] = None
    ):
        self.event_queue = queue.Queue()
        self.cmd_queue = queue.Queue()
        self.output_queue = queue.Queue()
        self.target_os = target_os
        self.is_ca32 = is_ca32
        self.elf_file = elf_file or ""
        self.rom_file = rom_file or ""
        self.elf_exists = os.path.exists(self.elf_file)
        self.debug = debug
        self.logAGG_enabled = True if logAGG else False
        self.log_handler = LogHandler(self.elf_file, self.output_queue, timestamps, enable_address_decoding, toolchain_path,
                                      log_enabled, log_dir, port, logAGG, rom_elf_file=rom_file)
                                      
        if self.target_os == "freertos":
            from base.coredump_freertos import CoreDump
            self.coredump = CoreDump(decode_coredumps, self.event_queue, self.log_handler, self.elf_file, self.rom_file,
                                     toolchain_path) if self.elf_exists else None
        elif self.target_os == "freertos" and self.is_ca32:
            from base.coredump_freertos_ca32 import CoreDump
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
            self.serial_reader = SerialReader(port, baudrate, self.event_queue,
                                              reset_mode=reset_mode, debug=debug,
                                              remote_server=remote_server, remote_port=remote_port, remote_password=remote_password)

        else:
            self.serial = subprocess.Popen([self.elf_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT, bufsize=0)
            self.serial_reader = LinuxReader(self.serial, self.event_queue)

        self.serial_handler = SerialHandler("", self.log_handler, target_os, self.elf_file)

        self.console_parser = ConsoleParser(eol)
        self.console_reader = ConsoleReader(self.event_queue, self.cmd_queue, self.console_parser)

        # internal state
        self._invoke_processing_last_line_timer = None

    def __enter__(self):
        """ Use "with self" to temporarily disable monitoring behaviour """
        # self.serial_reader.stop()
        # self.console_reader.stop()
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):  # type: ignore
        raise NotImplementedError

    def _pre_start(self):
        self.console_reader.start()
        self.serial_reader.start()
        self.log_handler.start()
    def main_loop(self):
        self._pre_start()
        try:
            while self.console_reader.alive and self.serial_reader.alive:
                self._main_loop()
        except SerialStopException:
            print_normal("Stopping condition has been received\n")
        except KeyboardInterrupt:
            print("Received exit signal (Ctrl+C), shutting down program...")
        finally:
            try:
                self.console_reader._stop()
                self.serial_reader._stop()
                self.log_handler._stop()
                # Cancelling _invoke_processing_last_line_timer is not
                # important here because receiving empty data doesn"t matter.
                if self._invoke_processing_last_line_timer:
                    self._invoke_processing_last_line_timer.cancel()
                    self._invoke_processing_last_line_timer = None
            except Exception as e:  # noqa
                print_red(f"{e}")

    def serial_write(self, data):
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
            self.serial_write(data)
        elif event_tag == TAG_SERIAL:
            if self.logAGG_enabled:            
                events = self.log_handler.logAGG_parse(data)           
                for ev in events:
                    src = 0
                    payload = ''
                    if ev[0] == "raw":
                        _, payload = ev
                    else: 
                        _, src, payload = ev
                    if not payload:
                        payload_str = ''
                    else:
                        payload_str = payload.decode('utf-8', errors='ignore')
                    self.serial_handler.handle_serial_input(payload_str, self.coredump, src)
            else:
                payload_str = self.serial_reader.decode(data)              
                self.serial_handler.handle_serial_input(payload_str, self.coredump)

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
        pass
        # self.console_reader.start()
        # if self.elf_exists:
        #     self.serial_reader.gdb_exit = self.gdb_helper.gdb_exit  # type: ignore # write gdb_exit flag
        # self.serial_reader.start()

    def _pre_start(self):
        super()._pre_start()
        self.serial_handler.start_cmd_sent = False

    def serial_write(self, data):
        try:
            data_to_send = data.encode('utf-8')
            # Display raw byte data in debug mode
            if self.debug:
                hex_str = ' '.join(f'{b:02X}' for b in data_to_send)
                print(f"[Sent Data (Hex)]: {hex_str}")
            self.serial_reader.serial.write(data_to_send)
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

def main():
    colorama.init()
    parser = get_parser()
    args = parser.parse_args()

    if args.decode_coredumps :
        if not args.toolchain_dir:
            print_red("Note: No toolchain_dir specified for decode-coredumps, monitor starts failed!")
            sys.exit(1)
        if not args.axf_file:
            print_red("Note: No axf_file specified for decode-coredumps, monitor starts failed!")
            sys.exit(1)
        if not args.target_os:
            print_red("Note: No target_os specified for decode-coredumps, monitor starts failed!")
            sys.exit(1)

    if args.enable_address_decoding:
        if not args.toolchain_dir:
            print_red("Note: No toolchain_dir specified for enable-address-decoding, monitor starts failed!")
            sys.exit(1)
        if not args.axf_file:
            print_red("Note: No axf_file specified for enable-address-decoding, monitor starts failed!")
            sys.exit(1)
    toolchain_path = ''
    if args.toolchain_dir:
        if os.name == "nt":
            toolchain_path = os.path.join(args.toolchain_dir, "mingw32", "newlib")
        else:
            toolchain_path = os.path.join(args.toolchain_dir, "linux", "newlib")
        if not os.path.exists(args.toolchain_dir):
            print_red(f"Error: Toolchain '{args.toolchain_dir}' does not exist")

    target_os = args.target_os
    args.eol = args.eol or ("LF" if sys.platform == "linux" else "CRLF")

    elf_file = args.axf_file
    remote_server = args.remote_server
    remote_port = REMOTE_PORT

    try:
        if target_os == "linux":
            cls = LinuxMonitor
            print_yellow(f"--- monitor {__version__} on linux ---")
        else:
            # The port name is changed in cases described in the following lines.
            # Use a local argument and avoid the modification of args.port.
            port = args.port
            # GDB uses CreateFile to open COM port, which requires the COM name to be r"\\.\COMx" if the COM
            # number is larger than 10
            if os.name == "nt" and port.startswith("COM"):
                port = port.replace("COM", r"\\.\COM")
            elif port.startswith("/dev/tty.") and sys.platform == "darwin":
                port = port.replace("/dev/tty.", "/dev/cu.")

            cls = SerialMonitor
        rom_file = ""
        monitor = cls(args.port, args.baud,
                        timestamps=args.timestamps,
                        elf_file=elf_file,
                        toolchain_path=toolchain_path,
                        eol=args.eol,
                        decode_coredumps=args.decode_coredumps,
                        target_os=target_os,
                        is_ca32=args.ca32,
                        enable_address_decoding=args.enable_address_decoding,
                        rom_file=rom_file,
                        reset_mode=args.reset,
                        debug=args.debug,
                        remote_server=remote_server,
                        remote_port=remote_port,
                        remote_password=args.remote_password,
                        log_enabled = args.log,
                        log_dir=args.log_dir,
                        logAGG = args.logAGG)

        print_yellow("--- Exit monitor: Ctrl+C ---")

        monitor.main_loop()
    except KeyboardInterrupt:
        pass


def get_parser():
    parser = argparse.ArgumentParser("monitor - a serial output monitor.")

    parser.add_argument("-p", "--port", help="Serial port device, e.g., COM3 (Windows) or /dev/ttyUSB0 (Linux)")
    parser.add_argument("-b", "--baud", type=int, default=1500000, help="Serial port baud rate, default is 1500000")
    parser.add_argument("--decode-coredumps", action="store_true",
                        help=f"If set will handle core dumps found in serial output. Default is False")
    parser.add_argument("--enable-address-decoding", action="store_true",
                        help="Print lines about decoded addresses from the application ELF file, default is False")
    parser.add_argument("--axf-file", nargs="?", help="AXF file of application")
    parser.add_argument("--eol", choices=["CR", "LF", "CRLF"], default="CRLF",
                        help="End of line to use when sending to the serial port. Defaults to LF for Linux targets and CR otherwise.")
    parser.add_argument("--rom-file", nargs="?", help="Rom of application")
    parser.add_argument("--target-os", help="Target os name (used when core dump decoding is enabled)")
    parser.add_argument("--ca32", action="store_true", help="If core is ca32, should set this.")
    parser.add_argument("--timestamps", action="store_true",
                        help="Add timestamp for each line. Default is False")
    parser.add_argument("--toolchain-dir", help="Set toolchain dir. If not set, will get from config.")

    parser.add_argument('--reset', action='store_true', 
                       help='Enable reset mode: Wait 100ms after connection to send "reboot" command, start output only after detecting "ROM:["')
    parser.add_argument('--debug', action='store_true', 
                       help='Enable debug mode: Display raw hexadecimal data of sent and received bytes')
    parser.add_argument('--remote-server', type=str, help='remote serial server IP address')
    parser.add_argument('--remote-password', type=str, help='remote serial server validation password')
    parser.add_argument('--log', action='store_true', 
                       help='Enable logging mode: save logs to log file')
    parser.add_argument('--log-dir', type=str, default="",
                       help='Specify the target log file directory, if not, the logs will save to under xxxx_gcc_project when logging enabled')
    parser.add_argument('--logAGG', nargs='+', 
                         help='the logAGG enabled and source marked '
                        )

    return parser


if __name__ == "__main__":
    main()
