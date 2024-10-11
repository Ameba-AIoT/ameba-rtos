##################################################################################
#                                                                                #
#                           BAP demo                                             #
#                                                                                #
##################################################################################

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "CA32_SINGLE_CORE"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Basic Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select INIC Processor Role
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
    2.3 make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "KM4_SINGLE_CORE", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Basic Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select "Single_Core_WiFi" Processor Role
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

BAP demo ATCMD:
~~~~~~~~~~~
1.BAP unicast client
    QoS Config: must use before start unicast stream
        unicast session prefer QoS type config          AT+BLEBAP=unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   AT+BLEBAP=unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 Enable                                  AT+BTDEMO=bap,unicast,client,1
    1.2 Start ext scan                          AT+BLEBAP=unicast,client,escan,1
    1.3 Stop ext scan                           AT+BLEBAP=unicast,client,escan,0
    1.4 connect                                 AT+BLEGAP=conn,<address type>,<bd_addr>
    1.5 start unicast unidirectional stream     AT+BLEBAP=unicast,client,start,0,1
    1.6 start unicast bidirectional  stream     AT+BLEBAP=unicast,client,start,0,2
    1.7 stop unicast stream                     AT+BLEBAP=unicast,client,stop,0
    1.8 release unicast stream                  AT+BLEBAP=unicast,client,release,0
    1.9 release stream session                  AT+BLEBAP=unicast,client,stream_release,0

2.BAP unicast server
    2.1 config audio channel                    AT+BLEBAP=unicast,server,cfg,<sound channel>     sound channel:left,right or stereo
    2.2 enable                                  AT+BTDEMO=bap,unicast,server,1
    2.3 disable                                 AT+BTDEMO=bap,unicast,server,0

3.BAP broadcast source
    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config          AT+BLEBAP=broadcast,source,qos_cfg,<qos_type>
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                              AT+BTDEMO=bap,broadcast,source,1
    3.2 disable                                             AT+BTDEMO=bap,broadcast,source,0
    3.3 start broadcast stream                              AT+BLEBAP=broadcast,source,start
    3.4 stop broadcast stream                               AT+BLEBAP=broadcast,source,stop

4.BAP broadcast sink
    4.1 configure broadcast sink sound channel              AT+BLEBAP=broadcast,sink,cfg,<sound channel>  sound channel:left,right or stereo
    4.2 enable                                              AT+BTDEMO=bap,broadcast,sink,1
    4.3 disable                                             AT+BTDEMO=bap,broadcast,sink,0
    4.4 scan broadcast source start                         AT+BLEBAP=broadcast,sink,escan,1
    4.5 PA sync && big sync with bap broadcast source       AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with bap broadcast source            AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

5.BAP broadcast assistant
    5.1 enable                                              AT+BTDEMO=bap,broadcast,assistant,1
    5.2 disable                                             AT+BTDEMO=bap,broadcast,assistant,0
    5.3 ext scan start                                      AT+BLEBAP=unicast,client,escan,1
    5.4 ext scan stop                                       AT+BLEBAP=unicast,client,escan,0
    5.5 connect to bap unicast sink(&&scan delegator)       AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    5.6 broadcast remote scan start                         AT+BLEBAP=broadcast,assistant,bass_scan,1,1,0
    5.7 broadcast remote scan stop                          AT+BLEBAP=broadcast,assistant,bass_scan,0,1,0
    5.8 pa sync with broadcast source                       AT+BLEBAP=broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    5.9 pa sync terminate with broadcast stream             AT+BLEBAP=broadcast,assistant,sync_term,<adv_addr type>,<adv_addr>
    5.10 reception start                                    AT+BLEBAP=broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    5.11 reception stop                                     AT+BLEBAP=broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    5.12 reception remove                                   AT+BLEBAP=broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>

6.BAP scan delegator
    6.1 enable                                              AT+BTDEMO=bap,broadcast,delegate,1
    6.2 disable                                             AT+BTDEMO=bap,broadcast,delegate,0


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

