#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import socket
import subprocess
import sys
import ctypes
import threading
import time
import serial
import serial.tools.list_ports
import json
import signal
import logging
import base64
from typing import List, Dict, Optional
from dataclasses import dataclass
import select
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtCore import QObject, pyqtSignal
import os

APP_NAME = "AmebaSerialServer"
version = "1.0.3"
APP_VERSION = f"v{version}"
PORT = 58916
MAX_CONNECTIONS = 10
INSTALLER = 1

@dataclass
class SerialConfig:
    baud_rate: int = 9600
    data_bits: int = 8
    stop_bits: int = 1
    parity: str = "none"

@dataclass
class ClientInfo:
    serials: List[str]
    thread: threading.Thread
    validate: bool

class AmebaSerialServer:
    def __init__(self):
        # Initialize logger
        self.logger = self._setup_logger()
        self.server_socket: Optional[socket.socket] = None
        self.running = False
        self.connected_clients: Dict[socket.socket, ClientInfo] = {}
        self.open_serials: Dict[str, serial.Serial] = {}
        self.serial_lock = threading.Lock()
        self.tcp_lock = threading.Lock()
        self.start_time = time.time()
        self.cur_time = 0
        self.total_bytes = 0
        self.password = ""

    def _setup_logger(self) -> logging.Logger:
        """Set up and return a Logger object"""
        logger = logging.getLogger(APP_NAME)
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

    # --- Firewall Related Methods ---
    def is_admin(self) -> bool:
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False

    def setup_firewall(self) -> bool:
        self.logger.info("Checking firewall settings...")
        try:
            subprocess.run(
                ['netsh', 'advfirewall', 'firewall', 'show', 'rule', f'name={APP_NAME}'],
                check=True,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
            self.logger.info(f"Firewall rule '{APP_NAME}' already exists.")
            return True
        except subprocess.CalledProcessError:
            self.logger.info(f"Adding new firewall rule '{APP_NAME}'...")

        add_cmd = [
            'netsh', 'advfirewall', 'firewall', 'add', 'rule',
            f'name={APP_NAME}', 'dir=in', 'action=allow',
            'protocol=TCP', f'localport={PORT}'
        ]
        try:
            subprocess.run(add_cmd, check=True, capture_output=True, text=True)
            self.logger.info(f"Firewall rule '{APP_NAME}' added successfully.")
            return True
        except subprocess.CalledProcessError as e:
            self.logger.error(f"Failed to add firewall rule: {e.stderr}")
            return False
        except FileNotFoundError:
            self.logger.error("Error: 'netsh' command not found (may not be a Windows environment).")
            return False

    # --- TCP Server ---
    def start_server(self):
        self.running = True
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        try:
            self.server_socket.bind(('0.0.0.0', PORT))
            self.server_socket.listen(MAX_CONNECTIONS)
            self.logger.info(f"TCP Server started, listening on port {PORT}...")

            while self.running:
                try:
                    client_socket, client_addr = self.server_socket.accept()
                    with self.tcp_lock:
                        if len(self.connected_clients) >= MAX_CONNECTIONS:
                            self.logger.warning(f"Max connections reached, refusing: {client_addr[0]}:{client_addr[1]}")
                            client_socket.close()
                            continue
                        self.logger.info(f"New connection: {client_addr[0]}:{client_addr[1]}")
                        client_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                        self.logger.info(f"Set TCP_NODELAY for {client_addr}")

                        client_thread = threading.Thread(
                            target=self.handle_client,
                            args=(client_socket, client_addr),
                            daemon=True
                        )
                        client_validate = True
                        if self.password != "":
                            client_validate = False
                        self.connected_clients[client_socket] = ClientInfo(serials=[], thread=client_thread, validate=client_validate)
                        client_thread.start()
                except OSError:
                    if self.running:
                        self.logger.info("Server Socket closed, stopping accepting new connections.")
                    break
                except Exception as e:
                    if self.running:
                        self.logger.error(f"Error occurred while accepting connection", exc_info=True)
                    break
        except OSError as e:
            self.logger.critical(f"Failed to start Server: {e} (Port may already be occupied)")
            self.stop_server()

    def stop_server(self):
        if not self.running:
            return
        self.running = False
        self.logger.info("Shutting down the server...")

        # close Server Socket
        if self.server_socket:
            try:
                self.server_socket.close()
            except Exception as e:
                self.logger.error(f"Error occurred while closing server socket", exc_info=True)

        # Close all client connections
        with self.tcp_lock:
            for client_socket in list(self.connected_clients.keys()):
                try:
                    client_socket.close()
                except Exception as e:
                    self.logger.error(f"Error occurred while closing client", exc_info=True)
            self.connected_clients.clear()

        # Close all serial ports
        with self.serial_lock:
            for port, ser in self.open_serials.items():
                try:
                    if ser.is_open:
                        ser.close()
                        self.logger.info(f"Serial port {port} closed.")
                except Exception as e:
                    self.logger.error(f"Error occurred while closing serial port {port}", exc_info=True)
            self.open_serials.clear()

        self.logger.info("Server has been closed successfully.")

    def handle_client(self, client_socket: socket.socket, client_addr: tuple):
        buffer = ""
        addr_str = f"{client_addr[0]}:{client_addr[1]}"
        try:
            while self.running:
                data = client_socket.recv(1024)
                if not data:
                    self.logger.info(f"Client disconnected: {addr_str}")
                    break

                buffer += data.decode('utf-8', errors='ignore')
                while '\n' in buffer:
                    message_str, buffer = buffer.split('\n', 1)
                    message_str = message_str.strip()
                    if message_str:
                        result = self.process_client_command(client_socket, message_str)
                        if not result:
                            raise ConnectionAbortedError
        except (ConnectionResetError, ConnectionAbortedError):
            self.logger.info(f"Client connection interrupted: {addr_str}")
        except Exception as e:
            if self.running:
                self.logger.error(f"Unknown error while handling client {addr_str}", exc_info=True)
        finally:
            with self.tcp_lock:
                client = self.connected_clients.pop(client_socket, None)
                self.logger.info(f"Client disconnected pop: {addr_str}")
            if client:
                for port in client.serials:
                    self.close_serial_port(port)
            try:
                client_socket.close()
            except:
                pass

    def process_client_command(self, client_socket: socket.socket, command_str: str):
        self.logger.debug(f"Received command: {command_str}")
        response = None
        try:
            command = json.loads(command_str)
            cmd_type = command.get("type")
            self.logger.debug(f"Received command: {cmd_type}")

            if cmd_type == "validate":
                self.logger.info(f"Received command: {command_str}")
                pw = command["password"]
                success = True
                msg = ""
                if self.password != "":
                    if pw != self.password:
                        success = False
                        msg = "Password is mis-matched"
                response = {
                    "type": "command_response",
                    "success": success,
                    "message": msg
                }
                self.connected_clients[client_socket].validate = success
                self.send_to_client(client_socket, response)
                return success
            else:
                if not self.connected_clients[client_socket].validate:
                    response = {
                        "type": "command_response",
                        "success": False,
                        "message": "Password should be validated first"
                    }
                    self.send_to_client(client_socket, response)
                    return False
            if cmd_type == "list_com_ports":
                self.logger.info(f"Received command: {command_str}")
                current_ports = self.scan_com_ports()
                response = {
                    "type": "com_ports_update",
                    "ports": current_ports
                }

            elif cmd_type == "open_port":
                self.logger.info(f"Received command: {command_str}")
                port = command["port"]
                options = command["options"]
                success, msg = self.open_serial_port(
                    port=port,
                    baud_rate=options.get("baudRate", 9600),
                    data_bits=options.get("dataBits", 8),
                    stop_bits=options.get("stopBits", 1),
                    parity=options.get("parity", "none")
                )
                response = {"type": "command_response", "port": port, "success": success, "message": msg}
                self.total_bytes = 0
                if success:
                    self.connected_clients[client_socket].serials.append(port)

            elif cmd_type == "close_port":
                self.logger.info(f"Received command: {command_str}")
                port = command["port"]
                success, msg = self.close_serial_port(port)
                response = {"type": "command_response", "port": port, "success": success, "message": msg}
                self.total_bytes = 0
                if success:
                    self.connected_clients[client_socket].serials.remove(port)

            elif cmd_type == "write_data":
                port = command["port"]
                base64_data = command["data"]

                try:
                    raw_data = base64.b64decode(base64_data, validate=True)
                    data_len = len(raw_data)
                    data_hex = raw_data.hex(' ', 4)
                    #self.logger.debug(f"[Serial Write] Port: {port} | Length: {data_len} Bytes | Hex: {data_hex}")
                    self.logger.debug(f"[Serial Write] Port: {port} | Length: {data_len} Bytes")

                    success, msg = self.write_serial_data(port, raw_data)
                    response = {
                        "type": "command_response",
                        "port": port,
                        "success": success,
                        "message": msg
                    }
                    self.cur_time = time.time()
                    self.total_bytes += data_len
                    if self.cur_time - self.start_time >= 1.0:
                        self.logger.info(f"[Serial Write] Port: {port} | Speed: {self.total_bytes/(self.cur_time - self.start_time)} Bytes/sec")
                        self.start_time = self.cur_time
                        self.total_bytes = 0
                except base64.binascii.Error as e:
                    self.logger.error(f"Base64 decode failed: {e}")
                    response = {
                        "type": "command_response", "port": port,
                        "success": False, "message": f"Base64 decode failed: {e}"
                    }
            else:
                raise ValueError(f"Unknown command type: {cmd_type}")

            if response:
                self.send_to_client(client_socket, response)

        except json.JSONDecodeError:
            self.logger.error(f"Command JSON parsing failed: {command_str}")
            error_response = {"type": "command_response", "success": False, "message": f"Command JSON parsing failed", "port": ""}
            self.send_to_client(client_socket, error_response)
        except Exception as e:
            self.logger.error(f"Error occurred while processing the command: {command_str}", exc_info=True)
            port_in_cmd = command.get("port", "") if 'command' in locals() else ""
            error_response = {"type": "command_response", "success": False, "message": f"Command processing error: {e}", "port": port_in_cmd}
            self.send_to_client(client_socket, error_response)
        return True

    # --- Serial port ---
    def scan_com_ports(self) -> List[str]:
        ports = []
        try:
            for port in serial.tools.list_ports.comports():
                if port.device:
                    ports.append(port.device)
        except Exception as e:
            self.logger.error("Error occurred while scanning COM ports", exc_info=True)
        return sorted(ports)

    def open_serial_port(self, port: str, baud_rate: int, data_bits: int, stop_bits: int, parity: str) -> tuple[bool, str]:
        with self.serial_lock:
            if port in self.open_serials:
                self.logger.warning(f"Attempted to open an already opened serial port {port}")
                return True, "Serial port already opened"
            try:
                parity_map = {"none": serial.PARITY_NONE, "even": serial.PARITY_EVEN, "odd": serial.PARITY_ODD}
                stop_bits_map = {1: serial.STOPBITS_ONE, 1.5: serial.STOPBITS_ONE_POINT_FIVE, 2: serial.STOPBITS_TWO}

                self.logger.info(f"Opening serial port {port} (Baud: {baud_rate}, Config: {data_bits}{parity[0].upper()}{stop_bits})...")

                ser = serial.Serial(
                    port=port,
                    baudrate=baud_rate,
                    bytesize=data_bits,
                    parity=parity_map.get(parity, serial.PARITY_NONE),
                    stopbits=stop_bits_map.get(stop_bits, serial.STOPBITS_ONE),
                    timeout=0.001
                )

                if ser.is_open:
                    self.open_serials[port] = ser
                    threading.Thread(target=self.listen_serial_data, args=(port, ser), daemon=True).start()
                    self.logger.info(f"Serial port {port} opened successfully.")
                    return True, "Serial port opened successfully"
                else:
                    self.logger.error(f"Serial port {port} failed to open (unknown error).")
                    return False, "Failed to open serial port (unknown error)"
            except Exception as e:
                self.logger.error(f"Failed to open serial port {port}: {e}", exc_info=False)
                return False, f"Open failed: {e}"

    def close_serial_port(self, port: str) -> tuple[bool, str]:
        with self.serial_lock:
            if port not in self.open_serials:
                self.logger.warning(f"Attempted to close a serial port that is not open or already closed {port}")
                return True, "Serial port closed or not opened"
            try:
                ser = self.open_serials.pop(port)
                if ser.is_open:
                    ser.close()
                self.logger.info(f"Serial port {port} closed successfully.")
                return True, "Serial port closed successfully"
            except Exception as e:
                self.logger.error(f"Error occurred while closing serial port {port}", exc_info=True)
                return False, f"Close failed: {e}"

    def write_serial_data(self, port: str, data: bytes) -> tuple[bool, str]:
        with self.serial_lock:
            if port not in self.open_serials:
                self.logger.error(f"Write failed: Serial port {port} is not opened.")
                return False, "Serial port not opened"
            try:
                ser = self.open_serials[port]
                ser.write(data)
                return True, f"Binary data sent successfully (length: {len(data)} bytes)"
            except Exception as e:
                self.logger.error(f"Error occurred while writing to serial port {port}", exc_info=True)
                return False, f"Send failed: {e}"

    def listen_serial_data(self, port: str, ser: serial.Serial):
        while self.running and port in self.open_serials and ser.is_open:
            try:
                #rlist, _, _ = select.select([ser], [], [], 0.001)
                #if rlist:
                #    raw_data = ser.read(ser.in_waiting)
                raw_data = ser.read(1)
                if not raw_data:
                    continue
                raw_data += ser.read(ser.in_waiting)
                base64_data = base64.b64encode(raw_data).decode('utf-8')
                self.logger.debug(f"[Serial Receive] Port: {port} | Raw Length: {len(raw_data)} | Base64 Length: {len(base64_data)}")

                message = {
                    "type": "serial_data",
                    "port": port,
                    "data": base64_data
                }
                with self.tcp_lock:
                    for client_socket in list(self.connected_clients.keys()):
                        self.send_to_client(client_socket, message)

                self.logger.debug(f"[Serial Receive] tcp send complete")
            except Exception as e:
                # If the serial port is pulled out, an exception will be thrown here
                self.logger.error(f"Exception occurred while listening to serial port {port}, listening stopped.", exc_info=True)
                self.close_serial_port(port)
                break

    def send_to_client(self, client_socket: socket.socket, message: dict):
        if not self.running or client_socket not in self.connected_clients:
            return
        try:
            message_str = json.dumps(message) + "\n"
            #self.logger.error(f"Serial port sent tcp length:{len(message_str)}")
            client_socket.sendall(message_str.encode('utf-8'))
        except Exception as e:
            self.logger.error(f"Error occurred while sending message to client", exc_info=True)

class QTextEditLogger(QObject, logging.Handler):
    new_record = pyqtSignal(str)

    def __init__(self, text_edit):
        QObject.__init__(self)
        logging.Handler.__init__(self)
        self.widget = text_edit
        self.new_record.connect(self.append_log)

    def emit(self, record):
        msg = self.format(record)
        self.new_record.emit(msg)

    def append_log(self, msg):
        self.widget.appendPlainText(msg)

class MsFormatter(logging.Formatter):
    def formatTime(self, record, datefmt=None):
        ct = self.converter(record.created)
        t = time.strftime("%Y-%m-%d %H:%M:%S", ct)
        ms = int(record.msecs)
        return "%s.%03d" % (t, ms)

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, tray_icon, logger):
        super().__init__()
        self.tray_icon = tray_icon
        self.setWindowTitle(f"{APP_NAME} {APP_VERSION}")
        self.resize(800, 500)
        self.log_edit = QtWidgets.QPlainTextEdit(self)
        self.log_edit.setReadOnly(True)
        self.setCentralWidget(self.log_edit)

        qt_handler = QTextEditLogger(self.log_edit)
        formatter = MsFormatter("%(asctime)s [%(levelname)s]: %(message)s")
        qt_handler.setFormatter(formatter)
        logger.addHandler(qt_handler)

    def append_log(self, text):
        self.log_edit.appendPlainText(text)

    def closeEvent(self, event):
        event.ignore()
        self.hide()
        self.tray_icon.showMessage(f"{APP_NAME} {APP_VERSION}", "The program has been minimized to the system tray.", QtWidgets.QSystemTrayIcon.Information, 2000)

    def changeEvent(self, event):
        if event.type() == QtCore.QEvent.WindowStateChange:
            if self.isMinimized():
                self.showMinimized()

