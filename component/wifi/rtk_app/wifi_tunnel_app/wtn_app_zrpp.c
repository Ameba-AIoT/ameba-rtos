/**
  ******************************************************************************
  * @file    wtn_app_zrpp.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#define __WTN_APP_ZRPP_C__


/* -------------------------------- Includes -------------------------------- */
#include "rtw_autoconf.h"
#ifdef CONFIG_WIFI_TUNNEL
#include "wifi_api.h"
#include "wifi_intf_drv_to_upper.h"
#include "dhcp/dhcps.h"

#define CHANNEL_SCAN_TIME_MS 200
#define ZRPP_SCAN_TIMEOUT_MS 300000
#define ZRPP_TASK_STACK_SIZE_BYTES 2048
#define ZRPP_TASK_PRIORITY 1

static int wtn_zrpp_scan_and_req_ap_info(u16 chan_scan_time_ms)
{
	struct rtw_scan_param scan_param;
	int ret = RTK_SUCCESS;

	memset(&scan_param, 0, sizeof(struct rtw_scan_param));
	scan_param.options = RTW_SCAN_FOR_ZRPP;
	scan_param.chan_scan_time.passive_scan_time = chan_scan_time_ms;

	wifi_scan_networks(&scan_param, 1);

	return ret;
}

void wtn_zrpp_task(void *param)
{
	UNUSED(param);
	u32 start_time, cur_time;
	int ret;

	/*step 1: scan to get helper info*/

	start_time = rtos_time_get_current_system_time_ms();
	while (1) {
		ret = wtn_zrpp_scan_and_req_ap_info(CHANNEL_SCAN_TIME_MS);
		cur_time = rtos_time_get_current_system_time_ms();
		if (ret == RTK_SUCCESS || cur_time - start_time > ZRPP_SCAN_TIMEOUT_MS) {
			break;
		}
	}

	rtos_task_delete(NULL);
}

void wtn_zrpp_start(void)
{
	if (rtos_task_create(NULL, ((const char *)"zrpp_task"), wtn_zrpp_task, NULL, ZRPP_TASK_STACK_SIZE_BYTES, ZRPP_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "zrpp task create fail\n");
	}
}

#endif