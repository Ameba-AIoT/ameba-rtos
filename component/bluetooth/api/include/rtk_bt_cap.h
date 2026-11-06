/**
 * @file      rtk_bt_cap.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE Audio Common Audio Profile (CAP) and related service type and function definitions.
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
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
/* CAP role definitions. */
#define RTK_BT_CAP_ACCEPTOR_ROLE    0x01                         /*!< Acceptor Role. */
#define RTK_BT_CAP_INITIATOR_ROLE   0x02                         /*!< Initiator Role. */
#define RTK_BT_CAP_COMMANDER_ROLE   0x04                         /*!< Commander Role. */
#define RTK_BT_LE_CSIS_SIRK_LEN (16)                             /*!< Length of the Set Identity Resolving Key (SIRK) in octets. */
#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/* CSIP role definitions. */
#define RTK_BT_CSIP_ROLE_SET_COORDINATOR 0x00                    /*!< Set Coordinator Role. */
#define RTK_BT_CSIP_ROLE_SET_MEMBER 0x01                         /*!< Set Member Role. */
#define RTK_BT_LE_CSI_RSI_LEN 6                                  /*!< Length of the Resolvable Set Identifier (RSI) in octets. */
/* Coordinated Set Identification Service (CSIS) feature flags. */
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_LOCK_EXIST 0x01          /*!< Set Member Lock characteristic exists. */
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_EXIST 0x02          /*!< Coordinated Set Size characteristic exists. */
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_RANK_EXIST 0x04          /*!< Set Member Rank characteristic exists. */
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIRK_NOTIFY_SUPPORT 0x10 /*!< SIRK characteristic supports notification. */
#define RTK_BT_LE_AUDIO_CSIS_SET_MEMBER_SIZE_NOTIFY_SUPPORT 0x20 /*!< Coordinated Set Size characteristic supports notification. */
#endif
#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/* Volume Control Service (VCS) value limits and defaults. */
#define RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL 15                                                                             /*!< Maximum VCS output level. */
#define RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING 255                                                                             /*!< Maximum VCS volume setting. */
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_SETTING 153                                                                         /*!< Default VCS volume setting. */
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_MUTE RTK_BT_LE_AUDIO_MICS_NOT_MUTE                                                      /*!< Default VCS mute state. */
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_CHG_CNT 0                                                                               /*!< Default VCS change counter. */
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_VOL_FLAG 0                                                                              /*!< Default VCS volume flags. */
#define RTK_BT_LE_AUDIO_DEFAULT_VCS_STEP_SIZE (RTK_BT_LE_AUDIO_MAX_VCS_VOL_SETTING/RTK_BT_LE_AUDIO_MAX_VCS_OUTPUT_LEVEL)    /*!< Default VCS volume step size. */
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/* Audio Input Control Service (AICS) definitions. */
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_INACTIVE 0                                                                       /*!< AICS audio input is inactive. */
#define RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_ACTIVE 1                                                                         /*!< AICS audio input is active. */
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM 1                                                                             /*!< Default number of AICS in VCS. */
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM 1                                                                            /*!< Default number of AICS in MICS. */
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM (RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM + RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM)    /*!< Default total number of AICS instances. */
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX 0                                                                       /*!< AICS index of the Bluetooth input. */
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX 1                                                                             /*!< AICS index of the microphone input. */
#define RTK_BT_LE_AUDIO_BLUETOOTH_DES "Ameba Bluetooth"                                                                    /*!< Description of the Bluetooth input. */
#define RTK_BT_LE_AUDIO_MIC_DES "Ameba MIC"                                                                                /*!< Description of the microphone input. */
#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/* Audio channel and Volume Offset Control Service (VOCS) definitions. */
#define RTK_BT_LE_AUDIO_LEFT 1                      /*!< Left audio channel. */
#define RTK_BT_LE_AUDIO_RIGHT 2                     /*!< Right audio channel. */
#define RTK_BT_LE_AUDIO_STEREO 3                    /*!< Stereo (left and right) audio channels. */
#define RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM 1          /*!< Default number of VOCS instances. */
#define RTK_BT_LE_AUDIO_VOCS_DES "Ameba Speaker"    /*!< Description string of the VOCS instance. */
#define RTK_BT_LE_AUDIO_VOCS_IDX 0                  /*!< Default VOCS instance index. */
#define RTK_BT_LE_AUDIO_VOCS_MAX_VAL 255            /*!< Maximum magnitude of the VOCS volume offset. */
#endif

#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
#define RTK_BT_LE_AUDIO_GMCS_CCID  0x20             /*!< Content Control ID (CCID) of the Generic Media Control Service (GMCS). */
#endif

#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
#define RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX 2         /*!< Maximum number of calls in the call list. */
#endif
/**
 * @typedef   rtk_bt_le_audio_cap_role_t
 * @brief     Bluetooth LE Audio Common Audio Profile (CAP) role.
 */
typedef enum {
	RTK_BT_LE_AUDIO_CAP_ROLE_UNKNOWN = 0,                              /*!< Unknown CAP role. */
	RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR = RTK_BT_CAP_INITIATOR_ROLE,    /*!< CAP Initiator. */
	RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR = RTK_BT_CAP_ACCEPTOR_ROLE,      /*!< CAP Acceptor. */
	RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER = RTK_BT_CAP_COMMANDER_ROLE,    /*!< CAP Commander. */
} rtk_bt_le_audio_cap_role_t;

#if (defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT)
/**
 * @typedef   rtk_bt_le_audio_csis_lock_t
 * @brief     Bluetooth LE Audio CSIS lock state.
 */
typedef enum {
	RTK_BT_LE_CSIS_NONE_LOCK  = 0,       /*!< No lock state. */
	RTK_BT_LE_CSIS_UNLOCKED   = 0x01,    /*!< Set member is unlocked. */
	RTK_BT_LE_CSIS_LOCKED     = 0x02,    /*!< Set member is locked. */
} rtk_bt_le_audio_csis_lock_t;

/**
 * @typedef   rtk_bt_le_audio_csis_sirk_type_t
 * @brief     Bluetooth LE Audio CSIS SIRK type.
 */
typedef enum {
	RTK_BT_LE_CSIS_SIRK_ENC = 0x00,    /*!< Encrypted SIRK. */
	RTK_BT_LE_CSIS_SIRK_PLN = 0x01,    /*!< Plain text SIRK. */
} rtk_bt_le_audio_csis_sirk_type_t;
#endif

/************************************** Data Types *************************************/
/* act struct */
#if defined(RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_CCP_CALL_CONTROL_SERVER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_ccp_state_t
 * @brief     Bluetooth LE Audio state of a call in the telephone bearer service.
 */
typedef enum {
	RTK_BT_LEA_TBS_CALL_STATE_INCOMING                                                        = 0x00,    /*!< A remote party is calling (incoming call). */
	RTK_BT_LEA_TBS_CALL_STATE_DIALING                                                         = 0x01,    /*!< The outgoing call is being dialed, the remote party is not
                                                                                                          *   yet alerted. */
	RTK_BT_LEA_TBS_CALL_STATE_ALERTING                                                        = 0x02,    /*!< The remote party is being alerted (outgoing call). */
	RTK_BT_LEA_TBS_CALL_STATE_ACTIVE                                                          = 0x03,    /*!< The call is in an active conversation. */
	RTK_BT_LEA_TBS_CALL_STATE_LOCALLY_HELD                                                    = 0x04,    /*!< The call is held locally, the state can be changed by the
                                                                                                          *   server or the client. */
	RTK_BT_LEA_TBS_CALL_STATE_REMOTELY_HELD                                                   = 0x05,    /*!< The call is held remotely, the state is controlled by the
                                                                                                          *   remote party. */
	RTK_BT_LEA_TBS_CALL_STATE_LOCALLY_AND_REMOTELY_HELD                                       = 0x06,    /*!< The call is held both locally and remotely. */
	RTK_BT_LEA_TBS_CALL_STATE_RFU,                                                                       /*!< Reserved for future use. */
} rtk_bt_le_audio_ccp_state_t;

/**
 * @typedef   rtk_bt_le_audio_tbs_call_t
 * @brief     Bluetooth LE Audio call entry of the telephone bearer service.
 */
typedef struct {
	uint8_t                     call_index;    /*!< Call index assigned by the server to identify the call. */
	uint8_t                     call_flags;    /*!< Call flags bit field. */
	rtk_bt_le_audio_ccp_state_t call_state;    /*!< Current state of the call. */
} rtk_bt_le_audio_tbs_call_t;

/**
 * @typedef   rtk_bt_le_audio_ccp_t
 * @brief     Bluetooth LE Audio call control server data.
 */
typedef struct {
	uint8_t                    gtbs_id;                                         /*!< Generic Telephone Bearer Service (GTBS) service ID. */
	uint16_t                   ccp_enabled_cccd;                                /*!< CCCD enable flags of the CCP service. */
	uint8_t                    call_num;                                        /*!< Number of valid entries in call_list. */
	rtk_bt_le_audio_tbs_call_t call_list[RTK_BT_LE_AUDIO_CCP_CALL_LIST_MAX];    /*!< List of the current calls. */
} rtk_bt_le_audio_ccp_t;
#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT

/**
 * @typedef   rtk_bt_le_audio_csis_cfg_t
 * @brief     Bluetooth LE Audio CSIS set member configuration.
 */
typedef enum {
	RTK_BT_LEA_CSIS_CFG_NOT_EXIST     = 0,    /*!< CSIS is not configured. */
	RTK_BT_LEA_CSIS_CFG_RANK_1        = 1,    /*!< Set member with rank 1. */
	RTK_BT_LEA_CSIS_CFG_RANK_2        = 2,    /*!< Set member with rank 2. */
} rtk_bt_le_audio_csis_cfg_t;

/**
 * @typedef   rtk_bt_le_audio_csis_gen_rsi_info_t
 * @brief     Bluetooth LE Audio CSIS RSI generation parameters.
 */
typedef struct {
	uint8_t sirk[RTK_BT_LE_CSIS_SIRK_LEN];    /*!< SIRK used to generate the RSI. */
	uint8_t *p_rsi;                           /*!< Output: receives the generated RSI.
                                               *   Length is @ref RTK_BT_LE_CSI_RSI_LEN octets. */
} rtk_bt_le_audio_csis_gen_rsi_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_update_sirk_param_t
 * @brief     Bluetooth LE Audio CSIS SIRK update parameters.
 */
typedef struct {
	rtk_bt_le_audio_csis_sirk_type_t sirk_type;    /*!< Type of the new SIRK. */
	uint8_t sirk[RTK_BT_LE_CSIS_SIRK_LEN];         /*!< New SIRK. */
} rtk_bt_le_audio_csis_update_sirk_param_t;

/**
 * @typedef   rtk_bt_le_audio_csis_update_size_param_t
 * @brief     Bluetooth LE Audio CSIS coordinated set size update parameters.
 */
typedef struct {
	uint8_t csis_size;    /*!< Coordinated Set Size characteristic value.
                           *   Range: 0x01 to 0xFF. */
} rtk_bt_le_audio_csis_update_size_param_t;

#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t
 * @brief     Bluetooth LE Audio CSIS set coordinator RSI check parameters of an advertising report.
 */
typedef struct {
	uint8_t report_data_len;      /*!< Length of the advertising report data in octets. */
	uint8_t *p_report_data;       /*!< Advertising report data to be checked for an RSI. */
	rtk_bt_le_addr_t adv_addr;    /*!< Address of the advertiser. */
} rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t
 * @brief     Bluetooth LE Audio CSIS set coordinator set member discovery configuration parameters.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;    /*!< Handle of the coordinated set group. */
	bool discover;                                  /*!< Whether to discover set members.
                                                     *   - true: Start discovering set members.
                                                     *   - false: Stop discovering set members. */
	uint32_t timeout_ms;                            /*!< Discovery timeout in ms. */
} rtk_bt_le_audio_csis_set_coordinator_cfg_discover_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_set_mem_info_t
 * @brief     Bluetooth LE Audio CSIS set member information.
 */
typedef struct {
	rtk_bt_le_addr_t                    bd_addr;                          /*!< Address of the set member. */
	uint16_t                            srv_uuid;                         /*!< UUID of the CSIS service. */
	uint8_t                             srv_instance_id;                  /*!< CSIS service instance ID, used when more than one CSIS instance. */
	uint8_t                             char_exit;                        /*!< Bit field of the CSIS characteristics present on the remote device. */
	uint8_t                             rank;                             /*!< Rank of the set member. */
	rtk_bt_le_audio_csis_lock_t         lock;                             /*!< Lock state of the set member. */
	uint8_t                             set_mem_size;                     /*!< Size of the coordinated set. */
	rtk_bt_le_audio_csis_sirk_type_t    sirk_type;                        /*!< Type of the SIRK. */
	uint8_t                             sirk[RTK_BT_LE_CSIS_SIRK_LEN];    /*!< SIRK of the coordinated set. */
} rtk_bt_le_audio_csis_set_mem_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_set_coordinator_add_group_info_t
 * @brief     Bluetooth LE Audio CSIS set coordinator add group parameters.
 */
