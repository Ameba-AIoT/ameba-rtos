# Example Description

In this application, Ameba is designed as an USB WHC device which can communicate with USB host like Linux PC.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	WHC
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[WHC-I] USBD WHC demo start
	```

2. Connect the USB port of Ameba board to USB host (with USB WHC driver installed) with USB cable.

3. USB host will then recognize Ameba board as WHC device and communicate as required.

# Note

Specific USB WHC driver is required for USB host to recoganize this USB WHC device, please refer to AN for details.

# Supported IC

RTL8721F
