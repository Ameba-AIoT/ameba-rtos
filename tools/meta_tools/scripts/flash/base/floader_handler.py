#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

import time
import ctypes

from .sense_status import *
from .device_info import *
from .next_op import *
from .flash_utils import *

BAUDSET = 0x81
QUERY = 0x02
CONFIG = 0x83
WRITE = 0x84
READ = 0x05
CHKSM = 0x06
SENSE = 0x07
NEXTOP = 0x08

FS_ERASE = 0xA0
FS_RDSTS = 0x21
FS_WTSTS = 0xA2
FS_MKBAD = 0xA3
FS_CHKMAP = 0x24
FS_CHKBAD = 0x25
FS_CHKBLK = 0x26

OTP_RRAW = 0x40
OTP_WRAW = 0xC1
OTP_RMAP = 0x42
OTP_WMAP = 0xC3

ACK_BUF_EMPTY = 0xB0
ACK_BUF_FULL = 0xB1

NP_EXIT = 0x01
NP_BAUDRATE_RECOVER = 0x02

SOF = 0xA5

QUERY_DATA_OFFSET_DID = 0
QUERY_DATA_OFFSET_IMAGE_TYPE = 2
QUERY_DATA_OFFSET_CMD_SET_VERSION = 4
QUERY_DATA_OFFSET_MEMORY_TYPE = 6
QUERY_DATA_OFFSET_FLASH_MID = 7
QUERY_DATA_OFFSET_FLASH_DID = 8
QUERY_DATA_OFFSET_FLASH_MFG = 10
QUERY_DATA_OFFSET_FLASH_MODEL = 22
QUERY_DATA_OFFSET_FLASH_PAGE_SIZE = 42
QUERY_DATA_OFFSET_FLASH_OOB_SIZE = 46
QUERY_DATA_OFFSET_FLASH_PAGES_PER_BLOCK = 48
QUERY_DATA_OFFSET_FLASH_BLOCKS_PER_LUN = 52
QUERY_DATA_OFFSET_FLASH_LUNS_PER_TARGET = 56
QUERY_DATA_OFFSET_FLASH_MAX_BAD_BLOCKS_PER_LUN = 57
QUERY_DATA_OFFSET_FLASH_REQ_HOST_ECC_LEVEL = 59
QUERY_DATA_OFFSET_FLASH_TARGETS = 60
QUERY_DATA_OFFSET_FLASH_CAPACITY = 61
QUERY_DATA_OFFSET_WIFI_MAC = 65

# Read otp logical map time:26ms, physical map time: 170ms
# Program otp logical map time: 7ms
OTP_READ_TIMEOUT_IN_SECONDS = 10


