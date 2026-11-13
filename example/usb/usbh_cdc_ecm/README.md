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

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	CDC ECM
				Select USB Ethernet (USB Ethernet)  --->
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Set up the test environment as the topology.

2. Reset the board, following log shall be printed on the LOGUART console and DHCP will start, make sure there is no USB related error reported:
	```
	[ECM-I] USBH CDC ECM demo start
	```

3. If the test run success, the console will show:
	```
	got ip : xx.xx.xx.xx
	Switch to link
	```

4. After get the ip address, type command "AT+PING=XX", XX is the Router Ip Address, make sure the ping success and no packet losted

5. The example also supports throughput tests with iperf, see [Throughput test](#throughput-test).

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

Replace `<PC_IP>` with the PC address and `<BOARD_IP>` with the address printed by `got ip`.

| Test | Board | PC |
| --- | --- | --- |
| TCP, board sends | `AT+IPERF3=-c,<PC_IP>,-i,1,-t,10,-l,1460` | `iperf3 -s` |
| TCP, board receives | `AT+IPERF3=-s` | `iperf3 -c <BOARD_IP> -i 1 -t 10 -l 1460` |
| UDP, board sends | `AT+IPERF3=-c,<PC_IP>,-u,-b,40M,-i,1,-t,10,-l,1460` | `iperf3 -s` |
| UDP, board receives | `AT+IPERF3=-s` | `iperf3 -c <BOARD_IP> -u -b 30M -i 1 -t 10 -l 1460` |

`AT+IPERF3=stop` ends a test still running on the board.
