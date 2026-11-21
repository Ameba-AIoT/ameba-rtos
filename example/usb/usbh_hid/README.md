# Example Description

In this application, Ameba is designed as a USB HID host. The HID class driver is generic: it serves every HID interface of the device (up to `USBH_HID_MAX_ITF_NUM`), fetches its Report descriptor, sends SET_IDLE and hands every Input report to the application as raw bytes. The example decodes one device type, chosen by `CONFIG_USBH_HID_APP` in `example_usbh_hid.c`, each matching a device type of the `usbd_hid` example:

| `CONFIG_USBH_HID_APP` | Device | Decoding | `usbd_hid` peer |
|---|---|---|---|
| `USBH_HID_APP_MEDIA` (default) | Headset, speaker or keyboard media keys | Volume Up (0xE9), Volume Down (0xEA), Mute (0xE2), Play/Pause (0xCD) and Stop (0xB7) of the Consumer page, located in the Report descriptor with `usbh_hid_find_field()` | `CONFIG_USBD_HID_HEADSET` |
| `USBH_HID_APP_KEYBOARD` | Boot keyboard | Boot protocol, typed keys printed in US layout, LEDs set with `AT+USBHHID=led` | `CONFIG_USBD_HID_KEYBOARD` |
| `USBH_HID_APP_MOUSE` | Boot mouse | Boot protocol, buttons, X/Y and wheel printed | `CONFIG_USBD_HID_MOUSE` |
| `USBH_HID_APP_VENDOR` | Vendor HID, Usage Page 0xFF00 | `AT+USBHHID=tx` sends Output report 0x02, the Input report 0x01 looped back is checked | `CONFIG_USBD_HID_VENDOR` |

The first HID interface that fits the chosen application is used; the others are set up and their reports ignored.

Hot-plug is supported: when the HID device is unplugged and re-plugged, the host stack is automatically torn down and re-initialized so the new device is re-enumerated without a board reset.

# HW Configuration

Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

## Board Rework Reference

### RTL8730E

| Board Variant      | Resistor | Rework Action                                  |
|--------------------|----------|------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

### RTL8721F

| Board Variant    | Resistor | Rework Action                              |
|------------------|----------|---------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW Configuration

1. (Optional) Choose the application with `CONFIG_USBH_HID_APP` in `example_usbh_hid.c`, see the table above. Hot-plug support is enabled by default (`CONFIG_USBH_HID_HOTPLUG 1`). Set it to `0` to disable.

2. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Host)  --->
	[*] HID
	```

3. Build

	```
	./ameba.py build -a usbh_hid -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

4. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 3, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_hid`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

5. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect Result

1. Reset the board. The following log shall be printed on the LOGUART console:
	```
	[HID-I] USBH HID demo start
	```

2. Connect the USB port of the Ameba board to a USB HID device of the chosen type using a USB cable, e.g. a USB headset with in-line volume keys for `USBH_HID_APP_MEDIA`, or a second Ameba board running `usbd_hid` with the matching device type.

3. Once the HID driver enumerates successfully, one `SETUP` line per HID interface appears, then the interface used:
	```
	[HID-I] ATTACH
	[HID-I] SETUP itf 0 sub 0 proto 0 desc 25 OUT 0
	[HID-I] Use itf 0
	```

4. Use the device. The decoded input is printed (without tag prefix):
	* `USBH_HID_APP_MEDIA`: one line per press, a held key printed once:
		```
		Volume Up
		Volume Down
		Mute
		Play/Pause
		Stop
		```
	* `USBH_HID_APP_KEYBOARD`: one line per key press, e.g. `Key 04 'a' mod 00`, `Key 04 'A' mod 02` for Shift+a. `AT+USBHHID=led,<mask>` sets the LEDs (bit0 Num, bit1 Caps, bit2 Scroll Lock) through the Interrupt OUT endpoint, or SET_REPORT when the keyboard has none.
	* `USBH_HID_APP_MOUSE`: one line per report, e.g. `Mouse L0 R0 M0 x 50 y 0 wheel 0`.
	* `USBH_HID_APP_VENDOR`: `AT+USBHHID=tx[,<len>]` (len 1..63, default 10) sends one Output report; the `usbd_hid` vendor device loops it back and `[HID-I] Vendor loopback <len> bytes OK` is printed.

	`AT+USBHHID=status` shows the interface in use, `AT+USBHHID=help` lists the commands of the chosen application.

5. Unplug the USB device. The detach log appears and the hotplug thread reinitializes the host stack:
	```
	[HID-I] DETACH
	[HID-I]
	========== Hotplug #1 ==========
	[HID-I] Free heap: 0x...
	[HID-I] Reinit done
	```

	The hotplug counter increases on every unplug. If the stack cannot be rebuilt, `[HID-E] Reinit fail: <err>` and `[HID-E] Hotplug thread exit` are printed and the board must be reset.

6. Re-plug the device. It is re-enumerated automatically (steps 3–4 repeat) without a board reset.

7. Reset and check the log via LOGUART console, make sure there is no error reported.

# Supported IC

RTL8730E
RTL8721F
