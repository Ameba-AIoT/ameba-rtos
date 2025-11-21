# Example Description

In this application, Ameba is designed as an USB INIC device which can communicate with USB host like Linux PC.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	INIC
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[INIC-I] USBD INIC demo start
	```

2. Connect the USB port of Ameba board to USB host (with USB INIC driver installed) with USB cable.

3. USB host will then recognize Ameba board as INIC device and communicate as required.

# Note

Specific USB INIC driver is required for USB host to recoganize this USB INIC device, please refer to AN for details.

# Supported IC

- AmebaGreen2
  * RTL8721F
