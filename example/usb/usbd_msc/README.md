# Example Description

In this application, Ameba is designed as an USB MSC (mass storage class) device, with the storage media configurable as on-chip flash, second flash (NOR), SD card (SDIO or SPI mode), or a RAM disk.

USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as a MSC device and access the data on the configured media via USB interface.

# HW Configuration

Attach SD card to the SDIOH slot on the board (only for the SD card media).

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	MSC
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

	For `RAM disk`, select the medium and size in `prj.conf` (they live under
	`CONFIG File System`, applied automatically when building this example):
	```
	CONFIG_VFS_FATFS_INCLUDED=y
	CONFIG_VFS_RAM=y
	CONFIG_VFS_RAM_SRAM=y
	CONFIG_VFS_RAM_SIZE_KB=64
	```
	- PSRAM medium: `CONFIG_VFS_RAM_PSRAM=y`, size e.g. `CONFIG_VFS_RAM_SIZE_KB=1024`
	- FATFS needs at least 64 KB
	- The SRAM medium needs one contiguous block below `PSRAM_BASE`; pick `PSRAM` if it does not fit
	- Data is lost on unregister / reset: every attach starts from an empty disk

	For `Second FLASH (NOR)`, leave `CONFIG_VFS_SECOND_FLASH_NAND` unset: FATFS
	does not support a second NAND flash, and the build fails with a clear
	`#error`.

	For the `FLASH (on-chip)` media, the example mounts a FATFS volume on an
	on-chip flash region through VFS and exposes it as a U-disk. The region is
	chosen by modifying the macro below in `example_usbd_msc.c`:

	```C
	/* choose VFS_REGION_1 or VFS_REGION_2 */
	#define USBD_MSC_FLASH_VFS_REGION VFS_REGION_2
	...
	ret = vfs_user_register(USBD_MSC_VFS_PREFIX, VFS_FATFS, VFS_INF_FLASH, USBD_MSC_FLASH_VFS_REGION, VFS_RW);
	```

	1. Change `USBD_MSC_FLASH_VFS_REGION` to `VFS_REGION_1` if that region is free
	   (see the note below), otherwise keep `VFS_REGION_2`.

	2. Configure the chosen region's offset and size in the SoC flash layout
	   menuconfig (`CONFIG User Config → Flash Layout Configuration`). The region
	   must be enabled and lie inside the physical flash, without overlapping the
	   BOOT/OTA/system or another VFS region.

	The chosen region is dedicated to the MSC U-disk: it must not be registered
	by another filesystem (e.g. `app_filesystem_init()` may already register
	LITTLEFS on `VFS_REGION_1`, so use `VFS_REGION_2` in that case).

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

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
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed. The storage read/write data path prints no log.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[MSC-I] DETACHED
	[MSC-I] Deinit
	[USBD-A] DEINIT
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
[MSC-I] Disk deinit
[MSC-I] Deinit
[USBD-A] DEINIT
[MSC-I] Free heap: 0x<value>
[MSC-I] SD callback status: <value>
[MSC-I] SD card insert, re-init USB
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
