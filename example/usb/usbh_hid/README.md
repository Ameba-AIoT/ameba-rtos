# Example Description

In this application, Ameba is designed as a USB HID host. It can connect to HID devices such as headphones with media control buttons. When a button on the device is pressed (volume up / down / mute / play-pause / stop), Ameba recognizes and logs the corresponding command.

Only the Consumer Control page (0x0C) usages Volume Up (0xE9), Volume Down (0xEA), Mute (0xE2), Play/Pause (0xCD) and Stop (0xB7) are decoded (see `usbh_hid.h`). A keyboard or mouse still enumerates, but its key presses and movements print nothing.

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

1. (Optional) Hot-plug support is enabled by default (`CONFIG_USBH_HID_HOTPLUG 1` in `example_usbh_hid.c`). Set it to `0` to disable.

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

2. Connect the USB port of the Ameba board to a USB HID device with Consumer Control keys using a USB cable, e.g. a USB headset with an in-line volume/mute control, a USB speaker with volume keys, or a multimedia keyboard with media keys.

3. Once the HID driver enumerates successfully, the following logs appear:
	```
	[HID-I] ATTACH
	[HID-I] SETUP
	```

4. Press a button on the HID device. The corresponding event is printed (without tag prefix), one line per press:
	```
	Volume Up
	Volume Down
	Mute
	Play/Pause
	Stop
	```
	A key the device does not report through these usages prints nothing.

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
