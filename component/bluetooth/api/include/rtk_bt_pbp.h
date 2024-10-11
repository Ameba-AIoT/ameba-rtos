/**
 * @file      rtk_bt_pbp.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Audio Public Broadcast Profile type and function definition
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_PBP_H__
#define __RTK_BT_PBP_H__

#include <rtk_bt_le_audio.h>
#include <app_bt_le_audio_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_pbp BT PBP APIs
 * @brief     BT PBP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     config pbp broadcast sink channel.
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_pbp_broadcast_sink_cfg(uint8_t channel);

/**
 * @}
 */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_PBP_H__ */
