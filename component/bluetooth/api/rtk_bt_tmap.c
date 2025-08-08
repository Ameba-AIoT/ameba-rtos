/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
#include <stdio.h>
#include <string.h>
#include <rtk_bt_device.h>
#include <rtk_bt_tmap.h>

uint16_t rtk_bt_le_audio_tmas_client_read_role(uint16_t conn_handle)
{
	uint16_t info = 0;
	info = conn_handle;

	return rtk_bt_send_cmd(RTK_BT_LE_GP_TMAP, RTK_BT_LE_AUDIO_ACT_TMAS_READ_ROLE,
						   (void *)&info, sizeof(uint16_t));
}

#endif