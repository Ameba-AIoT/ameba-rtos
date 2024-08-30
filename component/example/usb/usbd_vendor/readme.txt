##################################################################################
#                                                                                #
#                             Example example_usbd_vendor                        #
#                                                                                #
##################################################################################

Date: 2023-11-23

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
        In this application, Ameba is designed as an USB vendor specific device which can only communicate with the USB vendor
        specific USB host as defined in usbh_vendor_new application.
    
Setup Guide
~~~~~~~~~~~
		Taking AmebaSmart in GCC environmnet for example.

		To run USB vendor device application on AP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] Vendor
           Save and exit.
        2. Type command "make all EXAMPLE=usbd_vendor" under auto_build to build the images.
        3. Download the generated images under auto_build to Ameba board.
        4. Plugin Reset the board, following log shall be printed on the LOGUART console:
			[VND] USBD vendor demo start
        5. Connect the USB port of Ameba board to USB vendor host (another Ameba board as USB vendor host) with USB cable.
        6. Reset and check the log via LOGUART console, make sure there is no error reported, the test result will be printed
           on the LOGUART console of USB vendor host, refer to the readme.txt of USB vendor host for details.
		
		To run USB MSC device application on HP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] Vendor
           Save and exit.
        2. Type command "make all" under auto_build and type "make EXAMPLE=usbd_vendor" under project_hp to build the images.
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
        Supported: AmebaD, AmebaSmart, AmebaDplus
        Source code not in project: Ameba-1, Ameba-z, Ameba-pro, RTL8720F