CAP demo ATCMD:
~~~~~~~~~~~
1.CAP Initiator
    QoS Config: must use before start unicast stream
        unicast session prefer QoS type config          AT+BLEBAP=unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   AT+BLEBAP=unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 enable                                      AT+BTDEMO=cap,initiator,1
    1.2 disable                                     AT+BTDEMO=cap,initiator,0
    1.3 start ext scan                              AT+BLEBAP=unicast,client,escan,1
    1.4 stop ext scan                               AT+BLEBAP=unicast,client,escan,0
    1.5 connect to acceptor                         AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.6 start unicast unidirectional stream         AT+BLEBAP=unicast,client,start,0,1
    1.7 start unicast bidirectional  stream         AT+BLEBAP=unicast,client,start,0,2
    1.8 stop unicast stream                         AT+BLEBAP=unicast,client,stop,0
    1.9 mcs server send data：MEDIA_PLAYER_NAME     AT+BLECAP=initiator,mcp,send,0x2B93
    1.10 mcs server send data：TRACK_TITLE          AT+BLECAP=initiator,mcp,send,0x2B97
    1.11 mcs server send data：TRACK_DURATION       AT+BLECAP=initiator,mcp,send,0x2B98,value
    1.12 mcs server send data：TRACK_POSITION       AT+BLECAP=initiator,mcp,send,0x2B99,value
    1.13 mcs server send data：TRACK_CHANGED        AT+BLECAP=initiator,mcp,send,0x2B96
    1.14 start broadcast stream                     AT+BLEBAP=broadcast,source,start
    1.15 stop broadcast stream                      AT+BLEBAP=broadcast,source,stop

2.CAP Acceptor
    2.1 configure sound channel and neighbour address           AT+BLECAP=acceptor,cfg,<sound channel>,<neighbour address>
    2.2 enable                                                  AT+BTDEMO=cap,acceptor,1
    2.3 disable                                                 AT+BTDEMO=cap,acceptor,0
    2.4 vcs server get param                                    AT+BLECAP=acceptor,vcp,get
    2.5 mics server get param                                   AT+BLECAP=acceptor,micp,get
    2.6 vocs server get param                                   AT+BLECAP=acceptor,vocs,get
    2.7 aics server get param                                   AT+BLECAP=acceptor,aics,get,<srv_instance_id>,<param_type> 
                                                                                                -->srv_instance_id <= RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM 
                                                                                                -->param_type: refer to rtk_bt_le_audio_aics_param_type_t except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES
    2.8 mcs client discovery                                    AT+BLECAP=acceptor,mcp,dis,<conn_handle>
    2.9 mcs client read characteristic:MEDIA_PLAYER_NAME        AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B93
    2.10 mcs client read characteristic:TRACK_TITLE             AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B97
    2.11 mcs client read characteristic:TRACK_DURATION          AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B98
    2.12 mcs client read characteristic:TRACK_POSITION          AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B99
    2.13 mcs client read characteristic:MEDIA_STATE             AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BA3
    2.14 mcs client read characteristic:CONTENT_CONTROL_ID      AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BBA
    2.15 mcs client write opcode:PLAY                           AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x01
    2.16 mcs client write opcode:PAUSE                          AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x02
    2.17 mcs client write opcode:FAST_REWIND                    AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x03
    2.18 mcs client write opcode:FAST_FORWARD                   AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x04
    2.19 mcs client write opcode:STOP                           AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x05
    2.20 mcs client write opcode:MOVE_RELATIVE                  AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x10,<offset>
    2.21 mcs client write opcode:GOTO_SEGMENT                   AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x24,<n>
    2.22 mcs client write opcode:PREVIOUS_TRACK                 AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x30
    2.23 mcs client write opcode:NEXT_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x31
    2.24 mcs client write opcode:FIRST_TRACK                    AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x32
    2.25 mcs client write opcode:LAST_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x33
    2.26 mcs client write opcode:GOTO_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x34,<n>
    2.27 mcs client write opcode:GOTO_GROUP                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x44,<n>
    2.28 mcs client enable CCCD                                 AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,1
    2.29 mcs client disable CCCD                                AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,0
    2.30 scan broacast source start                             AT+BLEBAP=broadcast,sink,escan,1
    2.31 sync with broadcast source                             AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    2.32 terminate sync with broadcast source                   AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

