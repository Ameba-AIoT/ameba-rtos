/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
#include <rtk_bt_common.h>
#include <rtk_bt_gmap.h>

uint16_t rtk_bt_le_audio_gmas_client_read_role(uint16_t conn_handle)
{
	return rtk_bt_send_cmd(RTK_BT_LE_GP_GMAP, RTK_BT_LE_AUDIO_GMAS_ACT_READ_ROLE,
						   (void *)&conn_handle, sizeof(uint16_t));
}

uint16_t rtk_bt_le_audio_gmas_client_read_features(uint16_t conn_handle, uint16_t char_uuid)
{
	rtk_bt_le_audio_gmas_client_read_features_t info = {0};

	info.conn_handle = conn_handle;
	info.char_uuid = char_uuid;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_GMAP, RTK_BT_LE_AUDIO_GMAS_ACT_READ_FEATURES,
						   (void *)&info, sizeof(rtk_bt_le_audio_gmas_client_read_features_t));
}
#endif