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
        unicast session prefer QoS type config          ATBC=bap_cmd,unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   ATBC=bap_cmd,unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 Enable                                  ATBE=bap,unicast,client,1
    1.2 Start ext scan                          ATBC=bap_cmd,unicast,client,escan,1
    1.3 Stop ext scan                           ATBC=bap_cmd,unicast,client,escan,0
    1.4 connect                                 ATBC=le_gap,conn,<address type>,<bd_addr>
    1.5 start unicast unidirectional stream     ATBC=bap_cmd,unicast,client,start,0,1
    1.6 start unicast bidirectional  stream     ATBC=bap_cmd,unicast,client,start,0,2
    1.7 stop unicast stream                     ATBC=bap_cmd,unicast,client,stop,0

2.BAP unicast server
    2.1 config audio channel                    ATBC=bap_cmd,unicast,server,cfg,<sound channel>     sound channel:left,right or stereo
    2.2 enable                                  ATBE=bap,unicast,server,1
    2.3 disable                                 ATBE=bap,unicast,server,0

3.BAP broadcast source
    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config          ATBC=bap_cmd,broadcast,source,qos_cfg,<qos_type>
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                              ATBE=bap,broadcast,source,1
    3.2 disable                                             ATBE=bap,broadcast,source,0
    3.3 start broadcast stream                              ATBC=bap_cmd,broadcast,source,start
    3.4 stop broadcast stream                               ATBC=bap_cmd,broadcast,source,stop

4.BAP broadcast sink
    4.1 configure broadcast sink sound channel              ATBC=bap_cmd,broadcast,sink,cfg,<sound channel>  sound channel:left,right or stereo
    4.2 enable                                              ATBE=bap,broadcast,sink,1
    4.3 disable                                             ATBE=bap,broadcast,sink,0
    4.4 scan broadcast source start                         ATBC=bap_cmd,broadcast,sink,escan,1
    4.5 PA sync && big sync with bap broadcast source       ATBC=bap_cmd,broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with bap broadcast source            ATBC=bap_cmd,broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

5.BAP broadcast assistant
    5.1 enable                                              ATBE=bap,broadcast,assistant,1
    5.2 disable                                             ATBE=bap,broadcast,assistant,0
    5.3 ext scan start                                      ATBC=bap_cmd,unicast,client,escan,1
    5.4 ext scan stop                                       ATBC=bap_cmd,unicast,client,escan,0
    5.5 connect to bap unicast server(&&scan delegator)     ATBC=le_gap,conn,<peer_addr_type>,<peer_addr>
    5.6 broadcast remote scan start                         ATBC=bap_cmd,broadcast,assistant,bass_scan,1,1,0
    5.7 broadcast remote scan stop                          ATBC=bap_cmd,broadcast,assistant,bass_scan,0,1,0
    5.8 pa sync with broadcast source                       ATBC=bap_cmd,broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    5.9 pa sync terminate with broadcast stream             ATBC=bap_cmd,broadcast,assistant,sync_term,<adv_addr type>,<adv_addr>
    5.10 reception start                                    ATBC=bap_cmd,broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    5.11 reception stop                                     ATBC=bap_cmd,broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    5.12 reception remove                                   ATBC=bap_cmd,broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>

