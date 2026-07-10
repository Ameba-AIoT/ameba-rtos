#! /usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0


class RtSettings():
    MIN_ROM_BOOT_DELAY_IN_MILLISECOND = 50
    MIN_USB_ROM_BOOT_DELAY_IN_MILLISECOND = 200
    MIN_USB_FLOADER_BOOT_DELAY_IN_MILLISECOND = 200

    FLASH_PROTECTION_PROCESS_PROMPT = 0
    FLASH_PROTECTION_PROCESS_TRY = 1
    FLASH_PROTECTION_PROCESS_UNLOCK = 2
    FLASH_PROTECTION_PROCESS_ABORT = 3

    def __init__(self, **kwargs):
        # Settings.json is split into two sections:
        #   "General"     – RT / download-engine parameters (shared by CLI and GUI)
        #   "GUIRunTime" – GUI runtime selections (profile, port, image, etc.)
        # For backward compatibility with the old flat layout, any key missing
        # from its section falls back to the top-level kwargs.
        general = kwargs.get("General") or {}
        gui = kwargs.get("GUIRunTime") or {}

        def _c(key, default):
            return general[key] if key in general else kwargs.get(key, default)

        def _g(key, default):
            return gui[key] if key in gui else kwargs.get(key, default)

        # ── General: RT / download-engine fields ────────────────────────────
        self.sense_packet_count = _c("SensePacketCount", 32)
        self.request_retry_count = _c("RequestRetryCount", 3)
        self.request_retry_interval_second = round(_c("RequestRetryIntervalInMillisecond", 10) / 1000, 2)
        self.async_response_timeout_in_second = round(_c("AsyncResponseTimeoutInMillisecond", 1000) / 1000, 2)
        self.sync_response_timeout_in_second = round(_c("SyncResponseTimeoutInMillisecond", 1000) / 1000, 2)
        self.baudrate_switch_delay_in_second = round(_c("BaudrateSwitchDelayInMillisecond", 200) / 1000, 2)
        self.rom_boot_delay_in_second = round(max(_c("RomBootDelayInMillisecond", 100), self.MIN_ROM_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.usb_rom_boot_delay_in_second = round(max(_c("UsbRomBootDelayInMillisecond", 1000), self.MIN_USB_ROM_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.usb_floader_boot_delay_in_second = round(max(_c("UsbFloaderBootDelayInMillisecond", 1000), self.MIN_USB_FLOADER_BOOT_DELAY_IN_MILLISECOND) / 1000, 2)
        self.switch_baudrate_at_floader = _c("SwitchBaudrateAtFloader", 0)
        self.write_response_timeout_in_second = round(_c("WriteResponseTimeoutInMillisecond", 2000) / 1000, 2)
        self.floader_boot_delay_in_second = round(_c("FloaderBootDelayInMillisecond", 1000) / 1000, 2)
        self.auto_switch_to_download_mode_with_dtr_rts = _c("AutoSwitchToDownloadModeWithDtrRts", 1)
        self.auto_reset_device_with_dtr_rts = _c("AutoResetDeviceWithDtrRts", 1)
        self.auto_switch_to_download_mode_with_dtr_rts_first = _c("AutoSwitchToDownloadModeWithDtrRtsFirst", 0)
        self.erase_by_block = _c("EraseByBlock", 0)
        self.program_config1 = _c("ProgramConfig1", 0)
        self.program_config2 = _c("ProgramConfig2", 0)
        self.disable_nand_access_with_uart = _c("DisableNandAccessWithUart", 0)
        self.ram_download_padding_byte = _c("RamDownloadPaddingByte", 0x00)
        self.auto_program_spic_addr_mode_4byte = _c("AutoProgramSpicAddrMode4Byte", 1)
        self.auto_switch_to_download_mode_with_dtr_rts_file = _c("AutoSwitchToDownloadModeWithDtrRtsTimingFile", "Reburn.cfg")
        self.auto_reset_device_with_dtr_rts_file = _c("AutoResetDeviceWithDtrRtsTimingFile", "Reset.cfg")
        self.serial_initial_read_timeout_in_second = round(_c("SerialInitialReadTimeoutInMillisecond", 20) / 1000, 2)
        self.rom_check_alive_timeout_in_second = round(_c("RomCheckAliveTimeoutMilliSeconds", 1000) / 1000, 2)

        # PostProcess is shared: CLI writes "NONE"/"RESET", GUI uses "NONE"/"INDICATION"/"RESET"/"BOOT"
        self.post_process = _c("PostProcess", "RESET")

        self.flash_protection_process = int(_c("FlashProtectionProcess", self.FLASH_PROTECTION_PROCESS_UNLOCK))

        # ── GUIRunTime: download target / GUI runtime fields ───────────────
        self.profile = _g("Profile", "")
        self.port = _g("Port", [])
        self.baudrate = _g("Baudrate", 1500000)
        self.image = _g("Image", "")
        self.image_dir = _g("ImageDir", "")
        self.address = _g("Address", 0x08000000)
        self.length = _g("Length", 0)
        self.factory_mode = _g("FactoryMode", False)
        self.chip_erase = _g("ChipErase", False)
        self.usb_download = _g("UsbDownload", False)
        self.save_log = _g("SaveLog", True)
        self.log_level = _g("LogLevel", "info")
        self.gui_language_zh = _g("GuiLanguageZh", False)
        # Number of port rows the GUI sizes its startup height to show without scrolling.
        self.gui_visible_ports = int(_g("GuiVisiblePorts", 20))

    def to_dict(self):
        return {
            # ── General: RT / download-engine fields ────────────────────────
            "General": {
                "SensePacketCount": self.sense_packet_count,
                "RequestRetryCount": self.request_retry_count,
                "RequestRetryIntervalInMillisecond": int(self.request_retry_interval_second * 1000),
                "AsyncResponseTimeoutInMillisecond": int(self.async_response_timeout_in_second * 1000),
                "SyncResponseTimeoutInMillisecond": int(self.sync_response_timeout_in_second * 1000),
                "BaudrateSwitchDelayInMillisecond": int(self.baudrate_switch_delay_in_second * 1000),
                "RomBootDelayInMillisecond": int(self.rom_boot_delay_in_second * 1000),
                "UsbRomBootDelayInMillisecond": int(self.usb_rom_boot_delay_in_second * 1000),
                "UsbFloaderBootDelayInMillisecond": int(self.usb_floader_boot_delay_in_second * 1000),
                "SerialInitialReadTimeoutInMillisecond": int(self.serial_initial_read_timeout_in_second * 1000),
                "SwitchBaudrateAtFloader": self.switch_baudrate_at_floader,
                "WriteResponseTimeoutInMillisecond": int(self.write_response_timeout_in_second * 1000),
                "FloaderBootDelayInMillisecond": int(self.floader_boot_delay_in_second * 1000),
                "RomCheckAliveTimeoutMilliSeconds": int(self.rom_check_alive_timeout_in_second * 1000),
                "AutoSwitchToDownloadModeWithDtrRts": self.auto_switch_to_download_mode_with_dtr_rts,
                "AutoSwitchToDownloadModeWithDtrRtsFirst": self.auto_switch_to_download_mode_with_dtr_rts_first,
                "AutoResetDeviceWithDtrRts": self.auto_reset_device_with_dtr_rts,
                "EraseByBlock": self.erase_by_block,
                "ProgramConfig1": self.program_config1,
                "ProgramConfig2": self.program_config2,
                "DisableNandAccessWithUart": self.disable_nand_access_with_uart,
                "RamDownloadPaddingByte": self.ram_download_padding_byte,
                "AutoProgramSpicAddrMode4Byte": self.auto_program_spic_addr_mode_4byte,
                "AutoSwitchToDownloadModeWithDtrRtsTimingFile": self.auto_switch_to_download_mode_with_dtr_rts_file,
                "AutoResetDeviceWithDtrRtsTimingFile": self.auto_reset_device_with_dtr_rts_file,
                "PostProcess": self.post_process,
                "FlashProtectionProcess": self.flash_protection_process,
            },
            # ── GUIRunTime: download target / GUI runtime fields ───────────
            "GUIRunTime": {
                "Profile": self.profile,
                "Port": self.port,
                "Baudrate": self.baudrate,
                "Image": self.image,
                "ImageDir": self.image_dir,
                "Address": self.address,
                "Length": self.length,
                "FactoryMode": self.factory_mode,
                "ChipErase": self.chip_erase,
                "UsbDownload": self.usb_download,
                "SaveLog": self.save_log,
                "LogLevel": self.log_level,
                "GuiLanguageZh": self.gui_language_zh,
                "GuiVisiblePorts": self.gui_visible_ports,
            },
        }
