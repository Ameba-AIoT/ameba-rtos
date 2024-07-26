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

	To run USB WIFI BRIDGE application on HP:
	1. Set up the test environment, the TOP is as below:
		[other device] ---- [USB CDC ECM device] ---- [Ameba]
	2. Type command "make menuconfig" under auto_build and choose MENUCONFIG FOR KM4 CONFIG => CONFIG USB
		or choose MENUCONFIG FOR CA32 CONFIG => CONFIG USB :
		[*] Enable USB
		(Host) USB Mode
		[*] CDC ECM
		[*] Enable ethernet bridge
	Save and exit.
	3. Type command "make all" under auto_build and type "make EXAMPLE=usbh_wifi_bridge" to build the images.
	4. Download the generated images under auto_build to Ameba board.
	5. Reset the board, following log shall be printed on the LOGUART console and the download will start, make sure there is no
	USB related error reported:
		[CDC_ECM] USB host usbh_wifi_bridge demo started...

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	None.

Other Reference
~~~~~~~~~~~~~~~
	None.

Supported List
~~~~~~~~~~~~~~
	Supported : AmebaSmart
