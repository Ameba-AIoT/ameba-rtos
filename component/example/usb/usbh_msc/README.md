# Example Description

In this application, Ameba is designed as an USB MSC host which can communicate with USB MSC device such as U-disk.

# HW Configuration

None

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

To run USB MSC host application on AP:
1. Make sure `FF_LFN_UNICODE` is set to 2 in `component/file_system/fatfs/<version>/include/ffconf.h` (default)

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
			*** USB Host Class ***
	[*] 	MSC
	```
	choose `CONFIG_VFS`:
	```
	[*] Enable VFS FATFS
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbh_msc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB MSC host application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	MSC
	```
	choose `CONFIG_VFS`:
	```
	[*] Enable VFS FATFS
	```
	Save and exit.

3. How to use:
   - Run `./build.py --app-for-km4 usbh_msc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[MSC] USBH MSC demo start
	```

2. Attach an U disk formatted with FATFS to the USB port of the board with USB OTG cable.

3. Ameba board will recoganize the MSC device and do write/read performance test, check the log on the LOGUART console and make sure there is no USB related error reported.

4. Before U disk attached, following log shall be printed on the LOGUART console:
	```
	[MAIN-I] File System Init Success
	[MSC] USB host MSC demo started...
	Register USB disk driver
	FATFS Register: disk driver 0
	FatFS USB Write/Read performance test started...
	```
	After U disk attached, following kind of log shall be printed on the LOGUART console:
	```
	[USB-I] USB Device Attached
	[USB-I] PID: 0x1666
	[USB-I] VID: 0x0951
	[USB-I] Address (#1) assigned
	[USB-I] Manufacturer: Kingston
	[USB-I] Product: DataTraveler 3.0
	[USB-I] Serial Number: E0D55EA574061750A94101FD
	[USB-I] Enumeration done, total 1 configuration(s)
	[USB-I] Default configuration set
	[USB-I] Switching to Interface (#0)
	[USB-I] Class: 0x08
	[USB-I] SubClass: 0x06
	[USB-I] Protocol: 0x50
	[MSC] ATTACH
	[USB-I] 8 class started
	[MSC] SETUP
	[MSC] open file path: 0:/TEST0.DAT
	[MSC] Write test: size = 512 round = 20...
	[MSC] Write rate 476.1 KB/s for 20 round @ 21 ms
	[MSC] Read test: size = 512 round = 20...
	[MSC] Read rate 500.0 KB/s for 20 round @ 20 ms
	[MSC] Write test: size = 1024 round = 20...
	[MSC] Write rate 175.4 KB/s for 20 round @ 114 ms
	[MSC] Read test: size = 1024 round = 20...
	[MSC] Read rate 833.3 KB/s for 20 round @ 24 ms
	[MSC] Write test: size = 2048 round = 20...
	[MSC] Write rate 1142.8 KB/s for 20 round @ 35 ms
	[MSC] Read test: size = 2048 round = 20...
	[MSC] Read rate 1212.1 KB/s for 20 round @ 33 ms
	[MSC] Write test: size = 4096 round = 20...
	[MSC] Write rate 1509.4 KB/s for 20 round @ 53 ms
	[MSC] Read test: size = 4096 round = 20...
	[MSC] Read rate 1600.0 KB/s for 20 round @ 50 ms
	[MSC] FatFS USB Write/Read performance test done
	[MSC] File close success
	```
	Otherwise, the result will be failed.

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
