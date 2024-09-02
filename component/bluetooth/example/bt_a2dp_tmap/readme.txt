##################################################################################
#                                                                                #
#                           a2dp sink tmap demo                                  #
#                                                                                #
##################################################################################

LE Audio Configuration
~~~~~~~~~~~
Configure the following Macros in bt_api_config.h
    RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT   to                1
    RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT     to                1

A2DP sink + TMAP Unicast Media Sender Configuration
~~~~~~~~~~~
1. CIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms
    1.1 Config the following Macros in app_bt_le_audio_common.h
          change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
          change   LEA_CIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS
          change   RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE      to         0

2. CIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 10 ms
    2.1 Config the following Macros in app_bt_le_audio_common.h
          change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
          change RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE        to         0

3. CIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 20 ms
    3.1 Config the following Macros in app_bt_le_audio_common.h
          change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
          change   LEA_CIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS

A2DP sink + TMAP Broadcast Media Sender Configuration
~~~~~~~~~~~
1. BIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms
    1.1 Config the following Macros in app_bt_le_audio_common.h
          change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
          change   RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE  to         0
          change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS

2. BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms 
    2.1 Config the following Macros in app_bt_le_audio_common.h
          change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
          change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_30_MS

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> Select AP Core "CA32"
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO TMAP"
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
                        --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO TMAP"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

A2DP sink + TMAP Unicast Media Sender Test ATCMD
~~~~~~~~~~~
1.A2DP sink + TMAP unicast media sender demo
    1.1 Enable a2dp sink + tmap unicast media sender             AT+BTDEMO=a2dp_tmap,ums,1
    1.2 Start ext scan                                           AT+BLEBAP=unicast,client,escan,1
    1.3 Stop ext scan                                            AT+BLEBAP=unicast,client,escan,0
    1.4 Start connect to the LE Audio Music Player               AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.5 Use Phone to connect to A2DP sink device named "a2dp tmap player"
    1.6 Start unicast media stream                               AT+BLEBAP=unicast,client,start,0,1
    1.7 Play music on Phone
    1.8 vcs client set mute by group                             AT+BLECAP=commander,vcp,gmute,0,1
    1.9 vcs client set unmute by group                           AT+BLECAP=commander,vcp,gmute,0,0
    1.10 vcs client change volume by group
    1.11 vcs client get volume state                             AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.12 vcs client get volume state                             AT+BLECAP=commander,vcp,get,<conn_handle>
    1.13 vcs client write opcode:Relative Volume Down            AT+BLECAP=commander,vcp,write,<conn_handle>,0
    1.14 vcs client write opcode: Relative Volume Up             AT+BLECAP=commander,vcp,write,<conn_handle>,1
    1.15 vcs client write opcode：Unmute/Relative Volume Down    AT+BLECAP=commander,vcp,write,<conn_handle>,2
    1.16 vcs client write opcode: Unmute/Relative Volume Up      AT+BLECAP=commander,vcp,write,<conn_handle>,3
    1.17 vcs client write opcode: Set Absolute Volume            AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting>
    1.18 vcs client write opcode:Unmute                          AT+BLECAP=commander,vcp,write,<conn_handle>,5
    1.19 vcs client write opcode: Mute                           AT+BLECAP=commander,vcp,write,<conn_handle>,6

2.TMAP unicast media receiver (peer device)  
    Please Enable "BLE AUDIO Telephony and Media Audio Profile" in Menuconfig
    2.1  Enable TMAP unicast media receiver                      AT+BTDEMO=tmap,umr,1

A2DP sink + TMAP Broadcast Media Sender Test ATCMD
~~~~~~~~~~~
1.A2DP sink + TMAP broadcast media sender
    1.1 enable:                                                  AT+BTDEMO=a2dp_tmap,bms,1
    1.2 Start ext scan                                           AT+BLEBAP=unicast,client,escan,1
    1.3 Stop ext scan                                            AT+BLEBAP=unicast,client,escan,0
    1.4 Start LE connect to the peer device                      AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.5 use Phone to connect BR/EDR device named "a2dp tmap player"
    1.6 start broadcast:                                         AT+BLEBAP=broadcast,source,start
    1.7 Play music on the Phone
    1.8 vcs client set mute by group                             AT+BLECAP=commander,vcp,gmute,0,1
    1.9 vcs client set unmute by group                           AT+BLECAP=commander,vcp,gmute,0,0
    1.10 vcs client change volume by group
    1.11 vcs client get volume state                             AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.12 vcs client get volume state                             AT+BLECAP=commander,vcp,get,<conn_handle>
    1.13 vcs client write opcode:Relative Volume Down            AT+BLECAP=commander,vcp,write,<conn_handle>,0
    1.14 vcs client write opcode: Relative Volume Up             AT+BLECAP=commander,vcp,write,<conn_handle>,1
    1.15 vcs client write opcode：Unmute/Relative Volume Down    AT+BLECAP=commander,vcp,write,<conn_handle>,2
    1.16 vcs client write opcode: Unmute/Relative Volume Up      AT+BLECAP=commander,vcp,write,<conn_handle>,3
    1.17 vcs client write opcode: Set Absolute Volume            AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting>
    1.18 vcs client write opcode: Unmute                         AT+BLECAP=commander,vcp,write,<conn_handle>,5
    1.19 vcs client write opcode: Mute                           AT+BLECAP=commander,vcp,write,<conn_handle>,6

