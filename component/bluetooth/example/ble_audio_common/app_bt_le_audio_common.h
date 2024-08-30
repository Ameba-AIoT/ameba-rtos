/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __APP_BT_LE_AUDIO_COMMON_H__
#define __APP_BT_LE_AUDIO_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <rtk_bt_def.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <rtk_bt_le_gap.h>
#include <rtk_bt_le_audio.h>
#include <bt_audio_intf.h>

#define RTK_BLE_AUDIO_BIRDS_SING_PCM_SUPPORT    1
#define RTK_BLE_AUDIO_RECORD_SUPPORT            0

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (a > b ? a : b)
#endif
#define CODEC_DFG_TEST 0

/* Define Audio channel.*/
#define RTK_BT_LE_AUDIO_LEFT            1
#define RTK_BT_LE_AUDIO_RIGHT           2
#define RTK_BT_LE_AUDIO_STEREO          3

/* Define Audio volume.*/
#define DEFAULT_AUDIO_TRACK_LEFT_VOLUME         (0.8)
#define DEFAULT_AUDIO_TRACK_RIGHT_VOLUME        (0.8)
#define DEFAULT_AUDIO_HARDWARE_LEFT_VOLUME      (0.8)
#define DEFAULT_AUDIO_HARDWARE_RIGHT_VOLUME     (0.8)

/* Define Audio VCS and MICS value.*/
#define RTK_BLE_AUDIO_MAX_VCS_OUTPUT_LEVEL      15
#define RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING       255
#define RTK_BLE_AUDIO_DEFAULT_VCS_VOL_SETTING   153
#define RTK_BLE_AUDIO_DEFAULT_VCS_MUTE          RTK_BT_LE_AUDIO_MICS_NOT_MUTE
#define RTK_BLE_AUDIO_DEFAULT_VCS_CHG_CNT       0
#define RTK_BLE_AUDIO_DEFAULT_VCS_VOL_FLAG      0
#define RTK_BLE_AUDIO_DEFAULT_VCS_STEP_SIZE     (RTK_BLE_AUDIO_MAX_VCS_VOL_SETTING/RTK_BLE_AUDIO_MAX_VCS_OUTPUT_LEVEL)

/* Define PCM BIT Width, cannot changed.*/
#define DEFAULT_PCM_BIT_WIDTH                   (16)
/* Define ADV max length*/
#define RTK_LE_AUDIO_ADV_LEN_MAX                251
/* Define default sync timeout Time = N*10 ms */
#define RTK_LE_AUDIO_SYNC_TIMEOUT               (100)
/** @brief  Default minimum primary advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MIN    (48)
/** @brief  Default maximum primary advertising interval */
#define RTK_BT_LE_AUDIO_PRIMARY_ADV_INTERVAL_MAX    (48)
/** @brief  Default minimum periodic advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MIN             (80)
/** @brief  Default maximum periodic advertising interval */
#define RTK_BT_LE_AUDIO_PA_INTERVAL_MAX             (80)

#define RTK_BLE_AUDIO_DEFAULT_BROADCASTER_PRESENTATION_DELAY    (40000) //units: us
#define RTK_BLE_AUDIO_DEFAULT_BROADCASTER_SUBGROUP_NUM          (1)
#define RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM               (1)
/*
    If want to broadcast 2-channels stereo audio in one BIS,please config:
    RTK_BLE_AUDIO_DEFAULT_BROADCASTER_BIS_NUM       to     1
    RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE    to     1
*/
#define RTK_BLE_AUDIO_BROADCASTER_ONE_BIS_SETEO_MODE        1

/*
    If want to start 2-channels stereo audio in one CIS,please config:
    RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE       to     1
*/
#define RTK_BLE_AUDIO_UNICAST_ONE_CIS_SETEO_MODE            1

/*
   Only apply in RTK_BT_LE_AUDIO_PLAY_MODE_CONVERSATION mode.
   Configure peer device source ase codec according to needs:
   For example
   -----> :sink ase cfg 48KHz birds sing by default app_lea_default_prefer_codec in app_bt_le_audio_common.h
   <----- :source ase cfg 32KHz microphone by app_lea_source_ase_prefer_codec
*/
#define LEA_SOURCE_ASE_PREFER_CODEC_CONFIG                  0

#define RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S              (10) //units: ms
#define RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_S_M              (10) //units: ms
#define RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_M_S_US           (10000) //units: us
#define RTK_BLE_AUDIO_DEFAULT_SDU_INTERVAL_S_M_US           (10000) //units: us
#define RTK_BLE_AUDIO_DEFAULT_LATENCY_M_S                   (10)
#define RTK_BLE_AUDIO_DEFAULT_LATENCY_S_M                   (10)
#define RTK_BLE_AUDIO_MAX_SDU_M_S                           (155) //units: byte
#define RTK_BLE_AUDIO_MAX_SDU_S_M                           (155) //units: byte
#define RTK_BLE_AUDIO_DEFAULT_RTN_M_S                       (2)
#define RTK_BLE_AUDIO_DEFAULT_RTN_S_M                       (2)

