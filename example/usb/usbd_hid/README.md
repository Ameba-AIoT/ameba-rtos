# Example Description

In this application, Ameba is designed as a USB HID mouse, keyboard, headset buttons or vendor device, PC will recognize Ameba as a mouse, a keyboard, the buttons of a Type-C headset, or a vendor defined HID device.

The vendor device is a reference for a customer HID protocol: one interface on the Vendor Defined usage page 0xFF00 with an INTR IN and an INTR OUT endpoint, one 64-byte Input report with Report ID 0x01 and one 64-byte Output report with Report ID 0x02 (Report ID included, 63 bytes of payload each). The host OS binds no driver of its own to it, so generic HID tools built on hidapi reach it directly. Add or resize reports in `hid_vendor_report_desc[]` of `usbd_hid.c` and the `USBD_HID_VENDOR_*` defines of `usbd_hid.h`.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`, then select the HID device type under `HID`
	(Mouse is the default; Mouse, Keyboard, Headset buttons and Vendor are mutually exclusive):
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
			# AT+USBDHID=mouse,<left>,<right>,<middle>,<x>,<y>,<wheel>
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

	- For HID headset buttons:
		- If `HID_CONSTANT_DATA` in `example_usbd_hid.c` is set to 1 (default), a Volume Up then a Volume Down click is reported once Ameba board is connected, the OS volume bar pops up.
			```
			[HID-I] Headset key TX test start
			[HID-I] Test round 1/10
			...
			[HID-I] Test done
			```
		- The headset buttons are meant to be combined with UAC, refer to the `usbd_composite_hid_uac` example.
	- For HID vendor, use HIDPyToy tool (download address https://github.com/todbot/hidpytoy) or any other hidapi based HID tool:
		- Connect the HID device: click rescan, choose "Realtek HID Device" device, and click connect.
		- Loopback: type "2,1,2,3,4" (Report ID 2 first) in the HIDPyToy, and click "Send Out Report". The board echoes the payload back as an Input report with Report ID 1, click "Read In Reports" to get it. The LOGUART console prints:
			```
			[HID-I] RX report ID 0x02, <n> bytes, payload[0] 0x01
			```
			`<n>` is 64 when the tool pads the Output report to the length the report descriptor declares, as hidapi on Windows does.
		- Device TX: type `AT+USBDHID=tx,<len>` in the LOGUART console, then click "Read In Reports" in the HIDPyToy, an Input report with Report ID 1 and `len` bytes of value `len` is read.
			```
			[HID-I] Send vendor report, <len> bytes of payload
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
| `AT+USBDHID=mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]` | Send one HID mouse report to the host. Only `<left>` is mandatory, the remaining arguments default to 0. `<left>`/`<right>`/`<middle>`: `0` or `1` (pressed); `<x>`/`<y>`/`<wheel>`: -127..127, the range the mouse report descriptor declares. Any other value is rejected. Only registered on a mouse build with `HID_MOUSE_CMD` set to 1. |
| `AT+USBDHID=tx[,<len>]` | Send one vendor Input report to the host: Report ID 0x01, then `len` bytes of value `len & 0xFF`, the rest of the 63-byte payload is 0. `len`: 1..63, default 10; any other value is rejected. Only registered on a vendor build. |

# Note

Tested on Windows 7/10, MacOS, Ubuntu.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
