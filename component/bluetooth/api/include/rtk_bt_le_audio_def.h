/**
 * @file      rtk_bt_le_audio_def.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Audio user configuration definition.
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
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
/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_LE_Audio_Constants BT LE Audio Constants
 * @{
 */
/** @addtogroup BT_LE_Audio_Def_Constants BT LE Audio Def Constants
 * @{
 */
/*                                           user cfg max config start                                     */
/// @cond
#define RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM 2
#define RTK_BT_LE_AUDIO_CIG_MAX_NUM 2
#define RTK_BT_LE_AUDIO_CIS_MAX_NUM 4
#define RTK_BT_LE_AUDIO_CCID_MAX_NUM 4
#define RTK_BT_LE_AUDIO_PA_ADV_SET_MAX_NUM 4
#define RTK_BT_LE_AUDIO_SYNC_HANDLE_MAX_NUM 4
/// @endcond
/**
 * @def       RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM
 * @brief     Maximum number of BISes of a broadcast source.
 */
#define RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM 2
/// @cond
#define RTK_BT_LE_AUDIO_RECEIVER_BIG_MAX_NUM 2
#define RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM 4
#define RTK_BT_LE_AUDIO_BROADCASTER_GROUP_MAX_NUM 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_ADV_SID 0
/// @endcond
/**
 * @def       RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE
 * @brief     Enable the stereo mode of the unicast audio.
 * @note
 *            - 1: Enabled. 0: Disabled. Default value is 1.
 *            - If the deprecated RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE is defined by the user, its value is taken over.
 */
#if defined(RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE)
#define RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE
#else
#define RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE 1
/**
 * @def       RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE
 * @brief     Deprecated alias of @ref RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE.
 * @deprecated Use @ref RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE instead.
 */
#define RTK_BT_LE_AUDIO_UNICAST_SETEO_MODE RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE
#endif
/**
 * @def       RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE
 * @brief     Enable the stereo mode of the broadcast audio.
 * @note
 *            - 1: Enabled. 0: Disabled. Default value is 1.
 *            - It takes effect only when @ref RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM is 1.
 *              In this case, the only BIS carries both the left and right channels and the Max SDU of the BIG is doubled.
 *            - If the deprecated RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE is defined by the user, its value is taken over.
 */
#if defined(RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE)
#define RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE
#else
#define RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE 1
/**
 * @def       RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE
 * @brief     Deprecated alias of @ref RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE.
 * @deprecated Use @ref RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE instead.
 */
#define RTK_BT_LE_AUDIO_BROADCASTER_SETEO_MODE RTK_BT_LE_AUDIO_BROADCASTER_STEREO_MODE
#endif
/// @cond
#define RTK_BT_LE_AUDIO_ISO_TX_SYNC_SUPPORT 0
#define RTK_BT_LE_AUDIO_ISO_RX_SYNC_SUPPORT 1
/// @endcond
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM 1       /*!< Number of BISes of the broadcast source, <= @ref RTK_BT_LE_AUDIO_BROADCASTER_BIS_MAX_NUM. */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM 1 /* number of subgroups in a broadcast source group */
/// @cond
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM 1
/*                                            user cfg max config end                                   */
/*                                             user cfg media start                                     */
/* stream source config */
#define RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT 1
#define RTK_BLE_AUDIO_RECORD_SUPPORT 0
#define RTK_BLE_AUDIO_BROADCAST_LOCAL_PLAY_SUPPORT 0
#define RTK_BT_LE_AUDIO_BIRDS_SING_SAMPLING_RATE RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
/* Define Audio volume. */
#define RTK_BT_DEFAULT_AUDIO_TRACK_LEFT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_TRACK_RIGHT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_HARDWARE_LEFT_VOLUME (0.8)
#define RTK_BT_DEFAULT_AUDIO_HARDWARE_RIGHT_VOLUME (0.8)
/// @endcond
/*                                              user cfg media end                                      */
/*                                            user cfg unicast start                                     */
/* user LE Audio config */
/* user codec config */
/* user session qos config */
/**
 * @def       RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT
 * @brief     Bit mask of the unicast audio configurations supported when setting up a media audio stream.
 */
