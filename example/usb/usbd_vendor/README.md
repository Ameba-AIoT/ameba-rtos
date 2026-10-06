# Example Description

In this application, Ameba is designed as an USB vendor specific device which can only communicate with the USB vendor specific USB host as defined in `usbh_vendor` application.

Currently it is only for RD internal debug usage and not available as default.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	Vendor
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[USBD-A] INIT
	[VND-I] USBD VENDOR demo start
	[VND-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[VND-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.
	* On AmebaGreen2 an extra `[USB-I] UPHY para from ...` line is printed by the USB HAL before `[USBD-A] INIT`.

2. Connect the USB port of Ameba board to USB vendor host (another Ameba board as USB vendor host) with USB cable.

3. Check the log via LOGUART console, make sure there is no error reported. The transfer threads are disabled by default, so the device side prints no further log, the test result is printed on the LOGUART console of the USB vendor host instead. Refer to the `README.md` of USB vendor host for details.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[VND-I] DETACHED
	[USBD-A] DEINIT
	[VND-I] Free heap: 0x<value>
	[USBD-A] INIT
	[VND-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

RTL8730E
RTL8721Dx
RTL8721F
RTL8735C