3.CAP Commander
    3.1 enable                                                              AT+BTDEMO=cap,commander,1
    3.2 disable                                                             AT+BTDEMO=cap,commander,0
    3.3 ext scan start                                                      AT+BLEBAP=unicast,client,escan,1
    3.4 ext scan stop                                                       AT+BLEBAP=unicast,client,escan,0
    3.5 connect to acceptor                                                 AT+BLEGAP=conn,<address type>,<bd_addr>
    3.6 broadcast remote scan start                                         AT+BLEBAP=broadcast,assistant,bass_scan,1,1,0
    3.7 broadcast remote scan stop                                          AT+BLEBAP=broadcast,assistant,bass_scan,0,1,0
    3.8 pa sync with broadcast source                                       AT+BLEBAP=broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    3.9 terminate pa sync with broadcast source                             AT+BLEBAP=broadcast,assistant,sync_term,<adv_addr type>,<adv_addr>
    3.10 reception start                                                    AT+BLEBAP=broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    3.11 reception stop                                                     AT+BLEBAP=broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    3.12 reception remove                                                   AT+BLEBAP=broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>
    3.13 vcs client set mute by group                                       AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.14 vcs client set unmute by group                                     AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.15 vcs client change volume by group                                  AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    3.16 vcs client get volume state                                        AT+BLECAP=commander,vcp,get,<conn_handle>
    3.17 vcs client write opcode: Relative Volume Down                      AT+BLECAP=commander,vcp,write,<conn_handle>,0
    3.18 vcs client write opcode: Relative Volume Up                        AT+BLECAP=commander,vcp,write,<conn_handle>,1
    3.19 vcs client write opcode: Unmute/Relative Volume Down               AT+BLECAP=commander,vcp,write,<conn_handle>,2
    3.20 vcs client write opcode: Unmute/Relative Volume Up                 AT+BLECAP=commander,vcp,write,<conn_handle>,3
    3.21 vcs client write opcode: Set Absolute Volume                       AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting> 
    3.22 vcs client write opcode:Unmute                                     AT+BLECAP=commander,vcp,write,<conn_handle>,5
    3.23 vcs client write opcode: Mute                                      AT+BLECAP=commander,vcp,write,<conn_handle>,6
    3.24 mics client set unmute by group                                    AT+BLECAP=commander,micp,gmute,<group_idx>,0
    3.25 mics client set mute by group                                      AT+BLECAP=commander,micp,gmute,<group_idx>,1
    3.26 mics client set unmute                                             AT+BLECAP=commander,micp,set_mute,<conn_handle>,0
    3.27 mics client set mute                                               AT+BLECAP=commander,micp,set_mute,<conn_handle>,1
    3.28 mics client get mute                                               AT+BLECAP=commander,micp,get_mute,<conn_handle>
    3.29 vocs client get char value                                         AT+BLECAP=commander,vocs,get_char,<conn_handle>
    3.30 vocs client get srv data                                           AT+BLECAP=commander,vocs,get_srv,<conn_handle>
    3.31 vocs client write output description                               AT+BLECAP=commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    3.32 vocs client write opcode: Set Volume Offset                        AT+BLECAP=commander,vocs,write,<conn_handle>,1,<volume_offset>
    3.33 vocs client write opcode by group: Set Volume Offset               AT+BLECAP=commander,vocs,gwrite,<group_index>,1,<volume_offset>
    3.34 aics client get char value                                         AT+BLECAP=commander,aics,get_char,<conn_handle>
    3.35 aics client get srv data                                           AT+BLECAP=commander,aics,get_srv,<conn_handle>
    3.36 aics client write opcode:SET_GAIN_SETTING                          AT+BLECAP=commander,aics,write,<conn_handle>,1,<gaining_setting>
    3.37 aics client write opcode:UNMUTE                                    AT+BLECAP=commander,aics,write,<conn_handle>,2
    3.38 aics client write opcode:MUTE                                      AT+BLECAP=commander,aics,write,<conn_handle>,3
    3.39 aics client write opcode:SET_MANUAL_GAIN_MODE                      AT+BLECAP=commander,aics,write,<conn_handle>,4
    3.40 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE          AT+BLECAP=commander,aics,write,<conn_handle>,5
    3.41 aics client write opcode by group:SET_GAIN_SETTING                 AT+BLECAP=commander,aics,gwrite,0,1,<gaining_setting>
    3.42 aics client write opcode by group:UNMUTE                           AT+BLECAP=commander,aics,gwrite,0,2
    3.43 aics client write opcode by group:MUTE                             AT+BLECAP=commander,aics,gwrite,0,3
    3.44 aics client write opcode by group:SET_MANUAL_GAIN_MODE             AT+BLECAP=commander,aics,gwrite,0,4
    3.45 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE          AT+BLECAP=commander,aics,gwrite,0,5