#define RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT (RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT | \
                                           RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT | \
                                           RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT)
/**
 * @def       RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER
 * @brief     Preferred unicast audio configuration bit mask when setting up a media audio stream.
 * @note
 *            - The audio configurations supported by the peer devices are filtered by @ref RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT.
 *              If the result contains this bit mask, only this bit mask is kept.
 *            - If @ref RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE is 1, it is forced to @ref RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT.
 *            - It is recommended to keep it consistent with @ref RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG.
 */
/// @cond
#if !defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || !RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT
#else
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT
#endif
/// @endcond
#if defined(RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE) && RTK_BT_LE_AUDIO_UNICAST_STEREO_MODE
#undef RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT
#endif
/**
 * @def       RTK_BT_LE_MEDIA_CODEC_CFG_PREFER
 * @brief     Preferred codec configuration item bit mask when setting up a media audio stream.
 * @note
 *            - If the codec configuration items supported by the peer device contain this bit mask, only this bit mask is kept.
 *            - It is recommended to keep it consistent with @ref RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM.
 */
#define RTK_BT_LE_MEDIA_CODEC_CFG_PREFER                     RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT
/**
 * @def       RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG
 * @brief     Default unicast audio configuration type of a group, used as the initial value when a device is added to the group.
 * @note
 *            - It is recommended to keep it consistent with @ref RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER.
 */
#define RTK_BT_LE_AUDIO_DEFAULT_UNICAST_AUDIO_CFG            RTK_BT_LE_UNICAST_AUDIO_CFG_4
/**
 * @def       RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM
 * @brief     Default codec configuration item of a group, used as the initial value when a device is added to the group.
 * @note
 *            - It is recommended to keep it consistent with @ref RTK_BT_LE_MEDIA_CODEC_CFG_PREFER.
 */
#define RTK_BT_LE_AUDIO_DEFAULT_CODEC_CFG_ITEM               RTK_BT_LE_CODEC_CFG_ITEM_48_2
/// @cond
#define RTK_BT_LE_AUDIO_DEFAULT_QOS_CFG_TYPE                 RTK_BT_LE_QOS_CFG_CIS_LOW_LATENCY
#define RTK_BT_LE_AUDIO_DEFAULT_UNICAST_ASE_TARGET_LATENCY   RTK_BLE_AUDIO_ASCS_ASE_TARGET_LOWER_LATENCY
/*                                             user cfg unicast end                                      */
/*                                             user cfg LE Audio ISO interval config start              */
#define RTK_BT_ISO_INTERVAL_10_MS                            0x01
#define RTK_BT_ISO_INTERVAL_20_MS                            0x02
#define RTK_BT_ISO_INTERVAL_30_MS                            0x03
#define RTK_BT_LE_AUDIO_CIG_ISO_INTERVAL_CONFIG              RTK_BT_ISO_INTERVAL_10_MS
#define RTK_BT_LE_AUDIO_BIG_ISO_INTERVAL_CONFIG              RTK_BT_ISO_INTERVAL_10_MS
/*                                             user cfg LE Audio ISO interval config end                */
/*                                               user cfg broadcast start                               */
/* broadcast init parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_CODEC_CFG               RTK_BT_LE_CODEC_CFG_ITEM_48_2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_QOS_CFG                 RTK_BT_LE_QOS_CFG_BIS_LOW_LATENCY
/// @endcond
/**
 * @def       RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONTEXTS_CFG
 * @brief     Default streaming audio contexts configuration of the broadcast source.
 * @note
 *            - Default value is @ref RTK_BT_LE_AUDIO_CONTEXT_MEDIA.
 */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONTEXTS_CFG   RTK_BT_LE_AUDIO_CONTEXT_MEDIA
/**
 * @def       RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONETXTS_CFG
 * @brief     Deprecated alias of @ref RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONTEXTS_CFG.
 * @deprecated Use @ref RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONTEXTS_CFG instead.
 */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONETXTS_CFG RTK_BT_LE_AUDIO_BROADCAST_SOURCE_STREAM_AUDIO_CONTEXTS_CFG
