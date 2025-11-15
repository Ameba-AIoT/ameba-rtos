# Example Description

In this application, Ameba is designed as bridge which supports USB CDC ECM device to communicate with router
The topology:
```
[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
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
				Select USB Ethernet(USB Ethernet Bridge)  --->
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Set up the test environment as the topology.

2. Reset the board, following log shall be printed on the LOGUART console and DHCP will start, make sure there is no USB related error reported:
	```
	[ECM] USB host usbh_wifi_bridge demo started...
	```

3. Type command "AT+WLCONN=ssid,xx,pw,yy" to connect to the router, xx is the routr ssid name,yy is the password,you can type command "AT+WLCONN" to get more details about the command.

4. After conneting to the route by wifi, "other device" will get a valid ip address, after "other device" gets the valid ip address,  type command "ping=XX" in "other device", XX is the Router Ip Address, make sure the ping success and no packet losted

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
  * RTL8730E
