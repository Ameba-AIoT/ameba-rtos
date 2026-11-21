# Example Description

In this application, Ameba is designed as a layer-2 bridge between a USB CDC ECM device (USB-to-Ethernet adapter) and a Wi-Fi router. Ameba connects to the router as a Wi-Fi station and forwards Ethernet frames between the Wi-Fi STA interface and the USB Ethernet interface, so a downstream device plugged into the adapter gets its IP address from the router by DHCP and talks to the router network directly.

The topology:
```
[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
```

Notes:
- Ameba itself runs no DHCP on either interface (lwIP prints `skip DHCP !!`); it does not route or NAT.
- The source MAC of every frame sent to Wi-Fi is replaced by the Ameba STA MAC (also inside ARP), and frames from Wi-Fi are delivered to the MAC of the downstream device, so only one downstream device is supported.
- IPv6 frames are dropped in both directions; IPv4 and ARP are forwarded.

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
	[*] 	CDC ECM
				Select USB Ethernet (USB Ethernet Bridge)  --->
	```
	```
	CONFIG LWIP --->
	[*] Enable LWIP USB ETHERNET
	```

2. Build

	```
	./ameba.py build -a usbh_wifi_bridge -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_wifi_bridge`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Set up the test environment as the topology. The USB CDC ECM device shall be a standard CDC ECM USB-to-Ethernet adapter (bInterfaceSubClass 0x06); e.g. an RTL8152/RTL8153/RTL8156 based adapter (its CDC ECM configuration is selected automatically). RNDIS/NCM-only or vendor-specific adapters are not supported. Set the "other device" (e.g. a PC) to obtain its IP address automatically (DHCP).

2. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ECMB-I] USBH WIFI BRIDGE demo start
	[ECMB-I] INIT
	[ECMB-I] Wait for WIFI connection
	[ECMB-I] Please use AT+WLCONN=ssid,xx,pw,yy to connect AP first time
	```
	The two `Wait for ...` lines repeat every 2 s until Wi-Fi is connected.

3. Connect the USB CDC ECM device. After enumeration and once the Ethernet cable of the adapter is plugged in (link up), the following log is printed:
	```
	[ECMB-I] ATTACH
	[ECMB-I] SETUP
	[ECMB-I] Switch to link
	```
	Unplugging the Ethernet cable prints `[ECMB-I] Switch to unlink`, plugging it back prints `Switch to link` again.

4. Type command `AT+WLCONN=ssid,xx,pw,yy` to connect to the router, xx is the router ssid name, yy is the password. Type `AT+WLCONN` to get more details about the command. Once connected, the `Wait for WIFI connection` log stops and the bridge is active.

5. Verify the bridge on the "other device":
	- `ipconfig` (Windows) or `ip addr` (Linux): the Ethernet interface gets an IP address in the router subnet (renew it with `ipconfig /renew` if it was assigned before the bridge was up).
	- `ping <router IP>`: make sure the ping succeeds and no packet is lost.
	- (Optional) Throughput: run `iperf -s` on a PC behind the router and `iperf -c <server IP>` on the "other device", or the reverse.

6. Hotplug: unplug the USB CDC ECM device, the stack is torn down and re-initialized:
	```
	[ECMB-I] DETACH
	[ECMB-I] Switch to unlink
	[ECMB-I] DEINIT
	[ECMB-I] Free heap: 0x...
	[ECMB-I] INIT
	[ECMB-I] Reinit done
	```
	The order of these lines may vary. Re-plug the device and step 3 repeats; the bridge works again without re-connecting Wi-Fi. If the stack cannot be rebuilt, `[ECMB-E] Reinit fail` and `[ECMB-E] Hotplug thread exit` are printed and the board must be reset.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F