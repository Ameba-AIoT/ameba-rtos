##################################################################################
#                                                                                #
#                             Example example_usbh_cdc_ecm                       #
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
	In this application, Ameba is designed as an USB CDC ECM host which can communicate with USB CDC ECM device and do DHCP test.

Setup Guide
~~~~~~~~~~~
	Taking AmebaSmart in GCC environmnet for example.

	Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100
	boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device
	via VBUS. Please refer to USB AN for details.

	To run USB CDC ECM host application on AP:
	1. Set up the test environment, the TOP is as below:
		[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
	2. Type command "menuconfig.py" under amebasmart_gcc_project and choose MENUCONFIG FOR CA32 CONFIG => CONFIG USB
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
		(Host) Select USB Eternet(USB Ethernet)
	Save and exit.
	3. Type command "./build.py -a usbh_cdc_ecm" under amebasmart_gcc_project to build the images.
	4. Download the generated images under amebasmart_gcc_project to Ameba board.
	5. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[ECM] USB host cdc_ecm demo started...


	To run USB CDC ECM host application on HP:
	1. Set up the test environment, the TOP is as below:
		[Ameba] ---USB Cable--- [USB CDC ECM device] ---RJ45--- [Router]
	2. Type command "menuconfig.py" under amebasmart_gcc_project and choose CONFIG Application Processor => Select AP Core
		Select AP Core(KM4)
		choose MENUCONFIG FOR KM4 CONFIG => CONFIG USB
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
		(Host) Select USB Eternet(USB Ethernet)
	Save and exit.
	3. Type command "./build.py --app-for-km4 usbh_cdc_ecm" under amebasmart_gcc_project to build the images.
	4. Download the generated images under amebasmart_gcc_project to Ameba board.
	5. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[ECM] USB host cdc_ecm demo started...


	For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	None.

Result description
~~~~~~~~~~~~~~~~~~
	On the serial console, USB CDC ECM host log will be printed and the DHCP test will be started,
		make sure there is no error reported. if the test run success, the console will show:
			IP address : xx.xx.xx.xx
			Switch to link !!

Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported : AmebaD, AmebaSmart
	Source code not in project: Ameba1, AmebaZ, AmebaPro, AmebaDplus, RTL8720F
