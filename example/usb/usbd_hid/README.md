# Example Description

In this application, Ameba is designed as a USB HID mouse or keyboard device, PC will recognize Ameba as a mouse or keyboard.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`, then select the HID device type under `HID`
	(Mouse is the default; Mouse and Keyboard are mutually exclusive):
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	HID
				Select HID device type (Mouse)  --->
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[HID-I] USBD HID demo start
	[USBD-A] INIT
	[HID-I] INIT
	[HID-I] ATTACHED
	```
	Notes:
	* `demo start` is printed before the stack is brought up, so by itself it does not prove the init succeeded, check that `[USBD-A] INIT` follows and that no `[HID-E] USBD HID demo aborted` shows up.
	* `[USBD-A] INIT` comes from the USB device core and the first `[HID-I] INIT` comes from the example `init` callback invoked by the class driver. The hotplug thread prints the same `INIT` text when it observes the initial attach status, so this line may appear more than once.
	* `[HID-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to PC with USB cable.

3. Test with HID device:
	- For HID mouse:
		- If `HID_CONSTANT_DATA` in `example_usbd_hid.c` is set to 1 (default), PC mouse cursor will automatically move according to array `mdata[]` once Ameba board is connected to PC.
			```
			[HID-I] Mouse data TX test start
			[HID-I] Test round 1/10
			[HID-I] Test round 2/10
			[HID-I] Test round 3/10
			...
			[HID-I] Test round 8/10
			[HID-I] Test round 9/10
			[HID-I] Test round 10/10
			[HID-I] Test done
			```
		- If `HID_MOUSE_CMD` in `example_usbd_hid.c` is set to 1 (default), type following command from Ameba LOGUART console to control the PC cursor behavior:
			```
			# mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>
			```
			Each command prints `[HID-I] Send mouse data`.
	- For HID keyboard:
		- If `HID_CONSTANT_DATA` in `example_usbd_hid.c` is set to 1 (default), key data `aA` will report to PC once Ameba board is connected, just open a text editor on PC and make sure it gets the cursor focus, `aA` will keep typing into the text editor.
			```
			[HID-I] Keyboard data TX test start
			[HID-I] Test round 1/10
			[HID-I] Test round 2/10
			[HID-I] Test round 3/10
			...
			[HID-I] Test round 8/10
			[HID-I] Test round 9/10
			[HID-I] Test round 10/10
			[HID-I] Test done
			```
		- While type the leds key such as CAPsLock and NumLock from PC, PC will send a message to the device, e.g., keep typing NumLock, LOGUART console will print following log:
			```
			[HID-I] RX 1 byte(s): 0x00
			[HID-I] RX 1 byte(s): 0x01
			[HID-I] RX 1 byte(s): 0x00
			[HID-I] RX 1 byte(s): 0x01
			```

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[HID-I] DETACHED
	[HID-I] DEINIT
	[USBD-A] DEINIT
	[HID-I] Free heap: 0x<value>
	[USBD-A] INIT
	[HID-I] INIT
	[HID-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak. Each re-attach starts a brand new constant data session, so the `Test round 1/10` ... `Test done` sequence runs again.

# Shell commands

| Command | Description |
| --- | --- |
| `mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>` | Send one HID mouse report to the host. Only `<left>` is mandatory, the remaining arguments default to 0. The axis values are clamped to the -127..127 range declared by the mouse report descriptor. Only registered on a mouse build with `HID_MOUSE_CMD` set to 1. |

# Note

Tested on Windows 7/10, MacOS, Ubuntu.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
