##################################################################################
#                                                                                #
#                      Generic LE Audio demo                   #
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
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BLE Audio --> BLE Audio Generic Demo
2. Audio Related:
    ./menuconfig.py --> MENUCONFIG FOR CA32 CONFIG --> Audio Config --> Select Audio Interfaces (Mixer)
    or
    ./menuconfig.py --> MENUCONFIG FOR KM4 CONFIG --> Audio Config --> Select Audio Interfaces (Mixer)
3. GCC : use CMD "./build.py" to compile example
4. To Config Media Source (component/bluetooth/api/include/rtk_bt_le_audio_def.h)
     4.1 Config RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT to 1 for enabling birds thing audio stream
     4.2 Config RTK_BLE_AUDIO_RECORD_SUPPORT to 1 for enabling microphone audio stream
           ** RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT and RTK_BLE_AUDIO_RECORD_SUPPORT should not be enabled both
     4.3 Config RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE to change birds thing audio stream frequency
     4.4 Config RTK_BT_LE_MEDIA_CODEC_CFG_PREFER to change le audio codec config, this parameter should be related to RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE(Frequency should be equal to Audio Source Frequency)
     4.5 Config RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER to change le audio config type(Default is RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT for single channel and RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT for two channels)
     ** default is 16k birdsthing audio source for 10ms duration (------->>), two channels example.

Generic LE Audio demo ATCMD:
~~~~~~~~~~~
1.CAP Initiator role
    1.1 enable                                      AT+BTDEMO=generic_le_audio_demo,initiator,1
    1.2 disable                                      AT+BTDEMO=generic_le_audio_demo,initiator,0
    1.3 scan start                                  AT+BLEBAP=escan,1
    1.4 scan stop                                   AT+BLEBAP=escan,0
    1.5 connect                                     AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.6 start unicast stream                        AT+BLEBAP=client_start,<group_idx>,<sink_context>,<source_context> ex: AT+BLEBAP=client_start,0,4,0   4 for RTK_BT_LE_AUDIO_CONTEXT_MEDIA
    1.7 stop unicast stream                         AT+BLEBAP=client_stop,<group_idx>,1000
    1.8 release unicast stream                      AT+BLEBAP=client_release,<group_idx>
    1.9 start broadcast stream                      AT+BLEBAP=broadcast_start
    1.10 stop broadcast stream                      AT+BLEBAP=broadcast_stop

2.CAP Acceptor role
    2.1 enable                                          AT+BTDEMO=generic_le_audio_demo,acceptor,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 disable                                         AT+BTDEMO=generic_le_audio_demo,acceptor,0
    2.3 stop ext adv before ext scan                    AT+BLEGAP=eadv,0,0
    2.4 scan start                                      AT+BLEBAP=escan,1
    2.5 create pa sync with broadcast source            AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    2.6 terminate pa sync with broadcast source         AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    2.7 create big sync with broadcast source           AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>
    2.8 terminate big sync with broadcast source        AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>

3.CAP Commander role
    3.1 enable                                             AT+BTDEMO=generic_le_audio_demo,commander,1
    3.2 disable                                            AT+BTDEMO=generic_le_audio_demo,commander,0
    3.3 scan start                                         AT+BLEBAP=escan,1
    3.4 scan stop                                          AT+BLEBAP=escan,0
    3.5 connect                                            AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    3.6 start remote scan                                  AT+BLEBAP=remote_scan,1,<group_idx>
    3.7 start remote scan                                  AT+BLEBAP=remote_scan,1,<group_idx>
    3.8 create pa sync with broadcast source               AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    3.9 terminate pa sync with broadcast source            AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    3.10 reception_start                                   AT+BLEBAP=reception_start,<bd_addr type>,<bd_addr>,<group_idx>
    3.11 reception_stop                                    AT+BLEBAP=reception_stop,<bd_addr type>,<bd_addr>,<group_idx>
    3.12 reception_remove                                  AT+BLEBAP=reception_remove,<bd_addr type>,<bd_addr>,<group_idx>

##################################################################################
#                                                                                #
#                           PBP demo                                             #
#                                                                                #
##################################################################################
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
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BLE Audio --> BLE Audio Telephony and Media Audio Profile
2. Audio Related:
    ./menuconfig.py --> MENUCONFIG FOR CA32 CONFIG --> Audio Config --> Select Audio Interfaces (Mixer)
    or
    ./menuconfig.py --> MENUCONFIG FOR KM4 CONFIG --> Audio Config --> Select Audio Interfaces (Mixer)
3. GCC : use CMD "./build.py" to compile example

