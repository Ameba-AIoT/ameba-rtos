# Example Description

In this application, Ameba is designed as an USB CDC ECM device which can communicate with USB CDC ECM host (e.g., Ubuntu PC) and do DHCP test.

The topology:
```
[Ameba] ---USB Cable--- [USB CDC ECM Host]
```

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	CDC ECM
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[USBD-A] INIT
	[ECM-I] USBD CDC ECM demo start
	[ECM-I] Enter link status task!
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB host (Another Ameba board as USB CDC ECM host) with USB cable.

3. If the test run success, the console will show:
	```
	[ECM-I] Status change 0 -> 1
	[ECM-I] Attached
	[ECM-I] DHCP Server MAC: 00:e0:4c:xx:xx:xx
	[ECM-I] Device IP: 192.168.45.1
	[ECM-I] DHCP Server started
	```

4. After get the ip address, type command "AT+PING=XX", XX is the ECM Host Ip Address, make sure the ping success and no packet losted

5. The example also support iperf test, type "AT+IPERF=-s" to act as a TCP iperf server, type "AT+IPERF=-c,xx.xx.xx.x,-i,1,-t,10" to act as a TCP client. of course it also support UDP test, type "AT+IPERF" to get more information about the iperf

	Note: For detailed information about iperf and ping, please refer to https://aiot.realmcu.com/zh/latest/rtos/atcmd/at_command_network.html

6. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[ECM-I] Status change 1 -> 2
	[ECM-I] DETACHED
	[USBD-A] DEINIT
	[ECM-I] Free heap 0x<value>
	[USBD-A] INIT
	[ECM-I] Reinit done
	[ECM-I] Stopping USB ECM DHCP Server...
	[ECM-I] DHCP Server stopped
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Shell commands

| Command | Description |
| --- | --- |
| `usbd_ecm_link <0\|1>` | Report the upper-layer network link state to the host. `1` means link up, `0` means link down. The call is edge-triggered, repeating the same value is a no-op inside the class driver. |

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
