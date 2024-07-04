##################################################################################
#                                                                                #
#                               example_wifi_cast                                #
#                                                                                #
##################################################################################

Date: 2023 - 04 - 26

Table of Contents
~~~~~~~~~~~~~~~~~
- Description
- Setup Guide
- Result description
- Supported List


Description
~~~~~~~~~~~
	This example shows how to use wifi cast.

Setup Guide
~~~~~~~~~~~
	GCC:use CMD "make all EXAMPLE=wifi_cast" to compile wifi_cast example(under ap project or single core project).

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	1. Enviornment setup
		Wifi cast test need at least two boards. One board for master node, and another board for slave node.
		
	2. Link lib_wifi_cast.a.
		--- a/amebasmart_gcc_project/project_ap/asdk/Makefile
		+++ b/amebasmart_gcc_project/project_ap/asdk/Makefile
		@@ -64,6 +64,7 @@ APP_LIB = $(ROOTDIR)/lib/application/lib_wps.a \
				$(ROOTDIR)/lib/application/lib_httpc.a \
				$(ROOTDIR)/lib/application/lib_httpd.a \
				$(ROOTDIR)/lib/application/lib_websocket.a \
		+       $(ROOTDIR)/lib/application/lib_wifi_cast.a \
				#$(ROOTDIR)/lib/application/lib_mdns.a \
				#$(ROOTDIR)/lib/application/lib_coap.a \
				#$(ROOTDIR)/lib/application/lib_eap.a \

	3. For master node:
		1) #define APMODE 1
		2) Add slave node and modify node mac address into mac address of slave node board.
			char s_node_mac[ETH_LEN] = {0x00, 0xe0, 0x4c, 0x87, 0x30, 0x01};
		3) Re-build project and download image to one board.

	4. For slave node:
		1) #define APMODE 0
		2) Add master node and modify node mac address into mac address of master node board.
			char m_node_mac[ETH_LEN] = {0x00, 0xe0, 0x4c, 0x00, 0x02, 0xD3};
		3) Re-build project and download image to another board.
	
	5. Set trx_mode for master node and slave mode, choose 1) or 2).
		1) set WIFI_CAST_TRX_UNICAST for master node, then set WIFI_CAST_TRX_MONITOR for slave node.
		2) set WIFI_CAST_TRX_BRODCAST for master node, then set WIFI_CAST_TRX_UNICAST for slave node.

	6. Reset master node and slave node, then start tests.

	7. For throughput test:
		1) type command "wifi_cast_test stop" on master node and slave node side respectively.
		2) type command "wifi_cast_test start 0 1800" on master node side to start throughput test.
		Note:
			1) Default bandwidth is 40Mbps, users can modify it in wifi_cast_send_task.
			2) For high throughput, users can modify skb_num_np and skb_num_ap in wifi_set_user_config().

Result description
~~~~~~~~~~~~~~~~~~
	After compile success, the wifi_cast_send_task may run on the master node side. When slave node connect to master 
	node success, master node will send data to slave node shown in trace tool or other uart tools at regular intervals.

Support List
~~~~~~~~~~~~~~
    Supported IC:
            AmebaSmart 