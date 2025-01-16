##################################################################################
#                                                                                #
#                           ble_iso_demo                                         #
#                                                                                #
##################################################################################

Date: 2024 - 08 - 02

Setup Guide
~~~~~~~~~~~
1. unmask the following script in  '~/repo/source/amebasmart_gcc_project/menuconfig/config.in'
#       bool "  BLE ISO Test"                   CONFIG_BT_ISO_TEST_FOR_AP
#       bool "  BLE ISO Test"                   CONFIG_BT_ISO_TEST_FOR_HP

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> select "CA32"
        make menuconfig --> CONFIG BT --> Enable BT
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE ISO Test"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
2. for KM4 single core:
    2.1 Config Boot_AP_Enbale in component/soc/amebasmart/usrcfg/ameba_bootcfg.c
        change:
                u8 Boot_AP_Enbale = ENABLE;
        to:
                u8 Boot_AP_Enbale = DISABLE;
    2.2 Config Boot_AP_Enbale in amebasmart_layout.ld
        change:
                KM4_BD_DRAM (rwx) :                     ORIGIN = 0x60000020, LENGTH = 0x6015B000 - 0x60000020
        to:
                KM4_BD_DRAM (rwx) :                     ORIGIN = 0x60000020, LENGTH = 0x6025B000 - 0x60000020
    2.3 make menuconfig --> CONFIG Application Processor --> select "KM4"
        make menuconfig --> CONFIG BT --> Enable BT
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE ISO Test"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
3. GCC : use CMD "make clean && make all" in auto_build to compile example

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
	1.1.5 send data: 'AT+BLEISO=cis,initiator,send_data,1' 
1.2  CIS acceptor:
	1.2.1 enable : 'AT+BTDEMO=iso,cis,acceptor,1'
	1.2.2 disable : 'AT+BTDEMO=iso,cis,acceptor,0'
	1.2.3 disconnect with specified cis: AT+BLEISO=cis,acceptor,disconnect,<cis_conn_handle>
	1.2.4 send data: 'AT+BLEISO=cis,acceptor,send_data,1'

2. BIS
2.1  BIS broadcaster:
	2.1.1 enable: 'AT+BTDEMO=iso,bis,broadcaster,1'  
	2.1.2 send data: 'AT+BLEISO=bis,broadcaster,send_data,1'
2.2  BIS receiver:
	2.2.1 enable: 'AT+BTDEMO=iso,bis,receiver,1'
	2.2.2 PA sync: 'AT+BLEGAP=pa_sync,1,2,0,0,0,<broadcaster_mac>' e.g. AT+BLEGAP=pa_sync,1,2,0,0,0,00e04c800427
	2.2.3 create sync: 'AT+BLEISO=bis,receiver,create_sync,<sync_handle>'  The sync_handle will be printed when successfully PA synchronized in step 2.2.2
	2.2.4 terminate sync: 'AT+BLEISO=bis,receiver,terminate_sync,<big_handle>' The big_handle will be printed when successfully BIG synchronized in step 2.2.3

[Supported List]
Supported IC :
	Ameba - Smart
