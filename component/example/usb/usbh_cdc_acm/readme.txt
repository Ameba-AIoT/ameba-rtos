##################################################################################
#                                                                                #
#                             Example example_usbh_cdc_acm                       #
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
	In this application, Ameba is designed as an USB CDC ACM host which can communicate with specific USB CDC ACM device as 
	defined in usbd_cdc_acm(_new) application.
    
Setup Guide
~~~~~~~~~~~
	Taking AmebaSmart in GCC environmnet for example.

	Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100
	boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device
	via VBUS. Please refer to USB AN for details.

	To run USB CDC ACM host application on AP:
	1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ACM
	Save and exit.
	2. Type command "make all EXAMPLE=usbh_cdc_acm" under auto_build to build the images.
	3. Download the generated images under auto_build to Ameba board.
	4. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
		[ACM] USBH CDC ACM demo start
	5. Connect the specific USB CDC ACM device (e.g. another Ameba board running usbd_cdc_acm_new application) to the USB port of
	the board with USB cable.
	6. Reset and check the test log via LOGUART console, make sure there is no error reported.
		
	To run USB CDC ACM host application on HP:
	1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ACM
	Save and exit.
	2. Type command "make all" under auto_build and type "make EXAMPLE=usbh_cdc_acm" under project_hp to build the images.
	3. Download the generated images under project_hp to Ameba board.
	4-6. Same with steps for AP.

	For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	None.

Result description
~~~~~~~~~~~~~~~~~~
    On the serial console, USB CDC ACM host log will be printed
        make sure there is no error reported. if the test run success, the console will show:
        [ACM] Ctrl test PASS
        [ACM] Bulk loopback test start, loopback times:100, size: 1024
        [ACM] Bulk loopback test PASS

Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported : Ameba-D, Ameba-D2
	Source code not in project: Ameba-1, Ameba-z, Ameba-pro, AmebaDplus, RTL8720F