6.BAP scan delegator
    6.1 enable                                              ATBE=bap,broadcast,delegate,1
    6.2 disable                                             ATBE=bap,broadcast,delegate,0


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
        unicast session prefer QoS type config          ATBC=bap_cmd,unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   ATBC=bap_cmd,unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 enable                                      ATBE=cap,initiator,1
    1.2 disable                                     ATBE=cap,initiator,0
    1.3 start ext scan                              ATBC=bap_cmd,unicast,client,escan,1
    1.4 stop ext scan                               ATBC=bap_cmd,unicast,client,escan,0
    1.5 connect to acceptor                         ATBC=le_gap,conn,<peer_addr_type>,<peer_addr>
    1.6 start unicast unidirectional stream         ATBC=bap_cmd,unicast,client,start,0,1
    1.7 start unicast bidirectional  stream         ATBC=bap_cmd,unicast,client,start,0,2
    1.8 stop unicast stream                         ATBC=bap_cmd,unicast,client,stop,<group id>
    1.9 mcs server send data：MEDIA_PLAYER_NAME     ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.10 mcs server send data：TRACK_TITLE          ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.11 mcs server send data：TRACK_DURATION       ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.12 mcs server send data：TRACK_POSITION       ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.13 mcs server send data：TRACK_CHANGED        ATBC=cap_cmd,initiator,mcp,send,0x2B96
    1.14 start broadcast stream                     ATBC=bap_cmd,broadcast,source,start
    1.15 stop broadcast stream                      ATBC=bap_cmd,broadcast,source,stop

2.CAP Acceptor
    2.1 configure sound channel and neighbour address           ATBC=cap_cmd,acceptor,cfg,<sound channel>,<neighbour address>
    2.2 enable                                                  ATBE=cap,acceptor,1
    2.3 disable                                                 ATBE=cap,acceptor,0
    2.4 vcs server get param                                    ATBC=cap_cmd,acceptor,vcp,get
    2.5 mics server get param                                   ATBC=cap_cmd,acceptor,micp,get
    2.6 vocs server get param                                   ATBC=cap_cmd,acceptor,vocs,get
    2.7 aics server get param                                   ATBC=cap_cmd,acceptor,aics,get,<srv_instance_id>,<param_type> 
                                                                                                -->srv_instance_id <= RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM 
                                                                                                -->param_type: refer to rtk_bt_le_audio_aics_param_type_t except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES
    2.8 mcs client discovery                                    ATBC=cap_cmd,acceptor,mcp,dis,<conn_handle>
    2.9 mcs client read characteristic:MEDIA_PLAYER_NAME        ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B93
    2.10 mcs client read characteristic:TRACK_TITLE             ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B97
    2.11 mcs client read characteristic:TRACK_DURATION          ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B98
    2.12 mcs client read characteristic:TRACK_POSITION          ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B99
    2.13 mcs client read characteristic:MEDIA_STATE             ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2BA3
    2.14 mcs client read characteristic:CONTENT_CONTROL_ID      ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2BBA
    2.15 mcs client write opcode:PLAY                           ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x01
    2.16 mcs client write opcode:PAUSE                          ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x02
    2.17 mcs client write opcode:FAST_REWIND                    ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x03
    2.18 mcs client write opcode:FAST_FORWARD                   ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x04
    2.19 mcs client write opcode:STOP                           ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x05
    2.20 mcs client write opcode:MOVE_RELATIVE                  ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x10,<offset>
    2.21 mcs client write opcode:GOTO_SEGMENT                   ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x24,<n>
    2.22 mcs client write opcode:PREVIOUS_TRACK                 ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x30
    2.23 mcs client write opcode:NEXT_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x31
    2.24 mcs client write opcode:FIRST_TRACK                    ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x32
    2.25 mcs client write opcode:LAST_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x33
    2.26 mcs client write opcode:GOTO_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x34,<n>
    2.27 mcs client write opcode:GOTO_GROUP                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x44,<n>
    2.28 mcs client enable CCCD                                 ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,<cfg_cccd>,1
    2.29 mcs client disable CCCD                                ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,<cfg_cccd>,0
    2.30 scan broacast source start                             ATBC=bap_cmd,broadcast,sink,escan,1
    2.31 sync with broadcast source                             ATBC=bap_cmd,broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    2.32 terminate sync with broadcast source                   ATBC=bap_cmd,broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

