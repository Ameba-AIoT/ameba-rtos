#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import socket
import json
import time
import threading
import base64
from typing import Optional, Dict, Any
import serial
from serial.serialutil import SerialException, SerialTimeoutException
from datetime import datetime


class RemoteSerial:
    def __init__(self, logger, remote_server: str, remote_port: int, port: str, baudrate: int = 9600):
        """
        Initialize remote serial proxy
        :param logger: logger object (external input, for unified log format)
        :param remote_server: remote serial server IP
        :param remote_port: remote serial server port
        :param port: remote serial port name (e.g. "COM3", "/dev/ttyUSB0")
        :param baudrate: serial baud rate
        """
        self.logger = logger
        self.remote_server = remote_server
        self.remote_port = remote_port
        self.port = port
        self.baudrate = baudrate

        # Core state variables
        self.tcp_socket: Optional[socket.socket] = None
        self.receive_buffer = b""  # Binary receive buffer
        self.receive_thread: Optional[threading.Thread] = None
        self.is_open = False  # Whether serial is open (TCP connected + serial command succeeds)
        self.response_event = threading.Event()  # Command response synchronization event
        self.last_response: Dict[str, Any] = {}  # Last command response

        # Initialize logger
        self.logger.debug(
            f"[RemoteSerial][{self.port}] Initialize remote serial proxy - "
            f"Server: {self.remote_server}:{self.remote_port}, baudrate: {self.baudrate}"
        )
        # 1. Establish TCP connection (just connect, not start the receive thread)
        self._connect_tcp()
        self.receive_thread = threading.Thread(
            target=self._receive_loop,
            daemon=True,
            name=f"RemoteSerialRecv-{self.port}"
        )
        self.receive_thread.start()
        self.logger.debug(f"[RemoteSerial][{self.port}] Receive thread started: {self.receive_thread.name}")

    def _connect_tcp(self) -> bool:
        """
        Establish TCP connection with remote serial server
        :return: Returns True if connected successfully, raises SerialException on failure
        """
        self.logger.debug(f"[RemoteSerial][{self.port}] Trying TCP connection: {self.remote_server}:{self.remote_port}")
        try:
            self.tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # self.tcp_socket.settimeout(10)  # TCP connect timeout 10s
            self.tcp_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            self.logger.debug(f"[RemoteSerial][{self.port}] TCP_NODELAY set")
            self.tcp_socket.connect((self.remote_server, self.remote_port))
            self.logger.debug(f"[RemoteSerial][{self.port}] TCP connection succeeded")
            return True
        except socket.timeout:
            raise SerialException(f"[RemoteSerial][{self.port}] TCP connection timed out (10s)- {self.remote_server}:{self.remote_port}")
        except ConnectionRefusedError:
            raise SerialException(f"[RemoteSerial][{self.port}] TCP connection refused - server not started or wrong port")
        except Exception as e:
            raise SerialException(f"[RemoteSerial][{self.port}] TCP connection failed: {str(e)}")

    def _receive_loop(self):
        """
        Core logic of receive thread: continuously read TCP data, parse into buffer or trigger response
        """
        self.logger.debug(f"[RemoteSerial][{self.port}] Receive thread running")
        buffer = ""
        while self.tcp_socket:
            try:
                # Read TCP data (as text, as server sends JSON+newline-delimited)
                data = self.tcp_socket.recv(4096).decode('utf-8', errors='strict')
                if not data:
                    self.logger.error(f"[RemoteSerial][{self.port}] Server closed connection, recv returned empty data")
                    raise ConnectionAbortedError("Server closed connection")

                buffer += data

                # Split by newline for complete messages (handle sticky packets)
                while '\n' in buffer:
                    msg_str, buffer = buffer.split('\n', 1)
                    msg_str = msg_str.strip()
                    if not msg_str:
                        continue
                    self._parse_message(msg_str)
            except ConnectionResetError as e:
                self.logger.error(f"[RemoteSerial][{self.port}] Connection reset: {str(e)}")
                break
            except ConnectionAbortedError as e:
                self.logger.error(f"[RemoteSerial][{self.port}] Receive thread exception: {str(e)}")
                break
            except Exception as e:
                self.logger.error(f"[RemoteSerial][{self.port}] Receive thread exception: {str(e)}", exc_info=True)
                break

        self.is_open = False
        if self.tcp_socket:
            self.tcp_socket.close()
        self.logger.info(f"[RemoteSerial][{self.port}] Receive thread exited")

    def _parse_message(self, msg_str: str):
        """
        Parse JSON message from server
        :param msg_str: Complete JSON string (without newline)
        """
        try:
            msg = json.loads(msg_str)
            msg_type = msg.get("type")

            if msg_type == "command_response":
                self.last_response = msg
                self.logger.debug(
                    f"[RemoteSerial][{self.port}] Received command response - "
                    f"Success: {msg.get('success')}, message: {msg.get('message', '无')}"
                )
                self.response_event.set()

            elif msg_type == "serial_data":
                if msg.get("port") != self.port:
                    self.logger.warning(f"[RemoteSerial][{self.port}] Received data from other port (ignored): {msg.get('port')}")
                    return

                base64_data = msg.get("data", "")
                if not base64_data:
                    self.logger.warning(f"[RemoteSerial][{self.port}] Serial data empty (Base64)")
                    return

                try:
                    raw_data = base64.b64decode(base64_data, validate=True)
                    self.receive_buffer += raw_data
                except base64.binascii.Error as e:
                    self.logger.error(f"[RemoteSerial][{self.port}] Base64 decode failed: {str(e)}, data: {base64_data[:100]}...")

            else:
                self.logger.warning(f"[RemoteSerial][{self.port}] Unknown message type: {msg_type}, message: {msg_str[:200]}...")

        except json.JSONDecodeError as e:
            self.logger.error(f"[RemoteSerial][{self.port}] JSON parse failed: {str(e)}, raw: {msg_str[:200]}...")

    def _send_command(self, cmd: Dict[str, Any], timeout: float = 5.0) -> Dict[str, Any]:
        """
        Send command to remote server and wait for response
        :param cmd: command dict (will be converted to JSON)
        :param timeout: response timeout (seconds)
        :return: server response dict
        """
        if not self.is_open or not self.tcp_socket:
            raise SerialException(f"[RemoteSerial][{self.port}] Send command failed: serial not open")

        self.response_event.clear()
        self.last_response = {}
        cmd_str = json.dumps(cmd) + "\n"

        try:
            self.logger.debug(f"[RemoteSerial][{self.port}] Sending command (timeout {timeout}s): {cmd_str.strip()[:300]}...")
            self.tcp_socket.sendall(cmd_str.encode('utf-8'))

            # Wait for response or timeout
            if self.response_event.wait(timeout):
                return self.last_response
            else:
                raise SerialTimeoutException(
                    f"[RemoteSerial][{self.port}] Command response timeout ({timeout}s) - Command: {cmd.get('type')}"
                )
        except Exception as e:
            raise SerialException(f"[RemoteSerial][{self.port}] Send command exception: {str(e)}")

    def validate(self, password):
        try:
            validate_cmd = {
                "type": "validate",
                "password": password
            }
            self.is_open = True
            resp = self._send_command(validate_cmd, timeout=10.0)

            if not resp.get("success", False):
                self.logger.debug(f"[RemoteSerial][{self.port}] Validate failed")
                raise SerialException(f"[RemoteSerial][{self.port}] Remote serial validate failed: Wrong password")
            self.is_open = False
            self.logger.info(f"[RemoteSerial][{self.port}] Remote serial port validate successfully")
        except Exception as e:
            self.close()
            raise SerialException(f"[RemoteSerial][{self.port}] Validate serial failed: {str(e)}")

    def open(self):
        if self.is_open:
            self.logger.debug(f"[RemoteSerial][{self.port}] Serial already opened, skip")
            return
        self.logger.debug(f"[RemoteSerial][{self.port}] Trying to open serial")
        try:
            self.is_open = True
            open_cmd = {
                "type": "open_port",
                "port": self.port,
                "options": {
                    "baudRate": self.baudrate,
                    "dataBits": 8,
                    "stopBits": 1,
                    "parity": "none",
                    "timeout": 0.1
                }
            }
            resp = self._send_command(open_cmd, timeout=10.0)

            if not resp.get("success", False):
                self.logger.debug(f"[RemoteSerial][{self.port}] Open failed")
                self.is_open = False
                err_msg = resp.get("message", "Unknown error")
                raise SerialException(f"[RemoteSerial][{self.port}] Remote serial open failed: {err_msg}")

            # set is_open and start receive thread
            self.is_open = True

            self.logger.info(f"[RemoteSerial][{self.port}] Remote serial port opened successfully (baudrate: {self.baudrate})")
        except Exception as e:
            self.close()
            raise SerialException(f"[RemoteSerial][{self.port}] Open serial failed: {str(e)}")

    def close(self):
        """
        Close remote serial: send close_port command + cleanup TCP and thread
        """
        self.logger.debug(f"[RemoteSerial][{self.port}] Start closing remote serial")

        # 1. Mark state as closed (end receive thread loop)
        if not self.is_open:
            self.logger.debug(f"[RemoteSerial][{self.port}] Serial already closed, skip")
            return
        # 2. Send close serial command (try even if TCP error)
        try:
            if self.tcp_socket:
                close_cmd = {"type": "close_port", "port": self.port}
                resp = self._send_command(close_cmd, timeout=3.0)
                if resp.get("success", False):
                    self.logger.debug(f"[RemoteSerial][{self.port}] Remote serial close command succeeded")
                else:
                    self.logger.warning(f"[RemoteSerial][{self.port}] Remote serial close command failed: {resp.get('message', 'No response')}")
        except Exception as e:
            self.logger.error(f"[RemoteSerial][{self.port}] Send close command exception: {str(e)}")

        self.is_open = False
        self.receive_buffer = b""
        self.logger.debug(f"[RemoteSerial][{self.port}] Remote serial closed")

    def write(self, data: bytes):
        """
        Write binary data to remote serial (Base64 encode first)
        :param data: binary data to send
        """
        if not self.is_open:
            raise SerialException(f"[RemoteSerial][{self.port}] Write failed: serial not open")
        if not data:
            self.logger.debug(f"[RemoteSerial][{self.port}] Write empty data (ignored)")
            return

        try:
            base64_data = base64.b64encode(data).decode('utf-8')
            write_cmd = {
                "type": "write_data",
                "port": self.port,
                "data": base64_data
            }

            self.logger.debug(
                f"[RemoteSerial][{self.port}] Write data - "
                f"Raw length: {len(data)}B, Base64 length: {len(base64_data)}B"
            )
            resp = self._send_command(write_cmd, timeout=10.0)

            if not resp.get("success", False):
                raise SerialException(f"[RemoteSerial][{self.port}] Write data failed: {resp.get('message', 'Unknown error')}")
            self.logger.debug(f"[RemoteSerial][{self.port}] Write data succeeded")
        except Exception as e:
            raise SerialException(f"[RemoteSerial][{self.port}] Write exception: {str(e)}")

    def read(self, size: int = 1) -> bytes:
        """
        Read specified length of binary data from receive buffer
        :param size: length to read (bytes)
        :return: binary data read (returns actual length if less than size)
        """
        if not self.is_open:
            raise SerialException(f"[RemoteSerial][{self.port}] Read failed: serial not open")

        # Get data from buffer
        read_data = self.receive_buffer[:size]
        self.receive_buffer = self.receive_buffer[size:]
        self.logger.debug(
            f"[RemoteSerial][{self.port}] Read data - "
            f"Requested: {size}B, Read: {len(read_data)}B, Buffer left: {len(self.receive_buffer)}B"
        )
        return read_data

    def inWaiting(self) -> int:
        """
        Return number of bytes waiting in receive buffer
        :return: buffer length
        """
        if not self.is_open:
            raise SerialException(f"[RemoteSerial][{self.port}] Read failed: serial not open")
        waiting = len(self.receive_buffer)
        return waiting

    def flushInput(self):
        """
        Clear receive buffer (local only, no server interaction)
        """
        old_len = len(self.receive_buffer)
        self.receive_buffer = b""
        self.logger.debug(f"[RemoteSerial][{self.port}] Cleared receive buffer - old length: {old_len}B")

    def flushOutput(self):
        """
        Flush output buffer (no local output buffer for remote serial, just for serial.Serial API compatibility)
        """
        self.logger.debug(f"[RemoteSerial][{self.port}] flushOutput: No action required for remote serial (no local output buffer)")

    # ------------------------------
    # Simulate serial.Serial's DTR/RTS properties
    # ------------------------------
    @property
    def dtr(self):
        self.logger.debug(f"[RemoteSerial][{self.port}] Get DTR: not supported, return False")
        return False

    @dtr.setter
    def dtr(self, value):
        self.logger.debug(f"[RemoteSerial][{self.port}] Set DTR: not supported, ignored value={value}")

    @property
    def rts(self):
        self.logger.debug(f"[RemoteSerial][{self.port}] Get RTS: not supported, return False")
        return False

    @rts.setter
    def rts(self, value):
        self.logger.debug(f"[RemoteSerial][{self.port}] Set RTS: not supported, ignored value={value}")
    # ------------------------------
    # Context manager support (with statement)
    # ------------------------------
    def __enter__(self):
        self.open()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        self.logger.info(f"[RemoteSerial][{self.port}] __exit__ close")
        if exc_type:
            self.logger.error(f"[RemoteSerial][{self.port}] Context manager exception: {exc_type.__name__}: {exc_val}")

    def __del__(self):
        """Destructor: make sure to close resource"""
        if self.is_open:
            self.logger.debug(f"[RemoteSerial][{self.port}] Destructor: closing unreleased remote serial")
            self.close()