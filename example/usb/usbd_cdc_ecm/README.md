# Example Description

In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.

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

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ECM-I] ECM demo start
	```

2. Connect the USB port of Ameba board to USB host (Another Ameba board as USB CDC ECM host) with USB cable.

3. If the test run success, the console will show:
	```
	Device IP: 192.168.45.1
	```


4. After get the ip address, type command "AT+PING=XX", XX is the ECM Host Ip Address, make sure the ping success and no packet losted

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
