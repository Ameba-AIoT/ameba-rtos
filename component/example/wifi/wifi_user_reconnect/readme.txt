#                                                                                #
#                           example_wifi_user_reconnect                          #
#                                                                                #
##################################################################################

Date: 2024-08-22

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
        This example shows how to customize wifi reconnect task.
	
Setup Guide
~~~~~~~~~~~
		
		1.Set test_ssid and test_password in example_wifi_user_reconnect.c.

        2.GCC:use CMD "make all EXAMPLE=wifi_user_reconnect" to compile wifi_user_reconnect example.

Result description
~~~~~~~~~~~~~~~~~~
       The example thread will connect test_ap after wifi inited, and do auto reconnect after disconnected.

Supported List
~~~~~~~~~~~~~~
		AmebaDplus, AmebaSmart, AmebaLite