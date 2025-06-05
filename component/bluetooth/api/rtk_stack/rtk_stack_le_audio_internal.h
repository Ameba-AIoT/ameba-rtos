/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_STACK_LE_AUDIO_INTERVAL_H__
#define __RTK_STACK_LE_AUDIO_INTERVAL_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>
#include <ascs_mgr.h>
#include <bass_client.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT

#define RTK_BT_BIG_SYNC_MAX_BIS_NUM 2
#define RTK_BT_LE_EXT_ADV_MAX_LOCAL_NAME_LENGTH 20
#define RTK_BT_LE_AUDIO_COMMON_SCAN_TYPE 1
#define RTK_BT_LE_AUDIO_BASS_SCAN_TYPE 2
#define RTK_BT_LE_AUDIO_BIG_PROC_BIG_SYNC_REQ 0x01
#define RTK_BT_LE_AUDIO_BIG_PROC_BIG_INFO_RECEIVED 0x02
#define RTK_BT_LE_AUDIO_BIG_PROC_BROADCAST_CODE_EXIST 0x04
#define RTK_BT_LE_AUDIO_DEMO_CCID_MAX_NUM 4
#define RTK_BT_LE_AUDIO_DEMO_SINK_ASE_MAX_NUM 2
#define RTK_BT_LE_AUDIO_DEMO_SOURCE_ASE_MAX_NUM 1
#define RTK_BT_LE_AUDIO_DEMO_ASE_MAX_NUM (RTK_BT_LE_AUDIO_DEMO_SINK_ASE_MAX_NUM + RTK_BT_LE_AUDIO_DEMO_SOURCE_ASE_MAX_NUM)
#define RTK_BT_LE_AUDIO_CIG_ISO_MODE 0x01
#define RTK_BT_LE_AUDIO_BIG_ISO_MODE 0x02
#define RTK_BT_LE_AUDIO_CCID_MAX 4
#define RTK_BT_LE_AUDIO_MAX_SET_MEM_SIZE 2
/* Define Audio channel.*/
#define RTK_BT_LE_AUDIO_DEMO_RTK_AUDIO_LEFT 1
#define RTK_BT_LE_AUDIO_DEMO_RTK_AUDIO_RIGHT 2
#define RTK_BT_LE_AUDIO_DEMO_RTK_AUDIO_STEREO 3
#define RTK_BT_LE_AUDIO_DEMO_CSIS_DISV_TIMEOUT (10000)
#define RTK_BT_LE_AUDIO_DEMO_ASE_QOS_RETRANS_NUM 2
#define RTK_BT_LE_AUDIO_DEMO_ASE_QOS_MAX_SDU 100
#define RTK_BT_LE_AUDIO_DEMO_CAP_METADATA_DATA_MAX_LEN 20

typedef struct {
	uint16_t count;
	void *mtx;
	struct list_head head;
} rtk_bt_le_audio_list_t;

typedef struct {
	bool used;
	uint8_t ase_id;
	uint8_t ase_state;
	uint8_t cig_id;
	uint8_t direction;
	uint8_t ccid_num;
	uint8_t ccid[RTK_BT_LE_AUDIO_DEMO_CCID_MAX_NUM];
	uint16_t iso_conn_handle;
	uint16_t audio_contexts;
} rtk_bt_le_audio_ase_t;

typedef struct {
	bool brs_is_used;
	uint8_t instance_id;
	uint8_t source_id;
	void *handle;
} rtk_bt_le_audio_bass_broadcast_source_data_t;