/// @cond
/* broadcast create parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SDU_INTERVAL 10000
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_MAX_SDU 155
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_LATENCY 10
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_RTN 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_PHY 2
#define RTK_BT_LE_AUDIO_BROADCAST_SOURCE_FRAME_DURATION RTK_BT_LE_FRAME_DURATION_CFG_10_MS
/* pa sync create parameter */
#define RTK_BT_LE_AUDIO_BROADCAST_SYNC_TIMEOUT  100
/// @endcond
/** @brief  Default minimum primary advertising interval when the device is discoverable. Time = N * 0.625 ms, 192 = 120 ms. */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN (192)
/** @brief  Default maximum primary advertising interval. Time = N * 0.625 ms, 384 = 240 ms. */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX (384)
/** @brief  Default minimum periodic advertising interval when the device is discoverable. Time = N * 1.25 ms, 192 = 240 ms. */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MIN (192)
/** @brief  Default maximum periodic advertising interval. Time = N * 1.25 ms, 192 = 240 ms. */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MAX (192)
/*                                             user cfg broadcast end                                   */
/*                                            user cfg LE Audio combo demo configuration start          */
/// @cond
#define RTK_BT_LE_AUDIO_COMPO_DEMO_AUDIO_STREAM_SAMPLE_RATE     RTK_BT_LE_SAMPLING_FREQUENCY_CFG_48K
#define RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_CODEC_CFG        RTK_BT_LE_CODEC_CFG_ITEM_48_2
#define RTK_BT_LE_AUDIO_COMPO_DEMO_DEFAULT_BIS_QOS_CFG          RTK_BT_LE_QOS_CFG_BIS_HIG_RELIABILITY
/// @endcond
/*                                            user cfg LE Audio combo demo configuration end            */
/*                                                  cap cfg start                                       */
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/* Define Audio VCS and MICS value. */
/// @cond
#define RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL 15
#define RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING 255
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_SETTING 153
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_MUTE RTK_BT_LE_AUDIO_MICS_NOT_MUTE
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_CHG_CNT 0
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_FLAG 0
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE (RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING/RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL)
/// @endcond
#endif
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/**
 * @def       RTK_BLE_AUDIO_DEFAULT_CSIS_DISCOVERY_TIMEOUT
 * @brief     Default timeout, in milliseconds, of the CSIS set member discovery.
 */
#define RTK_BLE_AUDIO_DEFAULT_CSIS_DISCOVERY_TIMEOUT 10000
/**
 * @def       RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT
 * @brief     Deprecated alias of @ref RTK_BLE_AUDIO_DEFAULT_CSIS_DISCOVERY_TIMEOUT.
 * @deprecated Use @ref RTK_BLE_AUDIO_DEFAULT_CSIS_DISCOVERY_TIMEOUT instead.
 */
#define RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT RTK_BLE_AUDIO_DEFAULT_CSIS_DISCOVERY_TIMEOUT
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/* Define AICS Audio Input Status Type. */
/// @cond
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_INACTIVE 0
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_ACTIVE 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM 1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM (RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM + RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM)
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX 0
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX 1
#define RTK_BT_LE_AUDIO_BLUETOOTH_DES "Ameba Bluetooth"
#define RTK_BT_LE_AUDIO_MIC_DES "Ameba MIC"
/// @endcond
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/* Define Audio channel. */
/// @cond
#define RTK_BT_LE_AUDIO_LEFT 1
#define RTK_BT_LE_AUDIO_RIGHT 2
#define RTK_BT_LE_AUDIO_STEREO 3
#define RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM 1
#define RTK_BT_LE_AUDIO_VOCS_DES "Ameba Speaker"
#define RTK_BT_LE_AUDIO_VOCS_IDX 0
#define RTK_BT_LE_AUDIO_VOCS_MAX_VAL 255
/// @endcond
#endif
#if (defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT)
/* Define maximum length of the characteristic description. */
#define RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH 32
#endif
#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
/// @cond
#define RTK_BT_LE_AUDIO_GMCS_CCID 0x20
/// @endcond
#endif
#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
/// @cond
#define RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX 2
/// @endcond
#endif
/*                                                   cap cfg end                                           */
/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_LE_AUDIO_DEF_H__ */
