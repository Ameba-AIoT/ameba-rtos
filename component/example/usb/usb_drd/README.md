# Example Description

In this application, Ameba is designed as an USB DRD (dual role device) which will switch role from MSC device to MSC host.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

To run USB DRD application on AP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (DRD)  --->
			*** USB Device Class ***
	[*] 	MSC
			*** USB Host Class ***
	[*] 	MSC
	```
	Save and exit.

2. How to use:
   - Run `./build.py -a usb_drd` under `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Insert SD card to the SD slot of the board and reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[DRD] USB DRD demo start
	```

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed within 20s until following log be printed on the LOGUART console:
	```
	[DRD] Role switch
	```

4. Disconnect the USB port of Ameba board from USB host, connect it to a UDisk formatted with FATFS with OTG cable.

5. Ameba board will recoganize the MSC device and do write/read performance test, check the log on the LOGUART console and make sure there is no USB related error reported.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
