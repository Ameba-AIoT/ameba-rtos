# Example Description

In this application, Ameba is designed as a USB HID mouse or keyboard device, PC will recognize Ameba as a mouse or keyboard.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	HID
	```
	Save and exit.

2. Build
   Type command `./build.py -a usbd_hid` under the project directory to build images.

3. Download
	Download images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[HID-I] USBD HID demo start
	```

2. Connect the USB port of Ameba board to PC with USB cable.

3. Test with HID device:
	1. For HID mouse:
	   - If `CONFIG_USBD_HID_CONSTANT_DATA` is set to 1 (default), PC mouse cursor will move according to array `mdata[]` once Ameba board is connected to PC.
	   - If `CONFIG_USBD_HID_MOUSE_CMD` is set to 1 (default), type following command from Ameba LOGUART console to control the PC cursor behavior:
			```
			# mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>
			```
	2. For HID keyboard, key data `aA` will report to PC once Ameba board is connected, while type the leds key such as CAPsLock and NumLock from PC, PC will send a message to the device.

# Note

Tested on Windows 7/10, MacOS, Ubuntu.

# Supported IC

- RTL872XD
- RTL8721Dx
- RTL8720F
- RTL8721F
- RTL8730E
- RTL8730F