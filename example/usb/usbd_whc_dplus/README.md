# Example Description

**This example is for internal USB interface testing only.** It is a loopback test fixture of the USB WHC device interface, not a WHC network card: no Wi-Fi or Bluetooth traffic is carried. For the complete WHC function, check https://aiot.realmcu.com/en/latest/rtos/whc/index.html .

On RTL8721Dx the device enumerates as a Full Speed vendor specific device with one interface (`0bda:8722` "802.11n WLAN Adapter") and the example loops the BULK data back:

| OUT endpoint | Behaviour |
| --- | --- |
| WiFi BULK OUT 2 (0x04) | echoed on WiFi BULK IN (0x83) |
| WiFi BULK OUT 1 (0x02) | received and dropped |

Each echo carries the payload of one OUT transfer, up to 64 bytes. The vendor control request `bRequest` 0xF0 (D2H, `wIndex` 0x01 QUERY or 0x06 RESET, `wLength` 24) answers a 24-byte query packet; RESET also resets the board 500 ms later.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	WHC
	```

2. Build

	```
	./ameba.py build -a usbd_whc_dplus -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_whc_dplus`.

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
	[WHC-I] USBD WHC demo start
	[USBD-A] INIT
	[WHC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[WHC-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* Failure lines: `[WHC-E] Create USBD WHC thread fail`, `[WHC-E] USBD WHC demo aborted` (start failed), `[WHC-E] Hotplug thread fail` (re-init after a detach failed).

2. Connect the USB port of Ameba board to the USB host (e.g. Linux PC) with USB cable. No WHC host driver is needed for this test.

3. The data path prints no log. Check the loopback from the host, the device shall return every packet written to EP 0x04 on EP 0x83 unchanged.
	For example with pyusb on a Linux PC (`pip install pyusb`, run as root or with a udev rule; on Windows bind WinUSB to the device with Zadig first), the script prints `True`:
	```
	import usb.core
	dev = usb.core.find(idVendor=0x0bda, idProduct=0x8722)
	if dev.is_kernel_driver_active(0):
		dev.detach_kernel_driver(0)
	data = bytes(range(32))
	dev.write(0x04, data, 1000)
	print(bytes(dev.read(0x83, 64, 1000)) == data)
	```
	Keep the payload shorter than 64 bytes (the Full Speed MPS), so that each write is one short packet.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[WHC-I] DETACHED
	[USBD-A] DEINIT
	[WHC-I] Free heap: 0x<value>
	[USBD-A] INIT
	[WHC-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

The real WHC function (WHC host driver, Wi-Fi over USB) is not part of this example, refer to https://aiot.realmcu.com/en/latest/rtos/whc/index.html .

# Supported IC

RTL8721Dx