#define RTK_BLE_AUDIO_DEFAULT_PHY_1M                        (1) /**< bit 0:LE PHY 1M used. */
#define RTK_BLE_AUDIO_DEFAULT_PHY_2M                        (2) /**< bit 1:LE PHY 2M used. */
#define RTK_BLE_AUDIO_DEFAULT_PHY_CODED                     (4) /**< bit 2:LE Coded PHY used. */
#define RTK_BLE_AUDIO_DEFAULT_CSIS_DISV_TIMEOUT             (10000)
#define RTK_BLE_AUDIO_DEFAULT_BROADCAST_SOURCE_ADV_SID      0
#define ISO_INTERVAL_10_MS                                  0x01
#define ISO_INTERVAL_20_MS                                  0x02
#define ISO_INTERVAL_30_MS                                  0x03
/* Define broadcast audio announcement max length */
#define RTK_LE_AUDIO_BROADCAST_AUDIO_ANNOUNCEMENT_LEN_MAX       100
/* Define broadcast audio announcement default length */
#define RTK_LE_AUDIO_BROADCAST_AUDIO_ANNOUNCEMENT_DEFAULT_LEN   7
/* Define default sample frequecy in source for audio test.*/
#define LEA_SOURCE_FIX_SAMPLE_FREQUENCY                 RTK_BT_LE_SAMPLING_FREQUENCY_CFG_16K  //now support 8k,16k,24k,32k,48k birds sing array
#define LEA_SOURCE_FIX_FRAME_DUARTION                   RTK_BT_LE_FRAME_DURATION_CFG_10_MS   //support 10ms and 7.5ms 
#define LEA_BIG_ISO_INTERVAL_CONFIG                     ISO_INTERVAL_10_MS // if config 30ms, only support sample rate 48 KHz bis stereo mode
#define LEA_CIG_ISO_INTERVAL_CONFIG                     ISO_INTERVAL_10_MS // now only support 10ms and 20ms

#define RTK_BT_LE_MEDIA_AUDIO_CFG_SUPPORT (RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT |RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT | \
                                    RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT)
#define RTK_BT_LE_MEDIA_AUDIO_CFG_PREFER  (RTK_BT_LE_UNICAST_AUDIO_CFG_1_BIT |RTK_BT_LE_UNICAST_AUDIO_CFG_6_I_BIT | \
                                    RTK_BT_LE_UNICAST_AUDIO_CFG_6_II_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_4_BIT)
#define RTK_BT_LE_MEDIA_CODEC_CFG_SUPPORT (RTK_BT_LE_CODEC_CFG_ITEM_8_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_8_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_16_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_16_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_24_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_24_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT | RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_3_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_4_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_5_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_6_BIT )

#define RTK_BT_LE_CONVERSATIONAL_AUDIO_CFG_SUPPORT (RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT |RTK_BT_LE_UNICAST_AUDIO_CFG_7_II_BIT| \
                                            RTK_BT_LE_UNICAST_AUDIO_CFG_8_I_BIT|RTK_BT_LE_UNICAST_AUDIO_CFG_8_II_BIT| \
                                            RTK_BT_LE_UNICAST_AUDIO_CFG_11_I_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_11_II_BIT)
#define RTK_BT_LE_CONVERSATIONAL_AUDIO_CFG_PREFER  (RTK_BT_LE_UNICAST_AUDIO_CFG_3_BIT | \
                                            RTK_BT_LE_UNICAST_AUDIO_CFG_11_I_BIT | RTK_BT_LE_UNICAST_AUDIO_CFG_11_II_BIT)

#define RTK_BT_LE_CONVERSATIONAL_CODEC_CFG_SUPPORT (RTK_BT_LE_CODEC_CFG_ITEM_8_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_8_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_16_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_16_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_24_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_24_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT | RTK_BT_LE_CODEC_CFG_ITEM_32_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_1_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_2_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_3_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_4_BIT | \
                                    RTK_BT_LE_CODEC_CFG_ITEM_48_5_BIT | RTK_BT_LE_CODEC_CFG_ITEM_48_6_BIT )


#define RTK_BT_LE_AUDIO_CHARACTERISTIC_DESCRIPTION_MAX_LENGTH   32

#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM        1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM       1
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_NUM            (RTK_BT_LE_AUDIO_DEFAULT_AICS_VCS_NUM + RTK_BT_LE_AUDIO_DEFAULT_AICS_MICS_NUM)
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_BLUETOOTH_IDX  0
#define RTK_BT_LE_AUDIO_DEFAULT_AICS_MIC_IDX        1
#define RTK_BT_LE_AUDIO_BLUETOOTH_DES               "Ameba Bluetooth"
#define RTK_BT_LE_AUDIO_MIC_DES                     "Ameba MIC"
#endif

