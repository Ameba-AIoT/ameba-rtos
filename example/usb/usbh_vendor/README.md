# Example Description

In this application, Ameba is designed as a vendor-specific USB host as a design reference for users to design customized USB applications. The USB vendor-specific host can communicate with vendor-specific USB device as defined in `usbd_vendor` application.

Currently it is only for RD internal debug usage and not available as default.

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
	[*] 	Vendor
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[VND-I] USBH vendor demo start
	```
2. Connect the vendor-specific USB device (e.g. another Ameba board running usbd_vendor_new application) to the USB port of the board with USB cable.

3. Reset and check the test log via LOGUART console, make sure there is no error reported and the transfer success rate is reasonable, e.g.:
	```
	[VND-I] ISOC test start, times:100, size: 1024
	[VEN-I] ISOC OUT test finish 100/100:
	[VEN-I]   0   1   2   3   4   5   6   7   8   9
	...
	[VEN-I]  90  91  92  93  94  95  96  97  98  99
	[VEN-I] ISOC IN test finish 100/100:
	[VEN-I]   0   1   2   3   4   5   6   7   8   9
	...
	[VEN-I]  90  91  92  93  94  95  96  97  98  99
	[VND-I] ISOC test PASS
	[VND-I] BULK loopback test start, times:100, size: 512
	[VND-I] INTR loopback test start, times:100, size: 1024
	[VND-I] BULK loopback test PASS 100/100
	[VND-I] INTR loopback test PASS 100/100
	[VND-I] USBH vendor demo stop
	```
# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
