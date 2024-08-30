##################################################################################
#                                                                                #
#                             Example example_usbd_msc                            #
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
        In this application, Ameba is designed as an USB MSC (mass storage class) device with SD card as its physical memory media.
        USB host (e.g. PC or Ameba as USB MSC host) will recognize Ameba as MSC device and access the data on SD card via USB
        interface.
    
Setup Guide
~~~~~~~~~~~
		Taking AmebaSmart in GCC environmnet for example.

		To run USB MSC device application on AP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] MSC
           Save and exit.
        2. Type command "make all EXAMPLE=usbd_msc" under auto_build to build the images.
        3. Download the generated images under auto_build to Ameba board.
        4. Plugin Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
			[MSC] USBD MSC demo start
        5. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB MSC host) with USB cable.
        6. USB host will recognize Ameba board as MSC device, and the data on SD card can be accessed.
		
		To run USB MSC device application on HP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] MSC
           Save and exit.
        2. Type command "make all" under auto_build and type "make EXAMPLE=usbd_msc" under project_hp to build the images.
        3. Download the generated images under project_hp to Ameba board.
        4-6. Same with steps for AP.

        For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        None

Other Reference
~~~~~~~~~~~~~~~
        None

Supported List
~~~~~~~~~~~~~~
        Supported: Ameba-D2
        Source code not in project: Ameba-1, Ameba-z, Ameba-pro, Ameba-D, AmebaDplus, RTL8720F