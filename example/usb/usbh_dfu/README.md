# Example Description

In this application, Ameba acts as a USB DFU host. It connects to a USB DFU device, downloads a firmware image to the device via `DFU_DNLOAD`, waits for the device to manifest (activate) the image, and optionally uploads the firmware back via `DFU_UPLOAD` to verify the transfer.

The firmware payload is not read from flash or a file: the host generates a 2 KB (`USBH_DFU_DEMO_FW_SIZE`) sequential test pattern in RAM at start-up, and each uploaded block is compared with it on the fly. For a compatible DFU device, refer to the `usbd_dfu` application.

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

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	DFU
	```

2. Build

	```
	./ameba.py build -a usbh_dfu -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_dfu`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

5. Prepare the DFU device: build the `usbd_dfu` example for another Ameba board with its default options (`Device generates its own detach after DFU_DETACH`, `Tolerate further operations after manifestation` and `Support DFU_UPLOAD` all enabled), download it and reset that board. It enumerates in Run-Time mode and needs no further operation.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[DFU-I] USBH DFU demo start
	[DFU-I] FW size: 2048 B
	```

2. Connect the USB DFU device (e.g. another Ameba board running the `usbd_dfu` application) to the USB port of the board with a USB cable. The device must attach in DFU mode within 10 s after the demo starts, otherwise the demo ends with:
	```
	[DFU-E] Wait attach timeout
	[DFU-E] USBH DFU demo aborted
	```
	The demo runs once per boot; reset the board to run it again.

3. A Run-Time mode device (such as `usbd_dfu`) is switched to DFU mode first (DFU 1.1 §5 reconfiguration). The host sends `DFU_DETACH`, the device re-enumerates in DFU mode, and only then the application is notified with `ATTACH`. The following log is printed (USB core enumeration lines omitted):
	```
	[DFU-I] DFU func: attr=0x0f detach=1000 xferSize=1024 ver=0x0110
	[DFU-I] Run-Time DFU device: starting reconfiguration
	[DFU-I] DFU_DETACH sent (wDetachTimeOut=1000 ms)
	[DFU-I] bitWillDetach=1: waiting for device to re-enumerate in DFU mode
	[DFU-I] Run-Time reconfiguration: waiting for DFU re-enumeration
	[DFU-I] DFU func: attr=0x0f detach=1000 xferSize=1024 ver=0x0110
	[DFU-I] ATTACH
	```
	A device that enumerates directly in DFU mode prints only the `DFU func: ...` and `ATTACH` lines.

4. The host starts the download automatically. The following log is printed:
	```
	[DFU-I] Downloading 2048 bytes...
	[DFU-I] Send blk 0 offset 0/2048 B
	[DFU-I] Send blk 1 offset 1024/2048 B
	[DFU-I] DNLOAD final (block 2) OK
	[DFU-I] Download OK
	```

5. If the device supports upload and is manifestation tolerant, the host uploads the firmware back and compares every block with the downloaded pattern:
	```
	[DFU-I] Uploading for verification...
	[DFU-I] Verify blk 0 OK
	[DFU-I] Verify blk 1 OK
	[DFU-I] Verify blk 2 OK
	[DFU-I] Upload OK
	```
	Otherwise the upload step is skipped with one of:
	```
	[DFU-I] Skip upload: non-tolerant device
	[DFU-I] Skip upload: no upload support
	```

6. The demo ends with one verdict line. The test PASSes when the following line is printed:
	```
	[DFU-I] USBH DFU demo done
	```
	On any failure the reason is logged first, followed by:
	```
	[DFU-E] USBH DFU demo aborted
	```
	Typical failure reasons:
	```
	[DFU-E] Wait attach timeout
	[DFU-E] Device does not support DFU_DNLOAD
	[DFU-E] Download FAIL: <err>
	[DFU-E] Download timeout
	[DFU-E] Download fail: <err>, skip upload
	[DFU-E] Verify blk <n> FAIL
	[DFU-E] Upload verify fail
	[DFU-E] Upload timeout
	[DFU-E] Upload fail: <err>
	```
	Unplugging the device during a transfer prints `[DFU-E] Detach in transfer` and fails the running step. A device detach after the test prints `[DFU-I] DETACH`.

# Note

- The demo uses a fixed 2 KB (`USBH_DFU_DEMO_FW_SIZE` in `example_usbh_dfu.c`) sequential test pattern (`0x00, 0x01, ..., 0xFF, 0x00, ...`) generated in RAM as the firmware payload. Replace `dfu_demo_fw` initialization in `example_usbh_dfu.c` with actual firmware data for production use.
- If the DFU device is not manifestation tolerant (`bitManifestationTolerant=0`), the upload step is skipped because the device enters `MANIFEST-WAIT-RESET` state after manifest. The host then prints `[DFU-I] MANIFEST-WAIT-RESET: device will self-detach` (`bitWillDetach=1`), or resets the device and prints `[DFU-I] MANIFEST-WAIT-RESET: reset device` (`bitWillDetach=0`). The download alone then decides the verdict, so the demo still ends with `done`.
- The host handles both Run-Time mode devices (DFU 1.1 §5 reconfiguration: issues `DFU_DETACH` and waits for re-enumeration in DFU mode, resetting the device itself when `bitWillDetach=0`) and devices that enumerate directly in DFU mode.
- The block size is the device `wTransferSize`, clamped to 1024 B (`USBH_DFU_DEFAULT_XFER_SIZE`); a clamp is logged as `Clamp wTransferSize <dev> -> <host>`. Each download/upload run must complete within 30 s.

# Supported IC

RTL8730E
RTL8721F
