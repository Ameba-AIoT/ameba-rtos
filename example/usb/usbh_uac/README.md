# Example Description

With USB host v2.0 interface, Ameba can be designed to a USB vendor-specific UAC host.

In this application, Ameba boots up as USB host which can communicate with USB vendor-specific UAC device as defined in `usbd_uac` application.

Only for Reaktek RD internal usage.

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

1. Pre-configuration
   Adjust `USBH_UAC_ISOC_BUF_SIZE` to sync with the settings of USB vendor-specific UAC device.

2. Menuconfig
	Use menuconfig and:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	UAC
	```
	Save and exit.

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

USB UAC specific log will be printed on loguart console, make sure there is no error reported.
```
[UAC-I] USBH UAC demo start
```

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
