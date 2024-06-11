/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if (defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT)
#include <stdio.h>
#include <string.h>
#include <rtk_bt_gmap.h>
#include <rtk_bt_device.h>
#include <app_bt_le_audio_common.h>

extern app_bt_le_audio_acceptor_info_t g_gmap_bgr_info;
extern app_bt_le_audio_acceptor_info_t g_gmap_ugt_info;

uint16_t rtk_bt_le_audio_gmas_client_read_role(uint16_t conn_handle)
{
	uint16_t info = 0;
	info = conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GMAS_READ_ROLE,
						   (void *)&info, sizeof(uint16_t));
}

uint16_t rtk_bt_le_audio_gmas_client_read_features(uint16_t conn_handle, uint8_t gmap_role)
{
	rtk_bt_le_audio_gmas_client_read_role_result_t info = {0};

	info.conn_handle = conn_handle;
	info.gmap_role = gmap_role;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_GMAS_READ_FEATURES,
						   (void *)&info, sizeof(rtk_bt_le_audio_gmas_client_read_role_result_t));
}

uint16_t rtk_bt_gmap_broadcast_game_receiver_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	char ch = 0;
	uint16_t ret = RTK_BT_OK;
	/* should only be invoked before BT enable */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: wrong channel(%d) should be within [1-3] \r\n", __func__, channel);
		return RTK_BT_FAIL;
	}
	//update device name
	uint8_t device_name_len = strlen((char *)g_gmap_bgr_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("device_name_len(%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
		return RTK_BT_FAIL;
	}
	switch (channel) {
	case RTK_BT_LE_AUDIO_LEFT:
		ch = 'L';
		break;
	case RTK_BT_LE_AUDIO_RIGHT:
		ch = 'R';
		break;
	case RTK_BT_LE_AUDIO_STEREO:
		ch = 'S';
		break;
	default:
		BT_LOGE("%s: wrong sound channel \r\n", __func__);
		break;
	}
	g_gmap_bgr_info.device_name[device_name_len - 1] = ch;
	g_gmap_bgr_info.sound_channel = channel;

	if (p_neighbor_addr) {
		memcpy((void *)&g_gmap_bgr_info.csis_neighbor_addr, p_neighbor_addr, 6);
	}

	return ret;
}

uint16_t rtk_bt_gmap_unicast_game_terminal_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	char ch = 0;
	uint16_t ret = RTK_BT_OK;
	/* should only be invoked before BT enable */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: wrong channel(%d) should be within [1-3] \r\n", __func__, channel);
		return RTK_BT_FAIL;
	}
	//update device name
	uint8_t device_name_len = strlen((char *)g_gmap_ugt_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("device_name_len(%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
		return RTK_BT_FAIL;
	}
	switch (channel) {
	case RTK_BT_LE_AUDIO_LEFT:
		ch = 'L';
		break;
	case RTK_BT_LE_AUDIO_RIGHT:
		ch = 'R';
		break;
	case RTK_BT_LE_AUDIO_STEREO:
		ch = 'S';
		break;
	default:
		BT_LOGE("%s: wrong sound channel \r\n", __func__);
		break;
	}
	g_gmap_ugt_info.device_name[device_name_len - 1] = ch;
	g_gmap_ugt_info.sound_channel = channel;

	if (p_neighbor_addr) {
		memcpy((void *)&g_gmap_ugt_info.csis_neighbor_addr, p_neighbor_addr, 6);
	}

	return ret;
}

#endif