typedef struct {
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;        /*!< Information of the first set member in the group. */
	rtk_bt_le_audio_group_handle_t *p_group_handle;      /*!< Output: receives the handle of the created group. */
	rtk_bt_le_audio_device_handle_t *p_device_handle;    /*!< Output: receives the handle of the added device. */
} rtk_bt_le_audio_csis_set_coordinator_add_group_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_set_coordinator_add_device_info_t
 * @brief     Bluetooth LE Audio CSIS set coordinator add device parameters.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;         /*!< Handle of the group the device is added to. */
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;        /*!< Information of the set member to add. */
	rtk_bt_le_audio_device_handle_t *p_device_handle;    /*!< Output: receives the handle of the added device. */
} rtk_bt_le_audio_csis_set_coordinator_add_device_info_t;

/**
 * @typedef   rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t
 * @brief     Bluetooth LE Audio CSIS set coordinator lock write parameters.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t group_handle;    /*!< Handle of the coordinated set group. */
	bool lock;                                      /*!< Whether to lock or unlock the set members.
                                                     *   - true: Lock the set members.
                                                     *   - false: Unlock the set members. */
} rtk_bt_le_audio_csis_set_coordinator_write_lock_info_t;

#endif

#if (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT)
/**
 * @typedef   rtk_bt_le_audio_mcp_t
 * @brief     Bluetooth LE Audio media control service data.
 */
typedef struct {
	uint8_t       gmcs_id;             /*!< Generic Media Control Service (GMCS) service ID. */
	uint8_t       media_state;         /*!< Current media state, see @ref rtk_bt_le_audio_mcs_media_state_t. */
	uint16_t      mcp_enabled_cccd;    /*!< CCCD enable flags of the MCP service. */
} rtk_bt_le_audio_mcp_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_char_uuid_t
 * @brief     Bluetooth LE Audio media control service characteristic UUIDs in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME             = 0x2B93,           /*!< Media Player Name. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL         = 0x2B95,           /*!< Media Player Icon URL. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED                 = 0x2B96,           /*!< Track Changed. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE                   = 0x2B97,           /*!< Track Title. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION                = 0x2B98,           /*!< Track Duration. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION                = 0x2B99,           /*!< Track Position. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED                = 0x2B9A,           /*!< Playback Speed. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED                 = 0x2B9B,           /*!< Seeking Speed. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER                 = 0x2BA1,           /*!< Playing Order. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED      = 0x2BA2,           /*!< Playing Orders Supported. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE                   = 0x2BA3,           /*!< Media State. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT           = 0x2BA4,           /*!< Media Control Point. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODE_SUPPORTED = 0x2BA5,    /*!< Media Control Point Opcodes Supported. */
	RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID            = 0x2BBA            /*!< Content Control ID. */
} rtk_bt_le_audio_mcs_char_uuid_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_cccd_flag_t
 * @brief     Bluetooth LE Audio media control service CCCD flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_PLAYER_NAME                       = 0x00000001,    /*!< CCCD of the Media Player Name characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_CHANGED                           = 0x00000002,    /*!< CCCD of the Track Changed characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_TITLE                             = 0x00000004,    /*!< CCCD of the Track Title characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_DURATION                          = 0x00000008,    /*!< CCCD of the Track Duration characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_TRACK_POSITION                          = 0x00000010,    /*!< CCCD of the Track Position characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_STATE                             = 0x00000020,    /*!< CCCD of the Media State characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT                     = 0x00000040,    /*!< CCCD of the Media Control Point characteristic. */
	RTK_BT_LE_AUDIO_MCP_CLIENT_CFG_CCCD_FLAG_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED   = 0x00000080     /*!< CCCD of the Media Control Point Opcodes Supported
                                                                                                       *   characteristic. */
} rtk_bt_le_audio_mcs_cccd_flag_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_media_cp_opcode_t
 * @brief     Bluetooth LE Audio media control point opcodes in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY                  = 0x01,    /*!< Play the current track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE                 = 0x02,    /*!< Pause the current track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND           = 0x03,    /*!< Fast rewind the current track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD          = 0x04,    /*!< Fast forward the current track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP                  = 0x05,    /*!< Stop playing and reset the track position. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE         = 0x10,    /*!< Move the track position by a relative offset. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT          = 0x24,    /*!< Go to the given segment of the current track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK        = 0x30,    /*!< Go to the previous track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK            = 0x31,    /*!< Go to the next track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_TRACK           = 0x32,    /*!< Go to the first track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_TRACK            = 0x33,    /*!< Go to the last track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK            = 0x34,    /*!< Go to the given track. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP            = 0x44,    /*!< Go to the given group. */
} rtk_bt_le_audio_mcs_media_cp_opcode_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_media_state_t
 * @brief     Bluetooth LE Audio media state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_INACTIVE = 0x00,    /*!< Media player is inactive. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PLAYING,            /*!< Media is playing. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PAUSED,             /*!< Media is paused. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_SEEKING             /*!< Media is seeking. */
} rtk_bt_le_audio_mcs_media_state_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_playing_orders_supported_t
 * @brief     Bluetooth LE Audio supported playing order bit values.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SINGLE_ONCE = 0x0001,        /*!< Single track played once. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SINGLE_REPEAT = 0x0002,      /*!< Single track repeated. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_IN_ORDER_ONCE = 0x0004,      /*!< Tracks played in order once. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_IN_ORDER_REPEAT = 0x0008,    /*!< Tracks played in order and repeated. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_OLDEST_ONCE = 0x0010,        /*!< Oldest track first, played once. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_OLDEST_REPEAT  = 0x0020,     /*!< Oldest track first, repeated. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_NEWEST_ONCE = 0x0040,        /*!< Newest track first, played once. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_NEWEST_REPEAT = 0x0080,      /*!< Newest track first, repeated. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SHUFFLE_ONCE = 0x0100,       /*!< Tracks shuffled, played once. */
	RTK_BT_LE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_CHAR_BIT_VALUE_SHUFFLE_REPEAT = 0x0200      /*!< Tracks shuffled and repeated. */
} rtk_bt_le_audio_mcs_playing_orders_supported_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t
 * @brief     Bluetooth LE Audio supported media control point opcode bit values.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PLAY = 0x00000001,              /*!< Play opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PAUSE = 0x00000002,             /*!< Pause opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_REWIND = 0x00000004,       /*!< Fast rewind opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_FORWARD = 0x00000008,      /*!< Fast forward opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_STOP = 0x00000010,              /*!< Stop opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_MOVE_RELATIVE = 0x00000020,     /*!< Move relative opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_TRACK = 0x00000800,    /*!< Previous track opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_TRACK = 0x00001000,        /*!< Next track opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FIRST_TRACK = 0x00002000,       /*!< First track opcode is supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_LAST_TRACK = 0x00004000,        /*!< Last track opcode is supported. */
} rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t
 * @brief     Bluetooth LE Audio media control point notification result codes in MCS or GMCS.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_SUCCESS                    = 0x01,    /*!< Opcode was executed successfully. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED       = 0x02,    /*!< Opcode is not supported. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_MEDIA_PLAYER_INACTIVE      = 0x03,    /*!< Media player is inactive. */
	RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_COMMAND_CANNOT_BE_COMPLETED = 0x04    /*!< Command cannot be completed. */
} rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_media_play_name_t
 * @brief     Bluetooth LE Audio media player name of the MCP server.
 */
typedef struct {
	uint8_t       *p_media_player_name;     /*!< Media player name, UTF-8 string. */
	uint16_t      media_player_name_len;    /*!< Length of the media player name in octets. */
} rtk_bt_le_audio_mcp_server_media_play_name_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_track_title_t
 * @brief     Bluetooth LE Audio track title of the MCP server.
 */
typedef struct {
	uint8_t       *p_track_title;     /*!< Track title, UTF-8 string. */
	uint16_t      track_title_len;    /*!< Length of the track title in octets. */
} rtk_bt_le_audio_mcp_server_track_title_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_media_player_icon_url_t
 * @brief     Bluetooth LE Audio media player icon URL of the MCP server.
 */
typedef struct {
	uint8_t       *p_media_player_icon_url;     /*!< Media player icon URL, UTF-8 string. */
	uint16_t      media_player_icon_url_len;    /*!< Length of the media player icon URL in octets. */
} rtk_bt_le_audio_mcp_server_media_player_icon_url_t;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_mcp_client_write_media_cp_info_t
 * @brief     Bluetooth LE Audio MCS client media control point write parameters.
 */
typedef struct {
	uint16_t    conn_handle;                      /*!< Connection handle of the ACL connection to the media control server.
                                                   *   Range: 0x0000 to 0x0EFF. */
	uint8_t     srv_instance_id;                  /*!< Service instance ID. */
	bool        general_mcs;                      /*!< Whether the target is the Generic Media Control Service (GMCS).
                                                   *   - true: GMCS.
                                                   *   - false: MCS. */
	uint8_t     opcode;                           /*!< Media control point opcode, see @ref rtk_bt_le_audio_mcs_media_cp_opcode_t. */
	union {
		int32_t   move_relative_opcode_offset;    /*!< Offset of the move relative opcode. */
		int32_t   goto_segment_opcode_n;          /*!< Segment number of the go to segment opcode. */
		int32_t   goto_track_opcode_n;            /*!< Track number of the go to track opcode. */
		int32_t   goto_group_opcode_n;            /*!< Group number of the go to group opcode. */
	} op_param;                                   /*!< Opcode parameter, valid for the opcodes that carry a parameter. */
	bool        is_req;                           /*!< Write type of the media control point write.
                                                   *   - true: Write request.
                                                   *   - false: Write command. */
} rtk_bt_le_audio_mcp_client_write_media_cp_info_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_client_read_char_value_info_t
 * @brief     Bluetooth LE Audio MCS client characteristic read parameters.
 */
typedef struct {
	uint16_t    conn_handle;        /*!< Connection handle of the ACL connection to the media control server.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t     srv_instance_id;    /*!< Service instance ID. */
	bool        general_mcs;        /*!< Whether the target is the Generic Media Control Service (GMCS).
                                     *   - true: GMCS.
                                     *   - false: MCS. */
	uint16_t    char_uuid;          /*!< UUID of the characteristic to read, see @ref rtk_bt_le_audio_mcs_char_uuid_t. */
} rtk_bt_le_audio_mcp_client_read_char_value_info_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_client_cfg_cccd_info_t
 * @brief     Bluetooth LE Audio MCS client CCCD configuration parameters.
 */
typedef struct {
	uint16_t    conn_handle;        /*!< Connection handle of the ACL connection to the media control server.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t     srv_instance_id;    /*!< Service instance ID. */
	bool        general_mcs;        /*!< Whether the target is the Generic Media Control Service (GMCS).
                                     *   - true: GMCS.
                                     *   - false: MCS. */
	uint32_t    cfg_flags;          /*!< Bit mask of the CCCDs to configure, see @ref rtk_bt_le_audio_mcs_cccd_flag_t. */
	bool        enable;             /*!< Whether to enable or disable the notification.
                                     *   - true: Enable.
                                     *   - false: Disable. */
} rtk_bt_le_audio_mcp_client_cfg_cccd_info_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio Media control service discovery done indication of the MCP client.
 */
typedef struct {
	uint16_t    conn_handle;      /*!< Connection handle of the ACL connection to the media control server.
                                   *   Range: 0x0000 to 0x0EFF. */
	bool        is_found;         /*!< Whether the media control service is found.
                                   *   - true: Found.
                                   *   - false: Not found. */
	bool        load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                                   *   - true: Loaded from FTL.
                                   *   - false: Discovered over the air. */
	bool        general_mcs;      /*!< Whether the discovered service is the Generic Media Control Service (GMCS).
                                   *   - true: GMCS.
                                   *   - false: MCS. */
	uint8_t     srv_num;          /*!< Number of discovered service instances. */
} rtk_bt_le_audio_mcp_client_discovery_done_ind_t;
/**
 * @typedef   rtk_bt_le_audio_mcp_client_read_result_ind_t
 * @brief     Bluetooth LE Audio MCP client read result indication.
 */
