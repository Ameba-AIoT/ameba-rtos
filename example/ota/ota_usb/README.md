# Example Description

This example demonstrates image update using Over-the-Air (OTA) programming over USB DFU (Device Firmware Upgrade).

The Ameba board enumerates as a USB DFU device, the USB host (PC) downloads the OTA file (`ota_all.bin`) with `dfu-util`, the OTA engine programs it into the inactive OTA slot and the board reboots into the new image.

USB DFU is an OTA transport, `OTA_USB` (`component/ota/transport/ota_usb.c`). The example runs the same flow as the other OTA examples, plus one call to report the result to the host:

```c
ota_init(ctx, NULL, 0, "dfu", OTA_USB);
ret = ota_start(ctx);
ota_usb_finish(ret);
ota_deinit(ctx);
if (ret == OTA_OK) sys_reset();
```

`ota_usb_finish()` must be called after every `OTA_USB` `ota_start()`; it answers the final zero-length `DFU_DNLOAD` (manifestation), waits until the host has read the final `DFU_GETSTATUS`, and stops USB. `ota_usb_abort()` makes a read waiting for the host return EOF, so `ota_start()` returns `OTA_ERR`.

# HW Configuration

None

# SW Configuration

1. **Menuconfig**

   The example `prj.conf` enables:
   ```
   CONFIG_USB_OTG_EN_MENU=y
   CONFIG_USBD_DFU_MENU=y
   CONFIG_USBD_DFU_CAN_UPLOAD=n
   ```
   `CONFIG_USBD_DFU_CAN_UPLOAD=n` disables `DFU_UPLOAD`: it is served from ISR context and reading back a half-programmed OTA slot is not meaningful.

2. **Build and Flash**
   - Refer to the [SDK Examples](https://aiot.realmcu.com/en/latest/rtos/sdk/sdk_example/index.html) to generate images (`./ameba.py build -a ota_usb -p`, `-p` makes sure `prj.conf` is applied)
   - Download images to board using Ameba Image Tool

3. **Prepare the OTA file**

   Build a new firmware with a higher image version (`OTA_SWITCH_BY_VERSION` boots the higher version); the build output contains `ota_all.bin`. Refer to [Image Pattern and Version Number](https://aiot.realmcu.com/zh/latest/rtos/ota/index.html#version-number) for the OTA file format and the image switch strategy.

4. **Install dfu-util on the host**

   - Linux: install from the system package manager (e.g. `sudo apt install dfu-util`) or from the [dfu-util project page](https://dfu-util.sourceforge.net)
   - Windows: download dfu-util and bind the WinUSB driver with [Zadig](https://zadig.akeo.ie) (`Options` -> `List All Devices`, select `Realtek DFU Device` with USB ID `0BDA 8006`)

5. **Start OTA Update**

   Reset the board and run `dfu-util -d 0bda:8006 -D ota_all.bin` on the host

# Expected Result

1. Reset the board and connect its USB port to the host (PC). The following log is printed on the LOGUART console:
	```
	<<<<<< OTA USB Example >>>>>>>
	...
	[OTA-I] [TRANSPORT] USB DFU Run-Time mode, wait for dfu-util
	[OTA-I] [TRANSPORT] Connected
	```

2. On the host, the device is listed in Run-Time mode:
	```
	dfu-util -l
	Found Runtime: [0bda:8006] ver=0200, devnum=xx, cfg=1, intf=0, path="x-x", alt=0, name="DFU Interface", serial="UNKNOWN"
	```

3. Download the OTA file (select the device by VID:PID when more than one DFU device is present):
	```
	dfu-util -d 0bda:8006 -D ota_all.bin
	```
	`dfu-util` sends `DFU_DETACH` first, the device re-enumerates in DFU mode:
	```
	[DFU-I] Detach: re-enumerating in DFU mode
	[OTA-I] [TRANSPORT] USB re-enumerating
	```
	Then the OTA engine prints its progress:
	```
	[OTA-I] [CORE] OTA2 address space will be upgraded
	[OTA-I] [CORE] OTA download percentage: 10
	...
	[OTA-I] ========== OTA SUCCESS ==========
	```

4. After the final zero-length `DFU_DNLOAD`, the device manifests the image and reboots into the new firmware:
	```
	[DFU-I] Manifest done -> MANIFEST-SYNC
	[ota_task] Ready to reboot
	```
	The first line reads `-> WAIT-RESET` instead when the class is not built manifest-tolerant.

# Notes

- Only the OTA file format (`ota_all.bin`) is accepted. A raw image without the OTA header is rejected by the OTA engine and the host gets `errWRITE`.
- `Invalid DFU suffix signature` printed by `dfu-util` can be ignored, `ota_all.bin` carries no DFU suffix.
- After a failure the example task exits and USB is stopped; reset the board to retry. If the host ends the download before the whole OTA file is received, the OTA engine reports `Unexpected EOF`; the slot manifest is never written, so the old image keeps booting.
- A checksum or manifest failure is reported on the last data block, so `dfu-util` shows `errWRITE`. The exception is a host that disconnects after the last data block but before the zero-length `DFU_DNLOAD`: that block already returned success and there is no host left to report to. The image is still not activated in that case, the old one keeps booting.
- The `-d` option shall carry the real VID:PID `0bda:8006`: in Run-Time mode `dfu-util` matches it strictly, any other ID reports `No DFU capable USB device available`.
- Do not use the libusb-win32 (libusb0) driver: with it Windows rejects the zero-length control OUT requests (`ControlTransfer failed: [87]`), so `DFU_DETACH` fails with `error detaching` and the final zero-length `DFU_DNLOAD` fails with `Error sending completion packet (LIBUSB_ERROR_IO)`. Check the bound driver with `LIBUSB_DEBUG=4`, the log reports `driver(s): WINUSB` for the device.

# Supported IC

RTL8735C
