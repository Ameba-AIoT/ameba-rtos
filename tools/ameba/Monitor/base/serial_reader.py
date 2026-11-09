#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from queue import Queue
from subprocess import Popen
import sys
import threading
import time
import serial
from serial import Serial
from serial.tools import list_ports

from .constants import ASYNC_CLOSING_WAIT_NONE, BOOT_MARKERS, CHECK_ALIVE_FLAG_TIMEOUT, HARD_RESET_BOOT_TIMEOUT, \
    RECONNECT_DELAY, REMOTE_RESET_TIMEOUT, RESET_HOLD, RESET_RELEASE, ROM_MARKER, STARTUP_PHASE_TIMEOUT, \
    STARTUP_WAIT_TIMEOUT, TAG_SERIAL, TAG_KEY
from .color_output import print_red, print_yellow
from .stoppable_thread import StoppableThread
import os
import logging
from typing import Optional, Dict, Any
from .console_reader import console_update_session
import re
from pathlib import Path

current_script_path = Path(__file__).resolve().parent

# Define possible target paths
_possible_paths = [
    current_script_path.parents[1] / 'RemoteService',
    *(p / 'tools/ameba/RemoteService' for p in current_script_path.parents)
]

# Find the first existing path, default to the first one if none found
remote_service_path = next((p for p in _possible_paths if p.exists()), _possible_paths[0])

RemoteSerial = None

