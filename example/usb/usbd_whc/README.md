# Example Description

In this application, Ameba is designed as an USB WHC device which can communicate with USB host like Linux PC.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	WHC
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console. The test PASSes only if the lines below show up and no error level log (`-E`) is reported:
	```
	[USBD-A] INIT
	[WHC-I] USBD WHC demo start
	[WHC-I] ATTACHED
	```
	Notes:
	* `[USBD-A] INIT` comes from the USB device core, not from the example.
	* `[WHC-I] ATTACHED` is printed once the USB host has enumerated the device, so it only shows up after the cable is connected.

2. Connect the USB port of Ameba board to USB host (with USB WHC driver installed) with USB cable.

3. USB host will then recognize Ameba board as WHC device and communicate as required. The data path prints no log.

4. Hotplug check: the example tears down and re-inits the USB stack on every detach, so unplug and re-plug the cable, following log shall be printed for each cycle:
	```
	[WHC-I] DETACHED
	[USBD-A] DEINIT
	[WHC-I] Free heap: 0x<value>
	[USBD-A] INIT
	[WHC-I] ATTACHED
	```
	The `Free heap` value shall stay stable across cycles, a value that keeps dropping indicates a memory leak.

# Note

Specific USB WHC driver is required for USB host to recoganize this USB WHC device, please refer to AN for details.

# Supported IC

RTL8721F