##################################################################################
#                                                                                #
#                           TMAP demo                                            #
#                                                                                #
##################################################################################

Configure the following Macros in bt_api_config.h
    RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT         to                1
    RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT           to                1
    RTK_BLE_AUDIO_VOCS_SUPPORT                          to                1
    RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT      to                1
    RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT      to                1
    RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT          to                1
    RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT               to                1

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "CA32_SINGLE_CORE"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Telephony and Media Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select INIC Processor Role
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
    2.3 make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "KM4_SINGLE_CORE", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Telephony and Media Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select "Single_Core_WiFi" Processor Role
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

TMAP demo ATCMD:
~~~~~~~~~~~
1.TMAP unicast media sender
    QoS Config: must use before start unicast stream
        unicast session prefer QoS type config          AT+BLEBAP=unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   AT+BLEBAP=unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 enable                                                      AT+BTDEMO=tmap,ums,1
    1.2 disable                                                     AT+BTDEMO=tmap,ums,0
    1.3 start ext scan                                              AT+BLEBAP=unicast,client,escan,1
    1.4 stop ext scan                                               AT+BLEBAP=unicast,client,escan,0
    1.5 connect to tmap unicast media receiver                      AT+BLEGAP=conn,<address type>,<bd_addr>
    1.6 start unicast unidirectional stream                         AT+BLEBAP=unicast,client,start,<group id>,<stream mode>  
    1.7 stop unicast unidirectional  stream                         AT+BLEBAP=unicast,client,stop,<group id>
    1.8 vcs client set mute by group                                AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.9 vcs client set unmute by group                              AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.10 vcs client change volume by group                          AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.11 vcs client get volume state                                AT+BLECAP=commander,vcp,get,<conn_handle>
    1.12 vcs client write opcode: Relative Volume Down              AT+BLECAP=commander,vcp,write,<conn_handle>,0
    1.13 vcs client write opcode: Relative Volume Up                AT+BLECAP=commander,vcp,write,<conn_handle>,1
    1.14 vcs client write opcode: Unmute/Relative Volume Down       AT+BLECAP=commander,vcp,write,<conn_handle>,2
    1.15 vcs client write opcode: Unmute/Relative Volume Up         AT+BLECAP=commander,vcp,write,<conn_handle>,3
    1.16 vcs client write opcode: Set Absolute Volume               AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting> 
    1.17 vcs client write opcode:Unmute                             AT+BLECAP=commander,vcp,write,<conn_handle>,5
    1.18 vcs client write opcode: Mute                              AT+BLECAP=commander,vcp,write,<conn_handle>,6
    1.19 vocs client get char value                                 AT+BLECAP=commander,vocs,get_char,<conn_handle>
    1.20 vocs client get srv data                                   AT+BLECAP=commander,vocs,get_srv,<conn_handle>
    1.21 vocs client write output description                       AT+BLECAP=commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    1.22 vocs client write opcode: Set Volume Offset                AT+BLECAP=commander,vocs,write,<conn_handle>,1,<volume_offset>
    1.23 vocs client write opcode by group: Set Volume Offset       AT+BLECAP=commander,vocs,gwrite,<group_index>,1,<volume_offset>
    1.24 mcs server send data：MEDIA_PLAYER_NAME                    AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.25 mcs server send data：TRACK_TITLE                          AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.26 mcs server send data：TRACK_DURATION                       AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.27 mcs server send data：TRACK_POSITION                       AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.28 mcs server send data：TRACK_CHANGED                        AT+BLECAP=initiator,mcp,send,0x2B96

