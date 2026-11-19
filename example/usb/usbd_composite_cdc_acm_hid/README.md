# Example Description

In this application, Ameba is designed as a USB CDC ACM and HID composite device, USB host can recognize Ameba as a CDC virtual serial port and a HID mouse.

# HW Configuration

None

# SW configuration

1. Default configuration

	The `prj.conf` of this example sets the options below by default, no menuconfig is needed to run it:
	```
	CONFIG USB --->
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] CDC ACM
	[*] HID
			Select HID device type (Mouse)  --->
	```

2. Build

	```
	./ameba.py build -a usbd_composite_cdc_acm_hid -p
	```
	`-p` (pristine) drops the previous configuration and applies the `prj.conf` of this example.

3. (Optional) Change the configuration

	Only needed when the default configuration does not fit:
	1. Build once with `-p` as in step 2, so that the configuration starts from `prj.conf`.
	2. Run `./ameba.py menuconfig` and change the options.
	3. Rebuild without `-p`: `./ameba.py build -a usbd_composite_cdc_acm_hid`.

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
	[COMP-I] USBD ACM+HID comp demo start
	[USBD-A] INIT
	[COMP-I] ATTACHED
	```
	Notes:
	* `demo start` is printed before the stack is brought up; if any class or the composite framework fails to initialize, `demo aborted` is printed instead of the attach log.
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[COMP-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to PC with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages. The echo data path prints no log.

	The peer side of the ACM port (VID `0x0BDA`, PID of the SoC: `0x8730` RTL8730E, `0x8722` RTL8721Dx, `0xF851` RTL8721F, `0x8006` RTL8735C):
	* Linux host: the inbox `cdc_acm` driver creates `/dev/ttyACMx` (see `dmesg | grep ttyACM`). Stop ModemManager first (`sudo systemctl stop ModemManager`), then open it with `picocom -b 115200 /dev/ttyACM0`, every typed character shall come back.
	* Windows 10/11 host: the inbox `usbser` driver binds it, the port shows up as `USB Serial Device (COMx)` in Device Manager > Ports (COM & LPT).

4. Type following command from Ameba LOGUART console to send mouse data to PC:
	```
	# AT+USBDACMHID=mouse,<left>,<right>,<middle>,<x>,<y>,<wheel>
	```
	E.g., to move mouse cursor to right by 50 pixels:
	```
	# AT+USBDACMHID=mouse,0,0,0,50,0,0
	```
	Then the position of PC mouse pointer shall be changed accordingly, and following log is printed:
	```
	[COMP-I] Send mouse data
	```
	Check on the host side:
	* Windows: the device shows up as `HID-compliant mouse` in Device Manager > Mice and other pointing devices, and the cursor moves 50 pixels to the right.
	* Linux: the cursor moves as well; to see the events, run `sudo evtest`, select the event device of `Realtek ACM+HID Composite Device`, then `AT+USBDACMHID=mouse,0,0,0,50,0,0` shows a `REL_X` event of value 50, and `AT+USBDACMHID=mouse,1` followed by `AT+USBDACMHID=mouse,0` shows `BTN_LEFT` value 1 then 0.

5. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[COMP-I] DETACHED
	[USBD-A] DEINIT
	[COMP-I] Free heap: 0x<value>
	[USBD-A] INIT
	[COMP-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Shell commands

| Command | Description |
| --- | --- |
| `AT+USBDACMHID=help` | Print the usage, also when the example is not running. |
| `AT+USBDACMHID=mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]` | Send one HID mouse report to the host. Only `<left>` is mandatory, the remaining arguments default to 0. `<left>`/`<right>`/`<middle>`: `0` or `1` (pressed); `<x>`/`<y>`/`<wheel>`: -127..127, the range the mouse report descriptor declares. Any other value is rejected. |

Errors:
* `AT+USBDACMHID` without a parameter prints the help and ends with `ERROR: -2`.
* Any sub command other than `help` while the example is not running prints `[COMP-E] Not running` and ends with `ERROR: -7`.
* An unknown sub command prints `[COMP-E] Invalid cmd: <cmd>, try AT+USBDACMHID=help` and ends with `ERROR: -2`.
* An out of range or non numeric argument prints `[COMP-E] Invalid arg <i>: <s>` (`<i>` is the argument index, 1 for `<left>`), then `[COMP-E] Usage: AT+USBDACMHID=mouse,<left>[,<right>,<middle>,<x>,<y>,<wheel>]`, and ends with `ERROR: -2`. Too few or too many arguments print the usage line only.
* If the report can not be sent (e.g. the host has not configured the device yet), `[COMP-W] HID tx fail <ret>` is printed after `Send mouse data` and the command ends with `ERROR: <ret>`.

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
