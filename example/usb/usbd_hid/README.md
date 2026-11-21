# Example Description

In this application, Ameba is designed as a USB HID mouse, keyboard, headset buttons or vendor device, PC will recognize Ameba as a mouse, a keyboard, the buttons of a Type-C headset, or a vendor defined HID device.

The vendor device is a reference for a customer HID protocol: one interface on the Vendor Defined usage page 0xFF00 with an INTR IN and an INTR OUT endpoint, one 64-byte Input report with Report ID 0x01 and one 64-byte Output report with Report ID 0x02 (Report ID included, 63 bytes of payload each). The host OS binds no driver of its own to it, so generic HID tools built on hidapi reach it directly. Add or resize reports in `hid_vendor_report_desc[]` of `usbd_hid.c` and the `USBD_HID_VENDOR_*` defines of `usbd_hid.h`.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	HID
				Select HID device type (Mouse)  --->
	```

2. Build

	```
	./ameba.py build -a usbd_hid -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_hid`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The HID device type is selected under `HID` (Mouse is the default; Mouse, Keyboard, Headset buttons and Vendor are mutually exclusive).
	The default `prj.conf` selects Mouse (`CONFIG_USBD_HID_MOUSE=y`). To test another type, replace it in `prj.conf` with `CONFIG_USBD_HID_KEYBOARD=y`, `CONFIG_USBD_HID_HEADSET=y` or `CONFIG_USBD_HID_VENDOR=y`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[HID-I] USBD HID demo start
	[USBD-A] INIT
	[HID-I] INIT
	[HID-I] ATTACHED
	```
	Notes:
	* `demo start` is printed before the stack is brought up, so by itself it does not prove the init succeeded, check that `[USBD-A] INIT` follows and that none of the failure lines `[HID-E] Create USBD HID thread fail`, `[HID-E] Start fail` (stack bring up failed) or `[HID-E] Hotplug thread fail` (re-init after a detach failed) shows up.
	* `[USBD-A] INIT` comes from the USB device core and the first `[HID-I] INIT` comes from the example `init` callback invoked by the class driver. The hotplug thread prints the same `INIT` text when it observes the initial attach status, so this line may appear more than once.
	* `[HID-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.

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
		- If `HID_CONSTANT_DATA` in `example_usbd_hid.c` is set to 1 (default), key data `aA` will report to PC once Ameba board is connected, just open a text editor on PC and make sure it gets the cursor focus, `aA` is typed into the text editor once per test round (10 rounds).
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
		- While typing an LED key such as CapsLock or NumLock on the PC, the PC sends the LED Output report to the device. The report is received silently in the ISR, no log is printed for it.

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
			`<n>` is 64 when the tool pads the Output report to the length the report descriptor declares, as hidapi on Windows does. Output reports whose first byte is not Report ID 2 are dropped silently.
		- Device TX: type `AT+USBDHID=tx,<len>` in the LOGUART console, then click "Read In Reports" in the HIDPyToy, an Input report with Report ID 1 and `len` bytes of value `len` is read.
			```
			[HID-I] Send vendor report, <len> bytes of payload
			```

4. Host side check:
	- Windows: the device shows up in Device Manager under `Mice and other pointing devices` (mouse), `Keyboards` (keyboard) or `Human Interface Devices` (headset buttons, vendor), and the cursor / text editor / volume bar reacts as described above.
	- Linux: `lsusb` lists `0bda:<pid>` (`<pid>` is `8730` on RTL8730E, `8722` on RTL8721Dx, `f851` on RTL8721F and `8006` on RTL8735C), `dmesg` shows the `hid-generic` binding and the `/dev/hidrawN` node. For mouse, keyboard and headset buttons run `sudo evtest`, pick the `Realtek HID Device` event node and check the reported events (`REL_X`/`REL_Y`/`REL_WHEEL`/`BTN_*`, `KEY_A`, `KEY_VOLUMEUP`/`KEY_VOLUMEDOWN`). For the vendor device write a 64-byte Output report (Report ID 2 first) to `/dev/hidrawN` and read back a 64-byte Input report with Report ID 1 and the same payload, e.g. with hidapi in Python:
		```
		import hid
		d = hid.device(); d.open(0x0bda, 0x<pid>)
		d.write([0x02, 1, 2, 3, 4] + [0] * 59)
		print(d.read(64, 1000))
		```

5. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
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
| `AT+USBDHID=help` or `AT+USBDHID=?` | Print the usage of the commands available in this build. |
| `AT+USBDHID=mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]` | Send one HID mouse report to the host. Only `<left>` is mandatory, the remaining arguments default to 0. `<left>`/`<right>`/`<middle>`: `0` or `1` (pressed); `<x>`/`<y>`/`<wheel>`: -127..127, the range the mouse report descriptor declares. Any other value is rejected. Only registered on a mouse build with `HID_MOUSE_CMD` set to 1. |
| `AT+USBDHID=tx[,<len>]` | Send one vendor Input report to the host: Report ID 0x01, then `len` bytes of value `len & 0xFF`, the rest of the 63-byte payload is 0. `len`: 1..63, default 10; any other value is rejected. Only registered on a vendor build. |

The `AT+USBDHID` command is only registered on a mouse build (with `HID_MOUSE_CMD` set to 1) and on a vendor build, the keyboard and headset buttons builds have no command. A command other than `help` is refused with `[HID-E] Not running` while the example is not running, an unknown sub command prints `[HID-E] Invalid cmd: <cmd>, try AT+USBDHID=help`. The command ends with `OK`, or with `ERROR: <err>` on failure.

Examples:
```
AT+USBDHID=mouse,0,0,0,50,0,0
AT+USBDHID=mouse,0,0,0,0,-50,0
AT+USBDHID=mouse,1
AT+USBDHID=mouse,0
AT+USBDHID=tx,8
```
The first two move the cursor 50 pixels right then 50 pixels up, the next two press and release the left button (a click), the last one sends a vendor Input report carrying 8 bytes of value 8.

# Note

Tested on Windows 7/10, MacOS, Ubuntu.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