typedef struct {
	uint16_t    cause;                                                     /*!< Result code of the read operation. 0 indicates success. */
	uint16_t    conn_handle;                                               /*!< Connection handle of the ACL connection.
                                                                            *   Range: 0x0000 to 0x0EFF. */
	bool        general_mcs;                                               /*!< Whether the service is the Generic Media Control Service (GMCS).
                                                                            *   - true: GMCS.
                                                                            *   - false: MCS. */
	uint8_t     srv_instance_id;                                           /*!< Index of the service instance. */
	uint16_t    char_uuid;                                                 /*!< UUID of the characteristic that was read. */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*!< Media player name.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME. */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;              /*!< Track title.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE. */
		int32_t track_duration;                                            /*!< Length of the current track, zero or greater.
                                                                            *   Unit: 0.01 s.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION.
                                                                            *   0xFFFFFFFF: No current track, or the track duration is unknown. */
		int32_t track_position;                                            /*!< Current playing position, as an offset from the start of the track.
                                                                            *   Unit: 0.01 s.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION.
                                                                            *   0x00000000: Starting position, or the start of the track is not well defined.
                                                                            *   0xFFFFFFFF: No current track, or the start of the track is not well defined. */
		int8_t  playback_speed;                                            /*!< Playback speed of the current track.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED. */
		int8_t  seeking_speed;                                             /*!< Seeking speed of the media player.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED. */
		uint8_t playing_order;                                             /*!< Current playing order of the media player.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER. */
		uint8_t media_state;                                               /*!< Current media state, see @ref rtk_bt_le_audio_mcs_media_state_t.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE. */
		uint8_t content_control_id;                                        /*!< Content Control ID, which identifies a unique instance of a service that controls or
                                                                            *   provides status information on an audio-related feature.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID. */
		uint16_t playing_orders_supported;                                 /*!< Supported playing orders, a combination of
                                                                            *   @ref rtk_bt_le_audio_mcs_playing_orders_supported_t values.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED. */
		uint32_t media_control_point_opcodes_supported;                    /*!< Supported media control point opcodes, a combination of
                                                                            *   @ref rtk_bt_le_audio_mcs_media_control_point_opcodes_supported_t values.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODE_SUPPORTED. */
	} data;                                                                /*!< Characteristic value, selected by char_uuid. */
} rtk_bt_le_audio_mcp_client_read_result_ind_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_client_notify_ind_t
 * @brief     Bluetooth LE Audio MCP client notification indication.
 */
typedef struct {
	uint16_t    conn_handle;                                               /*!< Connection handle of the ACL connection.
                                                                            *   Range: 0x0000 to 0x0EFF. */
	bool        general_mcs;                                               /*!< Whether the service is the Generic Media Control Service (GMCS).
                                                                            *   - true: GMCS.
                                                                            *   - false: MCS. */
	uint8_t     srv_instance_id;                                           /*!< Index of the service instance. */
	uint16_t    char_uuid;                                                 /*!< UUID of the characteristic carried by the notification, see
                                                                            *   @ref rtk_bt_le_audio_mcs_char_uuid_t. */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*!< Media player name.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME. */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;              /*!< Track title.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE. */
		int32_t track_duration;                                            /*!< Length of the current track, zero or greater.
                                                                            *   Unit: 0.01 s.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION.
                                                                            *   0xFFFFFFFF: No current track, or the track duration is unknown. */
		int32_t track_position;                                            /*!< Current playing position, as an offset from the start of the track.
                                                                            *   Unit: 0.01 s.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION.
                                                                            *   0x00000000: Starting position, or the start of the track is not well defined.
                                                                            *   0xFFFFFFFF: No current track, or the start of the track is not well defined. */
		int8_t  playback_speed;                                            /*!< Playback speed of the current track.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED. */
		int8_t  seeking_speed;                                             /*!< Seeking speed of the media player.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED. */
		uint8_t playing_order;                                             /*!< Current playing order of the media player.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER. */
		uint8_t media_state;                                               /*!< Current media state, see @ref rtk_bt_le_audio_mcs_media_state_t.
                                                                            *   Valid for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE. */
	} data;                                                                /*!< Characteristic value, selected by char_uuid. */
} rtk_bt_le_audio_mcp_client_notify_ind_t;

/**
 * @typedef   rtk_bt_le_audio_mcs_media_cp_nodify_ind_t
 * @brief     Bluetooth LE Audio MCP client media control point notification indication.
 */
typedef struct {
	uint16_t    conn_handle;        /*!< Connection handle of the ACL connection.
                                     *   Range: 0x0000 to 0x0EFF. */
	bool        general_mcs;        /*!< Whether the service is the Generic Media Control Service (GMCS).
                                     *   - true: GMCS.
                                     *   - false: MCS. */
	uint8_t     srv_instance_id;    /*!< Index of the service instance. */
	uint8_t requested_opcode;       /*!< Opcode of the media control point, see @ref rtk_bt_le_audio_mcs_media_cp_opcode_t. */
	uint8_t     result_code;        /*!< Result of the requested opcode, see @ref rtk_bt_le_audio_mcs_media_control_point_notification_result_codes_t. */
} rtk_bt_le_audio_mcs_media_cp_nodify_ind_t;

#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT

/**
 * @typedef   rtk_bt_le_audio_mcp_server_send_data_param_t
 * @brief     Bluetooth LE Audio MCP server send data parameters.
 */
typedef struct {
	uint16_t    char_uuid;                                                 /*!< UUID of the characteristic to send, see @ref rtk_bt_le_audio_mcs_char_uuid_t. */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;    /*!< Media player name.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME. */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;              /*!< Track title.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE. */
		int32_t track_duration;                                            /*!< Length of the current track, zero or greater.
                                                                            *   Unit: 0.01 s.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION.
                                                                            *   0xFFFFFFFF: No current track, or the track duration is unknown. */
		int32_t track_position;                                            /*!< Current playing position, as an offset from the start of the track.
                                                                            *   Unit: 0.01 s.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION.
                                                                            *   0x00000000: Starting position, or the start of the track is not well defined.
                                                                            *   0xFFFFFFFF: No current track, or the start of the track is not well defined. */
		int8_t  playback_speed;                                            /*!< Playback speed of the current track.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED. */
		int8_t  seeking_speed;                                             /*!< Seeking speed of the media player.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED. */
		uint8_t playing_order;                                             /*!< Current playing order of the media player.
                                                                            *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER. */
	} param;                                                               /*!< Characteristic value, selected by char_uuid. */
} rtk_bt_le_audio_mcp_server_send_data_param_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_read_confirm_param_t
 * @brief     Bluetooth LE Audio MCP server read confirm parameters.
 */
typedef struct {
	uint16_t  conn_handle;                                                            /*!< Connection handle of the ACL connection.
                                                                                       *   Range: 0x0000 to 0x0EFF. */
	uint16_t  cid;                                                                    /*!< L2CAP channel identifier, copied from the read indication. */
	uint8_t   service_id;                                                             /*!< Service ID of the MCS instance, copied from the read indication. */
	uint16_t  char_uuid;                                                              /*!< UUID of the characteristic being read, copied from the read indication. */
	uint16_t  offset;                                                                 /*!< Offset of the read request, copied from the read indication. */
	union {
		rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;               /*!< Media player name.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME. */
		rtk_bt_le_audio_mcp_server_media_player_icon_url_t  media_player_icon_url;    /*!< Media player icon URL.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL. */
		rtk_bt_le_audio_mcp_server_track_title_t track_title;                         /*!< Track title.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE. */
		int32_t track_duration;                                                       /*!< Length of the current track, zero or greater.
                                                                                       *   Unit: 0.01 s.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION.
                                                                                       *   0xFFFFFFFF: No current track, or the track duration is unknown. */
		int32_t track_position;                                                       /*!< Current playing position, as an offset from the start of the track.
                                                                                       *   Unit: 0.01 s.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION.
                                                                                       *   0x00000000: Starting position, or the start of the track is not well defined.
                                                                                       *   0xFFFFFFFF: No current track, or the start of the track is not well defined. */
		int8_t  playback_speed;                                                       /*!< Playback speed of the current track.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYBACK_SPEED. */
		int8_t  seeking_speed;                                                        /*!< Seeking speed of the media player.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_SEEKING_SPEED. */
		uint8_t playing_order;                                                        /*!< Current playing order of the media player.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_PLAYING_ORDER. */
		uint8_t content_control_id;                                                   /*!< Content Control ID, which identifies a unique instance of a service that
                                                                                       *   controls or provides status information on an audio-related feature.
                                                                                       *   Used for @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID. */
	} param;                                                                          /*!< Characteristic value to confirm, selected by char_uuid. */
} rtk_bt_le_audio_mcp_server_read_confirm_param_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_write_media_cp_ind_t
 * @brief     Bluetooth LE Audio MCP server write media control point indication.
 */
typedef struct {
	uint16_t        conn_handle;                  /*!< Connection handle of the ACL connection.
                                                   *   Range: 0x0000 to 0x0EFF. */
	uint16_t        cid;                          /*!< L2CAP channel identifier assigned by the host. */
	uint8_t         service_id;                   /*!< Service ID of the MCS instance. */
	uint8_t         opcode;                       /*!< Media control point opcode written by the remote client, see @ref rtk_bt_le_audio_mcs_media_cp_opcode_t. */
	union {
		int32_t   move_relative_opcode_offset;    /*!< Offset of the move relative operation.
                                                   *   Used when opcode is @ref RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE. */
		int32_t   goto_segment_opcode_n;          /*!< Segment number to go to.
                                                   *   Used when opcode is @ref RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT. */
		int32_t   goto_track_opcode_n;            /*!< Track number to go to.
                                                   *   Used when opcode is @ref RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK. */
		int32_t   goto_group_opcode_n;            /*!< Group number to go to.
                                                   *   Used when opcode is @ref RTK_BT_LE_AUDIO_MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP. */
	} param;                                      /*!< Opcode parameter, selected by opcode. */
} rtk_bt_le_audio_mcp_server_write_media_cp_ind_t;

/**
 * @typedef   rtk_bt_le_audio_mcp_server_read_ind_t
 * @brief     Bluetooth LE Audio MCP server read indication.
 */
typedef struct {
	uint16_t  conn_handle;                                      /*!< Connection handle of the ACL connection.
                                                                 *   Range: 0x0000 to 0x0EFF. */
	uint16_t  cid;                                              /*!< L2CAP channel identifier assigned by the host. */
	uint8_t   service_id;                                       /*!< Service ID of the MCS instance. */
	uint16_t  char_uuid;                                        /*!< UUID of the characteristic requested to read, see @ref rtk_bt_le_audio_mcs_char_uuid_t. */
	uint16_t  offset;                                           /*!< Offset of the read request. */
	rtk_bt_le_audio_mcp_server_read_confirm_param_t  *p_cfm;    /*!< Confirm parameters to be filled in by the APP when handling this indication. */
} rtk_bt_le_audio_mcp_server_read_ind_t;
#endif
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vcs_mute_state_t
 * @brief     Bluetooth LE Audio VCS mute state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_NOT_MUTED =  0,    /*!< Volume is not muted. */
	RTK_BT_LE_AUDIO_VCS_MUTED  = 1,        /*!< Volume is muted. */
} rtk_bt_le_audio_vcs_mute_state_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_param_t
 * @brief     Bluetooth LE Audio VCS parameters.
 */
typedef struct {
	uint8_t volume_setting;    /*!< Volume setting of the volume renderer.
                                *   Range: 0x00 to 0xFF. */
	uint8_t mute;              /*!< Mute state, see @ref rtk_bt_le_audio_vcs_mute_state_t. */
	uint8_t change_counter;    /*!< Initial value of the volume state change counter.
                                *   Takes effect only when the parameters are set for the first time. */
	uint8_t volume_flags;      /*!< Volume flags of the volume renderer. */
	uint8_t step_size;         /*!< Volume step size used by relative volume control point operations. */
} rtk_bt_le_audio_vcs_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vcs_cccd_flags_t
 * @brief     Bluetooth LE Audio VCS characteristic flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_VOLUME_STATE_FLAG = 0x01,    /*!< Volume State characteristic flag. */
	RTK_BT_LE_AUDIO_VCS_VOLUME_FLAGS_FLAG = 0x02,    /*!< Volume Flags characteristic flag. */
} rtk_bt_le_audio_vcs_cccd_flags_t;