2.TMAP unicast media receiver
    2.1 config audio channel and csis neighbor addr             AT+BLETMAP=umr,cfg,<sound_channel>,<neighbour_address>
    2.2 enable                                                  AT+BTDEMO=tmap,umr,1
    2.3 disable                                                 AT+BTDEMO=tmap,umr,0
    2.4 vcs server get param                                    AT+BLECAP=acceptor,vcp,get
    2.5 vocs server get param                                   AT+BLECAP=acceptor,vocs,get
    2.6 mcs client discovery                                    AT+BLECAP=acceptor,mcp,dis,<conn_handle>
    2.7 mcs client read characteristic:MEDIA_PLAYER_NAME        AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B93
    2.8 mcs client read characteristic:TRACK_TITLE              AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B97
    2.9 mcs client read characteristic:TRACK_DURATION           AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B98
    2.10 mcs client read characteristic:TRACK_POSITION          AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B99
    2.11 mcs client read characteristic:MEDIA_STATE             AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BA3
    2.12 mcs client read characteristic:CONTENT_CONTROL_ID      AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BBA
    2.13 mcs client write opcode:PLAY                           AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x01
    2.14 mcs client write opcode:PAUSE                          AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x02
    2.15 mcs client write opcode:FAST_REWIND                    AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x03
    2.16 mcs client write opcode:FAST_FORWARD                   AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x04
    2.17 mcs client write opcode:STOP                           AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x05
    2.18 mcs client write opcode:MOVE_RELATIVE                  AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x10,<offset>
    2.19 mcs client write opcode:GOTO_SEGMENT                   AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x24,<n>
    2.20 mcs client write opcode:PREVIOUS_TRACK                 AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x30
    2.21 mcs client write opcode:NEXT_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x31
    2.22 mcs client write opcode:FIRST_TRACK                    AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x32
    2.23 mcs client write opcode:LAST_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x33
    2.24 mcs client write opcode:GOTO_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x34,<n>
    2.25 mcs client write opcode:GOTO_GROUP                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x44,<n>
    2.26 mcs client enable CCCD                                 AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,1
    2.27 mcs client disable CCCD                                AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,0

3.TMAP broadcast media sender
    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config                AT+BLEBAP=broadcast,source,qos_cfg,<qos_type>  
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                                      AT+BTDEMO=tmap,bms,1
    3.2 disable                                                     AT+BTDEMO=tmap,bms,0
    3.3 start ext scan                                              AT+BLEBAP=unicast,client,escan,1
    3.4  stop ext scan                                              AT+BLEBAP=unicast,client,escan,0
    3.5 connect to tmap broadcast media receiver                    AT+BLEGAP=conn,<address type>,<bd_addr>
    3.6 start broadcast stream                                      AT+BLEBAP=broadcast,source,start
    3.7 stop broadcast stream                                       AT+BLEBAP=broadcast,source,stop
    3.8 vcs client set mute by group                                AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.9 vcs client set unmute by group                              AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.10 vcs client change volume by group                          AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    3.11 vcs client get volume state                                AT+BLECAP=commander,vcp,get,<conn_handle>
    3.12 vcs client write opcode: Relative Volume Down              AT+BLECAP=commander,vcp,write,<conn_handle>,0
    3.13 vcs client write opcode: Relative Volume Up                AT+BLECAP=commander,vcp,write,<conn_handle>,1
    3.14 vcs client write opcode: Unmute/Relative Volume Down       AT+BLECAP=commander,vcp,write,<conn_handle>,2
    3.15 vcs client write opcode: Unmute/Relative Volume Up         AT+BLECAP=commander,vcp,write,<conn_handle>,3
    3.16 vcs client write opcode: Set Absolute Volume               AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting> 
    3.17 vcs client write opcode:Unmute                             AT+BLECAP=commander,vcp,write,<conn_handle>,5
    3.18 vcs client write opcode: Mute                              AT+BLECAP=commander,vcp,write,<conn_handle>,6

4.TMAP broadcast media receiver
    4.1 config audio channel and csis neighbor addr                 AT+BLETMAP=bmr,cfg,<sound channel>,{<neighbor addr>}
    4.2 enable                                                      AT+BTDEMO=tmap,bmr,1
    4.3 disable                                                     AT+BTDEMO=tmap,bmr,0
    4.4  scan broadcast source start                                AT+BLEBAP=broadcast,sink,escan,1
    4.5 PA sync && big sync with bap broadcast source               AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with bap broadcast source                    AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>


##################################################################################
#                                                                                #
#                           PBP demo                                             #
#                                                                                #
##################################################################################

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "CA32_SINGLE_CORE"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Public Broadcast Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select INIC Processor Role
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
    2.3 make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "KM4_SINGLE_CORE", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Public Broadcast Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select "Single_Core_WiFi" Processor Role
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