2.TMAP broadcast media receiver (peer device)
    Please Enable "BLE AUDIO Telephony and Media Audio Profile" in Menuconfig
    2.1 Enable TMAP broadcast media receiver:                    AT+BTDEMO=tmap,bmr,1
    2.2 scan for the broadcast:                                  AT+BLEBAP=broadcast,sink,escan,1
    2.3 PA sync to the broadcast stream:                         AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>  e.g. AT+BLEBAP=broadcast,sink,sync_start,0,00E04C800427


##################################################################################
#                                                                                #
#                           tmap a2dp source demo                                  #
#                                                                                #
##################################################################################

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> Select AP Core "CA32"
        make menuconfig --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO TMAP"
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
                        --> BT Mode Selection is "DUAL_MODE"--> enable BT Example Demo "BT A2DP BLE AUDIO TMAP"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

TMAP unicast media receiver + A2DP source Test ATCMD
~~~~~~~~~~~
1.TMAP unicast media receiver + A2DP source demo

    DUT:            TMAP unicast media receiver + A2DP source
    SUT1:           A2DP sink
    SUT2:           TMAP unicast media sender 


Before test, TMAP unicast media sender (SUT2) need to be configured as following:

CIS configuration:
    (1) CIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 10 ms
        Config the following Macros in app_bt_le_audio_common.h
            change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K

    (2) CIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 20 ms
        Config the following Macros in app_bt_le_audio_common.h
            change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
            change   LEA_CIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_20_MS

    1.1 enable DUT:                                    AT+BTDEMO=a2dp_tmap,umr,1

    1.2 enable A2DP sink device (SUT1):                AT+BTDEMO=a2dp,snk,1

    1.3 DUT connect to A2DP sink device (SUT1):
        1.3.1 scan:                                    AT+BRGAP=inquiry_start,0,8 
        1.3.2 pair:                                    AT+BTA2DP=conn,<bd_address>      e.g. AT+BTA2DP=conn,00E04C800427
        1.3.3 play music:                              AT+BTA2DP=start,<bd_address>     e.g. AT+BTA2DP=start,00E04C800427

    1.4 SUT2 establish connection with DUT and start unicast stream
        1.4.1 SUT2 enable                              AT+BTDEMO=tmap,ums,1
        1.4.2 SUT2 start ext scan                      AT+BLEBAP=unicast,client,escan,1
        1.4.3 SUT2 stop ext scan                       AT+BLEBAP=unicast,client,escan,0
        1.4.4 SUT2 connect to DUT                      AT+BLEGAP=conn,<address type>,<bd_addr>
        1.4.5 start unicast unidirectional stream      AT+BLEBAP=unicast,client,start,<group id>,<stream mode>

TMAP broadcast media receiver + A2DP source Test ATCMD
~~~~~~~~~~~
2.TMAP broadcast media receiver + A2DP source demo

    DUT:            TMAP broadcast media receiver + A2DP source
    SUT1:           A2DP sink
    SUT2:           TMAP broadcast media sender 

Before test, TMAP broadcast media sender (SUT2) need to be configured as following:

    BIS configuration : BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms 
        Config the following Macros in app_bt_le_audio_common.h
        change LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
        change   LEA_BIG_ISO_INTERVAL_CONFIG                   to         ISO_INTERVAL_30_MS

    2.1 enable DUT:                                    AT+BTDEMO=a2dp_tmap,bmr,1

    2.2 enable A2DP sink device (SUT1):                AT+BTDEMO=a2dp,snk,1

    2.3 DUT connect to A2DP sink device (SUT1):
        2.3.1 scan:                                    AT+BRGAP=inquiry_start,0,8 
        2.3.2 pair:                                    AT+BTA2DP=conn,<bd_address>      e.g. AT+BTA2DP=conn,00E04C800427
        2.3.3 play music:                              AT+BTA2DP=start,<bd_address>     e.g. AT+BTA2DP=start,00E04C800427

    2.4 TMAP broadcast media sender(SUT2) start broadcast
        2.4.1 enable                                              AT+BTDEMO=tmap,bms,1
        2.4.2 start broadcast stream                              AT+BLEBAP=broadcast,source,start

    2.5 DUT start sync to SUT2
        2.5.1  scan broadcast source start                        AT+BLEBAP=broadcast,sink,escan,1
        2.5.2  PA sync && big sync with bap broadcast source      AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
        2.5.3  terminate sync with bap broadcast source           AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