/**
 * @typedef   rtk_bt_le_audio_vcs_cp_op_t
 * @brief     Bluetooth LE Audio VCS control point opcodes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_DOWN        = 0x00,    /*!< Relative volume down. */
	RTK_BT_LE_AUDIO_VCS_CP_RELATIVE_VOLUME_UP          = 0x01,    /*!< Relative volume up. */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN = 0x02,    /*!< Unmute and relative volume down. */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE_RELATIVE_VOLUME_UP   = 0x03,    /*!< Unmute and relative volume up. */
	RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME         = 0x04,    /*!< Set absolute volume. */
	RTK_BT_LE_AUDIO_VCS_CP_UNMUTE                      = 0x05,    /*!< Unmute. */
	RTK_BT_LE_AUDIO_VCS_CP_MUTE                        = 0x06,    /*!< Mute. */
	RTK_BT_LE_AUDIO_VCS_CP_MAX                                    /*!< Upper bound of the opcodes, not a valid opcode. */
} rtk_bt_le_audio_vcs_cp_op_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_cp_param_t
 * @brief     Bluetooth LE Audio VCS control point parameters.
 */
typedef struct {
	uint8_t volume_setting;    /*!< Volume setting to apply, used only when the opcode is @ref RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME.
                                *   Range: 0x00 to 0xFF. */
} rtk_bt_le_audio_vcs_cp_param_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_write_cp_param_t
 * @brief     Bluetooth LE Audio VCS write control point parameters.
 */
typedef struct {
	uint16_t conn_handle;                       /*!< Connection handle of the ACL connection.
                                                 *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_vcs_cp_op_t cp_op;          /*!< Volume control point opcode to write. */
	rtk_bt_le_audio_vcs_cp_param_t cp_param;    /*!< Parameters of the opcode. */
} rtk_bt_le_audio_vcs_write_cp_param_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_volume_cp_ind_t
 * @brief     Bluetooth LE Audio VCS volume control point indication.
 */
typedef struct {
	uint16_t conn_handle;                 /*!< Connection handle of the ACL connection.
                                           *   Range: 0x0000 to 0x0EFF. */
	uint8_t volume_setting;               /*!< Volume setting reported with the control point indication.
                                           *   Range: 0x00 to 0xFF. */
	uint8_t mute;                         /*!< Mute state reported with the control point indication, see @ref rtk_bt_le_audio_vcs_mute_state_t. */
	rtk_bt_le_audio_vcs_cp_op_t cp_op;    /*!< Control point opcode written by the remote client. */
} rtk_bt_le_audio_vcs_volume_cp_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_volume_state_t
 * @brief     Bluetooth LE Audio VCS volume state.
 */
typedef struct {
	uint8_t volume_setting;    /*!< Volume setting.
                                *   Range: 0x00 to 0xFF. */
	uint8_t mute;              /*!< Mute state, see @ref rtk_bt_le_audio_vcs_mute_state_t. */
	uint8_t change_counter;    /*!< Change counter of the volume state. */
} rtk_bt_le_audio_vcs_volume_state_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_get_volume_state_param_t
 * @brief     Bluetooth LE Audio VCS get volume state parameters.
 */
typedef struct {
	uint16_t conn_handle;                                       /*!< Connection handle of the ACL connection.
                                                                 *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state ;    /*!< Output: receives the volume state of the remote device. */
} rtk_bt_le_audio_vcs_get_volume_state_param_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_get_volume_flags_param_t
 * @brief     Bluetooth LE Audio VCS get volume flags parameters.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
	uint8_t volume_flags;    /*!< Output: receives the volume flags of the remote device. */
} rtk_bt_le_audio_vcs_get_volume_flags_param_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_change_mute_param_t
 * @brief     Bluetooth LE Audio VCS change mute parameters.
 */
typedef struct {
	uint8_t group_idx;    /*!< Index of the audio group. */
	uint8_t mute;         /*!< Mute state to set, see @ref rtk_bt_le_audio_vcs_mute_state_t. */
} rtk_bt_le_audio_vcs_change_mute_param_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_change_volume_param_t
 * @brief     Bluetooth LE Audio VCS change volume parameters.
 */
typedef struct {
	uint8_t group_idx;         /*!< Index of the audio group. */
	uint8_t volume_setting;    /*!< Volume setting to apply.
                                *   Range: 0x00 to 0xFF. */
} rtk_bt_le_audio_vcs_change_volume_param_t;
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_mics_mute_state_t
 * @brief     Bluetooth LE Audio MICS mute state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_MICS_NOT_MUTE   = 0,    /*!< Microphone is not muted. */
	RTK_BT_LE_AUDIO_MICS_MUTED      = 1,    /*!< Microphone is muted. */
	RTK_BT_LE_AUDIO_MICS_DISABLED   = 2,    /*!< Microphone mute is disabled. */
	RTK_BT_LE_AUDIO_MICS_RFU,               /*!< Reserved for future use. */
} rtk_bt_le_audio_mics_mute_state_t;
/**
 * @typedef   rtk_bt_le_audio_mics_change_mute_param_t
 * @brief     Bluetooth LE Audio MICS change mute parameters.
 */
typedef struct {
	uint8_t group_idx;                             /*!< Index of the audio group. */
	rtk_bt_le_audio_mics_mute_state_t mic_mute;    /*!< Microphone mute state to set. */
} rtk_bt_le_audio_mics_change_mute_param_t;

/**
 * @typedef   rtk_bt_le_audio_mics_set_mute_value_param_t
 * @brief     Bluetooth LE Audio MICS set mute value parameters.
 */
typedef struct {
	uint16_t conn_handle;                          /*!< Connection handle of the ACL connection.
                                                    *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_mics_mute_state_t mic_mute;    /*!< Microphone mute state to set. */
} rtk_bt_le_audio_mics_set_mute_value_param_t;

/**
 * @typedef   rtk_bt_le_audio_mics_get_mute_value_param_t
 * @brief     Bluetooth LE Audio MICS get mute value parameters.
 */
typedef struct {
	uint16_t conn_handle;                             /*!< Connection handle of the ACL connection.
                                                       *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_mics_mute_state_t *p_mic_mute;    /*!< Output: receives the microphone mute state of the remote device. */
} rtk_bt_le_audio_mics_get_mute_value_param_t;
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vocs_feature_mask_t
 * @brief     Bluetooth LE Audio VOCS feature mask.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT = 0x01,      /*!< Writing the Audio Location without response is supported. */
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT = 0x02,    /*!< Writing the Audio Output Description without response is supported. */
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT = 0x04,                 /*!< Audio Location notification is supported. */
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT = 0x08                /*!< Audio Output Description notification is supported. */
} rtk_bt_le_audio_vocs_feature_mask_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_output_des_t
 * @brief     Bluetooth LE Audio VOCS audio output description.
 */
typedef struct {
	uint16_t    output_des_len;    /*!< Length of the audio output description in octets. */
	uint8_t    *p_output_des;      /*!< Buffer holding the audio output description. */
} rtk_bt_le_audio_vocs_output_des_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_param_t
 * @brief     Bluetooth LE Audio VOCS parameters.
 */
typedef struct {
	int16_t                             volume_offset;     /*!< Volume offset.
                                                            *   Range: -255 to 255. */
	uint8_t                             change_counter;    /*!< Initial value of the volume offset change counter.
                                                            *   Takes effect only when the parameters are set for the first time. */
	uint32_t                            audio_location;    /*!< Audio location, see @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t   output_des;        /*!< Audio output description. */
} rtk_bt_le_audio_vocs_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_offset_state_ind_t
 * @brief     Bluetooth LE Audio VOCS write offset state indication.
 */
typedef struct {
	uint16_t conn_handle;        /*!< Connection handle of the ACL connection.
                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t  srv_instance_id;    /*!< Index of the VOCS service instance. */
	int16_t  volume_offset;      /*!< Volume offset written by the remote client. */
} rtk_bt_le_audio_vocs_write_offset_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_audio_location_ind_t
 * @brief     Bluetooth LE Audio VOCS write audio location indication.
 */
typedef struct {
	uint16_t    conn_handle;        /*!< Connection handle of the ACL connection.
                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t     srv_instance_id;    /*!< Index of the VOCS service instance. */
	uint32_t    audio_location;     /*!< Audio location written by the remote client, see @ref rtk_bt_le_audio_location_t. */
} rtk_bt_le_audio_vocs_write_audio_location_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_output_des_ind_t
 * @brief     Bluetooth LE Audio VOCS write audio output description indication.
 */
typedef struct {
	uint16_t    conn_handle;                         /*!< Connection handle of the ACL connection.
                                                      *   Range: 0x0000 to 0x0EFF. */
	uint8_t     srv_instance_id;                     /*!< Index of the VOCS service instance. */
	rtk_bt_le_audio_vocs_output_des_t output_des;    /*!< Audio output description written by the remote client. */
} rtk_bt_le_audio_vocs_write_output_des_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_set_param_t
 * @brief     Bluetooth LE Audio VOCS set parameters.
 */
typedef struct {
	uint8_t                             srv_instance_id;    /*!< Index of the VOCS service instance. */
	uint8_t                             set_mask;           /*!< Mask of the parameters to apply, a combination of @ref rtk_bt_le_audio_vocs_cccd_flags_t values. */
	rtk_bt_le_audio_vocs_param_t        vocs_param;         /*!< VOCS parameters to set. */
} rtk_bt_le_audio_vocs_set_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_get_param_t
 * @brief     Bluetooth LE Audio VOCS get parameters.
 */
typedef struct {
	uint8_t                             srv_instance_id;    /*!< Index of the VOCS service instance. */
	rtk_bt_le_audio_vocs_param_t        *p_vocs_param;      /*!< Output: receives the VOCS parameters. */
} rtk_bt_le_audio_vocs_get_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_cfg_cccd_param_t
 * @brief     Bluetooth LE Audio VOCS configure CCCD parameters.
 */
typedef struct {
	uint16_t conn_handle;       /*!< Connection handle of the ACL connection.
                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;    /*!< Index of the VOCS service instance. */
	uint8_t cfg_flags;          /*!< CCCD flags selecting the characteristics to configure, a combination of @ref rtk_bt_le_audio_vocs_cccd_flags_t values. */
	bool enable;                /*!< Whether to enable the CCCD.
                                 *   - true: Enable the CCCD.
                                 *   - false: Disable the CCCD. */
} rtk_bt_le_audio_vocs_cfg_cccd_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_char_type_t
 * @brief     Bluetooth LE Audio VOCS characteristic type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_CHAR_OFFSET_STATE,         /*!< Volume Offset State characteristic. */
	RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_LOCATION,       /*!< Audio Location characteristic. */
	RTK_BT_LE_AUDIO_VOCS_CHAR_AUDIO_OUTPUT_DESC,    /*!< Audio Output Description characteristic. */
} rtk_bt_le_audio_vocs_char_type_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_read_char_value_param_t
 * @brief     Bluetooth LE Audio VOCS read characteristic value parameters.
 */
typedef struct {
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection.
                                               *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                  /*!< Index of the VOCS service instance. */
	rtk_bt_le_audio_vocs_char_type_t type;    /*!< Characteristic to read. */
} rtk_bt_le_audio_vocs_read_char_value_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_cp_param_t
 * @brief     Bluetooth LE Audio VOCS control point parameters.
 */
typedef struct {
	bool    counter_used;      /*!< Whether change_counter is provided with the operation.
                                *   - true: change_counter is provided.
                                *   - false: change_counter is not provided.
                                *   Currently not forwarded to the stack, only volume_offset is written. */
	uint8_t change_counter;    /*!< Change counter value of the control point operation.
                                *   Currently not forwarded to the stack, only volume_offset is written. */
	int16_t volume_offset;     /*!< Volume offset to set, used when the opcode is @ref RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET.
                                *   Range: -255 to 255. */
} rtk_bt_le_audio_vocs_cp_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_cp_op_t
 * @brief     Bluetooth LE Audio VOCS volume offset control point opcodes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_CP_SET_VOLUME_OFFSET = 0x01,    /*!< Set volume offset. */
	RTK_BT_LE_AUDIO_VOCS_CP_MAX                          /*!< Upper bound of the opcodes, not a valid opcode. */
} rtk_bt_le_audio_vocs_cp_op_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_cp_param_t
 * @brief     Bluetooth LE Audio VOCS write control point parameters.
 */
typedef struct {
	uint16_t conn_handle;                        /*!< Connection handle of the ACL connection.
                                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                     /*!< Index of the VOCS service instance. */
	rtk_bt_le_audio_vocs_cp_op_t cp_op;          /*!< Volume offset control point opcode. */
	rtk_bt_le_audio_vocs_cp_param_t cp_param;    /*!< Volume offset control point parameters. */
} rtk_bt_le_audio_vocs_write_cp_param_t;
/**
 * @typedef   rtk_bt_le_audio_vocs_write_cp_by_group_param_t
 * @brief     Bluetooth LE Audio Group-based VOCS control point write parameters.
 */
typedef struct {
	uint8_t group_idx;                           /*!< Index of the target device group. */
	uint8_t srv_instance_id;                     /*!< VOCS service instance index. */
	rtk_bt_le_audio_vocs_cp_op_t cp_op;          /*!< Volume Offset Control Point opcode. */
	rtk_bt_le_audio_vocs_cp_param_t cp_param;    /*!< Volume Offset Control Point parameter. */
} rtk_bt_le_audio_vocs_write_cp_by_group_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_audio_location_param_t
 * @brief     Bluetooth LE Audio VOCS audio location write parameters.
 */
