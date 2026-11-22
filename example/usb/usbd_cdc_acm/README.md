# Example Description

In this application, Ameba is designed as an USB CDC ACM device, USB host can recognize Ameba as a virtual serial port and communicate with it via serial port tools.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	CDC ACM
	```

2. Build

	```
	./ameba.py build -a usbd_cdc_acm -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_cdc_acm`.

	To keep a change permanently, add its `CONFIG_XXX=y` lines to `prj.conf` instead and build with `-p`.

4. Download

	Flash the images to the board with any of the tools below:
	* VS Code extension: https://aiot.realmcu.com/en/latest/tools/vscode/index.html
	* Image Tool (Windows): https://aiot.realmcu.com/en/latest/tools/image_tool/index.html
	* Flash Tool (cross-platform): https://aiot.realmcu.com/en/latest/tools/flash/index.html
	* Web flash tool: https://aiot.realmcu.com/en/resource/web_flash_tool.html

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[ACM-I] USBD CDC ACM demo start
	[USBD-A] INIT
	[ACM-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[ACM-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB CDC ACM host) with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages. The echo data path prints no log.

	The device enumerates with VID `0x0BDA` and the PID of the SoC: `0x8730` (RTL8730E), `0x8722` (RTL8721Dx), `0xF851` (RTL8721F), `0x8006` (RTL8735C). The peer side can be one of:
	* Linux host: the inbox `cdc_acm` driver binds it, `dmesg` shows the port name:
		```
		dmesg | grep ttyACM
		cdc_acm 1-1:1.0: ttyACM0: USB ACM device
		```
		Stop ModemManager first (`sudo systemctl stop ModemManager`), otherwise it probes the new port with AT commands. Then open the port, every typed character shall come back:
		```
		picocom -b 115200 /dev/ttyACM0
		```
		or check the echo with pyserial:
		```
		python3 -c "import serial; s=serial.Serial('/dev/ttyACM0', 115200, timeout=1); s.write(b'hello'); print(s.read(5))"
		b'hello'
		```
		The baud rate is not used by the USB link, any value works.
	* Windows 10/11 host: the inbox `usbser` driver binds it, the device shows up as `USB Serial Device (COMx)` in Device Manager > Ports (COM & LPT); open `COMx` with the serial port tool.
	* Another Ameba board running the `usbh_cdc_acm` example, see its README.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[ACM-I] DETACHED
	[USBD-A] DEINIT
	[ACM-I] Free heap: 0x<value>
	[USBD-A] INIT
	[ACM-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

Windows 10/11 needs no driver. Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, please make sure the VID/PID value used in CDC ACM class is included in the INF file:
```
[DeviceList]
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
 [DeviceList.NTamd64] 
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8720
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8721
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8722
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8006
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8061
%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_F851
; Add support for new VID/PID
```

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
