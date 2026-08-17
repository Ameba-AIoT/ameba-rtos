# Example Description

In this application, Ameba is designed as a USB HID host. It can connect to HID devices such as headphones with media control buttons. When a button on the device is pressed (volume up / down / mute / play-pause / stop), Ameba recognizes and logs the corresponding command.

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

1. Menuconfig
	Use menuconfig and:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] HID
	Save and exit.
	```

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

3. (Optional) Hot-plug support is enabled by default (`CONFIG_USBH_HID_HOTPLUG 1` in `example_usbh_hid.c`). Set it to `0` to disable.

# Expect Result

1. Reset the board. The following log shall be printed on the LOGUART console:
	```
	[HID-I] USBH HID demo start
	```

2. Connect the USB port of the Ameba board to a USB HID device (e.g. a headphone with media control buttons) using a USB cable.

3. Once the HID driver enumerates successfully, the following logs appear:
	```
	[HID-I] CONNECT
	[HID-I] HID attach
	[HID-I] HID setup
	```

4. Press a button on the HID device. The corresponding event is printed:
	```
	Volume Up
	Volume Down
	Play/Pause
	```

5. Unplug the USB device. The detach log appears and the hotplug thread reinitializes the host stack:
	```
	[HID-I] HID detach
	[HID-I] DISCONNECT
	[HID-I] ========== Hotplug #1 ==========
	[HID-I] Free heap: 0x...
	[HID-I] Re-init USB host...
	[HID-I] Re-init complete
	```

6. Re-plug the device. It is re-enumerated automatically (steps 3–4 repeat) without a board reset.

7. Reset and check the log via LOGUART console, make sure there is no error reported.

# Supported IC

RTL8730E
RTL8721F