3.CAP Commander
    3.1 enable                                                              ATBE=cap,commander,1
    3.2 disable                                                             ATBE=cap,commander,0
    3.3 ext scan start                                                      ATBC=bap_cmd,unicast,client,escan,1
    3.4 ext scan stop                                                       ATBC=bap_cmd,unicast,client,escan,0
    3.5 connect to acceptor                                                 ATBC=le_gap,conn,<peer_addr_type>,<peer_addr>
    3.6 broadcast remote scan start                                         ATBC=bap_cmd,broadcast,assistant,bass_scan,1,1,0
    3.7 broadcast remote scan stop                                          ATBC=bap_cmd,broadcast,assistant,bass_scan,0,1,0
    3.8 pa sync with broadcast source                                       ATBC=bap_cmd,broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    3.9 terminate pa sync with broadcast source                             ATBC=bap_cmd,broadcast,assistant,sync_term,<adv_addr type>,<adv_addr>
    3.10 reception start                                                    ATBC=bap_cmd,broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    3.11 reception stop                                                     ATBC=bap_cmd,broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    3.12 reception remove                                                   ATBC=bap_cmd,broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>
    3.13 vcs client set mute by group                                       ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.14 vcs client set unmute by group                                     ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.15 vcs client change volume by group                                  ATBC=cap_cmd,commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    3.16 vcs client get volume state                                        ATBC=cap_cmd,commander,vcp,get,<conn_handle>
    3.17 vcs client write opcode: Relative Volume Down                      ATBC=cap_cmd,commander,vcp,write,<conn_handle>,0
    3.18 vcs client write opcode: Relative Volume Up                        ATBC=cap_cmd,commander,vcp,write,<conn_handle>,1
    3.19 vcs client write opcode: Unmute/Relative Volume Down               ATBC=cap_cmd,commander,vcp,write,<conn_handle>,2
    3.20 vcs client write opcode: Unmute/Relative Volume Up                 ATBC=cap_cmd,commander,vcp,write,<conn_handle>,3
    3.21 vcs client write opcode: Set Absolute Volume                       ATBC=cap_cmd,commander,vcp,write,<conn_handle>,4,<volume_setting> 
    3.22 vcs client write opcode:Unmute                                     ATBC=cap_cmd,commander,vcp,write,<conn_handle>,5
    3.23 vcs client write opcode: Mute                                      ATBC=cap_cmd,commander,vcp,write,<conn_handle>,6
    3.24 mics client set unmute by group                                    ATBC=cap_cmd,commander,micp,gmute,<group_idx>,0
    3.25 mics client set mute by group                                      ATBC=cap_cmd,commander,micp,gmute,<group_idx>,1
    3.26 mics client set unmute                                             ATBC=cap_cmd,commander,micp,set_mute,<conn_handle>,0
    3.27 mics client set mute                                               ATBC=cap_cmd,commander,micp,set_mute,<conn_handle>,1
    3.28 mics client get mute                                               ATBC=cap_cmd,commander,micp,get_mute,<conn_handle>
    3.29 vocs client get char value                                         ATBC=cap_cmd,commander,vocs,get_char,<conn_handle>
    3.30 vocs client get srv data                                           ATBC=cap_cmd,commander,vocs,get_srv,<conn_handle>
    3.31 vocs client write output description                               ATBC=cap_cmd,commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    3.32 vocs client write opcode: Set Volume Offset                        ATBC=cap_cmd,commander,vocs,write,<conn_handle>,1,<volume_offset>
    3.33 vocs client write opcode by group: Set Volume Offset               ATBC=cap_cmd,commander,vocs,gwrite,<group_index>,1,<volume_offset>
    3.34 aics client get char value                                         ATBC=cap_cmd,commander,aics,get_char,<conn_handle>
    3.35 aics client get srv data                                           ATBC=cap_cmd,commander,aics,get_srv,<conn_handle>
    3.36 aics client write opcode:SET_GAIN_SETTING                          ATBC=cap_cmd,commander,aics,write,<conn_handle>,1,<gaining_setting>
    3.37 aics client write opcode:UNMUTE                                    ATBC=cap_cmd,commander,aics,write,<conn_handle>,2
    3.38 aics client write opcode:MUTE                                      ATBC=cap_cmd,commander,aics,write,<conn_handle>,3
    3.39 aics client write opcode:SET_MANUAL_GAIN_MODE                      ATBC=cap_cmd,commander,aics,write,<conn_handle>,4
    3.40 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE          ATBC=cap_cmd,commander,aics,write,<conn_handle>,5
    3.41 aics client write opcode by group:SET_GAIN_SETTING                 ATBC=cap_cmd,commander,aics,gwrite,0,1,<gaining_setting>
    3.42 aics client write opcode by group:UNMUTE                           ATBC=cap_cmd,commander,aics,gwrite,0,2
    3.43 aics client write opcode by group:MUTE                             ATBC=cap_cmd,commander,aics,gwrite,0,3
    3.44 aics client write opcode by group:SET_MANUAL_GAIN_MODE             ATBC=cap_cmd,commander,aics,gwrite,0,4
    3.45 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE          ATBC=cap_cmd,commander,aics,gwrite,0,5


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
        unicast session prefer QoS type config          ATBC=bap_cmd,unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   ATBC=bap_cmd,unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY

    1.1 enable                                                      ATBE=tmap,ums,1
    1.2 disable                                                     ATBE=tmap,ums,0
    1.3 start ext scan                                              ATBC=bap_cmd,unicast,client,escan,1
    1.4 stop ext scan                                               ATBC=bap_cmd,unicast,client,escan,0
    1.5 connect to tmap unicast media receiver                      ATBC=le_gap,conn,<address type>,<bd_addr>
    1.6 start unicast unidirectional stream                         ATBC=bap_cmd,unicast,client,start,<group id>,<stream mode>  
    1.7 stop unicast unidirectional  stream                         ATBC=bap_cmd,unicast,client,stop,<group id>
    1.8 vcs client set mute by group                                ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.9 vcs client set unmute by group                              ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.10 vcs client change volume by group                          ATBC=cap_cmd,commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.11 vcs client get volume state                                ATBC=cap_cmd,commander,vcp,get,<conn_handle>
    1.12 vcs client write opcode: Relative Volume Down              ATBC=cap_cmd,commander,vcp,write,<conn_handle>,0
    1.13 vcs client write opcode: Relative Volume Up                ATBC=cap_cmd,commander,vcp,write,<conn_handle>,1
    1.14 vcs client write opcode: Unmute/Relative Volume Down       ATBC=cap_cmd,commander,vcp,write,<conn_handle>,2
    1.15 vcs client write opcode: Unmute/Relative Volume Up         ATBC=cap_cmd,commander,vcp,write,<conn_handle>,3
    1.16 vcs client write opcode: Set Absolute Volume               ATBC=cap_cmd,commander,vcp,write,<conn_handle>,4,<volume_setting> 
    1.17 vcs client write opcode:Unmute                             ATBC=cap_cmd,commander,vcp,write,<conn_handle>,5
    1.18 vcs client write opcode: Mute                              ATBC=cap_cmd,commander,vcp,write,<conn_handle>,6
    1.19 vocs client get char value                                 ATBC=cap_cmd,commander,vocs,get_char,<conn_handle>
    1.20 vocs client get srv data                                   ATBC=cap_cmd,commander,vocs,get_srv,<conn_handle>
    1.21 vocs client write output description                       ATBC=cap_cmd,commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    1.22 vocs client write opcode: Set Volume Offset                ATBC=cap_cmd,commander,vocs,write,<conn_handle>,1,<volume_offset>
    1.23 vocs client write opcode by group: Set Volume Offset       ATBC=cap_cmd,commander,vocs,gwrite,<group_index>,1,<volume_offset>
    1.24 mcs server send data：MEDIA_PLAYER_NAME                    ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.25 mcs server send data：TRACK_TITLE                          ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.26 mcs server send data：TRACK_DURATION                       ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.27 mcs server send data：TRACK_POSITION                       ATBC=cap_cmd,initiator,mcp,send,<uuid>,{<value>}
    1.28 mcs server send data：TRACK_CHANGED                        ATBC=cap_cmd,initiator,mcp,send,0x2B96

