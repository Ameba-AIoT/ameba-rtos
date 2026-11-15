#! /usr/bin/env python
# -*- coding: utf-8 -*-

# pyserial-compatible transport for the Realtek USB download port (VID 0x0BDA) that bypasses
# the macOS CDC-ACM driver. That driver splits every write into <=384-byte USB transfers, while
# the ROM USB download handler treats each OUT transfer as one complete command (an STX packet
# must arrive as a single 2056-byte transfer), so every STX is NAKed. Writing through libusb
# sends one request as one bulk transfer.

import time
import usb.core
import usb.util
import usb.backend.libusb1

try:
    import libusb_package
except ImportError:
    libusb_package = None

_CDC_SET_CONTROL_LINE_STATE = 0x22
_CDC_DATA_INTERFACE_CLASS = 0x0A


def _backend():
    if libusb_package is not None:
        return usb.backend.libusb1.get_backend(find_library=lambda _: libusb_package.get_library_path())
    return usb.backend.libusb1.get_backend()


class UsbCdcSerial(object):
    def __init__(self, port, vid, serial_number=None, location=None, baudrate=115200, timeout=None,
                 open_timeout=5.0):
        self.port = port
        self.vid = vid
        self.serial_number = serial_number
        self.location = location
        self.baudrate = baudrate
        self.timeout = timeout
        self.bytesize = 8
        self.parity = 'N'
        self.stopbits = 1
        self.dtr = True
        self.rts = True
        self.open_timeout = open_timeout
        self._backend = _backend()
        self._dev = None
        self._intf = None
        self._ep_out = None
        self._ep_in = None
        self._rx = bytearray()
        self.open()

    @property
    def is_open(self):
        return self._dev is not None

    def _location_of(self, dev):
        ports = getattr(dev, 'port_numbers', None)
        return f"{dev.bus}-{'.'.join(str(p) for p in ports)}" if ports else None

    def _find(self):
        # Match by location only when it is known: the ROM/flashloader USB serial string is the same
        # on every board, so falling back to it could bind another board on a multi-board host
        devs = list(usb.core.find(find_all=True, idVendor=self.vid, backend=self._backend) or [])
        if self.location:
            return next((dev for dev in devs if self._location_of(dev) == self.location), None)
        if self.serial_number:
            for dev in devs:
                try:
                    if usb.util.get_string(dev, dev.iSerialNumber) == self.serial_number:
                        return dev
                except Exception:
                    continue
            return None
        if len(devs) > 1:
            raise IOError(f"{self.port}: {len(devs)} Realtek USB devices found, cannot tell which one is this port")
        return devs[0] if devs else None

    def open(self):
        if self._dev is not None:
            return
        deadline = time.monotonic() + self.open_timeout
        while True:
            dev = self._find()
            if dev is not None:
                try:
                    self._claim(dev)
                    return
                except usb.core.USBError as err:
                    # The device is re-enumerating (ROM -> flashloader); retry until it settles
                    try:
                        usb.util.dispose_resources(dev)
                    except Exception:
                        pass
                    if time.monotonic() > deadline:
                        raise IOError(f"{self.port}: open failed: {err}")
            elif time.monotonic() > deadline:
                raise IOError(f"{self.port}: Realtek USB device not found")
            time.sleep(0.1)

    def _claim(self, dev):
        cfg = dev.get_active_configuration()
        intf = None
        for itf in cfg:
            if itf.bInterfaceClass == _CDC_DATA_INTERFACE_CLASS:
                intf = itf
                break
        if intf is None:
            raise IOError(f"{self.port}: no CDC data interface")
        ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(
            e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
        ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(
            e.bEndpointAddress) == usb.util.ENDPOINT_IN)
        # The kernel CDC driver stays attached; claiming the data interface needs no root on macOS
        usb.util.claim_interface(dev, intf.bInterfaceNumber)
        self._dev, self._intf, self._ep_out, self._ep_in = dev, intf.bInterfaceNumber, ep_out, ep_in
        self._rx = bytearray()
        # The ROM switches from UART xmodem to USB download only after the host "opens" the
        # virtual COM port, i.e. SET_CONTROL_LINE_STATE with RTS set
        self._set_control_line_state()

    def _set_control_line_state(self):
        value = (0x01 if self.dtr else 0) | (0x02 if self.rts else 0)
        ctx = self._dev._ctx
        ctx.managed_open()
        ctx.backend.ctrl_transfer(ctx.handle, 0x21, _CDC_SET_CONTROL_LINE_STATE, value, 0,
                                  usb.util.create_buffer(0), 1000)

    def close(self):
        if self._dev is None:
            return
        try:
            usb.util.release_interface(self._dev, self._intf)
        except Exception:
            pass
        try:
            usb.util.dispose_resources(self._dev)
        except Exception:
            pass
        self._dev = None
        self._rx = bytearray()

    def write(self, data):
        if self._dev is None:
            raise IOError(f"{self.port}: port not open")
        data = bytes(data)
        n = self._ep_out.write(data, timeout=5000)
        return n

    def _pull(self, timeout_ms):
        try:
            chunk = self._ep_in.read(512, timeout=max(1, int(timeout_ms)))
            self._rx += bytes(chunk)
            return len(chunk)
        except usb.core.USBTimeoutError:
            return 0

    def read(self, size=1):
        if self._dev is None:
            raise IOError(f"{self.port}: port not open")
        timeout = self.timeout
        deadline = None if timeout is None else time.monotonic() + timeout
        while len(self._rx) < size:
            if deadline is None:
                remaining_ms = 1000
            else:
                remaining_ms = (deadline - time.monotonic()) * 1000
                if remaining_ms <= 0:
                    break
            self._pull(min(remaining_ms, 50))
        out = bytes(self._rx[:size])
        del self._rx[:size]
        return out

    def flushInput(self):
        self._rx = bytearray()
        if self._dev is not None:
            while self._pull(1):
                pass

    reset_input_buffer = flushInput

    def flushOutput(self):
        pass

    reset_output_buffer = flushOutput
