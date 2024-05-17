##################################################################################
#                                                                                #
#                           example_wifi_manager                                 #
#                                                                                #
##################################################################################

Date: 2019-11-22

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
        This example shows how to register wifi event callback function.
	
Setup Guide
~~~~~~~~~~~
		1.disable auto-reconnect in driver layer because we do reconnect in up layer.
		(for example:component\wifi\driver\include\rtw_autoconf.h)
		#define CONFIG_AUTO_RECONNECT 0

        2.GCC:use CMD "make all EXAMPLE=wifi_manager" to compile wifi_manager example.

Result description
~~~~~~~~~~~~~~~~~~
        When define CONFIG_EXAMPLE_WIFI_MANAGER, the callback functiona of wifi events(WIFI_EVENT_CONNECT, WIFI_EVENT_DISCONNECT) are automatically registered.

Supported List
~~~~~~~~~~~~~~
		RTL8730A, RTL872XE