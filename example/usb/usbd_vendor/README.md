# Example Description

In this application, Ameba is designed as an USB vendor specific device which can only communicate with the USB vendor specific USB host as defined in `usbh_vendor` application, i.e. another Ameba board running the `usbh_vendor` example.

The device exposes one BULK, one INTR and one ISOC endpoint pair and loops back every packet it receives: the data received on an OUT endpoint is sent back on the IN endpoint of the same type, directly from the receive callback in the USB ISR. Setting `VENDOR_BULK_ASYNC_XFER` / `VENDOR_INTR_ASYNC_XFER` / `VENDOR_ISOC_ASYNC_XFER` to 1 in `example_usbd_vendor.c` moves that loopback into a thread of its own. The endpoint addresses are:

| SoC | BULK IN/OUT | INTR IN/OUT | ISOC IN/OUT |
| --- | --- | --- | --- |
| RTL8721F | 0x86 / 0x03 | 0x84 / 0x05 | 0x82 / 0x02 |
| RTL8730E, RTL8721Dx, RTL8735C | 0x81 / 0x02 | 0x85 / 0x05 | 0x83 / 0x04 |

Currently it is only for RD internal debug usage and not available as default.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	VENDOR
	```

2. Build

	```
	./ameba.py build -a usbd_vendor -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_vendor`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[VND-I] USBD VENDOR demo start
	[USBD-A] INIT
	[VND-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[VND-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 (RTL8721F) an extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.
	* Failure lines: `[VND-E] Create USBD VENDOR thread fail` or `[VND-E] USBD VENDOR demo aborted` (start failed), `[VND-E] Hotplug thread fail` (re-init after a detach failed).

2. Connect the USB port of Ameba board to USB vendor host (another Ameba board as USB vendor host) with USB cable.

3. Check the log via LOGUART console, make sure there is no error reported. The loopback prints no log on the device side, the test result is printed on the LOGUART console of the USB vendor host instead, refer to the `README.md` of `usbh_vendor` for details. The test PASSes when the host prints:
	```
	[VND-I] ISOC test PASS
	[VND-I] BULK loopback test PASS 100/100
	[VND-I] INTR loopback test PASS 100/100
	```
	The BULK and INTR lines may come in either order, they run in parallel.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[VND-I] DETACHED
	[USBD-A] DEINIT
	[VND-I] Free heap: 0x<value>
	[USBD-A] INIT
	[VND-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
