# Example Description

In this application, Ameba is designed as an USB MSC (mass storage class) device with SD card or SRAM as the memory media.

USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as a MSC device and access the data on SD card via USB interface.

# HW Configuration

Attach SD card to the SDIOH slot on the board.

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	MSC
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[MSC-I] USBD MSC demo start
	```

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed.

# Note

None

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaGreen2
  * RTL8721F