typedef struct {
	bool used;
	uint8_t conn_id;
	uint16_t mtu_size;
	uint16_t conn_handle;
	uint8_t state;
	uint8_t bd_addr[6];
	uint8_t bd_type;
	bool mtu_received;
	bool auth_cmpl;
	bool start_discover;
	uint16_t lea_disc_flags;
	uint16_t lea_srv_found_flags;
	uint8_t brs_char_num;
	rtk_bt_le_audio_bass_broadcast_source_data_t *brs_char_tbl;
	rtk_bt_le_audio_ase_t lea_ase_tbl[RTK_BT_LE_AUDIO_DEMO_ASE_MAX_NUM];
#if defined (RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT) && RTK_BLE_AUDIO_VCP_VOLUME_RENDERER_SUPPORT
	uint8_t volume_setting;
	uint8_t mute;
#endif
} rtk_bt_le_audio_link_t;

typedef struct {
	uint8_t subgroup_idx;
	uint16_t stream_audio_contexts;
	uint8_t bis_idx[RTK_BT_LE_AUDIO_BROADCAST_SOURCE_BIS_NUM];
} rtk_bt_le_audio_broadcast_sub_group_info_t;

typedef struct {
	uint8_t group_idx;
	uint32_t presentation_delay;
	uint8_t group_bis_num;
	rtk_bt_le_audio_broadcast_sub_group_info_t sub_group[RTK_BT_LE_AUDIO_BROADCAST_SOURCE_SUB_GROUP_NUM];
} rtk_bt_le_audio_broadcast_group_info_t;

typedef struct {
	rtk_bt_le_audio_broadcast_source_handle_t source_handle;
	rtk_bt_le_audio_broadcast_source_state_t state; /* current state */
	rtk_bt_le_audio_broadcast_source_state_t prefer_state; /* expected state */
	uint8_t encryption;
	rtk_bt_le_audio_broadcast_group_info_t group[RTK_BT_LE_AUDIO_BROADCAST_SOURCE_GROUP_NUM];
	rtk_bt_le_audio_cfg_codec_t codec_cfg;
	rtk_bt_le_audio_qos_cfg_preferred_t prefer_qos;
	uint8_t source_adv_sid;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
	uint8_t source_address[RTK_BD_ADDR_LEN];
	rtk_bt_le_audio_codec_cfg_item_t cfg_codec_index;
	rtk_bt_le_audio_qos_cfg_type_t cfg_qos_type;
	rtk_bt_le_addr_type_t cfg_local_addr_type;
	bool cfg_encryption;
} rtk_bt_le_audio_broadcast_source_cb_t;

typedef struct {
	struct list_head list;
	rtk_bt_le_audio_sync_handle_t sync_handle;
	uint8_t adv_sid;
	rtk_bt_le_addr_t adv_addr;
	uint8_t broadcast_id[RTK_BT_LE_AUDIO_BROADCAST_ID_LEN];
	uint8_t source_id;
	uint8_t pa_sync_state;
	uint8_t big_sync_state;
	uint8_t big_proc_flags;
	uint8_t encryption;
	bool is_past;
	uint16_t pa_interval;
	rtk_bt_le_audio_base_data_mapping_t *p_base_data_mapping;
	uint32_t support_bis_array;
	uint8_t broadcast_code[RTK_BT_LE_AUDIO_BROADCAST_CODE_LEN];
	uint8_t bis_num;
	uint8_t bis_idx[RTK_BT_LE_AUDIO_RECEIVER_BIS_MAX_NUM];
} rtk_bt_le_audio_sync_dev_info_t;

typedef struct {
	bool brs_is_used;
	uint8_t instance_id;
	uint8_t source_id;
	void *sync_handle;
} rtk_bt_le_audio_bass_brs_char_info_t;

typedef struct {
	uint8_t bd_addr[RTK_BD_ADDR_LEN];
	uint8_t bd_type;
	uint8_t adv_sid;
	uint8_t broadcast_id[3];
} rtk_bt_le_audio_baas_dev_info_t;

typedef struct {
	struct list_head list;
	uint8_t iso_mode;
	uint8_t frame_num;
	uint8_t path_direction;   //DATA_PATH_INPUT_FLAG or DATA_PATH_OUTPUT_FLAG
	bool is_processing;
	uint16_t iso_conn_handle;
	uint16_t pkt_seq_num;
	uint32_t presentation_delay;
	rtk_bt_le_audio_cfg_codec_t codec_data;
} rtk_bt_le_audio_iso_channel_info_t;