TMAP demo ATCMD:
~~~~~~~~~~~
1.TMAP unicast media sender
    1.1 enable                                                      AT+BTDEMO=tmap,ums,1
    1.2 disable                                                     AT+BTDEMO=tmap,ums,0
    1.3 connect                                                     AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.4 start unicast stream                                        AT+BLEBAP=client_start,<group_idx>,<sink_context>,<source_context> ex: AT+BLEBAP=client_start,0,4,0   4 for RTK_BT_LE_AUDIO_CONTEXT_MEDIA
    1.5 stop unicast stream                                         AT+BLEBAP=client_stop,<group_idx>,1000
    1.6 vcs client set mute by group                                AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.7 vcs client set unmute by group                              AT+BLECAP=commander,vcp,gmute,<group_id>,<mute_state> -->group_id is 0, mute_state is 0,1 (0:not mute, 1:mute)
    1.8 vcs client change volume by group                           AT+BLECAP=commander,vcp,gvolume,<group_id>,<volume_setting> -->group_id is 0, volume_setting is 0~255
    1.9 vcs client get volume state                                 AT+BLECAP=commander,vcp,get,<conn_handle>
    1.10 vcs client write opcode: Relative Volume Down              AT+BLECAP=commander,vcp,write,<conn_handle>,0
    1.11 vcs client write opcode: Relative Volume Up                AT+BLECAP=commander,vcp,write,<conn_handle>,1
    1.12 vcs client write opcode: Unmute/Relative Volume Down       AT+BLECAP=commander,vcp,write,<conn_handle>,2
    1.13 vcs client write opcode: Unmute/Relative Volume Up         AT+BLECAP=commander,vcp,write,<conn_handle>,3
    1.14 vcs client write opcode: Set Absolute Volume               AT+BLECAP=commander,vcp,write,<conn_handle>,4,<volume_setting> 
    1.15 vcs client write opcode:Unmute                             AT+BLECAP=commander,vcp,write,<conn_handle>,5
    1.16 vcs client write opcode: Mute                              AT+BLECAP=commander,vcp,write,<conn_handle>,6
    ~~~~~~~~~~~~~~~~~~~~ <srv_instance_id> can be read from event RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_DISCOVERY_DONE_IND srv num ~~~~~~~~~~~~~~~~~~~
    1.17 vocs client get char value                                 AT+BLECAP=commander,vocs,get_char,<conn_handle>,<srv_instance_id>
    1.18 vocs client get srv data                                   AT+BLECAP=commander,vocs,get_srv,<conn_handle>,<srv_instance_id>
    1.19 vocs client write output description                       AT+BLECAP=commander,vocs,wdes,<conn_handle>,<srv instance id>,<des_str>
    1.20 vocs client write opcode: Set Volume Offset                AT+BLECAP=commander,vocs,write,<conn_handle>,<srv instance id>,<cp_op>,<volume_offset>
    1.21 vocs client write opcode by group: Set Volume Offset       AT+BLECAP=commander,vocs,gwrite,<group_index>,<srv instance id>,<cp_op>,<volume_offset>
    1.22 mcs server send data：MEDIA_PLAYER_NAME                    AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.23 mcs server send data：TRACK_TITLE                          AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.24 mcs server send data：TRACK_DURATION                       AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.25 mcs server send data：TRACK_POSITION                       AT+BLECAP=initiator,mcp,send,<uuid>,{<value>}
    1.26 mcs server send data：TRACK_CHANGED                        AT+BLECAP=initiator,mcp,send,0x2B96