2.TMAP unicast media receiver
    2.1 config audio channel and csis neighbor addr             ATBC=tmap_cmd,umr,cfg,<sound_channel>,<neighbour_address>
    2.2 enable                                                  ATBE=tmap,umr,1
    2.3 disable                                                 ATBE=tmap,umr,0
    2.4 vcs server get param                                    ATBC=cap_cmd,acceptor,vcp,get
    2.5 vocs server get param                                   ATBC=cap_cmd,acceptor,vocs,get
    2.6 mcs client discovery                                    ATBC=cap_cmd,acceptor,mcp,dis,<conn_handle>
    2.7 mcs client read characteristic:MEDIA_PLAYER_NAME        ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B93
    2.8 mcs client read characteristic:TRACK_TITLE              ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B97
    2.9 mcs client read characteristic:TRACK_DURATION           ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B98
    2.10 mcs client read characteristic:TRACK_POSITION          ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2B99
    2.11 mcs client read characteristic:MEDIA_STATE             ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2BA3
    2.12 mcs client read characteristic:CONTENT_CONTROL_ID      ATBC=cap_cmd,acceptor,mcp,read,<conn_handle>,0x2BBA
    2.13 mcs client write opcode:PLAY                           ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x01
    2.14 mcs client write opcode:PAUSE                          ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x02
    2.15 mcs client write opcode:FAST_REWIND                    ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x03
    2.16 mcs client write opcode:FAST_FORWARD                   ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x04
    2.17 mcs client write opcode:STOP                           ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x05
    2.18 mcs client write opcode:MOVE_RELATIVE                  ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x10,<offset>
    2.19 mcs client write opcode:GOTO_SEGMENT                   ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x24,<n>
    2.20 mcs client write opcode:PREVIOUS_TRACK                 ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x30
    2.21 mcs client write opcode:NEXT_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x31
    2.22 mcs client write opcode:FIRST_TRACK                    ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x32
    2.23 mcs client write opcode:LAST_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x33
    2.24 mcs client write opcode:GOTO_TRACK                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x34,<n>
    2.25 mcs client write opcode:GOTO_GROUP                     ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,0x44,<n>
    2.26 mcs client enable CCCD                                 ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,<cfg_cccd>,1
    2.27 mcs client disable CCCD                                ATBC=cap_cmd,acceptor,mcp,write,<conn_handle>,<cfg_cccd>,0

