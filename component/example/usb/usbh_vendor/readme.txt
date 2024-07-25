##################################################################################
#                                                                                #
#                             Example example_usbh_vendor                        #
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
	In this application, Ameba is designed as a vendor-specific USB host as a design reference for users to design 
	customized USB applications. The USB vendor-specific host can communicate with vendor-specific USB device as
	defined in usbd_vendor_new application.
    
Setup Guide
~~~~~~~~~~~
	Taking AmebaSmart in GCC environmnet for example.

	Firstly, please make sure USB host mode is supported by the Ameba board hardware, specially for RTL8730EA/QFN100
	boards, the register R20 shall be parted on with 0Ohm register or just shorted to provide power for USB device
	via VBUS. Please refer to USB AN for details.

	To run vendor-specific USB host application on AP:
	1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] Vendor
	Save and exit.
	2. Type command "make all EXAMPLE=usbh_vendor" under auto_build to build the images.
	3. Download the generated images under auto_build to Ameba board.
	4. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
		[VND] USBH vendor demo start
	5. Connect the vendor-specific USB device (e.g. another Ameba board running usbd_vendor_new application) to the USB port of
	the board with USB cable.
	6. Reset and check the test log via LOGUART console, make sure there is no error reported and the transfer success rate is
	reasonable, e.g.:
		INTR test result: IN/OUT = 100/100:
		0   1   2   3   4   5   6   7   8   9
		...
		90  91  92  93  94  95  96  97  98  99
	 		
		ISOC test result: IN/OUT = 90/100:
		0   1   2   3   4   5   6   7   8   9
		...
		87  88  90  91  92  93  95  96  97  99
		
	To run vendor-specific USB host application on HP:
	1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
		[*] Enable USB
		(Host) USB Mode
		[*] Vendor
	Save and exit.
	2. Type command "make all" under auto_build and type "make EXAMPLE=usbh_vendor" under project_hp to build the images.
	3. Download the generated images under project_hp to Ameba board.
	4-6. Same with steps for AP.

	For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	None.

Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported: Ameba-D, Ameba-D2
	Source code not in project: Ameba-1, Ameba-z, Ameba-pro, Ameba-Dplus, RTL8720F