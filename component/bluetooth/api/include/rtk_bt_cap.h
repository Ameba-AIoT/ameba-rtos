/**
 * @file      rtk_bt_cap.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_CAP_H__
#define __RTK_BT_CAP_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_le_audio_def.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
/* cap role */
#define RTK_BT_CAP_ACCEPTOR_ROLE    0x01    /**< Acceptor Role. */
#define RTK_BT_CAP_INITIATOR_ROLE   0x02    /**< Initiator Role. */
#define RTK_BT_CAP_COMMANDER_ROLE   0x04    /**< Commander Role. */
#define RTK_BT_LE_CSIS_SIRK_LEN (16)        /* Define Set Identity Resolving Key Length.*/
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/* csip role */
#define RTK_BT_CSIP_ROLE_SET_COORDINATOR 0x00    /**< Set Coordinator Role. */
#define RTK_BT_CSIP_ROLE_SET_MEMBER 0x01         /**< Set Member Role. */
#define RTK_BT_LE_CSI_RSI_LEN 6
/* Define Coordinated Set Identification Service Features Exist flags*/
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_LOCK_EXIST 0x01
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_EXIST 0x02
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_RANK_EXIST 0x04
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIRK_NOTIFY_SUPPORT 0x10
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_NOTIFY_SUPPORT 0x20
#endif
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
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/* Define AICS Audio Input Status Type.*/
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

#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
#define RTK_BT_LE_AUDIO_GMCS_CCID  0x20
#endif

#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
#define RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX 2
#endif
/**
 * @typedef    rtk_bt_le_audio_cap_role_t
 * @brief     Bluetooth LE AUDIO CAP Role.
 */
typedef enum {
	RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN = 0,
	RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR = RTK_BT_CAP_INITIATOR_ROLE,                 /**< CAP Initiator. */
	RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR = RTK_BT_CAP_ACCEPTOR_ROLE,                   /**< CAP Acceptor. */
	RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER = RTK_BT_CAP_COMMANDER_ROLE,                 /**< CAP Commander. */
} rtk_bt_le_audio_cap_role_t;

#if (defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT)
/**
 * @typedef    rtk_bt_le_audio_csis_lock_t
 * @brief     enum of LE audio csis lock
 */
typedef enum {
	RTK_BT_LE_CSIS_NONE_LOCK  = 0,
	RTK_BT_LE_CSIS_UNLOCKED   = 0x01,
	RTK_BT_LE_CSIS_LOCKED     = 0x02,
} rtk_bt_le_audio_csis_lock_t;

/**
 * @typedef    rtk_bt_le_audio_csis_sirk_type_t
 * @brief     enum of LE audio csis sirk type
 */
typedef enum {
	RTK_BT_LE_CSIS_SIRK_ENC = 0x00,             /**< Encrypted SIRK. */
	RTK_BT_LE_CSIS_SIRK_PLN = 0x01,             /**< Plain text SIRK. */
} rtk_bt_le_audio_csis_sirk_type_t;
#endif

/* ------------------------------- Data Types ------------------------------- */
/* act struct */
#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
typedef enum {
	RTK_BT_LEA_TBS_CALL_STATE_INCOMING                                                        = 0x00, /**< A remote party is calling (incoming call). */
	RTK_BT_LEA_TBS_CALL_STATE_DIALING                                                         = 0x01, /**< The process to call the remote party has started on the server, but the remote party is not being alerted (outgoing call). */
	RTK_BT_LEA_TBS_CALL_STATE_ALERTING                                                        = 0x02, /**< A remote party is being alerted (outgoing call). */
	RTK_BT_LEA_TBS_CALL_STATE_ACTIVE                                                          = 0x03, /**< The call is in an active conversation. */
	RTK_BT_LEA_TBS_CALL_STATE_LOCALLY_HELD                                                    = 0x04, /**< The call is connected but held locally. "Locally Held" implies that either the server or the client can affect the state. */
	RTK_BT_LEA_TBS_CALL_STATE_REMOTELY_HELD                                                   = 0x05, /**< The call is connected but held remotely. "Remotely Held" means that the state is controlled by the remote party of a call. */
	RTK_BT_LEA_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD                                       = 0x06, /**< The call is connected but held both locally and remotely. */
	RTK_BT_LEA_TBS_CALL_STATE_RFU,                                                                    /**< Reserved for future use. */
} rtk_bt_le_audio_ccp_state_t;

typedef struct {
	uint8_t                     call_index;
	uint8_t                     call_flags;
	rtk_bt_le_audio_ccp_state_t call_state;
} rtk_bt_le_audio_tbs_call_t;

typedef struct {
	uint8_t                    gtbs_id;
	uint16_t                   ccp_enabled_cccd;
	uint8_t                    call_num;
	rtk_bt_le_audio_tbs_call_t call_list[RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX];
} rtk_bt_le_audio_ccp_t;
#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT

typedef enum {
	RTK_BT_LEA_CSIS_CFG_NOT_EXIST     = 0,
	RTK_BT_LEA_CSIS_CFG_RANK_1        = 1,
	RTK_BT_LEA_CSIS_CFG_RANK_2        = 2,
} rtk_bt_le_audio_csis_cfg_t;

/**
 * @typedef    rtk_bt_le_audio_csis_gen_rsi_info_t
 * @brief     Bluetooth LE audio info for csis gen for rsi.
 */
typedef struct {
	uint8_t sirk[RTK_BT_LE_CSIS_SIRK_LEN];      /**< csis sirk (set identity resolving key)*/
	uint8_t *p_rsi;                             /**< returned resolvable set identifier. */
} rtk_bt_le_audio_csis_gen_rsi_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_update_sirk_param_t
 * @brief     Bluetooth LE audio info for csis change sirk.
 */
typedef struct {
	rtk_bt_le_audio_csis_sirk_type_t sirk_type;        /**< csis sirk type. @ref rtk_bt_le_audio_csis_sirk_type_t. */
	uint8_t sirk[RTK_BT_LE_CSIS_SIRK_LEN];             /**< new csis sirk. */
} rtk_bt_le_audio_csis_update_sirk_param_t;

/**
 * @typedef    rtk_bt_le_audio_csis_update_size_param_t
 * @brief     Bluetooth LE audio info for csis change size.
 */
typedef struct {
	uint8_t csis_size;                                 /**< Coordinated Set Size characteristic value. Range:0x01 to 0xFF. */
} rtk_bt_le_audio_csis_update_size_param_t;

#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t
 * @brief     Bluetooth LE audio info for csis set coordinator check adv rsi.
 */
typedef struct {
	uint8_t report_data_len;
	uint8_t *p_report_data;
	rtk_bt_le_addr_t adv_addr;              /**< @ref rtk_bt_le_addr_t. */
} rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t
 * @brief     Bluetooth LE audio info for csis set coordinator config discover.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                    /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	bool discover;
	uint32_t timeout_ms;
} rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_set_mem_info_t
 * @brief     Bluetooth LE audio csis set mem info definition .
 */
typedef struct {
	rtk_bt_le_addr_t                    bd_addr;
	uint16_t                            srv_uuid;
	uint8_t                             srv_instance_id;
	uint8_t                             char_exit;
	uint8_t                             rank;
	rtk_bt_le_audio_csis_lock_t         lock;
	uint8_t                             set_mem_size;
	rtk_bt_le_audio_csis_sirk_type_t    sirk_type;
	uint8_t                             sirk[RTK_BT_LE_CSIS_SIRK_LEN];
} rtk_bt_le_audio_csis_set_mem_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_set_coordinator_add_group_info_t
 * @brief     Bluetooth LE audio info for csis set coordinator add group.
 */
typedef struct {
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;
	rtk_bt_le_audio_group_handle_t *p_group_handle;                    /**< returned group handle. @ref rtk_bt_le_audio_group_handle_t */
	rtk_bt_le_audio_device_handle_t *p_device_handle;
} rtk_bt_le_audio_csis_set_coordinator_add_group_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_set_coordinator_add_device_info_t
 * @brief     Bluetooth LE audio info for csis set coordinator add device in group.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                    /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;
	rtk_bt_le_audio_device_handle_t *p_device_handle;               /**< returned device handle. @ref rtk_bt_le_audio_group_handle_t */
} rtk_bt_le_audio_csis_set_coordinator_add_device_info_t;

/**
 * @typedef    rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t
 * @brief     Bluetooth LE audio info for csis set coordinator write lock or unlock in group.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;                    /**< group handle. @ref rtk_bt_le_audio_group_handle_t */
	bool lock;
} rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t;

#endif