#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
#define RTK_BT_LE_AUDIO_DEFAULT_VOCS_NUM            1
#define RTK_BT_LE_AUDIO_VOCS_DES                    "Ameba Speaker"
#define RTK_BT_LE_AUDIO_VOCS_IDX                    0
#define RTK_BT_LE_AUDIO_VOCS_MAX_VAL                255
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_CLIENT_SUPPORT
#define RTK_BLE_AUDIO_MCS_STATE_TRANSLATE(state)    \
        (state == RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PLAYING)? "playing": \
        (state == RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_PAUSED)? "paused": \
        (state == RTK_BT_LE_AUDIO_MCS_MEDIA_STATE_SEEKING)? "seeking":"inactive"
#endif

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
#define RTK_BLE_AUDIO_MCS_MEDIA_PLAYER_NAME         "Ameba Media Player"
#define RTK_BLE_AUDIO_MCS_TRACK_TITLE               "Ameba Track"
#endif

#define RTK_BLE_AUDIO_ADV_DATA_ASCS_BIT             0x01
#define RTK_BLE_AUDIO_ADV_DATA_BASS_BIT             0x02
#define RTK_BLE_AUDIO_ADV_DATA_CAP_BIT              0x04
#define RTK_BLE_AUDIO_ADV_DATA_RSI_BIT              0x08
#define RTK_BLE_EXT_ADV_MAX_LOCAL_NAME_LENGTH       20

typedef struct {
	struct list_head list;
	uint16_t iso_conn_handle;
	uint16_t couple_iso_conn_handle;
	bool is_couple_iso_conn_handle;
	bool is_add_new_entity;
	bool is_removing;
	bool is_processing;
	rtk_bt_audio_track_t *track_hdl;
	rtk_bt_audio_record_t *record_hdl;
	void *codec_entity;
	uint16_t pkt_seq_num;       //both tx and rx
	uint32_t status_ok_cnt;     //both tx and rx
	uint32_t status_fail_cnt;   //both tx and rx
	uint32_t status_retry_cnt;  //for tx only
	uint32_t status_lost_cnt;   //for rx only
	uint8_t iso_mode;
	uint8_t iso_idx;
	uint8_t iso_data_tx_queue_num;
	uint8_t iso_data_rx_queue_num;
	struct list_head iso_data_head;
	short *p_encode_data;
	struct enc_codec_buffer *p_enc_codec_buffer_t;
	uint32_t encode_byte;
	uint32_t last_decode_offset;
	rtk_bt_le_audio_iso_data_path_direction_t path_direction;   /**< @ref rtk_bt_le_audio_iso_data_path_direction_t.*/
	rtk_bt_le_audio_cfg_codec_t codec;                          /**< @ref rtk_bt_le_audio_cfg_codec_t*/
	uint32_t time_stamp;
} app_lea_iso_data_path_t;

typedef struct {
	uint8_t tx_iso_path_num;
	uint8_t rx_iso_path_num;
	void *mtx;
	struct list_head head;
} app_lea_iso_data_path_list_t;

typedef struct {
	bool brs_is_used;
	uint8_t instance_id;
	uint8_t source_id;
	void *sync_handle;
} app_bt_le_audio_bass_brs_char_info_t;

typedef struct {
	struct list_head list;
	uint16_t conn_handle;
	rtk_bt_le_audio_device_handle_t device_handle;
	bool pacs_is_found;
	uint8_t sink_pac_num;
	uint8_t source_pac_num;
	bool ascs_is_found;
	uint8_t sink_ase_num;
	uint8_t source_ase_num;
	bool bass_is_found;
	uint8_t brs_char_num;
	app_bt_le_audio_bass_brs_char_info_t *brs_char_tbl;
	bool vcs_is_found;
#if (defined(RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_CONTROLLER_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT)
	uint8_t vcs_type_exist;
	rtk_bt_le_audio_vcs_volume_state_t vcs_volume_state;
	uint8_t vcs_volume_flags;
#endif
	bool mics_is_found;
#if (defined(RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_DEVICE_SUPPORT) || \
    (defined(RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT) && RTK_BLE_AUDIO_MICP_MIC_CONTROLLER_SUPPORT)
	uint8_t mic_mute;
#endif
	bool cas_is_found;  /**< Whether to find CAS. */
	bool cas_inc_csis;  /**< Whether the CAS instance include the CSIS instance. */
#if defined(RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_COORDINATOR_SUPPORT
	bool csis_is_found;
	uint8_t csis_srv_num;
#endif
	bool vocs_is_found;
#if defined(RTK_BLE_AUDIO_VOCS_SUPPORT) && RTK_BLE_AUDIO_VOCS_SUPPORT
	uint8_t vocs_srv_num;
#endif
	bool aics_is_found;
#if defined(RTK_BLE_AUDIO_AICS_SUPPORT) && RTK_BLE_AUDIO_AICS_SUPPORT
	uint8_t aics_srv_num;
#endif
} app_bt_le_audio_device_info_t;

