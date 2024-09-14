##################################################################################
#                                                                                #
#                             Example example_usbd_cdc_acm                       #
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
        In this application, Ameba is designed as an USB CDC ACM device, USB host can recognize Ameba as a virtual serial
        port and communicate with it via serial port tools.
    
Setup Guide
~~~~~~~~~~~
		Taking AmebaSmart in GCC environmnet for example.

		To run USB CDC ACM device application on AP:
        1. Type command "make menuconfig" under amebasmart_gcc_project and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] CDC ACM
           Save and exit.
        2. Type command "make all EXAMPLE=usbd_cdc_acm" under amebasmart_gcc_project to build the images.
        3. Download the generated images under amebasmart_gcc_project to Ameba board.
        4. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
			[ACM] USBD CDC ACM demo start
        5. Connect the USB port of Ameba board to USB host (PC or another Ameba board as USB CDC ACM host) with USB cable.
        6. Launch any serial port tool (e.g. Realtek Trace Tool), open the virtual serial port against the USB port of
           Ameba board, send messages to the Ameba board and the board will echo back the received messages.
		
		To run USB CDC ACM device application on HP:
        1. Type command "make menuconfig" under amebasmart_gcc_project and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] CDC ACM
           Save and exit.
        2. Type command "make all" under amebasmart_gcc_project and type "make EXAMPLE=usbd_cdc_acm" under amebasmart_gcc_project/project_hp to build the images.
        3. Download the generated images under amebasmart_gcc_project/project_hp to Ameba board.
        4-6. Same with steps for AP.

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
        Supported: AmebaD, AmebaSmart, AmebaDplus, RTL8720F
        Source code not in project: Ameba-1, Ameba-z, Ameba-pro