#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
typedef struct {
	uint8_t       gmcs_id;
	uint8_t       media_state;
	uint16_t      mcp_enabled_cccd;
} rtk_bt_le_audio_mcp_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_char_uuid_t
 * @brief     Bluetooth LE audio mcs characteristic UUID in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME             = 0x2B93,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL         = 0x2B95,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED                 = 0x2B96,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE                   = 0x2B97,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION                = 0x2B98,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION                = 0x2B99,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED                = 0x2B9A,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED                 = 0x2B9B,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER                 = 0x2BA1,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED      = 0x2BA2,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE                   = 0x2BA3,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT           = 0x2BA4,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODE_SUPPORTED = 0x2BA5,
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID            = 0x2BBA
} rtk_bt_le_audio_mcs_char_uuid_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_cccd_flag_t
 * @brief     Bluetooth LE audio mcs cccd flag.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_PLAYER_NAME                       = 0x00000001,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_CHANGED                           = 0x00000002,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_TITLE                             = 0x00000004,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_DURATION                          = 0x00000008,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_POSITION                          = 0x00000010,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_STATE                             = 0x00000020,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT                     = 0x00000040,
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED   = 0x00000080
} rtk_bt_le_audio_mcs_cccd_flag_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_media_cp_opcode_t
 * @brief     Bluetooth LE audio mcs media control point characteristic opcodes in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY                  = 0x01,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE                 = 0x02,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND           = 0x03,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD          = 0x04,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP                  = 0x05,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE         = 0x10,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT          = 0x24,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK        = 0x30,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK            = 0x31,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_TRACK           = 0x32,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_TRACK            = 0x33,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK            = 0x34,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP            = 0x44,
} rtk_bt_le_audio_mcs_media_cp_opcode_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_media_state_t
 * @brief     Bluetooth LE audio mcs media state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_INACTIVE = 0x00,
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PLAYING,
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PAUSED,
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_SEEKING
} rtk_bt_le_audio_mcs_media_state_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_playing_orders_supported_t
 * @brief     Bluetooth LE audio mcs playing orders supported characteristic bit values .
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SINGLE_ONCE = 0x0001,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SINGLE_REPEAT = 0x0002,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_IN_ORDER_ONCE = 0x0004,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_IN_ORDER_REPEAT = 0x0008,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_OLDEST_ONCE = 0x0010,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_OLDEST_REPEAT  = 0x0020,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_NEWEST_ONCE = 0x0040,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_NEWEST_REPEAT = 0x0080,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SHUFFLE_ONCE = 0x0100,
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SHUFFLE_REPEAT = 0x0200
} rtk_bt_le_audio_mcs_playing_orders_supported_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t
 * @brief     Bluetooth LE audio mcs media control point opcodes supported characteristic bit values.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PLAY = 0x00000001,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PAUSE = 0x00000002,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_REWIND = 0x00000004,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_FORWARD = 0x00000008,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_STOP = 0x00000010,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_MOVE_RELATIVE = 0x00000020,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_TRACK = 0x00000800,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_TRACK = 0x00001000,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FIRST_TRACK = 0x00002000,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_LAST_TRACK = 0x00004000,
} rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t
 * @brief     Bluetooth LE audio mcs media control point notification result codes in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_SUCCESS                    = 0x01,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED       = 0x02,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_MEDIA_PLAYER_INACTIVE      = 0x03,
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_COMMAND_CANNOT_BE_COMPLETED = 0x04
} rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_media_play_name_t
 * @brief     Bluetooth LE audio mcp server media play name.
 */
typedef struct {
	uint8_t       *p_media_player_name;     /* UTF-8 string */
	uint16_t      media_player_name_len;
} rtk_bt_le_audio_mcp_server_media_play_name_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_track_title_t
 * @brief     Bluetooth LE audio mcp server track title.
 */
typedef struct {
	uint8_t       *p_track_title;           /* UTF-8 string */
	uint16_t      track_title_len;
} rtk_bt_le_audio_mcp_server_track_title_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_track_title_t
 * @brief     Bluetooth LE audio mcp server media player icon url.
 */
typedef struct {
	uint8_t       *p_media_player_icon_url;
	uint16_t      media_player_icon_url_len;
} rtk_bt_le_audio_mcp_server_media_player_icon_url_t;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_mcp_client_write_media_cp_info_t
 * @brief     Bluetooth LE audio info for mcs client send mcp op.
 */
typedef struct {
	uint16_t    conn_handle;                            /**< connection handle. */
	uint8_t     srv_instance_id;                        /**< Service Instance Id. */
	bool        general_mcs;                            /**< is generic media control service (GMCS) */
	uint8_t     opcode;                                 /**< @ref rtk_bt_le_audio_mcs_media_cp_opcode_t */
	union {
		int32_t   move_relative_opcode_offset;
		int32_t   goto_segment_opcode_n;
		int32_t   goto_track_opcode_n;
		int32_t   goto_group_opcode_n;
	} op_param;
	bool        is_req;                                 /**<true: Write request, false: Write command */
} rtk_bt_le_audio_mcp_client_write_media_cp_info_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_client_read_char_value_info_t
 * @brief     Bluetooth LE audio info for mcs client read char value.
 */
typedef struct {
	uint16_t    conn_handle;                            /**< Connection handle. */
	uint8_t     srv_instance_id;                        /**< Service Instance Id. */
	bool        general_mcs;                            /**< is generic media control service (GMCS) */
	uint16_t    char_uuid;                              /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
} rtk_bt_le_audio_mcp_client_read_char_value_info_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_client_cfg_cccd_info_t
 * @brief     Bluetooth LE audio info for mcs client config cccd.
 */
typedef struct {
	uint16_t    conn_handle;                            /**< Connection handle. */
	uint8_t     srv_instance_id;                        /**< Service Instance Id. */
	bool        general_mcs;                            /**< is generic media control service (GMCS)*/
	uint32_t    cfg_flags;                              /**< @ref rtk_bt_le_audio_mcs_cccd_flag_t */
	bool        enable;
} rtk_bt_le_audio_mcp_client_cfg_cccd_info_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio mcp client discovery done indication.
 */
typedef struct {
	uint16_t    conn_handle;                            /**< Connection handle. */
	bool        is_found;
	bool        load_from_ftl;                          /**< Whether the service table is loaded from FTL. */
	bool        general_mcs;                            /**< Is generic media control service (GMCS)*/
	uint8_t     srv_num;
} rtk_bt_le_audio_mcp_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_client_read_result_ind_t
 * @brief     Bluetooth LE audio mcp client read result indication.
 */
typedef struct {
	uint16_t    cause;
	uint16_t    conn_handle;                            /**< Connection handle. */
	bool        general_mcs;                            /**< Is generic media control service (GMCS)*/
	uint8_t     srv_instance_id;                        /**< Service Instance id. */
	uint16_t    char_uuid;                              /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*@ref rtk_bt_le_audio_mcp_server_media_play_name_t */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;              /*@ref rtk_bt_le_audio_mcp_server_track_title_t */
		int32_t track_duration;  /* Length of the current track in 0.01-second resolution,zero or greater.
                                            * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN : no current track or the duration of the current track is unknown */
		int32_t track_position;  /* Current track position of the current track in 0.01-second resolution, Offset from the start of the track to the current playing position.
                                            * \arg    0: starting position or start of the track is not well defined
                                            * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE : no current track or the start of the track is not well defined
                                            * \arg    other values */
		int8_t  playback_speed;
		int8_t  seeking_speed;
		uint8_t playing_order;      /*@ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
		uint8_t media_state;        /*@ref rtk_bt_le_audio_mcs_media_state_t */
		uint8_t content_control_id; /*Represent a unique instance of a service that either controls or provides status information on an audio-related feature */
		uint16_t playing_orders_supported; /*Combination of @ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
		uint32_t media_control_point_opcodes_supported; /*Combination of @ref rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t */
	} data;
} rtk_bt_le_audio_mcp_client_read_result_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_client_notify_ind_t
 * @brief     Bluetooth LE audio mcs client notify result indication.
 */
typedef struct {
	uint16_t    conn_handle;                            /**< Connection handle. */
	bool        general_mcs;                            /**< Is generic media control service (GMCS)*/
	uint8_t     srv_instance_id;                        /**< Service Instance Id. */
	uint16_t    char_uuid;                              /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*@ref rtk_bt_le_audio_mcp_server_media_play_name_t */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;               /*@ref rtk_bt_le_audio_mcp_server_track_title_t */
		int32_t track_duration;  /* Length of the current track in 0.01-second resolution,zero or greater. @ref RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN : no current track or the duration of the current track is unknown */
		int32_t track_position;  /* Current track position of the current track in 0.01-second resolution, Offset from the start of the track to the current playing position.                                                                         * \arg    0: starting position or start of the track is not well defined
                                    * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE : no current track or the start of the track is not well defined
                                    * \arg    other values */
		int8_t  playback_speed;
		int8_t  seeking_speed;
		uint8_t playing_order;      /*@ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
		uint8_t media_state;        /*@ref rtk_bt_le_audio_mcs_media_state_t */
	} data;
} rtk_bt_le_audio_mcp_client_notify_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mcs_media_cp_nodify_ind_t
 * @brief     Bluetooth LE audio mcs client media cp notify indication.
 */