PBP demo ATCMD:
~~~~~~~~~~~
1.PBP broadcast source
    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config                AT+BLEBAP=broadcast,source,qos_cfg,<qos_type>  
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    1.1 enable                                          AT+BTDEMO=pbp,source,1
    1.2 disable                                         AT+BTDEMO=pbp,source,0
    1.3 start broadcast stream                          AT+BLEBAP=broadcast,source,start
    1.4 stop broadcast stream                           AT+BLEBAP=broadcast,source,stop

2.PBP broadcast sink
    2.1 configure broadcast sink sound channel          AT+BLEPBP=sink,cfg,<sound_channel>
    2.2 enable                                          AT+BTDEMO=pbp,sink,1
    2.3 disable                                         AT+BTDEMO=pbp,sink,0
    2.4 scan broadcast source start                     AT+BLEBAP=broadcast,sink,escan,1
    2.5 PA sync && big sync with PBP broadcast source   AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    2.6 terminate sync with PBP broadcast source        AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

3.PBP broadcast assistant
    3.1 enable                                                      AT+BTDEMO=pbp,assistant,1
    3.2 disable                                                     AT+BTDEMO=pbp,assistant,0
    3.3 start ext scan                                              AT+BLEBAP=unicast,client,escan,1
    3.4 stop ext scan                                               AT+BLEBAP=unicast,client,escan,0
    3.5 connect to PBP broadcast sink                               AT+BLEGAP=conn,<address type>,<bd_addr>
    3.6 broadcast remote scan start                                 AT+BLEBAP=broadcast,assistant,bass_scan,1,1,0
    3.7 broadcast remote scan stop                                  AT+BLEBAP=broadcast,assistant,bass_scan,0,1,0
    3.8 PBP broadcast assistant pa sync with broadcast stream       AT+BLEBAP=broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    3.9 reception start                                             AT+BLEBAP=broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    3.10 reception stop                                             AT+BLEBAP=broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    3.11 reception remove                                           AT+BLEBAP=broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>

##################################################################################
#                                                                                #
#                           GMAP demo                                             #
#                                                                                #
##################################################################################

Configure the following Macros in bt_api_config.h
    RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT         to                1
    RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT           to                1
    RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT           to                1
    RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT               to                1
    RTK_BLE_AUDIO_AICS_SUPPORT                          to                1
    RTK_BLE_AUDIO_VOCS_SUPPORT                          to                1
    RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT          to                1
    RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT               to                1

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "CA32_SINGLE_CORE"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Gaming Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select INIC Processor Role
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
    2.3 make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "KM4_SINGLE_CORE", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Gaming Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select "Single_Core_WiFi" Processor Role
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

