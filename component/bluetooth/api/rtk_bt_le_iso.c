/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_iso.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>

uint16_t rtk_bt_le_iso_cig_get_conn_handle(uint16_t cis_conn_handle, uint16_t *p_conn_handle)
{
	rtk_bt_le_iso_cig_get_conn_handle_param_t param = {0};

	if (!cis_conn_handle || !p_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.p_conn_handle = p_conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_GET_CONN_HANDLE,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_get_conn_handle_param_t));
}

uint16_t rtk_bt_le_iso_cig_get_cis_info(uint16_t cis_conn_handle, rtk_bt_le_iso_cig_cis_info_t *p_info)
{
	rtk_bt_le_iso_cig_get_cis_info_param_t param = {0};

	if (!cis_conn_handle || !p_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.p_cis_info = (void *)p_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_GET_CIS_INFO,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_get_cis_info_param_t));
}

uint16_t rtk_bt_le_iso_cig_get_isoch_info(uint16_t cis_conn_handle, rtk_bt_le_iso_cis_channel_info_t *p_info)
{
	rtk_bt_le_iso_cig_get_isoch_info_param_t param = {0};

	if (!cis_conn_handle || !p_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.p_isoch_info = (void *)p_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_GET_ISOCH_INFO,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_get_isoch_info_param_t));
}

uint16_t rtk_bt_le_iso_cig_setup_path(rtk_bt_le_iso_setup_path_param_t *param)
{
	if (!param || !param->iso_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_SETUP_PATH,
						   (void *)param, sizeof(rtk_bt_le_iso_setup_path_param_t));
}

uint16_t rtk_bt_le_iso_cig_remove_path(uint16_t cis_conn_handle, rtk_bt_le_iso_data_path_direction_flag_t data_path_direction)
{
	rtk_bt_le_iso_remove_path_param_t param = {0};

	if (!cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.iso_conn_handle = cis_conn_handle;
	param.data_path_direction = data_path_direction;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_REMOVE_PATH,
						   (void *)&param, sizeof(rtk_bt_le_iso_remove_path_param_t));
}


uint16_t rtk_bt_le_iso_cig_disconnect(uint16_t cis_conn_handle)
{
	if (!cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_DISCONNECT,
						   (void *)&cis_conn_handle, sizeof(uint16_t));
}

uint16_t rtk_bt_le_iso_cig_read_iso_tx_sync(uint16_t cis_conn_handle, rtk_bt_le_iso_cig_read_iso_tx_sync_info_t *p_tx_sync_info)
{
	rtk_bt_le_iso_cig_read_iso_tx_sync_t param = {0};

	if (!cis_conn_handle || !p_tx_sync_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.p_tx_sync_info = p_tx_sync_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_READ_ISO_TX_SYNC,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_read_iso_tx_sync_t));
}

uint16_t rtk_bt_le_iso_cig_read_link_quality(uint16_t cis_conn_handle, rtk_bt_le_iso_cig_read_link_quality_info_t *p_link_quality_info)
{
	rtk_bt_le_iso_cig_read_link_quality_t param = {0};

	if (!cis_conn_handle || !p_link_quality_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.p_link_quality_info = p_link_quality_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_READ_LINK_QUALITY,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_read_link_quality_t));

}