3.TMAP broadcast media sender
    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config                ATBC=bap_cmd,broadcast,source,qos_cfg,<qos_type>  
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                                      ATBE=tmap,bms,1
    3.2 disable                                                     ATBE=tmap,bms,0
    3.3 start ext scan                                              ATBC=bap_cmd,unicast,client,escan,1
    3.4  stop ext scan                                              ATBC=bap_cmd,unicast,client,escan,0
    3.5 connect to tmap broadcast media receiver                    ATBC=le_gap,conn,<address type>,<bd_addr>
    3.6 start broadcast stream                                      ATBC=bap_cmd,broadcast,source,start
    3.7 stop broadcast stream                                       ATBC=bap_cmd,broadcast,source,stop
    3.8 vcs client set mute by group                                ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.9 vcs client set unmute by group                              ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    3.10 vcs client change volume by group                          ATBC=cap_cmd,commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    3.11 vcs client get volume state                                ATBC=cap_cmd,commander,vcp,get,<conn_handle>
    3.12 vcs client write opcode: Relative Volume Down              ATBC=cap_cmd,commander,vcp,write,<conn_handle>,0
    3.13 vcs client write opcode: Relative Volume Up                ATBC=cap_cmd,commander,vcp,write,<conn_handle>,1
    3.14 vcs client write opcode: Unmute/Relative Volume Down       ATBC=cap_cmd,commander,vcp,write,<conn_handle>,2
    3.15 vcs client write opcode: Unmute/Relative Volume Up         ATBC=cap_cmd,commander,vcp,write,<conn_handle>,3
    3.16 vcs client write opcode: Set Absolute Volume               ATBC=cap_cmd,commander,vcp,write,<conn_handle>,4,<volume_setting> 
    3.17 vcs client write opcode:Unmute                             ATBC=cap_cmd,commander,vcp,write,<conn_handle>,5
    3.18 vcs client write opcode: Mute                              ATBC=cap_cmd,commander,vcp,write,<conn_handle>,6

