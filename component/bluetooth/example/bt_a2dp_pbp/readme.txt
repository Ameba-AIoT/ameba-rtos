##################################################################################
#                                                                                #
#                           a2dp sink pbp source demo                            #
#                                                                                #
##################################################################################

Test Configuration
~~~~~~~~~~~
1. BIS num 1 + Sample rate 16kHz + 2-channel + ISO interval 20 ms 
    1.1 Config the following Macros in app_bt_le_audio_common.h
            change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS

2. BIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms
    2.1 Config the following Macros in app_bt_le_audio_common.h
            change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
            change   RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE  to         0
            change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS

3. BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms 
    3.1 Config the following Macros in app_bt_le_audio_common.h
            change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
            change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_30_MS

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> Select AP Core "CA32"
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO PBP"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
        make menuconfig --> AP Config --> MENUCONFIG FOR AP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
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
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO PBP"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

Test ATCMD
~~~~~~~~~~~
1. a2dp sink + pbp source demo 
    1.1 enable: AT+BTDEMO=a2dp_pbp,1
    1.2 disable: AT+BTDEMO=a2dp_pbp,0
    1.3 connect to the a2dp device: use Phone to connect to the device "a2dp sink pbp source"
    1.4 start PBP broadcast: AT+BLEBAP=broadcast,source,start
    1.5 stop PBP broadcast: AT+BLEBAP=broadcast,source,stop

2.pbp sink demo 
2.1 enable: AT+BTDEMO=cap,acceptor,1
2.2 scan for the PBP broadcast: AT+BLEBAP=broadcast,sink,escan,1
2.3 PA sync to the PBP broadcast: AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr> e.g. AT+BLEBAP=broadcast,sink,sync_start,0,00e04c8002eb
3. Play music on the Phone
