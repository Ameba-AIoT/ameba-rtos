##################################################################################
#                                                                                #
#                           example_wifi_roaming								 #
#                                                                                #
##################################################################################

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
        This example realizes wifi roaming among local area network with the same SSID. 

Setup Guide
~~~~~~~~~~~
	1.set wifi_fast_connect_enable(1) in wlan_initialize()
	2.Set the RSSI_THRESHOLD in "example_wifi_roaming.c" according to the WLAN envrionment.
	3.GCC:use CMD "make all EXAMPLE=wifi_roaming" to compile wifi_roaming example.

Result description
~~~~~~~~~~~~~~~~~~
	1.The device can connect to a better RSSI AP and store the connection message to flash.
	2.If the RSSI of connected AP is lower than "RSSI_THRESHOLD", the roaming decision is triggered to begin to detect whether there is a better AP.
	3.If there is an alternative AP with a RSSI higher than already connected AP, it is suggested to switch to that AP.
	4.If the RSSI of connected AP is already stronger than "RSSI_THRESHOLD", then it will not automatically switch to even better AP unless disconnect or reboot from current AP connection.
Supported List
~~~~~~~~~~~~~~
[Supported List]
	Supported :
	    RTL8730A, RTL872XE