4.TMAP broadcast media receiver
    4.1 config audio channel and csis neighbor addr                 ATBC=tmap_cmd,bmr,cfg,<sound channel>,{<neighbor addr>}
    4.2 enable                                                      ATBE=tmap,bmr,1
    4.3 disable                                                     ATBE=tmap,bmr,0
    4.4  scan broadcast source start                                ATBC=bap_cmd,broadcast,sink,escan,1
    4.5 PA sync && big sync with bap broadcast source               ATBC=bap_cmd,broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with bap broadcast source                    ATBC=bap_cmd,broadcast,sink,sync_term,<bd_addr type>,<bd_addr>


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
            broadcast source QoS type config                ATBC=bap_cmd,broadcast,source,qos_cfg,<qos_type>  
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    1.1 enable                                          ATBE=pbp,source,1
    1.2 disable                                         ATBE=pbp,source,0
    1.3 start broadcast stream                          ATBC=bap_cmd,broadcast,source,start
    1.4 stop broadcast stream                           ATBC=bap_cmd,broadcast,source,stop

2.PBP broadcast sink
    2.1 configure broadcast sink sound channel          ATBC=pbp_cmd,sink,cfg,<sound_channel>
    2.2 enable                                          ATBE=pbp,sink,1
    2.3 disable                                         ATBE=pbp,sink,0
    2.4 scan broadcast source start                     ATBC=bap_cmd,broadcast,sink,escan,1
    2.5 PA sync && big sync with PBP broadcast source   ATBC=bap_cmd,broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    2.6 terminate sync with PBP broadcast source        ATBC=bap_cmd,broadcast,sink,sync_term,<bd_addr type>,<bd_addr>

