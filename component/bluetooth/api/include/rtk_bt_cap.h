/**
 * @file      rtk_bt_cap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_CAP_H__
#define __RTK_BT_CAP_H__

#include <rtk_bt_common.h>
#include <rtk_bt_bap.h>
#include <app_bt_le_audio_common.h>
#include <rtk_bt_le_audio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_cap BT CAP APIs
 * @brief     BT CAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/**
 * @brief     media control server send data.
 * @param[in] char_uuid: char uuid, only support RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION,
 *                      RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED
 *                      track_value means track duration for RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION
 *                      track_value means track position for RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mcp_media_send(uint16_t char_uuid, int32_t track_value);
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/**
 * @brief     media control server discover.
 * @param[in] conn_handle: le connection handle
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mcp_media_discover(uint16_t conn_handle);

/**
 * @brief     media control data write.
 * @param[in] conn_handle: connection handle
 * @param[in] opcode: op code. @ref rtk_bt_le_audio_mcs_media_cp_opcode_t
 * @param[in] op_param: op code param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mcp_media_write(uint16_t conn_handle, uint8_t opcode, int32_t op_param);

/**
 * @brief     media control read char value.
 * @param[in] conn_handle: connection handle
 * @param[in] char_uuid: char_uuid, only support RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION,
 *                      RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mcp_media_read(uint16_t conn_handle, uint16_t char_uuid);

/**
 * @brief     media control config CCCD.
 * @param[in] conn_handle: connection handle
 * @param[in] cfg_cccd: cfg flags
 * @param[in] enable: enable
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mcp_media_cccd(uint16_t conn_handle, uint32_t cfg_cccd, bool enable);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/**
 * @brief     vcs parameter get.
 * @param[out] p_vcs_param: vcs param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vcp_server_get(rtk_bt_le_audio_vcs_param_t *p_vcs_param);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @brief     vcp change mute state.
 * @param[in] group_idx: group index
 * @param[in] vcs_mute: vcs mute
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vcp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_vcs_mute_state_t vcs_mute);

/**
 * @brief     vcp change volume setting.
 * @param[in] group_idx: group index
 * @param[in] volume_setting: volume setting
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vcp_client_gvolume(uint8_t group_idx, uint8_t volume_setting);

/**
 * @brief     vcs send volume cp.
 * @param[in] conn_handle: connection handle
 * @param[in] cp_op: cp op code
 * @param[in] volume_setting: volume setting
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vcp_client_write(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, uint8_t volume_setting);

/**
 * @brief     vcs get volume state.
 * @param[in] conn_handle: connection handle
 * @param[out] p_volume_state: the returned vcs volume state
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vcp_client_get(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_volume_state);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
/**
 * @brief     vcs get volume state.
 * @param[out] p_mics_param: returned mics param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_micp_server_get(rtk_bt_le_audio_mics_param_t *p_mics_param);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @brief     mics change mute state.
 * @param[in] group_idx: group index
 * @param[in] mic_mute: mic mute
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_micp_client_gmute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     mics set mute value.
 * @param[in] conn_handle: connect handle
 * @param[in] mic_mute: mic mute
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_micp_client_set_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     mics set mute value.
 * @param[in] conn_handle: connect handle
 * @param[out] p_mic_mute: returned mic mute
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_micp_client_get_mute(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute);
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @brief     vocs get parameters.
 * @param[out] p_vocs_param: returned vocs parameters
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_server_get(rtk_bt_le_audio_vocs_param_t *p_vocs_param);

/**
 * @brief     vocs write control point.
 * @param[in] conn_handle: connect handle
 * @param[in] cp_op: @ref rtk_bt_le_audio_vocs_cp_op_t
 * @param[in] volume_offset: volume offset
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_client_write(uint16_t conn_handle, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset);

/**
 * @brief     vocs write control point by group.
 * @param[in] group_idx: group index
 * @param[in] cp_op: @ref rtk_bt_le_audio_vocs_cp_op_t
 * @param[in] volume_offset: volume offset
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_vocs_cp_op_t cp_op, int16_t volume_offset);

/**
 * @brief     vocs write output descriptor.
 * @param[in] conn_handle: connect handle
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] des_str: descriptor.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str);

/**
 * @brief     vocs get parameters.
 * @param[in] conn_handle: connect handle
 * @param[out] p_srv_data: returned srv data
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data);

/**
 * @brief     vocs get char value.If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND
 * @param[in] conn_handle: connect handle
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_vocs_client_get_char(uint16_t conn_handle);
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @brief     aics get parameters
 * @param[in] srv_instance_id: service instance id
 * @param[in] aics_param_type: aics param type
 * @param[in] value_len: length of value
 * @param[out] p_value: returned value
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_server_get(uint8_t srv_instance_id, uint8_t aics_param_type, uint8_t value_len, uint8_t *p_value);

/**
 * @brief     aics write control point.
 * @param[in] conn_handle: connect handle
 * @param[in] cp_op: @ref rtk_bt_le_audio_aics_cp_op_t
 * @param[in] gaining_setting: gaining offset
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_client_write(uint16_t conn_handle, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting);

/**
 * @brief     aics group write control point.
 * @param[in] group_idx: group index
 * @param[in] cp_op: @ref rtk_bt_le_audio_aics_cp_op_t
 * @param[in] gaining_setting: gaining offset
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_client_gwrite(uint8_t group_idx, rtk_bt_le_audio_aics_cp_op_t cp_op, int8_t gaining_setting);

/**
 * @brief     aics write input descriptor.
 * @param[in] conn_handle: connect handle
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] des_str: descriptor.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_client_wdes(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t *des_str);

/**
 * @brief     aics get parameters.
 * @param[in] conn_handle: connect handle
 * @param[out] p_srv_data: returned srv data
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_client_get_srv(uint16_t conn_handle, rtk_bt_le_audio_aics_srv_data_t *p_srv_data);

/**
 * @brief     aics get char value.If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND
 * @param[in] conn_handle: connect handle
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_aics_client_get_char(uint16_t conn_handle);
#endif

/**
 * @brief     config acceptor channel or neighbor device
 * @param[in] channel: channel allocation, 1 for left, 2 for right, 3 for stereo
 * @param[in] p_neighbor_addr: pointer of neghbor device bt address, NULL for no neighbor
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_cap_acceptor_cfg(uint8_t channel, uint8_t *p_neighbor_addr);

/**
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_CAP_H__ */