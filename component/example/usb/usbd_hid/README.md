# Example Description

In this application, Ameba is designed as a USB HID mouse or keyboard device, PC will recognize Ameba as a mouse or keyboard.

# HW Configuration

None

# SW configuration

1. Set HID device type via USBD_HID_DEVICE_TYPE defined in <sdk>\component\usb\device\hid\usbd_hid.h
	- For HID mouse (default):
	#define USBD_HID_DEVICE_TYPE		USBD_HID_MOUSE_DEVICE
	- For HID keyboard:
	#define USBD_HID_DEVICE_TYPE		USBD_HID_KEYBOARD_DEVICE

2. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	HID
	```
	Save and exit.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[HID-I] USBD HID demo start
	```

2. Connect the USB port of Ameba board to PC with USB cable.

3. Test with HID device:
	- For HID mouse:
		- If `CONFIG_USBD_HID_CONSTANT_DATA` is set to 1 (default), PC mouse cursor will automatically move according to array `mdata[]` once Ameba board is connected to PC.
		- If `CONFIG_USBD_HID_MOUSE_CMD` is set to 1 (default), type following command from Ameba LOGUART console to control the PC cursor behavior:
			```
			# mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>
			```
	- For HID keyboard:
		- Key data `aA` will report to PC once Ameba board is connected, just open a text editor on PC and make sure it gets the cursor focus, `aA` will keep typing into the text editor.
		- While type the leds key such as CAPsLock and NumLock from PC, PC will send a message to the device, e.g., keep typing NumLock, LOGUART console will print following log:
			[HID-I] RX 1 byte(s): 0x00
			[HID-I] RX 1 byte(s): 0x01
			[HID-I] RX 1 byte(s): 0x00
			[HID-I] RX 1 byte(s): 0x01
			...

# Note

Tested on Windows 7/10, MacOS, Ubuntu.

# Supported IC

- RTL872XD
- RTL8721Dx
- RTL8720F
- RTL8721F
- RTL8730E
