# Example Description

In this application, Ameba is designed as a USB CDC ACM and MSC composite device, USB host can recognize Ameba as a CDC virtual serial port and an UDisk.

# HW Configuration

Attach SD card to the SDIOH slot on the board (only for the SD card media).

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] CDC ACM
	[*] MSC
			Select storage media (SD card, SDIO mode)  --->
	```
	The `prj.conf` of this example enables these options and keeps the default media,
	`SD card, SDIO mode` (`CONFIG_USBD_MSC_SD_CARD`), which needs an SD card in the SDIO host slot.

2. Build

	```
	./ameba.py build -a usbd_composite_cdc_acm_msc -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_composite_cdc_acm_msc`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The media is a single choice, listed here in menu order:

	* `FLASH (on-chip)` (`CONFIG_USBD_MSC_FLASH`) — on-chip primary flash, VFS2 region
	* `Second FLASH (NOR)` (`CONFIG_USBD_MSC_SECOND_FLASH`) — external SPI NOR; NAND is not supported
	* `SD card, SDIO mode` (`CONFIG_USBD_MSC_SD_CARD`, default) — SD card on the SDIO host
	* `SD card, SPI mode` (`CONFIG_USBD_MSC_SD_SPI`) — SD card / SD NAND over a generic SPI master
	* `RAM disk` (`CONFIG_USBD_MSC_RAM`) — VFS RAM disk, SRAM or PSRAM

	RTL8721Dx has no SDIO host, so `SD card, SDIO mode` is not offered there: use `SD card, SPI mode`
	(the one selected when none of the other media is configured), `FLASH (on-chip)` or `RAM disk`.

	`FLASH` and `RAM` mount a FATFS volume through VFS; the other media are
	accessed by the MSC class through `ll_diskio_drv` directly.

	To switch to another media, add its lines to `prj.conf` (parent options before their children) and build with `-p`:
	* `SD card, SPI mode`:
		```
		CONFIG_USBD_MSC_SD_SPI=y
		```
	* `FLASH (on-chip)`, only selectable once VFS2 is FATFS:
		```
		CONFIG_VFS2_FATFS=y
		CONFIG_USBD_MSC_FLASH=y
		```
		The target is the macro below in `example_usbd_composite_cdc_acm_msc.c`, registered with `vfs_target_register()`:
		```C
		#define COMP_MSC_FLASH_VFS_TARGET VFS_TARGET_FLASH_VFS2
		...
		ret = vfs_target_register(COMP_MSC_VFS_PREFIX, COMP_MSC_FLASH_VFS_TARGET, VFS_RW);
		```
		Keep `VFS_TARGET_FLASH_VFS2`: VFS1 only offers LittleFS (system KV / BT FTL), so it can not carry the FATFS U-disk.
		Configure the VFS2 offset and size in `CONFIG User Config` > `Flash Layout Configuration` (`VFS2 Offset` / `VFS2 Size`, unset by default);
		the region must lie inside the physical flash, without overlapping the BOOT/OTA/system or the VFS1 region.
	* `Second FLASH (NOR)`, only selectable once the second flash is FATFS:
		```
		CONFIG_VFS_SECOND_FLASH=y
		CONFIG_VFS_SECOND_FLASH_FATFS=y
		CONFIG_USBD_MSC_SECOND_FLASH=y
		```
		Leave `CONFIG_VFS_SECOND_FLASH_NAND` unset: FATFS does not support a second NAND flash.
	* `RAM disk`:
		```
		CONFIG_USBD_MSC_RAM=y
		CONFIG_VFS_RAM=y
		CONFIG_VFS_RAM_FATFS=y
		CONFIG_VFS_RAM_SRAM=y
		CONFIG_VFS_RAM_SIZE_KB=64
		```
		For PSRAM use `CONFIG_VFS_RAM_PSRAM=y` instead of `CONFIG_VFS_RAM_SRAM=y`, with a size such as `CONFIG_VFS_RAM_SIZE_KB=1024`.
		FATFS needs at least 64 KB; set the size according to the RAM/PSRAM actually available on the board.
		Content is volatile: every attach formats an empty disk.

	Only write user-settable options in a `.conf`: derived hidden symbols such as `CONFIG_VFS_FATFS_INCLUDED` or
	`CONFIG_FATFS_SD_MODE` are computed from the options above and are silently ignored.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

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
	On Linux the port is `/dev/ttyACMx` (stop ModemManager first, e.g. `sudo systemctl stop ModemManager`, then `picocom -b 115200 /dev/ttyACM0`); on Windows 10/11 it is `USB Serial Device (COMx)`.

4. Also USB host will recognize Ameba board as an UDisk, and the data on the configured media can be accessed. The storage read/write data path prints no log. A blank media has to be formatted (FAT/FAT32, or exFAT) on the host first; the `FLASH` and `RAM` media are already formatted by VFS.
	- Linux PC:
		```
		lsblk                       # find the new disk, e.g. /dev/sdb1
		sudo mount /dev/sdb1 /mnt
		dd if=/dev/urandom of=test.bin bs=1k count=32
		sudo cp test.bin /mnt/ && sync
		md5sum test.bin /mnt/test.bin
		sudo umount /mnt
		```
		The two `md5sum` values shall be identical; re-plug the cable and compare again to check the data persisted on the media (the RAM disk starts empty after every re-attach).
	- Windows PC: the U-disk shows up in Explorer. Copy a file to it, copy it back and compare with `fc /b <src> <copy>`, then use `Safely Remove Hardware` before unplugging.

5. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[COMP-I] DETACHED
	[MSC-I] Deinit
	[USBD-A] DEINIT
	[MSC-I] Disk deinit
	[COMP-I] Free heap: 0x<value>
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[COMP-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

Windows 10/11 needs no driver. Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please make sure the VID/PID value used in CDC ACM class is included in the INF file:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
 [DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
```

# Note

SD card hotplug is supported but disabled by default. Set `MSC_SD_HOTPLUG` to 1 in `example_usbd_composite_cdc_acm_msc.c` to enable it (only for the `SD card, SDIO mode` media on AmebaSmart, and `COMP_HOTPLUG` must stay 1), then inserting and removing the card prints:
```
[COMP-I] SD card removed
[MSC-I] Deinit
[USBD-A] DEINIT
[MSC-I] Disk deinit
[COMP-I] Free heap: 0x<value>
[COMP-I] SD card insert, re-init USB
[MSC-I] Disk init
[USBD-A] INIT
[MSC-I] Init
[COMP-I] ATTACHED
```
`[COMP-I] ATTACHED` shows up after the card insertion only while the cable stays connected.

This is independent of `SD card hotplug` (`CONFIG_VFS_SD_CARD_HOTPLUG`) under `CONFIG File System`,
which only re-inits the FATFS disk layer of an app-mounted `VFS_TARGET_SD` volume.
Do not enable both: the SD driver holds a single card-detect callback, so the
second `SD_SetCdCallback()` replaces the first.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