3.PBP broadcast assistant
    3.1 enable                                                      ATBE=pbp,assistant,1
    3.2 disable                                                     ATBE=pbp,assistant,0
    3.3 start ext scan                                              ATBC=bap_cmd,unicast,client,escan,1
    3.4 stop ext scan                                               ATBC=bap_cmd,unicast,client,escan,0
    3.5 connect to PBP broadcast sink                               ATBC=le_gap,conn,<peer_addr_type>,<peer_addr>
    3.6 broadcast remote scan start                                 ATBC=bap_cmd,broadcast,assistant,bass_scan,1,1,0
    3.7 broadcast remote scan stop                                  ATBC=bap_cmd,broadcast,assistant,bass_scan,0,1,0
    3.8 PBP broadcast assistant pa sync with broadcast stream       ATBC=bap_cmd,broadcast,assistant,sync_start,<adv_addr type>,<adv_addr>
    3.9 reception start                                             ATBC=bap_cmd,broadcast,assistant,reception_start,<adv_addr type>,<adv_addr>,<group_idx>
    3.10 reception stop                                             ATBC=bap_cmd,broadcast,assistant,reception_stop,<adv_addr type>,<adv_addr>,<group_idx>
    3.11 reception remove                                           ATBC=bap_cmd,broadcast,assistant,reception_remove,<adv_addr type>,<adv_addr>,<group_idx>

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
        unicast session prefer QoS type config          ATBC=bap_cmd,unicast,client,session_qos,<qos_type>
                    <qos_type> 0: RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
                    <qos_type> 1: RTK_BT_LE_QOS_CFG_CIS_HIG_RELIABILITY
        unicast ase target QoS config                   ATBC=bap_cmd,unicast,client,ase_qos,<target_qos>
                    <target_qos> 0x01: RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
                    <target_qos> 0x02: RTK_BLE_AUDIO_ASCS_ASE_TARGET_BALANCED
                    <target_qos> 0x03: RTK_BLE_AUDIO_ASCS_ASE_TARGET_HIGHER_RELIABILITY
    1.1 enable                                                        ATBE=gmap,ugg,1
    1.2 disable                                                       ATBE=gmap,ugg,0
    1.3 start ext scan                                                ATBC=bap_cmd,unicast,client,escan,1
    1.4 stop ext scan                                                 ATBC=bap_cmd,unicast,client,escan,0
    1.5 connect to GMAP Unicast Game Receiver                         ATBC=le_gap,conn,<address type>,<bd_addr>
    1.6 start unicast unidirectional stream                           ATBC=bap_cmd,unicast,client,start,<group id>,<stream mode>  
    1.7 stop unicast unidirectional  stream                           ATBC=bap_cmd,unicast,client,stop,<group id>
    1.8 vcs client set mute by group                                  ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.9 vcs client set unmute by group                                ATBC=cap_cmd,commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.10 vcs client change volume by group                            ATBC=cap_cmd,commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.11 vcs client get volume state                                  ATBC=cap_cmd,commander,vcp,get,<conn_handle>
    1.12 vcs client write opcode: Relative Volume Down                ATBC=cap_cmd,commander,vcp,write,<conn_handle>,0
    1.13 vcs client write opcode: Relative Volume Up                  ATBC=cap_cmd,commander,vcp,write,<conn_handle>,1
    1.14 vcs client write opcode: Unmute/Relative Volume Down         ATBC=cap_cmd,commander,vcp,write,<conn_handle>,2
    1.15 vcs client write opcode: Unmute/Relative Volume Up           ATBC=cap_cmd,commander,vcp,write,<conn_handle>,3
    1.16 vcs client write opcode: Set Absolute Volume                 ATBC=cap_cmd,commander,vcp,write,<conn_handle>,4,<volume_setting> 
    1.17 vcs client write opcode:Unmute                               ATBC=cap_cmd,commander,vcp,write,<conn_handle>,5
    1.18 vcs client write opcode: Mute                                ATBC=cap_cmd,commander,vcp,write,<conn_handle>,6
    1.19 vocs client get char value                                   ATBC=cap_cmd,commander,vocs,get_char,<conn_handle>
    1.20 vocs client get srv data                                     ATBC=cap_cmd,commander,vocs,get_srv,<conn_handle>
    1.21 vocs client write output description                         ATBC=cap_cmd,commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    1.22 vocs client write opcode: Set Volume Offset                  ATBC=cap_cmd,commander,vocs,write,<conn_handle>,1,<volume_offset>
    1.23 vocs client write opcode by group: Set Volume Offset         ATBC=cap_cmd,commander,vocs,gwrite,<group_index>,1,<volume_offset>
    1.24 mics client set unmute by group                              ATBC=cap_cmd,commander,micp,gmute,<group_idx>,0
    1.25 mics client set mute by group                                ATBC=cap_cmd,commander,micp,gmute,<group_idx>,1
    1.26 mics client set unmute                                       ATBC=cap_cmd,commander,micp,set_mute,<conn_handle>,0
    1.27 mics client set mute                                         ATBC=cap_cmd,commander,micp,set_mute,<conn_handle>,1
    1.28 mics client get mute                                         ATBC=cap_cmd,commander,micp,get_mute,<conn_handle>
    1.29 aics client get char value                                   ATBC=cap_cmd,commander,aics,get_char,<conn_handle>
    1.30 aics client get srv data                                     ATBC=cap_cmd,commander,aics,get_srv,<conn_handle>
    1.31 aics client write opcode:SET_GAIN_SETTING                    ATBC=cap_cmd,commander,aics,write,<conn_handle>,1,<gaining_setting>
    1.32 aics client write opcode:UNMUTE                              ATBC=cap_cmd,commander,aics,write,<conn_handle>,2
    1.33 aics client write opcode:MUTE                                ATBC=cap_cmd,commander,aics,write,<conn_handle>,3
    1.34 aics client write opcode:SET_MANUAL_GAIN_MODE                ATBC=cap_cmd,commander,aics,write,<conn_handle>,4
    1.35 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE    ATBC=cap_cmd,commander,aics,write,<conn_handle>,5
    1.36 aics client write opcode by group:SET_GAIN_SETTING           ATBC=cap_cmd,commander,aics,gwrite,0,1,<gaining_setting>
    1.37 aics client write opcode by group:UNMUTE                     ATBC=cap_cmd,commander,aics,gwrite,0,2
    1.38 aics client write opcode by group:MUTE                       ATBC=cap_cmd,commander,aics,gwrite,0,3
    1.39 aics client write opcode by group:SET_MANUAL_GAIN_MODE       ATBC=cap_cmd,commander,aics,gwrite,0,4
    1.40 aics client write opcode by group:SET_AUTOMATIC_GAIN_MODE    ATBC=cap_cmd,commander,aics,gwrite,0,5