typedef struct {
	uint16_t conn_handle;       /*!< Connection handle of the ACL connection.
                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;    /*!< VOCS service instance index. */
	uint32_t audio_location;    /*!< Audio location, see @ref rtk_bt_le_audio_location_t. */
} rtk_bt_le_audio_vocs_write_audio_location_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_write_output_des_param_t
 * @brief     Bluetooth LE Audio VOCS audio output description write parameters.
 */
typedef struct {
	uint16_t conn_handle;       /*!< Connection handle of the ACL connection.
                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;    /*!< VOCS service instance index. */
	uint16_t output_des_len;    /*!< Length of the output descriptor value, in octets. */
	uint8_t *p_output_des;      /*!< Output descriptor value to be written. */
} rtk_bt_le_audio_vocs_write_output_des_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_volume_offset_state_t
 * @brief     Bluetooth LE Audio VOCS Volume Offset State characteristic value.
 */
typedef struct {
	int16_t volume_offset;     /*!< Volume offset.
                                *   Range: -255 to 255. */
	uint8_t change_counter;    /*!< Change counter of the volume offset state. */
} rtk_bt_le_audio_vocs_volume_offset_state_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_srv_data_t
 * @brief     Bluetooth LE Audio VOCS service data.
 */
typedef struct {
	uint8_t                                             srv_instance_id;    /*!< VOCS service instance index. */
	uint8_t                                             type_exist;         /*!< Bit mask of the characteristics found on the remote service.
                                                                             *   See @ref rtk_bt_le_audio_vocs_cccd_flags_t. */
	rtk_bt_le_audio_vocs_volume_offset_state_t          volume_offset;      /*!< Volume offset state. */
	uint32_t                                            audio_location;     /*!< Audio location, see @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t                   output_des;         /*!< Audio output description. */
} rtk_bt_le_audio_vocs_srv_data_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_get_srv_data_param_t
 * @brief     Bluetooth LE Audio VOCS service data get parameters.
 */
typedef struct {
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                        /*!< VOCS service instance index. */
	rtk_bt_le_audio_vocs_srv_data_t *p_srv_data;    /*!< Buffer that receives the returned service data. */
} rtk_bt_le_audio_vocs_get_srv_data_param_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_cccd_flags_t
 * @brief     Bluetooth LE Audio VOCS characteristic flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_VOCS_VOLUME_OFFSET_STATE_FLAG = 0x01,    /*!< Volume Offset State characteristic flag. */
	RTK_BT_LE_AUDIO_VOCS_AUDIO_LOCATION_FLAG = 0x02,         /*!< Audio Location characteristic flag. */
	RTK_BT_LE_AUDIO_VOCS_AUDIO_OUTPUT_DES_FLAG = 0x04        /*!< Audio Output Description characteristic flag. */
} rtk_bt_le_audio_vocs_cccd_flags_t;
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_aics_param_type_t
 * @brief     Bluetooth LE Audio AICS parameter type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATE       = 0x01,    /*!< Audio Input State. The parameter data is @ref rtk_bt_le_audio_aics_input_state_t. */
	RTK_BT_LE_AUDIO_AICS_PARAM_GAIN_SETTING_PROP = 0x02,    /*!< Gain Setting Properties. The parameter data is @ref rtk_bt_le_audio_aics_gain_setting_prop_t. */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_TYPE        = 0x03,    /*!< Audio Input Type. The parameter data is 1 octet. */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_STATUS      = 0x04,    /*!< Audio Input Status. The parameter data is 1 octet. */
	RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES         = 0x05,    /*!< Audio Input Description. The parameter data is @ref rtk_bt_le_audio_aics_input_des_t. */
} rtk_bt_le_audio_aics_param_type_t;

/**
 * @typedef   rtk_bt_le_audio_input_type_t
 * @brief     Bluetooth LE Audio AICS audio input type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_INPUT_UNSPECIFIED         = 0x00,    /*!< Unspecified input. */
	RTK_BT_LE_AUDIO_INPUT_BLUETOOTH           = 0x01,    /*!< Bluetooth audio stream. */
	RTK_BT_LE_AUDIO_INPUT_MICROPHONE          = 0x02,    /*!< Microphone. */
	RTK_BT_LE_AUDIO_INPUT_ANALOG              = 0x03,    /*!< Analog interface. */
	RTK_BT_LE_AUDIO_INPUT_DIGITAL             = 0x04,    /*!< Digital interface. */
	RTK_BT_LE_AUDIO_INPUT_RADIO               = 0x05,    /*!< Radio, such as AM, FM or XM. */
	RTK_BT_LE_AUDIO_INPUT_STREAMING           = 0x06,    /*!< Streaming audio source. */
} rtk_bt_le_audio_input_type_t;

/**
 * @typedef   rtk_bt_le_audio_aics_char_type_t
 * @brief     Bluetooth LE Audio AICS characteristic type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATE,          /*!< Audio Input State. */
	RTK_BT_LE_AUDIO_AICS_CHAR_GAIN_SETTING_PROP,    /*!< Gain Setting Properties. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_TYPE,           /*!< Audio Input Type. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_STATUS,         /*!< Audio Input Status. */
	RTK_BT_LE_AUDIO_AICS_CHAR_INPUT_DES,            /*!< Audio Input Description. */
} rtk_bt_le_audio_aics_char_type_t;

/**
 * @typedef   rtk_bt_le_audio_aics_input_state_t
 * @brief     Bluetooth LE Audio AICS Audio Input State characteristic value.
 */
typedef struct {
	int8_t  gain_setting;      /*!< Gain setting value. */
	uint8_t mute;              /*!< Mute state of the audio input, see @ref rtk_bt_le_audio_aics_mute_state_t. */
	uint8_t gain_mode;         /*!< Gain mode of the audio input, see @ref rtk_bt_le_audio_aics_gain_mode_t. */
	uint8_t change_counter;    /*!< Change counter of the audio input state. */
} rtk_bt_le_audio_aics_input_state_t;

/**
 * @typedef   rtk_bt_le_audio_aics_set_param_t
 * @brief     Bluetooth LE Audio AICS parameter set parameters.
 */
typedef struct {
	uint8_t                             srv_instance_id;       /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_param_type_t   aics_param_type;       /*!< AICS parameter type to set. */
	uint8_t                             value_len;             /*!< Length of the data pointed to by p_value, in octets. */
	uint8_t                             *p_value;              /*!< Pointer to the AICS parameter data. */
	bool                                set_change_counter;    /*!< Whether to update the change counter along with the parameter.
                                                                *   - true: The change counter is updated.
                                                                *   - false: The change counter is left unchanged. */
} rtk_bt_le_audio_aics_set_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_get_param_t
 * @brief     Bluetooth LE Audio AICS parameter get parameters.
 */
typedef struct {
	uint8_t                             srv_instance_id;    /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_param_type_t   aics_param_type;    /*!< AICS parameter type to get.
                                                             *   @ref RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES is not supported. */
	uint8_t                             value_len;          /*!< Length of the buffer pointed to by p_value, in octets. */
	uint8_t                             *p_value;           /*!< Buffer that receives the AICS parameter data. */
} rtk_bt_le_audio_aics_get_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_cfg_cccd_param_t
 * @brief     Bluetooth LE Audio AICS CCCD configuration parameters.
 */
typedef struct {
	uint16_t conn_handle;       /*!< Connection handle of the ACL connection.
                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;    /*!< AICS service instance index. */
	uint8_t cfg_flags;          /*!< Bit mask of the AICS characteristics to configure, see @ref rtk_bt_le_audio_aics_cccd_flags_t. */
	bool enable;                /*!< Whether to enable the CCCD.
                                 *   - true: Enable the CCCD.
                                 *   - false: Disable the CCCD. */
} rtk_bt_le_audio_aics_cfg_cccd_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_read_char_value_param_t
 * @brief     Bluetooth LE Audio AICS characteristic read parameters.
 */
typedef struct {
	uint16_t conn_handle;                     /*!< Connection handle of the ACL connection.
                                               *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                  /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_char_type_t type;    /*!< AICS characteristic type to read. */
} rtk_bt_le_audio_aics_read_char_value_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_cp_op_t
 * @brief     Bluetooth LE Audio AICS Audio Input Control Point opcodes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING        = 0x01,    /*!< Set Gain Setting. */
	RTK_BT_LE_AUDIO_AICS_CP_UNMUTE                  = 0x02,    /*!< Unmute. */
	RTK_BT_LE_AUDIO_AICS_CP_MUTE                    = 0x03,    /*!< Mute. */
	RTK_BT_LE_AUDIO_AICS_CP_SET_MANUAL_GAIN_MODE    = 0x04,    /*!< Set Manual Gain Mode. */
	RTK_BT_LE_AUDIO_AICS_CP_SET_AUTOMATIC_GAIN_MODE = 0x05,    /*!< Set Automatic Gain Mode. */
	RTK_BT_LE_AUDIO_AICS_CP_MAX                                /*!< Upper bound of the opcodes, not a valid opcode. */
} rtk_bt_le_audio_aics_cp_op_t;

/**
 * @typedef   rtk_bt_le_audio_aics_cp_param_t
 * @brief     Bluetooth LE Audio AICS Audio Input Control Point parameter.
 */
typedef struct {
	int8_t  gaining_setting;    /*!< Gain setting value. Used only with @ref RTK_BT_LE_AUDIO_AICS_CP_SET_GAIN_SETTING. */
}  rtk_bt_le_audio_aics_cp_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_write_cp_param_t
 * @brief     Bluetooth LE Audio AICS control point write parameters.
 */
typedef struct {
	uint16_t conn_handle;                        /*!< Connection handle of the ACL connection.
                                                  *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                     /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_cp_op_t cp_op;          /*!< AICS control point opcode. */
	rtk_bt_le_audio_aics_cp_param_t cp_param;    /*!< AICS control point parameter. */
} rtk_bt_le_audio_aics_write_cp_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_write_cp_by_group_param_t
 * @brief     Bluetooth LE Audio Group-based AICS control point write parameters.
 */
typedef struct {
	uint8_t group_idx;                           /*!< Index of the target device group. */
	uint8_t srv_instance_id;                     /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_cp_op_t cp_op;          /*!< AICS control point opcode. */
	rtk_bt_le_audio_aics_cp_param_t cp_param;    /*!< AICS control point parameter. */
} rtk_bt_le_audio_aics_write_cp_by_group_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_write_input_des_param_t
 * @brief     Bluetooth LE Audio AICS audio input description write parameters.
 */
typedef struct {
	uint16_t conn_handle;       /*!< Connection handle of the ACL connection.
                                 *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;    /*!< AICS service instance index. */
	uint16_t input_des_len;     /*!< Length of the input descriptor value, in octets. */
	uint8_t *p_input_des;       /*!< Input descriptor value to be written. */
} rtk_bt_le_audio_aics_write_input_des_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_gain_setting_prop_t
 * @brief     Bluetooth LE Audio AICS Gain Setting Properties characteristic value.
 */
typedef struct {
	uint8_t gain_setting_units;    /*!< Gain setting units. */
	int8_t  gain_setting_min;      /*!< Minimum gain setting value. */
	int8_t  gain_setting_max;      /*!< Maximum gain setting value. */
} rtk_bt_le_audio_aics_gain_setting_prop_t;

/**
 * @typedef   rtk_bt_le_audio_aics_input_des_t
 * @brief     Bluetooth LE Audio AICS Audio Input Description characteristic value.
 */
typedef struct {
	uint16_t input_des_len;    /*!< Length of the audio input description, in octets. */
	uint8_t  *p_input_des;     /*!< Audio input description data. */
} rtk_bt_le_audio_aics_input_des_t;

/**
 * @typedef   rtk_bt_le_audio_aics_srv_data_t
 * @brief     Bluetooth LE Audio AICS service data.
 */
typedef struct {
	uint8_t                                             srv_instance_id;    /*!< AICS service instance index. */
	uint8_t                                             type_exist;         /*!< Bit mask of the characteristics found on the remote service.
                                                                             *   See @ref rtk_bt_le_audio_aics_cccd_flags_t. */
	rtk_bt_le_audio_aics_input_state_t                  input_state;        /*!< Audio Input State. */
	rtk_bt_le_audio_aics_gain_setting_prop_t            setting_prop;       /*!< Gain Setting Properties. */
	uint8_t                                             input_type;         /*!< Audio input type, see @ref rtk_bt_le_audio_input_type_t. */
	uint8_t                                             input_status;       /*!< Audio Input Status. */
	rtk_bt_le_audio_aics_input_des_t                    input_des;          /*!< Audio Input Description. */
} rtk_bt_le_audio_aics_srv_data_t;

