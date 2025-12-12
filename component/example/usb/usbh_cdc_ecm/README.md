# Example Description

In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.
The topology:
```
[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
```

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
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
