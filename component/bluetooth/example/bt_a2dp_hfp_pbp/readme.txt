##################################################################################
#                                                                                #
#                   A2DP sink and HFP HandFree and LE Audio PBP source demo      #
#                                                                                #
##################################################################################

Test Configuration
~~~~~~~~~~~
1. BIS num 1 + Sample rate 16kHz + 2-channel + ISO interval 20 ms 
    1.1 Config the following Macros in app_bt_le_audio_common.h
            change   RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG               to         RTK_BT_ISO_INTERVAL_20_MS
            change   RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE   to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K
            change   RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_CODEC_CFG      to         RTK_BT_LE_CODEC_CFG_ITEM_16_2
            change   RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE              to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K
2. BIS num 1 + Sample rate 48kHz + 1-channel + ISO interval 20 ms
    2.1 Config the following Macros in rtk_bt_le_audio_def.h
            change   RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG               to         RTK_BT_ISO_INTERVAL_20_MS
            change   RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE                to         0
            change   RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE              to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K
3. BIS num 1 + Sample rate 48kHz + 2-channel + ISO interval 30 ms 
    3.1 Config the following Macros in app_bt_le_audio_common.h
            change   RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG               to         RTK_BT_ISO_INTERVAL_30_MS
            change   RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE              to         RTK_BT_LE_SAMPLING_FREQUENCY_CFG_8K
GCC menuconfig 
~~~~~~~~~~~
1. BT Related:
    ./menuconfig.py --> CONFIG BT --> BT Example Demo --> BT A2DP HFP and LE Audio PBP
                                  --> BT Example Demo --> BLE Audio --> BLE Audio Public Broadcast Profile
2. Audio Related:
    ./menuconfig.py --> CONFIG Application --> Audio Config --> Select Audio Interfaces (Mixer)
3. GCC : use CMD "./build.py" to compile example

Test ATCMD
~~~~~~~~~~~
If user want to open audio local play fuction, must change the Maros in rtk_bt_le_audio_def.h before test:
    change the RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT   to  1
1. A2DP sink + AVRCP + HFP handfree + PBAP + Auracast demo 
    1.1 enable: AT+BTDEMO=a2dp_hfp_pbp,1
    1.2 disable: AT+BTDEMO=a2dp_hfp_pbp,0
    1.3 use Phone to connect to the device named "A2DP HFP Auracast"
    1.4 start LE Audio PBP broadcast: AT+BLEBAP=broadcast_start
    1.5 stop LE Audio PBP broadcast: AT+BLEBAP=broadcast_stop
For HFP HF part:
    1.6 HFP HF call answer: AT+BTHFP=call_answer,<peer_phone_bd_addr>
    1.7 HFP HF call terminate: AT+BTHFP=call_terminate,<peer_phone_bd_addr>
For AVRCP part:
    1.8 get album information, aritist name.etc: 
                AT+BTAVRCP=element_attr,<bd_addr>,1
                AT+BTAVRCP=element_attr,<bd_addr>,2
                AT+BTAVRCP=element_attr,<bd_addr>,3
                AT+BTAVRCP=element_attr,<bd_addr>,4
                AT+BTAVRCP=element_attr,<bd_addr>,5
                AT+BTAVRCP=element_attr,<bd_addr>,6
                AT+BTAVRCP=element_attr,<bd_addr>,7
        get cover art:
                AT+BTAVRCP=cover_art_conn,<bd_addr>
                AT+BTAVRCP=element_attr,<bd_addr>,8
For PBAP part:
    1.9 connect: AT+BTPBAP=conn,<bd_addr>
    1.10 disconnect: AT+BTPBAP=disconn,<bd_addr>
    1.11 get phone book object: AT+BTPBAP=get_pb_size,<bd_addr>,<repository>,<phone_book_object> 
                        // <repository> refer to rtk_bt_pbap_repository
                        // <phone_book_object> refer to RTK_BT_PBAP_PROPERTY_MASK
                        // For example, get local repository Main book object:  AT+BTPBAP=get_pb_size,fc5b8c1aedd8,1,0
    1.12 get phone book detailed information: AT+BTPBAP=pull_pb,<bt_mac>,<repository>,<phone_book_object>,<offset>,<size>,<mask> 
                        // For example AT+BTPBAP=pull_pb,fc5b8c1aedd8,1,0,0,128,0x82

2.PBP sink demo (peer device)
    2.1 enable: AT+BTDEMO=pbp,sink,1,<sound channel> sound_channel:{left, right, stereo}
    2.2 stop ext adv before ext scan: AT+BLEGAP=eadv,0,0
    2.3 scan for the PBP broadcast: AT+BLEBAP=escan,1
    2.4 create PA sync to the PBP broadcast: AT+BLEBAP=pa_sync_create,<bd_addr type>,<bd_addr>,<adv_sid>,<broadcast_id> 
                                             e.g. AT+BLEBAP=pa_sync_create,0,00e04c8002eb,0,123abc
    2.5 create BIG sync to the PBP broadcast: AT+BLEBAP=big_sync_create,<bd_addr type>,<bd_addr>

3. Phone 
    3.1 Use Phone to connect to the device "A2DP HFP Auracast"
    3.2 Play music on the Phone 
    3.3 Receive Phone call by HFP HF