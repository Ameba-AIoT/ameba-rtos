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
from .spic_addr_mode import *
from .memory_info import *

_RTK_USB_VID = "0BDA"

CmdEsc = b'\x1b\r\n'
CmdSetBackupRegister = bytes("EW 0x480003C0 0x200\r\n", encoding="utf-8")
CmdResetIntoDownloadMode = bytes("reboot uartburn\r\n", encoding="utf-8")

OtpSpicAddrModeAddr = 0x02
OtpSpicAddrModeMask = 0x02
OtpSpicAddrModePos = 1
OtpSpicAddrMode4Byte = (1 << OtpSpicAddrModePos)
OtpSpicAddrMode3Byte = (0 << OtpSpicAddrModePos)

OtpSpicAddrModeAddrForAmebaD = 0x0E
OtpSpicAddrModeMaskForAmebaD = 0x40
OtpSpicAddrModePosForAmebaD = 6
OtpSpicAddrMode4ByteForAmebaD = (1 << OtpSpicAddrModePosForAmebaD)
OtpSpicAddrMode3ByteForAmebaD = (0 << OtpSpicAddrModePosForAmebaD)


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
        self.device_info = None
        self.erase_info = erase_info
        self.is_all_ram = True
        if self.is_usb:
            if self.serial_port.is_open:
                self.serial_port.close()
                while self.serial_port.is_open:
                    pass
            self.serial_port = serial.Serial(self.serial_port.port,
                                             baudrate=self.profile_info.handshake_baudrate,
                                             parity=serial.PARITY_NONE,
                                             stopbits=serial.STOPBITS_ONE,
                                             bytesize=serial.EIGHTBITS)
        self.rom_handler = RomHandler(self)
        self.floader_handler = FloaderHandler(self)

    def __del__(self):
        if self.serial_port:
            if self.serial_port.is_open:
                self.serial_port.close()
            while self.serial_port.is_open:
                pass
            if not self.serial_port.is_open:
                self.logger.info(f"{self.serial_port.port} closed.")

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

                    self.switch_baudrate(self.baudrate, self.setting.baudrate_switch_delay_in_second, True)

                self.logger.debug(f"Check whether in floader with baudrate {self.baudrate}")
                ret, status = self.floader_handler.sense(self.setting.sync_response_timeout_in_second)
                if ret == ErrType.OK:
                    # do not reset floader
                    is_floader = True
                    self.logger.debug("Floader handshake ok")
                    break
                else:
                    self.logger.debug(f"Floader handshake fail: {ret}")
            except Exception as err:
                self.logger.error(f"Check download mode exception: {err}")

        return ret, is_floader

    def prepare(self):
        ret = ErrType.OK
        floader_init_baud = self.baudrate if self.is_usb else (self.profile_info.handshake_baudrate if
                                                               (self.setting.switch_baudrate_at_floader == 1) else self.baudrate)
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
            self.logger.info(f'* FlashMID: 0x{format(self.device_info.flash_mid, "02X")}')  # customized, do not modify
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

        if (self.device_info.did != self.profile_info.device_id) and (self.device_info.did != 0xFFFF):
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

    def check_flash_lock(self, flash_status):
        ret = ErrType.OK

        if self.device_info.is_boot_from_nand():
            cmd = FlashUtils.NandCmdGetFeatures.value
            address = FlashUtils.NandRegProtection.value
            bp_mask = FlashUtils.NandRegProtectionBpMask.value
            if self.device_info.flash_mid == FlashUtils.NandMfgWinbond or self.device_info.flash_mid == FlashUtils.NandMfgMicron:
                bp_mask = FlashUtils.NandRegProtectionBpMaskWinbondMicron
        else:
            cmd = FlashUtils.NorCmdReadStatusReg1.value
            address = 0
            bp_mask = FlashUtils.NorStatusReg1BpMask.value

        ret, protection = self.read_flash_status_register(cmd, address)
        flash_status.protection = protection
        if ret == ErrType.OK:
            if (protection & bp_mask) != 0:
                flash_status.is_locked = True

        return ret

    def check_and_process_flash_lock(self, flash_status):
        follow_up_action = self.setting.flash_protection_process

        ret = self.check_flash_lock(flash_status)
        if ret != ErrType.OK:
            self.logger.error(f"Flash protection check fail: {ret}")

        if flash_status.is_locked:
            self.logger.warning("Flash block protection detected")

            if follow_up_action == RtSettings.FLASH_PROTECTION_PROCESS_PROMPT:
                self.logger.info(f"Follow-up Actions:")
                self.logger.info(f"1: Try operation with block protected(may fail)")
                self.logger.info(f"2: Remove the protection and restore the protection after operation")
                self.logger.info(f"3: Abort the operation")
                retry = 0
                while retry < 3:
                    try:
                        follow_up_action = int(input("Please Input the selected action index: ").strip())
                        if RtSettings.FLASH_PROTECTION_PROCESS_PROMPT < follow_up_action <= RtSettings.FLASH_PROTECTION_PROCESS_ABORT:
                            break
                        else:
                            self.logger.info(f"{follow_up_action} is invalid")
                    except Exception as err:
                        self.logger.error(f"Input is invalid: {err}")
                        continue
                else:
                    return ErrType.SYS_PARAMETER

            if follow_up_action == RtSettings.FLASH_PROTECTION_PROCESS_UNLOCK:
                flash_status.need_unlock = True
                self.logger.info("Remove the flash block protection...")
                ret = self.unlock_flash()
                if ret != ErrType.OK:
                    self.logger.error(f"Fail to remove the flash lock protection: {ret}")
                    return ret
            elif follow_up_action == RtSettings.FLASH_PROTECTION_PROCESS_ABORT:
                self.logger.warning(f"Operation aborted for block protection")
                return ErrType.SYS_CANCEL
            else:
                self.logger.warning(f"Trying to operate with block protection")
                return ErrType.SYS_PARAMETER

        return ret

    def unlock_flash(self):
        return self.lock_flash(0)

    def lock_flash(self, protection):
        if self.device_info.is_boot_from_nand():
            cmd = FlashUtils.NandCmdSetFeatures.value
            address = FlashUtils.NandRegProtection.value
        else:
            cmd = FlashUtils.NorCmdWriteStatusReg1.value
            address = 0

        ret = self.write_flash_status_register(cmd, address, protection)

        return ret

    def read_flash_status_register(self, cmd, address):
        return self.floader_handler.read_status_register(cmd, address)

    def write_flash_status_register(self, cmd, address, status):
        return self.floader_handler.write_status_register(cmd, address, status)

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

    def _process_image(self, img_name):
        if img_name.strip().startswith(("A:", "B:")):
            img_name = img_name.split(":")[1].split("(")[0].strip()
        if img_name.endswith(".dtb"):
            img_path_files = os.listdir(self.image_path)
            for img_f in img_path_files:
                self.logger.debug(img_f)
                if img_f.endswith(".dtb") and os.path.isfile(os.path.join(self.image_path, img_f)):
                    img_name = img_f
                    break
            else:
                img_name = None

        return img_name

    def verify_images(self):
        ret = ErrType.OK
        image_selected = False

        all_images = self.profile_info.images
        if self.download_img_info:
            all_images = self.download_img_info

        for image_info in all_images:
            if not image_info.mandatory:
                continue
            if not self.download_img_info:
                image_name = self._process_image(image_info.image_name)
                if image_name is None:
                    self.logger.error(f"Cannot find a valid {image_name} for download")
                    ret = ErrType.SYS_PARAMETER
                    break
                image_path = os.path.realpath(os.path.join(self.image_path, image_name))
            else:
                image_name = os.path.basename(image_info.image_name)
                image_path = image_info.image_name

            if not os.path.exists(image_path):
                self.logger.error(f"Image file {image_name} dose not exist: {image_path}")
                ret = ErrType.SYS_PARAMETER
                break

            if image_info.start_address < 0:
                self.logger.error(f"Start address is not valid specified for image {image_name}")
                ret = ErrType.SYS_PARAMETER
                break
            if image_info.end_address < 0:
                self.logger.error(f"End address is not valid specified for image {image_name}")
                ret = ErrType.SYS_PARAMETER
                break
            if image_info.start_address >= image_info.end_address:
                self.logger.error(
                    f"Invalid address range {image_info.start_address}-{image_info.end_address} for {image_name}")
                ret = ErrType.SYS_PARAMETER
                break
            image_size = os.path.getsize(image_path)
            if image_size > (image_info.end_address - image_info.start_address):
                self.logger.error(
                    f"Image file {image_path} is too large for {image_name}, please adjust the memory layout")
                ret = ErrType.SYS_PARAMETER
                break

            is_start_address_in_ram = self.profile_info.is_ram_address(image_info.start_address)
            is_end_address_in_ram = self.profile_info.is_ram_address(image_info.end_address)
            if (((self.memory_type == MemoryInfo.MEMORY_TYPE_RAM) and (
                    (not is_start_address_in_ram) or (not is_end_address_in_ram))) or
                    ((self.memory_type == MemoryInfo.MEMORY_TYPE_RAM) and (
                            is_start_address_in_ram or is_end_address_in_ram))):
                self.logger.error(
                    f"Invalid address range {image_info.start_address}-{image_info.end_address} for {image_name}")
                ret = ErrType.SYS_PARAMETER
                break
            if not is_start_address_in_ram:
                self.is_all_ram = False

            image_selected = True
        else:
            if not image_selected:
                self.logger.warning(f"No image selected!")
                ret = ErrType.SYS_PARAMETER

        return ret

    def post_verify_images(self):
        ret = ErrType.OK

        max_addr = self.profile_info.flash_start_address + self.device_info.flash_capacity
        image_dowload_list = self.download_img_info if self.download_img_info else self.profile_info.images
        for image_info in image_dowload_list:
            is_ram = self.profile_info.is_ram_address(image_info.start_address)
            if not is_ram:
                if (image_info.start_address > max_addr) or (image_info.end_address > max_addr):
                    self.logger.error(f"Invalid layout, image {image_info.image_name} address overflow")
                    ret = ErrType.SYS_OVERRANGE
                    break
                if (self.device_info.is_boot_from_nand() and
                        (((image_info.start_address % self.device_info.flash_block_size()) != 0) or
                         ((image_info.end_address % self.device_info.flash_block_size()) != 0))):
                    self.logger.error(f"{image_info.image_name} address range not aligned")
                    ret = ErrType.SYS_PARAMETER
                    break
        return ret

    def validate_config_for_erase(self):
        ret = ErrType.OK

        if self.erase_info:
            if ((self.profile_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND) and (
                    self.memory_type == MemoryInfo.MEMORY_TYPE_NOR)) or \
                    ((self.profile_info.memory_type == MemoryInfo.MEMORY_TYPE_NOR) and (
                            self.memory_type == MemoryInfo.MEMORY_TYPE_NAND)):
                self.logger.error("Unsupported memory type.")
                ret = ErrType.SYS_PARAMETER
                return ret

            if (self.memory_type == MemoryInfo.MEMORY_TYPE_RAM) and (not self.profile_info.is_ram_address(self.erase_info.start_address)):
                self.logger.error(f"Invalid RAM start address: {self.erase_info.start_address}")
                ret = ErrType.SYS_PARAMETER
                return ret
            elif (self.memory_type != MemoryInfo.MEMORY_TYPE_RAM) and \
                    (not ((self.profile_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND) or self.profile_info.is_flash_address(self.erase_info.start_address))):
                self.logger.error(f"Invalid start address: {self.erase_info.start_address}")
                ret = ErrType.SYS_PARAMETER
                return ret

            if (self.memory_type == MemoryInfo.MEMORY_TYPE_NOR):
                if not self.is_address_block_aligned(self.erase_info.start_address):
                    self.logger.warning(f"NOR flash start address should be aligned to {FlashUtils.NorDefaultBlockSize.value}B.")
                    ret = ErrType.SYS_PARAMETER
                    return ret
                if (self.erase_info.size_in_kbyte != 0xFFFFFFFF) and (not self.is_address_block_aligned(self.erase_info.size_in_byte())):
                    self.logger.warning(f"NOR flash erase size should be aligned to {FlashUtils.NorDefaultBlockSize.value}B.")
                    ret = ErrType.SYS_PARAMETER
                    return ret
                self.logger.info(f"NOR flash erase: start address={hex(self.erase_info.start_address)}, size={self.erase_info.size_in_kbyte}KB.")
            else:
                if not self.is_address_block_aligned(self.erase_info.start_address):
                    self.logger.warning(f"NAND flash start address should be block size aligned!")
                    ret = ErrType.SYS_PARAMETER
                    return ret
                if not self.is_address_block_aligned(self.erase_info.end_address):
                    self.logger.warning(f"NAND flash end address should be block size aligned!")
                    ret = ErrType.SYS_PARAMETER
                    return ret
                if (self.erase_info.end_address <= self.erase_info.start_address):
                    self.logger.warning(f"NAND flash start address should be less than end address!")
                    ret = ErrType.SYS_PARAMETER
                    return ret

                self.logger.info(f"NAND flash erase: start address={hex(self.erase_info.start_address)}, end address={hex(self.erase_info.end_address)}")

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
                    f"Flash start address should be aligned to block size {self.device_info.flash_block_size()}KB")
            if self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND and (
                    not self.is_address_block_aligned(self.erase_info.end_address)):
                ret = ErrType.SYS_PARAMETER
                self.logger.warning(
                    f"Flash end address should be aligned to block size {self.device_info.flash_block_size()}KB")
            if self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NOR and (
                    not self.is_address_block_aligned(self.erase_info.size_in_byte())):
                ret = ErrType.SYS_PARAMETER
                self.logger.warning(
                    f"Flash size should be aligned to block size {self.device_info.flash_block_size()}KB")
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

    def erase_flash_chip(self):
        self.logger.info(f"Chip erase start")  # customized, do not modify
        ret = self.floader_handler.erase_flash(MemoryInfo.MEMORY_TYPE_NOR, RtkDeviceProfile.DEFAULT_FLASH_START_ADDR,
                                               0, 0xFFFFFFFF,
                                               nor_erase_timeout_in_second(0xFFFFFFFF),
                                               sense=True, force=False)
        return ret

    def download_images(self):
        ret = ErrType.OK

        # support chip erase
        if self.chip_erase and (self.memory_type == MemoryInfo.MEMORY_TYPE_NOR):
            ret = self.erase_flash_chip()
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
            is_area_A = False
            is_area_B = False
            for image_info in self.profile_info.images:
                is_mandatory = image_info.mandatory
                if not is_mandatory:
                    continue
                img_name = image_info.image_name
                self.logger.info(f"{img_name} download...")
                if img_name.strip().startswith(("A:", "a_")):
                    is_area_A = True
                elif img_name.strip().startswith(("B:", "b_")):
                    is_area_B = True

                if is_area_A and is_area_B:
                    self.logger.error(f"NOT support both A and B download at the same time")
                    ret = ErrType.SYS_PARAMETER
                    break

                img_name = self._process_image(img_name)
                img_path = os.path.realpath(os.path.join(self.image_path, img_name))
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
        write_timeout = 0
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
            write_timeout = nand_program_timeout_in_second(block_size,
                                                           page_size) + FlashUtils.NandBlockEraseTimeoutInSeconds.value
            img = io.BytesIO(img_content)
            img_bytes = img.read(img_length)
            data_bytes = img_bytes.ljust(aligned_img_length, padding_data.to_bytes(1, byteorder="little"))

            is_last_page = False
            progress_int = 0
            while not is_last_page:
                if addr >= image_info.end_address:
                    self.logger.debug(f"Overrange target={hex(addr)}, end={hex(image_info.end_address)}")
                    ret = ErrType.SYS_OVERRANGE
                    break

                ret = self.floader_handler.erase_flash(image_info.memory_type, addr, addr + block_size, block_size,
                                                       nand_erase_timeout_in_second(block_size, block_size),
                                                       sense=True)
                if ret == ErrType.DEV_NAND_BAD_BLOCK.value or ret == ErrType.DEV_NAND_WORN_BLOCK.value:
                    self.logger.info(
                        f"{'Bad' if ret == ErrType.DEV_NAND_BAD_BLOCK else 'Worn'} block: 0x{format(addr, '08X')}")
                    addr += self.device_info.flash_block_size()
                    next_erase_addr = addr
                    continue
                elif ret != ErrType.OK:
                    break

                next_erase_addr = addr + self.device_info.flash_block_size()

                i = 0
                while i < pages_per_block:
                    if tx_sum + page_size >= aligned_img_length:
                        is_last_page = True

                    need_sense = (is_last_page or (i == pages_per_block - 1))
                    ret = self.floader_handler.write(image_info.memory_type, data_bytes[tx_sum: tx_sum + page_size],
                                                     page_size, addr, write_timeout, need_sense=need_sense)
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
                    self.logger.info(f"Programming progress: {progress}")  # customized, do not modified

                if ret != ErrType.OK:
                    break

            if ret == ErrType.OK:
                if image_info.full_erase and (next_erase_addr < image_info.end_address):
                    self.logger.debug(
                        f"Erase extra address range: {hex(next_erase_addr)}-{hex(image_info.end_address)}")
                    ret = self.floader_handler.erase_flash(image_info.memory_type, next_erase_addr,
                                                           image_info.end_address,
                                                           (image_info.end_address - next_erase_addr),
                                                           nand_erase_timeout_in_second(
                                                               (image_info.end_address - next_erase_addr), block_size),
                                                           sense=True)
                    if ret == ErrType.DEV_NAND_BAD_BLOCK.value or ret == ErrType.DEV_NAND_WORN_BLOCK.value:
                        self.logger.debug(
                            f"{'Bad' if ret == ErrType.DEV_NAND_BAD_BLOCK else 'Worn'} block: {hex(addr)}")
                        ret = ErrType.OK
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
                    write_timeout = FlashUtils.NorPageProgramTimeoutInSeconds.value * int(
                        max(self.setting.sense_packet_count, pages_per_block)) + nor_erase_timeout_in_second(
                        divide_then_round_up(block_size, 1024))
                    if erase_addr != last_erase_addr:
                        if (not is_ram) and (erase_addr % block_size) != 0:
                            # error: # customized, do not modify
                            self.logger.error(
                                f"Flash erase address align error: addr {hex(erase_addr)} not aligned to block size {hex(block_size)}")
                            ret = ErrType.SYS_PARAMETER
                            break

                        ret = self.floader_handler.erase_flash(image_info.memory_type, erase_addr,
                                                               erase_addr + block_size, block_size,
                                                               nor_erase_timeout_in_second(
                                                                   divide_then_round_up(block_size, 1024)))
                        if ret != ErrType.OK:
                            break

                        last_erase_addr = erase_addr
                        next_erase_addr = erase_addr + block_size

                need_sense = ((((write_pages + 1) % self.setting.sense_packet_count) == 0) or
                              (write_pages + 1 >= pages_per_block) or
                              (tx_sum + page_size >= aligned_img_length))
                ret = self.floader_handler.write(image_info.memory_type, data_bytes[tx_sum: tx_sum + page_size],
                                                 page_size, addr, write_timeout, need_sense=need_sense)
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
                    self.logger.info(f"Programming progress: {progress}")  # customized, do not modified

            if image_info.full_erase and (next_erase_addr < image_info.end_address):
                self.logger.debug(f"Erase extra address range: {hex(next_erase_addr)}-{hex(image_info.end_address)}")
                ret = self.floader_handler.erase_flash(image_info.memory_type, next_erase_addr, image_info.end_address,
                                                       (image_info.end_address - next_erase_addr),
                                                       nor_erase_timeout_in_second(divide_then_round_up(
                                                           (image_info.end_address - next_erase_addr), 1024)),
                                                       sense=True)
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
            if image_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND:
                checksum_timeout = nand_checksum_timeout_in_second(aligned_img_length, None)
            else:
                checksum_timeout = nor_checksum_timeout_in_second(aligned_img_length)
            ret, cal_checksum = self.floader_handler.checksum(image_info.memory_type, image_info.start_address,
                                                              image_info.end_address, aligned_img_length,
                                                              checksum_timeout)
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
                                                       addr + self.device_info.flash_block_size(),
                                                       self.device_info.flash_block_size(),
                                                       nand_erase_timeout_in_second(self.device_info.flash_block_size(),
                                                                                    self.device_info.flash_block_size()),
                                                       sense=True)
                if ret == ErrType.OK:
                    erase_size += self.device_info.flash_block_size()
                    # TODO：update erase progress
                    # self.logger.info(f"Erase progress: {int()}")
                    self.logger.debug(
                        f"NAND erase address  ={hex(addr)}, size = {self.device_info.flash_block_size() / 1024}KB OK")
                elif ret == ErrType.DEV_NAND_BAD_BLOCK:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size() / 1024}KB skipped: bad block")
                    ret = ErrType.OK
                elif ret == ErrType.DEV_NAND_WORN_BLOCK:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size() / 1024}KB failed: mark warning block")
                    ret = ErrType.OK
                else:
                    self.logger.warning(
                        f"NAND erase address = {hex(addr)} size = {self.device_info.flash_block_size() / 1024}KB failed: {ret}")
                    break
                addr += self.device_info.flash_block_size()

            if ret == ErrType.OK:
                self.logger.info(f"Erase nand done")
        elif self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_NOR:
            if self.setting.erase_by_block != 0:
                addr = self.erase_info.start_address
                size_erased = 0
                while size_erased < self.erase_info.size_in_byte():
                    if ((addr % (64 * FlashUtils.NorDefaultPageSize.value)) == 0) and \
                            ((
                                     self.erase_info.size_in_byte() - size_erased) >= 64 * FlashUtils.NorDefaultPageSize.value):
                        block_size = 64 * FlashUtils.NorDefaultPageSize.value
                    elif ((addr % (32 * FlashUtils.NorDefaultPageSize.value)) == 0) and \
                            ((
                                     self.erase_info.size_in_byte() - size_erased) >= 32 * FlashUtils.NorDefaultPageSize.value):
                        block_size = 32 * FlashUtils.NorDefaultPageSize.value
                    else:
                        block_size = 4 * FlashUtils.NorDefaultPageSize.value

                    need_sense = ((size_erased + block_size) >= self.erase_info.size_in_byte())
                    ret = self.floader_handler.erase_flash(self.erase_info.memory_type, addr, addr + block_size,
                                                           block_size,
                                                           nor_erase_timeout_in_second(
                                                               divide_then_round_up(block_size, 1024)),
                                                           sense=need_sense)
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
                                                       self.erase_info.size_in_kbyte,
                                                       nor_erase_timeout_in_second(
                                                           divide_then_round_up(self.erase_info.size_in_byte(), 1024)),
                                                       sense=True)
                if ret == ErrType.OK:
                    self.logger.info(f"Erase nor done")
        elif self.erase_info.memory_type == MemoryInfo.MEMORY_TYPE_RAM:
            # memset 16MB ram, cost time: 0.01s
            ret = self.floader_handler.erase_flash(self.erase_info.memory_type,
                                                   self.erase_info.start_address,
                                                   self.erase_info.end_address,
                                                   self.erase_info.size_in_byte(),
                                                   self.setting.sync_response_timeout_in_second,
                                                   sense=True)
            if ret == ErrType.OK:
                self.logger.info("Erase ram done")
        else:
            # TBD
            pass

        return ret

    def set_spic_address_mode(self, mode):
        ret = ErrType.OK
        is_amebad = self.profile_info.is_amebad()
        otp_spic_addr_mode_addr = OtpSpicAddrModeAddrForAmebaD if is_amebad else OtpSpicAddrModeAddr
        otp_spic_addr_mode_mask = OtpSpicAddrModeMask if is_amebad else OtpSpicAddrModeMask
        otp_spic_addr_mode_pos = OtpSpicAddrModePosForAmebaD if is_amebad else OtpSpicAddrModePos

        if self.device_info.is_boot_from_nand():
            return ret

        ret, buf = self.floader_handler.otp_read_logical_map(0, self.profile_info.logical_efuse_len)
        if ret != ErrType.OK:
            self.logger.error(f"Fail to read eFuse: {ret}")
            return

        cfg = buf[otp_spic_addr_mode_addr]
        if cfg != 0xFF:
            if ((cfg & otp_spic_addr_mode_mask) >> otp_spic_addr_mode_pos) == mode:
                self.logger.info(f"No need to change supported flash size")
                return ErrType.OK

            cfg &= (~otp_spic_addr_mode_mask)
            cfg |= ((mode << otp_spic_addr_mode_pos) & otp_spic_addr_mode_mask)
        else:
            if mode == SpicAddrMode.FOUR_BYTE_MODE.value:
                cfg = OtpSpicAddrMode4Byte
            else:
                self.logger.info(f"No need to change default supported flash size")
                return ErrType.OK

        # buf[otp_spic_addr_mode_addr] = cfg
        buf_array = bytearray(buf)
        buf_array[otp_spic_addr_mode_addr] = cfg
        buf = bytes(buf_array)
        self.logger.info(f"Program eFuse to change supported flash size {'>' if (mode == SpicAddrMode.FOUR_BYTE_MODE.value) else '<='}16MB")

        ret = self.floader_handler.otp_write_logical_map(otp_spic_addr_mode_addr, 1, buf)
        if ret != ErrType.OK:
            self.logger.error(f"Fail to program eFuse[{otp_spic_addr_mode_addr}]: {ret}")
            return ret
        time.sleep(0.01)

        ret, buf = self.floader_handler.otp_read_logical_map(0, self.profile_info.logical_efuse_len)
        if ret != ErrType.OK:
            self.logger.error(f"Fail to read eFuse: {ret}")
            return ret

        cfg = buf[otp_spic_addr_mode_addr]
        if ((cfg & otp_spic_addr_mode_mask) >> otp_spic_addr_mode_pos) == mode:
            self.logger.info(f"Program eFuse done")
            return ErrType.OK
        else:
            self.logger.error(f"Fail to verify eFuse[{otp_spic_addr_mode_addr}]")
            self.logger.error(f"Fail to program eFuse")
            return ErrType.SYS_CHECKSUM

    def get_spic_address_mode(self):
        ret = ErrType.OK
        is_amebad = self.profile_info.is_amebad()
        otp_spic_addr_mode_addr = OtpSpicAddrModeAddrForAmebaD if is_amebad else OtpSpicAddrModeAddr
        otp_spic_addr_mode_mask = OtpSpicAddrModeMask if is_amebad else OtpSpicAddrModeMask
        otp_spic_addr_mode_4byte = OtpSpicAddrMode4ByteForAmebaD if is_amebad else OtpSpicAddrMode4Byte
        mode = SpicAddrMode.THREE_BYTE_MODE.value

        if self.device_info.is_boot_from_nand():
            return ret, mode

        ret, buf = self.floader_handler.otp_read_logical_map(0, self.profile_info.logical_efuse_len)
        if ret != ErrType.OK:
            self.logger.error(f"Fail to read eFuse: {ret}")
            return ret, mode

        cfg = buf[otp_spic_addr_mode_addr]
        if cfg != 0xFF:
            if (cfg & otp_spic_addr_mode_mask) == otp_spic_addr_mode_4byte:
                mode = SpicAddrMode.FOUR_BYTE_MODE.value
                self.logger.info(f"Current supported flash size >16MB")
            else:
                self.logger.info(f"Current supported flash size <=16MB")
        else:
            self.logger.info(f"Current supported flash size <=16MB as default")

        return ret, mode

    def check_supported_flash_size(self):
        ret = ErrType.OK

        if self.device_info.memory_type == MemoryInfo.MEMORY_TYPE_NAND:
            return ret

        ret, mode = self.get_spic_address_mode()
        if ret != ErrType.OK:
            return ret

        flash_size = self.device_info.flash_capacity // 1024 // 1024  # MB
        if self.setting.auto_program_spic_addr_mode_4byte == 0:
            if (mode == SpicAddrMode.THREE_BYTE_MODE.value) and (flash_size > 16):
                self.logger.warning(f"Flash size: {flash_size}MB, OTP supported flash size <= 16MB")
                ret = ErrType.SYS_CANCEL
            if (mode == SpicAddrMode.FOUR_BYTE_MODE.value) and (flash_size <= 16):
                self.logger.warning(f"Flash size: {flash_size}MB, OTP supported flash size >16MB")
                ret = ErrType.SYS_CANCEL
        else:
            if mode == SpicAddrMode.THREE_BYTE_MODE.value and flash_size > 16:
                self.logger.warning(f"OTP supports flash size <=16MB, tool will program OTP to support flash size >16MB")
                ret = self.set_spic_address_mode(SpicAddrMode.FOUR_BYTE_MODE.value)
            elif mode == SpicAddrMode.FOUR_BYTE_MODE.value and flash_size <= 16:
                self.logger.warning(f"OTP supports flash size >16MB, tool will program OTP to support flash size <=16MB")
                ret = self.set_spic_address_mode(SpicAddrMode.THREE_BYTE_MODE.value)

        return ret