GMAP demo ATCMD:
~~~~~~~~~~~
1.GMAP Unicast Game Gateway
    QoS Config: must use before start unicast stream
        unicast session prefer QoS type config          AT+BLEBAP=unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   AT+BLEBAP=unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY
    1.1 enable                                                        AT+BTDEMO=gmap,ugg,1
    1.2 disable                                                       AT+BTDEMO=gmap,ugg,0
    1.3 start ext scan                                                AT+BLEBAP=unicast,client,escan,1
    1.4 stop ext scan                                                 AT+BLEBAP=unicast,client,escan,0
    1.5 connect to GMAP Unicast Game Receiver                         AT+BLEGAP=conn,<address type>,<bd_addr>
    1.6 start unicast unidirectional stream                           AT+BLEBAP=unicast,client,start,<group id>,<stream mode>  
    1.7 stop unicast unidirectional  stream                           AT+BLEBAP=unicast,client,stop,<group id>
    1.8 vcs client set mute by group                                  AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.9 vcs client set unmute by group                                AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.10 vcs client change volume by group                            AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.11 vcs client get volume state                                  AT+BLECAP=commander,vcp,get,<conn_handle>
    1.12 vcs client write opcode: Relative Volume Down                AT+BLECAP=commander,vcp,write,<conn_handle>,0
    1.13 vcs client write opcode: Relative Volume Up                  AT+BLECAP=commander,vcp,write,<conn_handle>,1
    1.14 vcs client write opcode: Unmute/Relative Volume Down         AT+BLECAP=commander,vcp,write,<conn_handle>,2
    1.15 vcs client write opcode: Unmute/Relative Volume Up           AT+BLECAP=commander,vcp,write,<conn_handle>,3
    1.16 vcs client write opcode: Set Absolute Volume                 AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting> 
    1.17 vcs client write opcode:Unmute                               AT+BLECAP=commander,vcp,write,<conn_handle>,5
    1.18 vcs client write opcode: Mute                                AT+BLECAP=commander,vcp,write,<conn_handle>,6
    1.19 vocs client get char value                                   AT+BLECAP=commander,vocs,get_char,<conn_handle>
    1.20 vocs client get srv data                                     AT+BLECAP=commander,vocs,get_srv,<conn_handle>
    1.21 vocs client write output description                         AT+BLECAP=commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    1.22 vocs client write opcode: Set Volume Offset                  AT+BLECAP=commander,vocs,write,<conn_handle>,1,<volume_offset>
    1.23 vocs client write opcode by group: Set Volume Offset         AT+BLECAP=commander,vocs,gwrite,<group_index>,1,<volume_offset>
    1.24 mics client set unmute by group                              AT+BLECAP=commander,micp,gmute,<group_idx>,0
    1.25 mics client set mute by group                                AT+BLECAP=commander,micp,gmute,<group_idx>,1
    1.26 mics client set unmute                                       AT+BLECAP=commander,micp,set_mute,<conn_handle>,0
    1.27 mics client set mute                                         AT+BLECAP=commander,micp,set_mute,<conn_handle>,1
    1.28 mics client get mute                                         AT+BLECAP=commander,micp,get_mute,<conn_handle>
    1.29 aics client get char value                                   AT+BLECAP=commander,aics,get_char,<conn_handle>
    1.30 aics client get srv data                                     AT+BLECAP=commander,aics,get_srv,<conn_handle>
    1.31 aics client write opcode:SET_GAIN_SETTING                    AT+BLECAP=commander,aics,write,<conn_handle>,1,<gaining_setting>
    1.32 aics client write opcode:UNMUTE                              AT+BLECAP=commander,aics,write,<conn_handle>,2
    1.33 aics client write opcode:MUTE                                AT+BLECAP=commander,aics,write,<conn_handle>,3
    1.34 aics client write opcode:SET_MANUAL_GAIN_MODE                AT+BLECAP=commander,aics,write,<conn_handle>,4
    1.35 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE    AT+BLECAP=commander,aics,write,<conn_handle>,5
    1.36 aics client write opcode by group:SET_GAIN_SETTING           AT+BLECAP=commander,aics,gwrite,0,1,<gaining_setting>
    1.37 aics client write opcode by group:UNMUTE                     AT+BLECAP=commander,aics,gwrite,0,2
    1.38 aics client write opcode by group:MUTE                       AT+BLECAP=commander,aics,gwrite,0,3
    1.39 aics client write opcode by group:SET_MANUAL_GAIN_MODE       AT+BLECAP=commander,aics,gwrite,0,4
    1.40 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE    AT+BLECAP=commander,aics,gwrite,0,5

2.GMAP Unicast Game Terminal
    2.1 configure sound channel and neighbour address           AT+BLEGMAP=ugt,cfg,<sound channel>,<neighbour address>
    2.2 enable                                                  AT+BTDEMO=gmap,ugt,1
    2.3 disable                                                 AT+BTDEMO=gmap,ugt,0
    2.4 vcs server get param                                    AT+BLECAP=acceptor,vcp,get
    2.5 mics server get param                                   AT+BLECAP=acceptor,micp,get
    2.6 vocs server get param                                   AT+BLECAP=acceptor,vocs,get
    2.7 aics server get param                                   AT+BLECAP=acceptor,aics,get,<srv_instance_id>,<param_type> 
                                                                                                -->srv_instance_id <= RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM 
                                                                                                -->param_type: refer to rtk_bt_le_audio_aics_param_type_t except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES

3.GMAP Broadcast Game Sender
    For the GMAP BGS only support sample rate 48kHz and 32kHz
    Please configure the following Macros in app_bt_le_audio_common.h
        LEA_SOURCE_FIX_SAMPLE_FREQUENCY  to  RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
        or                               to  RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K

    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config                AT+BLEBAP=broadcast,source,qos_cfg,<qos_type>
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                              AT+BTDEMO=gmap,bgs,1
    3.2 disable                                             AT+BTDEMO=gmap,bgs,0
    3.3 start broadcast stream                              AT+BLEBAP=broadcast,source,start
    3.4 stop broadcast stream                               AT+BLEBAP=broadcast,source,stop

