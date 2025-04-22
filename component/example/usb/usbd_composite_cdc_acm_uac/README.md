# Example Description

In this application, Ameba is designed as a USB CDC ACM and HID composite device, USB host can recognize Ameba as a CDC virtual serial port and a HID mouse.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and :
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] Composite
			Select Composite Class (CDC ACM + UAC)  --->
	- Choose UAC Version 1.0 or 2.0 :
	(X) CDC ACM + UAC
			Select UAC Version (UAC 2.0)  --->
	```
	- Choose `CONFIG APPLICATION --->` -> `Audio Config --->`:
	```
	[*] Enable Audio Framework
			Select Audio Interfaces (Mixer)  --->
	```
	Save and exit.

2. Build
	Type command `./build.py -a usbd_composite_cdc_acm_uac` under the project directory to build images.

3. Download
	Download images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console:
	```
	[COMP-I] USBD COMP demo start
	```

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages.

4. Play music on a PC, if you connect a speaker to the board, sound will come out.

5. Reset and check the log via LOGUART console, make sure there is no error reported.

# Note

Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please make sure the VID/PID value used in CDC ACM class is included in the INF file:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
; Add support for new VID/PID
 [DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
; Add support for new VID/PID
```

# Supported IC

- RTL8721Dx
- RTL8730E