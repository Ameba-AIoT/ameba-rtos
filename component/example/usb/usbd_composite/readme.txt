##################################################################################
#                                                                                #
#                             Example example_usbd_composite                     #
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
        In this application, Ameba is designed as a USB CDC ACM and HID composite device, USB host can recognize Ameba
        as a CDC virtual serial port and a HID mouse.

Setup Guide
~~~~~~~~~~~
		Taking AmebaSmart in GCC environmnet for example.

		To run USB composite device application on AP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
              [*] Enable USB
              (Device) USB Mode
              [*] Composite
           Save and exit.
        2. Type command "make all EXAMPLE=usbd_composite" under auto_build to build the images.
        3. Download the generated images under auto_build to Ameba board.
        4. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
			[COMP] USBD COMP demo start
        5. Connect the USB port of Ameba board to PC with USB cable.
        6. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of
           Ameba board, send messages to the Ameba board and the board will echo back the received messages.
        7. Type following command from Ameba LOGUART console to send mouse data to PC:
			# mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>
		   E.g., to move mouse cursor to right by 50 pixels:
			# mouse 0 0 0 50 0 0
		   Then the position of PC mouse pointer shall be changed accordingly.
		
		To run USB CDC ACM device application on HP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] Composite
           Save and exit.
        2. Type command "make all" under auto_build and type "make EXAMPLE=usbd_composite" under project_hp to build the images.
        3. Download the generated images under project_hp to Ameba board.
        4-7. Same with steps for AP.

        For other chips, refer to the AN for setup guide.

		Specially for Win7/XP host, please manually install the CDC ACM driver RtkUsbCdcAcmSetup.INF, and before the installation, 
		please modify the PID/VID value in this INF file to sync with CDC ACM class code:
			[DeviceList]
			%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730
			[DeviceList.NTamd64] 
			%DESCRIPTION%=DriverInstall, USB\VID_0BDA&PID_8730

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        None

Other Reference
~~~~~~~~~~~~~~~
        None

Supported List
~~~~~~~~~~~~~~
        Supported: Ameba-D2, Ameba-Dplus
        Source code not in project: Ameba-1, Ameba-z, Ameba-pro, RTL8720F