2.GMAP Unicast Game Terminal
    2.1 configure sound channel and neighbour address           ATBC=gmap_cmd,ugt,cfg,<sound channel>,<neighbour address>
    2.2 enable                                                  ATBE=gmap,ugt,1
    2.3 disable                                                 ATBE=gmap,ugt,0
    2.4 vcs server get param                                    ATBC=cap_cmd,acceptor,vcp,get
    2.5 mics server get param                                   ATBC=cap_cmd,acceptor,micp,get
    2.6 vocs server get param                                   ATBC=cap_cmd,acceptor,vocs,get
    2.7 aics server get param                                   ATBC=cap_cmd,acceptor,aics,get,<srv_instance_id>,<param_type> 
                                                                                                -->srv_instance_id <= RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM 
                                                                                                -->param_type: refer to rtk_bt_le_audio_aics_param_type_t except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES

3.GMAP Broadcast Game Sender
    For the GMAP BGS only support sample rate 48kHz and 32kHz
    Please configure the following Macros in app_bt_le_audio_common.h
        LEA_SOURCE_FIX_SAMPLE_FREQUENCY  to  RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
        or                               to  RTK_BT_LE_SAMPLING_FREQUENCY_CFG_32K

    QoS Config: must use before start broadcast stream 
            broadcast source QoS type config                ATBC=bap_cmd,broadcast,source,qos_cfg,<qos_type>
                        <qos_type> 2: RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY   
                        <qos_type> 3: RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
    3.1 enable                                              ATBE=gmap,bgs,1
    3.2 disable                                             ATBE=gmap,bgs,0
    3.3 start broadcast stream                              ATBC=bap_cmd,broadcast,source,start
    3.4 stop broadcast stream                               ATBC=bap_cmd,broadcast,source,stop

4.GMAP Broadcast Game Receiver
    4.1 configure sound channel and neighbour address           ATBC=gmap_cmd,bgr,cfg,<sound channel>,<neighbour address>
    4.2 enable                                                  ATBE=gmap,bgr,1
    4.3 disable                                                 ATBE=gmap,bgr,0
    4.4 scan broacast source start                              ATBC=bap_cmd,broadcast,sink,escan,1
    4.5 sync with broadcast source                              ATBC=bap_cmd,broadcast,sink,sync_start,<bd_addr type>,<bd_addr>
    4.6 terminate sync with broadcast source                    ATBC=bap_cmd,broadcast,sink,sync_term,<bd_addr type>,<bd_addr>