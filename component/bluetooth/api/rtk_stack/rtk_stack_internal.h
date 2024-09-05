/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __RTK_STACK_INTERNAL_H__
#define __RTK_STACK_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <app_msg.h>

uint16_t bt_stack_gap_init(void);
void bt_stack_pending_cmd_init(void);
void bt_stack_pending_cmd_deinit(void);
rtk_bt_cmd_t *bt_stack_pending_cmd_search(uint32_t msg_type);
void bt_stack_pending_cmd_insert(rtk_bt_cmd_t *p_cmd);
void bt_stack_pending_cmd_delete(rtk_bt_cmd_t *p_cmd);
bool bt_stack_profile_check(rtk_bt_profile_t profile);

uint16_t bt_stack_le_gap_wait_ready(void);
void bt_stack_le_gap_handle_io_msg(uint16_t gap_type, void *gap_msg);

uint16_t bt_stack_le_gap_init(void *gap_conf);
uint16_t bt_stack_le_gap_deinit(void);
uint16_t bt_stack_le_gap_get_conn_id(uint16_t conn_handle, uint8_t *p_conn_id);
uint16_t bt_stack_gatts_init(rtk_bt_app_conf_t *app_conf);
void bt_stack_gatts_deinit(void);
void bt_stack_gatts_disconnect_queue_clear(uint8_t conn_id);
uint16_t bt_stack_gattc_init(rtk_bt_app_conf_t *app_conf);
void  bt_stack_gattc_deinit(void);
uint16_t bt_stack_le_gatts_get_tx_pending_num(uint16_t conn_handle, uint16_t *tx_pending_num);
uint16_t bt_stack_le_gattc_get_tx_pending_num(uint16_t conn_handle, uint16_t *tx_pending_num);

uint16_t bt_stack_le_gap_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_gatts_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_gattc_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_gap_act_handle(rtk_bt_cmd_t *p_cmd);
void bt_stack_vendor_callback(uint8_t cb_type, void *p_cb_data);

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
uint16_t bt_stack_le_iso_act_handle(rtk_bt_cmd_t *p_cmd);
#endif
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
uint16_t bt_stack_br_gap_wait_ready(void);
uint16_t rtk_stack_framework_event_handler(uint8_t event);

uint16_t bt_stack_br_gap_init(void *p_evt_msg_q);
uint16_t bt_stack_br_gap_deinit(void);
uint16_t bt_stack_a2dp_init(uint8_t role);
void bt_stack_a2dp_deinit(void);
uint16_t bt_stack_avrcp_init(uint8_t role);
void bt_stack_avrcp_deinit(void);
uint16_t bt_stack_spp_init(uint8_t role);
void bt_stack_spp_deinit(void);
uint16_t bt_stack_hid_init(uint8_t role);
void bt_stack_hid_deinit(void);
uint16_t bt_stack_sdp_init(void);
void bt_stack_sdp_deinit(void);
uint16_t bt_stack_hfp_init(uint8_t role);
void bt_stack_hfp_deinit(void);
uint16_t bt_stack_pbap_init(uint8_t role);
void bt_stack_pbap_deinit(void);

uint16_t bt_stack_br_gap_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_a2dp_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_hfp_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_pbap_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_avrcp_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_sdp_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_spp_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_hid_act_handle(rtk_bt_cmd_t *p_cmd);
#else
static inline uint16_t bt_stack_br_gap_wait_ready(void)
{
	return RTK_BT_OK;
}

static inline uint16_t rtk_stack_framework_event_handler(uint8_t event)
{
	(void)event;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_br_gap_init(void *p_evt_msg_q)
{
	(void)p_evt_msg_q;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_br_gap_deinit(void)
{
	return 0;
}

static inline uint16_t bt_stack_a2dp_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_a2dp_deinit(void)
{

}

static inline uint16_t bt_stack_avrcp_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_avrcp_deinit(void)
{

}

static inline uint16_t bt_stack_spp_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_spp_deinit(void)
{

}

static inline uint16_t bt_stack_hid_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_hid_deinit(void)
{

}

static inline uint16_t bt_stack_sdp_init(void)
{
	return RTK_BT_OK;
}

static inline void bt_stack_sdp_deinit(void)
{

}

static inline uint16_t bt_stack_hfp_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_hfp_deinit(void)
{

}

static inline uint16_t bt_stack_pbap_init(uint8_t role)
{
	(void)role;
	return RTK_BT_OK;
}

static inline void bt_stack_pbap_deinit(void)
{

}

static inline uint16_t bt_stack_br_gap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_a2dp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_hfp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_pbap_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_avrcp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_spp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_hid_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}

static inline uint16_t bt_stack_sdp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}
#endif

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
uint16_t bt_stack_le_iso_act_handle(rtk_bt_cmd_t *p_cmd);
#else
static inline uint16_t bt_stack_le_iso_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
uint16_t bt_stack_le_audio_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_le_audio_init(rtk_bt_app_conf_t *papp_conf, void *io_msg_q, void *evt_msg_q);
void bt_stack_le_audio_deinit(void);
#else
static inline uint16_t bt_stack_le_audio_act_handle(rtk_bt_cmd_t *p_cmd)
{
	(void)p_cmd;
	return RTK_BT_OK;
}
static inline uint16_t bt_stack_le_audio_init(rtk_bt_app_conf_t *papp_conf, void *io_msg_q, void *evt_msg_q)
{
	(void)papp_conf;
	(void)io_msg_q;
	(void)evt_msg_q;
	return RTK_BT_OK;
}
static inline void bt_stack_le_audio_deinit(void)
{
}
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_STACK_INTERNAL_H__ */
