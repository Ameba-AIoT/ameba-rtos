/**
 * @file      rtk_bt_bap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_BAP_H__
#define __RTK_BT_BAP_H__

#include <rtk_bt_common.h>
#include <app_bt_le_audio_common.h>
#include <rtk_bt_le_audio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ------------------------------- Data Types ------------------------------- */
/********************************************bro source struct*************************************/
/**
 * @struct    rtk_bt_bap_bass_scan_dev_info_t
 * @brief     bap scan bass device infomation struct.
 */
typedef struct {
	rtk_bt_le_addr_t    adv_addr;
} rtk_bt_bap_bass_scan_dev_info_t;

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_bap BT BAP APIs
 * @brief     BT BAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

/**
 * @brief     start basic audio profile source.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_source_start(void);

/**
 * @brief     stop basic audio profile source.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_source_stop(void);

/**
 * @brief     enable / disable bap ext scan.
 * @param[in] enable: 0 for disable, 1 for enable
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_ext_scan(bool enable);

/**
 * @brief     bap broadcast sink sync start.
 * @param[in] p_info: ext scanned information
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_sync_start(rtk_bt_bap_bass_scan_dev_info_t *p_info);

/**
 * @brief     stop broadcast sink sync.
 * @param[in] padv_addr: bt address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_sync_term(rtk_bt_le_addr_t *padv_addr);

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
/**
 * @brief     config broadcast sink channel.
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_sink_cfg(uint8_t channel);
#endif

/**
 * @brief     enable / disable remote scan.
 * @param[in] enable: 0 for disable, 1 for enable
 * @param[in] remote_scan_enable: remote scan control 0 for disable(same as ext scan when 0), 1 for enable
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_bass_scan(bool enable, bool remote_scan_enable, uint8_t group_idx);

/**
 * @brief     start broadcast assistant sync.
 * @param[in] p_info: bass scanned information
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_assistant_sync_start(rtk_bt_bap_bass_scan_dev_info_t *p_info);

/**
 * @brief     terminate broadcast assistant sync.
 * @param[in] padv_addr: ble address
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_assistant_sync_term(rtk_bt_le_addr_t *padv_addr);

/**
 * @brief     start broadcast assistant reception.
 * @param[in] padv_addr: ble address
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_assistant_reception_start(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx);

/**
 * @brief     stop broadcast assistant reception.
 * @param[in] padv_addr: ble address
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_assistant_reception_stop(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx);

/**
 * @brief     remove broadcast assistant reception.
 * @param[in] padv_addr: ble address
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_broadcast_assistant_reception_remove(rtk_bt_le_addr_t *padv_addr, uint8_t group_idx);

/**
 * @brief     start bap unicast client procedure
 * @param[in] group_idx: group index
 * @param[in] play_mode: rtk_bt_le_audio_play_mode_t
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_unicast_client_start(uint8_t group_idx, rtk_bt_le_audio_play_mode_t play_mode);

/**
 * @brief     stop bap unicast client procedure
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_unicast_client_stop(uint8_t group_idx);

/**
 * @brief     release bap unicast client session stream procedure
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_unicast_client_release(uint8_t group_idx);

/**
 * @fn        uint16_t rtk_bt_bap_unicast_client_stream_session_release(uint8_t group_idx)
 * @brief     bap unicast client release stream session procedure
 * @param[in] group_idx: group index
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_unicast_client_stream_session_release(uint8_t group_idx);

#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
/**
 * @brief     config bap unicast server channel.
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_bap_unicast_server_cfg(uint8_t channel);
#endif

/**
 * @}
 */
#endif


#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_BAP_H__ */
