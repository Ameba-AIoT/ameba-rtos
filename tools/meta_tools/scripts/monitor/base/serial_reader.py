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

from .constants import ASYNC_CLOSING_WAIT_NONE, CHECK_ALIVE_FLAG_TIMEOUT, RECONNECT_DELAY, TAG_SERIAL
from .color_output import print_red, print_yellow
from .stoppable_thread import StoppableThread


class SerialReader(StoppableThread):
    """
    Read data from the serial port and push to the event queue, until stopped.
    """

    def __init__(self, serial_instance: Serial, event_queue: Queue):
        super(SerialReader, self).__init__()
        self.baud = serial_instance.baudrate
        self.serial = serial_instance
        self.event_queue = event_queue
        self.gdb_exit = False
        if not hasattr(self.serial, "cancel_read"):
            self.serial.timeout = CHECK_ALIVE_FLAG_TIMEOUT

    def run(self):
        if not self.serial.is_open:
            self.serial.baudrate = self.baud
            try:
                self.open_serial()
            except serial.SerialException as e:
                print_yellow(str(e))
                port_list = "\n".join([p.device for p in list_ports.comports()])
                print_yellow(f"Connection to {self.serial.portstr} failed. Available ports:\n{port_list}")
                return
            self.gdb_exit = False
        try:
            while self.alive:
                try:
                    data = self.serial.read(self.serial.in_waiting or 1)
                    data = self.decode(data)
                except (serial.SerialException, IOError) as e:
                    data = ""
                    print_red(e)
                    print_yellow("Waiting for the device to reconnect")
                    self.close_serial()
                    while self.alive:  # so that exiting monitor works while waiting
                        try:
                            time.sleep(RECONNECT_DELAY)
                            self.open_serial()
                            break
                        except serial.SerialException:
                            print_yellow('.', end='')
                    print_yellow("\n")  # go to new line
                if data:
                    self.event_queue.put((TAG_SERIAL, data), False)
        finally:
            self.close_serial()

    def decode(self, data):
        try:
            self.decode_error_cnt = 0
            decode_data = data.decode()
        except UnicodeDecodeError:
            decode_data = data.decode(errors='ignore')
            self.decode_error_cnt += 1
            if self.decode_error_cnt >= 3:
                print_yellow(
                    'Multiple decode errors occurred: Try checking the baud rate and XTAL frequency setting in menuconfig')
                self.decode_error_cnt = 0
        return decode_data

    def open_serial(self, ):
        self.serial.open()

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
