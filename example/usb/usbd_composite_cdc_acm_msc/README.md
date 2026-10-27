# Example Description

In this application, Ameba is designed as a USB CDC ACM and MSC composite device, USB host can recognize Ameba as a CDC virtual serial port and an UDisk.

# HW Configuration

Attach SD card to the SDIOH slot on the board (only for the SD card media).

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] CDC ACM
	[*] MSC
			Select storage media (SD card, SDIO mode)  --->
	```
	Save and exit. The media is a single choice, listed here in menu order:

	* `FLASH (on-chip)` — on-chip primary flash
	* `Second FLASH (NOR)` — external SPI NOR; NAND is not supported
	* `SD card, SDIO mode` — SD card on the SDIO host
	* `SD card, SPI mode` — SD card / SD NAND over a generic SPI master
	* `RAM disk` — VFS RAM disk, SRAM or PSRAM

	`FLASH` and `RAM` mount a FATFS volume through VFS; the other media are
	accessed by the MSC class through `ll_diskio_drv` directly.

	`COMP_MSC_FLASH_VFS_REGION` in `example_usbd_composite_cdc_acm_msc.c` selects
	the flash region for the `FLASH (on-chip)` media (VFS1 or VFS2, default VFS2);
	use VFS1 only if it is free. See `example/usb/usbd_msc/README.md` for the
	region and menuconfig details.

	For `RAM disk`, select the medium and size in `prj.conf`, as in
	`example/usb/usbd_msc/README.md`. Set the size according to the RAM/PSRAM
	actually available on the board. Content is volatile: every attach formats
	an empty disk.

	For `Second FLASH (NOR)`, leave `CONFIG_VFS_SECOND_FLASH_NAND` unset; see
	`example/usb/usbd_msc/README.md` for details.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[COMP-I] USBD ACM+MSC comp demo start
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[COMP-I] ATTACHED
	```
	Notes:
	* `demo start` is printed before the stack is brought up; if any class or the composite framework fails to initialize, `demo aborted` is printed instead of the attach log.
	* `[USBD-A] INIT` comes from the USB device core and `[MSC-I] Disk init` / `[MSC-I] Init` come from the MSC class driver, not from the example.
	* `[COMP-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to PC with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages. The echo data path prints no log.

4. Also USB host will recognize Ameba board as an UDisk, and the data on SD card can be accessed. The storage read/write data path prints no log.

5. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[COMP-I] DETACHED
	[MSC-I] Deinit
	[USBD-A] DEINIT
	[COMP-I] Free heap: 0x<value>
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[COMP-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please make sure the VID/PID value used in CDC ACM class is included in the INF file:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
; Add support for new VID/PID
 [DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
; Add support for new VID/PID
```

# Note

SD card hotplug is supported but disabled by default. Set `MSC_SD_HOTPLUG` to 1 in `example_usbd_composite_cdc_acm_msc.c` to enable it (only for the `SD card, SDIO mode` media on AmebaSmart, and `COMP_HOTPLUG` must stay 1), then inserting and removing the card prints:
```
[COMP-I] SD card removed
[MSC-I] Disk deinit
[MSC-I] Deinit
[USBD-A] DEINIT
[COMP-I] Free heap: 0x<value>
[COMP-I] SD card insert, re-init USB
[MSC-I] Disk init
[USBD-A] INIT
[MSC-I] Init
```

This is independent of `SD card hotplug (FATFS)` under `CONFIG File System`,
which only re-inits the FATFS disk layer of an app-mounted `VFS_INF_SD` volume.
Do not enable both: the SD driver holds a single card-detect callback, so the
second `SD_SetCdCallback()` replaces the first.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
