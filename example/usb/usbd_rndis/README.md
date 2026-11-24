# Example Description

In this application, Ameba is designed as an USB RNDIS (Remote NDIS 1.0) device which can communicate with an RNDIS host (Windows inbox driver, Linux `rndis_host`, or another Ameba board running `usbh_rndis`) and do DHCP test.

The topology:
```
[Ameba] ---USB Cable--- [USB RNDIS Host]
```

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	RNDIS
	```

	```
	CONFIG LWIP --->
	[*] Enable LWIP USB ETHERNET
	```

2. Build

	```
	./ameba.py build -a usbd_rndis -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_rndis`.

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
	[RNDIS-I] USBD RNDIS demo start
	[USBD-A] INIT
	[RNDIS-I] Enter link status task!
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB host (a Linux PC, see [Linux host](#linux-host), or another Ameba board running the `usbh_rndis` example) with USB cable. Windows binds its inbox RNDIS driver ("Remote NDIS Compatible Device") automatically.

3. If the test run success, the console will show:
	```
	[RNDIS-I] Status change 0 -> 1
	[RNDIS-I] Attached
	[RNDIS-I] DHCP Server MAC: 02:11:22:33:44:56
	[RNDIS-I] Device IP: 192.168.45.1
	[RNDIS-I] DHCP Server started
	```
	`DHCP Server MAC` is the MAC of the board side of the link; the host side network interface gets MAC `02:11:22:33:44:55`. The DHCP server lines show up once the host has set the RNDIS packet filter.

4. After the host gets its IP address (the first lease is `192.168.45.100`), type command `AT+PING=192.168.45.100` (the RNDIS host IP address), make sure the ping success and no packet losted

5. The example also support iperf test, type "AT+IPERF=-s" to act as a TCP iperf server, type "AT+IPERF=-c,xx.xx.xx.x,-i,1,-t,10" to act as a TCP client. of course it also support UDP test, type "AT+IPERF" to get more information about the iperf

	Note: For detailed information about iperf and ping, please refer to https://aiot.realmcu.com/zh/latest/rtos/atcmd/at_command_network.html

6. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[RNDIS-I] Status change 1 -> 2
	[RNDIS-I] DETACHED
	[USBD-A] DEINIT
	[RNDIS-I] Free heap 0x<value>
	[USBD-A] INIT
	[RNDIS-I] Reinit done
	[RNDIS-I] Stopping USB RNDIS DHCP Server...
	[RNDIS-I] DHCP Server stopped
	```
	After re-plug:
	```
	[RNDIS-I] Status change 0 -> 1
	[RNDIS-I] Attached
	[RNDIS-I] DHCP Server MAC: 02:11:22:33:44:56
	[RNDIS-I] Device IP: 192.168.45.1
	[RNDIS-I] DHCP Server started
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak. The DHCP server lines come from a separate thread polling every second, so they may interleave with the lines above.

# Shell commands

| Command | Description |
| --- | --- |
| `AT+USBDRNDIS=help` | Print the usage, also when the example is not running. |
| `AT+USBDRNDIS=link,<0\|1>` | Report the upper-layer network link state to the host. `1` means link up, `0` means link down; any other value is rejected. The call is edge-triggered, repeating the same value is a no-op inside the class driver. The example reports `1` after every class init; the host only sees the link up once it has set a non-zero packet filter. A change is reported with `REMOTE_NDIS_INDICATE_STATUS_MSG` (media connect / disconnect). |

Example:
```
AT+USBDRNDIS=link,0
[RNDIS-I] Set RNDIS link status -> DOWN
OK
[RNDIS-I] Stopping USB RNDIS DHCP Server...
[RNDIS-I] DHCP Server stopped

AT+USBDRNDIS=link,1
[RNDIS-I] Set RNDIS link status -> UP
OK
[RNDIS-I] DHCP Server MAC: 02:11:22:33:44:56
[RNDIS-I] Device IP: 192.168.45.1
[RNDIS-I] DHCP Server started
```
The link thread of the example follows the link state seen by the host, so it stops the DHCP server within a second after `link,0` and starts it again after `link,1`. On a Linux host `ip link show enx021122334455` shows `NO-CARRIER` after `link,0` and the carrier back after `link,1`.

Errors:
* `AT+USBDRNDIS` without a parameter prints the help and ends with `ERROR: -2`.
* A bad sub command or value prints `[RNDIS-E] Usage: AT+USBDRNDIS=link,<0|1>` and ends with `ERROR: -2`.
* `link` while the example is not running prints `[RNDIS-E] Not running` and ends with `ERROR: -7`.

# Windows host

Windows 10/11 binds the inbox "Remote NDIS Compatible Device" driver to the Wireless Controller / RF / RNDIS interface class without any INF. The new network adapter gets its IP by DHCP from the board; check it with `ipconfig`, then `ping 192.168.45.1`.

# Linux host

1. The inbox `rndis_host` driver binds the device, check with `dmesg`:
	```
	rndis_host 1-1:1.0 usb0: register 'rndis_host' at usb-..., RNDIS device, 02:11:22:33:44:55
	rndis_host 1-1:1.0 enx021122334455: renamed from usb0
	```
	The interface name depends on the distribution, `enx021122334455` is the name derived from the MAC.

2. NetworkManager usually runs DHCP on its own. Otherwise request a lease by hand and check it:
	```
	sudo dhclient enx021122334455
	ip addr show enx021122334455
	```
	The interface shall get `192.168.45.100/24`.

3. Ping the board, no packet shall be lost:
	```
	ping -c 10 192.168.45.1
	```

4. Throughput with iperf2, the board runs `AT+IPERF`, the PC shall run iperf2 (not iperf3):
	| Test | Board | PC |
	| --- | --- | --- |
	| TCP, board receives | `AT+IPERF=-s` | `iperf -c 192.168.45.1 -i 1 -t 10` |
	| TCP, board sends | `AT+IPERF=-c,192.168.45.100,-i,1,-t,10` | `iperf -s` |
	| UDP, board receives | `AT+IPERF=-s,-u` | `iperf -c 192.168.45.1 -u -b 30M -i 1 -t 10 -l 1460` |
	| UDP, board sends | `AT+IPERF=-c,192.168.45.100,-u,-b,30M,-i,1,-t,10` | `iperf -s -u` |

	Block size (`-l`): the block size is the length of one read or write, one TCP segment or one UDP datagram. iperf2: the board defaults to 1460 bytes as client and 1500 bytes as server, and the PC does not pass its own size to the board, so `-l` is optional. Keep it at most 1460 for TCP, and at most 1472 for UDP so a datagram fits one Ethernet frame without IP fragmentation.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