class FloaderHandler(object):
    def __init__(self, ameba_obj):
        self.ameba = ameba_obj
        self.serial_port = ameba_obj.serial_port
        self.profile = ameba_obj.profile_info
        self.logger = ameba_obj.logger
        self.setting = ameba_obj.setting
        super().__init__()

    def send_request(self, request, length, timeout, is_sync=True):
        ret = ErrType.SYS_UNKNOWN
        response_bytes = None

        len_l = length & 0xFF
        len_h = (length >> 8) & 0xFF

        frame_data = [(SOF)]
        frame_data.append(len_l)
        frame_data.append(len_h)
        frame_data.append((len_l ^ len_h) & 0xFF)

        frame_bytes = bytearray(frame_data)
        frame_bytes += request[:length]
        checksum = sum(request)

        frame_bytes += (checksum & 0xFF).to_bytes(1, byteorder="little")

        try:
            retry = 0
            while retry < self.setting.request_retry_count:
                retry += 1

                self.serial_port.flushInput()
                self.serial_port.flushOutput()

                self.ameba.write_bytes(frame_bytes)
                self.logger.debug(f"Request: len={length}, payload={request.hex()}")

                ret, ret_byte = self.ameba.read_bytes(timeout)
                if ret != ErrType.OK:
                    self.logger.error(f"Response error: {ret}, timeout:{timeout}")
                    continue
                if is_sync:
                    if ret_byte[0] == SOF:
                        ret, ret_bytes = self.ameba.read_bytes(timeout, size=3)
                        if ret == ErrType.OK:
                            len_l = ret_bytes[0]
                            len_h = ret_bytes[1]
                            len_xor = ret_bytes[2]
                            response_len = (len_h << 8) + len_l
                            if len_xor == (len_l ^ len_h):
                                ret, response_bytes = self.ameba.read_bytes(self.setting.async_response_timeout_in_second,
                                                                            size=response_len + 1)
                                if ret == ErrType.OK:
                                    if response_len >= len(response_bytes) - 1:
                                        self.logger.debug(
                                            f"Response: len={response_len}, payload={response_bytes.hex()}")
                                        checksm = sum(response_bytes[:response_len]) % 256
                                        if checksm == response_bytes[response_len]:
                                            self.logger.debug(f"Checksum={checksm}({hex(checksm)}), ok")
                                            break
                                        else:
                                            self.logger.debug(
                                                f"Response checksum error: expect {hex(checksm)}, get {hex(response_bytes[response_len + 1])}")
                                            ret = ErrType.SYS_CHECKSUM
                                    else:
                                        ret = ErrType.SYS_PROTO
                                        self.logger.debug(
                                            f"Response length error:  expect {len_xor}, get {len_l ^ len_h}")
                                else:
                                    self.logger.debug(f"Read response payload error: {ret}")
                            else:
                                ret = ErrType.SYS_PROTO
                                self.logger.debug(f"Response length check error:  {ret_bytes.hex()}")
                        else:
                            ret = ErrType.SYS_PROTO
                            self.logger.error(f"Read response length error: {ret}")
                    elif ret_byte[0] >= ErrType.DEV_ERR_BASE.value:
                        ret = ret_byte
                        self.logger.debug(f"Negative response 0x{ret_byte.hex()}: ")
                        if ret_byte[0] == ErrType.DEV_FULL.value:
                            time.sleep(self.setting.request_retry_interval_second)
                    else:
                        ret = ErrType.SYS_PROTO
                        self.logger.debug(f"Unexpected response opcode {ret_byte.hex()}")
                else:
                    if ret_byte[0] == ACK_BUF_FULL:
                        self.logger.debug(f"ACK: Rx buffer full, wait {self.setting.request_retry_interval_second}s")
                        time.sleep(self.setting.request_retry_interval_second)
                        ret = ErrType.OK
                    elif ret_byte[0] == ACK_BUF_EMPTY:
                        self.logger.debug(f"Response: ACK")
                        ret = ErrType.OK
                        break
                    elif ret_byte[0] >= ErrType.DEV_ERR_BASE.value:
                        ret = ret_byte
                        self.logger.debug(f"Negative response: {ret_byte}")
                        if ret_byte[0] == ErrType.DEV_FULL.value or ErrType.DEV_BUSY.value:
                            time.sleep(self.setting.request_retry_interval_second)
                    else:
                        ret = ErrType.SYS_PROTO
                        self.logger.debug(f"Unexpected response {ret_byte}")

                if ret == ErrType.OK or ret == ErrType.SYS_CANCEL:
                    break
                else:
                    time.sleep(self.setting.request_retry_interval_second)
        except Exception as err:
            self.logger.debug(f"Response exception: {err}")
            ret = ErrType.SYS_IO

        return ret, response_bytes

    def sense(self, timeout, op_code=None, data=None):
        self.logger.debug(f"Sense...")
        ret, sense_ack = self.send_request(SENSE.to_bytes(1, byteorder="little"), length=1, timeout=timeout)
        if ret == ErrType.OK:
            sense_status = SenseStatus()
            self.logger.debug(f"Sense response raw data: {sense_ack.hex()}")
            if sense_ack[0] == (SENSE):
                ret = sense_status.parse(sense_ack, 1)
                if ret == ErrType.OK:
                    self.logger.debug(
                        f"Sense response: opcode={hex(sense_status.op_code)}, status=0x{format(sense_status.status, '02x')}, data={hex(sense_status.data)}")
                    if sense_status.status != ErrType.OK.value:
                        ret = sense_status.status
                        self.logger.warning(
                            f"Sense fail: opcode={hex(sense_status.op_code)}, data={sense_status.data}, status={ret}")
                    elif (op_code is not None) and (sense_status.op_code != op_code):
                        ret = ErrType.SYS_PROTO
                        self.logger.error(
                            f"Sense protocol error: expect opcode-data {op_code.hex()}-{hex(data)}, get {hex(sense_status.op_code)}-{hex(sense_status.data)}")
                    else:
                        if (data is not None) and sense_status.data != data:
                            self.logger.debug(
                                f"Sense protocol warning: opcode {op_code} expect data {data}, get {sense_status.data}, ignored")
                        self.logger.debug("Sense ok")
                else:
                    self.logger.debug(f"Sense fail to parse sense response")
            else:
                self.logger.debug(f"Sense fail: unexpected opcode {sense_ack[0]}")
        else:
            self.logger.debug(f"Sense fail: {ret}")
        return ret, sense_ack

    def handshake(self, baudrate):
        ret = ErrType.SYS_UNKNOWN

        self.logger.debug(f"Floader handshake start")
        try:
            if self.serial_port.baudrate != baudrate and self.setting.switch_baudrate_at_floader == 1:
                baudset = [BAUDSET]

                baudset.extend(list(baudrate.to_bytes(4, byteorder="little")))

                self.logger.debug(f"BAUDSET {baudrate}")

                baudset_bytes = bytearray(baudset)
                ret, _ = self.send_request(baudset_bytes, len(baudset_bytes), self.setting.async_response_timeout_in_second,
                                           is_sync=False)
                if ret != ErrType.OK:
                    self.logger.debug(f"BAUDSET fail: {ret}")
                    return ret
                ret = self.ameba.switch_baudrate(baudrate, self.setting.baudrate_switch_delay_in_second, True)
                if ret != ErrType.OK:
                    self.logger.error(f"Fail to switch baudrate to {baudrate}: {ret}")
                    return ret

            for retry in range(3):
                if retry > 0:
                    self.logger.debug(f"Sense retry {retry}#")
                else:
                    self.logger.debug("Sense")

                ret, resp = self.sense(self.setting.sync_response_timeout_in_second)
                if ret == ErrType.OK:
                    break
                else:
                    self.logger.debug(f"Sense failed: {ret}")
                    time.sleep(self.setting.request_retry_interval_second)
                    continue

            if ret == ErrType.OK:
                self.logger.debug(f"Floader handshake done")
            else:
                self.logger.error(f"Floader handshake timeout")
        except Exception as err:
            self.logger.error(f"Floader handshake exception: {err}")

        return ret

    def query(self):
        device_info = DeviceInfo()
        self.logger.debug(f"QUERY...")
        ret, resp = self.send_request(QUERY.to_bytes(1, byteorder="little"), length=1,
                                      timeout=self.setting.sync_response_timeout_in_second)
        if ret == ErrType.OK:
            if resp[0] == (QUERY):
                device_info.did = resp[QUERY_DATA_OFFSET_DID + 1] + (resp[QUERY_DATA_OFFSET_DID + 2] << 8)
                device_info.image_type = resp[QUERY_DATA_OFFSET_IMAGE_TYPE + 1] + (
                            resp[QUERY_DATA_OFFSET_IMAGE_TYPE + 2] << 8)
                device_info.cmd_set_version = resp[QUERY_DATA_OFFSET_CMD_SET_VERSION + 1] + (
                            resp[QUERY_DATA_OFFSET_CMD_SET_VERSION + 2] << 8)
                device_info.wifi_mac = resp[QUERY_DATA_OFFSET_WIFI_MAC + 1: QUERY_DATA_OFFSET_WIFI_MAC + 1 + 6]
                device_info.memory_type = resp[QUERY_DATA_OFFSET_MEMORY_TYPE + 1]
                device_info.flash_mid = resp[QUERY_DATA_OFFSET_FLASH_MID + 1]
                device_info.flash_did = resp[QUERY_DATA_OFFSET_FLASH_DID + 1] + (
                            resp[QUERY_DATA_OFFSET_FLASH_DID + 2] << 8)
                device_info.flash_mfg = resp[
                                        QUERY_DATA_OFFSET_FLASH_MFG + 1: QUERY_DATA_OFFSET_FLASH_MFG + 1 + 12].decode("utf-8")
                device_info.flash_model = resp[
                                          QUERY_DATA_OFFSET_FLASH_MODEL + 1: QUERY_DATA_OFFSET_FLASH_MODEL + 1 + 20].decode("utf-8")
                device_info.flash_page_size = (resp[QUERY_DATA_OFFSET_FLASH_PAGE_SIZE + 1]
                                               + (resp[QUERY_DATA_OFFSET_FLASH_PAGE_SIZE + 2] << 8)
                                               + (resp[QUERY_DATA_OFFSET_FLASH_PAGE_SIZE + 3] << 16)
                                               + (resp[QUERY_DATA_OFFSET_FLASH_PAGE_SIZE + 4] << 24))
                device_info.flash_oob_size = (resp[QUERY_DATA_OFFSET_FLASH_OOB_SIZE + 1] + (
                            resp[QUERY_DATA_OFFSET_FLASH_OOB_SIZE + 2] << 8))
                device_info.flash_pages_per_block = (resp[QUERY_DATA_OFFSET_FLASH_PAGES_PER_BLOCK + 1]
                                                     + (resp[QUERY_DATA_OFFSET_FLASH_PAGES_PER_BLOCK + 2] << 8)
                                                     + (resp[QUERY_DATA_OFFSET_FLASH_PAGES_PER_BLOCK + 3] << 16)
                                                     + (resp[QUERY_DATA_OFFSET_FLASH_PAGES_PER_BLOCK + 4] << 24))
                device_info.flash_blocks_per_lun = (resp[QUERY_DATA_OFFSET_FLASH_BLOCKS_PER_LUN + 1]
                                                    + (resp[QUERY_DATA_OFFSET_FLASH_BLOCKS_PER_LUN + 2] << 8)
                                                    + (resp[QUERY_DATA_OFFSET_FLASH_BLOCKS_PER_LUN + 3] << 16)
                                                    + (resp[QUERY_DATA_OFFSET_FLASH_BLOCKS_PER_LUN + 4] << 24))
                device_info.flash_luns_per_target = resp[QUERY_DATA_OFFSET_FLASH_LUNS_PER_TARGET + 1]
                device_info.flash_max_bad_block_per_lun = (resp[QUERY_DATA_OFFSET_FLASH_MAX_BAD_BLOCKS_PER_LUN + 1]
                                                           + (resp[
                                                                  QUERY_DATA_OFFSET_FLASH_MAX_BAD_BLOCKS_PER_LUN + 2] << 8))
                device_info.flash_req_host_ecc_level = resp[QUERY_DATA_OFFSET_FLASH_REQ_HOST_ECC_LEVEL + 1]
                device_info.flash_targets = resp[QUERY_DATA_OFFSET_FLASH_TARGETS + 1]
                device_info.flash_capacity = (resp[QUERY_DATA_OFFSET_FLASH_CAPACITY + 1]
                                              + (resp[QUERY_DATA_OFFSET_FLASH_CAPACITY + 2] << 8)
                                              + (resp[QUERY_DATA_OFFSET_FLASH_CAPACITY + 3] << 16)
                                              + (resp[QUERY_DATA_OFFSET_FLASH_CAPACITY + 4] << 24))
            else:
                self.logger.debug(f"Query: unexpected response: {resp[0]}")
                ret = ErrType.SYS_PROTO

        return ret, device_info

    def config(self, configs):
        config_data = [(CONFIG)]

        config_data.append(configs[0][0])
        config_data.append(configs[0][1])
        config_data.append(configs[0][2])
        config_data.append(configs[0][3])
        config_data.append(configs[0][4])
        config_data.append(configs[0][5])
        config_data.append(configs[0][6])
        config_data.append(configs[0][7])

        config_data.append(configs[1][0])
        config_data.append(configs[1][1])
        config_data.append(configs[1][2])
        config_data.append(configs[1][3])
        config_data.append(configs[1][4])
        config_data.append(configs[1][5])
        config_data.append(configs[1][6])
        config_data.append(configs[1][7])

        request_bytes = bytearray(config_data)
        self.logger.debug(f"CONFIG: {configs[0].hex()} {configs[1].hex()}")
        ret, _ = self.send_request(request_bytes, len(request_bytes), self.setting.async_response_timeout_in_second, is_sync=False)

        return ret

    def next_operation(self, opecode, operand):
        request_data = [NEXTOP]

        request_data.append((opecode) & 0xFF)
        request_data.extend(list(operand.to_bytes(4, byteorder="little")))

        self.logger.debug(f"NEXTOTP: opecode={opecode}, operand={operand}")

        request_bytes = bytearray(request_data)

        return self.send_request(request_bytes, len(request_bytes), self.setting.sync_response_timeout_in_second)

    def reset_in_download_mode(self):
        self.logger.debug(f"Reset in download mode")
        return self.next_operation(NextOpType.REBURN, 0)

    def write(self, mem_type, src, size, addr, timeout, need_sense=False):
        sense_status = SenseStatus()

        write_data = [WRITE]
        write_data.append(mem_type&0xFF)
        write_data.extend(list(addr.to_bytes(4, byteorder="little")))

        write_array = bytearray(write_data)
        write_array += src[:size]

        self.logger.debug(f"WRITE: addr={hex(addr)}, size={size}, mem_type={mem_type}, need_sense={need_sense}")
        ret, _ = self.send_request(write_array, len(write_array), self.setting.write_response_timeout_in_second, is_sync=False)
        if ret == ErrType.OK:
            if need_sense:
                ret, sense_ack = self.sense(timeout, op_code=WRITE, data=addr)
                if ret != ErrType.OK:
                    self.logger.error(f"WRITE addr={hex(addr)} fail: {ret}")

        return ret

    def read(self, mem_type, addr, size, timeout):
        resp = None

        read_data = [READ]
        read_data.append((mem_type & 0xFF))
        read_data.extend(list(addr.to_bytes(4, byteorder="little")))
        read_data.extend(list(size.to_bytes(4, byteorder="little")))

        self.logger.debug(f"READ: addr={hex(addr)}, size={size}, mem_type={mem_type}")
        read_bytes = bytearray(read_data)
        ret, resp_ack = self.send_request(read_bytes, len(read_bytes), timeout)
        if ret == ErrType.OK:
            if resp_ack[0] == READ:
                resp = resp_ack[1:]
            else:
                self.logger.debug(f"READ got unexpected response {hex(resp_ack[0])}")
                ret = ErrType.SYS_PROTO
        else:
            self.logger.debug(f"READ fail: {ret}")

        return ret

    def checksum(self, mem_type,  start_addr, end_addr, size, timeout):
        chk_rest = 0
        request_data = [CHKSM]
        request_data.append((mem_type & 0xFF))
        request_data.extend(list(start_addr.to_bytes(4, byteorder='little')))

        request_data.extend(list(end_addr.to_bytes(4, byteorder='little')))

        request_data.extend(list(size.to_bytes(4, byteorder='little')))

        self.logger.debug(f"CHKSM: start={hex(start_addr)}, end={hex(end_addr)}, size={size}, mem_type={mem_type}")
        request_bytes = bytearray(request_data)
        ret, resp = self.send_request(request_bytes, len(request_bytes), timeout)
        if ret == ErrType.OK:
            if resp[0] == int(CHKSM):
                chk_rest = resp[1] + (resp[2] << 8) + (resp[3] << 16) + (resp[4] << 24)
                self.logger.debug(f"CHKSM: result={hex(chk_rest)}")
            else:
                self.logger.debug(f"CHKSM: unexpected response {resp[0]}")
                ret = ErrType.SYS_PROTO
        else:
            self.logger.error(f"CHKSM fail: {ret}")

        return ret, chk_rest

    def erase_flash(self, mem_type, start_addr, end_addr, size, timeout, sense=False, force=False):
        self.logger.debug(f"Erase flash: start_addr={hex(start_addr)}, end_addr={hex(end_addr)} size={size}")
        request_data = [FS_ERASE]
        request_data.append((mem_type & 0xFF))
        request_data.append(1 if force else 0)

        request_data.extend(list(start_addr.to_bytes(4, byteorder="little")))
        request_data.extend(list((end_addr & 0xFFFFFFFF).to_bytes(4, byteorder="little")))

        request_data.extend(list((size & 0xFFFFFFFF).to_bytes(4, byteorder="little")))

        if force:
            self.logger.warning(f"FS_ERASE: start_addr={hex(start_addr)}, end_addr={hex(end_addr)}, size={size}, mem_type={mem_type} force")
        else:
            self.logger.debug(f"FS_ERASE: start_addr={hex(start_addr)}, end_addr={hex(end_addr)}, size={size}, mem_type={mem_type}")

        request_bytes = bytearray(request_data)
        ret, _ = self.send_request(request_bytes, len(request_bytes), self.setting.async_response_timeout_in_second, is_sync=False)
        if ret != ErrType.OK:
            self.logger.warning(f"FS_ERASE start_addr={hex(start_addr)}, end_addr={hex(end_addr)}, size={size}, force={force}, fail:{ret}")
            return ret

        if sense:
            ret, sense_status = self.sense(timeout, op_code=FS_ERASE, data=start_addr)
            if ret != ErrType.OK:
                self.logger.error(f"FS_ERASE start_addr={hex(start_addr)}, size={size} force={force} fail: {ret}")

        return ret

    def read_status_register(self, cmd, address):
        status = None

        request_data = [FS_RDSTS]
        request_data.append(cmd)
        request_data.append(address)

        self.logger.debug(f"FS_RDSTS: cmd={cmd}, address={address}")

        request_bytes = bytearray(request_data)
        ret, resp = self.send_request(request_bytes, len(request_bytes), self.setting.sync_response_timeout_in_second)
        if ret == ErrType.OK:
            if resp[0] == FS_RDSTS:
                status = resp[1]
                self.logger.debug(f"FS_RDSTS: status={status}")
            else:
                self.logger.debug(f"FS_RDSTS: got unexpected response, {resp[0]}")
                ret = ErrType.SYS_PROTO
        else:
            self.logger.debug(f"FS_RDSTS failed: {ret}")

        return ret, status

    def write_status_register(self, cmd, addr, value):
        request_data = [FS_WTSTS]
        request_data.append(cmd)
        request_data.append(addr)
        request_data.append(value)

        self.logger.debug(f"FS_WTSTS: cmd={hex(cmd)}, addr={hex(addr)}, value={hex(value)}")

        request_bytes = bytearray(request_data)
        ret, _ = self.send_request(request_bytes, len(request_bytes), self.setting.async_response_timeout_in_second, is_sync=False)

        return ret

    def mark_bad_block(self, address):
        request_data = [FS_MKBAD]

        request_data.extend(list(address.to_bytes(4, byteorder="little")))
        request_bytes = bytearray(request_data)
        self.logger.debug(f"FS_MKBAD: addr={format(address, '08x')}")
        return self.send_request(request_bytes, len(request_bytes), self.setting.async_response_timeout_in_second, is_sync=False)

    def check_bad_block(self, address):
        ret = ErrType.OK
        status = None

        request_data = [FS_CHKBAD]

        request_data.extend(list(address.to_bytes(4, byteorder="little")))
        request_bytes = bytearray(request_data)
        self.logger.debug(f"FS_CHKBAD: addr={format(address, '08x')}")

        ret, resp = self.send_request(request_bytes, len(request_bytes), self.setting.sync_response_timeout_in_second)
        if ret == ErrType.OK:
            if resp[0] == FS_CHKBAD:
                status = resp[1]
            else:
                self.logger.debug(f"FS_CHKBAD got unexpected response {hex(resp[0])}")
                ret = ErrType.SYS_PROTO
        else:
            self.logger.debug(f"FS_CHKBAD fail: {ret}")

        return ret, status

    def check_block_status(self, address):
        ret = ErrType.OK
        block_status = 0
        page_status = [0, 0]

        request_data = [FS_CHKBLK]

        request_data.extend(list(address.to_bytes(4, byteorder="little")))
        request_bytes = bytearray(request_data)
        self.logger.debug(f"FS_CHKBLK: addr={format(address, '08x')}")

        ret, resp = self.send_request(request_bytes, len(request_bytes), self.setting.sync_response_timeout_in_second)
        if ret == ErrType.OK:
            if resp[0] == FS_CHKBLK:
                block_status = resp[1]
                page_status[0] = ctypes.c_uint64(resp[2] + (resp[3] << 8) + (resp[4] << 16) +
                                                 (resp[5] << 24) + (resp[6] << 32) + (resp[7] << 40) +
                                                 (resp[8] << 48) + (resp[9] << 56)).value
                page_status[1] = ctypes.c_uint64(resp[10] + (resp[11] << 8) + (resp[12] << 16) +
                                                 (resp[13] << 24) + (resp[14] << 32) + (resp[15] << 40) +
                                                 (resp[16] << 48) + (resp[17] << 56)).value

                self.logger.debug(
                    f"FS_CHKBLK: block_status={hex(block_status)}, page status={format(page_status[0], '16x')} {format(page_status[1], '16x')}")
        else:
            self.logger.debug(f"FS_CHKBLK fail: {ret}")

        return ret, block_status

    def check_map_status(self, address):
        ret = ErrType.OK

        status = 0
        request_data = [FS_CHKMAP]
        request_data.extend(list(address.to_bytes(4, byteorder="little")))
        request_bytes = bytearray(request_data)
        self.logger.debug(f"FS_CHKMAP: addr={format(address, '08x')}")

        ret, resp = self.send_request(request_bytes, len(request_bytes), self.setting.sync_response_timeout_in_second)
        if ret == ErrType.OK:
            if resp[0] == FS_CHKMAP:
                status = (resp[1] + (resp[2] << 8) + (resp[3] << 16) + (resp[4] << 24)) & 0xFFFFFFFF
                self.logger.debug(f"FS_CHKMAP: status={format(status, '08x')}")
        else:
            self.logger.debug(f"FS_CHKMAP fail: {ret}")

        return ret, status

    def otp_read_map(self, cmd, address, size):
        request_data = [cmd]

        request_data.extend(list(address.to_bytes(4, byteorder="little")))

        request_data.extend(list(size.to_bytes(4, byteorder="little")))

        request_bytes = bytearray(request_data)

        ret, buf = self.send_request(request_bytes, len(request_bytes), OTP_READ_TIMEOUT_IN_SECONDS)
        if ret == ErrType.OK:
            if buf[0] == cmd:
                self.logger.debug(f"Otp read: {buf[0]}")
                return ret, buf[1:-1]
            else:
                self.logger.debug(f"Otp read fail: unexpected response {buf[0]}")
                ret = ErrType.SYS_PROTO

        return ret, buf

    def otp_write_map(self, cmd, address, size, data):
        request_data = [cmd]

        request_data.extend(list(address.to_bytes(4, byteorder="little")))

        request_data.extend(list(size.to_bytes(4, byteorder="little")))

        for idx in range(size):
            request_data.append(data[address + idx])

        request_bytes = bytearray(request_data)

        ret, _ = self.send_request(request_bytes, len(request_bytes), self.setting.async_response_timeout_in_second, is_sync=False)

        return ret

    def otp_read_physical_map(self, address, size):
        self.logger.debug(f"OTP_RRAW: addr={hex(address)}, size={size}")

        ret, buf = self.otp_read_map(OTP_RRAW, address, size)
        if ret == ErrType.OK:
            self.logger.debug(f"OTP_RRAW: {buf}")
            self.logger.debug("OTP_RRAW done")
        else:
            self.logger.debug(f"OTP_RRAW fail: {ret}")

        return ret, buf

    def otp_write_physical_map(self, address, size, data):
        self.logger.debug(f"OTP_WRAW: addr={hex(address)}, size={size}, data={data}")

        ret = self.otp_write_map(OTP_WMAP, address, size, data)
        if ret == ErrType.OK:
            self.logger.debug("OTP_WRAW done")
        else:
            self.logger.debug(f"OTP_WRAW fail: {ret}")

        return ret

    def otp_read_logical_map(self, address, size):
        self.logger.debug(f"OTP_RMAP: addr={hex(address)}, size={size}")

        ret, buf = self.otp_read_map(OTP_RMAP, address, size)
        if ret == ErrType.OK:
            self.logger.debug(f"OTP_RMAP: {buf}")
            self.logger.debug("OTP_RMAP done")
        else:
            self.logger.debug(f"OTP_RMAP fail: {ret}")

        return ret, buf

    def otp_write_logical_map(self, address, size, data):
        self.logger.debug(f"OTP_WMAP: addr={hex(address)}, size={size}, data={data}")

        ret = self.otp_write_map(OTP_WMAP, address, size, data)
        if ret == ErrType.OK:
            self.logger.debug("OTP_WMAP done")
        else:
            self.logger.debug(f"OTP_WMAP fail: {ret}")

        return ret
