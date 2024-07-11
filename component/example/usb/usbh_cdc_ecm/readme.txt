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

	To run USB CDC ECM host application on HP:
	1. Set up the test environment, the TOP is as below:
		[Ameba] ---- [USB CDC ECM device] ---- [Router]
	2. Set the Processor Role to Single Core WiFi. Type command "make menuconfig" under auto_build and choose CONFIG WiFi:
		[*] Enable WiFi
		(Single_Core_WiFi) Processor Role
	Save and exit.
	3. Type command "make menuconfig" under amebasmart_gcc_project and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
	Save and exit.
	4. Type command "make all" under amebasmart_gcc_project and type "make EXAMPLE=usbh_cdc_ecm" under project_hp to build the images.
	5. Download the generated images under amebasmart_gcc_project/project_hp/asdk/image to Ameba board.
	6. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[ECM] USBH ECM demo start

	To run USB CDC ECM host application on AP:
	1. Type command "make menuconfig" under amebasmart_gcc_project and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
	Save and exit.
	2. Type command "make all" under amebasmart_gcc_project and type "make EXAMPLE=usbh_cdc_ecm" under amebasmart_gcc_project to build the images.
	3. Download the generated images under amebasmart_gcc_project to Ameba board.
	4. Reset the board, following log shall be printed on the LOGUART console and HDCP will start, make sure there is no
	USB related error reported:
		[CDC_ECM] USB host cdc_ecm demo started...

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
