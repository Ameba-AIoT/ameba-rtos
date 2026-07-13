# Example Description

In this application, Ameba is designed as a USB DFU (Device Firmware Upgrade) device. The USB host can download a firmware image to the device and optionally upload it back for verification using the DFU 1.1 protocol.

The example uses a 4 KB RAM buffer to receive downloaded blocks. For production use, replace the `write` and `manifest` callbacks with flash erase/program routines.

The device first enumerates in Run-Time mode. When the host issues a `DFU_DETACH` request, the device re-enumerates in DFU mode and accepts firmware blocks via `DFU_DNLOAD`. After all blocks are received, the device activates the firmware in the `manifest` callback and returns to `dfuIDLE` state.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	DFU
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[DFU-I] USBD DFU demo start
	```

2. Connect the USB port of the Ameba board to the USB host (PC) with a USB cable. The host enumerates the device in Run-Time mode.

3. On the host, use `dfu-util` to trigger DFU mode and download a firmware image:
	```
	dfu-util -D firmware.bin
	```
	The board re-enumerates in DFU mode and the following log is printed:
	```
	[DFU-I] DFU detach
	[DFU-I] Detach: re-enumerating in DFU mode
	[DFU-I] Reconf: re-enumerating
	```

4. `dfu-util` then sends the firmware blocks. For each 1 KB(default DFU block size) block received:
	```
	[DFU-I] Write blk 0 offset 0 len 1024 (total 1024 B)
	[DFU-I] Write blk 1 offset 1024 len 1024 (total 2048 B)
	...
	```

5. After the final zero-length `DFU_DNLOAD`, the device manifests the image:
	```
	[DFU-I] Manifest OK (xxx B in RAM)
	```

6. Optionally upload the stored image back to verify:
	```
	dfu-util -U dump.bin
	```

# Note

- This example stores the firmware in a 4 KB RAM buffer and does not program flash. To use DFU for real OTA updates, implement the `write` and `manifest` callbacks in `example_usbd_dfu.c` to call the flash driver.
- On Windows, WinUSB driver binding is required. Use [Zadig](https://zadig.akeo.ie) to install WinUSB for the DFU interface before running `dfu-util`.
- `dfu-util` is available on Linux. Install via the system package manager (e.g. `sudo apt install dfu-util`) or from the [dfu-util project page](https://dfu-util.sourceforge.net).

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
