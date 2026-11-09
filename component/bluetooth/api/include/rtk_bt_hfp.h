/**
 * @file      rtk_bt_hfp.h
 * @brief     Bluetooth HFP type and function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_HFP_H__
#define __RTK_BT_HFP_H__

#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_RFC_HFP_CHANN_NUM               1          /*!< RFCOMM server channel number used by the HFP HF role. */
#define RTK_BT_RFC_HSP_CHANN_NUM               2          /*!< RFCOMM server channel number used by the HSP HS role. */
#define RTK_BT_RFC_HSP_AG_CHANN_NUM            22         /*!< RFCOMM server channel number used by the HSP AG role. */
#define RTK_BT_RFC_HFP_AG_CHANN_NUM            23         /*!< RFCOMM server channel number used by the HFP AG role. */
#define RTK_BT_HFP_MAX_SCO_DATA_LENGTH         120        /*!< Maximum length of a SCO audio data payload, in bytes. */
#define RTK_BT_HFP_MAX_PHONE_NUMBER            20         /*!< Maximum length of a phone number string, including the terminating NUL. */
#define RTK_BT_HFP_MAX_ATCMD_LENGTH            40         /*!< Maximum length of a vendor AT command string, including the terminating NUL. */
/* BT HFP AG Supported Local Feature */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_3WAY                   (1 << 0)    /*!< Three-way calling. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_EC_NR                  (1 << 1)    /*!< Echo canceling and noise reduction. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_VOICE_RECOGNITION      (1 << 2)    /*!< Voice recognition activation. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_INBAND_RINGING         (1 << 3)    /*!< In-band ringing. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_VOICE_TAG              (1 << 4)    /*!< Voice tag. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_REJECT_CALLS           (1 << 5)    /*!< Reject an incoming call. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_ENHANCED_CALL_STATUS   (1 << 6)    /*!< Enhanced call status. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_ENHANCED_CALL_CONTROL  (1 << 7)    /*!< Enhanced call control. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_EXTENED_ERROR_RESULT   (1 << 8)    /*!< Extended error result codes. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_CODEC_NEGOTIATION      (1 << 9)    /*!< Codec negotiation. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_HF_INDICATORS          (1 << 10)   /*!< HF indicators. */
#define RTK_BT_HFP_AG_LOCAL_CAPABILITY_ESCO_S4_T2_SUPPORTED   (1 << 11)   /*!< eSCO S4 (and T2) settings supported. */

#define RTK_BT_HFP_AG_CODEC_TYPE_CVSD                         (1 << 0)    /*!< CVSD codec. */
#define RTK_BT_HFP_AG_CODEC_TYPE_MSBC                         (1 << 1)    /*!< mSBC codec. */

/* BT HFP HF Supported Local Feature */
#define RTK_BT_HFP_HF_LOCAL_EC_NR_FUNCTION               (1 << 0)         /*!< Echo canceling and noise reduction. */
#define RTK_BT_HFP_HF_LOCAL_THREE_WAY_CALLING            (1 << 1)         /*!< Three-way calling. */
#define RTK_BT_HFP_HF_LOCAL_CLI_PRESENTATION_CAPABILITY  (1 << 2)         /*!< Calling line identification (CLI) presentation capability. */
#define RTK_BT_HFP_HF_LOCAL_VOICE_RECOGNITION_ACTIVATION (1 << 3)         /*!< Voice recognition activation. */
#define RTK_BT_HFP_HF_LOCAL_REMOTE_VOLUME_CONTROL        (1 << 4)         /*!< Remote volume control. */
#define RTK_BT_HFP_HF_LOCAL_ENHANCED_CALL_STATUS         (1 << 5)         /*!< Enhanced call status. */
#define RTK_BT_HFP_HF_LOCAL_ENHANCED_CALL_CONTROL        (1 << 6)         /*!< Enhanced call control. */
#define RTK_BT_HFP_HF_LOCAL_CODEC_NEGOTIATION            (1 << 7)         /*!< Codec negotiation. */
#define RTK_BT_HFP_HF_LOCAL_HF_INDICATORS                (1 << 8)         /*!< HF indicators. */
#define RTK_BT_HFP_HF_LOCAL_ESCO_S4_SETTINGS             (1 << 9)         /*!< eSCO S4 settings supported. */

