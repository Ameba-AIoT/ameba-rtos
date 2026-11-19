# Example Description

In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.
The topology:
```
[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
```

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
				Select USB Ethernet (USB Ethernet)  --->
	```
	```
	CONFIG LWIP --->
	[*] Enable LWIP USB ETHERNET
	```

2. Build

	```
	./ameba.py build -a usbh_cdc_ecm -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbh_cdc_ecm`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Set up the test environment as the topology. The USB CDC ECM device can be:
	* any USB Ethernet adapter that implements the standard CDC ECM class;
	* a Realtek RTL8152 / RTL8153 / RTL8156 USB Ethernet adapter (VID 0x0BDA). For these the host also programs the MAC address `00:11:22:33:44:55` and the LED setting given by the example into the adapter;
	* another Ameba board running the `usbd_cdc_ecm` example. It has no RJ45 side: it is the DHCP server itself at 192.168.45.1, and its first lease is 192.168.45.100.

2. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ECM-I] USBH CDC ECM demo start
	[ECM-I] INIT
	[ECM-I] Enter link status task
	[ECM-I] ECM link is down (1)
	```
	`ECM link is down (<n>)` repeats every 10 s while no device is attached or its Ethernet link is down.

3. Connect the USB CDC ECM device. If the test runs successfully, the console will show:
	```
	[ECM-I] ATTACH
	[ECM-I] SETUP
	[ECM-I] Do DHCP
	[ECM-I] MAC[00 11 22 33 44 55]
	[$]wifi got ip:"192.168.1.100"
	[ECM-I] Switch to link
	```
	The `MAC[...]` line shows the MAC address the device uses (`00 11 22 33 44 55` for a Realtek RTL815x adapter, the adapter's own MAC otherwise), and `wifi got ip` the address assigned by DHCP (printed by lwIP for every netif, not only Wi-Fi). If DHCP fails, `[$]wifi got ip timeout` and `[ECM-I] DHCP fail, retry` are printed and DHCP is retried 1 s later.

4. After getting the IP address, type command `AT+PING=<router_ip>`, make sure the ping succeeds without packet loss, i.e. the result shows `0% packet loss`.

5. The example also supports throughput tests with iperf, see [Throughput test](#throughput-test).

6. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the USB cable, following log shall be printed for each cycle (`Switch to unlink (detach event)` comes from the link thread, so its position may vary), followed by the log of step 3:
	```
	[ECM-I] DETACH
	[ECM-I] Switch to unlink (detach event)
	[ECM-I] DEINIT
	[ECM-I] Free heap: 0x<value>
	[ECM-I] INIT
	[ECM-I] Reinit done
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

7. Ethernet cable check, for a device with VID 0x0BDA only (for other VIDs the example takes the link as always up once enumerated): unplug the RJ45 cable of the adapter, `[ECM-I] Switch to unlink` is printed and the default route goes back to Wi-Fi; plug it in again, the log from `Do DHCP` in step 3 is printed again.

# Throughput test

The topology:
```
[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router] ---RJ45--- PC
```

Once `Switch to link` is printed, the default route is the USB Ethernet netif, so iperf traffic runs through the dongle. Connect the PC to the same router by cable, so Wi-Fi is not the bottleneck, and disconnect the board from Wi-Fi with `AT+WLDISCONN` if it is connected.

The board runs iperf2 with `AT+IPERF` and iperf3 with `AT+IPERF3`; the PC shall use the same tool, the two protocols are not compatible. Type `AT+IPERF` or `AT+IPERF3` for the full option list.

## Block size (`-l`)

The block size is the length of one read or write, one TCP segment or one UDP datagram:

* iperf3: **set `-l` on the client side, 1 to 2920 bytes, 1460 recommended** (one TCP MSS). The client sends its size to the server when the test starts. The board defaults to 1460 bytes to save RAM, while a PC iperf3 defaults to 128 KB for TCP:
	* board as server: a PC client without `-l` is refused, the board prints `blksize is too large, this test may fail` and `Please set the appropriate size by using '-l ' option`;
	* board as client: `-l` is optional, without it the board sends 1460 bytes. Setting it keeps the size explicit and the same as in the PC client tests.
* iperf2: the board defaults to 1460 bytes as client and 1500 bytes as server, and the PC does not pass its own size to the board, so `-l` is optional. Keep it at most 1460 for TCP, and at most 1472 for UDP so a datagram fits one Ethernet frame without IP fragmentation.

## Commands

Replace `<PC_IP>` with the PC address and `<BOARD_IP>` with the address printed by `wifi got ip`. The board uses the default ports, 5001 for iperf2 and 5201 for iperf3.

| Test | Board | PC |
| --- | --- | --- |
| TCP, board sends | `AT+IPERF3=-c,<PC_IP>,-i,1,-t,10,-l,1460` | `iperf3 -s` |
| TCP, board receives | `AT+IPERF3=-s` | `iperf3 -c <BOARD_IP> -i 1 -t 10 -l 1460` |
| UDP, board sends | `AT+IPERF3=-c,<PC_IP>,-u,-b,40M,-i,1,-t,10,-l,1460` | `iperf3 -s` |
| UDP, board receives | `AT+IPERF3=-s` | `iperf3 -c <BOARD_IP> -u -b 30M -i 1 -t 10 -l 1460` |

`AT+IPERF3=stop` ends a test still running on the board.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
RTL8735C
