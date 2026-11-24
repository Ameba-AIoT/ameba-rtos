# Example Description

In this application, Ameba is designed as a USB DFU (Device Firmware Upgrade) device. The USB host can download a firmware image to the device and optionally upload it back for verification using the DFU 1.1 protocol.

The example uses a 4 KB RAM buffer to receive downloaded blocks. For production use, replace the `write` and `manifest` callbacks with flash erase/program routines.

The device first enumerates in Run-Time mode. When the host issues a `DFU_DETACH` request, the device re-enumerates in DFU mode and accepts firmware blocks via `DFU_DNLOAD`. After all blocks are received, the device activates the firmware in the `manifest` callback and returns to `dfuIDLE` state.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	DFU
	```

2. Build

	```
	./ameba.py build -a usbd_dfu -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_dfu`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

	The three sub options under `DFU` set the DFU functional descriptor `bmAttributes` and all default to `y`:
	```
	[*] 	DFU
	[*] 		Device generates its own detach after DFU_DETACH
	[*] 		Tolerate further operations after manifestation
	[*] 		Support DFU_UPLOAD
	```
	* `CONFIG_USBD_DFU_WILL_DETACH` (bitWillDetach): `y` the device re-enumerates by itself after `DFU_DETACH`; `n` the device waits for a host USB Reset within `wDetachTimeOut` (1000 ms) and reverts to Run-Time mode if none arrives.
	* `CONFIG_USBD_DFU_MANIFESTATION_TOLERANT` (bitManifestationTolerant): `y` the device returns to `dfuIDLE` after manifest, so an upload can follow; `n` the device enters `dfuMANIFEST-WAIT-RESET`.
	* `CONFIG_USBD_DFU_CAN_UPLOAD` (bitCanUpload): `n` stalls every `DFU_UPLOAD`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

5. Host tool:
	* Linux: install `dfu-util` via the system package manager, e.g. `sudo apt install dfu-util`.
	* Windows: download `dfu-util` from the [dfu-util project page](https://dfu-util.sourceforge.net) and use [Zadig](https://zadig.akeo.ie) to bind WinUSB to the DFU interface. Bind it once in Run-Time mode, and again after the device re-enumerates in DFU mode.

# Expect result

1. Reset the board, the following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[DFU-I] USBD DFU demo start
	[USBD-A] INIT
	[DFU-I] Init
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core and `[DFU-I] Init` comes from the example `init` callback invoked by the class driver, not from the example thread itself.
	* On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.

2. Connect the USB port of the Ameba board to the USB host (PC) with a USB cable. The host enumerates the device in Run-Time mode.

3. Prepare a test image on the host. The example does not check any image header or signature, the `write` callback only copies the blocks into a 4 KB RAM buffer, so any raw binary of at most 4096 bytes works, e.g.:
	```
	dd if=/dev/urandom of=firmware.bin bs=1024 count=4
	```
	A larger image is rejected at the first block beyond 4 KB with `[DFU-E] Write out of range: blk=<n> len=<len>` followed by `[DFU-E] Write blk <n> err: <err>`, and `dfu-util` reports the error.

4. Check the device and download the image, `<pid>` is `8730` on RTL8730E, `8722` on RTL8721Dx and `f851` on RTL8721F:
	```
	dfu-util -l -d 0bda:<pid>
	dfu-util -d 0bda:<pid> -a 0 -D firmware.bin
	```
	`dfu-util -l` lists the device as `Found Runtime` before the detach and as `Found DFU` after it. When started on a Run-Time device, `dfu-util` issues `DFU_DETACH`, the board re-enumerates in DFU mode and the following log is printed:
	```
	[DFU-I] DFU detach
	[DFU-I] Detach: re-enumerating in DFU mode
	[DFU-I] Reconf: re-enumerating
	[DFU-I] Deinit
	[USBD-A] DEINIT
	[USBD-A] INIT
	[DFU-I] Init
	```
	With `CONFIG_USBD_DFU_WILL_DETACH=n`, `[DFU-I] Detach: waiting for host USB Reset (<ms>ms timeout)` is printed instead of the lines after `DFU detach`, and the device switches to DFU mode on the host USB Reset.

5. `dfu-util` then sends the firmware blocks. For each 1 KB (default DFU transfer size) block received:
	```
	[DFU-I] Write blk 0 offset 0 len 1024 (total 1024 B)
	[DFU-I] Write blk 1 offset 1024 len 1024 (total 2048 B)
	...
	```

6. After the final zero-length `DFU_DNLOAD`, the device manifests the image:
	```
	[DFU-I] Manifest OK (4096 B in RAM)
	[DFU-I] Manifest done -> MANIFEST-SYNC
	```
	The `Manifest done` line comes from the class driver poll timer, it is not printed when a host `DFU_GETSTATUS` advances the state first. It reads `-> WAIT-RESET` instead when `CONFIG_USBD_DFU_MANIFESTATION_TOLERANT=n`; the device then leaves DFU mode (re-enumerating in Run-Time mode by itself with `CONFIG_USBD_DFU_WILL_DETACH=y`) and the re-init clears the RAM buffer, so the upload below is not possible.

7. Upload the stored image back and compare it with the original, while the device still stays in DFU mode (every re-init, e.g. a board reset or a new detach, clears the RAM buffer):
	```
	dfu-util -d 0bda:<pid> -a 0 -U dump.bin
	cmp firmware.bin dump.bin
	```
	The test PASSes if `cmp` reports no difference. The upload is served from the ISR context and prints no device log.

# Note

- This example stores the firmware in a 4 KB RAM buffer (`USBD_DFU_DEMO_BUF_SIZE`) and does not program flash. To use DFU for real OTA updates, implement the `write` and `manifest` callbacks in `example_usbd_dfu.c` to call the flash driver.
- The device keeps the VID/PID in both modes (VID `0x0BDA`), only `bInterfaceProtocol` changes (Run-Time `0x01`, DFU `0x02`).

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