typedef struct {
	uint16_t    conn_handle;       /**< Connection handle. */
	bool        general_mcs;       /**< Is generic media control service (GMCS)*/
	uint8_t     srv_instance_id;   /**< Service Instance Id. */
	uint8_t     requested_opcode;  /* @ref rtk_bt_le_audio_mcs_media_cp_opcode_t */
	uint8_t     result_code;       /* @ref rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t */
} rtk_bt_le_audio_mcs_media_cp_nodify_ind_t;

#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT

/**
 * @typedef    rtk_bt_le_audio_mcp_server_write_media_cp_ind_t
 * @brief     Bluetooth LE audio mcp server send data parameter.
 */
typedef struct {
	uint16_t    char_uuid;                                      /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*@ref rtk_bt_le_audio_mcp_server_media_play_name_t */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;               /*@ref rtk_bt_le_audio_mcp_server_track_title_t */
		int32_t track_duration;  /* Length of the current track in 0.01-second resolution,zero or greater. @ref RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN : no current track or the duration of the current track is unknown */
		int32_t track_position;  /* Current track position of the current track in 0.01-second resolution, Offset from the start of the track to the current playing position.                                                                         * \arg    0: starting position or start of the track is not well defined
                                    * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE : no current track or the start of the track is not well defined
                                    * \arg    other values */
		int8_t  playback_speed;
		int8_t  seeking_speed;
		uint8_t playing_order;  /*@ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
	} param;
} rtk_bt_le_audio_mcp_server_send_data_param_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_read_confirm_param_t
 * @brief     Bluetooth LE audio mcp server read confirm parameter.
 */
typedef struct {
	uint16_t  conn_handle;
	uint16_t  cid;
	uint8_t   service_id;                                     /**< mcs service id */
	uint16_t  char_uuid;                                      /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
	uint16_t  offset;
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;             /*@ref rtk_bt_le_audio_mcp_server_media_play_name_t */
		rtk_bt_le_audio_mcp_server_media_player_icon_url_t  media_player_icon_url;  /*@ref  rtk_bt_le_audio_mcp_server_media_player_icon_url_t */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;                       /*@ref rtk_bt_le_audio_mcp_server_track_title_t */
		int32_t track_duration;  /* Length of the current track in 0.01-second resolution,zero or greater. @ref RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN : no current track or the duration of the current track is unknown */
		int32_t track_position;  /* Current track position of the current track in 0.01-second resolution, Offset from the start of the track to the current playing position.                                                                         * \arg    0: starting position or start of the track is not well defined
                                            * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE : no current track or the start of the track is not well defined
                                            * \arg    other values */
		int8_t  playback_speed;
		int8_t  seeking_speed;
		uint8_t playing_order;          /*@ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
		uint8_t content_control_id;
	} param;
} rtk_bt_le_audio_mcp_server_read_confirm_param_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_write_media_cp_ind_t
 * @brief     Bluetooth LE audio mcp server write media control point indication.
 */
typedef struct {
	uint16_t        conn_handle;
	uint16_t        cid;
	uint8_t         service_id;                     /**< mcs service id */
	uint8_t         opcode;                         /**< @ref rtk_bt_le_audio_mcs_media_cp_opcode_t */
	union {
		int32_t   move_relative_opcode_offset;
		int32_t   goto_segment_opcode_n;
		int32_t   goto_track_opcode_n;
		int32_t   goto_group_opcode_n;
	} param;
} rtk_bt_le_audio_mcp_server_write_media_cp_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mcp_server_read_ind_t
 * @brief     Bluetooth LE audio mcp read indication.
 */
typedef struct {
	uint16_t  conn_handle;
	uint16_t  cid;
	uint8_t   service_id;                             /**< mcs service id */
	uint16_t  char_uuid;                              /**< char_uuid. @ref rtk_bt_le_audio_mcs_char_uuid_t */
	uint16_t  offset;
	rtk_bt_le_audio_mcp_server_read_confirm_param_t  *p_cfm;
} rtk_bt_le_audio_mcp_server_read_ind_t;
#endif
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_vcs_mute_state_t
 * @brief     Bluetooth LE audio vcs mute state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_NOT_MUTED =  0,
	RTK_BT_LE_AUDIO_VCS_MUTED  = 1,
} rtk_bt_le_audio_vcs_mute_state_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_param_t
 * @brief     Bluetooth LE audio vcs param.
 */
typedef struct {
	uint8_t volume_setting;
	uint8_t mute;
	uint8_t change_counter;
	uint8_t volume_flags;
	uint8_t step_size;
} rtk_bt_le_audio_vcs_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_vcs_cccd_flags_t
 * @brief     Bluetooth LE audio VCS Characteristic Flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_VOLUME_STATE_FLAG = 0x01,   /**< Volume State Exist Flag. */
	RTK_BT_LE_AUDIO_VCS_VOLUME_FLAGS_FLAG = 0x02,   /**< Volume Flags Exist Flag. */
} rtk_bt_le_audio_vcs_cccd_flags_t;


/**
 * @typedef    rtk_bt_le_audio_vcs_cp_op_t
 * @brief     Bluetooth LE audio vcs cp op.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_DOWN        = 0x00,     /**< Relative Volume Down */
	RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_UP          = 0x01,     /**< Relative Volume Up */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN = 0x02,     /**< Unmute/Relative Volume Down */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_UP   = 0x03,     /**< Unmute/Relative Volume Up */
	RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME         = 0x04,     /**< Set Absolute Volume */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE                      = 0x05,     /**< Unmute */
	RTK_BT_LE_AUDIO_VCS_CP_MUTE                        = 0x06,     /**< Mute */
	RTK_BT_LE_AUDIO_VCS_CP_MAX
} rtk_bt_le_audio_vcs_cp_op_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_cp_param_t
 * @brief     Bluetooth LE audio vcs cp param.
 */
typedef struct {
	uint8_t volume_setting;             /**< volume_setting, used only for RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME */
} rtk_bt_le_audio_vcs_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_write_cp_param_t
 * @brief     Bluetooth LE audio vcs cp param.
 */
typedef struct {
	uint16_t conn_handle;                     /**< connection handle */
	rtk_bt_le_audio_vcs_cp_op_t cp_op;        /**<cp op. @ref rtk_bt_le_audio_vcs_cp_op_t */
	rtk_bt_le_audio_vcs_cp_param_t cp_param;  /**< cp param. @ref rtk_bt_le_audio_vcs_cp_param_t*/
} rtk_bt_le_audio_vcs_write_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_volume_cp_ind_t
 * @brief     Bluetooth LE audio vcs volume control point indicate.
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t volume_setting;
	uint8_t mute;
	rtk_bt_le_audio_vcs_cp_op_t cp_op;
} rtk_bt_le_audio_vcs_volume_cp_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_volume_state_t
 * @brief     Bluetooth LE audio vcs volume state.
 */
typedef struct {
	uint8_t volume_setting;
	uint8_t mute;
	uint8_t change_counter;
} rtk_bt_le_audio_vcs_volume_state_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_get_volume_state_param_t
 * @brief     Bluetooth LE audio vcs volume state get param.
 */
typedef struct {
	uint16_t conn_handle;                                     /**< connection handle */
	rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state ;  /**< vcs volume state. @ref rtk_bt_le_audio_vcs_volume_state_t*/
} rtk_bt_le_audio_vcs_get_volume_state_param_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_get_volume_flags_param_t
 * @brief     Bluetooth LE audio vcs volume flags get param.
 */
typedef struct {
	uint16_t conn_handle;                                     /**< connection handle */
	uint8_t volume_flags;                                     /**< vcs volume flags */
} rtk_bt_le_audio_vcs_get_volume_flags_param_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_change_mute_param_t
 * @brief     Bluetooth LE audio vcs change mute param.
 */
typedef struct {
	uint8_t group_idx;                                    /**< group index.*/
	uint8_t mute;                                         /**< vcs mute. @ref rtk_bt_le_audio_vcs_volume_state_t*/
} rtk_bt_le_audio_vcs_change_mute_param_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_change_volume_param_t
 * @brief     Bluetooth LE audio vcs change volume param.
 */
typedef struct {
	uint8_t group_idx;                                        /**< group index.*/
	uint8_t volume_setting;                                   /**< volume setting. */
} rtk_bt_le_audio_vcs_change_volume_param_t;
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_mics_mute_state_t
 * @brief     Bluetooth LE audio mics mute state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MICS_NOT_MUTE   = 0,
	RTK_BT_LE_AUDIO_MICS_MUTED      = 1,
	RTK_BT_LE_AUDIO_MICS_DISABLED   = 2,
	RTK_BT_LE_AUDIO_MICS_RFU,
} rtk_bt_le_audio_mics_mute_state_t;
/**
 * @typedef    rtk_bt_le_audio_mics_change_mute_param_t
 * @brief     Bluetooth LE audio mics change mute param.
 */
