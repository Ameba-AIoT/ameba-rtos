#! /usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import argparse
import threading
import sys
import signal
import time
import os

# Import different terminal control modules for different operating systems
try:
    import tty
    import termios
    is_posix = True
except ImportError:
    import msvcrt
    is_posix = False

class SerialMonitor:
    def __init__(self, port, baudrate, reset_mode=False, debug=False):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        self.receive_thread = None
        self.reset_mode = reset_mode  # Whether to enable reset mode
        self.start_output = not reset_mode  # Whether to start output (False initially in reset mode)
        self.data_buffer = b''  # Buffer for received data in reset mode
        self.target_keyword = b'ROM:['  # Target keyword set to ROM:[
        self.debug = debug  # Debug mode switch
        self.old_settings = None  # Save original terminal settings

    def connect(self):
        """Connect to the specified serial port"""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=1,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )
            if self.ser.is_open:
                print(f"Successfully connected to {self.port}, baud rate: {self.baudrate}")
                if self.reset_mode:
                    print("Reset mode enabled: Will send reboot command after 100ms")
                else:
                    print("Press Ctrl+C to exit, input characters will be sent in real-time")
                if self.debug:
                    print("Debug mode enabled: Raw byte data of sent and received data will be displayed")
                return True
            return False
        except Exception as e:
            print(f"Connection failed: {str(e)}")
            return False

    def _disable_echo(self):
        """Disable terminal echo"""
        if is_posix:
            # Save current terminal settings
            self.old_settings = termios.tcgetattr(sys.stdin)
            # Set terminal to raw mode and disable echo
            tty.setraw(sys.stdin.fileno())
        # msvcrt.getch() in Windows doesn't echo by default, no extra settings needed

    def _restore_terminal(self):
        """Restore original terminal settings"""
        if is_posix and self.old_settings:
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.old_settings)

    def _process_and_print(self, data):
        """Process and print received data"""
        # Display raw byte data in debug mode
        if self.debug:
            hex_str = ' '.join(f'{b:02X}' for b in data)
            print(f"[Received Data (Hex)]: {hex_str}")
        
        try:
            text = data.decode('utf-8')
            print(f"{text}", end='', flush=True)
        except UnicodeDecodeError:
            hex_str = ' '.join(f'{b:02X}' for b in data)
            print(f"[Received (Hex)] {hex_str}")

    def receive_data(self):
        """Thread function for receiving serial data"""
        while self.running and self.ser.is_open:
            try:
                if self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    
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
                            self._process_and_print(data_to_output)  # Print data containing the keyword
                    else:
                        # Directly process if not in reset mode or output has started
                        self._process_and_print(data)
            except Exception as e:
                if self.running:
                    print(f"Receiving error: {str(e)}")
                break

    def send_data(self, data):
        """Send data to serial port without automatically adding terminator"""
        if self.ser and self.ser.is_open:
            try:
                # Prepare data to send (no automatic terminator added)
                data_to_send = data.encode('utf-8')
                
                # Display raw byte data in debug mode
                if self.debug:
                    hex_str = ' '.join(f'{b:02X}' for b in data_to_send)
                    print(f"[Sent Data (Hex)]: {hex_str}")
                
                self.ser.write(data_to_send)
            except Exception as e:
                print(f"Sending error: {str(e)}")

    def start(self):
        """Start the serial monitor"""
        if not self.connect():
            return

        # Disable terminal echo
        self._disable_echo()

        self.running = True
        # Start receiving thread
        self.receive_thread = threading.Thread(target=self.receive_data, daemon=True)
        self.receive_thread.start()

        # Execute special process if reset mode is enabled
        if self.reset_mode:
            try:
                time.sleep(0.1)  # Wait for 100ms
                self.send_data('reboot\r\n')  # Send reboot command (manually add \r\n)
            except Exception as e:
                print(f"Failed to send reboot command: {str(e)}")

        # Handle user input (send in real-time)
        try:
            while self.running:
                # Read a single character
                if is_posix:
                    # For POSIX systems (Linux/macOS)
                    char = sys.stdin.read(1)
                else:
                    # For Windows systems
                    char = msvcrt.getch().decode('utf-8')
                
                # Handle Ctrl+C (ASCII code 3 for Ctrl+C)
                if char == '\x03':
                    raise KeyboardInterrupt
                
                # Send the character in real-time
                self.send_data(char)
                
        except KeyboardInterrupt:
            self.stop()
        finally:
            # Ensure terminal settings are restored even if exception occurs
            self._restore_terminal()

    def stop(self):
        """Stop the serial monitor and clean up resources"""
        print("\nClosing connection...")
        self.running = False
        if self.receive_thread and self.receive_thread.is_alive():
            self.receive_thread.join(timeout=1.0)
        if self.ser and self.ser.is_open:
            self.ser.close()
        self._restore_terminal()  # Ensure terminal settings are restored
        print("Exited successfully")

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Serial Monitor (Real-time Sending, No Local Echo)')
    parser.add_argument('-p', '--port', help='Serial port name, e.g., COM3 (Windows) or /dev/ttyUSB0 (Linux)')
    parser.add_argument('-b', '--baudrate', type=int, help='Serial baud rate, e.g., 9600, 115200')
    parser.add_argument('-reset', action='store_true', 
                       help='Enable reset mode: Wait 100ms after connection to send "reboot" command, start output only after detecting "ROM:["')
    parser.add_argument('-debug', action='store_true', 
                       help='Enable debug mode: Display raw hexadecimal data of sent and received bytes')
    args = parser.parse_args()

    # Validate required arguments
    if not args.port or not args.baudrate:
        print("Error: Both -p/--port and -b/--baudrate are required arguments")
        sys.exit(1)

    # Create SerialMonitor instance
    monitor = SerialMonitor(args.port, args.baudrate, args.reset, args.debug)
    
    # Register signal handler for Ctrl+C
    def signal_handler(signal, frame):
        monitor.stop()
        sys.exit(0)
    
    signal.signal(signal.SIGINT, signal_handler)
    
    # Start the monitor
    monitor.start()

if __name__ == "__main__":
    main()