/**
 * @typedef   rtk_bt_le_audio_aics_cp_ind_t
 * @brief     Bluetooth LE Audio AICS control point write indication.
 */
typedef struct {
	uint16_t                        conn_handle;        /*!< Connection handle of the ACL connection.
                                                         *   Range: 0x0000 to 0x0EFF. */
	uint8_t                         srv_instance_id;    /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_cp_op_t    cp_op;              /*!< Control point opcode written by the remote device. */
	int8_t                          gain_setting;       /*!< Gain setting value carried by the control point operation. */
} rtk_bt_le_audio_aics_cp_ind_t;

/**
 * @typedef   rtk_bt_le_audio_aics_write_input_des_ind_t
 * @brief     Bluetooth LE Audio AICS audio input description write indication.
 */
typedef struct {
	uint16_t         conn_handle;                  /*!< Connection handle of the ACL connection.
                                                    *   Range: 0x0000 to 0x0EFF. */
	uint8_t          srv_instance_id;              /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_input_des_t input_des;    /*!< Audio input description written by the remote device. */
} rtk_bt_le_audio_aics_write_input_des_ind_t;

/**
 * @typedef   rtk_bt_le_audio_aics_get_srv_data_param_t
 * @brief     Bluetooth LE Audio AICS service data get parameters.
 */
typedef struct {
	uint16_t conn_handle;                           /*!< Connection handle of the ACL connection.
                                                     *   Range: 0x0000 to 0x0EFF. */
	uint8_t srv_instance_id;                        /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_srv_data_t *p_srv_data;    /*!< Buffer that receives the returned service data. */
} rtk_bt_le_audio_aics_get_srv_data_param_t;

/**
 * @typedef   rtk_bt_le_audio_aics_cccd_flags_t
 * @brief     Bluetooth LE Audio AICS characteristic flags.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_INPUT_STATE_FLAG       = 0x01,    /*!< Audio Input State Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_STATUS_FLAG      = 0x02,    /*!< Audio Input Status Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_DES_FLAG         = 0x04,    /*!< Audio Input Description Flag for read and CCCD configuration. */
	RTK_BT_LE_AUDIO_AICS_INPUT_TYPE_FLAG        = 0x08,    /*!< Audio Input Type Flag for read. */
	RTK_BT_LE_AUDIO_AICS_GAIN_SETTING_PROP_FLAG = 0x10,    /*!< Gain Setting Properties Flag for read. */
} rtk_bt_le_audio_aics_cccd_flags_t;

/**
 * @typedef   rtk_bt_le_audio_aics_gain_mode_t
 * @brief     Bluetooth LE Audio AICS gain modes.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL_ONLY       = 0,    /*!< Manual gain mode only. */
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC_ONLY    = 1,    /*!< Automatic gain mode only. */
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_MANUAL            = 2,    /*!< Manual gain mode. */
	RTK_BT_LE_AUDIO_AICS_GAIN_MODE_AUTOMATIC         = 3     /*!< Automatic gain mode. */
} rtk_bt_le_audio_aics_gain_mode_t;

/**
 * @typedef   rtk_bt_le_audio_aics_mute_state_t
 * @brief     Bluetooth LE Audio AICS audio mute state.
 */
typedef enum {
	RTK_BT_LE_AUDIO_AICS_NOT_MUTED                  = 0,    /*!< Audio input is not muted. */
	RTK_BT_LE_AUDIO_AICS_MUTED                      = 1,    /*!< Audio input is muted. */
	RTK_BT_LE_AUDIO_AICS_MUTE_DISABLED              = 2     /*!< Mute is disabled. */
} rtk_bt_le_audio_aics_mute_state_t;
#endif
/* event struct */
/**
 * @typedef   rtk_bt_le_audio_cap_discovery_done_ind_t
 * @brief     Bluetooth LE Audio CAP discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;      /*!< Connection handle of the ACL connection.
                                *   Range: 0x0000 to 0x0EFF. */
	bool     load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                                *   - true: The service table is restored from FTL.
                                *   - false: The service table is obtained by discovery on the remote device. */
	bool     cas_is_found;     /*!< Whether the Common Audio Service (CAS) is found.
                                *   - true: CAS is found.
                                *   - false: CAS is not found. */
	bool     cas_inc_csis;     /*!< Whether the CAS instance includes a CSIS instance.
                                *   - true: CAS includes a CSIS instance.
                                *   - false: CAS does not include a CSIS instance. */
	bool     vcs_is_found;     /*!< Whether the Volume Control Service (VCS) is found.
                                *   - true: VCS is found.
                                *   - false: VCS is not found. */
	bool     mics_is_found;    /*!< Whether the Microphone Control Service (MICS) is found.
                                *   - true: MICS is found.
                                *   - false: MICS is not found. */
} rtk_bt_le_audio_cap_discovery_done_ind_t;

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT

/**
 * @typedef   rtk_bt_le_audio_csis_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio CSIS client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;     /*!< Connection handle of the ACL connection.
                               *   Range: 0x0000 to 0x0EFF. */
	bool    is_found;         /*!< Whether the CSIS is found.
                               *   - true: CSIS is found.
                               *   - false: CSIS is not found. */
	bool    load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                               *   - true: The service table is restored from FTL.
                               *   - false: The service table is obtained by discovery on the remote device. */
	uint8_t srv_num;          /*!< Number of CSIS instances found. */
	uint8_t group_idx;        /*!< Index of the device group the remote device belongs to. */
} rtk_bt_le_audio_csis_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_csis_client_read_result_ind_t
 * @brief     Bluetooth LE Audio CSIS client read result indication.
 */
typedef struct {
	uint16_t                            cause;           /*!< Result code of the read operation. 0 indicates success. */
	uint16_t                            conn_handle;     /*!< Connection handle of the ACL connection.
                                                          *   Range: 0x0000 to 0x0EFF. */
	rtk_bt_le_audio_group_handle_t      group_handle;    /*!< Handle of the device group. */
	rtk_bt_le_audio_device_handle_t     dev_handle;      /*!< Handle of the remote device. */
	rtk_bt_le_audio_csis_set_mem_info_t mem_info;        /*!< Information of the coordinated set member. */
	uint8_t                             group_idx;       /*!< Index of the device group. */
} rtk_bt_le_audio_csis_client_read_result_ind_t;

/**
 * @typedef   rtk_bt_le_audio_csis_client_search_done_ind_t
 * @brief     Bluetooth LE Audio CSIS client search done or timeout indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t      group_handle;    /*!< Handle of the device group. */
	uint8_t         set_mem_size;                        /*!< Size of the coordinated set. */
	bool            search_done;                         /*!< Whether the search for set members is done.
                                                          *   - true: The search is done.
                                                          *   - false: The search is not done. */
	bool            search_timeout;                      /*!< Whether the search for set members ends by timeout.
                                                          *   - true: The search ended because of timeout.
                                                          *   - false: The search did not time out. */
} rtk_bt_le_audio_csis_client_search_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_csis_client_set_mem_found_ind_t
 * @brief     Bluetooth LE Audio CSIS client set member found indication.
 */
typedef struct {
	rtk_bt_le_audio_group_handle_t      group_handle;                     /*!< Handle of the device group. */
	rtk_bt_le_audio_device_handle_t     dev_handle;                       /*!< Handle of the found set member. */
	rtk_bt_le_addr_t                    bd_addr;                          /*!< Address of the found set member. */
	uint16_t                            srv_uuid;                         /*!< Service UUID of the found set member. */
	uint8_t                             rank;                             /*!< Rank of the set member within the coordinated set. */
	uint8_t                             set_mem_size;                     /*!< Size of the coordinated set. */
	uint8_t                             sirk[RTK_BT_LE_CSIS_SIRK_LEN];    /*!< Set Identity Resolving Key (SIRK) of the coordinated set.
                                                                           *   The length is @ref RTK_BT_LE_CSIS_SIRK_LEN. */
} rtk_bt_le_audio_csis_client_set_mem_found_ind_t;
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vcs_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio VCS client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;      /*!< Connection handle of the ACL connection.
                                *   Range: 0x0000 to 0x0EFF. */
	bool     is_found;         /*!< Whether the VCS is found.
                                *   - true: VCS is found.
                                *   - false: VCS is not found. */
	bool     load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                                *   - true: The service table is restored from FTL.
                                *   - false: The service table is obtained by discovery on the remote device. */
	uint8_t  type_exist;       /*!< Bit mask of the VCS characteristics found.
                                *   0x01: Volume State exists.
                                *   0x02: Volume Flags exists. */
} rtk_bt_le_audio_vcs_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_client_volume_state_ind_t
 * @brief     Bluetooth LE Audio VCS client volume state indication.
 */
typedef struct {
	uint16_t        conn_handle;                        /*!< Connection handle of the ACL connection.
                                                         *   Range: 0x0000 to 0x0EFF. */
	bool            is_notify;                          /*!< Whether the volume state is received by notification.
                                                         *   - true: Received by notification.
                                                         *   - false: Received by a read operation. */
	rtk_bt_le_audio_vcs_volume_state_t volume_state;    /*!< Volume state. */
} rtk_bt_le_audio_vcs_client_volume_state_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vcs_client_volume_flag_ind_t
 * @brief     Bluetooth LE Audio VCS client volume flags indication.
 */
typedef struct {
	uint16_t       conn_handle;     /*!< Connection handle of the ACL connection.
                                     *   Range: 0x0000 to 0x0EFF. */
	bool           is_notify;       /*!< Whether the volume flags are received by notification.
                                     *   - true: Received by notification.
                                     *   - false: Received by a read operation. */
	uint8_t        volume_flags;    /*!< Volume flags value. */
} rtk_bt_le_audio_vcs_client_volume_flag_ind_t;
#endif
#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_mics_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio MICS client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;      /*!< Connection handle of the ACL connection.
                                *   Range: 0x0000 to 0x0EFF. */
	bool     is_found;         /*!< Whether the MICS is found.
                                *   - true: MICS is found.
                                *   - false: MICS is not found. */
	bool     load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                                *   - true: The service table is restored from FTL.
                                *   - false: The service table is obtained by discovery on the remote device. */
	uint8_t  mic_mute;         /*!< Microphone mute state, see @ref rtk_bt_le_audio_mics_mute_state_t. */
} rtk_bt_le_audio_mics_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_mics_client_notify_ind_t
 * @brief     Bluetooth LE Audio MICS client notification indication.
 */
typedef struct {
	uint16_t conn_handle;    /*!< Connection handle of the ACL connection.
                              *   Range: 0x0000 to 0x0EFF. */
	uint8_t  mic_mute;       /*!< Microphone mute state, see @ref rtk_bt_le_audio_mics_mute_state_t. */
} rtk_bt_le_audio_mics_client_notify_ind_t;

/**
 * @typedef   rtk_bt_le_audio_mics_write_mute_ind_t
 * @brief     Bluetooth LE Audio MICS mute state write indication.
 */
typedef struct {
	uint8_t mic_mute;    /*!< Microphone mute state written by the remote device, see @ref rtk_bt_le_audio_mics_mute_state_t. */
} rtk_bt_le_audio_mics_write_mute_ind_t;
/**
 * @typedef   rtk_bt_le_audio_mics_param_t
 * @brief     Bluetooth LE Audio MICS parameters.
 */
typedef struct {
	uint8_t mic_mute;    /*!< Microphone mute state, see @ref rtk_bt_le_audio_mics_mute_state_t. */
} rtk_bt_le_audio_mics_param_t;

#endif
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @typedef   rtk_bt_le_audio_vocs_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio VOCS client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;     /*!< Connection handle of the ACL connection.
                               *   Range: 0x0000 to 0x0EFF. */
	bool    is_found;         /*!< Whether the VOCS is found.
                               *   - true: VOCS is found.
                               *   - false: VOCS is not found. */
	bool    load_from_ftl;    /*!< Whether the service table is loaded from FTL.
                               *   - true: The service table is restored from FTL.
                               *   - false: The service table is obtained by discovery on the remote device. */
	uint8_t srv_num;          /*!< Number of VOCS instances found. */
} rtk_bt_le_audio_vocs_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_char_data_t
 * @brief     Bluetooth LE Audio VOCS characteristic data.
 */
typedef union {
	rtk_bt_le_audio_vocs_volume_offset_state_t  volume_offset;     /*!< Volume Offset State characteristic value. */
	uint32_t                                    audio_location;    /*!< Audio location, see @ref rtk_bt_le_audio_location_t. */
	rtk_bt_le_audio_vocs_output_des_t           output_des;        /*!< Audio Output Description characteristic value. */
} rtk_bt_le_audio_vocs_char_data_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_client_read_result_ind_t
 * @brief     Bluetooth LE Audio VOCS client read result indication.
 */
