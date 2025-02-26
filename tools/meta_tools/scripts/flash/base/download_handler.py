#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

from serial.tools.list_ports import comports
import serial
import serial.tools.list_ports
from datetime import datetime

from .rom_handler import *
from .floader_handler import *
from .flash_utils import *
from .device_profile import *
from .json_utils import *
from .rt_settings import *

_RTK_USB_VID = "0BDA"

CmdEsc = b'\x1b\r\n'
CmdSetBackupRegister = bytes("EW 0x480003C0 0x200\r\n", encoding="utf-8")
CmdResetIntoDownloadMode = bytes("reboot uartburn\r\n", encoding="utf-8")


class Ameba(object):
    def __init__(self,
                 profile: RtkDeviceProfile,
                 serial_port: serial.Serial,
                 baudrate: int,
                 image_path: str,
                 setting: RtSettings,
                 logger,
                 download_img_info=None,
                 chip_erase=False,
                 memory_type=None,
                 erase_info=None):
        self.logger = logger
        self.profile_info = profile
        self.serial_port = serial_port
        self.baudrate = baudrate
        self.image_path = image_path
        self.download_img_info = download_img_info
        self.chip_erase = chip_erase
        self.memory_type = memory_type
        self.setting = setting
        self.is_usb = self.is_realtek_usb()
        self.rom_handler = RomHandler(self)
        self.floader_handler = FloaderHandler(self)
        self.download_image_list = []
        self.device_info = None
        self.erase_info = erase_info

    def read_bytes(self, timeout_seconds, size=1):
        ret = ErrType.OK
        read_ch = None

        start_time = datetime.now()
        while self.serial_port.inWaiting() < size:
            if (datetime.now() - start_time).seconds >= timeout_seconds:
                return ErrType.DEV_TIMEOUT, None

        try:
            read_ch = self.serial_port.read(size=size)
        except Exception as err:
            self.logger.error(f"[{self.serial_port.port}] read bytes err: {err}")
            ret = ErrType.SYS_IO

        return ret, read_ch

    def write_bytes(self, data_bytes):
        self.serial_port.write(data_bytes)

    def write_string(self, string):
        bytes_array = string.encode("utf-8")
        self.serial_port.write(bytes_array)

    def is_realtek_usb(self):
        ports = serial.tools.list_ports.comports()
        for port, desc, hvid in sorted(ports):
            if port == self.serial_port.port:
                # hvid: USB VID:PID=0BDA:8722 SER=5 LOCATION=1-1
                if _RTK_USB_VID in hvid:
                    return True
        else:
            return False

    def switch_baudrate(self, baud, delay_s, force=False):
        ret = ErrType.OK

        if (baud == self.serial_port.baudrate) and (not force):
            self.logger.debug(f"Reactive port {self.serial_port.port} ignored, baudrate no change")
            return ret

        if baud != self.serial_port.baudrate:
            self.logger.debug(
                f"Reactive port {self.serial_port.port} with baudrate from {self.serial_port.baudrate} to {baud}")
        else:
            self.logger.debug(
                f"Reactive port {self.serial_port.port} with baudrate {baud}")

        # if uart dtr/rts enable, should skip close/reopen operation
        # if USB port, should close/reopen port when switch baudrate
        if self.is_usb:
            # check if already activated
            for retry in range(10):
                try:
                    if self.serial_port.is_open:
                        self.serial_port.close()

                    while self.serial_port.is_open:
                        pass
                except:
                    ret = ErrType.SYS_IO

                if ret == ErrType.OK:
                    break

                time.sleep(0.1)

            if ret != ErrType.OK:
                self.logger.warning(f"Failed to close {self.serial_port.port} when reactive it.")

            time.sleep(delay_s)

        if self.serial_port.baudrate != baud:
            self.serial_port.baudrate = baud

        if self.is_usb:
            ret = ErrType.OK
            for rty in range(10):
                try:
                    self.serial_port.open()
                except:
                    ret = ErrType.SYS_IO

                if ret == ErrType.OK:
                    break

                time.sleep(0.1)

        if ret == ErrType.OK:
            self.logger.debug(f"Reactive port {self.serial_port.port} ok")
        else:
            self.logger.debug(f"Reactive port {self.serial_port.port} fail")

        return ret

    def check_download_mode(self):
        ret = ErrType.SYS_IO
        is_floader = False
        boot_delay = self.setting.usb_rom_boot_delay_in_second if self.profile_info.support_usb_download else self.setting.rom_boot_delay_in_second

        self.logger.debug(f"Check download mode with baudrate {self.serial_port.baudrate}")
        retry = 0
        while retry < 3:
            retry += 1
            try:
                self.logger.debug(f"Check whether in rom download mode")
                ret = self.rom_handler.handshake()
                if ret == ErrType.OK:
                    self.logger.debug(f"Handshake ok, device in rom download mode")
                    break
                self.logger.debug(f"Check whether in floader with baudrate {self.baudrate}")

                if not self.is_usb:
                    self.switch_baudrate(self.baudrate, self.setting.baudrate_switch_delay_in_second, True)

                ret = self.floader_handler.sense()
                if ret == ErrType.OK:
                    # do not reset floader
                    is_floader = True
                    self.logger.debug("Floader handshake ok")
                    break
                else:
                    self.logger.debug(f"Floader handshake fail: {ret}")

                if not self.is_usb:
                    self.logger.debug(
                        f'Assume in application or ROM normal mode with baudrate {self.profile_info.log_baudrate}')
                    self.switch_baudrate(self.profile_info.log_baudrate, self.setting.baudrate_switch_delay_in_second)

                    self.logger.debug("Try to reset device...")

                    self.serial_port.flushOutput()

                    self.write_bytes(CmdEsc)
                    time.sleep(0.1)

                    if self.profile_info.is_amebad():
                        self.serial_port.flushOutput()
                        self.write_bytes(CmdSetBackupRegister)
                        time.sleep(0.1)

                    self.serial_port.flushOutput()
                    self.write_bytes(CmdResetIntoDownloadMode)

                    self.switch_baudrate(self.profile_info.handshake_baudrate, boot_delay, True)

                    self.logger.debug(
                        f'Check whether reset in ROM download mode with baudrate {self.profile_info.handshake_baudrate}')

                    ret = self.rom_handler.handshake()
                    if ret == ErrType.OK:
                        self.logger.debug("Handshake ok, device in ROM download mode")
                        break
                    else:
                        self.logger.debug("Handshake fail, cannot enter UART download mode")
                else:
                    ret = ErrType.SYS_IO
                    self.logger.error(f"Handshake fail, cannot enter USB download mode")
                    break
            except Exception as err:
                self.logger.error(f"Check download mode exception: {err}")

        return ret, is_floader

    def prepare(self):
        ret = ErrType.OK
        floader_init_baud = self.baudrate if self.is_usb else (self.profile_info.handshake_baudrate if
                                                               (
                                                                       self.setting.switch_baudrate_at_floader == 1) else self.baudrate)
        boot_delay = self.setting.usb_floader_boot_delay_in_second if self.profile_info.support_usb_download else self.setting.floader_boot_delay_in_second

        if (not self.is_usb) and (self.setting.auto_switch_to_download_mode_with_dtr_rts != 0):
            ret = self.auto_enter_download_mode()
            if ret != ErrType.OK:
                self.logger.error(f"Enter download mode by DTR/RTS fail: {ret}")
                return ret

        ret, is_floader = self.check_download_mode()
        if ret != ErrType.OK:
            self.logger.error(f"Enter download mode fail: {ret}")
            return ret

        if not is_floader:
            # download flashloader to RAM
            ret = self.rom_handler.download_floader()
            if ret != ErrType.OK:
                self.rom_handler.abort()
                self.logger.error(f"Flashloader download fail: {ret}")
                return ret

            ret = self.switch_baudrate(floader_init_baud, boot_delay, True)
            if ret != ErrType.OK:
                self.logger.error(f"Flashloader boot fail: {ret}")
                return ret

            ret = self.floader_handler.handshake(self.baudrate)
            if ret != ErrType.OK:
                self.logger.error(f"Flashloader handshake fail: {ret}")
                return ret

        ret, self.device_info = self.floader_handler.query()
        if ret != ErrType.OK:
            self.logger.error(f"Query fail: {ret}")
            return ret

        self.logger.info("Device info:")
        self.logger.info(f'* DID: {hex(self.device_info.did)}')
        self.logger.info(f'* ImageType: 0x{format(self.device_info.image_type, "04x")}')
        self.logger.info(
            f'* CmdSetVersion: {(self.device_info.cmd_set_version >> 8) & 0xFF}.{self.device_info.cmd_set_version & 0xFF}')
        if self.device_info.is_boot_from_nand():
            self.logger.info(f'* MemoryType: NAND')
            self.logger.info(f'* FlashMID: 0x{format(self.device_info.flash_mid, "02X")}')
            if self.device_info.flash_mid == FlashUtils.NandMfgMicron:
                self.logger.info(f'* FlashDID: 0x{format(self.device_info.flash_did, "04X")}')
            else:
                self.logger.info(f'* FlashDID: 0x{format(self.device_info.flash_did, "02X")}')
            self.logger.info(f'* FlashMFG: {self.device_info.flash_mfg}')
            self.logger.info(f'* FlashModel: {self.device_info.flash_model}')
            self.logger.info(
                f'* FlashCapacity: {self.device_info.flash_capacity // 1024 // 1024 // (1024 // 8)}Gb/{self.device_info.flash_capacity // 1024 // 1024}MB')
            self.logger.info(f'* FlashBlockSize: {self.device_info.flash_block_size() // 1024}KB')
            self.logger.info(f'* FlashPageSize: {self.device_info.flash_page_size}B')
            self.logger.info(f'* FlashOobSize: {self.device_info.flash_oob_size}B')
            self.logger.info(f'* FlashPagesPerBlock: {self.device_info.flash_pages_per_block}')
            self.logger.info(f'* FlashBlocksPerLun: {self.device_info.flash_blocks_per_lun}')
            self.logger.info(f'* FlashLunsPerTarget: {self.device_info.flash_luns_per_target}')
            self.logger.info(f'* FlashTargets: {self.device_info.flash_targets}')
            self.logger.info(f'* FlashMaxBadBlocksPerLun: {self.device_info.flash_max_bad_block_per_lun}')
            self.logger.info(f'* FlashReqHostEccLevel: {self.device_info.flash_req_host_ecc_level}')
        else:
            self.logger.info(f'* MemoryType: NOR')
            self.logger.info(f'* FlashMID: {hex(self.device_info.flash_mid)}')
            self.logger.info(f'* FlashDID: {hex(self.device_info.flash_did)}')
            self.logger.info(
                f'* FlashCapacity: {self.device_info.flash_capacity // 1024 // (1024 // 8)}Mb/{self.device_info.flash_capacity // 1024 // 1024}MB')
            self.logger.info(f'* FlashPageSize: {self.device_info.flash_page_size}B')

        self.logger.info(f'* WiFiMAC: {self.device_info.get_wifi_mac_text()}')

        if self.device_info.did != self.profile_info.device_id:
            self.logger.error("Device ID mismatch:")
            self.logger.error(f'* Device: {hex(self.device_info.did)}')
            self.logger.error(f'* Device Profile: {hex(self.profile_info.device_id)}')
            return ErrType.SYS_PARAMETER

        if self.device_info.memory_type != self.profile_info.memory_type:
            self.logger.error("Flash type mismatch:")
            self.logger.error(f'* Device: {self.device_info.memory_type}')
            self.logger.error(f'* Device Profile: {self.profile_info.memory_type}')
            return ErrType.SYS_PARAMETER

        if self.device_info.is_boot_from_nand():
            if self.device_info.flash_req_host_ecc_level > 0:
                self.logger.error(f"Unsupported NAND flash model without internal ECC")
                return ErrType.SYS_IO
            if self.device_info.flash_pages_per_block != FlashUtils.NandDefaultPagePerBlock.value:
                self.logger.error(
                    f"Unsupported NAND flash model with {self.device_info.flash_pages_per_block} pages per block")
                return ErrType.SYS_IO

        program_param1 = self.setting.program_config1.to_bytes(8, byteorder="little")
        program_param2 = self.setting.program_config2.to_bytes(8, byteorder="little")
        param = [program_param1, program_param2]
        ret = self.floader_handler.config(param)
        if ret != ErrType.OK:
            self.logger.error(f"Config device fail: {ret}")

        return ret

    # def check_and_process_flash_lock(self, is_erase):
    #     follow_up_action =

    def auto_enter_download_mode(self):
        ret = ErrType.OK
        if not self.serial_port.is_open:
            return ErrType.SYS_IO

        try:
            dtr = self.serial_port.dtr
            rts = self.serial_port.rts

            self.serial_port.dtr = False
            self.serial_port.rts = True

            time.sleep(0.2)

            self.serial_port.dtr = True
            self.serial_port.rts = False

            time.sleep(0.1)
            self.serial_port.dtr = False

            self.serial_port.dtr = dtr
            self.serial_port.rts = rts
        except Exception as err:
            self.logger.error("Fail to auto enter download mode: %s" % err)
            ret = ErrType.SYS_IO
        return ret

    def check_protocol(self):
        ret = True

        if (not self.is_usb) and self.setting.disable_nand_access_with_uart == 1:
            self.logger.warning(
                f"NAND access via UART is not allowed, please check the COM port and make sure the device profile matches the attached device!")
            ret = False

        return ret

    def check_protocol_for_download(self):
        ret = True
        has_nand = False

        for image_info in self.profile_info.images:
            if (image_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND):
                has_nand = True
                break

        if has_nand:
            ret = self.check_protocol()

        return ret

    def verify_images(self):
        ret = True
        if not self.download_img_info:
            for image_info in self.profile_info.images:
                if not image_info.mandatory:
                    continue
                image_name = image_info.image_name
                image_path = os.path.realpath(os.path.join(self.image_path, image_name))
                if not os.path.exists(image_path):
                    self.logger.error(f"{image_name} NOT exists: {image_path}")
                    ret = False

        return ret

    def post_verify_images(self):
        ret = ErrType.OK

        max_addr = self.profile_info.flash_start_address + self.device_info.flash_capacity

        return ret

    def check_protocol_for_erase(self):
        ret = True

        if (not self.is_usb) and (self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND) and (
                self.setting.disable_nand_access_with_uart == 1):
            ret = False
            self.logger.warning(
                f"NAND access via UART is not allowed, please check the COM port and make sure the device profile matches the attached device!")
        return ret

    def is_address_block_aligned(self, address):
        ret = False

        if self.device_info:
            block_size = self.device_info.flash_block_size()
            ret = ((address % block_size) == 0)

        return ret

    def post_validate_config_for_erase(self):
        ret = ErrType.OK

        if self.erase_info and (not self.profile_info.is_ram_address(self.erase_info.start_address)):
            if not self.is_address_block_aligned(self.erase_info.start_address):
                ret = ErrType.SYS_PARAMETER
                self.logger.warning(
                    f"Flash start address should be aligned to block size {self.device_info.flash_block_size}KB")
            if self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND and (
                    not self.is_address_block_aligned(self.erase_info.end_address)):
                ret = ErrType.SYS_PARAMETER
                self.logger.warning(
                    f"Flash end address should be aligned to block size {self.device_info.flash_block_size}KB")
            if self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NOR and (
                    not self.is_address_block_aligned(self.erase_info.size_in_byte())):
                ret = ErrType.SYS_PARAMETER
                self.logger.warning(
                    f"Flash size should be aligned to block size {self.device_info.flash_block_size}KB")
        return ret

    def calculate_checksum(self, image):
        with open(image, 'rb') as stream:
            img_content = stream.read()
        img_len = len(img_content)
        img_arr = list(img_content)
        chksum = 0
        offset = 0
        while (img_len - offset) > 3:
            chksum += (img_arr[offset + 0] | (img_arr[offset + 1] << 8) | (img_arr[offset + 2] << 16) | (
                    img_arr[offset + 3] << 24))
            offset += 4

        tmp = 0
        while (img_len - offset - tmp) > 0:
            chksum += img_arr[offset + tmp] << (8 * tmp)
            tmp += 1

        chksum = chksum & 0xffffffff
        return chksum

    def download_images(self):
        ret = ErrType.OK

        if not self.verify_images():
            ret = ErrType.SYS_PARAMETER
            return ret

        # support chip erase
        if self.chip_erase and (self.memory_type == MemoryInfo.MEMORY_TYPE_NOR):
            self.logger.info(f"Chip erase start")
            ret = self.floader_handler.erase_flash(self.memory_type, RtkDeviceProfile.DEFAULT_FLASH_START_ADDR,
                                                   0, 0xFFFFFFFF, sense=True, force=False)
            if ret != ErrType.OK:
                self.logger.error(f"Chip erase fail")
                return ret
            self.logger.info(f"Chip erase end")

        if self.download_img_info:
            for image_info in self.download_img_info:
                img_path = image_info.image_name
                img_name = os.path.basename(img_path)
                image_info.image_name = img_name

                self.logger.info(f"{img_name} download...")
                ret = self._download_image(img_path, image_info)
                if ret != ErrType.OK:
                    self.logger.info(f"{img_name} download fail: {ret}")
                    break
        else:
            for image_info in self.profile_info.images:
                is_mandatory = image_info.mandatory
                if not is_mandatory:
                    continue
                img_name = image_info.image_name
                img_path = os.path.realpath(os.path.join(self.image_path, img_name))
                self.logger.info(f"{img_name} download...")

                ret = self._download_image(img_path, image_info)
                if ret != ErrType.OK:
                    self.logger.info(f"{img_name} download fail: {ret}")
                    break

        if ret == ErrType.OK:
            self.logger.info("All images download done")

        return ret

    def get_page_alligned_size(self, size, page_size):
        result = size

        if (size % page_size) != 0:
            result = (size // page_size + 1) * page_size

        return result

    def _download_image(self, image_path, image_info):
        ret = ErrType.OK

        page_size = self.device_info.flash_page_size
        block_size = self.device_info.flash_block_size()
        pages_per_block = self.device_info.flash_pages_per_block
        last_erase_addr = 0
        next_erase_addr = 0
        checksum = 0
        is_ram = (image_info.memory_type == MemoryInfo.MEMORY_TYPE_RAM)
        padding_data = self.setting.ram_download_padding_byte if is_ram else FlashUtils.FlashWritePaddingData.value
        start_time = datetime.now()

        with open(image_path, 'rb') as stream:
            img_content = stream.read()

        img_length = len(img_content)
        aligned_img_length = self.get_page_alligned_size(img_length, page_size)
        self.logger.debug(
            f"Image download size={aligned_img_length}({img_length}), start_addr={hex(image_info.start_address)}, "
            f"end_addr={hex(image_info.end_address)}")

        addr = image_info.start_address
        tx_sum = 0
        if ((image_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND) or (
                is_ram and (self.profile_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND))):
            img = io.BytesIO(img_content)
            img_bytes = img.read(img_length)
            data_bytes = img_bytes.ljust(aligned_img_length, padding_data.to_bytes(1, byteorder="little"))

            is_last_page = False
            while not is_last_page:
                if addr >= image_info.end_address:
                    self.logger.debug(f"Overrange target={hex(addr)}, end={hex(image_info.end_address)}")
                    ret = ErrType.SYS_OVERRANGE
                    break

                ret = self.floader_handler.erase_flash(image_info.memory_type, addr, addr + block_size, block_size,
                                                       sense=True)
                if ret == ErrType.DEV_NAND_BAD_BLOCK or ret == ErrType.DEV_NAND_WORN_BLOCK:
                    self.logger.debug(
                        f"{'Bad' if ret == ErrType.DEV_NAND_BAD_BLOCK else 'Worn'} block: 0x{format(addr, '08X')}")
                    addr += self.device_info.flash_block_size()
                    next_erase_addr = addr
                    continue
                elif ret != ErrType.OK:
                    break

                next_erase_addr = addr + self.device_info.flash_block_size()

                i = 0
                progress_int = 0
                while i < pages_per_block:
                    if tx_sum + page_size >= aligned_img_length:
                        is_last_page = True

                    need_sense = (is_last_page or (i == pages_per_block - 1))
                    ret = self.floader_handler.write(image_info.memory_type, data_bytes[tx_sum: tx_sum + page_size],
                                                     page_size, addr, need_sense)
                    if ret == ErrType.OK:
                        idx = 0
                        while idx < page_size:
                            checksum += (data_bytes[tx_sum + idx] + (data_bytes[tx_sum + idx + 1] << 8) + (
                                    data_bytes[tx_sum + idx + 2] << 16) + (data_bytes[tx_sum + idx + 3] << 24))
                            idx += 4
                        checksum &= 0xFFFFFFFF
                        addr += page_size
                        tx_sum += page_size
                    else:
                        self.logger.error(f"Write to addr={format(addr, '08x')}, size={page_size} fail: {ret}")
                        break

                    if is_last_page:
                        break

                    i += 1
                progress = int((tx_sum / aligned_img_length) * 100)
                if int((progress) / 10) != progress_int:
                    progress_int += 1
                    self.logger.info(f"Progress: {progress}%")

                if ret != ErrType.OK:
                    break

            if ret == ErrType.OK:
                if image_info.full_erase and (next_erase_addr < image_info.end_address):
                    self.logger.debug(
                        f"Erase extra address range: {hex(next_erase_addr)}-{hex(image_info.end_address)}")
                    ret = self.floader_handler.erase_flash(image_info.memory_type, next_erase_addr,
                                                           image_info.end_address,
                                                           (image_info.end_address - next_erase_addr), sense=True)
                    if ret == ErrType.DEV_NAND_BAD_BLOCK or ret == ErrType.DEV_NAND_WORN_BLOCK:
                        self.logger.debug(
                            f"{'Bad' if ret == ErrType.DEV_NAND_BAD_BLOCK else 'Worn'} block: {hex(addr)}")
                    elif ret != ErrType.OK:
                        self.logger.error(f"Fail to erase block {hex(addr)}:{ret}")

                if tx_sum >= aligned_img_length:
                    if aligned_img_length < 1024:
                        self.logger.debug(f"Image download done: {aligned_img_length}bytes")
                    elif aligned_img_length < 1024 * 1024:
                        self.logger.debug(f"Image download done: {aligned_img_length // 1024}KB")
                    else:
                        self.logger.debug(f"Image download done: {round(aligned_img_length / 1024 / 1024, 2)}MB")
                else:
                    self.logger.warning(f"Image download uncompleted: {tx_sum}/{aligned_img_length}")

                elapse_ms = round((datetime.now() - start_time).total_seconds() * 1000, 0)
                kbps = aligned_img_length * 8 // elapse_ms
                size_kb = aligned_img_length // 1024

                if self.is_usb:
                    self.logger.info(
                        f"{image_info.image_name} download done: {size_kb}KB / {elapse_ms}ms / {kbps / 1000}Mbps")
                else:
                    self.logger.info(f"{image_info.image_name} download done: {size_kb}KB / {elapse_ms}ms / {kbps}Kbps")
        else:
            write_pages = 0
            img = io.BytesIO(img_content)
            img_bytes = img.read(img_length)
            data_bytes = img_bytes.ljust(aligned_img_length, padding_data.to_bytes(1, byteorder="little"))
            progress_int = 0
            while tx_sum < aligned_img_length:
                if write_pages == 0:
                    if (addr % (64 * FlashUtils.NorDefaultPageSize.value)) == 0 and \
                            ((aligned_img_length - tx_sum >= 64 * FlashUtils.NorDefaultPageSize.value)):
                        block_size = 64 * FlashUtils.NorDefaultPageSize.value
                    else:
                        block_size = 4 * FlashUtils.NorDefaultPageSize.value

                    pages_per_block = block_size // page_size
                    erase_addr = addr

                    if erase_addr != last_erase_addr:
                        if (not is_ram) and (erase_addr % block_size) != 0:
                            self.logger.error(
                                f"Flash erase address align error: addr {hex(erase_addr)} not aligned to block size {hex(block_size)}")
                            ret = ErrType.SYS_PARAMETER
                            break

                        ret = self.floader_handler.erase_flash(image_info.memory_type, erase_addr,
                                                               erase_addr + block_size, block_size)
                        if ret != ErrType.OK:
                            break

                        last_erase_addr = erase_addr
                        next_erase_addr = erase_addr + block_size

                need_sense = ((((write_pages + 1) % self.setting.sense_packet_count) == 0) or
                              (write_pages + 1 >= pages_per_block) or
                              (tx_sum + page_size >= aligned_img_length))
                ret = self.floader_handler.write(image_info.memory_type, data_bytes[tx_sum: tx_sum + page_size],
                                                 page_size, addr,
                                                 need_sense=need_sense)
                if ret != ErrType.OK:
                    self.logger.debug(f"Write to addr={hex(addr)} size={page_size} fail: {ret}")
                    break

                write_pages += 1
                if write_pages >= pages_per_block:
                    write_pages = 0

                idx = 0
                while idx < page_size:
                    checksum += (data_bytes[tx_sum + idx] + (data_bytes[tx_sum + idx + 1] << 8) + (
                            data_bytes[tx_sum + idx + 2] << 16) + (data_bytes[tx_sum + idx + 3] << 24))
                    idx += 4

                checksum &= 0xFFFFFFFF

                addr += page_size
                tx_sum += page_size

                progress = int((tx_sum / aligned_img_length) * 100)
                if int((progress) / 10) != progress_int:
                    progress_int += 1
                    self.logger.info(f"Progress: {progress}%")

            if image_info.full_erase and (next_erase_addr < image_info.end_address):
                self.logger.debug(f"Erase extra address range: {hex(next_erase_addr)}-{hex(image_info.end_address)}")
                ret = self.floader_handler.erase_flash(image_info.memory_type, next_erase_addr, image_info.end_address,
                                                       (image_info.end_address - next_erase_addr), sense=True)
                if ret != ErrType.OK:
                    self.logger.warning(
                        f"Fail to extra address range {hex(next_erase_addr)}-{hex(image_info.end_address)}")

            if aligned_img_length < 1024:
                self.logger.debug(f"Image download done: {aligned_img_length}bytes")
            elif aligned_img_length < 1024 * 1024:
                self.logger.debug(f"Image download done: {aligned_img_length // 1024}KB")
            else:
                self.logger.debug(f"Image download done: {round(aligned_img_length / 1024 / 1024, 2)}MB")

            elapse_ms = round((datetime.now() - start_time).total_seconds() * 1000, 0)
            kbps = aligned_img_length * 8 // elapse_ms
            size_kb = aligned_img_length // 1024

            if self.is_usb:
                self.logger.info(
                    f"{image_info.image_name} download done: {size_kb}KB / {elapse_ms}ms / {kbps / 1000}Mbps")
            else:
                self.logger.info(f"{image_info.image_name} download done: {size_kb}KB / {elapse_ms}ms / {kbps}Kbps")

        if ret == ErrType.OK:
            cal_checksum = 0
            ret, cal_checksum = self.floader_handler.checksum(image_info.memory_type, image_info.start_address,
                                                              image_info.end_address, aligned_img_length)
            if ret == ErrType.OK:
                if cal_checksum != checksum:
                    self.logger.debug(f"Checksum fail: expect {hex(checksum)} get {hex(cal_checksum)}")
                    ret = ErrType.SYS_CHECKSUM

        return ret

    def erase_flash(self):
        ret = ErrType.OK

        if self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND:
            erase_total_size = self.erase_info.end_address - self.erase_info.start_address
            erase_size = 0
            addr = self.erase_info.start_address
            while addr < self.erase_info.end_address:
                ret = self.floader_handler.erase_flash(self.erase_info.memory_type,
                                                       addr,
                                                       addr + self.device_info.flash_block_size,
                                                       self.device_info.flash_block_size,
                                                       sense=True)
                if ret == ErrType.OK:
                    erase_size += self.device_info.flash_block_size
                    # TODO：update erase progress
                    # self.logger.info(f"Erase progress: {int()}")
                    self.logger.info(
                        f"NAND erase address  ={hex(addr)}, size = {self.device_info.flash_block_size / 1024}KB OK")
                elif ret == ErrType.DEV_NAND_BAD_BLOCK:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size / 1024}KB skipped: bad block")
                    ret = ErrType.OK
                elif ret == ErrType.DEV_NAND_WORN_BLOCK:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size / 1024}KB failed: mark warning block")
                    ret = ErrType.OK
                else:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size / 1024}KB failed: {ret}")
                    break
            if ret == ErrType.OK:
                self.logger.info(f"Erase nand done")
        elif self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NOR:
            if self.setting.erase_by_block != 0:
                addr = self.erase_info.start_address
                size_erased = 0
                while size_erased < self.erase_info.size_in_byte():
                    if ((addr % (64 * FlashUtils.NorDefaultPageSize.value)) == 0) and \
                            ((self.erase_info.size_in_byte() - size_erased) >= 64 * FlashUtils.NorDefaultPageSize.value):
                        block_size = 64 * FlashUtils.NorDefaultPageSize.value
                    elif ((addr % (32 * FlashUtils.NorDefaultPageSize.value)) == 0) and \
                            ((self.erase_info.size_in_byte() - size_erased) >= 32 * FlashUtils.NorDefaultPageSize.value):
                        block_size = 32 * FlashUtils.NorDefaultPageSize.value
                    else:
                        block_size = 4 * FlashUtils.NorDefaultPageSize.value

                    need_sense = ((size_erased + block_size) >= self.erase_info.size_in_byte())
                    ret = self.floader_handler.erase_flash(self.erase_info.memory_type, addr, addr + block_size,
                                                           block_size, sense=need_sense)
                    if ret != ErrType.OK:
                        break

                    addr += block_size
                    size_erased += block_size
                    # TODO：update erase progress
                    # self.logger.info(f"Erase Progress: {}")
            else:
                ret = self.floader_handler.erase_flash(self.erase_info.memory_type,
                                                       self.erase_info.start_address,
                                                       self.erase_info.end_address,
                                                       self.erase_info.size_in_byte(),
                                                       sense=True)
                if ret == ErrType.OK:
                    self.logger.info(f"Erase nor done")
        elif self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_RAM:
            ret = self.floader_handler.erase_flash(self.erase_info.memory_type,
                                                   self.erase_info.start_address,
                                                   self.erase_info.end_address,
                                                   self.erase_info.size_in_byte(),
                                                   sense=True)
            if ret == ErrType.OK:
                self.logger.info("Erase ram done")
        else:
            # TBD
            pass

        return ret
