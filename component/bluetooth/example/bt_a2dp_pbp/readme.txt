##################################################################################
#                                                                                #
#                           a2dp sink pbp source demo                           #
#                                                                                #
##################################################################################

Test Configuration
~~~~~~~~~~~
1. BIS num 1 + Sample rate 16kHz + 2-channel + ISO interval 20 ms 
    1.1 Config bap_broadcast_source_cfg in app_bt_le_audio_common.c
            bap_broadcast_source_cfg.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_16_2;
            bap_broadcast_source_cfg.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY;
    1.2 Config the following Macros in app_bt_le_audio_common.h
            change   RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM     to         1
            change   RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE  to         1

2. BIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms
    2.1 Config bap_broadcast_source_cfg in app_bt_le_audio_common.c
            bap_broadcast_source_cfg.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_48_2;
            bap_broadcast_source_cfg.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY;
    2.2 Config the following Macros in app_bt_le_audio_common.h
            change   RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM     to         1
            change   RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE  to         0
    2.3 Config max_transport_latency in function rtk_bt_bap_broadcast_source_start() in rtk_bt_bap.c
            change:
                def_le_audio_broadcast_source_create_big_param.max_transport_latency = qos_cfg_preferred.max_transport_latency;
            to:
                def_le_audio_broadcast_source_create_big_param.max_transport_latency = 60;

3. BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms 
    3.1 Config bap_broadcast_source_cfg in app_bt_le_audio_common.c
            bap_broadcast_source_cfg.cfg_codec_index = RTK_BT_LE_CODEC_CFG_ITEM_48_2;
            bap_broadcast_source_cfg.cfg_qos_type = RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY;
    3.2 Config the following Macros in app_bt_le_audio_common.h
            change   RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM     to         1
            change   RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE  to         1
    3.3 Config the following Macros in a2dp_sink_pbp_source.c
            change  RTK_BT_A2DP_PBP_DEMO_OUPUT_SAMPLE_RATE        to         48000
            change  RTK_BT_A2DP_PBP_DEMO_OUPUT_CHANNEL_NUM        to           2
    3.4 Config max_transport_latency in function rtk_bt_bap_broadcast_source_start() in rtk_bt_bap.c
            change:
                def_le_audio_broadcast_source_create_big_param.max_transport_latency = qos_cfg_preferred.max_transport_latency;
            to:
                def_le_audio_broadcast_source_create_big_param.max_transport_latency = 80;
    3.5 Config rtn in function rtk_bt_bap_broadcast_source_start() in rtk_bt_bap.c
            change:
                def_le_audio_broadcast_source_create_big_param.rtn = qos_cfg_preferred.retransmission_number;
            to:
                def_le_audio_broadcast_source_create_big_param.rtn = 3;

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
    1.1 enable: ATBE=a2dp_pbp,1
    1.2 disable: ATBE=a2dp_pbp,0
    1.3 connect to the a2dp device: use Phone to connect to the device "a2dp sink pbp source"
    1.4 start PBP broadcast: ATBC=bap_cmd,broadcast,source,start
    1.5 stop PBP broadcast: ATBC=bap_cmd,broadcast,source,stop

2.pbp sink demo 
2.1 enable: ATBE=cap,acceptor,1
2.2 scan for the PBP broadcast: ATBC=bap_cmd,broadcast,sink,escan,1
2.3 PA sync to the PBP broadcast: ATBC=bap_cmd,broadcast,sink,sync_start,0,00e04c8002eb
3. Play music on the Phone