#define RTK_BT_HFP_HF_CODEC_TYPE_CVSD                    (1 << 0)         /*!< CVSD codec. */
#define RTK_BT_HFP_HF_CODEC_TYPE_MSBC                    (1 << 1)         /*!< mSBC codec. */

#define RTK_BT_HFP_DEMO_DEFAULT_BATTERY_POWER_LEVEL 50    /*!< Default battery level reported by the demo application. Range: 0 to 100. */
#define RTK_BT_DEFAULT_SPEAKER_GAIN 14                    /*!< Default speaker gain level reported by the demo application. Range: 0 to 15. */
#define RTK_BT_DEFAULT_MICROPHONE_GAIN 14                 /*!< Default microphone gain level reported by the demo application. Range: 0 to 15. */
/************************************** Data Types *************************************/

/**
 * @typedef   rtk_bt_hfp_role_t
 * @brief     Bluetooth HFP role.
 */
typedef enum {
	RTK_BT_AUDIO_HFP_ROLE_AG = 0x00,    /*!< Audio Gateway (AG) role. */
	RTK_BT_AUDIO_HFP_ROLE_HF = 0x01,    /*!< Hands-Free (HF) role. */
} rtk_bt_hfp_role_t;

/**
 * @typedef   rtk_bt_hfp_call_status_t
 * @brief     Bluetooth HFP call status seen from the HF role.
 */
typedef enum {
	RTK_BT_HFP_CALL_IDLE                     = 0x00,    /*!< No call in progress. */
	RTK_BT_HFP_CALL_INCOMING                 = 0x01,    /*!< Incoming call is ringing. */
	RTK_BT_HFP_CALL_OUTGOING                 = 0x02,    /*!< Outgoing call is being set up. */
	RTK_BT_HFP_CALL_ACTIVE                   = 0x03,    /*!< Call is active. */
	RTK_BT_HFP_CALL_HELD                     = 0x04,    /*!< Call is on hold with no active call. */
	RTK_BT_HFP_CALL_ACTIVE_WITH_CALL_WAITING = 0x05,    /*!< Call is active and another incoming call is waiting. */
	RTK_BT_HFP_CALL_ACTIVE_WITH_CALL_HELD    = 0x06,    /*!< Call is active and another call is on hold. */
} rtk_bt_hfp_call_status_t;

/**
 * @typedef   rtk_bt_hfp_ag_call_status_t
 * @brief     Bluetooth HFP call status seen from the AG role.
 */
typedef enum {
	RTK_BT_HFP_AG_CALL_IDLE                     = 0x00,    /*!< No call in progress. */
	RTK_BT_HFP_AG_CALL_INCOMING                 = 0x01,    /*!< Incoming call is ringing. */
	RTK_BT_HFP_AG_CALL_OUTGOING                 = 0x02,    /*!< Outgoing call is being set up. */
	RTK_BT_HFP_AG_CALL_ACTIVE                   = 0x03,    /*!< Call is active. */
	RTK_BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING = 0x04,    /*!< Call is active and another incoming call is waiting. */
	RTK_BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD    = 0x05,    /*!< Call is active and another call is on hold. */
} rtk_bt_hfp_ag_call_status_t;

/**
 * @typedef   rtk_bt_cvsd_frame_duration
 * @brief     Bluetooth HFP CVSD frame duration.
 */
typedef enum {
	RTK_BT_AUDIO_CVSD_FRAME_DURATION_3_75_MS = 0x00,    /*!< Frame duration is 3.75 ms. */
	RTK_BT_AUDIO_CVSD_FRAME_DURATION_7_5_MS  = 0x01,    /*!< Frame duration is 7.5 ms. */
} rtk_bt_cvsd_frame_duration;

/**
 * @typedef   rtk_bt_hfp_ag_service_indicator
 * @brief     Bluetooth HFP AG service indicator.
 */