if remote_service_path.exists():
    sys.path.insert(0, str(remote_service_path))

    try:
        from remote_serial import RemoteSerial
    except ImportError as e:
        RemoteSerial = None
        print(f"error: RemoteSerial ImportError: ", str(e))
    except Exception as e:
        RemoteSerial = None
        print(f"error: RemoteSerial ImportException: ", str(e))

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
        self.boot_count = 0  # BOOT_MARKERS seen by the main read loop; guarded by _boot_cond
        self.rom_count = 0  # ROM banners (ROM_MARKER) seen by the main read loop; guarded by _boot_cond
        self._boot_cond = threading.Condition()
        self.main_loop_ready = threading.Event()  # set once the AT+LIST probe and --reset phase are done
        self._boot_tail = b''  # end of the previous read, so a marker split across reads is still found
        self._agg_pending = False  # previous chunk ended with the 0xFF of an AGG pair
        # Serializes DTR/RTS sequences, and for RemoteSerial every command round trip made while the main loop runs
        # (reset_device() and write(): RemoteSerial matches a reply to whichever request is waiting, so they must not
        # overlap). RemoteSerial.read()/inWaiting() only use its local receive buffer, filled by its own receive thread,
        # and never wait for a command reply, so the read loop does not take the lock. open/validate/query_version run
        # in open_serial() before the main loop; close() runs in _stop() under the lock.
        self.io_lock = threading.Lock()
        # if not hasattr(self.serial, "cancel_read"):
        #     self.serial.timeout = CHECK_ALIVE_FLAG_TIMEOUT

    def expired(self, start_time):
        cur_time = time.time()
        if cur_time - start_time > STARTUP_PHASE_TIMEOUT:
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
                if RemoteSerial and isinstance(self.serial, RemoteSerial):
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
                filtered = re.sub(rb'\xff.', b'', self.data_buffer)
                if b'AT+LIST' in filtered:
                    if b'#' in filtered:
                        self.parse_cmd_list(filtered)
                        self.data_buffer = b''
                        break
            except Exception as e:
                break

        # Reset mode: Try Soft reset first, fallback to hard reset
        if self.reset_mode:
            try:
                self.event_queue.put((TAG_KEY, 'reboot\r\n'), True) # Send reboot command (manually add \r\n)
            except Exception as e:
                print_red(f"Failed to send reboot command: {str(e)}")

            self.data_buffer = b''
            start_time = time.time()
            hard_reset_done = False
            reset_success = False
            pulse_offset = 0  # data_buffer length at the DTR/RTS pulse; only later data proves the hard reset

            while self.running:
                try:
                    if RemoteSerial and isinstance(self.serial, RemoteSerial):
                        while self.serial.inWaiting() < 1:
                            time.sleep(0.01)
                            if self.expired(start_time):
                                break
                        if self.expired(start_time):
                            raise Exception("Reset timeout")
                        data = self.serial.read(self.serial.inWaiting())
                    else:
                        if self.expired(start_time):
                            raise Exception("Reset timeout")
                        data = self.serial.read(1)
                        if not data:
                            continue
                        data += self.serial.read(self.serial.in_waiting)

                    # Display raw byte data in debug mode
                    if self.debug:
                        hex_str = ' '.join(f'{b:02X}' for b in data)
                        print(f"[Received Data (Hex)]: {hex_str}")

                    self.data_buffer += data
                    filtered = re.sub(rb'\xff.', b'', self.data_buffer, flags=re.S)
                    if hard_reset_done:
                        after_pulse = re.sub(rb'\xff.', b'', self.data_buffer[pulse_offset:], flags=re.S)
                        if any(marker in after_pulse for marker in BOOT_MARKERS):
                            reset_success = True
                            break
                    else:
                        # Only boot log after the "reboot" echo proves the soft reset, not a boot already under way.
                        # Known limit: a log line containing "reboot" printed before the echo is taken for it.
                        echo = filtered.find(b'reboot')
                        if echo >= 0 and any(marker in filtered[echo:] for marker in BOOT_MARKERS):
                            reset_success = True
                            break
                except Exception as e:
                    # Stopping (Ctrl+C) or port closed/preempted: not a soft-reset timeout, do not pulse
                    if hard_reset_done or not self.running or not self.serial.is_open:
                        break
                    # Soft reset timed out → escalate to hard reset
                    print_yellow("Hard reset triggered.")

                    def on_hold():  # data read after this comes from the new boot
                        nonlocal pulse_offset
                        pulse_offset = len(self.data_buffer)
                    if not self._pulse_reset(on_hold):
                        break
                    hard_reset_done = True
                    start_time = time.time()

            if not self.running:
                pass  # quitting: no verdict, the port is closing
            elif not self.serial or not self.serial.is_open:
                # preempted (e.g. flash took the port) or closed: aborted, not a reset failure
                print_yellow("Reset aborted: serial port closed.")
            elif not reset_success:
                print_red("Failed to reset, please reset manually.")
                # The IC did not reboot, so ROM:[ will never arrive: show what was received
                # instead of hiding all further output while waiting for it.
                self.start_output = True
                if self.data_buffer:
                    self.event_queue.put((TAG_SERIAL, self.data_buffer), False)
                self.data_buffer = b''
            else:
                # Flush data captured during the reset phase before entering the
                # main loop. Fast-boot boards finish their entire boot sequence
                # during the reset phase; the main loop only checks data_buffer
                # after new data arrives, which never happens on an already-idle
                # board, causing the complete log to be silently dropped.
                if self.data_buffer:
                    if self.target_keyword in self.data_buffer:
                        index = self.data_buffer.find(self.target_keyword)
                        self.event_queue.put((TAG_SERIAL, self.data_buffer[index:]), False)
                    else:
                        self.event_queue.put((TAG_SERIAL, self.data_buffer), False)
                    self.data_buffer = b''
                    self.start_output = True

        # Only the loop below detects BOOT_MARKERS for hard_reset()
        self.main_loop_ready.set()
        while self.running:
            try:
                if RemoteSerial and isinstance(self.serial, RemoteSerial):
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

                # _boot_tail holds the AGG-stripped end of earlier data, so a marker split across reads is still found;
                # only markers that end in the new data are counted, so nothing is counted twice
                kept = len(self._boot_tail)
                window = self._boot_tail + self._strip_agg(data)
                new = {m: sum(1 for hit in re.finditer(re.escape(m), window) if hit.end() > kept) for m in BOOT_MARKERS}
                if any(new.values()):
                    with self._boot_cond:
                        self.boot_count += sum(new.values())
                        self.rom_count += new.get(ROM_MARKER, 0)
                        self._boot_cond.notify_all()
                self._boot_tail = window[-16:]

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
        with self._boot_cond:
            self._boot_cond.notify_all()  # wake a hard_reset() waiting for boot log
        self.stop()
        # wait for a DTR/RTS sequence in progress, so closing cannot leave the IC held in reset
        with self.io_lock:
            if self.serial and self.serial.is_open:
                self.close_serial()
        print("Close serial port successfully")

    def parse_cmd_list(self, data):
        decode_data = self.decode(data)
        commands = re.findall(r'\bAT\+\w+', decode_data)
        commands_list = sorted(set(commands))
        console_update_session(commands_list)

    def _in_waiting(self):
        """Bytes waiting to be read: pyserial has the in_waiting property, RemoteSerial only inWaiting()."""
        try:
            if RemoteSerial and isinstance(self.serial, RemoteSerial):
                return self.serial.inWaiting()
            return self.serial.in_waiting
        except Exception:
            return 0

    def _strip_agg(self, data):
        """Remove AGG 0xFF <tag> pairs like re.sub(rb'\\xff.', b'', stream) on the whole stream would, also when a
        pair is split across two reads."""
        out = bytearray()
        i = 0
        if self._agg_pending and data:
            i = 1  # tag byte of a pair whose 0xFF ended the previous chunk
            self._agg_pending = False
        while i < len(data):
            if data[i] == 0xFF:
                if i + 1 == len(data):
                    self._agg_pending = True  # its tag byte arrives in the next chunk
                i += 2
            else:
                out.append(data[i])
                i += 1
        return bytes(out)

    def _run_lines(self, steps):
        for key, value in steps:
            if key == "dtr":
                self.serial.setDTR(bool(value))
            elif key == "rts":
                self.serial.setRTS(bool(value))
            else:
                time.sleep(value / 1000)

    def _pulse_reset(self, on_hold=None):
        """Run RESET_HOLD + RESET_RELEASE on DTR/RTS (or RemoteSerial.reset_device() for remote serial).

        on_hold() is called while the IC is held in reset, so anything read after it comes from the new boot.
        For RemoteSerial the server runs the whole sequence in one command, so on_hold() is called right after it.
        Returns True if the sequence was sent, False if it failed or the port is closed. That does not prove the IC
        rebooted: on boards without the auto-download circuit the lines do not reach CHIP_EN.
        """
        with self.io_lock:
            # stopping or preempted (flash tool owns the port now): do not touch the lines
            if not self.running or not self.serial or not self.serial.is_open:
                return False
            try:
                if RemoteSerial and isinstance(self.serial, RemoteSerial):
                    # reset_device() runs Reset.cfg (same as RESET_SEQUENCE) and raises on failure; its return value
                    # is not relied on. The server restores the lines after every command, so the hold cannot be split
                    # from the release: on_hold() runs when the reply arrives, after the server released CHIP_EN.
                    # Log the board printed before or during the pulse is then already counted; a real boot banner
                    # follows the release, so at worst a very fast one is missed (FAILED), never counted early.
                    self.serial.reset_device(timeout=REMOTE_RESET_TIMEOUT)
                    if on_hold:
                        on_hold()
                    return True
                try:
                    self._run_lines(RESET_HOLD)
                    if on_hold:
                        on_hold()
                finally:
                    self._run_lines(RESET_RELEASE)  # never leave the IC held in reset
                return True
            except Exception as e:
                print_red(f"Hard reset failed: could not send DTR/RTS sequence: {e}")
                return False

    def hard_reset(self):
        """Pulse DTR/RTS to reset the target, then wait for its boot log.

        Returns True only if BOOT_MARKERS are received within HARD_RESET_BOOT_TIMEOUT.
        """
        # Before the main read loop, boot log is consumed by the AT+LIST probe or the --reset phase
        # (which may itself be pulsing DTR/RTS), so wait for those to finish first.
        if not self.main_loop_ready.is_set():
            print_yellow("Hard reset: waiting for monitor startup to finish...")
            deadline = time.time() + STARTUP_WAIT_TIMEOUT
            while not self.main_loop_ready.wait(0.1):
                if not self.alive or time.time() > deadline:  # reader exited (e.g. port open failed) or stuck
                    print_red("Hard reset failed: monitor startup did not finish.")
                    return False
        mark = {}

        def on_hold():  # only markers read after this point prove that this reset rebooted the IC
            if not (RemoteSerial and isinstance(self.serial, RemoteSerial)):
                # Local: the IC is held in reset and sends nothing new; let the reader consume what it already
                # received, so an old marker still in the input buffer is counted before the mark, not after it.
                # (Remote: called when the reply arrives, after the release; the ROM banner follows within ~10 ms,
                # so the mark must be taken at once.)
                deadline = time.time() + 0.1
                while time.time() < deadline and self._in_waiting():
                    time.sleep(0.005)
                time.sleep(0.02)  # the chunk the reader is processing right now
            with self._boot_cond:
                mark["boot"], mark["rom"] = self.boot_count, self.rom_count

        if not self._pulse_reset(on_hold):
            if not self.running or not self.serial or not self.serial.is_open:
                print_red("Hard reset failed: serial port closed.")  # quitting or preempted; nothing was pulsed
            return False
        if RemoteSerial and isinstance(self.serial, RemoteSerial):
            # The server forwards the board's output in a burst after its reset command returns, so log printed before
            # the pulse can arrive after the mark. Only the ROM banner, printed by the mask ROM after a reset and never
            # by running firmware, is accepted as proof. Known limit: if the board was itself rebooting when !reset
            # started, that boot's ROM banner can still be taken for this one; ordering old and new log fully needs the
            # server to report it (AmebaRemoteService).
            proof = lambda: self.rom_count > mark["rom"]  # noqa: E731
        else:
            proof = lambda: self.boot_count > mark["boot"]  # noqa: E731
        port_gone = lambda: not self.running or not self.serial or not self.serial.is_open  # noqa: E731
        with self._boot_cond:
            if self._boot_cond.wait_for(lambda: proof() or port_gone(), HARD_RESET_BOOT_TIMEOUT) and proof():
                return True
        if port_gone():  # quit or preempted while waiting: no verdict about the board
            print_red("Hard reset failed: serial port closed.")
        else:
            print_red("Hard reset failed: DTR/RTS sequence sent but no boot log: check that the board's "
                      "auto-download circuit drives CHIP_EN.")
        return False

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

    def open_serial(self):
        try:
            if self.remote_server and self.remote_port:
                # print_yellow(f"Connect to remote serial server: {self.remote_server}:{self.remote_port} (Serial port: {self.port})")
                if RemoteSerial is None:
                    print_yellow(f"RemoteSerial doesn't exists at: {remote_service_path}")
                    sys.exit(1)
                self.serial = RemoteSerial(
                        remote_server=self.remote_server,
                        remote_port=self.remote_port,
                        port=self.port,
                        baudrate=self.baud,
                        logger=self._setup_logger(),
                        source="monitor"
                )
                if self.remote_password:
                    self.serial.validate(self.remote_password)
                self.serial.query_version()
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
