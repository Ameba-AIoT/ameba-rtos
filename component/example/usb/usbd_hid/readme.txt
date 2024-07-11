##################################################################################
#                                                                                #
#                             Example example_usbd_hid                           #
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
        In this application, Ameba is designed as a USB HID mouse or keyboard device, PC will recognize Ameba as a mouse or keyboard.
        NOTE: Tested on Windows 7/10, MacOS, Ubuntu.
    
Setup Guide
~~~~~~~~~~~
		Taking AmebaSmart in GCC environmnet for example.

		To run USB HID device application on AP:
		1. Default the device acts as a HID mouse, if you want to change to HID keyboard, please modify USBD_HID_DEVICE_TYPE to
		   USBD_HID_KEYBOARD_DEVICE in component/usb/device/hid/usbd_hid.h
        2. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR AP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] HID
           Save and exit.
        3. Type command "make all EXAMPLE=usbd_hid" under auto_build to build the images.
        4. Download the generated images under auto_build to Ameba board.
        5. Reset the board, following log shall be printed on the LOGUART console, make sure there is no USB related error reported:
			[HID] USBD HID demo start
        6. Connect the USB port of Ameba board to PC with USB cable.
        7. Test with HID device:
           1) For HID mouse:
              If CONFIG_USBD_HID_CONSTANT_DATA is set to 1 (default), PC mouse cursor will move according to array mdata[] once Ameba board is connected to PC.
              If CONFIG_USBD_HID_MOUSE_CMD is set to 1 (default), type following command from Ameba LOGUART console to control the PC cursor behavior:
				# mouse <left> <right> <middle> <x_axis> <y_axis> <wheel>
		   2) For HID keyboard, key data "aA" will report to PC once Ameba board is connected, while type the leds key such as CAPsLock
		      and NumLock from PC, PC will send a message to the device.
		
		To run USB HID device application on HP:
        1. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR HP CONFIG => CONFIG USB:
			[*] Enable USB
			(Device) USB Mode
			[*] HID
           Save and exit.
        2. Type command "make all" under auto_build and type "make EXAMPLE=usbd_hid" under project_hp to build the images.
        3. Download the generated images under project_hp to Ameba board.
        4-7. Same with steps for AP.

        For other chips, refer to the AN for setup guide.

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		None

Other Reference
~~~~~~~~~~~~~~~
        None

Supported List
~~~~~~~~~~~~~~
        Supported: Ameba-D, Ameba-D2, Ameba-Dplus, RTL8720F
        Source code not in project: Ameba-1, Ameba-z, Ameba-pro