typedef enum rtk_bt_hfp_ag_service_indicator {
	RTK_BT_HFP_AG_SERVICE_STATUS_AVAILABLE   = 0x00,    /*!< Network service is available. */
	RTK_BT_HFP_AG_SERVICE_STATUS_UNAVAILABLE = 0x01,    /*!< Network service is unavailable. */
} rtk_bt_hfp_ag_service_indicator;

/**
 * @typedef   rtk_bt_hfp_ag_call_indicator
 * @brief     Bluetooth HFP AG call indicator.
 */
typedef enum rtk_bt_hfp_ag_call_indicator {
	RTK_BT_HFP_AG_NO_CALL_IN_PROGRESS = 0x00,    /*!< No call is in progress. */
	RTK_BT_HFP_AG_CALL_IN_PROGRESS    = 0x01,    /*!< At least one call is in progress. */
} rtk_bt_hfp_ag_call_indicator;

/**
 * @typedef   rtk_bt_hfp_ag_call_setup_indicator
 * @brief     Bluetooth HFP AG call setup indicator.
 */
typedef enum rtk_bt_hfp_ag_call_setup_indicator {
	RTK_BT_HFP_AG_CALL_SETUP_STATUS_IDLE          = 0x00,    /*!< No call setup is in progress. */
	RTK_BT_HFP_AG_CALL_SETUP_STATUS_INCOMING_CALL = 0x01,    /*!< Incoming call setup is in progress. */
	RTK_BT_HFP_AG_CALL_SETUP_STATUS_OUTGOING_CALL = 0x02,    /*!< Outgoing call setup is in progress. */
	RTK_BT_HFP_AG_CALL_SETUP_STATUS_ALERTING      = 0x03,    /*!< Remote party is being alerted for an outgoing call. */
} rtk_bt_hfp_ag_call_setup_indicator;

/**
 * @typedef   rtk_bt_hfp_ag_call_held_indicator
 * @brief     Bluetooth HFP AG call held indicator.
 */
typedef enum rtk_bt_hfp_ag_call_held_indicator {
	RTK_BT_HFP_AG_CALL_HELD_STATUS_IDLE                 = 0x00,    /*!< No call is on hold. */
	RTK_BT_HFP_AG_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL = 0x01,    /*!< One call is on hold and another call is active. */
	RTK_BT_HFP_AG_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL  = 0x02,    /*!< One call is on hold and no call is active. */
} rtk_bt_hfp_ag_call_held_indicator;

/**
 * @typedef   rtk_bt_hfp_ag_roaming_indicator
 * @brief     Bluetooth HFP AG roaming indicator.
 */
typedef enum rtk_bt_hfp_ag_roaming_indicator {
	RTK_BT_HFP_AG_ROAMING_STATUS_INACTIVE = 0x00,    /*!< Roaming is not active. */
	RTK_BT_HFP_AG_ROAMING_STATUS_ACTIVE   = 0x01,    /*!< Roaming is active. */
} rtk_bt_hfp_ag_roaming_indicator;

/**
 * @struct    rtk_bt_hfp_ag_conf_t
 * @brief     Bluetooth HFP AG configuration.
 */
typedef struct {
	uint8_t link_num;                /*!< Maximum number of simultaneously connected HFP AG/HSP AG links. */
	uint8_t rfc_hfp_ag_chann_num;    /*!< RFCOMM channel number used for HFP AG. */
	uint8_t rfc_hsp_ag_chann_num;    /*!< RFCOMM channel number used for HSP AG. */
	int ag_supported_features;       /*!< HFP AG supported features.
                                      *   Bitwise OR of RTK_BT_HFP_AG_LOCAL_CAPABILITY_* flags. */
	uint8_t ag_supported_codecs;     /*!< HFP AG supported codecs.
                                      *   Bitwise OR of RTK_BT_HFP_AG_CODEC_TYPE_* flags. */
} rtk_bt_hfp_ag_conf_t;

/**
 * @struct    rtk_bt_hfp_hf_conf_t
 * @brief     Bluetooth HFP HF configuration.
 */
