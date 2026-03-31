/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_gmap.h>
#include <atcmd_bt_impl.h>

void fBLEGMAP(u16 argc, char *argv[])
{
	(void)argc;
	(void)argv;
	BT_LOGE("There is no available supported bt gmap at cmd \r\n");
#if defined(CONFIG_ATCMD_HOST_CONTROL) && CONFIG_ATCMD_HOST_CONTROL
	BT_AT_PRINTERROR(BT_AT_ERR_CMD_INVALID);
#endif
}
#endif