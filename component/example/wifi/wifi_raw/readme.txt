##################################################################################
#                                                                                #
#                           example_wifi_raw                                 #
#                                                                                #
##################################################################################

Date: 2023 - 06 - 20

Table of Contents
~~~~~~~~~~~~~~~~~
- Description
- Setup Guide
- Result description
- Supported List


Description
	~~~~~~~~~~~
	This example shows how to send probe_req and receive probe_rsp packets.

Setup Guide
	~~~~~~~~~~~
	(1) Change addr1, addr2 and addr3 as required.
		u8 addr1[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 1, should be the mac address of destination
		u8 addr2[6] = {0x00, 0xE0, 0x4C, 0x00, 0x02, 0xF0};		// Address 2, should be the mac address of myself as master
		u8 addr3[6] = {0xBC, 0x46, 0x99, 0x7B, 0x48, 0x74};		// Address 3, shoule be the BSSID
	
	(2)	GCC:use CMD "make all EXAMPLE=wifi_raw" to compile wifi_raw example. 

Result description
~~~~~~~~~~~~~~~~~~
	The example thread will be started automatically when booting.

Supported List
~~~~~~~~~~~~~~
	RTL872xE