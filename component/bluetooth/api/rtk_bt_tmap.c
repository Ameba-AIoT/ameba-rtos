/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if (defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT)
#include <stdio.h>
#include <string.h>
#include <rtk_bt_device.h>
#include <rtk_bt_tmap.h>
#include <app_bt_le_audio_common.h>

/* tmap broacast media receiver param */
extern app_bt_le_audio_acceptor_info_t g_tmap_bmr_info;
extern app_bt_le_audio_acceptor_info_t g_tmap_umr_info;

uint16_t rtk_bt_le_audio_tmas_client_read_role(uint16_t conn_handle)
{
	uint16_t info = 0;
	info = conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_AUDIO, RTK_BT_LE_AUDIO_ACT_TMAS_READ_ROLE,
						   (void *)&info, sizeof(uint16_t));
}

uint16_t rtk_bt_tmap_broadcast_media_receiver_cfg(uint8_t sound_channel, uint8_t *p_neighbor_addr)
{
	char ch = 0;
	uint16_t ret = RTK_BT_OK;
	/* should only be invoked before BT enable */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}
	if (sound_channel < 1 || sound_channel > 3) {
		BT_LOGE("%s: wrong channel(%d) should be within [1-3] \r\n", __func__, sound_channel);
		return RTK_BT_FAIL;
	}
	//update device name
	uint8_t device_name_len = strlen((char *)g_tmap_bmr_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("device_name_len(%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
	}
	if (sound_channel == RTK_BT_LE_AUDIO_LEFT) {
		ch = 'L';
	} else if (sound_channel == RTK_BT_LE_AUDIO_RIGHT) {
		ch = 'R';
	} else if (sound_channel == RTK_BT_LE_AUDIO_STEREO) {
		ch = 'S';
	} else {
		ch = 'L';
		BT_LOGE("%s: wrong sound channel \r\n", __func__);
	}
	g_tmap_bmr_info.device_name[device_name_len - 1] = ch;
	g_tmap_bmr_info.sound_channel = sound_channel;

	if (p_neighbor_addr) {
		memcpy((void *)&g_tmap_bmr_info.csis_neighbor_addr, p_neighbor_addr, 6);
	}

	return ret;
}

uint16_t rtk_bt_tmap_unicast_media_receiver_cfg(uint8_t channel, uint8_t *p_neighbor_addr)
{
	char ch = 0;
	uint8_t device_name_len = 0;
	/* Check bt enable status (rtk_bt_tmap_unicast_media_receiver_cfg should only be invoked before RTK_BT_IS_ENABLED()) */
	if (rtk_bt_is_enable()) {
		BT_LOGE("%s: should not be invoked after bt enable \r\n", __func__);
		return RTK_BT_FAIL;
	}

	if (channel < 1 || channel > 3) {
		BT_LOGE("%s: channel should be within 1 - 3 \r\n", __func__);
		return RTK_BT_FAIL;
	}
	device_name_len = strlen((char *)g_tmap_umr_info.device_name);
	if (device_name_len > RTK_BT_GAP_DEVICE_NAME_LEN) {
		BT_LOGE("%s: device_name_len (%d) > RTK_BT_GAP_DEVICE_NAME_LEN(%d)\r\n", __func__, device_name_len, RTK_BT_GAP_DEVICE_NAME_LEN);
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
		BT_LOGE("%s: unknown channel allocation \r\n", __func__);
		break;
	}
	g_tmap_umr_info.device_name[device_name_len - 1] = ch;
	g_tmap_umr_info.sound_channel = channel;
	if (p_neighbor_addr) {
		memcpy((void *)&g_tmap_umr_info.csis_neighbor_addr, p_neighbor_addr, 6);
	}

	return RTK_BT_OK;
}
#endif