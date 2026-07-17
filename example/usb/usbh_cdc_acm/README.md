# Example Description

In this application, Ameba is designed as an USB CDC ACM host which can communicate with specific USB CDC ACM device as defined in `usbd_cdc_acm` application.

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
|------------------|----------|--------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                 |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] CDC ACM
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ACM-I] USBH CDC ACM demo start
	```

2. Connect the specific USB CDC ACM device (e.g. another Ameba board running usbd_cdc_acm_new application) to the USB port of the board with USB cable.

3. Reset and check the test log via LOGUART console, USB CDC ACM host log will be printed, make sure there is no error reported. If the test run success, the console will show:
	```
	[ACM-I] Ctrl test PASS
	[ACM-I] Bulk loopback test start, loopback times:100, size: 1024
	[ACM-I] Bulk loopback test PASS
	```

# Note

None

# Supported IC

RTL8730E
RTL8721F
RTL8735C
