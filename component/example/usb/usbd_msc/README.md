# Example Description

In this application, Ameba is designed as an USB MSC (mass storage class) device with SD card as its physical memory media.

USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as MSC device and access the data on SD card via USB interface.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

To run USB MSC device application on AP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
			*** USB Device Class ***
	[*] 	MSC
	```
	Save and exit.

2. How to use:
   - Run `./build.py -a usbd_msc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB MSC device application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	MSC
	```
	Save and exit.

2. How to use:
   - Run `./build.py --app-for-km4 usbd_msc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[MSC] USBD MSC demo start
	```

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
