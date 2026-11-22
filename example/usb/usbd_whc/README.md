# Example Description

**This example is for internal USB interface testing only.** It is a loopback test fixture of the USB WHC device interface, not a WHC network card: no Wi-Fi or Bluetooth traffic is carried. For the complete WHC function, check https://aiot.realmcu.com/en/latest/rtos/whc/index.html .

On RTL8721F the device enumerates with the WHC descriptors (BT HCI, BT SCO and WiFi interfaces, or only the WiFi interface when BT is disabled in OTP; VID/PID and strings from OTP, default `0bda:f851` "802.11ax WLAN Adapter") and the example loops the BULK data back:

| OUT endpoint | Behaviour |
| --- | --- |
| WiFi BULK OUT 1 (0x05) | echoed on WiFi BULK IN (0x84) |
| WiFi BULK OUT 2/3 (0x06/0x07) | received and dropped |
| BT BULK OUT (0x02) | echoed on BT BULK IN (0x82), only when BT is enabled in OTP |

Each echo carries the payload of one OUT transfer, up to 512 bytes. With `CONFIG_WHC_ETH`, the Ethernet BULK OUT (0x03) is echoed on its BULK IN (0x82) and BT BULK IN moves to 0x86.

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
	./ameba.py build -a usbd_whc -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_whc`.

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
	* An extra USB PHY calibration line is printed by the USB HAL right after `[USBD-A] INIT`: `[USB-I] UPHY para from autoload`, `[USB-I] UPHY para from OTP` or `[USB-I] UPHY OTP flag 0x<flag>, use default`.
	* Failure lines: `[WHC-E] Create USBD WHC thread fail`, `[WHC-E] USBD WHC demo aborted` (start failed), `[WHC-E] Hotplug thread fail` (re-init after a detach failed). `[WHC-W] Fail to load OTP para` means the default identity is used.

2. Connect the USB port of Ameba board to the USB host (e.g. Linux PC) with USB cable. No WHC host driver is needed for this test.

3. The data path prints no log. Check the WiFi loopback from the host, the device shall return every packet written to EP 0x05 on EP 0x84 unchanged.
	For example with pyusb on a Linux PC (`pip install pyusb`, run as root or with a udev rule; on Windows bind WinUSB to the WiFi interface with Zadig first), the script prints `True`:
	```
	import usb.core, usb.util
	dev = usb.core.find(idVendor=0x0bda, idProduct=0xf851)
	intf = usb.util.find_descriptor(dev.get_active_configuration(), custom_match=lambda i: any(e.bEndpointAddress == 0x84 for e in i))
	if dev.is_kernel_driver_active(intf.bInterfaceNumber):
		dev.detach_kernel_driver(intf.bInterfaceNumber)
	data = bytes(range(32))
	dev.write(0x05, data, 1000)
	print(bytes(dev.read(0x84, 512, 1000)) == data)
	```
	Use the VID/PID programmed in OTP if any. Keep the payload shorter than the endpoint MPS (512 at High Speed, 64 at Full Speed), so that each write is one short packet.

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

The real WHC function (WHC host driver, Wi-Fi/BT over USB) is not part of this example, refer to https://aiot.realmcu.com/en/latest/rtos/whc/index.html .

# Supported IC

RTL8721F