typedef struct {
	struct list_head list;
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t set_mem_size;
	rtk_bt_le_audio_stream_session_handle_t stream_session_handle;
	rtk_bt_le_audio_unicast_session_qos_t session_qos;
	rtk_bt_le_audio_stream_state_t bap_state;
	rtk_bt_le_audio_unicast_cfg_type_t audio_cfg_type;
	rtk_bt_le_audio_ascs_ase_target_latency_t target_latency;
	rtk_bt_le_audio_unicast_ase_codec_t prefer_codec;
	uint32_t sink_used_location;
	uint32_t source_used_location;
	uint8_t play_mode;
	uint8_t dev_num;
	struct list_head device_info_head;
} app_bt_le_audio_group_info_t;

typedef struct {
	uint8_t group_idx;
	uint8_t subgroup_idx;
	rtk_bt_le_audio_bis_info_t bis_info;
	rtk_bt_le_audio_cfg_codec_t bis_codec_cfg;
	rtk_bt_le_audio_broadcast_source_handle_t broadcast_source_handle;
	uint8_t broadcast_source_state;
} app_bt_le_audio_bap_broadcast_source_info_t;

typedef struct {
	uint8_t rfu;
} app_bt_le_audio_bap_unicast_client_info_t;

typedef struct {
	uint8_t config_sink_ase_num;
	uint8_t config_source_ase_num;
	uint8_t enabling_sink_ase_num;
	uint8_t enabling_source_ase_num;
	uint8_t streaming_sink_ase_num;
	uint8_t streaming_source_ase_num;
} app_bt_le_audio_bap_unicast_server_info_t;

typedef struct {
	rtk_bt_le_audio_sync_handle_t sync_handle;
	uint32_t config_sink_audio_location;
	void *big_info_ind_sem;
} app_bt_le_audio_bap_broadcast_sink_info_t;

typedef struct {
	struct list_head list;
	rtk_bt_le_ext_scan_res_ind_t scan_res_ind;          /**< @ref rtk_bt_le_ext_scan_res_ind_t */
	uint8_t  adv_data_flags;
	uint8_t  ascs_announcement_type;
	uint16_t ascs_sink_available_contexts;
	uint16_t ascs_source_available_contexts;
	uint8_t  cap_announcement_type;
	uint8_t  local_name[RTK_BLE_EXT_ADV_MAX_LOCAL_NAME_LENGTH];
} app_bt_le_audio_scan_dev_info_t;

typedef struct {
	struct list_head list;
	uint8_t adv_sid;
	rtk_bt_le_addr_t adv_addr;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
} app_bt_le_audio_bass_scan_dev_info_t;

typedef struct {
	struct list_head list;
	rtk_bt_le_audio_sync_handle_t sync_handle;
	rtk_bt_le_audio_base_data_mapping_t base_data_mapping;
	uint8_t adv_sid;
	rtk_bt_le_addr_t adv_addr;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
	uint8_t source_id;
	uint8_t pa_sync_state;
	uint8_t big_sync_state;
	uint8_t is_encryption;
	bool is_past;
	uint16_t pa_interval;
	uint32_t support_bis_array;
	uint8_t broadcast_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];
	uint8_t num_bis;
	uint8_t bis_idx[RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM];
} app_bt_le_audio_sync_dev_info_t;

typedef struct {
	void *pa_sync_ind_sem;
} app_bt_le_audio_bap_broadcast_assistant_info_t;

#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
typedef struct {
	uint8_t mcs_service_id;
	rtk_bt_le_audio_mcp_server_media_play_name_t media_player_name;             /*@ref rtk_bt_le_audio_mcp_server_media_play_name_t */
	rtk_bt_le_audio_mcp_server_media_player_icon_url_t  media_player_icon_url;  /*@ref  rtk_bt_le_audio_mcp_server_media_player_icon_url_t */
	rtk_bt_le_audio_mcp_server_track_title_t track_title;                       /*@ref rtk_bt_le_audio_mcp_server_track_title_t */
	int32_t track_duration;  /* Length of the current track in 0.01-second resolution,zero or greater. @ref RTK_BT_LE_AUDIO_MCS_TRACK_DURATION_CHAR_VALUE_UNKNOWN : no current track or the duration of the current track is unknown */
	int32_t track_position;  /* Current track position of the current track in 0.01-second resolution, Offset from the start of the track to the current playing position.                                                                         * \arg    0: starting position or start of the track is not well defined
                                        * \arg    @ref RTK_BT_LE_AUDIO_MCS_TRACK_POSITION_CHAR_VALUE_UNAVAILABLE : no current track or the start of the track is not well defined
                                        * \arg    other values */
	int8_t  playback_speed;
	int8_t  seeking_speed;
	uint8_t playing_order;      /*@ref rtk_bt_le_audio_mcs_playing_orders_supported_t */
	uint8_t content_control_id;
} app_bt_le_audio_mcp_server_info_t;
#endif

