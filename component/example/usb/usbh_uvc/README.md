# Example Description

In this application, Ameba is designed as an USB UVC host which can communicate with specific USB MJPEG camera.

# HW Configuration

Attach an USB camera to the USB port of the Ameba board.

# SW configuration

Taking AmebaSmart in GCC environmnet for example.

Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100 boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device via VBUS. Please refer to USB AN for details.

To run USB UVC host application on AP:
1. Configure `CONFIG_USBH_UVC_APP` in `example_usbh_uvc.c` for different test scenarios:
   - `USBH_UVC_APP_SIMPLE`: Capture the frame and abandon it
   - `USBH_UVC_APP_VFS`: Capture the frame and save it to filesystem. User need to configure VFS correctly refer to VFS example or document.
   - `USBH_UVC_APP_HTTPC`: Capture the frame and save it to http server.

2. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR CA32 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
			*** USB Host Class ***
	[*] 	UVC
	```
	Save and exit.

3. How to use:
   - Run `./build.py -a usbh_uvc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

To run USB UVC host application on HP:
1. Type command `./menuconfig.py` under `amebasmart_gcc_project/` and choose `MENUCONFIG FOR KM4 CONFIG` -> `CONFIG USB`:
	```
	[*] Enable USB
			USB Mode (Host)  --->
	[*] 	UVC
	```
	Save and exit.

3. How to use:
   - Run `./build.py --app-for-km4 usbh_uvc` under `amebasmart_gcc_project/` to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
	```
	[UVC] USBH UVC demo start
	```

2. Ameba board will recoganize the USB camera and start to capture MJPEG frames at the rate of one frame per second.

3. Once attach uvc camera to host, host will capture 200 images cyclically. Following log shall be printed on the LOGUART console:
	```
	[UVC] USB host uvc demo started...
	[MAIN-I] KM4 START SCHEDULER 
	[USB-I] USB configurations:
	[USB-I] * pipes: 5
	[USB-I] * speed: 0
	[USB-I] * dma_enable: 1
	[USB-I] * main_task_priority: 3
	[USB-I] * isr_task_priority: 4
	[USB-I] * alt_max: 25
	[USB-I] * ptx_fifo_first: 1
	[USB-I] GSNPSID = 0x4F54310A
	UVC init
	[USB-I] USB Device Attached
	[USB-I] PID: 0x0530
	[USB-I] VID: 0x27C2
	[USB-I] Address (#1) assigned
	[USB-I] Manufacturer: USB FHD Camera
	[USB-I] Product: USB FHD Camera
	[USB-I] Serial Number: 2399_2093_IQ306_v47_still
	[USB-I] Enumeration done, total 1 configuration(s)
	[USB-I] Default configuration set
	[UVC-I] bDescriptorType: 11
	[USB-I] 14 class started
	UVC attach

	Set UVC parameters

	UVC para: 1280*720@30fps
	MJPEG Stream

	UVC stream on
	Get frame 0

	Capture uvc data, len=124008
	Put frame
	Get frame 1

	Capture uvc data, len=43085
	Put frame
	Get frame 2

	Capture uvc data, len=44906
	Put frame
	Get frame 3

	Capture uvc data, len=44879
	Put frame
	Get frame 4
	...
	```

# Note

For other chips, refer to the AN for setup guide.

# Supported IC

- AmebaSmart