typedef struct {
	uint8_t group_idx;                                            /**< group index.*/
	rtk_bt_le_audio_mics_mute_state_t mic_mute;                   /**< mic mute.  @ref rtk_bt_le_audio_mics_mute_state_t*/
} rtk_bt_le_audio_mics_change_mute_param_t;

/**
 * @typedef    rtk_bt_le_audio_mics_set_mute_value_param_t
 * @brief     Bluetooth LE audio mics set mute value param.
 */
typedef struct {
	uint16_t conn_handle;                                         /**< connection handle */
	rtk_bt_le_audio_mics_mute_state_t mic_mute;                   /**< mic mute.  @ref rtk_bt_le_audio_mics_mute_state_t*/
} rtk_bt_le_audio_mics_set_mute_value_param_t;

/**
 * @typedef    rtk_bt_le_audio_mics_get_mute_value_param_t
 * @brief     Bluetooth LE audio mics get mute value param.
 */
typedef struct {
	uint16_t conn_handle;                                       /**< connection handle */
	rtk_bt_le_audio_mics_mute_state_t *p_mic_mute;              /**< mic mute.  @ref rtk_bt_le_audio_mics_mute_state_t*/
} rtk_bt_le_audio_mics_get_mute_value_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_vocs_feature_mask_t
 * @brief     Bluetooth LE AUDIO Volume Offset Control Service Feature Mask.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT = 0x01,
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT = 0x02,
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT = 0x04,
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT = 0x08
} rtk_bt_le_audio_vocs_feature_mask_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_output_des_t
 * @brief     Bluetooth LE audio vocs audio output description value.
 */
typedef struct {
	uint16_t    output_des_len;
	uint8_t    *p_output_des;
} rtk_bt_le_audio_vocs_output_des_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_param_t
 * @brief     Bluetooth LE audio vocs param.
 */
typedef struct {
	int16_t                             volume_offset;
	uint8_t                             change_counter;
	uint32_t                            audio_location;  /**<  Audio location: @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t   output_des;
} rtk_bt_le_audio_vocs_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_offset_state_ind_t
 * @brief     Bluetooth LE audio vocs write offset state indicate.
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t  srv_instance_id;
	int16_t  volume_offset;
} rtk_bt_le_audio_vocs_write_offset_state_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_audio_location_ind_t
 * @brief     Bluetooth LE audio vocs write audio location indicate.
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     srv_instance_id;
	uint32_t    audio_location;  /**<  Audio location: @ref rtk_bt_le_audio_location_t. */
} rtk_bt_le_audio_vocs_write_audio_location_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_output_des_ind_t
 * @brief     Bluetooth LE audio vocs write output des indicate.
 */
typedef struct {
	uint16_t    conn_handle;
	uint8_t     srv_instance_id;
	rtk_bt_le_audio_vocs_output_des_t output_des;
} rtk_bt_le_audio_vocs_write_output_des_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_set_param_t
 * @brief     Bluetooth LE audio vocs set parameter.
 */
typedef struct {
	uint8_t                             srv_instance_id;
	uint8_t                             set_mask;
	rtk_bt_le_audio_vocs_param_t        vocs_param;
} rtk_bt_le_audio_vocs_set_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_get_param_t
 * @brief     Bluetooth LE audio vocs get parameter.
 */
typedef struct {
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_vocs_param_t        *p_vocs_param;
} rtk_bt_le_audio_vocs_get_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_cfg_cccd_param_t
 * @brief     Bluetooth LE audio vocs config cccd parameter.
 */
typedef struct {
	uint16_t conn_handle;           /**< Connection handle */
	uint8_t srv_instance_id;        /**< VOCS service index */
	uint8_t cfg_flags;              /**< VOCS CCCD Flag, @ref rtk_bt_le_audio_vocs_cccd_flags_t*/
	bool enable;                    /**< Whether to enable CCCD */
} rtk_bt_le_audio_vocs_cfg_cccd_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_char_type_t
 * @brief     Bluetooth LE audio vocs characteristics type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE,
	RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION,
	RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC,
} rtk_bt_le_audio_vocs_char_type_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_read_char_value_param_t
 * @brief     Bluetooth LE audio vocs read char value parameter.
 */
typedef struct {
	uint16_t conn_handle;                   /**<  Connection handle */
	uint8_t srv_instance_id;                /**<  VOCS service index */
	rtk_bt_le_audio_vocs_char_type_t type;     /**<  VOCS characteristic type, @ref rtk_bt_le_audio_vocs_char_type_t */
} rtk_bt_le_audio_vocs_read_char_value_param_t;

/**
* @typedef    rtk_bt_le_audio_vocs_cp_param_t
* @brief     Bluetooth LE audio vocs cp param.
*/
typedef struct {
	bool    counter_used;
	uint8_t change_counter;
	int16_t volume_offset;
} rtk_bt_le_audio_vocs_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_cp_op_t
 * @brief     Bluetooth LE audio vocs volume offset control point opcode.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET = 0x01,   /**< Set Volume Offset */
	RTK_BT_LE_AUDIO_VOCS_CP_MAX
} rtk_bt_le_audio_vocs_cp_op_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_cp_param_t
 * @brief     Bluetooth LE audio vocs write control point parameter.
 */
typedef struct {
	uint16_t conn_handle;                   /**<  Connection handle */
	uint8_t srv_instance_id;                /**<  VOCS service index */
	rtk_bt_le_audio_vocs_cp_op_t cp_op;     /**<  Volume offset control point opcode, @ref rtk_bt_le_audio_vocs_cp_op_t */
	rtk_bt_le_audio_vocs_cp_param_t cp_param;   /**<  Volume offset control point parameter, @ref rtk_bt_le_audio_vocs_cp_param_t */
} rtk_bt_le_audio_vocs_write_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_cp_by_group_param_t
 * @brief     Bluetooth LE audio vocs write control point parameter.
 */
typedef struct {
	uint8_t group_idx;                              /**< group index.*/
	uint8_t srv_instance_id;                        /**<  VOCS service index */
	rtk_bt_le_audio_vocs_cp_op_t cp_op;             /**<  Volume offset control point opcode, @ref rtk_bt_le_audio_vocs_cp_op_t */
	rtk_bt_le_audio_vocs_cp_param_t cp_param;       /**<  Volume offset control point parameter, @ref rtk_bt_le_audio_vocs_cp_param_t */
} rtk_bt_le_audio_vocs_write_cp_by_group_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_audio_location_param_t
 * @brief     Bluetooth LE audio vocs write audio location parameter.
 */
typedef struct {
	uint16_t conn_handle;                           /**<  Connection handle */
	uint8_t srv_instance_id;                        /**<  VOCS service index */
	uint32_t audio_location;                        /**<  Audio location: @ref rtk_bt_le_audio_location_t. */
} rtk_bt_le_audio_vocs_write_audio_location_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_write_output_des_param_t
 * @brief     Bluetooth LE audio vocs write output descriptor parameter.
 */
typedef struct {
	uint16_t conn_handle;                           /**<  Connection handle */
	uint8_t srv_instance_id;                        /**<  VOCS service index */
	uint16_t output_des_len;                        /**<  The length of the output descriptor value */
	uint8_t *p_output_des;                          /**<  Point to the output descriptor value. */
} rtk_bt_le_audio_vocs_write_output_des_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_volume_offset_state_t
 * @brief     Bluetooth LE audio vocs volume offset state value.
 */
typedef struct {
	int16_t volume_offset;
	uint8_t change_counter;
} rtk_bt_le_audio_vocs_volume_offset_state_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_srv_data_t
 * @brief     Bluetooth LE audio vocs service data.
 */
typedef struct {
	uint8_t                                             srv_instance_id;
	uint8_t                                             type_exist;
	rtk_bt_le_audio_vocs_volume_offset_state_t          volume_offset;
	uint32_t                                            audio_location;  /**<  Audio location: @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t                   output_des;
} rtk_bt_le_audio_vocs_srv_data_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_get_srv_data_param_t
 * @brief     Bluetooth LE audio vocs get service data parameter.
 */
typedef struct {
	uint16_t conn_handle;                           /**<  Connection handle */
	uint8_t srv_instance_id;                        /**<  VOCS service index */
	rtk_bt_le_audio_vocs_srv_data_t *p_srv_data;   /**<  returned service data, @ref rtk_bt_le_audio_vocs_srv_data_t */
} rtk_bt_le_audio_vocs_get_srv_data_param_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_cccd_flags_t
 * @brief     Bluetooth LE AUDIO Volume Offset Control Service CCCD Flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_VOLUME_OFFSET_STATE_FLAG = 0x01,
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_FLAG = 0x02,
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_FLAG = 0x04
} rtk_bt_le_audio_vocs_cccd_flags_t;
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_aics_param_type_t
 * @brief     Bluetooth LE audio AICS parameter type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE       = 0x01,   /**< Parameter Data @ref rtk_bt_le_audio_aics_input_state_t */
	RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP = 0x02,   /**< Parameter Data @ref rtk_bt_le_audio_aics_gain_setting_prop_t */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE        = 0x03,   /**< Parameter Data length is 1 byte */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS      = 0x04,   /**< Parameter Data length is 1 byte */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES         = 0x05,   /**< Parameter Data @ref rtk_bt_le_audio_aics_input_des_t */
} rtk_bt_le_audio_aics_param_type_t;

