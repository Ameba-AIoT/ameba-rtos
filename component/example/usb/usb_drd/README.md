# Example Description

In this application, Ameba is designed as an USB DRD (dual role device) which will switch role from MSC device to MSC host.

# HW Configuration

Some Ameba boards need to be reworked to support USB host, refer to AN for details.
Taking RTL8730EA/QFN100 boards for example, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS.

# SW configuration

1. Menuconfig
	Type command `./menuconfig.py` under the project directory:
	- Choose `CONFIG USB --->`:
	```
	[*] Enable USB
			USB Mode (DRD)  --->
			*** USB Device Class ***
	[*] 	MSC
			*** USB Host Class ***
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

1. Insert SD card to the SD slot of the board and reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[DRD-I] USB DRD demo start
	[DRD-I] Init disk
	[DRD-I] Init disk
	[MSC-I] SD init
	[SDH-I] Card Detect
	[SDH-I] Voltage match
	[SDH-I] This is a SDHC/SDXC card...
	[SDH-I] Keep 3.3V...
	[SDH-I] Manufacturer ID:3
	[SDH-I] OEM/Application ID:SD
	[SDH-I] Manufacturing date:2021/5
	[SDH-I] RCA = AAAA
	[SDH-I] CSD Version:2.0
	[SDH-I] User data area capacity: 14 GB
	[SDH-I] Max. read data block length: 512 Bytes
	[SDH-I] Max. write data block length: 512 Bytes
	[SDH-I] SD specification version: 3.0X
	[SDH-I] Data status after erase: 0
	[SDH-I] SD card changes to the specified speed mode successfully
	[SDH-I] SD card is initialized
	[DRD-I] Init device driver
	[USBD-A] INIT
	[DRD-I] Init MSC device class
	[MSC-I] Init
	[DRD-I] MSC device session start
	```

2. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.

3. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed within 20s until following log be printed on the LOGUART console:
	```
	[DRD-I] Role switch
	[DRD-I] Deinit MSC device class
	[DRD-I] Deinit device driver
	[USBD-A] DEINIT
	[DRD-I] Deinit disk
	[MSC-I] SD deinit
	[DRD-I] Init host driver
	[DRD-I] Init MSC host driver
	[DRD-I] MSC host session start
	[DRD-I] Register USB disk
	[DRD-I] FatFS USB W/R performance test start...
	```

4. Disconnect the USB port of Ameba board from USB host, connect it to a UDisk formatted with FATFS with OTG cable.

5. Ameba board will recoganize the MSC device and do write/read performance test, check the log on the LOGUART console and make sure there is no USB related error reported.
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
	[DRD-I] Host attach
	[MSC-I] Max lun 1
	[MSC-I] Lun 0
	[DRD-I] Host setup
	[DRD-I] Open file: 0:/TEST0.DAT
	[DRD-I] W test: size 512, round 20...
	[DRD-I] W rate 204.0 KB/s for 20 round @ 49 ms
	[DRD-I] R test: size = 512 round = 20...
	[DRD-I] R rate 476.1 KB/s for 20 round @ 21 ms
	[DRD-I] W test: size 1024, round 20...
	[DRD-I] W rate 540.5 KB/s for 20 round @ 37 ms
	[DRD-I] R test: size = 1024 round = 20...
	[DRD-I] R rate 800.0 KB/s for 20 round @ 25 ms
	[DRD-I] W test: size 2048, round 20...
	[DRD-I] W rate 655.7 KB/s for 20 round @ 61 ms
	[DRD-I] R test: size = 2048 round = 20...
	[DRD-I] R rate 1212.1 KB/s for 20 round @ 33 ms
	[DRD-I] W test: size 4096, round 20...
	[DRD-I] W rate 1095.8 KB/s for 20 round @ 73 ms
	[DRD-I] R test: size = 4096 round = 20...
	[DRD-I] R rate 1600.0 KB/s for 20 round @ 50 ms
	[DRD-I] FatFS USB W/R performance test done
	[DRD-I] File close OK
	```

# Note

The log in the expect result is only for reference.

# Supported IC

- RTL872XD
- RTL8721F
- RTL8730E
- RTL8730F
