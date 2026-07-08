# Example Description

In this application, Ameba acts as a USB DFU host. It connects to a USB DFU device, downloads a firmware image to the device via `DFU_DNLOAD`, waits for the device to manifest (activate) the image, and optionally uploads the firmware back via `DFU_UPLOAD` to verify the transfer.

The demo generates a 2 KB sequential test pattern as the firmware payload. For a compatible DFU device, refer to the `usbd_dfu` application.

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
	[*] 	DFU
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[DFU-I] FW size: 2048 B
	[DFU-I] USBH DFU demo start
	```

2. Connect a USB DFU device (e.g. another Ameba board running the `usbd_dfu` application) to the USB port of the board with a USB cable.

3. The host enumerates the device and starts the download automatically. The following log is printed:
	```
	[DFU-I] DFU device attached
	[DFU-I] Downloading 2048 bytes...
	[DFU-I] Send blk 0 offset 0/2048 B
	[DFU-I] Send blk 1 offset 1024/2048 B
	[DFU-I] DNLOAD final (block 2) OK
	[DFU-I] Download OK
	```

4. If the device supports upload and is manifestation tolerant, the host uploads the firmware back for verification:
	```
	[DFU-I] Uploading for verification...
	[DFU-I] Verify blk 0 OK
	[DFU-I] Verify blk 1 OK
	[DFU-I] Verify blk 2 OK
	[DFU-I] Upload OK
	```

# Note

- The demo uses a fixed 2 KB sequential test pattern (`0x00, 0x01, ..., 0xFF, 0x00, ...`) as the firmware payload. Replace `dfu_demo_fw` initialization in `example_usbh_dfu.c` with actual firmware data for production use.
- If the DFU device is not manifestation tolerant (`bitManifestationTolerant=0`), the upload step is skipped because the device enters `MANIFEST-WAIT-RESET` state and detaches after manifest.
- The host handles both Run-Time mode devices (DFU 1.1 §5 reconfiguration: issues `DFU_DETACH` and waits for re-enumeration in DFU mode) and devices that enumerate directly in DFU mode.

# Supported IC

RTL8730E
RTL8721F
