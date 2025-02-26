#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


class RtSettings():
    MIN_ROM_BOOT_DELAY_IN_MILLISECOND = 50
    MIN_USB_ROM_BOOT_DELAY_IN_MILLISECOND = 200
    MIN_USB_FLOADER_BOOT_DELAY_IN_MILLISECOND = 200

    def __init__(self, **kwargs):
        self.sense_packet_count = kwargs.get("SensePacketCount", 32)
        self.request_retry_count = kwargs.get("RequestRetryCount", 3)
        self.request_retry_interval_second = round(kwargs.get("RequestRetryIntervalInMillisecond", 10) / 1000, 2)
        self.async_response_timeout_in_second = round(kwargs.get("AsyncResponseTimeoutInMilliseccond", 1000) / 1000, 2)
        self.sync_response_timeout_in_second = round(kwargs.get("SyncResponseTimeoutInMillisecond", 10000) / 1000, 2)
        self.baudrate_switch_delay_in_second = round(kwargs.get("BaudrateSwitchDelayInMillisecond", 200) / 1000, 2)
        self.rom_boot_delay_in_second = round(max(kwargs.get("RomBootDelayInMillisecond", 100), self.MIN_ROM_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.usb_rom_boot_delay_in_second = round(max(kwargs.get("UsbRomBootDelayInMillisecond", 500), self.MIN_USB_ROM_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.usb_floader_boot_delay_in_second = round(max(kwargs.get("UsbFloaderBootDelayInMillisecond", 500), self.MIN_USB_FLOADER_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.switch_baudrate_at_floader = kwargs.get("SwitchBaudrateAtFloader", 0)
        self.write_response_timeout_in_second = round(kwargs.get("WriteResponseTimeoutInMillisecond", 2000) / 1000, 2)
        self.floader_boot_delay_in_second = round(kwargs.get("FloaderBootDelayInMillisecond", 1000) / 1000, 2)
        self.auto_switch_to_download_mode_with_dtr_rts = kwargs.get("AutoSwitchToDownloadModeWithDtrRts", 0)
        self.erase_by_block = kwargs.get("EraseByBlock", 0)
        self.program_config1 = kwargs.get("ProgramConfig1", 0)
        self.program_config2 = kwargs.get("ProgramConfig2", 0)
        self.disable_nand_access_with_uart = kwargs.get("DisableNandAccessWithUart", 0)
        self.ram_download_padding_byte = kwargs.get("RamDownloadPaddingByte", 0x00)

    def __repr__(self):
        profile_dict = {
            "SensePacketCount": self.sense_packet_count,
            "RequestRetryCount": self.request_retry_count,
            "RequestRetryIntervalInMillisecond": int(self.request_retry_interval_second * 1000),
            "AsyncResponseTimeoutInMilliseccond": int(self.async_response_timeout_in_second * 1000),
            "SyncResponseTimeoutInMillisecond": int(self.sync_response_timeout_in_second * 1000),
            "BaudrateSwitchDelayInMillisecond": int(self.baudrate_switch_delay_in_second * 1000),
            "RomBootDelayInMillisecond": int(self.rom_boot_delay_in_second * 1000),
            "UsbRomBootDelayInMillisecond": int(self.usb_rom_boot_delay_in_second * 1000),
            "UsbFloaderBootDelayInMillisecond": int(self.usb_floader_boot_delay_in_second * 1000),
            "SwitchBaudrateAtFloader": self.switch_baudrate_at_floader,
            "WriteResponseTimeoutInMillisecond": int(self.write_response_timeout_in_second * 1000),
            "FloaderBootDelayInMillisecond": int(self.floader_boot_delay_in_second * 1000),
            "AutoSwitchToDownloadModeWithDtrRts": self.auto_switch_to_download_mode_with_dtr_rts,
            "EraseByBlock": self.erase_by_block,
            "ProgramConfig1": self.program_config1,
            "ProgramConfig2": self.program_config2,
            "DisableNandAccessWithUart": self.disable_nand_access_with_uart,
            "RamDownloadPaddingByte": self.ram_download_padding_byte
        }

        return profile_dict