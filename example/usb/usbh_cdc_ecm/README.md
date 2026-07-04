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
	[ECM] USB host cdc_ecm demo started...
	```

3. If the test run success, the console will show:
	```
	got ip : xx.xx.xx.xx
	Switch to link !!
	```

4. After get the ip address, type command "AT+PING=XX", XX is the Router Ip Address, make sure the ping success and no packet losted

5. The example also support iperf test, type "AT+IPERF=-s" to act as a TCP iperf server, type "AT+IPERF=-c,xx.xx.xx.x,-i,1,-t,10" to act as a TCP client. of course it also support UDP test, type "AT+IPERF" to get more information about the iperf

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721F
