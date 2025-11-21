# Example Description

In this application, Ameba is designed as an USB vendor specific device which can only communicate with the USB vendor specific USB host as defined in `usbh_vendor` application.

Currently it is only for RD internal debug usage and not available as default.

# HW Configuration

None

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory and choose `CONFIG USB`:
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

1. Plugin Reset the board, following log shall be printed on the LOGUART console:
	```
	[VND-I] USBD vendor demo start
	```

2. Connect the USB port of Ameba board to USB vendor host (another Ameba board as USB vendor host) with USB cable.

3. Reset and check the log via LOGUART console, make sure there is no error reported, the test result will be printed on the LOGUART console of USB vendor host, refer to the `README.md` of USB vendor host for details.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
  * RTL8730E
- AmebaDplus
  * RTL8721Dx
- AmebaGreen2
  * RTL8721F
