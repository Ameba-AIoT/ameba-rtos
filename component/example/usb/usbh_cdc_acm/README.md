# Example Description

In this application, Ameba is designed as an USB CDC ACM host which can communicate with specific USB CDC ACM device as defined in `usbd_cdc_acm` application.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Menuconfig
	Use menuconfig and choose `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] CDC ACM
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ACM-I] USBH CDC ACM demo start
	```

2. Connect the specific USB CDC ACM device (e.g. another Ameba board running usbd_cdc_acm_new application) to the USB port of the board with USB cable.

3. Reset and check the test log via LOGUART console, USB CDC ACM host log will be printed, make sure there is no error reported. If the test run success, the console will show:
	```
	[ACM-I] Ctrl test PASS
	[ACM-I] Bulk loopback test start, loopback times:100, size: 1024
	[ACM-I] Bulk loopback test PASS
	```

# Note

None

# Supported IC

RTL8730E
RTL8721F
