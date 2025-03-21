# Example Description

In this application, Ameba is designed as bridge which supports USB CDC ECM device to communicate with router

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

To run USB WIFI BRIDGE application on AP:
1. Set up the test environment, the TOP is as below:
	```
	[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
	```

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
			*** USB Host Class ***
	[*] 	CDC ECM
				Select USB Ethernet(USB Ethernet Bridge)  --->
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbh_wifi_bridge` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB WIFI BRIDGE application on HP:
1. Set up the test environment, the TOP is as below:
	```
	[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
	```

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	CDC ECM
				Select USB Ethernet(USB Ethernet Bridge)   --->
	```
	Save and exit.

3. How to use:
   - Run `./build.py --app-for-km4 usbh_wifi_bridge` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console and DHCP will start, make sure there is no USB related error reported:
	```
	[ECM] USB host usbh_wifi_bridge demo started...
	```

2. Type command "AT+WLCONN=ssid,xx,pw,yy" to connect to the router, xx is the routr ssid name,yy is the password,you can type command "AT+WLCONN" to get more details about the command.

3. After conneting to the route by wifi, "other device" will get a valid ip address, after "other device" gets the valid ip address,  type command "ping=XX" in "other device", XX is the Router Ip Address, make sure the ping success and no packet losted

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