typedef struct {
	uint16_t                            conn_handle;        /*!< Connection handle of the ACL connection.
                                                             *   Range: 0x0000 to 0x0EFF. */
	uint8_t                             srv_instance_id;    /*!< VOCS service instance index. */
	rtk_bt_le_audio_vocs_char_type_t    type;               /*!< VOCS characteristic that was read. */
	rtk_bt_le_audio_vocs_char_data_t    data;               /*!< Value of the characteristic that was read. */
	uint16_t                            cause;              /*!< Result code of the read operation. 0 indicates success. */
} rtk_bt_le_audio_vocs_client_read_result_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_client_notify_ind_t
 * @brief     Bluetooth LE Audio VOCS client notification indication.
 */
typedef struct {
	uint16_t                            conn_handle;        /*!< Connection handle of the ACL connection.
                                                             *   Range: 0x0000 to 0x0EFF. */
	uint8_t                             srv_instance_id;    /*!< VOCS service instance index. */
	rtk_bt_le_audio_vocs_char_type_t    type;               /*!< VOCS characteristic that was notified. */
	rtk_bt_le_audio_vocs_char_data_t    data;               /*!< Value of the characteristic that was notified. */
} rtk_bt_le_audio_vocs_client_notify_ind_t;

/**
 * @typedef   rtk_bt_le_audio_vocs_client_cp_result_ind_t
 * @brief     Bluetooth LE Audio VOCS client control point write result indication.
 */
typedef struct {
	uint16_t                        conn_handle;        /*!< Connection handle of the ACL connection.
                                                         *   Range: 0x0000 to 0x0EFF. */
	uint8_t                         srv_instance_id;    /*!< VOCS service instance index. */
	uint16_t                        cause;              /*!< Result code of the control point write operation. 0 indicates success. */
	rtk_bt_le_audio_vocs_cp_op_t    cp_op;              /*!< Control point opcode that was written. */
} rtk_bt_le_audio_vocs_client_cp_result_ind_t;
#endif
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
#define RTK_BT_LE_ATTR_INSTANCE_NUM_MAX               20    /*!< Maximum number of attribute instances. */

/**
 * @typedef   rtk_bt_le_attr_instance_t
 * @brief     Bluetooth LE Audio List of included service instance indexes.
 */
typedef struct {
	uint8_t instance_num;                                    /*!< Number of valid entries in instance_id. */
	uint8_t instance_id[RTK_BT_LE_ATTR_INSTANCE_NUM_MAX];    /*!< Service instance indexes. */
} rtk_bt_le_attr_instance_t;

/**
 * @typedef   rtk_bt_le_audio_aics_client_discovery_done_ind_t
 * @brief     Bluetooth LE Audio AICS client discovery done indication.
 */
typedef struct {
	uint16_t conn_handle;                       /*!< Connection handle of the ACL connection.
                                                 *   Range: 0x0000 to 0x0EFF. */
	bool is_found;                              /*!< Whether the AICS is found.
                                                 *   - true: AICS is found.
                                                 *   - false: AICS is not found. */
	bool load_from_ftl;                         /*!< Whether the service table is loaded from FTL.
                                                 *   - true: The service table is restored from FTL.
                                                 *   - false: The service table is obtained by discovery on the remote device. */
	uint8_t srv_num;                            /*!< Number of AICS instances found. */
	rtk_bt_le_attr_instance_t vcs_instance;     /*!< AICS instances included by the VCS. */
	rtk_bt_le_attr_instance_t mics_instance;    /*!< AICS instances included by the MICS. */
} rtk_bt_le_audio_aics_client_discovery_done_ind_t;

/**
 * @typedef   rtk_bt_le_audio_aics_char_data_t
 * @brief     Bluetooth LE Audio AICS characteristic data.
 */
typedef union {
	rtk_bt_le_audio_aics_input_state_t          input_state;    /*!< Audio Input State characteristic value. */
	rtk_bt_le_audio_aics_gain_setting_prop_t    setting_prop;   /*!< Gain Setting Properties characteristic value. */
	uint8_t                                     input_type;     /*!< Audio Input Type characteristic value. */
	uint8_t                                     input_status;   /*!< Audio Input Status characteristic value. */
	rtk_bt_le_audio_aics_input_des_t            input_des;      /*!< Audio Input Description characteristic value. */
} rtk_bt_le_audio_aics_char_data_t;

/**
 * @typedef   rtk_bt_le_audio_aics_client_notify_ind_t
 * @brief     Bluetooth LE Audio AICS client notification indication.
 */
typedef struct {
	uint16_t                            conn_handle;        /*!< Connection handle of the ACL connection.
                                                             *   Range: 0x0000 to 0x0EFF. */
	uint8_t                             srv_instance_id;    /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_char_type_t    type;               /*!< AICS characteristic that was notified. */
	rtk_bt_le_audio_aics_char_data_t    data;               /*!< Value of the characteristic that was notified. */
} rtk_bt_le_audio_aics_client_notify_ind_t;

/**
 * @typedef   rtk_bt_le_audio_aics_client_read_result_ind_t
 * @brief     Bluetooth LE Audio AICS client read result indication.
 */
typedef struct {
	uint16_t                         conn_handle;        /*!< Connection handle of the ACL connection.
                                                          *   Range: 0x0000 to 0x0EFF. */
	uint8_t                          srv_instance_id;    /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_char_type_t type;               /*!< AICS characteristic that was read. */
	rtk_bt_le_audio_aics_char_data_t data;               /*!< Value of the characteristic that was read. */
	uint16_t                         cause;              /*!< Result code of the read operation. 0 indicates success. */
} rtk_bt_le_audio_aics_client_read_result_ind_t;

/**
 * @typedef   rtk_bt_le_audio_aics_client_cp_result_ind_t
 * @brief     Bluetooth LE Audio AICS client control point write result indication.
 */
typedef struct {
	uint16_t                        conn_handle;        /*!< Connection handle of the ACL connection.
                                                         *   Range: 0x0000 to 0x0EFF. */
	uint8_t                         srv_instance_id;    /*!< AICS service instance index. */
	rtk_bt_le_audio_aics_cp_op_t    cp_op;              /*!< Control point opcode that was written. */
	uint16_t                        cause;              /*!< Result code of the control point write operation. 0 indicates success. */
} rtk_bt_le_audio_aics_client_cp_result_ind_t;
#endif

/**
 * @typedef   rtk_bt_le_audio_cap_param_config_type_t
 * @brief     Bluetooth LE Audio CAP configuration type.
 */
typedef enum {
	RTK_BT_LE_AUDIO_CAP_CSIS_SIRK_CONFIG = 0x01,    /*!< Configure the default CSIS SIRK. */
} rtk_bt_le_audio_cap_param_config_type_t;

/**
 * @typedef   rtk_bt_le_audio_cap_param_config_t
 * @brief     Bluetooth LE Audio CAP configuration parameters.
 */
typedef union {
	rtk_bt_le_audio_cap_param_config_type_t cfg_type;    /*!< Type of the configuration to apply. */
	union {
		uint8_t csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN];      /*!< Default CSIS SIRK, valid when the type is @ref RTK_BT_LE_AUDIO_CAP_CSIS_SIRK_CONFIG. */
	} cfg;                                               /*!< Configuration data selected by the type. */
} rtk_bt_le_audio_cap_param_config_t;
/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_cap BT CAP APIs
 * @brief     BT CAP related function APIs.
 * @ingroup   bt_le_audio
 * @{
 */

#if defined(RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
/**
 * @brief     Generate a Resolvable Set Identifier (RSI) from a Set Identity Resolving Key (SIRK).
 * @param[in] p_sirk: Set Identity Resolving Key.
 *                    Must not be NULL.
 * @param[out] p_rsi: Buffer that receives the generated RSI.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_csis_gen_rsi(uint8_t *p_sirk, uint8_t *p_rsi);

/**
 * @brief     Update the Set Identity Resolving Key (SIRK) of the local Coordinated Set Identification Service (CSIS).
 * @param[in] sirk_type: Type of the new SIRK.
 * @param[in] p_sirk: New Set Identity Resolving Key.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - If the SIRK and its type are unchanged, this API does nothing and returns success.
 */
uint16_t rtk_bt_le_audio_csis_update_sirk(rtk_bt_le_audio_csis_sirk_type_t sirk_type, uint8_t *p_sirk);
/**
 * @brief     Update the Coordinated Set Size of the local Coordinated Set Identification Service (CSIS).
 * @param[in] csis_size: Number of devices in the coordinated set.
 *                       Range: 0x01 to 0xFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - If the size is unchanged, this API does nothing and returns success.
 */
uint16_t rtk_bt_le_audio_csis_update_size(uint8_t csis_size);
#endif

#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
/**
 * @brief     Check whether an advertising report carries a Resolvable Set Identifier (RSI) that matches a coordinated set known to the local set coordinator.
 * @param[in] adv_addr: Address of the advertiser.
 * @param[in] report_data_len: Length of the advertising report data in octets.
 * @param[in] p_report_data: Advertising report data.
 *                           Can be NULL only when report_data_len is 0.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_ERR_LOWER_STACK_API if the advertiser does not belong to a known coordinated set.
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_check_adv_rsi(rtk_bt_le_addr_t adv_addr, uint8_t report_data_len, uint8_t *p_report_data);
/**
 * @brief     Start or stop discovering the remaining members of a coordinated set. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SEARCH_DONE_IND.
 * @param[in] group_handle: Handle of the coordinated set group.
 *                          Must not be NULL.
 * @param[in] discover: Whether to discover set members.
 *                      - true: Start discovering set members.
 *                      - false: Stop discovering set members.
 * @param[in] timeout_ms: Discovery timeout in milliseconds.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Each set member found is reported via event @ref RTK_BT_LE_AUDIO_EVT_CSIS_CLIENT_SET_MEM_FOUND_IND.
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_cfg_discover(rtk_bt_le_audio_group_handle_t group_handle, bool discover, uint32_t timeout_ms);
/**
 * @brief     Create a coordinated set group and add the first set member to it.
 * @param[in] p_mem_info: Set member information of the device to add.
 *                        Must not be NULL.
 * @param[out] p_group_handle: Receives the handle of the created group.
 * @param[out] p_device_handle: Receives the handle of the added device.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Connection changes of the group devices are reported via event @ref RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND.
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_add_group(rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info, rtk_bt_le_audio_group_handle_t *p_group_handle,
														rtk_bt_le_audio_device_handle_t *p_device_handle);
/**
 * @brief     Add a set member to an existing coordinated set group.
 * @param[in] group_handle: Handle of the coordinated set group.
 *                          Must not be NULL.
 * @param[in] p_mem_info: Set member information of the device to add.
 *                        Must not be NULL.
 * @param[out] p_device_handle: Receives the handle of the added device.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Connection changes of the group devices are reported via event @ref RTK_BT_LE_AUDIO_EVT_GROUP_DEV_MSG_IND.
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_add_device(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_csis_set_mem_info_t *p_mem_info,
														 rtk_bt_le_audio_device_handle_t *p_device_handle);
/**
 * @brief     Lock or unlock the members of a coordinated set group.
 * @param[in] group_handle: Handle of the coordinated set group.
 *                          Must not be NULL.
 * @param[in] lock: Whether to lock the set members.
 *                  - true: Lock the set members.
 *                  - false: Unlock the set members.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_csis_set_coordinator_write_lock(rtk_bt_le_audio_group_handle_t group_handle, bool lock);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
/**
 * @brief     Set the parameters of the local Volume Control Service (VCS).
 * @param[in] param: VCS parameters to set.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vcs_set_param(rtk_bt_le_audio_vcs_param_t *param);

/**
 * @brief     Get the parameters of the local Volume Control Service (VCS).
 * @param[out] param: Receives the VCS parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vcs_get_param(rtk_bt_le_audio_vcs_param_t *param);
#endif

#if defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT
/**
 * @brief     Write the Volume Control Point of a remote Volume Control Service (VCS).
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] cp_op: Control point opcode.
 * @param[in] cp_param: Control point operation parameter.
 *                      Only used when the opcode is @ref RTK_BT_LE_AUDIO_VCS_CP_SET_ABSOLUTE_VOLUME.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vcs_write_cp(uint16_t conn_handle, rtk_bt_le_audio_vcs_cp_op_t cp_op, rtk_bt_le_audio_vcs_cp_param_t *cp_param);

/**
 * @brief     Get the volume state of a remote Volume Control Service (VCS).
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_vcs_volume_state: Receives the volume state.
 *                                 Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vcs_get_volume_state(uint16_t conn_handle, rtk_bt_le_audio_vcs_volume_state_t *p_vcs_volume_state);

/**
 * @brief     Get the volume flags of a remote Volume Control Service (VCS).
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_volume_flags: Receives the volume flags.
 *                             Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vcs_get_volume_flags(uint16_t conn_handle, uint8_t *p_volume_flags);

/**
 * @brief     Change the mute state of the Volume Control Service (VCS) on every member of a group.
 * @param[in] group_idx: Index of the target group.
 * @param[in] mute: Mute state to apply, see @ref rtk_bt_le_audio_vcs_mute_state_t.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_FAIL if the group is not found.
 */
uint16_t rtk_bt_le_audio_vcs_change_mute(uint8_t group_idx, uint8_t mute);

/**
 * @brief     Change the volume setting of the Volume Control Service (VCS) on every member of a group.
 * @param[in] group_idx: Index of the target group.
 * @param[in] volume_setting: Absolute volume setting to apply.
 *                            Range: 0x00 to 0xFF.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_FAIL if the group is not found.
 */
uint16_t rtk_bt_le_audio_vcs_change_volume(uint8_t group_idx, uint8_t volume_setting);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT
/**
 * @brief     Change the microphone mute state of the Microphone Control Service (MICS) on every member of a group.
 * @param[in] group_idx: Index of the target group.
 * @param[in] mic_mute: Microphone mute state to apply.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_FAIL if the group is not found.
 */
uint16_t rtk_bt_le_audio_mics_change_mute(uint8_t group_idx, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     Write the mute value of a remote Microphone Control Service (MICS).
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] mic_mute: Microphone mute state to write.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mics_set_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t mic_mute);

