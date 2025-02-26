# Example Description

In this application, Ameba is designed as an USB vendor specific device which can only communicate with the USB vendor specific USB host as defined in `usbh_vendor_new` application.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

To run USB vendor device application on AP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
			*** USB Device Class ***
	[*] 	Vendor
	```
	Save and exit.

2. How to use:
   - Run `./build.py -a usbd_vendor` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB vendor device application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Device)  --->
	[*] 	Vendor
	```
	Save and exit.

2. How to use:
   - Run `./build.py --app-for-km4 usbd_vendor` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console:
	```
	[VND] USBD vendor demo start
	```

2. Connect the USB port of Ameba board to USB vendor host (another Ameba board as USB vendor host) with USB cable.

3. Reset and check the log via LOGUART console, make sure there is no error reported, the test result will be printed on the LOGUART console of USB vendor host, refer to the `README.md` of USB vendor host for details.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
- AmebaDplus