/**
 * @typedef   rtk_bt_le_audio_input_type_t
 * @brief     Bluetooth LE audio input type
 */
typedef enum {
	RTK_BT_LE_AUDIO_INPUT_UNSPECIFIED         = 0x00,     /**< Unspecified Input */
	RTK_BT_LE_AUDIO_INPUT_BLUETOOTH           = 0x01,     /**< Bluetooth Audio Stream */
	RTK_BT_LE_AUDIO_INPUT_MICROPHONE          = 0x02,     /**< Microphone */
	RTK_BT_LE_AUDIO_INPUT_ANALOG              = 0x03,     /**< Analog Interface */
	RTK_BT_LE_AUDIO_INPUT_DIGITAL             = 0x04,     /**< Digital Interface */
	RTK_BT_LE_AUDIO_INPUT_RADIO               = 0x05,     /**< AM/FM/XM/etc. */
	RTK_BT_LE_AUDIO_INPUT_STREAMING           = 0x06,     /**< Streaming Audio Source */
} rtk_bt_le_audio_input_type_t;

/**
 * @typedef    rtk_bt_le_audio_aics_char_type_t
 * @brief     Bluetooth LE audio aics characteristic type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE,          /**< Audio Input State. */
	RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP,    /**< Gain Setting Properties. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE,           /**< Audio Input Type. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS,         /**< Audio Input Status. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES,            /**< Audio Input Description. */
} rtk_bt_le_audio_aics_char_type_t;

/**
 * @typedef    rtk_bt_le_audio_aics_input_state_t
 * @brief     Bluetooth LE audio AICS Service Audio Input State Characteristic Value.
 */
typedef struct {
	int8_t  gain_setting;
	uint8_t mute;
	uint8_t gain_mode;
	uint8_t change_counter;
} rtk_bt_le_audio_aics_input_state_t;

/**
 * @typedef    rtk_bt_le_audio_aics_set_param_t
 * @brief     Bluetooth LE audio aics set paramter.
 */
typedef struct {
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_aics_param_type_t   aics_param_type;    /**< @ref rtk_bt_le_audio_aics_param_type_t */
	uint8_t                             value_len;          /**< The value length of p_value. */
	uint8_t                             *p_value;           /**< Pointer to the AICS parameter data. */
	bool                                set_change_counter; /**< Whether to set change counter. */
} rtk_bt_le_audio_aics_set_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_get_param_t
 * @brief     Bluetooth LE audio aics get paramter.
 */
typedef struct {
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_aics_param_type_t   aics_param_type;    /**< @ref rtk_bt_le_audio_aics_param_type_t, except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES*/
	uint8_t                             value_len;          /**< The value length of p_value. */
	uint8_t                             *p_value;           /**< Pointer to the AICS parameter data. */
} rtk_bt_le_audio_aics_get_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_cfg_cccd_param_t
 * @brief     Bluetooth LE audio aics config cccd parameter.
 */
typedef struct {
	uint16_t conn_handle;           /**< Connection handle */
	uint8_t srv_instance_id;        /**< AICS service index */
	uint8_t cfg_flags;              /**< AICS CCCD Flag, @ref rtk_bt_le_audio_aics_cccd_flags_t*/
	bool enable;                    /**< Whether to enable CCCD */
} rtk_bt_le_audio_aics_cfg_cccd_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_read_char_value_param_t
 * @brief     Bluetooth LE audio aics read char value parameter.
 */
typedef struct {
	uint16_t conn_handle;                   /**<  Connection handle */
	uint8_t srv_instance_id;                /**<  AICS service index */
	rtk_bt_le_audio_aics_char_type_t type;  /**<  AICS characteristic type, @ref rtk_bt_le_audio_aics_char_type_t */
} rtk_bt_le_audio_aics_read_char_value_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_cp_op_t
 * @brief     Bluetooth LE audio AICS Audio Input Control Point Opcodes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING        = 0x01,
	RTK_BT_LE_AUDIO_AICS_CP_UNMUTE                  = 0x02,
	RTK_BT_LE_AUDIO_AICS_CP_MUTE                    = 0x03,
	RTK_BT_LE_AUDIO_AICS_CP_SET_MANUAL_GAIN_MODE    = 0x04,
	RTK_BT_LE_AUDIO_AICS_CP_SET_AUTOMATIC_GAIN_MODE = 0x05,
	RTK_BT_LE_AUDIO_AICS_CP_MAX
} rtk_bt_le_audio_aics_cp_op_t;

/**
* @typedef    rtk_bt_le_audio_aics_cp_param_t
* @brief     Bluetooth LE audio aics cp param.
*/
typedef struct {
	int8_t  gaining_setting;
}  rtk_bt_le_audio_aics_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_write_cp_param_t
 * @brief     Bluetooth LE audio aics write control point parameter.
 */
typedef struct {
	uint16_t conn_handle;                     /**<  Connection handle */
	uint8_t srv_instance_id;                  /**<  AICS service index */
	rtk_bt_le_audio_aics_cp_op_t cp_op;       /**<  AICS control point opcode, @ref rtk_bt_le_audio_aics_cp_op_t */
	rtk_bt_le_audio_aics_cp_param_t cp_param; /**<  AICS control point parameter, @ref rtk_bt_le_audio_aics_cp_param_t */
} rtk_bt_le_audio_aics_write_cp_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_write_cp_by_group_param_t
 * @brief     Bluetooth LE audio aics write control point parameter.
 */
typedef struct {
	uint8_t group_idx;                              /**< group index. */
	uint8_t srv_instance_id;                        /**<  AICS service index */
	rtk_bt_le_audio_aics_cp_op_t cp_op;             /**<  AICS control point opcode, @ref rtk_bt_le_audio_aics_cp_op_t */
	rtk_bt_le_audio_aics_cp_param_t cp_param;       /**<  AICS control point parameter, @ref rtk_bt_le_audio_aics_cp_param_t */
} rtk_bt_le_audio_aics_write_cp_by_group_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_write_input_des_param_t
 * @brief     Bluetooth LE audio aics write input descriptor parameter.
 */
typedef struct {
	uint16_t conn_handle;                           /**<  Connection handle */
	uint8_t srv_instance_id;                        /**<  AICS service index */
	uint16_t input_des_len;                         /**<  The length of the input descriptor value */
	uint8_t *p_input_des;                           /**<  Point to the input descriptor value. */
} rtk_bt_le_audio_aics_write_input_des_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_gain_setting_prop_t
 * @brief     Bluetooth LE audio AICS Service Gain Setting Properties Characteristic Value.
 */
typedef struct {
	uint8_t gain_setting_units;
	int8_t  gain_setting_min;
	int8_t  gain_setting_max;
} rtk_bt_le_audio_aics_gain_setting_prop_t;

/**
 * @typedef    rtk_bt_le_audio_aics_input_des_t
 * @brief     Bluetooth LE audio AICS service audio input description characteristic values.
 */
typedef struct {
	uint16_t input_des_len;
	uint8_t  *p_input_des;
} rtk_bt_le_audio_aics_input_des_t;

/**
 * @typedef    rtk_bt_le_audio_aics_srv_data_t
 * @brief     Bluetooth LE audio vocs service data.
 */
typedef struct {
	uint8_t                                             srv_instance_id;
	uint8_t                                             type_exist;      /**< @ref rtk_bt_le_audio_aics_cccd_flags_t. */
	rtk_bt_le_audio_aics_input_state_t                  input_state;     /**< @ref rtk_bt_le_audio_aics_input_state_t. */
	rtk_bt_le_audio_aics_gain_setting_prop_t            setting_prop;    /**< @ref rtk_bt_le_audio_aics_gain_setting_prop_t. */
	uint8_t                                             input_type;
	uint8_t                                             input_status;
	rtk_bt_le_audio_aics_input_des_t                    input_des;       /**< @ref rtk_bt_le_audio_aics_input_des_t. */
} rtk_bt_le_audio_aics_srv_data_t;

/**
 * @typedef    rtk_bt_le_audio_aics_cp_ind_t
 * @brief     Bluetooth LE audio aics control point indicate.
 */