class TrayApp(QtWidgets.QSystemTrayIcon):
    def __init__(self, icon, main_window=None, parent=None, server=None):
        super().__init__(icon, parent)
        self.main_window = main_window
        self.server = server
        menu = QtWidgets.QMenu()

        set_password_action = menu.addAction("Set Password")
        set_password_action.triggered.connect(self.set_password)

        set_log_level_action = menu.addAction("Set Log Level")
        set_log_level_action.triggered.connect(self.set_log_level)

        show_action = menu.addAction("Show Main Window")
        show_action.triggered.connect(self.show_main_window)
        menu.addSeparator()

        exit_action = menu.addAction("Exit")
        exit_action.triggered.connect(self.exit_all)
        self.setContextMenu(menu)
        self.setToolTip(f"{APP_NAME} {APP_VERSION}")
        self.activated.connect(self.on_tray_activated)

    def show_main_window(self):
        if self.main_window.isHidden():
            self.main_window.showNormal()
            self.main_window.activateWindow()

    def set_password(self):
        text, ok = QtWidgets.QInputDialog.getText(
            self.main_window, "Set Password", "Please input new password", QtWidgets.QLineEdit.Normal)
        if ok and text:
            QtWidgets.QMessageBox.information(self.main_window, "Set finished", "New password is set! Restart server after 100ms")
            if self.server.password != text:
                self.server.logger.info("Set new password, restart server...")
                self.server.stop_server()
                time.sleep(0.1)
                server_thread = threading.Thread(target=self.server.start_server, daemon=True)
                server_thread.start()
                self.server.password = text

    def set_log_level(self):
        log_levels = {"Debug": logging.DEBUG, "Info": logging.INFO, "Warning": logging.WARN, "Error": logging.ERROR}
        level, ok = QtWidgets.QInputDialog.getItem(
            self.main_window, "Set Log Level", "Log Level:", log_levels.keys(), 1, False)
        if ok and level:
            QtWidgets.QMessageBox.information(self.main_window, "Set finished", f"Log level is set to: {level}")
            self.server.logger.setLevel(log_levels[level])

    def on_tray_activated(self, reason):
        if reason == QtWidgets.QSystemTrayIcon.DoubleClick:
            if self.main_window.isHidden():
                self.main_window.showNormal()
                self.main_window.activateWindow()

    def exit_all(self):
        self.server.logger.info("Click menu Exit, exiting...")
        self.server.stop_server()
        QtWidgets.qApp.quit()