typedef struct {
	uint8_t sample_frequency;
	uint8_t frame_duration;
	uint8_t codec_frame_blocks_per_sdu;
	uint16_t octets_per_codec_frame;
} app_lea_audio_support_capablity;

typedef struct {
	uint8_t *p_mcs_service_id;
	uint8_t aics_vcs_num;
	uint8_t *p_aics_vcs_tbl;
	uint8_t aics_mics_num;
	uint8_t *p_aics_mics_tbl;
	uint8_t vocs_num;
	uint8_t *p_vocs_feature_tbl;
	uint8_t *p_csis_sirk;
	uint8_t csis_sirk_len;
} app_lea_audio_cap_param_t;

/********************************************bro source struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_BROADCAST_SOURCE_DISABLE = 0,
	RTK_BLE_AUDIO_BROADCAST_SOURCE_ENABLE,
	RTK_BLE_AUDIO_BROADCAST_SOURCE_START,
	RTK_BLE_AUDIO_BROADCAST_SOURCE_STOP,
	RTK_BLE_AUDIO_BROADCAST_SOURCE_NOT_SUPPORT,
} app_bt_le_audio_broadcast_source_status_t;

typedef struct {
	rtk_bt_le_audio_cfg_codec_t         codec_cfg;
	rtk_bt_le_audio_qos_cfg_preferred_t prefer_qos;
	bool                                qos_manual_cfg;
	uint8_t                             cfg_bis_num;
	rtk_bt_le_audio_codec_cfg_item_t    cfg_codec_index;
	rtk_bt_le_audio_qos_cfg_type_t      cfg_qos_type;
	bool                                cfg_encryption;
} app_bt_bap_broadcast_source_param_cfg_t;

typedef struct {
	rtk_bt_le_audio_cfg_codec_t         codec_cfg;
	rtk_bt_le_audio_qos_cfg_preferred_t prefer_qos;
	rtk_bt_le_audio_codec_cfg_item_t    cfg_codec_index;
	rtk_bt_le_audio_qos_cfg_type_t      cfg_qos_type;
} app_bt_bap_unicast_client_param_cfg_t;

typedef struct {
	uint8_t                                     device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t                  lea_app_conf;
	app_bt_le_audio_bap_broadcast_source_info_t *p_bap_bro_sour_info;
	uint8_t                                     status;
} app_bt_bap_broadcast_source_info_t;

/********************************************bro sink struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_BROADCAST_SINK_DISABLE = 0,
	RTK_BLE_AUDIO_BROADCAST_SINK_ENABLE,
	RTK_BLE_AUDIO_BROADCAST_SINK_START,
	RTK_BLE_AUDIO_BROADCAST_SINK_STOP,
	RTK_BLE_AUDIO_BROADCAST_SINK_NOT_SUPPORT
} app_bt_le_audio_broadcast_sink_status_t;

typedef struct {
	uint8_t sound_channel;
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info;
	uint8_t status;
} app_bt_le_audio_broadcast_sink_info_t;

/********************************************bro assistant struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISABLE = 0,
	RTK_BLE_AUDIO_BROADCAST_ASSISTANT_ENABLE,
	RTK_BLE_AUDIO_BROADCAST_ASSISTANT_CONN,
	RTK_BLE_AUDIO_BROADCAST_ASSISTANT_DISC,
	RTK_BLE_AUDIO_BROADCAST_ASSISTANT_NOT_SUPPORT,
} app_bt_le_audio_broadcast_assistant_status_t;

typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_broadcast_assistant_info_t *p_bap_bro_assi_info;
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t status;
} app_bt_le_audio_broadcast_assistant_info_t;
/********************************************scan delegator struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_SCAN_DELEGATOR_DISABLE = 0,
	RTK_BLE_AUDIO_SCAN_DELEGATOR_ENABLE,
	RTK_BLE_AUDIO_SCAN_DELEGATOR_CONN,
	RTK_BLE_AUDIO_SCAN_DELEGATOR_DISC,
	RTK_BLE_AUDIO_SCAN_DELEGATOR_NOT_SUPPORT,
} app_bt_le_audio_scan_delegator_status_t;

typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	uint8_t status;
} app_bt_le_audio_scan_delegator_info_t;
/********************************************bap unicast client struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_UNICAST_CLIENT_DISABLE = 0,
	RTK_BLE_AUDIO_UNICAST_CLIENT_ENABLE,
	RTK_BLE_AUDIO_UNICAST_CLIENT_CONNECT,
	RTK_BLE_AUDIO_UNICAST_CLIENT_START,
	RTK_BLE_AUDIO_UNICAST_CLIENT_STOP,
	RTK_BLE_AUDIO_UNICAST_CLIENT_DISCONNECT,
	RTK_BLE_AUDIO_UNICAST_CLIENT_NOT_SUPPORT,
} app_bt_le_audio_unicast_client_status_t;

typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_unicast_client_info_t *p_bap_uni_cli_info;
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t status;
} app_bt_le_audio_unicast_client_info_t;
/********************************************bap unicast server struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_UNICAST_SERVER_DISABLE = 0,
	RTK_BLE_AUDIO_UNICAST_SERVER_ENABLE,
	RTK_BLE_AUDIO_UNICAST_SERVER_START,
	RTK_BLE_AUDIO_UNICAST_SERVER_STOP,
	RTK_BLE_AUDIO_UNICAST_SERVER_CONN,
	RTK_BLE_AUDIO_UNICAST_SERVER_DISC,
	RTK_BLE_AUDIO_UNICAST_SERVER_NOT_SUPPORT,
} app_bt_le_audio_unicast_server_status_t;

typedef struct {
	uint16_t conn_handle;
	uint8_t sound_channel;
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info;
	uint8_t status;
} app_bt_le_audio_unicast_server_info_t;
/********************************************cap acceptor struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_ACCEPTOR_DISABLE = 0,
	RTK_BLE_AUDIO_ACCEPTOR_ENABLE,
	RTK_BLE_AUDIO_ACCEPTOR_CONN,
	RTK_BLE_AUDIO_ACCEPTOR_TX_READY,
	RTK_BLE_AUDIO_ACCEPTOR_DISC,
	RTK_BLE_AUDIO_ACCEPTOR_START,
	RTK_BLE_AUDIO_ACCEPTOR_STOP,
	RTK_BLE_AUDIO_ACCEPTOR_BIG_SYNC,
	RTK_BLE_AUDIO_ACCEPTOR_BIG_SYNC_TERMINATE,
	RTK_BLE_AUDIO_ACCEPTOR_NOT_SUPPORT,
} app_bt_le_audio_acceptor_status_t;

typedef struct {
	uint8_t sound_channel;
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	uint8_t csis_neighbor_addr[RTK_BD_ADDR_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_unicast_server_info_t *p_bap_uni_ser_info;
	app_bt_le_audio_bap_broadcast_sink_info_t *p_bap_bro_sink_info;
	uint8_t status;
} app_bt_le_audio_acceptor_info_t;
/******************************************end cap acceptor struct***********************************/
/*******************************************cap initiator struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_INITIATOR_DISABLE = 0,
	RTK_BLE_AUDIO_INITIATOR_ENABLE,
	RTK_BLE_AUDIO_INITIATOR_CONNECT,
	RTK_BLE_AUDIO_INITIATOR_UNICAST_START,
	RTK_BLE_AUDIO_INITIATOR_UNICAST_STOP,
	RTK_BLE_AUDIO_INITIATOR_DISCONNECT,
	RTK_BLE_AUDIO_INITIATOR_BROADCAST_START,
	RTK_BLE_AUDIO_INITIATOR_BROADCAST_STOP,
	RTK_BLE_AUDIO_INITIATOR_NOT_SUPPORT,
} app_bt_le_audio_initiator_status_t;

typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_unicast_client_info_t *p_bap_uni_cli_info;
	app_bt_le_audio_bap_broadcast_source_info_t *p_bap_bro_sour_info;
	app_bt_le_audio_bap_broadcast_assistant_info_t *p_bap_bro_assi_info;
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	app_bt_le_audio_mcp_server_info_t *p_mcp_svr_info;
#endif
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t status;
} app_bt_le_audio_initiator_info_t;
/*****************************************end cap initiator struct***********************************/
/*******************************************cap commander struct*************************************/
typedef enum {
	RTK_BLE_AUDIO_COMMANDER_DISABLE = 0,
	RTK_BLE_AUDIO_COMMANDER_ENABLE,
	RTK_BLE_AUDIO_COMMANDER_CONN,
	RTK_BLE_AUDIO_COMMANDER_DISC,
	RTK_BLE_AUDIO_COMMANDER_NOT_SUPPORT,
} app_bt_le_audio_commander_status_t;

typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];
	rtk_bt_le_audio_app_conf_t lea_app_conf;
	app_bt_le_audio_bap_broadcast_assistant_info_t *p_bap_bro_assi_info;
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t status;
} app_bt_le_audio_commander_info_t;
/*****************************************end cap commander struct***********************************/
extern app_lea_iso_data_path_list_t g_app_lea_iso_path_list;
extern rtk_bt_le_ext_scan_param_t app_lea_def_ext_scan_param;
/* common broadcast source information parameter */
extern app_bt_le_audio_bap_broadcast_source_info_t app_bap_bro_sour_info;
/* common broadcast sink information parameter */
extern app_bt_le_audio_bap_broadcast_sink_info_t app_bap_bro_sink_info;
/* common unicast client information parameter */
extern app_bt_le_audio_bap_unicast_client_info_t app_bap_uni_client_info;
/* common unicast server information parameter */
extern app_bt_le_audio_bap_unicast_server_info_t app_bap_uni_server_info;
/* common broadcast source information parameter */
extern app_bt_le_audio_bap_broadcast_assistant_info_t app_bap_bro_assi_info;
#if defined(RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
/* common mcp server info parameter */
extern app_bt_le_audio_mcp_server_info_t app_mcp_svr_info;
#endif
/* broadcast source param */
extern rtk_bt_le_audio_broadcast_source_group_param_t def_le_audio_broadcast_source_group_param;
extern rtk_bt_le_audio_broadcast_source_config_t def_le_audio_broadcast_source_config_param;
extern uint8_t app_lea_def_broadcast_audio_announcements[];
extern uint8_t app_lea_def_broadcast_audio_announcements_len;
extern uint8_t app_lea_def_broadcast_code[];
extern app_bt_bap_broadcast_source_param_cfg_t bap_broadcast_source_cfg;
extern app_bt_bap_unicast_client_param_cfg_t bap_unicast_client_cfg;
extern rtk_bt_le_audio_unicast_ase_qos_t bap_def_ase_qos;
extern uint8_t bap_broadcast_source_codec_level2[];
extern uint8_t bap_broadcast_source_codec_level2_len;
extern rtk_bt_le_audio_broadcast_source_create_big_t def_le_audio_broadcast_source_create_big_param;
/* unicast client param */
extern rtk_bt_le_audio_unicast_session_qos_t app_lea_default_unicast_session_qos;
extern rtk_bt_le_audio_unicast_ase_codec_t app_lea_default_prefer_codec;
#if defined(LEA_SOURCE_ASE_PREFER_CODEC_CONFIG) && LEA_SOURCE_ASE_PREFER_CODEC_CONFIG
extern rtk_bt_le_audio_unicast_ase_codec_t app_lea_source_ase_prefer_codec;
#endif
void app_bt_le_audio_group_list_init(void);
void app_bt_le_audio_group_list_deinit(void);
uint8_t app_bt_le_audio_group_list_get_num(void);
app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_find(rtk_bt_le_audio_group_handle_t group_handle);
app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_find_by_conn_handle(uint16_t conn_handle);
rtk_bt_le_audio_group_handle_t app_bt_le_audio_group_list_find_by_group_idx(uint8_t group_idx);
app_bt_le_audio_group_info_t *app_bt_le_audio_group_list_add_dev(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle,
																 uint16_t conn_handle);