typedef struct {
	uint16_t                        conn_handle;
	uint8_t                         srv_instance_id;
	rtk_bt_le_audio_aics_cp_op_t    cp_op;              /**< @ref rtk_bt_le_audio_aics_cp_op_t */
	int8_t                          gain_setting;
} rtk_bt_le_audio_aics_cp_ind_t;

/**
 * @typedef    rtk_bt_le_audio_aics_write_input_des_ind_t
 * @brief     Bluetooth LE audio aics audio input description write indicate.
 */
typedef struct {
	uint16_t         conn_handle;
	uint8_t          srv_instance_id;
	rtk_bt_le_audio_aics_input_des_t input_des;         /**< @ref rtk_bt_le_audio_aics_input_des_t */
} rtk_bt_le_audio_aics_write_input_des_ind_t;

/**
 * @typedef    rtk_bt_le_audio_aics_get_srv_data_param_t
 * @brief     Bluetooth LE audio aics get service data parameter.
 */
typedef struct {
	uint16_t conn_handle;                           /**<  Connection handle */
	uint8_t srv_instance_id;                        /**<  AICS service index */
	rtk_bt_le_audio_aics_srv_data_t *p_srv_data;   /**<   returned service data, @ref rtk_bt_le_audio_aics_srv_data_t */
} rtk_bt_le_audio_aics_get_srv_data_param_t;

/**
 * @typedef    rtk_bt_le_audio_aics_cccd_flags_t
 * @brief     Bluetooth LE audio AICS Characteristic Flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_INPUT_STATE_FLAG       = 0x01,   /**< Audio Input State Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_FLAG      = 0x02,   /**< Audio Input Status Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_DES_FLAG         = 0x04,   /**< Audio Input Description Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_TYPE_FLAG        = 0x08,   /**< Audio Input Type Flag for read. */
	RTK_BT_LE_AUDIO_AICS_GAIN_SETTING_PROP_FLAG = 0x10,   /**< Gain Setting Properties Flag for read. */
} rtk_bt_le_audio_aics_cccd_flags_t;

/**
 * @typedef    rtk_bt_le_audio_aics_gain_mode_t
 * @brief     Bluetooth LE audio AICS Gain Modes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL_ONLY       = 0,
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY    = 1,
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL            = 2,
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC         = 3
} rtk_bt_le_audio_aics_gain_mode_t;

/**
 * @typedef    rtk_bt_le_audio_aics_mute_state_t
 * @brief     Bluetooth LE audio AICS Audio Mute State .
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_NOT_MUTED                  = 0,
	RTK_BT_LE_AUDIO_AICS_MUTED                      = 1,
	RTK_BT_LE_AUDIO_AICS_MUTE_DISABLED              = 2
} rtk_bt_le_audio_aics_mute_state_t;
#endif
/* event struct */
/**
 * @typedef    rtk_bt_le_audio_cap_discovery_done_ind_t
 * @brief     Bluetooth LE audio info for cap discovery done indication. Common Audio Profile Client Discover Service Result
 */
typedef struct {
	uint16_t conn_handle;
	bool     load_from_ftl; /**< Whether the service table is loaded from FTL. */
	bool     cas_is_found;  /**< Whether to find CAS. */
	bool     cas_inc_csis;  /**< Whether the CAS instance include the CSIS instance. */
	bool     vcs_is_found;  /**< Whether to find VCS. */
	bool     mics_is_found; /**< Whether to find MICS. */
} rtk_bt_le_audio_cap_discovery_done_ind_t;

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT

/**
 * @typedef    rtk_bt_le_audio_csis_client_discovery_done_ind_t
 * @brief     Indication of csis client discovery done event with cb_type @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_DISCOVERY_DONE_IND .
 */
typedef struct {
	uint16_t conn_handle;
	bool    is_found;
	bool    load_from_ftl;
	uint8_t srv_num;
	uint8_t group_idx;
} rtk_bt_le_audio_csis_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_csis_client_read_result_ind_t
 * @brief     Indication of csis client read result event with cb_type @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_READ_RESULT_IND .
 */
typedef struct {
	uint16_t                            cause;
	uint16_t                            conn_handle;
	rtk_bt_le_audio_group_handle_t      group_handle;           /**< @ref rtk_bt_le_audio_group_handle_t. */
	rtk_bt_le_audio_device_handle_t     dev_handle;             /**< @ref rtk_bt_le_audio_device_handle_t. */
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;               /**< @ref rtk_bt_le_audio_csis_set_mem_info_t. */
	uint8_t                             group_idx;
} rtk_bt_le_audio_csis_client_read_result_ind_t;

/**
 * @typedef    rtk_bt_le_audio_csis_client_search_done_ind_t
 * @brief     Indication of csis client search done or timeout event with cb_type @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t      group_handle;           /**< @ref rtk_bt_le_audio_group_handle_t. */
	uint8_t         set_mem_size;
	bool            search_done;
	bool            search_timeout;
} rtk_bt_le_audio_csis_client_search_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_csis_client_set_mem_found_ind_t
 * @brief     Indication of csis client set mem found event with cb_type @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t      group_handle;           /**< @ref rtk_bt_le_audio_group_handle_t. */
	rtk_bt_le_audio_device_handle_t     dev_handle;             /**< @ref rtk_bt_le_audio_device_handle_t. */
	rtk_bt_le_addr_t                    bd_addr;
	uint16_t                            srv_uuid;
	uint8_t                             rank;
	uint8_t                             set_mem_size;
	uint8_t                             sirk[RTK_BT_LE_CSIS_SIRK_LEN];
} rtk_bt_le_audio_csis_client_set_mem_found_ind_t;
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_vcs_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio vcs client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;
	bool     is_found;      /**< Whether to find service. */
	bool     load_from_ftl; /**< Whether the service table is loaded from FTL. */
	uint8_t  type_exist;    /**< vcs char exist flag. 0x01: Volume State Exist. 0x02:Volume Flags Exist. */
} rtk_bt_le_audio_vcs_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_client_volume_state_ind_t
 * @brief     Bluetooth LE audio vcs client volume state indication.
 */
typedef struct {
	uint16_t        conn_handle;
	bool            is_notify;
	rtk_bt_le_audio_vcs_volume_state_t volume_state;              /**< @ref rtk_bt_le_audio_vcs_volume_state_t */
} rtk_bt_le_audio_vcs_client_volume_state_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vcs_client_volume_flag_ind_t
 * @brief     Bluetooth LE audio vcs client volume flag indication.
 */
typedef struct {
	uint16_t       conn_handle;
	bool           is_notify;
	uint8_t        volume_flags;
} rtk_bt_le_audio_vcs_client_volume_flag_ind_t;
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_mics_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio mics client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;
	bool     is_found;      /**< Whether to find service. */
	bool     load_from_ftl; /**< Whether the service table is loaded from FTL. */
	uint8_t  mic_mute;      /**< mic mute state. @ref rtk_bt_le_audio_mics_mute_state_t */
} rtk_bt_le_audio_mics_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mics_client_notify_ind_t
 * @brief     Bluetooth LE audio mics client notify indication.
 */
typedef struct {
	uint16_t conn_handle;
	uint8_t  mic_mute;       /**< mic mute state. @ref rtk_bt_le_audio_mics_mute_state_t */
} rtk_bt_le_audio_mics_client_notify_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mics_write_mute_ind_t
 * @brief     Bluetooth LE audio mics write mute state indicate.
 */
typedef struct {
	uint8_t mic_mute;        /**< mic mute state. @ref rtk_bt_le_audio_mics_mute_state_t */
} rtk_bt_le_audio_mics_write_mute_ind_t;

/**
 * @typedef    rtk_bt_le_audio_mics_param_t
 * @brief     Bluetooth LE audio mics write mute state indicate.
 */
typedef struct {
	uint8_t mic_mute;       /**< mic mute state. @ref rtk_bt_le_audio_mics_mute_state_t */
} rtk_bt_le_audio_mics_param_t;

#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef    rtk_bt_le_audio_vocs_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio vocs client discovery done indicate.
 */
typedef struct {
	uint16_t conn_handle;
	bool    is_found;
	bool    load_from_ftl;
	uint8_t srv_num;       /**< Service instance number. */
} rtk_bt_le_audio_vocs_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_char_data_t
 * @brief     Bluetooth LE audio vocs characteristics data.
 */
