/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
#include <stdio.h>
#include <string.h>
#include <rtk_bt_device.h>
#include <rtk_bt_pbp.h>
#include <app_bt_le_audio_common.h>

/* pbp broadcast sink param */
extern app_bt_le_audio_broadcast_sink_info_t g_pbp_bsink_info;

uint16_t rtk_bt_pbp_broadcast_sink_cfg(uint8_t sound_channel)
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
	uint8_t device_name_len = strlen((char *)g_pbp_bsink_info.device_name);
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
	g_pbp_bsink_info.device_name[device_name_len - 1] = ch;
	g_pbp_bsink_info.sound_channel = sound_channel;

	return ret;
}

#endif