typedef struct {
	bool used;
	uint8_t ase_id;
	uint8_t ase_state;
	uint8_t cig_id;
	uint8_t direction;
	uint8_t ccid_num;
	uint8_t ccid[RTK_BT_LE_AUDIO_CCID_MAX_NUM];
	uint16_t iso_conn_handle;
	uint16_t audio_contexts;
} rtk_bt_le_audio_ase_info_t;

typedef struct {
	uint8_t iso_mode;
	uint8_t path_direction;//DATA_PATH_INPUT_FLAG or DATA_PATH_OUTPUT_FLAG
	uint8_t idx;
	uint16_t iso_conn_handle;
	uint32_t presentation_delay;
	rtk_bt_le_audio_cfg_codec_t codec_parsed_data;
} rtk_bt_le_audio_setup_data_path_info_t;

typedef struct {
	rtk_bt_le_audio_stream_session_handle_t session_handle;
	rtk_bt_le_audio_unicast_session_qos_t session_qos;
	uint16_t ready_conn_handle;
	rtk_bt_le_audio_stream_state_t bap_state;
	rtk_bt_le_audio_unicast_cfg_type_t audio_cfg_type;
	rtk_bt_le_audio_codec_cfg_item_t codec_cfg_item;
	rtk_bt_le_audio_ascs_ase_target_latency_t target_latency;
	rtk_bt_le_audio_qos_cfg_type_t qos_cfg_type;
	rtk_bt_le_audio_qos_cfg_preferred_t preferred_qos;
	bool release_req;
	uint16_t contexts_type;
	uint8_t ccid_num;
	uint8_t ccid[RTK_BT_LE_AUDIO_CCID_MAX];
	uint8_t dev_num;
	rtk_bt_le_audio_device_handle_t dev_tbl[RTK_BT_LE_AUDIO_MAX_SET_MEM_SIZE];
	uint32_t sink_used_location;
	uint32_t source_used_location;
} rtk_bt_le_audio_uni_info_t;

typedef struct {
	struct list_head list;
	uint8_t group_idx;
	uint8_t play_mode;
	rtk_bt_le_audio_group_handle_t group_handle;
	uint8_t set_mem_size;
	rtk_bt_le_audio_uni_info_t lea_unicast;
} rtk_bt_le_audio_group_info_t;

typedef struct {
	bool init_flag;
	rtk_bt_le_audio_link_t le_link[RTK_BT_LE_AUDIO_MAX_BLE_LINK_NUM];
	rtk_bt_le_audio_bap_role_t bap_role;
	rtk_bt_le_audio_cap_role_t cap_role;
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	uint8_t tmap_role;
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	uint8_t gmap_role;
#endif
	rtk_bt_le_audio_app_conf_t lea_app_conf;
#if defined (RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT) && RTK_BLE_AUDIO_CSIP_SET_MEMBER_SUPPORT
	rtk_bt_le_audio_csis_cfg_t csis_cfg;
	uint8_t lea_csis_sirk[RTK_BT_LE_CSIS_SIRK_LEN];
#endif
	uint16_t lea_sink_available_contexts;
	uint16_t lea_source_available_contexts;
	uint16_t lea_sink_supported_contexts;
	uint16_t lea_source_supported_contexts;
	uint32_t lea_sink_audio_location;
	uint32_t lea_source_audio_location;
	rtk_bt_le_audio_list_t iso_input_queue;        //host to controller
	rtk_bt_le_audio_list_t iso_output_queue;       //controller to host
	rtk_bt_le_audio_list_t sync_handle_queue;
	rtk_bt_le_audio_list_t group_info_queue;
	rtk_bt_le_audio_broadcast_source_cb_t bsrc;
#if defined (RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT) && RTK_BLE_AUDIO_MCP_MEDIA_CONTROL_SERVER_SUPPORT
	rtk_bt_le_audio_mcp_t mcp;
#endif
} rtk_bt_le_audio_priv_t;

