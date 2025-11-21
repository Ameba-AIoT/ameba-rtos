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
#ifdef CONFIG_RMESH_EN
#include "wifi_api.h"
#include "wifi_intf_drv_to_upper.h"
#include "dhcp/dhcps.h"
#include "wtn_app_zrpp.h"

#define CHANNEL_SCAN_TIME_MS 200
#define ZRPP_SCAN_TIMEOUT_MS 300000
#define ZRPP_TASK_STACK_SIZE_BYTES 2048
#define ZRPP_TASK_PRIORITY 4

rtos_sema_t zrpp_scan_sema = NULL;
struct rtw_wifi_setting *zrpp_ap_info = NULL;
enum zrpp_status_code zrpp_status = ZRPP_STATUS_START;
enum zrpp_scan_control_code zrpp_scan_control = ZRPP_CONTROL_ALLOW;

void wtn_zrpp_get_ap_info_evt_hdl(u8 *evt_info)
{
	if (zrpp_ap_info && zrpp_scan_sema) {
		memcpy(zrpp_ap_info, evt_info, sizeof(struct rtw_wifi_setting));
		rtos_sema_give(zrpp_scan_sema);
	}
}

static int wtn_zrpp_scan_and_req_ap_info(u16 chan_scan_time_ms)
{
	struct rtw_scan_param scan_param;
	int ret = RTK_FAIL;
	u32 start_time;

	memset(&scan_param, 0, sizeof(struct rtw_scan_param));
	scan_param.options = RTW_SCAN_FOR_ZRPP;
	scan_param.chan_scan_time.passive_scan_time = chan_scan_time_ms;

	rtos_sema_create_static(&zrpp_scan_sema, 0, 0xFFFFFFFF);
	if (zrpp_scan_sema == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit;
	}

	start_time = rtos_time_get_current_system_time_ms();
	while (rtos_time_get_current_system_time_ms() - start_time < ZRPP_SCAN_TIMEOUT_MS) {

		if (zrpp_scan_control == ZRPP_CONTROL_PAUSE) {
			rtos_time_delay_ms(1000);
			continue;
		}

		if (zrpp_scan_control == ZRPP_CONTROL_STOP) {
			break;
		}

		zrpp_status = ZRPP_STATUS_SCAN_ONGOING;
		wifi_scan_networks(&scan_param, 1);

		if (rtos_sema_take(zrpp_scan_sema, 3000) == RTK_SUCCESS) {
			zrpp_status = ZRPP_STATUS_GET_AP_INFO;
			ret = RTK_SUCCESS;
			break;
		} else {
			zrpp_status = ZRPP_STATUS_START;
		}
	}

exit:
	if (zrpp_scan_sema) {
		rtos_sema_delete_static(zrpp_scan_sema);
	}
	return ret;
}

int wtn_zrpp_sync_state_with_ble_config(enum zrpp_scan_control_code zrpp_control_op)
{
	int ret = RTK_SUCCESS;
	switch (zrpp_control_op) {
	case ZRPP_CONTROL_PAUSE:
		if (zrpp_status == ZRPP_STATUS_GET_AP_INFO) {
			ret = RTK_FAIL;
		} else {
			zrpp_scan_control = ZRPP_CONTROL_PAUSE;
			while (zrpp_status == ZRPP_STATUS_SCAN_ONGOING) {
				wifi_scan_abort();
				rtos_time_delay_ms(100);
			}
		}
		break;
	case ZRPP_CONTROL_ALLOW:
		if (zrpp_scan_control == ZRPP_CONTROL_PAUSE) {
			zrpp_scan_control = ZRPP_CONTROL_ALLOW;
		}
		break;
	case ZRPP_CONTROL_STOP:
		zrpp_scan_control = ZRPP_CONTROL_STOP;
		break;
	default:
		break;
	}

	return ret;
}

void wtn_zrpp_task(void *param)
{
	UNUSED(param);
	struct rtw_network_info connect_param = {0};
	int ret = RTK_FAIL;

	zrpp_ap_info = rtos_mem_zmalloc(sizeof(struct rtw_wifi_setting));

	/*step 1: scan to get AP info*/
	ret = wtn_zrpp_scan_and_req_ap_info(CHANNEL_SCAN_TIME_MS);

	if (ret != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "ZRPP scan failed to get AP info\n");
		goto exit;
	}

	/*step 2: use wifi ssid and password to connect*/
	connect_param.ssid.len = strlen((char *)zrpp_ap_info->ssid);
	strcpy((char *)connect_param.ssid.val, (char *)zrpp_ap_info->ssid);
	connect_param.password = zrpp_ap_info->password;
	connect_param.password_len = strlen((char *)zrpp_ap_info->password);
	connect_param.channel = zrpp_ap_info->channel;
	connect_param.key_id = zrpp_ap_info->key_idx;
	connect_param.security_type = zrpp_ap_info->security_type;
	ret = wifi_connect(&connect_param, 1);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	/*step 3: get IP address using DHCP*/
#ifdef CONFIG_LWIP_LAYER
	if (DHCP_ADDRESS_ASSIGNED != LwIP_IP_Address_Request(NETIF_WLAN_STA_INDEX)) {
		RTK_LOGE(NOTAG, "ZRPP get IP failed\n");
		goto exit;
	}
#endif

exit:
	rtos_mem_free(zrpp_ap_info);
	rtos_task_delete(NULL);
}

void wtn_zrpp_start(void)
{
	if (rtos_task_create(NULL, ((const char *)"zrpp_task"), wtn_zrpp_task, NULL, ZRPP_TASK_STACK_SIZE_BYTES, ZRPP_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGE(NOTAG, "zrpp task create fail\n");
	}
}

#endif