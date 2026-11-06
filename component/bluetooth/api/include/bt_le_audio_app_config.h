/**
 * @file      bt_le_audio_app_config.h
 * @author    sherman_sun@realsil.com.cn
 * @brief     Bluetooth LE Audio application default configuration definition.
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
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

#define RTK_BT_GAP_DEVICE_NAME_LEN (39+1)    /*!< Maximum length of the device name including the terminating NUL. A longer name is truncated. */

/**
 * @typedef   rtk_bt_le_audio_pacs_init_param_t
 * @brief     Default PACS initialization parameters.
 *            The audio locations, supported contexts and PAC records are applied only when the CAP role includes the acceptor role, in which case the PAC ID
 *            pointers must not be NULL.
 */
typedef struct {
	uint32_t sink_audio_location;          /*!< Sink audio location bit mask, see @ref rtk_bt_le_audio_location_t. */
	uint32_t source_audio_location;        /*!< Source audio location bit mask, see @ref rtk_bt_le_audio_location_t. */
	uint16_t sink_available_contexts;      /*!< Sink audio contexts currently available for use, as a bit mask. */
	uint16_t sink_supported_contexts;      /*!< Sink audio contexts supported by the device, as a bit mask. */
	uint16_t source_available_contexts;    /*!< Source audio contexts currently available for use, as a bit mask. */
	uint16_t source_supported_contexts;    /*!< Source audio contexts supported by the device, as a bit mask. */
	uint8_t *p_sink_pac_id;                /*!< Pointer to a variable that receives the sink PAC record ID assigned during initialization. */
	uint8_t *p_pac_sink_codec;             /*!< Pointer to the sink PAC data: the number of PAC records followed by the PAC records.
                                            *   The buffer must remain valid after initialization. */
	uint16_t pac_sink_codec_len;           /*!< Length of the sink PAC data in bytes. Must not exceed 255. */
	uint8_t *p_source_pac_id;              /*!< Pointer to a variable that receives the source PAC record ID assigned during initialization. */
	uint8_t *p_pac_source_codec;           /*!< Pointer to the source PAC data: the number of PAC records followed by the PAC records.
                                            *   The buffer must remain valid after initialization. */
	uint16_t pac_source_codec_len;         /*!< Length of the source PAC data in bytes. Must not exceed 255. */
} rtk_bt_le_audio_pacs_init_param_t;

/**
 * @typedef   rtk_bt_le_audio_ascs_init_param_t
 * @brief     Default ASCS initialization parameters, used by the BAP Unicast Server role.
 */
typedef struct {
	uint8_t sink_ase_num;      /*!< Number of sink ASEs.
                                *   0: the default number is used. */
	uint8_t source_ase_num;    /*!< Number of source ASEs.
                                *   0: the default number is used. */
} rtk_bt_le_audio_ascs_init_param_t;

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_csis_init_param_t
 * @brief     Default CSIS initialization parameters, used when the CAP role includes the acceptor role.
 */
typedef struct {
	uint8_t csis_num;                              /*!< Number of coordinated sets this device is a member of. */
	uint8_t csis_size;                             /*!< Number of devices in the coordinated set. */
	rtk_bt_le_audio_csis_cfg_t csis_cfg;           /*!< Rank of this device in the coordinated set.
                                                    *   CSIS is initialized only if it is not @ref RTK_BT_LEA_CSIS_CFG_NOT_EXIST. */
	uint8_t csis_feature;                          /*!< Set member feature bit mask of the RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_* macros. */
	uint8_t csis_sirk_type;                        /*!< SIRK type, see @ref rtk_bt_le_audio_csis_sirk_type_t. */
	uint8_t csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN];    /*!< Set Identity Resolving Key (SIRK). */
} rtk_bt_le_audio_csis_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_aics_init_param_t
 * @brief     Default AICS initialization parameters, used when the CAP role includes the acceptor role.
 */
typedef struct {
	uint8_t aics_vcs_num;        /*!< Number of AICS instances included in VCS. */
	uint8_t *p_aics_vcs_tbl;     /*!< Pointer to the table of AICS service instance IDs included in VCS. The table size is aics_vcs_num. */
	uint8_t aics_mics_num;       /*!< Number of AICS instances included in MICS. */
	uint8_t *p_aics_mics_tbl;    /*!< Pointer to the table of AICS service instance IDs included in MICS. The table size is aics_mics_num. */
} rtk_bt_le_audio_aics_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vocs_init_param_t
 * @brief     Default VOCS initialization parameters, used when the CAP role includes the acceptor role.
 */
