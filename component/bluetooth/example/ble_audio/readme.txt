##################################################################################
#                                                                                #
#                           CAP demo                                             #
#                                                                                #
##################################################################################

Configure the following Macros in bt_api_config.h
    RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT         to                1
    RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT           to                1
    RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT           to                1
    RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT               to                1
    RTK_BLE_AUDIO_AICS_SUPPORT                          to                1
    RTK_BLE_AUDIO_VOCS_SUPPORT                          to                1
    RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT      to                1
    RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT      to                1
    RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT          to                1
    RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT               to                1

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG Application Processor --> Select AP Core "CA32"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Common Audio Profile"
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
    2.3 make menuconfig --> CONFIG Application Processor --> Select AP Core "KM4", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Common Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example
4. To Config Media Source (component/bluetooth/api/include/rtk_bt_le_audio_def.h)
     4.1 Config RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT to 1 for enabling birds thing audio stream
     4.2 Config RTK_BLE_AUDIO_RECORD_SUPPORT to 1 for enabling microphone audio stream
           ** RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT and RTK_BLE_AUDIO_RECORD_SUPPORT should not be enabled both
     4.3 Config RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE to change birds thing audio stream frequency
     4.4 Config RTK_BT_LE_MEDIA_CODEC_CFG_PREFER to change le audio codec config, this parameter should be related to RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE(Frequency should be equal to Audio Source Frequency)
     4.5 Config RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER to change le audio config type(Default is RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT for single channel and RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT for two channels)
     ** default is 16k birdsthing audio source for 10ms duration (------->>), two channels example.

CAP demo ATCMD:
~~~~~~~~~~~
1.CAP Initiator
    1.1 enable                                      AT+BTDEMO=cap,initiator,1
    1.2 disable                                     AT+BTDEMO=cap,initiator,0
    1.3 scan start                                  AT+BLEBAP=escan,1
    1.4 scan stop                                   AT+BLEBAP=escan,0
    1.5 connect                                     AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.6 start unicast stream                        AT+BLEBAP=client_start,<group_idx>,<sink_context>,<source_context> ex: AT+BLEBAP=client_start,0,4,0   4 for RTK_BT_LE_AUDIO_CONTEXT_MEDIA
    1.7 stop unicast stream                         AT+BLEBAP=client_stop,<group_idx>,1000
    1.8 release unicast stream                      AT+BLEBAP=client_release,<group_idx>
    1.9 start broadcast stream                      AT+BLEBAP=broadcast_start
    1.10 stop broadcast stream                      AT+BLEBAP=broadcast_stop

2.CAP Acceptor
    2.1 enable                                          AT+BTDEMO=cap,acceptor,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 disable                                         AT+BTDEMO=cap,acceptor,0
    2.3 stop ext adv before ext scan                    AT+BLEGAP=eadv,0,0
    2.4 scan start                                      AT+BLEBAP=escan,1
    2.5 create pa sync with broadcast source            AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    2.6 terminate pa sync with broadcast source         AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    2.7 create big sync with broadcast source           AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>
    2.8 terminate big sync with broadcast source        AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>

3.CAP Commander
    3.1 enable                                             AT+BTDEMO=cap,commander,1
    3.2 disable                                            AT+BTDEMO=cap,commander,0
    3.3 scan start                                         AT+BLEBAP=escan,1
    3.4 scan stop                                          AT+BLEBAP=escan,0
    3.5 connect                                            AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    3.6 start remote scan                                  AT+BLEBAP=remote_scan,1,<group_idx>
    3.7 stop remote scan                                   AT+BLEBAP=remote_scan,0,<group_idx>
    3.8 create pa sync with broadcast source               AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    3.9 terminate pa sync with broadcast source            AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    3.10 reception_start                                   AT+BLEBAP=reception_start,<bd_addr type>,<bd_addr>,<group_idx>
    3.11 reception_stop                                    AT+BLEBAP=reception_stop,<bd_addr type>,<bd_addr>,<group_idx>
    3.12 reception_remove                                  AT+BLEBAP=reception_remove,<bd_addr type>,<bd_addr>,<group_idx>

PBP demo ATCMD:
~~~~~~~~~~~
1.PBP Broadcast Source
    1.1 enable                                             AT+BTDEMO=pbp,source,1
    1.2 disable                                            AT+BTDEMO=pbp,source,0
    1.3 start broadcast stream                             AT+BLEBAP=broadcast_start
    1.4 stop broadcast stream                              AT+BLEBAP=broadcast_stop

2.PBP Broadcast Sink
    2.1 enable                                             AT+BTDEMO=pbp,sink,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 disable                                            AT+BTDEMO=pbp,sink,0
    2.3 scan start                                         AT+BLEBAP=escan,1
    2.4 scan stop                                          AT+BLEBAP=escan,0
    2.5 create pa sync with broadcast source               AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    2.6 terminate pa sync with broadcast source            AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    2.7 create big sync with broadcast source              AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>
    2.8 terminate big sync with broadcast source           AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>

3.PBP Broadcast Assistant
    3.1 enable                                             AT+BTDEMO=pbp,assistant,1
    3.2 disable                                            AT+BTDEMO=pbp,assistant,0
    3.3 scan start                                         AT+BLEBAP=escan,1
    3.4 scan stop                                          AT+BLEBAP=escan,0
    3.5 connect                                            AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    3.6 start remote scan                                  AT+BLEBAP=remote_scan,1,<group_idx>
    3.7 stop remote scan                                   AT+BLEBAP=remote_scan,0,<group_idx>
    3.8 create pa sync with broadcast source               AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    3.9 terminate pa sync with broadcast source            AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    3.10 reception_start                                   AT+BLEBAP=reception_start,<bd_addr type>,<bd_addr>,<group_idx>
    3.11 reception_stop                                    AT+BLEBAP=reception_stop,<bd_addr type>,<bd_addr>,<group_idx>
    3.12 reception_remove                                  AT+BLEBAP=reception_remove,<bd_addr type>,<bd_addr>,<group_idx>