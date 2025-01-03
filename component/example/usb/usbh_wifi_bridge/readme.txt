##################################################################################
#                                                                                #
#                             Example example_usbh_wifi_bridge                   #
#                                                                                #
##################################################################################

Date: 2024-1-30

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
	In this application, Ameba is designed as bridge which supports USB CDC ECM device to communicate with router

Setup Guide
~~~~~~~~~~~
	Taking AmebaSmart in GCC environmnet for example.

	Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100
	boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device
	via VBUS. Please refer to USB AN for details.

	To run USB WIFI BRIDGE application on AP:
	1. Set up the test environment, the TOP is as below:
		[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
	2. Type command "menuconfig.py" under amebasmart_gcc_project and choose MENUCONFIG FOR CA32 CONFIG => CONFIG USB
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
		(Host) Select USB Eternet(USB Ethernet Bridge)
	Save and exit.
	3. Type command "./build.py -a usbh_wifi_bridge" under amebasmart_gcc_project to build the images.
	4. Download the generated images under amebasmart_gcc_project to Ameba board.
	5. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[ECMB] USB host usbh_wifi_bridge demo started...


	To run USB WIFI BRIDGE application on HP:
	1. Set up the test environment, the TOP is as below:
		[Other device]---RJ45---[USB CDC ECM device]---USB Cable---[Ameba]---Wifi---[Router]
	2. Type command "menuconfig.py" under amebasmart_gcc_project and choose CONFIG Application Processor => Select AP Core
		Select AP Core(KM4)
		choose MENUCONFIG FOR KM4 CONFIG => CONFIG USB
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
		(Host) Select USB Eternet(USB Ethernet Bridge)
	Save and exit.
	3. Type command "./build.py --app-for-km4 usbh_wifi_bridge" under amebasmart_gcc_project to build the images.
	4. Download the generated images under amebasmart_gcc_project to Ameba board.
	5. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[ECMB] USB host usbh_wifi_bridge demo started...

	For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	None.

Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported : AmebaSmart