typedef struct {
	uint8_t link_num;               /*!< Maximum number of simultaneously connected HFP/HSP links. */
	uint8_t rfc_hfp_chann_num;      /*!< RFCOMM channel number used for HFP. */
	uint8_t rfc_hsp_chann_num;      /*!< RFCOMM channel number used for HSP. */
	int hf_supported_features;      /*!< HFP HF supported features.
                                     *   Bitwise OR of RTK_BT_HFP_HF_LOCAL_* flags. */
	uint8_t hf_supported_codecs;    /*!< HFP HF supported codecs.
                                     *   Bitwise OR of RTK_BT_HFP_HF_CODEC_TYPE_* flags. */
} rtk_bt_hfp_hf_conf_t;

/**
 * @struct    rtk_bt_hfp_batt_level_t
 * @brief     Bluetooth HFP battery level report parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t level;         /*!< Current battery level.
                            *   Range: 0 to 100. */
} rtk_bt_hfp_batt_level_t;

/**
 * @struct    rtk_bt_hfp_microphone_gain_t
 * @brief     Bluetooth HFP microphone gain report parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t gain;          /*!< Microphone gain level.
                            *   Range: 0 to 15. */
} rtk_bt_hfp_microphone_gain_t;

/**
 * @struct    rtk_bt_hfp_vnd_at_cmd_t
 * @brief     Bluetooth HFP vendor AT command.
 */
typedef struct {
	uint8_t bd_addr[6];                                /*!< Remote Bluetooth device address. */
	const char at_cmd[RTK_BT_HFP_MAX_ATCMD_LENGTH];    /*!< Vendor AT command string. */
	uint16_t len;                                      /*!< Length of the AT command in bytes. */
} rtk_bt_hfp_vnd_at_cmd_t;

/**
 * @struct    rtk_bt_hfp_speaker_gain_t
 * @brief     Bluetooth HFP speaker gain report parameters.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t gain;          /*!< Speaker gain level.
                            *   Range: 0 to 15. */
} rtk_bt_hfp_speaker_gain_t;

/**
 * @struct    rtk_bt_hfp_hf_battery_ind_t
 * @brief     Bluetooth HFP HF battery indicator request from the AG.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t state;         /*!< Battery indicator state reported by the AG. */
	uint8_t *ret_info;     /*!< Output: pointer to the battery level reported back to the AG.
                            *   Range of the level: 0 to 100. */
} rtk_bt_hfp_hf_battery_ind_t;

/**
 * @struct    rtk_bt_hfp_hf_speaker_volume_changed_ind_t
 * @brief     Bluetooth HFP HF speaker volume change from the AG.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t volume;        /*!< New speaker volume. */
} rtk_bt_hfp_hf_speaker_volume_changed_ind_t;

/**
 * @struct    rtk_bt_hfp_hf_mic_volume_changed_ind_t
 * @brief     Bluetooth HFP HF microphone volume change from the AG.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t volume;        /*!< New microphone volume. */
} rtk_bt_hfp_hf_mic_volume_changed_ind_t;

/**
 * @struct    rtk_bt_hfp_call_incoming_t
 * @brief     Bluetooth HFP AG incoming call parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                  /*!< Remote Bluetooth device address. */
	const char call_num[RTK_BT_HFP_MAX_PHONE_NUMBER];    /*!< Incoming call number string. */
	uint8_t call_num_len;                                /*!< Length of the call number including the terminating '\0'.
                                                          *   Maximum: RTK_BT_HFP_MAX_PHONE_NUMBER. */
	uint8_t call_num_type;                               /*!< Type of the call number. */
} rtk_bt_hfp_call_incoming_t;

/**
 * @struct    rtk_bt_hfp_sdp_attr_info_t
 * @brief     Bluetooth HFP remote SDP attribute information.
 */
typedef struct {
	uint8_t                 bd_addr[6];             /*!< Remote Bluetooth device address. */
	br_gap_uuid_type_t      srv_class_uuid_type;    /*!< Service class UUID type. */
	br_gap_uuid_data_t      srv_class_uuid_data;    /*!< Service class UUID data. */
	uint16_t                profile_version;        /*!< Remote profile version. */
	uint16_t                protocol_version;       /*!< Remote protocol version. */
	uint8_t                 server_channel;         /*!< RFCOMM server channel number of the remote service. */
	uint16_t                supported_feat;         /*!< Supported features bitmask of the remote service. */
	uint16_t                l2c_psm;                /*!< L2CAP PSM of the remote service. */
	uint8_t                 supported_repos;        /*!< Supported repositories bitmask of the remote service. */
	uint32_t                pbap_supported_feat;    /*!< PBAP supported features bitmask of the remote service. */
} rtk_bt_hfp_sdp_attr_info_t;

