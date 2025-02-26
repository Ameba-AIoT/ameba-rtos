# Example Description

In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

To run USB CDC ECM host application on AP:
1. Set up the test environment, the TOP is as below:
	```
	[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
	```

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
			*** USB Host Class ***
	[*] 	CDC ECM
				Select USB Ethernet (USB Ethernet)  --->
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbh_cdc_ecm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB CDC ECM host application on HP:
1. Set up the test environment, the TOP is as below:
	```
	[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
	```

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	CDC ECM
				Select USB Ethernet (USB Ethernet)  --->
	```
	Save and exit.

3. How to use:
   - Run `./build.py --app-for-km4 usbh_cdc_ecm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console and DHCP will start, make sure there is no USB related error reported:
	```
	[ECM] USB host cdc_ecm demo started...
	```

2. If the test run success, the console will show:
	```
	IP address : xx.xx.xx.xx
	Switch to link !!
	```

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
