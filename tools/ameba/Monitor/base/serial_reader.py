#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from queue import Queue
from subprocess import Popen
import sys
import time
import serial
from serial import Serial
from serial.tools import list_ports

from .constants import ASYNC_CLOSING_WAIT_NONE, CHECK_ALIVE_FLAG_TIMEOUT, RECONNECT_DELAY, TAG_SERIAL, TAG_KEY
from .color_output import print_red, print_yellow
from .stoppable_thread import StoppableThread
import os
from .remote_serial import RemoteSerial
import logging
from typing import Optional, Dict, Any
from .console_reader import console_update_session
import re

class SerialReader(StoppableThread):
    """
    Read data from the serial port and push to the event queue, until stopped.
    """

    def __init__(self, port, baudrate,
                 event_queue: Queue,
                 reset_mode=False,
                 debug=False,
                 remote_server: Optional[str] = None,
                 remote_port: Optional[int] = None,
                 remote_password: Optional[str] = None):
        super(SerialReader, self).__init__()
        self.port = port
        self.baud = baudrate
        self.serial = None
        self.event_queue = event_queue
        self.reset_mode = reset_mode  # Whether to enable reset mode
        self.start_output = not reset_mode  # Whether to start output (False initially in reset mode)
        self.target_keyword = b'ROM:['  # Target keyword set to ROM:[
        self.debug = debug  # Debug mode switch
        self.remote_server = remote_server
        self.remote_port = remote_port
        self.remote_password = remote_password
        self.gdb_exit = False
        self.running = False
        # if not hasattr(self.serial, "cancel_read"):
        #     self.serial.timeout = CHECK_ALIVE_FLAG_TIMEOUT

    def expired(self, start_time):
        cur_time = time.time()
        if cur_time - start_time > 2:
            return True
        return False

    def run(self):
        try:
            self.open_serial()
            if not self.serial:
                raise serial.SerialException
            if not self.serial.is_open:
                raise serial.SerialException
            print_yellow(f"Successfully connected to {self.port}, baud rate: {self.baud}")
            if self.debug:
                print_yellow("Debug mode enabled: Raw byte data of sent and received data will be displayed")
        except serial.SerialException as e:
            port_list = "\n".join([p.device for p in list_ports.comports()])
            print_red(f"Connection to {self.port} failed: {str(e)}. Available ports:\n{port_list}")
            return
        self.gdb_exit = False
        self.running = True
        # Execute special process if reset mode is enabled
        time.sleep(0.1)  # Wait for 100ms
        self.event_queue.put((TAG_KEY, 'AT+LIST\r\n'), True) # Send AT+LIST command (manually add \r\n)
        self.data_buffer = b''
        start_time = time.time()
        while self.running:
            try:
                if isinstance(self.serial, RemoteSerial):
                    while self.serial.inWaiting() < 1:
                        time.sleep(0.01)
                        if self.expired(start_time):
                            raise Exception("Get cmd list expired")
                    if self.expired(start_time):
                        raise Exception("Get cmd list expired")
                    data = self.serial.read(self.serial.inWaiting())
                else:
                    if self.expired(start_time):
                        raise Exception("Get cmd list expired")
                    data = self.serial.read(1)
                    if not data:
                        continue
                    data += self.serial.read(self.serial.in_waiting)

                # Display raw byte data in debug mode
                if self.debug:
                    hex_str = ' '.join(f'{b:02X}' for b in data)
                    print(f"[Received Data (Hex)]: {hex_str}")

                self.data_buffer += data
                if b'AT+LIST' in self.data_buffer:
                    if b'#' in self.data_buffer:
                        self.parse_cmd_list(self.data_buffer)
                        self.data_buffer = b''
                        break
            except Exception as e:
                print_red(f"Failed to get cmd list: {str(e)}")
                break
        if self.reset_mode:
            try:
                self.event_queue.put((TAG_KEY, 'reboot\r\n'), True) # Send reboot command (manually add \r\n)
            except Exception as e:
                print_red(f"Failed to send reboot command: {str(e)}")

            self.data_buffer = b''
            start_time = time.time()
            while self.running:
                try:
                    if isinstance(self.serial, RemoteSerial):
                        while self.serial.inWaiting() < 1:
                            time.sleep(0.01)
                            if self.expired(start_time):
                                raise Exception("Reset expired")
                        if self.expired(start_time):
                            raise Exception("Reset expired")
                        data = self.serial.read(self.serial.inWaiting())
                    else:
                        if self.expired(start_time):
                            raise Exception("Reset expired")
                        data = self.serial.read(1)
                        if not data:
                            continue
                        data += self.serial.read(self.serial.in_waiting)

                    # Display raw byte data in debug mode
                    if self.debug:
                        hex_str = ' '.join(f'{b:02X}' for b in data)
                        print(f"[Received Data (Hex)]: {hex_str}")

                    self.data_buffer += data
                    if b'reboot' in self.data_buffer:
                        if b'BOOT-I' in self.data_buffer:
                            break
                except Exception as e:
                    print_red(f"Failed to reset, pelase reset manually: {str(e)}")
                    self.data_buffer = b''
                    break

        while self.running:
            try:
                if isinstance(self.serial, RemoteSerial):
                    while self.serial.inWaiting() < 1:
                        time.sleep(0.01)
                    data = self.serial.read(self.serial.inWaiting())
                else:
                    data = self.serial.read(1)
                    if not data:
                        continue
                    data += self.serial.read(self.serial.in_waiting)

                # Display raw byte data in debug mode
                if self.debug:
                    hex_str = ' '.join(f'{b:02X}' for b in data)
                    print(f"[Received Data (Hex)]: {hex_str}")

                # Handle output control in reset mode
                if self.reset_mode and not self.start_output:
                    self.data_buffer += data  # Accumulate data into buffer
                    # Check if target keyword ROM:[ is included
                    if self.target_keyword in self.data_buffer:
                        # Extract from the position of the keyword (including the keyword itself)
                        index = self.data_buffer.find(self.target_keyword)
                        data_to_output = self.data_buffer[index:]
                        self.start_output = True  # Mark to start output
                        self.data_buffer = b''  # Clear the buffer
                        self.event_queue.put((TAG_SERIAL, data_to_output), False)  # Print data containing the keyword
                else:
                    # Directly process if not in reset mode or output has started
                    self.event_queue.put((TAG_SERIAL, data), False)
            except Exception as e:
                if not self.serial.is_open:
                    print("Serial port closed")
                else:
                    self._stop()
                break

    def _stop(self):
        """Stop the serial monitor and clean up resources"""
        print("Closing connection...")
        self.running = False
        self.stop()
        if self.serial and self.serial.is_open:
            self.close_serial()
        print("Close serial port successfully")

    def parse_cmd_list(self, data):
        decode_data = self.decode(data)
        commands = re.findall(r'\bAT\+\w+', decode_data)
        commands_list = sorted(set(commands))
        console_update_session(commands_list)

    def decode(self, data):
        try:
            self.decode_error_cnt = 0
            decode_data = data.decode('utf-8')
        except UnicodeDecodeError:
            decode_data = data.decode(errors='ignore')
            self.decode_error_cnt += 1
            if self.decode_error_cnt >= 3:
                print_yellow(
                    'Multiple decode errors occurred: Try checking the baud rate and XTAL frequency setting in menuconfig')
                self.decode_error_cnt = 0
        return decode_data

    def _setup_logger(self) -> logging.Logger:
        """Set up and return a Logger object"""
        logger = logging.getLogger("monitor")
        logger.setLevel(logging.INFO)  # Set log level to DEBUG to capture all levels

        if not logger.handlers:
            # Output to console
            handler = logging.StreamHandler(sys.stdout)
            formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
            handler.setFormatter(formatter)
            logger.addHandler(handler)

            # (Optional) Output to file
            # file_handler = logging.FileHandler(f"{APP_NAME}.log")
            # file_handler.setFormatter(formatter)
            # logger.addHandler(file_handler)
        return logger

    def open_serial(self, ):
        try:
            if self.remote_server and self.remote_port:
                # print_yellow(f"Connect to remote serial server: {self.remote_server}:{self.remote_port} (Serial port: {self.port})")
                self.serial = RemoteSerial(
                        remote_server=self.remote_server,
                        remote_port=self.remote_port,
                        port=self.port,
                        baudrate=self.baud,
                        logger=self._setup_logger()
                )
                if self.remote_password:
                    self.serial.validate(self.remote_password)
                self.serial.open()
            else:
                self.serial = serial.Serial(
                        port=self.port,
                        baudrate=self.baud,
                        timeout=1,
                        parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                        bytesize=serial.EIGHTBITS
                )
            return True
        except (Exception, serial.SerialException) as e:
            print(f"Connection failed: {str(e)}")
            return False

    def close_serial(self):
        self._disable_closing_wait()  # disable closing wait for serial port in linux
        self.serial.close()

    def _disable_closing_wait(self):
        """
        check serial info, confirm serial port will be closed immediately
        """
        # ignore setting closing wait for network ports such as RFC2217
        if sys.platform == 'linux' and hasattr(self.serial, 'fd') and self.serial.is_open:
            import fcntl
            import struct
            import termios

            # `serial_struct` format based on linux kernel source:
            # https://github.com/torvalds/linux/blob/25aa0bebba72b318e71fe205bfd1236550cc9534/include/uapi/linux/serial.h#L19
            struct_format = 'iiIiiiiiHcciHHPHIL'
            buf = bytes(struct.calcsize(struct_format))

            # get serial_struct
            try:
                buf = fcntl.ioctl(self.serial.fd, termios.TIOCGSERIAL, buf)
            except IOError:
                # port has been disconnected
                return
            serial_struct = list(struct.unpack(struct_format, buf))

            # set `closing_wait` - amount of time, in hundredths of a second, that the kernel should wait before closing port
            # `closing_wait` is 13th (indexing from 0) variable in `serial_struct`, for reference see struct_format var
            if serial_struct[12] == ASYNC_CLOSING_WAIT_NONE:
                return
            # set serial closing wait none
            serial_struct[12] = ASYNC_CLOSING_WAIT_NONE

            # set serial_struct
            buf = struct.pack(struct_format, *serial_struct)
            try:
                fcntl.ioctl(self.serial.fd, termios.TIOCSSERIAL, buf)
            except OSError:
                # Discard written but not yet transmitted data
                termios.tcflush(self.serial.fd, termios.TCOFLUSH)
        else:
            pass

    def _cancel(self):
        # not all serial support cancel read
        if hasattr(self.serial, 'cancel_read'):
            try:
                self.serial.cancel_read()
            except Exception:
                pass


class LinuxReader(StoppableThread):
    """ Read data from the subprocess that runs runnable and push to the
    event queue, until stopped.
    """

    def __init__(self, process: Popen, event_queue: Queue):
        super().__init__()
        self.proc = process
        self.event_queue = event_queue

    def run(self):
        try:
            while self.alive:
                c = self.proc.stdout.read(1)  # type: ignore
                if c:
                    self.event_queue.put((TAG_SERIAL, c), False)
        finally:
            self.proc.terminate()

    def _cancel(self):
        self.proc.terminate()