/**
 * @struct    rtk_bt_hfp_ag_features_ind_t
 * @brief     Bluetooth HFP supported features of the remote HF.
 */
typedef struct {
	uint16_t capability;    /*!< Supported features bitmask reported by the remote HF. */
} rtk_bt_hfp_ag_features_ind_t;

/**
 * @struct    rtk_bt_hfp_conn_ind_t
 * @brief     Bluetooth HFP connection information.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_conn_ind_t;

/**
 * @struct    rtk_bt_hfp_disconn_ind_t
 * @brief     Bluetooth HFP disconnection information.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_disconn_ind_t;

/**
 * @struct    rtk_bt_hfp_call_status_ind_t
 * @brief     Bluetooth HFP call status change.
 */
typedef struct {
	uint8_t bd_addr[6];     /*!< Remote Bluetooth device address. */
	uint8_t prev_status;    /*!< Call status before the change. */
	uint8_t curr_status;    /*!< Call status after the change. */
} rtk_bt_hfp_call_status_ind_t;

/**
 * @struct    rtk_bt_hfp_caller_id_ind_t
 * @brief     Bluetooth HFP caller number.
 */
typedef struct {
	uint8_t bd_addr[6];                             /*!< Remote Bluetooth device address. */
	char    number[RTK_BT_HFP_MAX_PHONE_NUMBER];    /*!< Caller number string. */
	uint8_t type;                                   /*!< Type of the caller number. */
} rtk_bt_hfp_caller_id_ind_t;

/**
 * @struct    rtk_bt_hfp_codec_t
 * @brief     Bluetooth HFP SCO codec configuration.
 */
typedef struct {
	uint8_t bd_addr[6];                       /*!< Remote Bluetooth device address. */
	uint8_t codec_type;                       /*!< Codec type in use on the SCO link.
                                               *   Value: RTK_BT_AUDIO_CODEC_mSBC or RTK_BT_AUDIO_CODEC_CVSD. */
	union {
		struct {
			uint8_t     channel_mode;         /*!< Channel mode. Fixed to mono (0) for mSBC. */
			int         sample_rate;          /*!< Sampling frequency in Hz. 16000 for mSBC. */
			uint8_t     bitpool;              /*!< Bitpool value. 26 for mSBC. */
			uint8_t     allocation_method;    /*!< Allocation method. Loudness for mSBC. */
			uint8_t     subbands;             /*!< Number of subbands. 8 for mSBC. */
			uint8_t     block_length;         /*!< Number of blocks per frame. 15 for mSBC. */
		} msbc;
		struct {
			uint8_t     channel_num;          /*!< Number of audio channels. 1 for CVSD. */
			int         sample_rate;          /*!< Sampling frequency in Hz. 8000 for CVSD. */
			uint8_t     frame_duration;       /*!< Frame duration.
                                               *   Value: @ref rtk_bt_cvsd_frame_duration. */
		} cvsd;
	};
} rtk_bt_hfp_codec_t;


/**
 * @typedef   rtk_bt_sco_pkt_status
 * @brief     Bluetooth SCO packet status.
 */
typedef enum rtk_bt_sco_pkt_status {
	RTK_BT_SCO_PKT_STATUS_OK           = 0x00,    /*!< Packet is received correctly. */
	RTK_BT_SCO_PKT_STATUS_INVALID      = 0x01,    /*!< Packet data is invalid. */
	RTK_BT_SCO_PKT_STATUS_NO_DATA      = 0x02,    /*!< No data is received. */
	RTK_BT_SCO_PKT_STATUS_PARTIAL_LOST = 0x03,    /*!< Part of the packet data is lost. */
} rtk_bt_sco_pkt_status;

/**
 * @struct    rtk_bt_hfp_sco_data_ind_t
 * @brief     Bluetooth HFP received SCO data.
 */
typedef struct {
	uint8_t bd_addr[6];                               /*!< Remote Bluetooth device address. */
	rtk_bt_sco_pkt_status status;                     /*!< Status of the received SCO packet. */
	uint32_t length;                                  /*!< Length of the SCO data in bytes.
                                                       *   Maximum: RTK_BT_HFP_MAX_SCO_DATA_LENGTH. */
	uint8_t  data[RTK_BT_HFP_MAX_SCO_DATA_LENGTH];    /*!< SCO data payload. */
} rtk_bt_hfp_sco_data_ind_t;

/**
 * @struct    rtk_bt_hfp_dial_number_t
 * @brief     Bluetooth HFP dial number parameters.
 */
typedef struct {
	uint8_t bd_addr[6];                                /*!< Remote Bluetooth device address. */
	const char number[RTK_BT_HFP_MAX_PHONE_NUMBER];    /*!< Phone number string to dial. */
} rtk_bt_hfp_dial_number_t;

/**
 * @struct    rtk_bt_hfp_sco_data_send_t
 * @brief     Bluetooth HFP SCO data to send.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t seq_num;       /*!< Sequence number of the SCO data packet. */
	uint8_t len;           /*!< Length of the SCO buffer in bytes. */
	uint8_t *buf;          /*!< SCO data buffer to send. */
} rtk_bt_hfp_sco_data_send_t;

/**
 * @struct    rtk_bt_hfp_ag_indicators_status_req_t
 * @brief     Bluetooth HFP AG indicators status request from the remote HF.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
	uint8_t *ret_info;     /*!< Output: pointer to the indicators status filled in by the application. */
} rtk_bt_hfp_ag_indicators_status_req_t;

/**
 * @struct    rtk_bt_hfp_ag_indicators_status_t
 * @brief     Bluetooth HFP AG indicators status.
 */
typedef struct {
	rtk_bt_hfp_ag_service_indicator service_indicator;          /*!< Service availability indicator. */
	rtk_bt_hfp_ag_call_indicator call_indicator;                /*!< Standard call status indicator. */
	rtk_bt_hfp_ag_call_setup_indicator call_setup_indicator;    /*!< Call setup status indicator. */
	rtk_bt_hfp_ag_call_held_indicator call_held_indicator;      /*!< Call hold status indicator. */
	uint8_t signal_indicator;                                   /*!< Signal strength indicator. */
	rtk_bt_hfp_ag_roaming_indicator roaming_indicator;          /*!< Roaming status indicator. */
	uint8_t batt_chg_indicator;                                 /*!< Battery charge indicator. */
} rtk_bt_hfp_ag_indicators_status_t;

/**
 * @struct    rtk_bt_hfp_ag_curr_calls_list_query_t
 * @brief     Bluetooth HFP AG current calls list query from the remote HF.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_ag_curr_calls_list_query_t;

/**
 * @struct    rtk_bt_hfp_ag_dial_last_number_ind_t
 * @brief     Bluetooth HFP AG last number indication.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_ag_dial_last_number_ind_t;

/**
 * @struct    rtk_bt_hfp_ag_call_answer_req_ind_t
 * @brief     Bluetooth HFP AG call answer request from the remote HF.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_ag_call_answer_req_ind_t;

/**
 * @struct    rtk_bt_hfp_ag_call_terminate_req_ind_t
 * @brief     Bluetooth HFP AG call terminate request from the remote HF.
 */
typedef struct {
	uint8_t bd_addr[6];    /*!< Remote Bluetooth device address. */
} rtk_bt_hfp_ag_call_terminate_req_ind_t;

/**
 * @struct    rtk_bt_hfp_unknown_at_event_t
 * @brief     Bluetooth HFP unknown AT command received from the remote device.
 */
typedef struct {
	uint8_t bd_addr[6];                                /*!< Remote Bluetooth device address. */
	const char at_cmd[RTK_BT_HFP_MAX_ATCMD_LENGTH];    /*!< Received AT command string. */
	uint16_t len;                                      /*!< Length of the AT command in bytes. */
} rtk_bt_hfp_unknown_at_event_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_hfp BT HFP APIs
 * @brief     BT HFP related function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Configure the HFP role and its stack parameters.
 * @param[in] role: HFP role.
 *                  Value: @ref RTK_BT_AUDIO_HFP_ROLE_AG or @ref RTK_BT_AUDIO_HFP_ROLE_HF.
 * @param[in] p_conf_t: Configuration of the selected role.
 *                      Points to @ref rtk_bt_hfp_ag_conf_t for the AG role, or to @ref rtk_bt_hfp_hf_conf_t for the HF role.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall be called before the BT is enabled.
 */
uint16_t rtk_bt_hfp_cfg(uint8_t role, void *p_conf_t);

/**
 * @brief     Connect HFP with a remote device. A service discovery is started first and the connection is set up with the discovered channel.
 *            The result is reported asynchronously via event @ref RTK_BT_HFP_EVT_CONN_CMPL (HF role) or @ref RTK_BT_HFP_EVT_AG_CONN_CMPL (AG role).
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_connect(uint8_t *bd_addr);

/**
 * @brief     Disconnect HFP from a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_HFP_EVT_DISCONN_CMPL (HF role) or @ref RTK_BT_HFP_EVT_AG_DISCONN_CMPL (AG role).
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_disconnect(uint8_t *bd_addr);

/**
 * @brief     Establish the SCO audio link with a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_HFP_EVT_SCO_CONN_CMPL.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_sco_connect(uint8_t *bd_addr);

/**
 * @brief     Release the SCO audio link with a remote device.
 *            The result is reported asynchronously via event @ref RTK_BT_HFP_EVT_SCO_DISCONNCTED_IND.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_sco_disconnect(uint8_t *bd_addr);

/**
 * @brief     Report an incoming call to the remote HF device.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] call_num: Incoming call number string.
 *                      Must not be NULL.
 * @param[in] call_num_len: Length of the call number including the terminating '\0'.
 *                          Maximum: 20.
 * @param[in] call_num_type: Type of the call number.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - In the HF role this API dials the given call number instead.
 */
uint16_t rtk_bt_hfp_call_incoming(uint8_t *bd_addr, const char *call_num, uint8_t call_num_len, uint8_t call_num_type);

/**
 * @brief     Answer the incoming call on the HFP link.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_call_answer(uint8_t *bd_addr);

/**
 * @brief     Terminate the current call on the HFP link.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_call_terminate(uint8_t *bd_addr);

/**
 * @brief     Dial a call with the given phone number.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] call_num: Phone number string to dial.
 *                      Must not be NULL.
 * @param[in] call_num_len: Length of the phone number in bytes.
 *                          Maximum: RTK_BT_HFP_MAX_PHONE_NUMBER.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_dial_with_number_req(uint8_t *bd_addr, const char *call_num, uint8_t call_num_len);

/**
 * @brief     Redial the last dialed number.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_dial_last_number_req(uint8_t *bd_addr);

/**
 * @brief     Send SCO audio data to the remote device.
 * @param[in] p_data_send_t: SCO data to send.
 *                           Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - The SCO link shall be established before calling this API.
 */
uint16_t rtk_bt_hfp_data_send(rtk_bt_hfp_sco_data_send_t *p_data_send_t);

/**
 * @brief     Report the local battery level to the remote device.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] power_level: Battery level.
 *                         Range: 0 to 100.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_batt_level_report(uint8_t *bd_addr, uint8_t power_level);

/**
 * @brief     Report the local speaker gain to the remote device.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] gain: Speaker gain level.
 *                  Range: 0 to 15.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_speaker_gain_report(uint8_t *bd_addr, uint8_t gain);

/**
 * @brief     Report the local microphone gain to the remote device.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] gain: Microphone gain level.
 *                  Range: 0 to 15.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_microphone_gain_report(uint8_t *bd_addr, uint8_t gain);

/**
 * @brief     Send a vendor-specific AT command to the remote device.
 * @param[in] bd_addr: Remote Bluetooth device address.
 *                     Must not be NULL.
 * @param[in] at_cmd: AT command string, terminated with "\r".
 *                    Must not be NULL.
 * @param[in] len: Length of the AT command in bytes.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_hfp_send_vnd_at_cmd_req(uint8_t *bd_addr, const char *at_cmd, uint16_t len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_HFP_H__ */
