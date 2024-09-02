##################################################################################
#                                                                                #
#                          example_wifi_scan_mcc                                 #
#                                                                                #
##################################################################################

Description
~~~~~~~~~~~
	This example performs scan which use partial scan and delay to make AP mode can send beacon during STA scan.


Setup Guide
~~~~~~~~~~~
	GCC:use CMD "make all EXAMPLE=wifi_scan_mcc" to compile wifi_scan_mcc example.

Notes
~~~~~~~~~~~
	Please adjust delay in wifi_scan_networks_mcc() accordingly to prevent fast_connect wifi scan conflicting with example_wifi_scan_mcc wifi scan

		/* Add 2s delay to prevent conflict with site_survey scan during fast connect */
		vTaskDelay(2000);

Result description
~~~~~~~~~~~~~~~~~~
	After compile&download success, the wifi_scan_mcc_thread will run when IC boot success.
	After a period of time, the scan results with per channel will be displayed, and during this period, the softap beacon can be captured by sniffer if alreay start softap mode.

	The example log is as follows,
		"	Current scan channel is 1
			Infra	 d8:ec:5e:7a:7e:0f -50	  1	   6	  Open		  linksys_2.4g
			Infra	 c8:3a:35:55:45:b3 -55	  1	   7	  Open		  Tenda_AX_2.4G
			Infra	 f4:2a:7d:2a:70:d7 -62	  1	   6	  WPA2 AES		  tplink_WAR1300
			Infra	 98:97:cc:35:d2:c5 -64	  1	   6	  Open		  TPLINK_XDR5430
			Infra	 48:0e:ec:9f:5e:60 -64	  1	   6	  WPA/WPA2 AES		  WN-BT3
			Infra	 3c:06:a7:03:41:19 -68	  1	   6	  WPA/WPA2 AES		  WDR5620_selftest_2.4G
			Total num of AP scanned = 6

			Current scan channel is 2
			Infra	 d8:ec:5e:7a:7e:0f -57	  1	   6	  Open		  linksys_2.4g
			Infra	 bc:5f:f6:15:8b:96 -60	  1	   6	  WPA/WPA2 AES		  MERCURY_8B97
			Infra	 a4:1a:3a:0d:0f:35 -64	  2	   6	  WPA/WPA2 AES		  TPLINK_0F35
			Total num of AP scanned = 3
		"
		
	notes: if surveying channel 2 can recv APs from  channel 1 or channel 3, it is ok, adjacent channels are allowed.
	       if sta is connected with a AP on channel 6. it is allowed to recv APs from channel 6 when surveying channel is not channel 6.
		   if sta is not connected with any AP, it is allowed to recv APs from channel 1 when surveying channel is not channel 1.	

Support List
~~~~~~~~~~~~~~
	Source code only support: Ameba - Lite / Smart / Dplus. 