uint16_t app_bt_le_audio_group_list_remove_dev(rtk_bt_le_audio_group_handle_t group_handle, rtk_bt_le_audio_device_handle_t device_handle);
uint16_t app_bt_le_audio_group_list_remove_dev_by_conn_handle(rtk_bt_le_audio_group_handle_t group_handle, uint16_t conn_handle);
uint16_t app_bt_le_audio_group_list_remove(rtk_bt_le_audio_group_handle_t group_handle);
uint16_t app_bt_le_audio_group_list_remove_all(void);
app_bt_le_audio_device_info_t *app_bt_le_audio_group_list_find_dev_by_idx(rtk_bt_le_audio_group_handle_t group_handle, uint8_t dev_idx);
app_bt_le_audio_device_info_t *app_bt_le_audio_group_list_find_dev_by_conn_handle(uint16_t conn_handle);
app_bt_le_audio_device_info_t *app_bt_le_audio_device_list_add(uint16_t conn_handle);
uint16_t app_bt_le_audio_device_list_remove(uint16_t conn_handle);
uint16_t app_bt_le_audio_new_device_add_in_group(uint16_t conn_handle, rtk_bt_le_audio_group_handle_t *p_group_handle);
rtk_bt_audio_track_t *app_bt_le_audio_track_add(rtk_bt_le_audio_cfg_codec_t *p_codec);
uint16_t app_bt_le_audio_track_remove(void *audio_track_hdl);
void *app_bt_le_audio_lc3_codec_entity_add(rtk_bt_le_audio_cfg_codec_t *p_codec);
uint16_t app_bt_le_audio_lc3_codec_entity_remove(void *codec_entity);
rtk_bt_audio_record_t *app_bt_le_audio_record_add(rtk_bt_le_audio_cfg_codec_t *p_codec);
uint16_t app_bt_le_audio_record_remove(void *audio_record_hdl);
uint32_t app_bt_le_audio_translate_le_chnl_to_audio_chnl(uint32_t audio_channel_allocation);
uint8_t app_bt_le_audio_get_lea_chnl_num(uint32_t audio_channel_allocation);
uint32_t app_bt_le_audio_get_channel_location(uint32_t pacs_audio_loc, uint8_t channel_num, uint32_t used_loc);
uint32_t app_bt_le_audio_translate_lea_samp_fre_to_audio_samp_rate(uint8_t sample_frequency);
uint16_t app_bt_le_audio_device_name_config(uint8_t *device_name, uint8_t sound_channel);
uint16_t app_bt_le_audio_lea_app_conf(void *conf, uint8_t sound_channel, uint8_t le_audio_cap_role, uint8_t le_audio_bap_role,
									  uint8_t *pac_source_codec, uint8_t pac_source_codec_len, uint8_t *pac_sink_codec, uint8_t pac_sink_codec_len, app_lea_audio_cap_param_t *p_cap_param);