uint16_t rtk_bt_le_iso_cig_initiator_add_cis(uint8_t cig_id, uint8_t cis_id)
{
	rtk_bt_le_iso_cig_cis_info_t param = {0};

	if (!cig_id || !cis_id) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cig_id = cig_id;
	param.cis_id = cis_id;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_ADD_CIS,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_cis_info_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_set_cig_param(rtk_bt_le_iso_cig_initiator_set_cig_param_t *param)
{
	if (!param) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIG_PARAM,
						   (void *)param, sizeof(rtk_bt_le_iso_cig_initiator_set_cig_param_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_set_cis_param(rtk_bt_le_iso_cig_initiator_set_cis_param_t *param)
{
	if (!param) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_PARAM,
						   (void *)param, sizeof(rtk_bt_le_iso_cig_initiator_set_cis_param_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_start_setting(uint8_t cig_id)
{
	if (!cig_id) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_START_SETTING,
						   (void *)&cig_id, sizeof(uint8_t));
}

extern uint16_t app_bt_le_iso_cis_id_availiable_check(uint8_t le_iso_role, uint8_t cis_id, uint8_t cig_id);

uint16_t rtk_bt_le_iso_cig_initiator_create_cis_by_cig_id(uint8_t cis_id, uint8_t cig_id, uint16_t conn_handle)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t param = {0};

	if (!cis_id || !cig_id || !conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	/* check whether cis_id and cig_id is legal */
	ret = app_bt_le_iso_cis_id_availiable_check(RTK_BLE_ISO_ROLE_CIS_INITIATOR, cis_id, cig_id);
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s app_bt_le_iso_cis_id_availiable_check fail 0x%x \r\n", __func__, ret);
		return RTK_BT_FAIL;
	}
	param.cis_id = cis_id;
	param.conn_handle = conn_handle;
	/* create cis by conn_handle */
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK,
						  (void *)&param, sizeof(rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t));
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK Fail 0x%x \r\n", __func__, ret);
	}
	/* config cis with cig */
	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_ID,
						   (void *)&cig_id, sizeof(uint8_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle(uint8_t cis_id, uint8_t cig_id, uint8_t cis_count, uint16_t conn_handle,
																   uint16_t *p_cis_conn_handle)
{
	uint16_t ret = RTK_BT_FAIL;
	rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t acl_param = {0};
	rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t param = {0};

	if (!cis_id || !cig_id || !cis_count || !conn_handle || !p_cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}
	acl_param.cis_id = cis_id;
	acl_param.conn_handle = conn_handle;
	/* create cis by conn_handle */
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK,
						  (void *)&acl_param, sizeof(rtk_bt_le_iso_cig_initiator_set_cis_acl_link_param_t));
	if (ret != RTK_BT_OK) {
		BT_LOGE("%s RTK_BT_LE_ISO_ACT_CIG_INITIATOR_SET_CIS_ACL_LINK Fail 0x%x \r\n", __func__, ret);
	}
	param.cig_id = cig_id;
	param.cis_count = cis_count;
	param.p_cis_conn_handle = p_cis_conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_CREATE_CIS_BY_CIG_CONN_HANDLE,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle_param_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_get_cis_conn_handle(uint8_t cis_id, uint16_t *p_cis_conn_handle)
{
	rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t param = {0};

	if (!cis_id || !p_cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_id = cis_id;
	param.p_cis_handle = p_cis_conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_GET_CIS_CONN_HANDLE,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_initiator_get_cis_handle_param_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_remove_cig(uint8_t cig_id)
{
	if (!cig_id) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REMOVE_CIG,
						   (void *)&cig_id, sizeof(uint8_t));
}

uint16_t rtk_bt_le_iso_cig_initiator_register_callback(uint8_t cig_id)
{
	if (!cig_id) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_INITIATOR_REGISTER_CALLBACK,
						   (void *)&cig_id, sizeof(uint8_t));
}

uint16_t rtk_bt_le_iso_cig_acceptor_accept_cis(uint16_t cis_conn_handle)
{
	if (!cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_ACCEPT_CIS,
						   (void *)&cis_conn_handle, sizeof(uint16_t));
}

uint16_t rtk_bt_le_iso_cig_acceptor_reject_cis(uint16_t cis_conn_handle, uint8_t reason)
{
	rtk_bt_le_iso_cig_acceptor_reject_cis_param_t param = {0};

	if (!cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.reason = reason;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REJECT_CIS,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_acceptor_reject_cis_param_t));
}

uint16_t rtk_bt_le_iso_cig_acceptor_config_sdu_param(uint16_t cis_conn_handle, bool acceptor_config_sdu_flag, uint32_t sdu_interval_m_s,
													 uint32_t sdu_interval_s_m, uint16_t max_sdu_m_s, uint16_t max_sdu_s_m)
{
	rtk_bt_le_iso_cig_acceptor_config_sdu_param_t param = {0};

	if (!cis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.cis_conn_handle = cis_conn_handle;
	param.acceptor_config_sdu_flag = acceptor_config_sdu_flag;
	param.sdu_interval_m_s = sdu_interval_m_s;
	param.sdu_interval_s_m = sdu_interval_s_m;
	param.max_sdu_m_s = max_sdu_m_s;
	param.max_sdu_s_m = max_sdu_s_m;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_SDU_PARAM,
						   (void *)&param, sizeof(rtk_bt_le_iso_cig_acceptor_config_sdu_param_t));
}

uint16_t rtk_bt_le_iso_cig_acceptor_config_cis_req_action(rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t cis_req_action)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_CONFIG_CIS_REQ_ACTION,
						   (void *)&cis_req_action, sizeof(rtk_bt_le_iso_cig_acceptor_config_cis_req_action_t));
}

uint16_t rtk_bt_le_iso_cig_acceptor_register_callback(void)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_CIG_ACCEPTOR_REGISTER_CALLBACK, NULL, NULL);
}

