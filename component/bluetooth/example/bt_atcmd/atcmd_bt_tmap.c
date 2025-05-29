/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if (defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_tmap.h>
#include <atcmd_bt_impl.h>

int atcmd_bt_tmap_cmd(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	BT_LOGE("There is no available supported bt tmap at cmd \r\n");

	return 0;
}
#endif