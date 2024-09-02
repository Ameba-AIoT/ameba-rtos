#                                                                                #
#                           example_wifi_event                                   #
#                                                                                #
##################################################################################

Date: 2024-08-28

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
        Using the most common event WIFI_EVENT_JOIN_STATUS to show how to use wifi event.
	
Setup Guide
~~~~~~~~~~~
        1.GCC:use CMD "make all EXAMPLE=wifi_event" to compile wifi_user_reconnect example.

Result description
~~~~~~~~~~~~~~~~~~
       1. Use AT command to do wifi connect:
        a. Will show join success log if use correct connect information.
        b. Will show join fail log if use incorrect connect information.

       2. Use different way to do wifi disconnect to show different types of disconnect resaon:
        a. Use AT+WLDISCONN to do wifi disconnect
        b. Modify AP's setting
        c. Power off AP

Supported List
~~~~~~~~~~~~~~
		AmebaDplus, AmebaSmart, AmebaLite