# Example Description

In this application, Ameba is designed as an USB MSC (mass storage class) device, with the storage media configurable as on-chip flash, second flash (NOR), SD card (SDIO or SPI mode), or a RAM disk.

USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as a MSC device and access the data on the configured media via USB interface.

# HW Configuration

Attach SD card to the SDIOH slot on the board (only for the SD card media).

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	MSC
			Select storage media (SD card, SDIO mode)  --->
	```

2. Build

	```
	./ameba.py build -a usbd_msc -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_msc`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The media is a single choice, listed here in menu order:

	* `FLASH (on-chip)` — on-chip primary flash
	* `Second FLASH (NOR)` — external SPI NOR; NAND is not supported
	* `SD card, SDIO mode` — SD card on the SDIO host
	* `SD card, SPI mode` — SD card / SD NAND over a generic SPI master
	* `RAM disk` — VFS RAM disk, SRAM or PSRAM

	`FLASH` and `RAM` mount a FATFS volume through VFS; the other media are
	accessed by the MSC class through `ll_diskio_drv` directly.

	For `RAM disk`, select the media, the file system, the medium and size in `prj.conf` (the RAM disk
	options live under `CONFIG File System`, applied when building this example with `-p`), parent
	options before their children:
	```
	CONFIG_USBD_MSC_RAM=y
	CONFIG_VFS_RAM=y
	CONFIG_VFS_RAM_FATFS=y
	CONFIG_VFS_RAM_SRAM=y
	CONFIG_VFS_RAM_SIZE_KB=64
	```
	Only write user-settable options in a `.conf`: derived hidden symbols such as `CONFIG_VFS_FATFS_INCLUDED`,
	`CONFIG_FATFS_SD_MODE` or `CONFIG_FATFS_SECOND_FLASH` are computed from the options above and are silently ignored.
	- PSRAM medium: `CONFIG_VFS_RAM_PSRAM=y`, size e.g. `CONFIG_VFS_RAM_SIZE_KB=1024`
	- FATFS needs at least 64 KB
	- The SRAM medium needs one contiguous block below `PSRAM_BASE`; pick `PSRAM` if it does not fit
	- Data is lost on unregister / reset: every attach starts from an empty disk

	For `Second FLASH (NOR)`, the media is only selectable once the second flash
	is configured as FATFS under `CONFIG File System`, e.g. in `prj.conf`:
	```
	CONFIG_VFS_SECOND_FLASH=y
	CONFIG_VFS_SECOND_FLASH_FATFS=y
	CONFIG_USBD_MSC_SECOND_FLASH=y
	```
	Leave `CONFIG_VFS_SECOND_FLASH_NAND` unset: FATFS does not support a second
	NAND flash, and the build fails with a clear `#error`. The external SPI NOR
	is wired to the SPI master pins defined in `vfs_second_nor_flash.h`
	(`SPI0_MOSI`/`SPI0_MISO`/`SPI0_SCLK`/`SPI0_CS`).

	For the `FLASH (on-chip)` media, the example mounts a FATFS volume on the
	on-chip flash VFS2 region through VFS and exposes it as a U-disk. The media
	depends on VFS2 being FATFS, so select it first, e.g. in `prj.conf`:
	```
	CONFIG_VFS2_FATFS=y
	CONFIG_USBD_MSC_FLASH=y
	```
	The target is the macro below in `example_usbd_msc.c`:

	```C
	#define USBD_MSC_FLASH_VFS_TARGET VFS_TARGET_FLASH_VFS2
	...
	ret = vfs_target_register(USBD_MSC_VFS_PREFIX, USBD_MSC_FLASH_VFS_TARGET, VFS_RW);
	```

	Keep `VFS_TARGET_FLASH_VFS2`: VFS1 only offers LittleFS (system KV / BT FTL), so
	it can not carry the FATFS U-disk. Configure the VFS2 offset and size in the
	SoC flash layout menuconfig (`CONFIG User Config → Flash Layout Configuration`,
	`VFS2 Offset` / `VFS2 Size`, unset by default; on RTL8735C the `VFS2` row of
	`component/soc/amebapro3/project/flash_layout/default.csv`). The region must lie inside the
	physical flash, without overlapping the BOOT/OTA/system or the VFS1 region.

	The VFS2 region is dedicated to the MSC U-disk while the example runs: with
	`CONFIG_VFS2_FATFS` it is also auto-registered at boot under the `vfs2` prefix,
	do not write it from the device side while the host has it mounted.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[MSC-I] USBD MSC demo start
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[MSC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core and `[MSC-I] Disk init` / `[MSC-I] Init` come from the MSC class driver, not from the example.
	* `[MSC-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.
	* Failure lines: `[MSC-E] Create USBD MSC thread fail`, `[MSC-E] Init disk fail` (media not ready, e.g. no SD card),
	  `[MSC-E] VFS register FATFS/FLASH fail: <err>` / `[MSC-E] VFS register FATFS/RAM fail: <err>`, `[MSC-E] Init USBD fail`,
	  `[MSC-E] Init MSC class fail` and `[MSC-E] Create hotplug thread fail`, each followed by `[MSC-E] USBD MSC demo aborted`
	  and `[MSC-E] Start fail`. A failed re-init after a detach prints `[MSC-E] Init disk fail <err>` and/or `[MSC-E] Hotplug thread fail`.

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on the configured media can be accessed. The storage read/write data path prints no log. A blank media has to be formatted (FAT/FAT32, or exFAT) on the host first; the `FLASH` and `RAM` media are already formatted by VFS.
	- Linux PC:
		```
		lsusb                       # 0bda:<pid> Realtek MSC Device
		lsblk                       # find the new disk, e.g. /dev/sdb1
		sudo mount /dev/sdb1 /mnt
		dd if=/dev/urandom of=test.bin bs=1k count=32
		sudo cp test.bin /mnt/ && sync
		md5sum test.bin /mnt/test.bin
		sudo umount /mnt
		sudo eject /dev/sdb
		```
		The two `md5sum` values shall be identical; re-plug the cable and compare again to check the data persisted on the media (the RAM disk starts empty after every re-attach).
	- Windows PC: the U-disk shows up in Explorer. Copy a file to it, copy it back and compare with `fc /b <src> <copy>`, then use `Safely Remove Hardware` before unplugging.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[MSC-I] DETACHED
	[MSC-I] Deinit
	[USBD-A] DEINIT
	[MSC-I] Disk deinit
	[MSC-I] Free heap: 0x<value>
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[MSC-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

SD card hotplug is supported but disabled by default. Set `MSC_SD_HOTPLUG` to 1 in `example_usbd_msc.c` to enable it (only for the `SD card, SDIO mode` media on AmebaSmart), then inserting and removing the card prints:
```
[MSC-I] SD callback status: <value>
[MSC-I] SD card removed
[MSC-I] Deinit
[USBD-A] DEINIT
[MSC-I] Free heap: 0x<value>
[MSC-I] SD callback status: <value>
[MSC-I] SD card insert, re-init USB
[USBD-A] INIT
[MSC-I] Init
```

The SD card removal only tears the USB stack down, the SD disk itself is not deinited, so no `Disk deinit` is printed.

This is independent of `SD card hotplug` under `CONFIG File System`,
which only re-inits the FATFS disk layer of an app-mounted `VFS_TARGET_SD` volume.
Do not enable both: the SD driver holds a single card-detect callback, so the
second `SD_SetCdCallback()` replaces the first.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
