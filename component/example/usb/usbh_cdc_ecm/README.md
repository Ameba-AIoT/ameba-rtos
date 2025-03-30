# Example Description

In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.

# HW Configuration

None

# SW configuration

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

1. Set up the test environment, the TOP is as below:
	```
	[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
	```

2. Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	CDC ECM
				Select USB Ethernet (USB Ethernet)  --->
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbh_cdc_ecm` under the project directory to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console and DHCP will start, make sure there is no USB related error reported:
	```
	[ECM] USB host cdc_ecm demo started...
	```

2. If the test run success, the console will show:
	```
	got ip : xx.xx.xx.xx
	Switch to link !!
	```

3. After get the ip address, type command "AT+PING=XX", XX is the Router Ip Address, make sure the ping success and no packet losted
4. The example also support iperf test, type "AT+IPERF=-s" to act as a TCP iperf server, type "AT+IPERF=-c,xx.xx.xx.x,-i,1,-t,10" to act as a TCP client. of course it also support UDP test, type "AT+IPERF" to get more information about the iperf

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- RTL8730E
- RTL872XD