typedef struct {
	uint8_t vocs_num;               /*!< Number of VOCS instances. */
	uint8_t *p_vocs_feature_tbl;    /*!< Pointer to the table of VOCS feature bit masks, one entry per instance, see @ref rtk_bt_le_audio_vocs_feature_mask_t. */
} rtk_bt_le_audio_vocs_init_param_t;
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_mcs_init_param_t
 * @brief     Default MCS initialization parameters, used when the CAP role includes the initiator role.
 */
typedef struct {
	uint32_t general_mcs: 1;                                                            /*!< Type of the media control service to register.
                                                                                         *   - true: Generic Media Control Service.
                                                                                         *   - false: Media Control Service. */
	uint32_t char_media_control_point_support: 1;                                       /*!< Whether the Media Control Point characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint32_t char_media_control_point_opcodes_supported_optional_property_notify: 1;    /*!< Notify property of the Media Control Point characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_media_player_name_optional_property_notify: 1;                        /*!< Notify property of the Media Player Name characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_track_title_optional_property_notify: 1;                              /*!< Notify property of the Track Title characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_track_duration_optional_property_notify: 1;                           /*!< Notify property of the Track Duration characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_track_position_optional_property_notify: 1;                           /*!< Notify property of the Track Position characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_playing_order_support: 1;                                             /*!< Whether the Playing Order characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint32_t char_playing_order_optional_property_notify: 1;                            /*!< Notify property of the Playing Order characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_playing_orders_supported_support: 1;                                  /*!< Whether the Playing Orders Supported characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint32_t char_playback_speed_support: 1;                                            /*!< Whether the Playback Speed characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint32_t char_playback_speed_optional_property_notify: 1;                           /*!< Notify property of the Playback Speed characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_seeking_speed_support: 1;                                             /*!< Whether the Seeking Speed characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint32_t char_seeking_speed_optional_property_notify: 1;                            /*!< Notify property of the Seeking Speed characteristic.
                                                                                         *   - true: Notify property is enabled.
                                                                                         *   - false: Notify property is disabled. */
	uint32_t char_media_player_icon_url_support: 1;                                     /*!< Whether the Media Player Icon URL characteristic is supported.
                                                                                         *   - true: The characteristic is included.
                                                                                         *   - false: The characteristic is not included. */
	uint8_t  media_state;                                                               /*!< Media state, see @ref rtk_bt_le_audio_mcs_media_state_t.
                                                                                         *   Currently not used by the stack, which sets the initial state to paused. */
	uint16_t playing_orders_supported;                                                  /*!< Supported playing orders bit mask.
                                                                                         *   See @ref rtk_bt_le_audio_mcs_playing_orders_supported_t. */
	uint32_t media_control_point_opcodes_supported;                                     /*!< Supported media control point opcodes bit mask.
                                                                                         *   See @ref rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t. */
} rtk_bt_le_audio_mcs_init_param_t;
#endif

/**
 * @typedef   rtk_bt_le_audio_cap_init_param_t
 * @brief     Default CAP initialization parameters for the enabled optional services.
 */
typedef struct {
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	rtk_bt_le_audio_csis_init_param_t csis_param;    /*!< Default CSIS parameters. */
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	rtk_bt_le_audio_aics_init_param_t aics_param;    /*!< Default AICS parameters. */
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	rtk_bt_le_audio_vocs_init_param_t vocs_param;    /*!< Default VOCS parameters. */
#endif
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	rtk_bt_le_audio_mcs_init_param_t mcs_param;      /*!< Default MCS parameters. */
#endif
} rtk_bt_le_audio_cap_init_param_t;

/**
 * @typedef   rtk_bt_le_audio_app_conf_t
 * @brief     Default LE Audio application configuration used when initializing the LE Audio module.
 */
typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];    /*!< Device name. */
	uint8_t device_name_len;                            /*!< Length of the device name in bytes. */
	rtk_bt_le_audio_bap_role_t bap_role;                /*!< Default BAP role. */
	rtk_bt_le_audio_cap_role_t cap_role;                /*!< Default CAP role. */
	rtk_bt_le_audio_pacs_init_param_t pacs_param;       /*!< Default PACS parameters. */
	rtk_bt_le_audio_ascs_init_param_t ascs_param;       /*!< Default ASCS parameters. */
	rtk_bt_le_audio_cap_init_param_t cap_param;         /*!< Default CAP parameters. */
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	rtk_bt_le_audio_tmap_role_t tmap_role;              /*!< Default TMAP role. */
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	rtk_bt_le_audio_gmap_role_t gmap_role;              /*!< Default GMAP role. */
#endif
} rtk_bt_le_audio_app_conf_t;

#endif

#ifdef __cplusplus
}
#endif
#endif /* __BT_LE_AUDIO_APP_CONFIG_H__ */
