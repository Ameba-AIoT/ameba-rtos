#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import io
import os
import time

from .errno import *

STX = 0x02  # Transfer data
EOT = 0x04  # End of transfer
BAUDSET = 0x05  # For handshake
BAUDCHK = 0x07  # For handshake, only for UART
ACK = 0x06  # Acknowledge
NAK = 0x15  # Negativ acknowledge
CAN = 0x18  # Cancel transfer, only for UART
ESC = 0x1B  # Cancel transfer

STX_BASE_LEN = 8
STX_UART_RAM_DATA_LEN = 1024
STX_USB_RAM_DATA_LEN = 2048
STX_MAX_LEN = STX_BASE_LEN + STX_USB_RAM_DATA_LEN
FloaderSizeLimit = 1024 * 1024  # 1MB

StxBaseLen = 1024
StxUartDataLen = 1024
StxUsbDataLen = 2048
StxMaxDataLen = StxBaseLen + StxUsbDataLen
DEFAULT_TIMEOUT = 0.5
STX_TIMEOUT = 1

FloaderDictionary = "devices/Floaders"


class RomHandler(object):
    def __init__(self, ameba_obj, padding="FF"):
        self.ameba = ameba_obj
        self.serial_port = ameba_obj.serial_port
        self.logger = ameba_obj.logger
        self.profile = ameba_obj.profile_info
        self.baudrate = ameba_obj.baudrate
        self.is_usb = ameba_obj.is_usb
        self.stx_packet_no = 1
        self.padding = padding
        self.setting = ameba_obj.setting

    def get_baudrate_idx(self, rate):
        ''' rom built-in rate table '''
        return {
            110: 0,
            300: 1,
            600: 2,
            1200: 3,
            2400: 4,
            4800: 5,
            9600: 6,
            14400: 7,
            19200: 8,
            28800: 9,
            38400: 10,
            57600: 11,
            76800: 12,
            115200: 13,
            128000: 14,
            153600: 15,
            230400: 16,
            380400: 17,
            460800: 18,
            500000: 19,
            921600: 20,
            1000000: 21,
            1382400: 22,
            1444400: 23,
            1500000: 24,
            1843200: 25,
            2000000: 26,
            2100000: 27,
            2764800: 28,
            3000000: 29,
            3250000: 30,
            3692300: 31,
            3750000: 32,
            4000000: 33,
            6000000: 34
        }.get(rate, 13)

    def send_request(self, request, length, timeout):
        ret = ErrType.SYS_UNKNOWN
        response = []

        try:
            for retry in range(2):
                if retry > 0:
                    self.logger.debug(f"Request retry {retry}#: len={length}, payload={request.hex()}")
                else:
                    self.logger.debug(f"Request: len={length}, payload={request.hex()}")

                self.serial_port.flushInput()
                self.serial_port.flushOutput()

                self.ameba.write_bytes(request)

                for resp_retry in range(3):
                    ret, ch = self.ameba.read_bytes(timeout)
                    if ret != ErrType.OK:
                        self.logger.error(f"Reponse error: {ret}")
                        break

                    if ch[0] == ACK:
                        self.logger.debug(f"Response ACK")
                        break
                    elif ch[0] == NAK:
                        ret = ErrType.SYS_NAK
                        self.logger.debug(f"Response NAK")
                        continue
                    elif ch[0] == CAN:
                        ret = ErrType.SYS_CANCEL
                        self.logger.debug(f"Response CAN")
                        break
                    else:
                        ret = ErrType.SYS_PROTO
                        self.logger.debug(f"Unexpected reponse: 0x{ch.hex()}")
                        continue

                if ret == ErrType.OK or ret == ErrType.SYS_CANCEL:
                    break
                else:
                    time.sleep(self.setting.request_retry_interval_second)
        except Exception as err:
            self.logger.error(f"Send request exception: {err}")
            ret = ErrType.SYS_IO

        return ret

    def reset(self):
        self.stx_packet_no = 1

    def handshake(self):
        ret = ErrType.OK
        self.logger.debug(f"Handshake:{'USB' if self.is_usb else 'UART'}")
        index = self.get_baudrate_idx(self.baudrate)

        if not self.is_usb:
            ret = self.check_alive()
            if ret != ErrType.OK:
                self.logger.debug(f"Handshake fail: device not alive")
                return ret

        retry = 0
        while retry < 2:
            if retry > 0:
                self.logger.debug(f"Handshake retry {retry}#")

            retry += 1

            if self.setting.switch_baudrate_at_floader == 0:
                self.logger.debug(f"BAUDSET: {index} ({self.baudrate})")
                _bytes = [BAUDSET]
                _bytes.append(index)
                cmd = bytearray(_bytes)

                ret = self.send_request(cmd, 2, DEFAULT_TIMEOUT)
                if ret == ErrType.OK:
                    self.logger.debug(f"BAUDSET ok")
                else:
                    self.logger.debug(f"BAUDSET fail: {ret}")
                    continue

                if self.is_usb:
                    break

                ret = self.ameba.switch_baudrate(self.baudrate, self.setting.baudrate_switch_delay_in_second)
                if ret != ErrType.OK:
                    continue

                self.logger.debug(f"BAUDCHK")
                ret = self.send_request(BAUDCHK.to_bytes(1, byteorder="little"), 1, DEFAULT_TIMEOUT)
                if ret == ErrType.OK:
                    self.logger.debug("BAUDCHK ok")
                    break
                else:
                    self.logger.error(f"BAUDCHK fail: {ret}")
            else:
                if self.is_usb:
                    self.logger.debug(f"BAUDSET")
                    _bytes = [BAUDSET]
                    _bytes.append(index)
                    cmd = bytearray(_bytes)
                    ret = self.send_request(cmd, 2, DEFAULT_TIMEOUT)
                    if ret == ErrType.OK:
                        self.logger.debug(f"Baudset ok")
                        break
                    else:
                        self.logger.debug(f"Baudset fail: {ret}")
                else:
                    self.logger.debug("BAUDCHK")
                    ret = self.send_request(BAUDCHK.to_bytes(1, byteorder="little"), 1, DEFAULT_TIMEOUT)
                    if ret == ErrType.OK:
                        self.logger.debug(f"Baudchk ok")
                        break
                    else:
                        self.logger.debug(f"Baudchk fail: {ret}")

        if ret == ErrType.OK:
            self.logger.debug("Handshake done")
        else:
            self.logger.debug(f"Handshake fail: {ret}")

        return ret

    def check_alive(self):
        ret, ch = self.ameba.read_bytes(DEFAULT_TIMEOUT)
        if ret == ErrType.OK:
            if ch[0] != NAK:
                self.logger.debug(f"Check alive error,expect NAK, get 0x{ch.hex()}")
        else:
            self.logger.debug(f"Check alive error: {ret}")

        return ret

    def transfer(self, address, data_bytes):
        self.logger.debug(f"STX {self.stx_packet_no}#: addr={hex(address)}")
        stx_data = [STX]
        stx_data.append(self.stx_packet_no & 0xFF)
        stx_data.append((~self.stx_packet_no) & 0xFF)

        stx_data.extend(list(address.to_bytes(4, byteorder='little')))

        stx_bytes = bytearray(stx_data)
        stx_bytes += data_bytes

        checksum = sum(stx_bytes[3:]) % 256
        stx_bytes += checksum.to_bytes(1, byteorder="little")

        ret = self.send_request(stx_bytes, len(stx_bytes), STX_TIMEOUT)
        if ret == ErrType.OK:
            self.logger.debug(f"STX {self.stx_packet_no}# done")
            self.stx_packet_no += 1
        else:
            self.logger.debug(f"STX {self.stx_packet_no}# fail: {ret}")

        return ret

    def end_transfer(self):
        self.logger.debug(f"EOT")
        return self.send_request(EOT.to_bytes(1, byteorder="little"), 1, DEFAULT_TIMEOUT)

    def abort(self):
        self.logger.info(f"ESC")
        self.send_request(ESC.to_bytes(1, byteorder="little"), 1, DEFAULT_TIMEOUT)

    def get_page_aligned_size(self, size, page_size):
        result = size

        if (size % page_size) != 0:
            result = (size // page_size + 1) * page_size

        return result

    def get_floader_path(self):
        floader_path = None

        if self.profile.floader:
            floader_path = os.path.join(os.path.abspath(os.path.dirname(os.path.dirname(__file__))), FloaderDictionary,
                                        self.profile.floader)

        if floader_path is None:
            self.logger.error("Flashloader not specified in device profile")
        elif not os.path.exists(floader_path):
            self.logger.error(f"Flashloader not found: {floader_path}")

        return floader_path

    def download_floader(self):
        ret = ErrType.OK
        page_size = StxUsbDataLen if self.is_usb else StxUartDataLen
        self.reset()

        floader = self.get_floader_path()
        if floader is None:
            return ErrType.SYS_PARAMETER
        with open(floader, 'rb') as stream:
            floader_content = stream.read()

        floader_size = len(floader_content)
        if floader_size > FloaderSizeLimit:
            self.logger.error(f"Invalid floader {floader} with too large size: {floader_size}")
            return ErrType.SYS_OVERRANGE

        _baud_bytes = self.serial_port.baudrate.to_bytes(4, byteorder='little')

        floader_aligned_size = self.get_page_aligned_size(floader_size, page_size)
        datas = io.BytesIO(floader_content)
        data_bytes = datas.read(floader_size)

        data_bytes += _baud_bytes

        data_bytes = data_bytes.ljust(floader_aligned_size, b"\x00")

        idx = 0
        floader_addr = self.profile.floader_address
        while idx < floader_aligned_size:
            trans_data = data_bytes[idx:idx+page_size]
            ret = self.transfer(floader_addr, trans_data)
            if ret != ErrType.OK:
                return ret
            idx += page_size
            floader_addr += page_size

        ret = self.end_transfer()
        if ret != ErrType.OK:
            if self.profile.is_amebaz():
                # AmebaZ would tx ACK after rx EOT but would not wait tx ACK done, so in low baudrate(115200...),
                # ACK will not be received
                # This issue has been fixed after AmebaD
                ret = ErrType.OK

        return ret