typedef union {
	rtk_bt_le_audio_vocs_volume_offset_state_t  volume_offset;
	uint32_t                                    audio_location;  /**<  Audio location: @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t           output_des;
} rtk_bt_le_audio_vocs_char_data_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_client_read_result_ind_t
 * @brief     Bluetooth LE audio vocs client read result indicate.
 */
typedef struct {
	uint16_t                            conn_handle;
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_vocs_char_type_t    type;
	rtk_bt_le_audio_vocs_char_data_t    data;
	uint16_t                            cause;
} rtk_bt_le_audio_vocs_client_read_result_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_client_notify_ind_t
 * @brief     Bluetooth LE audio vocs client notification indicate.
 */
typedef struct {
	uint16_t                            conn_handle;
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_vocs_char_type_t    type;
	rtk_bt_le_audio_vocs_char_data_t    data;
} rtk_bt_le_audio_vocs_client_notify_ind_t;

/**
 * @typedef    rtk_bt_le_audio_vocs_client_cp_result_ind_t
 * @brief     Bluetooth LE audio vocs write volume offset control point result indicate.
 */
typedef struct {
	uint16_t                        conn_handle;
	uint8_t                         srv_instance_id;
	uint16_t                        cause;
	rtk_bt_le_audio_vocs_cp_op_t    cp_op;
} rtk_bt_le_audio_vocs_client_cp_result_ind_t;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
#define RTK_BT_LE_ATTR_INSTANCE_NUM_MAX               20

typedef struct {
	uint8_t instance_num;
	uint8_t instance_id[RTK_BT_LE_ATTR_INSTANCE_NUM_MAX];
} rtk_bt_le_attr_instance_t;

/**
 * @typedef    rtk_bt_le_audio_aics_client_discovery_done_ind_t
 * @brief     Bluetooth LE audio aics client discovery done indicate.
 */
typedef struct {
	uint16_t conn_handle;
	bool is_found;
	bool load_from_ftl;
	uint8_t srv_num;
	rtk_bt_le_attr_instance_t vcs_instance;
	rtk_bt_le_attr_instance_t mics_instance;
} rtk_bt_le_audio_aics_client_discovery_done_ind_t;

/**
 * @typedef    rtk_bt_le_audio_aics_char_type_t
 * @brief     Bluetooth LE audio aics characteristic type.
 */
typedef union {
	rtk_bt_le_audio_aics_input_state_t          input_state;    /**< Audio Input State Data. */
	rtk_bt_le_audio_aics_gain_setting_prop_t    setting_prop;   /**< Gain Setting Properties Data. */
	uint8_t                                     input_type;     /**< Audio Input Type Data. */
	uint8_t                                     input_status;   /**< Audio Input Status Data. */
	rtk_bt_le_audio_aics_input_des_t            input_des;      /**< Audio Input Description Data. */
} rtk_bt_le_audio_aics_char_data_t;

/**
 * @typedef    rtk_bt_le_audio_aics_client_notify_ind_t
 * @brief     Bluetooth LE audio aics client receive notification indicate.
 */
typedef struct {
	uint16_t                            conn_handle;
	uint8_t                             srv_instance_id;
	rtk_bt_le_audio_aics_char_type_t    type;
	rtk_bt_le_audio_aics_char_data_t    data;
} rtk_bt_le_audio_aics_client_notify_ind_t;

/**
 * @typedef    rtk_bt_le_audio_aics_client_read_result_ind_t
 * @brief     Bluetooth LE audio aics client read characteristic result indicate.
 */
typedef struct {
	uint16_t                         conn_handle;
	uint8_t                          srv_instance_id;
	rtk_bt_le_audio_aics_char_type_t type;
	rtk_bt_le_audio_aics_char_data_t data;
	uint16_t                         cause;
} rtk_bt_le_audio_aics_client_read_result_ind_t;

/**
 * @typedef    rtk_bt_le_audio_aics_client_cp_result_ind_t
 * @brief     Bluetooth LE audio aics client write control point result indicate.
 */
typedef struct {
	uint16_t                        conn_handle;
	uint8_t                         srv_instance_id;
	rtk_bt_le_audio_aics_cp_op_t    cp_op;
	uint16_t                        cause;
} rtk_bt_le_audio_aics_client_cp_result_ind_t;
#endif

/**
 * @typedef    rtk_bt_le_audio_cap_param_config_type_t
 * @brief     Bluetooth LE audio cap config type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_CAP_CSIS_SIRK_CONFIG = 0x01,     /**< rtk_bt_le_audio_broadcast_source_config_t. */
} rtk_bt_le_audio_cap_param_config_type_t;

/**
 * @typedef    rtk_bt_le_audio_cap_param_config_t
 * @brief     Bluetooth LE audio cap parameter config struct .
 */
