# Example Description

In this application, Ameba is designed as a USB HID mouse or keyboard device, PC will recognize Ameba as a mouse or keyboard.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

To run USB HID device application on AP:
1. Default the device acts as a HID mouse, if you want to change to HID keyboard, please modify `USBD_HID_DEVICE_TYPE` to `USBD_HID_KEYBOARD_DEVIC`E in `component/usb/device/hid/usbd_hid.h`

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
			*** USB Device Class ***
	[*] 	HID
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbd_hid` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB HID device application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	HID
	```
	Save and exit.

2. How to use:
   - Run `./build.py --app-for-km4 usbd_hid` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[HID] USBD HID demo start
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

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
- AmebaDplus