void app_bt_le_audio_update_adv(void *app_conf, uint8_t *device_name, uint8_t *adv_data, uint8_t *adv_data_len);
void app_bt_le_audio_scan_dev_list_init(void);
void app_bt_le_audio_scan_dev_list_deinit(void);
void app_bt_le_audio_broadcast_dev_list_init(void);
void app_bt_le_audio_broadcast_dev_list_deinit(void);
void app_bt_le_audio_iso_data_path_init(void);
void app_bt_le_audio_iso_data_path_deinit(void);
uint8_t app_bt_le_audio_iso_data_path_get_num(uint8_t path_direction);
app_lea_iso_data_path_t *app_bt_le_audio_iso_data_path_find(uint16_t iso_conn_handle, uint8_t path_direction);
app_lea_iso_data_path_t *app_bt_le_audio_iso_data_path_find_by_idx(uint8_t idx, uint8_t path_direction);
uint16_t app_bt_le_audio_iso_data_path_add(uint8_t iso_mode, uint8_t iso_idx, uint8_t path_direction, uint16_t iso_conn_handle, void *codec_cfg);
uint16_t app_bt_le_audio_iso_data_path_remove(uint16_t iso_conn_handle, uint8_t path_direction);
bool app_bt_le_audio_broadcast_source_set_codec_to_level2(uint8_t *p_codec, uint8_t codec_len, uint8_t type, uint16_t value);
uint16_t app_bt_le_audio_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind);
uint16_t app_bt_le_audio_scan_report_show(void);
uint16_t app_bt_le_audio_scan_dev_list_remove_all(void);
uint16_t app_bt_le_audio_bass_scan_report_handle(rtk_bt_le_ext_scan_res_ind_t *scan_res_ind);
uint16_t app_bt_le_audio_bass_scan_report_show(void);
bool app_bt_le_audio_broadcast_source_get_codec_from_level2(uint8_t *p_codec, uint8_t codec_len, rtk_bt_le_audio_cfg_codec_t *p_bis_codec_cfg);
uint16_t app_bt_le_audio_bass_brs_char_init(app_bt_le_audio_device_info_t *p_device_info, uint8_t brs_char_num);
uint16_t app_bt_le_audio_bass_brs_char_update(app_bt_le_audio_device_info_t *p_device_info, uint8_t char_instance_id,
											  rtk_bt_le_audio_bass_brs_data_t *p_brs_data);
uint16_t app_bt_le_audio_bass_pa_show(void);
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_source_id(uint8_t source_id);
app_bt_le_audio_bass_scan_dev_info_t *app_bt_le_audio_bass_scan_dev_list_find(rtk_bt_le_addr_t adv_addr);
uint8_t app_bt_le_audio_bass_scan_dev_list_get_num(void);
uint16_t app_bt_le_audio_bass_scan_dev_list_remove_all(void);
uint8_t app_bt_le_audio_sync_dev_list_get_num(void);
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_add(rtk_bt_le_audio_sync_handle_t sync_handle);
uint16_t app_bt_le_audio_sync_dev_list_remove(rtk_bt_le_audio_sync_handle_t sync_handle);
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find(rtk_bt_le_audio_sync_handle_t sync_handle);
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_addr(rtk_bt_le_addr_t source_adv_addr);
app_bt_le_audio_sync_dev_info_t *app_bt_le_audio_sync_dev_list_find_by_idx(uint8_t sync_handle_idx);
uint16_t app_bt_le_audio_sync_dev_list_remove_all(void);
uint16_t app_bt_le_audio_cmd_cfg(int argc, char *argv[], uint8_t *p_sound_channel, uint8_t *p_csis_neighbor_addr, uint8_t *p_device_name);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __APP_BT_LE_AUDIO_COMMON_H__ */
