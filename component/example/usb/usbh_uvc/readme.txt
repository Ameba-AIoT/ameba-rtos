##################################################################################
#                                                                                #
#                             Example example_usbh_uvc                           #
#                                                                                #
##################################################################################

Date: 2024-1-16

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Other Reference
 - Supported List

 
Description
~~~~~~~~~~~
	In this application, Ameba is designed as an USB UVC host which can communicate with specific USB MJPEG camera.
    
Setup Guide
~~~~~~~~~~~
	Taking AmebaSmart in GCC environmnet for example.

	Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100
	boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device
	via VBUS. Please refer to USB AN for details.

	To run USB UVC host application on AP:
	1. Attach an USB camera to the USB port of the Ameba board.
	2. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] UVC
	Save and exit.
	3. Type command "make all EXAMPLE=usbh_uvc" under auto_build to build the images.
	4. Download the generated images under auto_build to Ameba board.
	5. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related
	error reported:
		[UVC] USBH UVC demo start
	6. Ameba board will recoganize the USB camera and start to capture MJPEG frames at the rate of one frame per second.
		
	To run USB UVC host application on HP:
	1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] UVC
	Save and exit.
	2. Type command "make all" under auto_build and type "make EXAMPLE=usbh_uvc" under project_hp to build the images.
	3. Download the generated images under project_hp to Ameba board.
	4-6. Same with steps for AP.

	For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Configure CONFIG_USBH_UVC_APP in example_usbh_uvc.c for different test scenarios:
	* USBH_UVC_APP_SIMPLE: Capture the frame and abandon it
	* USBH_UVC_APP_VFS: Capture the frame and save it to filesystem. User need to configure VFS correctly refer to VFS 
	example or document.
	* USBH_UVC_APP_HTTPC: Capture the frame and save it to http server.

Result description
~~~~~~~~~~~~~~~~~~
	once attach uvc camera to host, host will capture 200 images cyclically. following log shall be printed on the LOGUART console:
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


Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported : Ameba-D, Ameba-D2
	Source code not in project: Ameba-1, Ameba-z, Ameba-pro, Ameba-Dplus, RTL8720F
