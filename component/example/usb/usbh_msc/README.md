# Example Description

In this application, Ameba is designed as an USB MSC host which can communicate with USB MSC device such as U-disk.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	MSC
	```
	- Choose `CONFIG VFS --->`:
	```
	[*] Enable VFS FATFS
	```
	Save and exit.

2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[MAIN-I] File System Init Success
	[MSC-I] USBH MSC demo start
	[MSC-I] Register USB disk
	[MSC-I] FatFS USB W/R performance test start...
	```

2. Attach an U disk formatted with FATFS to the USB port of the board with USB OTG cable.

3. Ameba board will recoganize the MSC device and do write/read performance test, check the log on the LOGUART console and make sure there is no USB related error reported. The example of expected PASS log:
	```
	[USBH-A] Device attached,speed 0
	[USBH-A] PID: 0x6545
	[USBH-A] VID: 0x0930
	[USBH-A] Address 1 assigned
	[USBH-A] MFG: TOSHIBA
	[USBH-A] PROD: TransMemory
	[USBH-A] SN: C03FD5F7715FE3417000DE76
	[USBH-A] Enum done, total 1 cfg
	[USBH-A] Switch to itf: 0
	[USBH-A] Class: 0x08
	[USBH-A] SubClass: 0x06
	[USBH-A] Protocol: 0x50
	[MSC-I] ATTACH
	[MSC-I] Max lun 1
	[MSC-I] Lun 0
	[MSC-I] SETUP
	[MSC-I] Open file: 0:/TEST0.DAT
	[MSC-I] W test: size 512, round 20...
	[MSC-I] W rate 204.0 KB/s for 20 round @ 49 ms
	[MSC-I] R test: size = 512 round = 20...
	[MSC-I] R rate 476.1 KB/s for 20 round @ 21 ms
	[MSC-I] W test: size 1024, round 20...
	[MSC-I] W rate 540.5 KB/s for 20 round @ 37 ms
	[MSC-I] R test: size = 1024 round = 20...
	[MSC-I] R rate 800.0 KB/s for 20 round @ 25 ms
	[MSC-I] W test: size 2048, round 20...
	[MSC-I] W rate 655.7 KB/s for 20 round @ 61 ms
	[MSC-I] R test: size = 2048 round = 20...
	[MSC-I] R rate 1212.1 KB/s for 20 round @ 33 ms
	[MSC-I] W test: size 4096, round 20...
	[MSC-I] W rate 1095.8 KB/s for 20 round @ 73 ms
	[MSC-I] R test: size = 4096 round = 20...
	[MSC-I] R rate 1600.0 KB/s for 20 round @ 50 ms
	[MSC-I] FatFS USB W/R performance test done
	[MSC-I] File close OK

	```

# Note

None

# Supported IC

- RTL872XD
- RTL8720F
- RTL8721F
- RTL8730E
- RTL8730F
