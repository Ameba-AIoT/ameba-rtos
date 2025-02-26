# Example Description

In this application, Ameba is designed as an USB CDC ACM host which can communicate with specific USB CDC ACM device as defined in `usbd_cdc_acm(_new)` application.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

To run USB CDC ACM host application on AP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
			*** USB Host Class ***
	[*] 	CDC ACM
	```
	Save and exit.

2. How to use:
   - Run `./build.py -a usbh_cdc_acm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB CDC ACM host application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	CDC ACM
	```
	Save and exit.

2. How to use:
   - Run `./build.py --app-for-km4 usbh_cdc_acm` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[ACM] USBH CDC ACM demo start
	```

2. Connect the specific USB CDC ACM device (e.g. another Ameba board running usbd_cdc_acm_new application) to the USB port of the board with USB cable.

3. Reset and check the test log via LOGUART console, USB CDC ACM host log will be printed, make sure there is no error reported. If the test run success, the console will show:
	```
	[ACM] Ctrl test PASS
	[ACM] Bulk loopback test start, loopback times:100, size: 1024
	[ACM] Bulk loopback test PASS
	```

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