typedef union {
	rtk_bt_le_audio_cap_param_config_type_t cfg_type;
	union {
		uint8_t csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN];
	} cfg;
} rtk_bt_le_audio_cap_param_config_t;
/* ------------------------------ Functions Declaration ------------------------------ */
/**
 * @defgroup  bt_cap BT CAP APIs
 * @brief     BT CAP related function APIs
 * @ingroup   bt_le_audio
 * @{
 */

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/**
 * @brief     BLE AUDIO generate RSI for CSIS.
 * @param[in] p_sirk: sirk, set identity resolving key
 * @param[out] p_rsi: rsi, resolvable set identifier
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_gen_rsi(uint8_t *p_sirk, uint8_t *p_rsi);

/**
 * @brief     BLE AUDIO change SIRK for CSIS.
 * @param[in] sirk_type: sirk type
 * @param[out] p_sirk: new sirk, set identity resolving key
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_update_sirk(rtk_bt_le_audio_csis_sirk_type_t sirk_type, uint8_t *p_sirk);
/**
 * @brief     BLE AUDIO change SIZE for CSIS.
 * @param[in] csis_size: csis size
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_update_size(uint8_t csis_size);
#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/**
 * @brief     BLE AUDIO CHEK ADV RSI for CSIS SET COORDINATOR.
 * @param[in] adv_addr: adv addr
 * @param[in] report_data_len: report data len
 * @param[in] p_report_data: report data
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi(rtk_bt_le_addr_t adv_addr, uint8_t report_data_len, uint8_t *p_report_data);
/**
 * @brief     BLE AUDIO CONFIG DISCOVER for CSIS SET COORDINATOR.
 * @param[in] group_handle: group handle
 * @param[in] discover: discover
 * @param[in] timeout_ms: timeout,unit:ms
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_cfg_discover(rtk_bt_le_audio_group_handle_t group_handle, bool discover, uint32_t timeout_ms);
/**
 * @brief     BLE AUDIO CHEK ADD GROUP for CSIS SET COORDINATOR.
 * @param[in] p_mem_info: mem info
 * @param[out] p_group_handle: returned group handle
 * @param[out] p_device_handle: returned device handle
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_add_group(rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info, rtk_bt_le_audio_group_handle_t *p_group_handle,
														rtk_bt_le_audio_device_handle_t *p_device_handle);
/**
 * @brief     BLE AUDIO ADD GROUP for CSIS SET COORDINATOR.
 * @param[in] group_handle: group handle
 * @param[in] p_mem_info: mem info
 * @param[out] p_device_handle: returned device handle
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_add_device(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info,
														 rtk_bt_le_audio_device_handle_t *p_device_handle);
/**
 * @brief     BLE AUDIO WRITE LOCK for CSIS SET COORDINATOR.
 * @param[in] group_handle: group handle
 * @param[in] lock: lock or unlock
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_write_lock(rtk_bt_le_audio_group_handle_t group_handle, bool lock);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/**
 * @brief     BLE AUDIO VCS PARAM SET.
 * @param[in]  param: vcs param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_set_param(rtk_bt_le_audio_vcs_param_t *param);

/**
 * @brief     BLE AUDIO VCS PARAM GET.
 * @param[out]  param: vcs param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_get_param(rtk_bt_le_audio_vcs_param_t *param);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @brief     BLE AUDIO VCS SEND VOLUME CP.
 * @param[in]  conn_handle: connection handle
 * @param[in]  cp_op:  cp op code
 * @param[in]  cp_param: cp param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_write_cp(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, rtk_bt_le_audio_vcs_cp_param_t *cp_param);

/**
 * @brief     BLE AUDIO VCS GET VOLUME STATE.
 * @param[in]  conn_handle: connection handle
 * @param[out] p_vcs_volume_state:  the returned vcs volume state
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_get_volume_state(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state);

/**
 * @brief     BLE AUDIO VCS GET VOLUME FLAGS.
 * @param[in]  conn_handle: connection handle
 * @param[in] p_volume_flags:  the returned vcs volume flags
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_get_volume_flags(uint16_t conn_handle, uint8_t *p_volume_flags);

/**
 * @brief     BLE AUDIO VCS CHANGE MUTE STATE.
 * @param[in]  group_idx: group index
 * @param[in] mute:  vcs mute
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_change_mute(uint8_t group_idx, uint8_t mute);

/**
 * @brief     BLE AUDIO VCS CHANGE VOLUME SETTING.
 * @param[in]  group_idx: group index
 * @param[in] volume_setting:  volume setting
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vcs_change_volume(uint8_t group_idx, uint8_t volume_setting);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @brief     BLE AUDIO MICS CHANGE MUTE STATE.
 * @param[in]  group_idx: group index
 * @param[in] mic_mute:  mic mute
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mics_change_mute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     BLE AUDIO MICS SET MUTE VALUE.
 * @param[in]  conn_handle: connect handle
 * @param[in] mic_mute:  mic mute
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mics_set_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     BLE AUDIO MICS GET MUTE VALUE.
 * @param[in]  conn_handle: connect handle
 * @param[in] p_mic_mute:  returned mic mute
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mics_get_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
/**
 * @brief     BLE AUDIO MICS SET MUTE STATE.
 * @param[in] param:  mics param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mics_set_param(rtk_bt_le_audio_mics_param_t *param);

/**
 * @brief     BLE AUDIO MICS SET MUTE STATE.
 * @param[out] param:  returned mics param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mics_get_param(rtk_bt_le_audio_mics_param_t *param);
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @brief     BLE AUDIO VOCS SET PARAMETER.
 * @param[in] srv_instance_id:  service instance id. Used when the number of services is greater than 1.
 * @param[in] set_mask:  set_mask
 * @param[in] p_vocs_param:  pointer to the location of volume offset control service parameter
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_set_param(uint8_t srv_instance_id, uint8_t set_mask, rtk_bt_le_audio_vocs_param_t *p_vocs_param);

/**
 * @brief     BLE AUDIO VOCS GET PARAMETER.
 * @param[in] srv_instance_id:  service instance id. Used when the number of services is greater than 1.
 * @param[out] p_vocs_param:  returned the location of volume offset control service parameter
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_vocs_param_t *p_vocs_param);

/**
 * @brief     BLE AUDIO VOCS CONFIG CCCD.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] cfg_flags:  VOCS CCCD Flag: @ref rtk_bt_le_audio_vocs_cccd_flags_t
 * @param[in] enable:  Whether to enable CCCD.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable);

/**
 * @brief     BLE AUDIO VOCS READ CHAR VALUE. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] type:  @ref rtk_bt_le_audio_vocs_char_type_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_char_type_t type);

/**
 * @brief     BLE AUDIO VOCS WRITE CONTROL POINT. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] cp_op:  @ref rtk_bt_le_audio_vocs_cp_op_t
 * @param[in] cp_param:  @ref rtk_bt_le_audio_vocs_cp_param_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
									   rtk_bt_le_audio_vocs_cp_param_t cp_param);

/**
 * @brief     BLE AUDIO VOCS WRITE CONTROL POINT BY GROUP. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND.
 * @param[in] group_idx: group index
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] cp_op:  @ref rtk_bt_le_audio_vocs_cp_op_t
 * @param[in] cp_param:  @ref rtk_bt_le_audio_vocs_cp_param_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
												rtk_bt_le_audio_vocs_cp_param_t cp_param);

/**
 * @brief     BLE AUDIO VOCS WRITE AUDIO LOCATION.
 * @param[in] group_handle:  group handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] audio_location:  @ref rtk_bt_le_audio_location_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t audio_location);

/**
 * @brief     BLE AUDIO VOCS WRITE OUTPUT DESCRIPTOR.
 * @param[in] conn_handle:  conn handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[in] output_des_len:  length of the output descriptor value
 * @param[in] p_output_des:  output descriptor value.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t output_des_len, uint8_t *p_output_des);
/**
 * @brief     BLE AUDIO VOCS WRITE GET SERVICE DATA.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: VOCS service index.
 * @param[out] p_srv_data:  the returned service data @ref rtk_bt_le_audio_vocs_srv_data_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_vocs_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data);
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @brief     BLE AUDIO AICS SET PARAMETER.
 * @param[in] srv_instance_id:  service instance id. Used when the number of services is greater than 1.
 * @param[in] aics_param_type:  AICS service parameter type
 * @param[in] value_len:  The value length of p_value
 * @param[in] p_value:  pointer to the AICS parameter data
 * @param[in] set_change_counter:  Whether to set change counter
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_set_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value,
										bool set_change_counter);

/**
 * @brief     BLE AUDIO AICS GET PARAMETER.
 * @param[in] srv_instance_id:  service instance id. Used when the number of services is greater than 1.
 * @param[in] aics_param_type:  AICS service parameter type, except RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES
 * @param[in] value_len:  The value length of p_value
 * @param[out] p_value:  returned to the AICS parameter data
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value);

/**
 * @brief     BLE AUDIO AICS CONFIG CCCD.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] cfg_flags:  AICS CCCD Flag: @ref rtk_bt_le_audio_aics_cccd_flags_t
 * @param[in] enable:  Whether to enable CCCD.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable);

/**
 * @brief     BLE AUDIO AICS READ CHAR VALUE. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] type:  @ref rtk_bt_le_audio_aics_char_type_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_char_type_t type);

/**
 * @brief     BLE AUDIO AICS WRITE CONTROL POINT. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] cp_op:  @ref rtk_bt_le_audio_aics_cp_op_t
 * @param[in] cp_param:  @ref rtk_bt_le_audio_aics_cp_param_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
									   rtk_bt_le_audio_aics_cp_param_t cp_param);

/**
 * @brief     BLE AUDIO AICS WRITE CONTROL POINT BY GROUP. If this API is called successfully, the Application will receive RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND.
 * @param[in] group_idx: group index.
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] cp_op:  @ref rtk_bt_le_audio_aics_cp_op_t
 * @param[in] cp_param:  @ref rtk_bt_le_audio_aics_cp_param_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
												rtk_bt_le_audio_aics_cp_param_t cp_param);

/**
 * @brief     BLE AUDIO AICS WRITE INPUT DESCRIPTOR.
 * @param[in] conn_handle:  conn handle.
 * @param[in] srv_instance_id: AICS service index.
 * @param[in] input_des_len:  length of the input descriptor value
 * @param[in] p_input_des:  input descriptor value.
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_write_input_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t input_des_len, uint8_t *p_input_des);

/**
 * @brief     BLE AUDIO AICS WRITE GET SERVICE DATA.
 * @param[in] conn_handle:  connect handle.
 * @param[in] srv_instance_id: AICS service index.
 * @param[out] p_srv_data:  the returned service data @ref rtk_bt_le_audio_aics_srv_data_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_aics_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_srv_data_t *p_srv_data);

#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/**
 * @brief     BLE AUDIO MCS CLIENT WRITE MEDIA CP. If write operation is successful and Media Control Server sends Media Control Point Notification,
 *              Media Control Point Notification will be returned by callback registered by @ref rtk_bt_evt_register_callback with msg @ref RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND.
 * @param[in] conn_handle: connection handle
 * @param[in] srv_instance_id: Service Instance Id
 * @param[in] general_mcs: is generic media control service (GMCS)
 * @param[in] is_req: is request, true: Write request, false: Write command
 * @param[in] opcode: op code. @ref rtk_bt_le_audio_mcs_media_cp_opcode_t
 * @param[in] p_op_param: op code param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mcs_client_write_media_cp(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, bool is_req, uint8_t opcode,
												   void *p_op_param);

/**
 * @brief     BLE AUDIO MCS CLIENT READ CHAR VALUE. If read operation is successful, the result of read characteristic value will be returned by
 *              callback registered by @ref rtk_bt_evt_register_callback with msg @ref RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND.
 * @param[in] conn_handle: connection handle
 * @param[in] srv_instance_id: Service Instance Id
 * @param[in] general_mcs: is generic media control service (GMCS)
 * @param[in] char_uuid: char_uuid, only support RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION,
 *                      RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mcs_client_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint16_t char_uuid);

/**
 * @brief     BLE AUDIO MCS CLIENT CONFIG CCCD.
 * @param[in] conn_handle: connection handle
 * @param[in] srv_instance_id: Service Instance Id
 * @param[in] general_mcs: is generic media control service (GMCS)
 * @param[in] cfg_flags: cfg flags
 * @param[in] enable: enable
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mcs_client_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint32_t cfg_flags, bool enable);
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/**
 * @brief     BLE AUDIO MCS SERVER SEND DATA.
 * @param[in] char_uuid: char uuid, only support RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION,
 *                      RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION, RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED
 * @param[in] p_param: param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mcs_server_send_data(uint16_t char_uuid, void *p_param);

/**
 * @brief     BLE AUDIO MCS SERVER SET PARAM.
 * @param[in] conn_handle: conn handle
 * @param[in] cid: cid
 * @param[in] service_id: service id
 * @param[in] char_uuid: char uuid
 * @param[in] offset: offset
 * @param[in] p_param: param
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_le_audio_mcs_server_read_confirm(uint16_t conn_handle, uint16_t cid, uint8_t service_id, uint16_t char_uuid, uint16_t offset, void *p_param);
#endif

/**
 * @brief     Config cap parameters.
 * @param[in] p_param: config parameter struct ref rtk_bt_le_audio_cap_param_config_t
 * @return
 *            - RTK_BT_OK  : Succeed
 *            - Others: Failed
 */
uint16_t rtk_bt_cap_param_config(rtk_bt_le_audio_cap_param_config_t *p_param);

/**
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_CAP_H__ */