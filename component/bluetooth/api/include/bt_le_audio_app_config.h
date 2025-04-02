/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_LE_AUDIO_APP_CONFIG_H__
#define __BT_LE_AUDIO_APP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <rtk_bt_def.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <rtk_bt_le_gap.h>
#include <rtk_bt_bap.h>
#include <rtk_bt_cap.h>
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
#include <rtk_bt_tmap.h>
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
#include <rtk_bt_gmap.h>
#endif

#define RTK_BT_GAP_DEVICE_NAME_LEN (39+1)//!< Max length of device name, if device name length exceeds it, it will be truncated.

/**
 * @typedef   rtk_bt_le_audio_pacs_init_param_t
 * @brief     Bluetooth LE audio PACS init param
 */
typedef struct {
	uint32_t sink_audio_location;
	uint32_t source_audio_location;
	uint16_t sink_available_contexts;
	uint16_t sink_supported_contexts;
	uint16_t source_available_contexts;
	uint16_t source_supported_contexts;
	uint8_t *p_sink_pac_id;
	uint8_t *p_pac_sink_codec;
	uint16_t pac_sink_codec_len;
	uint8_t *p_source_pac_id;
	uint8_t *p_pac_source_codec;
	uint16_t pac_source_codec_len;
} rtk_bt_le_audio_pacs_init_param_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_init_param_t
 * @brief     Bluetooth LE audio ASCS init param
 */
typedef struct {
	uint8_t sink_ase_num;
	uint8_t source_ase_num;
} rtk_bt_le_audio_ascs_init_param_t;

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_csis_init_param_t
 * @brief     Bluetooth LE audio CSIS init param
 */
typedef struct {
	uint8_t csis_num;                                          /* config joined csis number, currently only support 1 */
	uint8_t csis_size;                                         /* config joined csis's member number, should be 2 */
	rtk_bt_le_audio_csis_cfg_t csis_cfg;                       /* left channel or right channel */
	uint8_t csis_feature;
	uint8_t csis_sirk_type;                                     /**< @ref rtk_bt_le_audio_csis_sirk_type_t  */
	uint8_t csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN];
} rtk_bt_le_audio_csis_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_aics_init_param_t
 * @brief     Bluetooth LE audio AICS init param
 */
typedef struct {
	uint8_t aics_vcs_num;
	uint8_t *p_aics_vcs_tbl;
	uint8_t aics_mics_num;
	uint8_t *p_aics_mics_tbl;
} rtk_bt_le_audio_aics_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vocs_init_param_t
 * @brief     Bluetooth LE audio VOCS init param
 */
typedef struct {
	uint8_t vocs_num;
	uint8_t *p_vocs_feature_tbl;            /*!< @ref rtk_bt_le_audio_vocs_feature_mask_t */
} rtk_bt_le_audio_vocs_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_mcs_init_param_t
 * @brief     Bluetooth LE audio MCS init param
 */
typedef struct {
	uint32_t general_mcs: 1;
	uint32_t char_media_control_point_support: 1;
	uint32_t char_media_control_point_opcodes_supported_optional_property_notify: 1;
	uint32_t char_media_player_name_optional_property_notify: 1;
	uint32_t char_track_title_optional_property_notify: 1;
	uint32_t char_track_duration_optional_property_notify: 1;
	uint32_t char_track_position_optional_property_notify: 1;
	uint32_t char_playing_order_support: 1;
	uint32_t char_playing_order_optional_property_notify: 1;
	uint32_t char_playing_orders_supported_support: 1;
	uint32_t char_playback_speed_support: 1;
	uint32_t char_playback_speed_optional_property_notify: 1;
	uint32_t char_seeking_speed_support: 1;
	uint32_t char_seeking_speed_optional_property_notify: 1;
	uint32_t char_media_player_icon_url_support: 1;
	uint8_t  media_state;                                           /*!< @ref rtk_bt_le_audio_mcs_media_state_t */
	uint16_t playing_orders_supported;                              /*!< @ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
	uint32_t media_control_point_opcodes_supported;                 /*!< @ref rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t */
} rtk_bt_le_audio_mcs_init_param_t;
#endif

/**
 * @typedef   rtk_bt_le_audio_cap_init_param_t
 * @brief     Bluetooth LE audio CAP init param
 */
typedef struct {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	rtk_bt_le_audio_csis_init_param_t csis_param;                         /*!< Default csis param */
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	rtk_bt_le_audio_aics_init_param_t aics_param;                         /*!< Default aics client param */
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	rtk_bt_le_audio_vocs_init_param_t vocs_param;                         /*!< Default vocs client param */
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	rtk_bt_le_audio_mcs_init_param_t mcs_param;                           /*!< Default mcs client param */
#endif
} rtk_bt_le_audio_cap_init_param_t;

/**
 * @typedef rtk_bt_le_audio_app_conf_t
 * @brief   structure of default LE audio configuration
 */
typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];             /*!< Device name */
	uint8_t device_name_len;                                     /*!< Device name length */
	rtk_bt_le_audio_bap_role_t bap_role;                         /*!< Default le audio bap role */
	rtk_bt_le_audio_cap_role_t cap_role;                         /*!< Default le audio cap role */
	rtk_bt_le_audio_pacs_init_param_t pacs_param;                /*!< Default pacs param */
	rtk_bt_le_audio_ascs_init_param_t ascs_param;                /*!< Default ascs param */
	rtk_bt_le_audio_cap_init_param_t cap_param;                  /*!< Default cap param */
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	rtk_bt_le_audio_tmap_role_t tmap_role;                       /*!< Default tmap param */
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	rtk_bt_le_audio_gmap_role_t gmap_role;                       /*!< Default gmap param */
#endif
} rtk_bt_le_audio_app_conf_t;

#endif

#ifdef __cplusplus
}
#endif
#endif  /* __BT_LE_AUDIO_APP_CONFIG_H__ */
