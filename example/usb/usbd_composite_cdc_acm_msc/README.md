# Example Description

In this application, Ameba is designed as a USB CDC ACM and MSC composite device, USB host can recognize Ameba as a CDC virtual serial port and an UDisk.

# HW Configuration

Attach SD card to the SDIOH slot on the board.

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
		*** USB Device Global Settings ***
	[*] Composite function enable
		*** USB Device Class Selection ***
	[*] CDC ACM
	[*] MSC
			Select storage media (RAM)  --->
	```
	Save and exit.

	As a USB storage device, Ameba supports flexible access and management of various storage media, which can be configured as:

	* RAM
	* SD Card (SD Mode: using SDIO interface)
	* Second Flash

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[COMP-I] USBD ACM+MSC comp demo start
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[COMP-I] USBD ACM+MSC comp demo ready
	[COMP-I] ATTACHED
	```
	Notes:
	* `demo start` is printed before the stack is brought up, `demo ready` is printed after both classes and the composite framework have been initialized, so `demo ready` is the PASS criterion.
	* `[USBD-A] INIT` comes from the USB device core and `[MSC-I] Disk init` / `[MSC-I] Init` come from the MSC class driver, not from the example. `Disk init` is only printed when the storage media is SD card or second flash, a RAM disk build does not print it.
	* `[COMP-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to PC with USB cable.

3. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of Ameba board, send messages to the Ameba board and the board will echo back the received messages. The echo data path prints no log.

4. Also USB host will recognize Ameba board as an UDisk, and the data on SD card can be accessed. The storage read/write data path prints no log.

5. Hotplug check: the example tears down and re-inits the whole composite stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[COMP-I] DETACHED
	[MSC-I] Deinit
	[USBD-A] DEINIT
	[COMP-I] Free heap: 0x<value>
	[MSC-I] Disk init
	[USBD-A] INIT
	[MSC-I] Init
	[COMP-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

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
RTL8721F
RTL8735C
