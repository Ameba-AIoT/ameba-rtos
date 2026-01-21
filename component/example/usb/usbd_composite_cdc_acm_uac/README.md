# Example Description

In this application, Ameba is designed as a USB CDC ACM and UAC composite device, USB host can recognize Ameba as a CDC virtual serial port and a audio speaker.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and :
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
			Select Audio Interfaces (PassThrough)  --->

	Note: If the audio speaker is not very smooth, choose `Select Audio Interfaces (Mixer)`, check the audio composite note to get more details.
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console:
	```
	[COMP-I] USBD COMP demo start
	```

2. Connect the USB port of Ameba board to USB UAC host (for example PC) with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages.

4. After the USB Audio driver is successfully loaded, USB host end will recognize Ameba as an audio device.

5. Play music on a PC, if you connect a speaker to the board, sound will come out.
   ```
	- Open the OS Sound Bar, enable the Realtek Composite Device speaker device if it is disabled. Locate the device in the Device Manager and enable it manually.
	- Open the OS Sound Bar, set Realtek Composite Device Speaker as default device.
	- Open the Sound Control Panel, select the Realtek Composite Device Speaker device -- configure the speakers -- choose a specific channel setup -- finalize the configuration, and then click Test.
	- If you connect a speaker to the board, sound will play.
   ```

6. Reset and check the log via LOGUART console, make sure there is no error reported.

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

RTL8730E
RTL8721Dx
