# Example Description

In this application, Ameba is designed as an USB CDC ACM device, USB host can recognize Ameba as a virtual serial port and communicate with it via serial port tools.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

To run USB CDC ACM device application on AP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
			*** USB Device Class ***
	[*] 	CDC ACM
	```
	Save and exit.

2. How to use:
   - Run `./build.py -a usbd_cdc_acm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB CDC ACM device application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	CDC ACM
	```
	Save and exit.

2. How to use:
   - Run `./build.py --app-for-km4 usbd_cdc_acm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ACM] USBD CDC ACM demo start
	```

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB CDC ACM host) with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages.

# Note

For other chips, refer to the AN for setup guide.

Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please modify the PID/VID value in this INF file to sync with CDC ACM class code:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
[DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
```

# Supported IC

- AmebaSmart
- AmebaDplus