if __name__ == '__main__':
    server = AmebaSerialServer()

    def handle_exit_signal(signal_num, frame):
        server.logger.info("Received exit signal (Ctrl+C), shutting down program...")
        server.stop_server()
        time.sleep(0.5)
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_exit_signal)

    if sys.platform == "win32" and not server.is_admin():
        print("Administrator privileges are required to set the firewall, attempting to restart as administrator...")
        try:
            ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
        except Exception as e:
            print(f"Automatic elevation failed: {e}")
            print("Please manually run this script as administrator.")
            input("Press Enter to exit.")
        sys.exit(0)
    else:
        if sys.platform == "win32":
            server.logger.info("Administrator privileges obtained.")
            server.setup_firewall()
        else:
            server.logger.info("Running on non-Windows platform, skipping firewall setup.")

    if INSTALLER:
        def resource_path(rel_path):
            if hasattr(sys, '_MEIPASS'):
                return os.path.join(sys._MEIPASS, rel_path)
            return os.path.join(os.path.abspath('.'), rel_path)

        app = QtWidgets.QApplication(sys.argv)
        app.setQuitOnLastWindowClosed(False)
        icon = QtGui.QIcon(resource_path("realtek.ico"))
        main_win = MainWindow(None, server.logger)
        tray_icon = TrayApp(icon=icon,
                            main_window=main_win,
                            server=server)
        main_win.tray_icon = tray_icon
        main_win.setWindowIcon(icon)
        tray_icon.show()
        main_win.show()

        QtWidgets.QApplication.processEvents()
        main_win.showNormal()

        tray_icon.showMessage(f"{APP_NAME} {APP_VERSION}",
                            "The server is already running in the background. Right-click to exit.",
                            QtWidgets.QSystemTrayIcon.Information,
                            3000)

        server.logger.info("The server is running...")
        server_thread = threading.Thread(target=server.start_server, daemon=True)
        server_thread.start()

        try:
            sys.exit(app.exec_())
        except Exception as e:
            server.logger.critical("Tray program encountered an error", exc_info=True)
            sys.exit(1)
    else:
        try:
            server_thread = threading.Thread(target=server.start_server, daemon=True)
            server_thread.start()

            # Show prompt only when server thread is alive
            if server_thread.is_alive():
                print("\nThe server is running... Press Enter or Ctrl+C to exit.\n")
                # Wait for user to input Enter or Ctrl+C
                input()
            server.stop_server()

        except Exception as e:
            server.logger.critical("A critical unexpected error occurred in the program", exc_info=True)
            server.stop_server()

        server.logger.info("Program exit successfully!")
        sys.exit(0)