/**
 * @brief     Get the mute value of a remote Microphone Control Service (MICS).
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[out] p_mic_mute: Receives the microphone mute state.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mics_get_mute_value(uint16_t conn_handle, rtk_bt_le_audio_mics_mute_state_t *p_mic_mute);
#endif

#if defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT
/**
 * @brief     Set the parameters of the local Microphone Control Service (MICS).
 * @param[in] param: MICS parameters to set.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mics_set_param(rtk_bt_le_audio_mics_param_t *param);

/**
 * @brief     Get the parameters of the local Microphone Control Service (MICS).
 * @param[out] param: Receives the MICS parameters.
 *                    Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mics_get_param(rtk_bt_le_audio_mics_param_t *param);
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
/**
 * @brief     Set the parameters of a local Volume Offset Control Service (VOCS) instance.
 * @param[in] srv_instance_id: VOCS service instance index.
 *                             Used when the number of services is greater than 1.
 * @param[in] set_mask: Bit mask of the parameters to set, see @ref rtk_bt_le_audio_vocs_cccd_flags_t.
 * @param[in] p_vocs_param: VOCS parameters to set.
 *                          Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_set_param(uint8_t srv_instance_id, uint8_t set_mask, rtk_bt_le_audio_vocs_param_t *p_vocs_param);

/**
 * @brief     Get the parameters of a local Volume Offset Control Service (VOCS) instance.
 * @param[in] srv_instance_id: VOCS service instance index.
 *                             Used when the number of services is greater than 1.
 * @param[out] p_vocs_param: Receives the VOCS parameters.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - If the output description buffer pointer in the parameter is NULL, only the output description length is returned.
 */
uint16_t rtk_bt_le_audio_vocs_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_vocs_param_t *p_vocs_param);

/**
 * @brief     Enable or disable notifications of Volume Offset Control Service (VOCS) characteristics.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] cfg_flags: Bit mask of the VOCS characteristics to configure, see @ref rtk_bt_le_audio_vocs_cccd_flags_t.
 * @param[in] enable: Whether to enable the notifications.
 *                    - true: Enable the notifications.
 *                    - false: Disable the notifications.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable);

/**
 * @brief     Read a characteristic value of a remote Volume Offset Control Service (VOCS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_READ_RESULT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] type: VOCS characteristic to read.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_char_type_t type);

/**
 * @brief     Write the Volume Offset Control Point of a remote Volume Offset Control Service (VOCS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] cp_op: Control point opcode.
 * @param[in] cp_param: Control point operation parameter.
 *                      Only volume_offset takes effect.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
									   rtk_bt_le_audio_vocs_cp_param_t cp_param);

/**
 * @brief     Write the Volume Offset Control Point of a Volume Offset Control Service (VOCS) instance on every member of a group. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_VOCS_CLIENT_CP_RESULT_IND.
 * @param[in] group_idx: Index of the target group.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] cp_op: Control point opcode.
 * @param[in] cp_param: Control point operation parameter.
 *                      Only volume_offset takes effect.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_FAIL if the group is not found.
 */
uint16_t rtk_bt_le_audio_vocs_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_cp_op_t cp_op,
												rtk_bt_le_audio_vocs_cp_param_t cp_param);

/**
 * @brief     Write the audio location of a remote Volume Offset Control Service (VOCS) instance.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] audio_location: Audio location bit mask, see @ref rtk_bt_le_audio_location_t.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id, uint32_t audio_location);
/**
 * @brief     Write the output description of a Volume Offset Control Service (VOCS) instance.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[in] output_des_len: Length of the output description in octets.
 * @param[in] p_output_des: Pointer to the output description data to write.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t output_des_len, uint8_t *p_output_des);
/**
 * @brief     Get the data of a Volume Offset Control Service (VOCS) instance.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: VOCS service instance index.
 * @param[out] p_srv_data: Buffer that receives the VOCS service data.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_vocs_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_vocs_srv_data_t *p_srv_data);
#endif

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
/**
 * @brief     Set a parameter of the local Audio Input Control Service (AICS) instance.
 * @param[in] srv_instance_id: AICS service instance index.
 *                             Used when the number of services is greater than 1.
 * @param[in] aics_param_type: Type of the AICS parameter to set.
 * @param[in] value_len: Length of the data pointed to by p_value in octets.
 * @param[in] p_value: Pointer to the AICS parameter data.
 *                     Must not be NULL.
 * @param[in] set_change_counter: Whether to update the change counter together with the parameter.
 *                                - true: Update the change counter.
 *                                - false: Leave the change counter unchanged.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_set_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value,
										bool set_change_counter);

/**
 * @brief     Get a parameter of the local Audio Input Control Service (AICS) instance.
 * @param[in] srv_instance_id: AICS service instance index.
 *                             Used when the number of services is greater than 1.
 * @param[in] aics_param_type: Type of the AICS parameter to get.
 *                             @ref RTK_BT_LE_AUDIO_AICS_PARAM_INPUT_DES is not supported.
 * @param[in] value_len: Length of the buffer pointed to by p_value in octets.
 * @param[out] p_value: Buffer that receives the AICS parameter data.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_get_param(uint8_t srv_instance_id, rtk_bt_le_audio_aics_param_type_t aics_param_type, uint8_t value_len, uint8_t *p_value);

/**
 * @brief     Enable or disable notifications of Audio Input Control Service (AICS) characteristics by configuring their Client Characteristic Configuration descriptors.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[in] cfg_flags: Bit mask of the AICS characteristics to configure, see @ref rtk_bt_le_audio_aics_cccd_flags_t.
 * @param[in] enable: Whether to enable the notifications.
 *                    - true: Enable.
 *                    - false: Disable.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable);

/**
 * @brief     Read a characteristic value of an Audio Input Control Service (AICS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_READ_RESULT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[in] type: AICS characteristic to read.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_char_type_t type);

/**
 * @brief     Write the Audio Input Control Point of an Audio Input Control Service (AICS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[in] cp_op: Control point opcode.
 * @param[in] cp_param: Control point operation parameter.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
									   rtk_bt_le_audio_aics_cp_param_t cp_param);

/**
 * @brief     Write the Audio Input Control Point of an Audio Input Control Service (AICS) instance on every member of a group. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_AICS_CLIENT_CP_RESULT_IND.
 * @param[in] group_idx: Index of the target group.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[in] cp_op: Control point opcode.
 * @param[in] cp_param: Control point operation parameter.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API returns @ref RTK_BT_FAIL if the group is not found.
 */
uint16_t rtk_bt_le_audio_aics_write_cp_by_group(uint8_t group_idx, uint8_t srv_instance_id, rtk_bt_le_audio_aics_cp_op_t cp_op,
												rtk_bt_le_audio_aics_cp_param_t cp_param);

/**
 * @brief     Write the input description of an Audio Input Control Service (AICS) instance.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[in] input_des_len: Length of the input description in octets.
 * @param[in] p_input_des: Pointer to the input description data to write.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_write_input_des(uint16_t conn_handle, uint8_t srv_instance_id, uint16_t input_des_len, uint8_t *p_input_des);

/**
 * @brief     Get the data of an Audio Input Control Service (AICS) instance.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: AICS service instance index.
 * @param[out] p_srv_data: Buffer that receives the AICS service data.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_aics_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, rtk_bt_le_audio_aics_srv_data_t *p_srv_data);

#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
/**
 * @brief     Write the Media Control Point of a remote Media Control Service (MCS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_MEDIA_CP_NOTIFY_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: MCS service instance index.
 * @param[in] general_mcs: Whether the target is the Generic Media Control Service (GMCS).
 *                         - true: Target the GMCS.
 *                         - false: Target a non-generic MCS instance.
 * @param[in] is_req: Whether to send a write request or a write command.
 *                    - true: Write request.
 *                    - false: Write command.
 * @param[in] opcode: Media Control Point opcode, see @ref rtk_bt_le_audio_mcs_media_cp_opcode_t.
 * @param[in] p_op_param: Opcode-specific parameter.
 *                        Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mcs_client_write_media_cp(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, bool is_req, uint8_t opcode,
												   void *p_op_param);

/**
 * @brief     Read a characteristic value of a remote Media Control Service (MCS) instance. The result is reported asynchronously
 *            via event @ref RTK_BT_LE_AUDIO_EVT_MCP_CLIENT_READ_RESULT_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: MCS service instance index.
 * @param[in] general_mcs: Whether the target is the Generic Media Control Service (GMCS).
 *                         - true: Target the GMCS.
 *                         - false: Target a non-generic MCS instance.
 * @param[in] char_uuid: UUID of the characteristic to read.
 *                       Supported values: @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_STATE, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mcs_client_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint16_t char_uuid);

/**
 * @brief     Enable or disable notifications of Media Control Service (MCS) characteristics by configuring their Client Characteristic Configuration descriptors.
 * @param[in] conn_handle: Connection handle of the ACL connection.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] srv_instance_id: MCS service instance index.
 * @param[in] general_mcs: Whether the target is the Generic Media Control Service (GMCS).
 *                         - true: Target the GMCS.
 *                         - false: Target a non-generic MCS instance.
 * @param[in] cfg_flags: Bit mask of the MCS characteristics to configure, see @ref rtk_bt_le_audio_mcs_cccd_flag_t.
 * @param[in] enable: Whether to enable the notifications.
 *                    - true: Enable.
 *                    - false: Disable.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mcs_client_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, bool general_mcs, uint32_t cfg_flags, bool enable);
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/**
 * @brief     Send the value of a Media Control Service (MCS) characteristic from the local generic MCS server to the connected client.
 * @param[in] char_uuid: UUID of the characteristic to send.
 *                       Supported values: @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_CHANGED.
 * @param[in] p_param: Characteristic data to send, its content depends on char_uuid.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mcs_server_send_data(uint16_t char_uuid, void *p_param);

/**
 * @brief     Confirm a read request on a Media Control Service (MCS) characteristic received from a remote client by supplying the value to return. The request
 *            is reported via event @ref RTK_BT_LE_AUDIO_EVT_MCP_SERVER_READ_IND.
 * @param[in] conn_handle: Connection handle of the ACL connection that issued the read request.
 *                         Range: 0x0000 to 0x0EFF.
 * @param[in] cid: L2CAP channel identifier of the read request.
 * @param[in] service_id: Identifier of the MCS service that received the read request.
 * @param[in] char_uuid: UUID of the characteristic being read.
 *                       Supported values: @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_MEDIA_PLAYER_NAME, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_TITLE,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_DURATION, @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_TRACK_POSITION,
 *                       @ref RTK_BT_LE_AUDIO_MCS_UUID_CHAR_CONTENT_CONTROL_ID.
 * @param[in] offset: Offset into the characteristic value requested by the client.
 * @param[in] p_param: Characteristic value to return, its content depends on char_uuid.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_le_audio_mcs_server_read_confirm(uint16_t conn_handle, uint16_t cid, uint8_t service_id, uint16_t char_uuid, uint16_t offset, void *p_param);
#endif

/**
 * @brief     Configure the CAP parameters.
 * @param[in] p_param: CAP configuration parameters.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before the CAP is initialized, otherwise it returns @ref RTK_BT_FAIL.
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
