# Example Description

In this application, Ameba is designed as an USB MSC (mass storage class) device with SD card or SRAM as the memory media.

USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as a MSC device and access the data on SD card via USB interface.

# HW Configuration

Attach SD card to the SDIOH slot on the board.

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	MSC
			Select storage media (RAM)  --->
	```
	Save and exit.

	As a USB storage device, Ameba supports flexible access and management of various storage media, which can be configured as:

	* RAM
	* SD Card (SD Mode: using SDIO interface)
	* Second Flash

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[MSC-I] USBD MSC demo start
	[MSC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core and `[MSC-I] Disk init` / `[MSC-I] Init` come from the MSC class driver, not from the example. `Disk init` is only printed when the storage media is SD card or second flash, a RAM disk build does not print it.
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

SD card hotplug is supported but disabled by default. Set `MSC_SD_HOTPLUG` to 1 in `example_usbd_msc.c` to enable it, then inserting and removing the card prints:
```
[MSC-I] SD callback status: <value>
[MSC-I] SD card removed
[USBD-A] DEINIT
[MSC-I] Free heap: 0x<value>
[MSC-I] SD card insert, re-init USB
[USBD-A] INIT
```

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