2.TMAP unicast media receiver
    2.1 enable                                                      AT+BTDEMO=tmap,umr,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 disable                                                     AT+BTDEMO=tmap,umr,0
    2.3 vcs server get param                                        AT+BLECAP=acceptor,vcp,get
    2.4 vocs server get param                                       AT+BLECAP=acceptor,vocs,get
    2.5 mcs client discovery                                        AT+BLECAP=acceptor,mcp,dis,<conn_handle>
    2.6 mcs client read characteristic:MEDIA_PLAYER_NAME            AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B93
    2.7 mcs client read characteristic:TRACK_TITLE                  AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B97
    2.8 mcs client read characteristic:TRACK_DURATION               AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B98
    2.9 mcs client read characteristic:TRACK_POSITION               AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2B99
    2.10 mcs client read characteristic:MEDIA_STATE                 AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BA3
    2.11 mcs client read characteristic:CONTENT_CONTROL_ID          AT+BLECAP=acceptor,mcp,read,<conn_handle>,0x2BBA
    2.12 mcs client write opcode:PLAY                               AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x01
    2.13 mcs client write opcode:PAUSE                              AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x02
    2.14 mcs client write opcode:FAST_REWIND                        AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x03
    2.15 mcs client write opcode:FAST_FORWARD                       AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x04
    2.16 mcs client write opcode:STOP                               AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x05
    2.17 mcs client write opcode:MOVE_RELATIVE                      AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x10,<offset>
    2.18 mcs client write opcode:GOTO_SEGMENT                       AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x24,<n>
    2.19 mcs client write opcode:PREVIOUS_TRACK                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x30
    2.20 mcs client write opcode:NEXT_TRACK                         AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x31
    2.21 mcs client write opcode:FIRST_TRACK                        AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x32
    2.22 mcs client write opcode:LAST_TRACK                         AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x33
    2.23 mcs client write opcode:GOTO_TRACK                         AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x34,<n>
    2.24 mcs client write opcode:GOTO_GROUP                         AT+BLECAP=acceptor,mcp,write,<conn_handle>,0x44,<n>
    2.25 mcs client enable CCCD                                     AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,1
    2.26 mcs client disable CCCD                                    AT+BLECAP=acceptor,mcp,write,<conn_handle>,<cfg_cccd>,0

3.TMAP broadcast media sender
    3.1 enable                                                      AT+BTDEMO=tmap,bms,1
    3.2 disable                                                     AT+BTDEMO=tmap,bms,0
    3.3 start ext scan                                              AT+BLEBAP=escan,1
    3.4 stop ext scan                                               AT+BLEBAP=escan,0
    3.5 connect to tmap broadcast media receiver                    AT+BLEGAP=conn,<address type>,<bd_addr>
    3.6 start broadcast stream                                      AT+BLEBAP=broadcast_start
    3.7 stop broadcast stream                                       AT+BLEBAP=broadcast_stop
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
    4.1 enable                                                      AT+BTDEMO=tmap,bmr,1,<sound channel> sound_channel:{left, right, stereo}
    4.2 disable                                                     AT+BTDEMO=tmap,bmr,0
    4.3 scan broadcast source start                                 AT+BLEBAP=escan,1
    4.4 create pa sync with broadcast source                        AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    4.5 terminate pa sync with broadcast source                     AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    4.6 create big sync with broadcast source                       AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>
    4.7 terminate big sync with broadcast source                    AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>


##################################################################################
#                                                                                #
#                           GMAP demo                                            #
#                                                                                #
##################################################################################

GMAP demo ATCMD:
~~~~~~~~~~~
1.GMAP unicast game gateway
    1.1 enable                                                      AT+BTDEMO=gmap,ugg,1
    1.2 disable                                                     AT+BTDEMO=gmap,ugg,0
    1.3 start ext scan                                              AT+BLEBAP=escan,1
    1.4 start ext scan                                              AT+BLEBAP=escan,0
    1.5 connect                                                     AT+BLEGAP=conn,<peer_addr_type>,<peer_addr>
    1.6 start unicast stream                                        AT+BLEBAP=client_start,<group_idx>,<sink_context>,<source_context> 
                                                                    ex: AT+BLEBAP=client_start,0,8,0   8 for RTK_BT_LE_AUDIO_CONTEXT_GAME
    1.7 stop unicast stream                                         AT+BLEBAP=client_stop,<group_idx>,1000
2.GMAP unicast game terminal
    2.1 enable                                                      AT+BTDEMO=gmap,ugt,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 disable                                                     AT+BTDEMO=gmap,ugt,0
3.GMAP broadcast game sender
    3.1 enable                                                      AT+BTDEMO=gmap,bgs,1
    3.2 disable                                                     AT+BTDEMO=gmap,bgs,0
    3.3 start broadcast stream                                      AT+BLEBAP=broadcast_start
    3.4 stop broadcast stream                                       AT+BLEBAP=broadcast_stop

4.GMAP broadcast game receiver
    4.1 enable                                                      AT+BTDEMO=gmap,bgr,1,<sound channel> sound_channel:{left, right, stereo}
    4.2 disable                                                     AT+BTDEMO=gmap,bgr,0
    4.3 scan broadcast source start                                 AT+BLEBAP=escan,1
    4.4 create pa sync with broadcast source                        AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id>
    4.5 terminate pa sync with broadcast source                     AT+BLEBAP=pa_sync_remove,<bd_addr type>,<bd_addr>
    4.6 create big sync with broadcast source                       AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>
    4.7 terminate big sync with broadcast source                    AT+BLEBAP=big_sync_remove,<bd_addr type>,<bd_addr>