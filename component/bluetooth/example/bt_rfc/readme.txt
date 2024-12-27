##################################################################################
#                                                                                #
#                           RFCOMM demo                                          #
#                                                                                #
##################################################################################

Test board：AmebaSmart

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> Select AP Core "CA32"
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT RFC"
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
    2.3 make menuconfig --> CONFIG Application Processor --> Select AP Core "KM4"
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT RFC"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
3. GCC : use CMD "make clean && make all" in auto_build to compile example

Test ATCMD
~~~~~~~~~~~
1.1 enable: AT+BTDEMO=rfc,1
1.2 start inquiry by cmd: AT+BRGAP=inquiry_start,false,10
1.3 connect to the peer rfc device: AT+BTRFC=conn,<bd_addr> e.g. AT+BTRFC=conn,00e04c80070b
1.4 send data to the peer rfc device: AT+BTRFC=send_data,<bd_addr>,<server_channel>,<data_value>
                                        e.g. if want to send {0x01,0x02,0x03} to address:[00:e0:4c:80:07:0b] and server channel:0x03, 
                                        use cmd: AT+BTRFC=send_data,00e04c80070b,0x03,010203
1.5 send credits to the peer rfc device: AT+BTRFC=give_credits,<bd_addr>,<server_channel>,<credits> 
                                         e.g.AT+BTRFC=give_credits,00e04c80070b,0x03,10
1.6 disconnect to the peer rfc device: AT+BTRFC=disconn,<bd_addr> e.g. AT+BTRFC=disconn,00e04c80070b

1.7 disable: AT+BTDEMO=rfc,0