uint16_t rtk_bt_le_iso_big_broadcaster_init(uint8_t big_num, uint8_t bis_num)
{
	rtk_bt_le_iso_big_init_param_t param = {0};

	if (!big_num || !bis_num) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.big_num = big_num;
	param.bis_num = bis_num;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_INIT,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_init_param_t));
}

uint16_t rtk_bt_le_iso_big_broadcaster_create(uint8_t adv_handle, rtk_bt_le_iso_big_broadcaster_param_t *big_param, uint8_t *big_handle)
{
	rtk_bt_le_iso_big_broadcaster_create_param_t param = {0};

	if (!big_param || !big_handle) { //big_param is NULL or big handle is NULL
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.adv_handle = adv_handle;
	param.p_big_param = big_param;
	param.big_handle = big_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_CREATE,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_broadcaster_create_param_t));
}

uint16_t rtk_bt_le_iso_big_broadcaster_terminate(uint8_t big_handle)
{
	if (!big_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_TERMINATE,
						   (void *)&big_handle, sizeof(uint8_t));
}

uint16_t rtk_bt_le_iso_big_broadcaster_read_tx_sync(uint16_t bis_conn_handle, rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t *p_tx_sync_info)
{
	rtk_bt_le_iso_big_broadcaster_read_tx_sync_t param = {0};

	if (!bis_conn_handle || !p_tx_sync_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.bis_conn_handle = bis_conn_handle;
	param.p_tx_sync_info = p_tx_sync_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_BROADCASTER_READ_TX_SYNC,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_broadcaster_read_tx_sync_t));
}

uint16_t rtk_bt_le_iso_big_receiver_init(uint8_t big_num, uint8_t bis_num)
{
	rtk_bt_le_iso_big_init_param_t param = {0};

	if (!big_num || !bis_num) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.big_num = big_num;
	param.bis_num = bis_num;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_RECEIVER_INIT,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_init_param_t));
}

uint16_t rtk_bt_le_iso_big_receiver_create_sync(uint16_t sync_handle, rtk_bt_le_iso_big_receiver_create_sync_param_t *sync_param, uint8_t *p_big_handle)
{
	rtk_bt_le_iso_big_receiver_create_sync_t param = {0};

	if (!sync_handle || !sync_param || !p_big_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.sync_handle = sync_handle;
	param.p_sync_param = sync_param;
	param.p_big_handle = p_big_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_RECEIVER_CREATE_SYNC,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_receiver_create_sync_t));
}

uint16_t rtk_bt_le_iso_big_receiver_terminate_sync(uint8_t big_handle)
{
	if (!big_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_RECEIVER_TERMINATE_SYNC,
						   (void *)&big_handle, sizeof(uint8_t));
}

uint16_t rtk_bt_le_iso_big_receiver_read_iso_link_quality(uint16_t bis_conn_handle, rtk_bt_le_iso_big_receiver_read_link_quality_info_t *p_link_quality_info)
{
	rtk_bt_le_iso_big_receiver_read_link_quality_t param = {0};

	if (!bis_conn_handle || !p_link_quality_info) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.bis_conn_handle = bis_conn_handle;
	param.p_link_quality_info = p_link_quality_info;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_RECEIVER_READ_LINK_QUALITY,
						   (void *)&param, sizeof(rtk_bt_le_iso_big_receiver_read_link_quality_t));
}

uint16_t rtk_bt_le_iso_big_setup_path(rtk_bt_le_iso_setup_path_param_t *param)
{
	if (!param) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_SETUP_PATH,
						   (void *)param, sizeof(rtk_bt_le_iso_setup_path_param_t));
}
uint16_t rtk_bt_le_iso_big_remove_path(uint16_t bis_conn_handle, rtk_bt_le_iso_data_path_direction_flag_t data_path_direction)
{
	rtk_bt_le_iso_remove_path_param_t param = {0};

	if (!bis_conn_handle) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	param.iso_conn_handle = bis_conn_handle;
	param.data_path_direction = data_path_direction;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_BIG_REMOVE_PATH,
						   (void *)&param, sizeof(rtk_bt_le_iso_remove_path_param_t));
}

uint16_t rtk_bt_le_iso_data_send(rtk_bt_le_iso_data_send_info_t *info)
{
	if (!info || !info->iso_conn_handle || !info->data_len || !info->p_data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return rtk_bt_send_cmd(RTK_BT_LE_GP_ISO, RTK_BT_LE_ISO_ACT_ISO_DATA_SEND,
						   (void *)info, sizeof(rtk_bt_le_iso_data_send_info_t));
}

#endif //end of #if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT