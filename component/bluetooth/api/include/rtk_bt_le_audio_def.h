/**
 * @file      rtk_bt_le_audio_def.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_LE_AUDIO_DEF_H__
#define __RTK_BT_LE_AUDIO_DEF_H__

#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
/*                                            user cfg max config start                                     */
#define RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM 2
#define RTK_BT_LE_AUDIO_CIG_MAX_NUM 2
#define RTK_BT_LE_AUDIO_CIS_MAX_NUM 4
#define RTK_BT_LE_AUDIO_CCID_MAX_NUM 4
#define RTK_BT_LE_AUDIO_PA_ADV_SET_MAX_NUM 4
#define RTK_BT_LE_AUDIO_SYNC_HANDLE_MAX_NUM 4
#define RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM 2
#define RTK_BT_LE_AUDIO_RECEIVER_BIG_MAX_NUM 2
#define RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM 4
#define RTK_BT_LE_AUDIO_BROADCASTER_GROUP_MAX_NUM 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_ADV_SID 0
#define RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE 1
#define RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE 1
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM 1 /* should be <= RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM within rtk_bt_bap.h */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM 1 /* should be <= RTK_BT_LE_AUDIO_BROADCASTER_SUBGROUP_MAX_NUM within rtk_bt_bap.h */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM 1
/*                                             user cfg max config end                                   */
/*                                              user cfg media start                                     */
/* stream source config */
#define RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT 1
#define RTK_BLE_AUDIO_RECORD_SUPPORT 0
#define RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT 0
#define RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
/* Define Audio volume.*/
#define RTK_BT_DEFAULT_AUDIO_TRACK_LEFT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_TRACK_RIGHT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_HARDWARE_LEFT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_HARDWARE_RIGHT_VOLUME (0.8)
/*                                               user cfg media end                                      */
/*                                             user cfg unicast start                                     */
/* user le audio config */
/* user codec config */
/* user session qos config */
#define RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT (RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT | \
                                           RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT | \
                                           RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT)
/*  !!!RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER should be associated with RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG   */
/*  !!!RTK_BT_LE_MEDIA_CODEC_CFG_PREFER should be associated with RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM   */
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT
#else
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT
#endif
#if defined(RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE) && RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE
#undef RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT
#endif
#define RTK_BT_LE_MEDIA_CODEC_CFG_PREFER                     RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT
#define RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG            RTK_BT_LE_UNICAST_AUDIO_CFG_4
#define RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM               RTK_BT_LE_CODEC_CFG_ITEM_48_2
#define RTK_BT_LE_AUDIO_DEFAULT_QOS_CFG_TYPE                 RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
#define RTK_BT_LE_AUDIO_DEFAULT_UNICAST_ASE_TARGET_LATENCY   RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
/*                                              user cfg unicast end                                      */
/*                                              user cfg le audio ISO interval config start              */
#define RTK_BT_ISO_INTERVAL_10_MS                            0x01
#define RTK_BT_ISO_INTERVAL_20_MS                            0x02
#define RTK_BT_ISO_INTERVAL_30_MS                            0x03
#define RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG              RTK_BT_ISO_INTERVAL_10_MS
#define RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG              RTK_BT_ISO_INTERVAL_10_MS
/*                                              user cfg le audio ISO interval config end                */
/*                                                user cfg broadcast start                               */
/* broadcast init parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_CODEC_CFG               RTK_BT_LE_CODEC_CFG_ITEM_48_2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_QOS_CFG                 RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONETXTS_CFG   RTK_BT_LE_AUDIO_CONTEXT_MEDIA
/* broadcast create parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SDU_INTERVAL 10000
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_MAX_SDU 155
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_LATENCY 10
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_RTN 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_PHY 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_FRAME_DURATION RTK_BT_LE_FRAME_DURATION_CFG_10_MS
/* pa sync create parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SYNC_TIMEOUT  100
/** @brief  Default minimum primary advertising interval when device is discoverable (units of 0.625ms, 96=60ms) */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN (192)
/** @brief  Default maximum primary advertising interval */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX (384)
/** @brief  Default minimum periodic advertising interval when device is discoverable (units of 1.25 ms, 48=60ms) */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MIN (192)
/** @brief  Default maximum periodic advertising interval */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MAX (192)
/*                                              user cfg broadcast end                                   */
/*                                             user cfg le audio combo demo configuration start          */
#define RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE     RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
#define RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_CODEC_CFG        RTK_BT_LE_CODEC_CFG_ITEM_48_2
#define RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_QOS_CFG          RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
/*                                             user cfg le audio combo demo configuration end            */
/*                                                   cap cfg start                                       */
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/* Define Audio VCS and MICS value.*/
#define RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL 15
#define RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING 255
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_SETTING 153
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_MUTE RTK_BT_LE_AUDIO_MICS_NOT_MUTE
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_CHG_CNT 0
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_FLAG 0
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE (RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING/RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL)
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
#define RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT 10000
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_INACTIVE 0
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_ACTIVE 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM (RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM + RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM)
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX 0
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX 1
#define RTK_BT_LE_AUDIO_BLUETOOTH_DES "Ameba Bluetooth"
#define RTK_BT_LE_AUDIO_MIC_DES "Ameba MIC"
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/* Define Audio channel.*/
#define RTK_BT_LE_AUDIO_LEFT 1
#define RTK_BT_LE_AUDIO_RIGHT 2
#define RTK_BT_LE_AUDIO_STEREO 3
#define RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM 1
#define RTK_BT_LE_AUDIO_VOCS_DES "Ameba Speaker"
#define RTK_BT_LE_AUDIO_VOCS_IDX 0
#define RTK_BT_LE_AUDIO_VOCS_MAX_VAL 255
#endif
#if (defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT)
/* Define AICS Audio Input Status Type.*/
#define RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH 32
#endif
#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
#define RTK_BT_LE_AUDIO_GMCS_CCID 0x20
#endif
#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
#define RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX 2
#endif
/*                                                    cap cfg end                                           */
#endif

/**
 * @defgroup  bt_le_audio BT LE AUDIO APIs
 * @brief     BT LE AUDIO related function APIs
 * @ingroup   BT_APIs
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_LE_AUDIO_DEF_H__ */