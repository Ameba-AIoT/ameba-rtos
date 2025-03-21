##################################################################################
#                                                                                #
#                           ble_iso_demo                                         #
#                                                                                #
##################################################################################

Date: 2025 - 03 - 20

Setup Guide
~~~~~~~~~~~
GCC menuconfig 
~~~~~~~~~~~
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BLE ISO Test
2. GCC : use CMD "./build.py" to compile example

Test Guide
~~~~~~~~~~~
ATCMD :
1. CIS
1.1  CIS initiator:
	1.1.1 enable : 'AT+BTDEMO=iso,cis,initiator,1'
	1.1.2 disable : 'AT+BTDEMO=iso,cis,initiator,0'
	1.1.3 connect 'AT+BLEGAP=conn,0,acceptor_mac' ##acceptor_mac is 00e04c230207, AT+BLEGAP=conn,0,00e04c230209
	1.1.4 disconnect with specified cis: AT+BLEISO=cis,initiator,disconnect,<cis_conn_handle>
	1.1.5 if want to create all cis included by the same cig
			>> invoke 'AT+BLEISO=cis,initiator,create_cis_by_cig,<cis_id>,<cig_id>,<acl_conn_handle>' // acl_conn_handle will be printed when successfully invoking step 1.1.3
				for example, if cig_id 1 contains two cis (cis_id 1 and cis_id 2), acl_conn_handle is 16, then need to do following
				1. AT+BLEISO=cis,initiator,create_cis_by_cig,1,1,16  // create cis for cis id 1
				2. AT+BLEISO=cis,initiator,create_cis_by_cig,2,1,16  // create cis for cis id 2
		  if want to create a single cis included by one cig
			>> invoke 'AT+BLEISO=cis,initiator,create_cis_by_hdl,<cis_id>,<cig_id>,<acl_conn_handle>,<cis_conn_handle>' // acl_conn_handle will be printed when successfully invoking step 1.1.3
																												cis_conn_handle will be printed when successfully initializing step 1.1.1
				for example, if cig_id 1 contains two cis (cis_id 1 and cis_id 2), and only cis_id 1 is our need, then we need to do following
				1. AT+BLEISO=cis,initiator,create_cis_by_hdl,1,1,16,0x1b  // create cis for cis id 1, 16 is acl_conn_handle, 0x1b is cis_conn_handle
1.2  CIS acceptor:
	1.2.1 enable : 'AT+BTDEMO=iso,cis,acceptor,1'
	1.2.2 disable : 'AT+BTDEMO=iso,cis,acceptor,0'
	1.2.3 disconnect with specified cis: AT+BLEISO=cis,acceptor,disconnect,<cis_conn_handle>

2. BIS
2.1  BIS broadcaster:
	2.1.1 enable: 'AT+BTDEMO=iso,bis,broadcaster,1'  

2.2  BIS receiver:
	2.2.1 enable: 'AT+BTDEMO=iso,bis,receiver,1'
	2.2.2 PA sync: 'AT+BLEGAP=pa_sync,1,2,0,0,0,<broadcaster_mac>' e.g. AT+BLEGAP=pa_sync,1,2,0,0,0,00e04c800427
	2.2.3 create sync: 'AT+BLEISO=bis,receiver,create_sync,<sync_handle>'  The sync_handle will be printed when successfully PA synchronized in step 2.2.2
	2.2.4 terminate sync: 'AT+BLEISO=bis,receiver,terminate_sync,<big_handle>' The big_handle will be printed when successfully BIG synchronized in step 2.2.3

[Supported List]
Supported IC :
	Ameba - Smart