4.GMAP Broadcast Game Receiver
    4.1 configure sound channel and neighbour address           AT+BLEGMAP=bgr,cfg,<sound channel>,<neighbour address>
    4.2 enable                                                  AT+BTDEMO=gmap,bgr,1
    4.3 disable                                                 AT+BTDEMO=gmap,bgr,0
    4.4 scan broacast source start                              AT+BLEBAP=broadcast,sink,escan,1
    4.5 sync with broadcast source                              AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with broadcast source                    AT+BLEBAP=broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

##################################################################################
#                                                                                #
#                           LE Audio BAP demo Outband Uart Test                  #
#                                                                                #
##################################################################################

Test Configuration
~~~~~~~~~~~
BT Audio outband 48kHz sine ware uart test:

change the following Macros:
    CONFIG_BT_AUDIO_SOURCE_OUTBAND        in  app_audio_data.h            to     1
    DEMO_UART_READ_THRESHOLD              in  app_audio_data.c            to     1920
    RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT  in  app_bt_le_audio_common.h     to     0  
    LEA_SOURCE_FIX_SAMPLE_FREQUENCY       in  app_bt_le_audio_common.h     to     RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K  

GCC menuconfig 
~~~~~~~~~~~
1. for CA32 single core:
        make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "CA32_SINGLE_CORE"
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Basic Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select INIC Processor Role
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
    2.3 make menuconfig --> CONFIG BT --> Enable BT and Processor Role is "KM4_SINGLE_CORE", 
                        --> BT Mode Selection is "BLE_ONLY"--> enable BT Example Demo "BLE Audio"
                        --> Enable "BLE Audio Basic Audio Profile"
        make menuconfig --> CONGIG WIFI --> Enable Wifi and select "Single_Core_WiFi" Processor Role
        make menuconfig --> HP Config --> MENUCONFIG FOR HP CONFIG --> Audio Config -> Enable Audio Framework and select Audio Interfaces to Mixer
3. GCC : use CMD "make clean && make all" in auto_build to compile example

Test Step
~~~~~~~~~~~
1. Use a uart tool, one end connect to the pinmux of test borad as configured in app_audio_data.c, and another connect to PC USB.
    uart tool  RTS  to   test board    PTS pinmux
    uart tool  PTS  to   test board    RTS pinmux
    uart tool  RX   to   test board    TX pinmux
    uart tool  TX   to   test board    RX pinmux
    uart tool  GND  to   test board    GNT

2. Open the uart tool bt_audio_docker_gui.exe
    2.1 choose the proper COM 
    2.2 choose Flow control Hardware Control
    2.3 click Open buttion
    2.4 Open the input audio file
    2.5 Open the Start Buttion

3. Test atcmd
broadcast test:
3.1 BAP broadcast source(Receive the audio data from uart tool and send it to air)
    3.1.1 enable                                  AT+BTDEMO=bap,broadcast,source,1
    3.1.2 start broadcast stream                  AT+BLEBAP=broadcast,source,start

3.2 BAP broadcast sink(Peer device)
    3.2.1 enable                                              AT+BTDEMO=bap,broadcast,sink,1
    3.2.2 scan broadcast source start                         AT+BLEBAP=broadcast,sink,escan,1
    3.2.3 PA sync && big sync with bap broadcast source       AT+BLEBAP=broadcast,sink,sync_start,<bd_addr type>,<bd_addr>

unicast test:
3.3 BAP unicast test
    3.3.1 Enable                                  AT+BTDEMO=bap,unicast,client,1
    3.3.2 Start ext scan                          AT+BLEBAP=unicast,client,escan,1
    3.3.3 Stop ext scan                           AT+BLEBAP=unicast,client,escan,0
    3.3.4 connect                                 AT+BLEGAP=conn,<address type>,<bd_addr>
    3.3.5 start unicast unidirectional stream     AT+BLEBAP=unicast,client,start,0,1

3.4 BAP unicast server(Peer device)
    3.4.1 enable                                  AT+BTDEMO=bap,unicast,server,1