rtk_bt_le_audio_link_t *bt_stack_le_audio_alloc_le_link(uint16_t conn_handle);
rtk_bt_le_audio_link_t *bt_stack_le_audio_find_link_by_conn_handle(uint16_t conn_handle);
bool bt_stack_le_audio_link_free(rtk_bt_le_audio_link_t *p_link);
rtk_bt_le_audio_ase_t *bt_stack_le_audio_find_ase(uint16_t conn_handle, uint8_t ase_id);
void bt_stack_le_audio_demo_update_ase_state(T_ASCS_ASE_STATE *p_data);
void bt_stack_le_audio_gen_metadata(uint8_t *p_data, uint8_t *p_data_len, uint16_t contexts_type, uint8_t ccid_num, uint8_t *p_ccid);
uint16_t bt_stack_le_audio_check_sync(rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info);
uint16_t bt_stack_le_audio_bass_get_supported_bis(rtk_bt_le_audio_sync_dev_info_t *p_sync_dev_info);
rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find_by_source_id(uint8_t source_id);
rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find_by_addr(uint8_t *bd_addr, uint8_t bd_type);
rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_find(void *sync_handle);
uint16_t bt_stack_le_audio_sync_dev_remove(void *sync_handle);
uint16_t bt_stack_le_audio_sync_dev_remove_all(void);
rtk_bt_le_audio_sync_dev_info_t *bt_stack_le_audio_sync_dev_add(void *sync_handle);
void bt_stack_le_audio_sync_list_show(void);
void *bt_stack_le_audio_link_find_brs_char_by_instance_id(rtk_bt_le_audio_link_t *p_link, uint8_t instance_id);
void *bt_stack_le_audio_link_find_brs_char_by_sync_handle(rtk_bt_le_audio_link_t *p_link, void *sync_handle);
void bt_stack_le_audio_link_update_brs_char(rtk_bt_le_audio_link_t *p_link, T_BASS_CLIENT_BRS_DATA *p_data);
void bt_stack_le_audio_link_init_brs_char(rtk_bt_le_audio_link_t *p_link, uint8_t brs_char_num);
rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group_by_conn_handle(uint16_t conn_handle);
rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group_by_idx(uint8_t group_idx);
rtk_bt_le_audio_group_info_t *bt_stack_le_audio_find_group(void *group_handle);
uint16_t bt_stack_le_audio_remove_group(void *group_handle);
uint16_t bt_stack_le_audio_remove_group_all(void);
rtk_bt_le_audio_group_info_t *bt_stack_le_audio_add_group(void *group_handle, uint8_t set_mem_size);
rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_find_iso_chann(uint16_t iso_conn_handle, uint8_t direction);
rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_add_iso_chann(uint16_t iso_conn_handle, uint8_t direction);
void bt_stack_le_audio_remove_iso_chann(uint16_t iso_conn_handle, uint8_t direction);
void bt_stack_le_audio_release_iso_chann(rtk_bt_le_audio_iso_channel_info_t *p_iso_chann);
uint16_t bt_stack_le_audio_hold_iso_chann(rtk_bt_le_audio_iso_channel_info_t *p_iso_chann);
rtk_bt_le_audio_iso_channel_info_t *bt_stack_le_audio_data_path_add(uint16_t event, uint8_t iso_mode, uint8_t direction, void *p_data);
void bt_stack_le_audio_handle_data_path_remove(uint16_t cis_conn_handle, uint8_t path_direction);
uint16_t bt_stack_le_audio_handle_data_path_remove_all(void);
uint16_t bt_stack_le_audio_common_init(void);
void bt_stack_le_audio_common_deinit(void);

/**
 * @}
 */

#endif //end of #if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_BT_LE_AUDIO_H__ */
