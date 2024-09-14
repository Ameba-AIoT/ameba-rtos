/**
  ******************************************************************************
  * @file    wifi_auto_reconnect.c
  * @author
  * @version
  * @date
  * @brief   This example demonstrate how to implement wifi auto reconnection
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include <platform_autoconf.h>
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include <wifi_auto_reconnect.h>
#include "os_wrapper.h"
#include <rtw_wakelock.h>
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#include "ethernetif.h"
#endif
#include "ameba_soc.h"

#ifdef CONFIG_ENABLE_EAP
extern void eap_autoreconnect_hdl(u8 method_id);
#endif

#if CONFIG_AUTO_RECONNECT
struct rtw_auto_reconn_t  rtw_reconn;

void rtw_reconn_join_status_hdl(char *buf, int flags)
{
	enum rtw_join_status_type join_status = (enum rtw_join_status_type)flags;
	static enum rtw_join_status_type join_status_last = RTW_JOINSTATUS_SUCCESS;
	u16 disconn_reason = 0;
	u8 need_reconn = 0;

	if ((join_status_last == join_status) && (join_status_last > RTW_JOINSTATUS_4WAY_HANDSHAKING)) {
		RTK_LOGS(NOTAG, "same joinstaus: %d\n", join_status);/*just for debug, delete when stable*/
	}
	join_status_last = join_status;

	if (rtw_reconn.b_enable == 0) {
		return;
	}

	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		rtw_reconn.cnt = 0;
		return;
	}

	if (join_status == RTW_JOINSTATUS_FAIL) {
		need_reconn = 1;
	}

	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconn_reason = ((struct rtw_event_disconn_info_t *)buf)->disconn_reason;
		if (!(disconn_reason > WLAN_REASON_APP_BASE && disconn_reason < WLAN_REASON_APP_BASE_END)) {/*disconnect by APP no need do reconnect*/
			need_reconn = 1;
		}
	}

	if (need_reconn == 0) {
		return;
	}

#ifdef CONFIG_ENABLE_EAP
	if (rtw_reconn.eap_method != 0) {
		RTK_LOGS(NOTAG, "auto reconn\n");
		eap_autoreconnect_hdl(rtw_reconn.eap_method);
		return;
	}
#endif

	if (rtw_reconn.b_waiting) {
		RTK_LOGS(NOTAG, "auto reconn ongoing\n");
		return;
	}

	rtw_reconn.cnt = rtw_reconn.b_infinite ? 0 : (rtw_reconn.cnt + 1);
	if (rtw_reconn.cnt > wifi_user_config.auto_reconnect_count) {
		RTK_LOGS(NOTAG, "auto reconn max times\n");
	} else {
		rtw_reconn.b_waiting = 1;
		rtw_wakelock_timeout(wifi_user_config.auto_reconnect_interval * 1000 + 10);
		rtos_timer_start(rtw_reconn.timer, 1000);
	}
}

void rtw_reconn_task_hdl(void *param)
{
	(void) param;
	int ret = RTW_ERROR;

	ret = wifi_connect(&rtw_reconn.conn_param, 1);
	if (ret != RTW_SUCCESS) {
		RTK_LOGS(NOTAG, "reconn fail:%d", ret);
		if ((ret == RTW_CONNECT_INVALID_KEY)) {
			RTK_LOGS(NOTAG, "(password format wrong)");
		} else if (ret == RTW_CONNECT_SCAN_FAIL) {
			RTK_LOGS(NOTAG, "(not found AP)");
		} else if (ret == RTW_BUSY) {
			RTK_LOGS(NOTAG, "(busy)");
		}
		RTK_LOGS(NOTAG, "\r\n");
	}

#ifdef CONFIG_LWIP_LAYER
	if (ret == RTW_SUCCESS) {
		LwIP_DHCP(0, DHCP_START);
	}
#endif
	rtos_task_delete(NULL);
}

void rtw_reconn_timer_hdl(rtos_timer_t timer_hdl)
{
	(void) timer_hdl;

	rtw_reconn.b_waiting = 0;
	/*Creat a task to do wifi reconnect because call WIFI API in WIFI event is not safe*/
	if (rtos_task_create(NULL, ((const char *)"rtw_reconn_task_hdl"), rtw_reconn_task_hdl, NULL, WIFI_STACK_SIZE_AUTO_RECONN_TASKLET, 6) != SUCCESS) {
		RTK_LOGS(NOTAG, "Create reconn task failed\n");
	} else {
		RTK_LOGS(NOTAG, "auto reconn %d\n", rtw_reconn.cnt);
	}
}

void rtw_reconn_new_conn(rtw_network_info_t *connect_param)
{
	if ((connect_param->by_reconn == 0) && (rtw_reconn.b_enable)) { /*a new wifi connect*/
		memcpy(&rtw_reconn.conn_param, connect_param, sizeof(rtw_network_info_t));
		/*fix auto reconnect fail: https://jira.realtek.com/browse/RSWLANDIOT-9031*/
		rtw_reconn.conn_param.channel = 0;
		rtw_reconn.conn_param.pscan_option = 0;

		if (connect_param->password_len) {
			memcpy(&rtw_reconn.pwd, connect_param->password, connect_param->password_len);
			rtw_reconn.pwd[connect_param->password_len] = '\0';
			rtw_reconn.conn_param.password = rtw_reconn.pwd;
		} else {
			rtw_reconn.conn_param.password = NULL;
		}
		rtw_reconn.conn_param.by_reconn = 1;
		rtos_timer_stop(rtw_reconn.timer, 1000);/*cancel ongoing reconnect*/
		rtw_reconn.b_waiting = 0;
		rtw_reconn.cnt = 0;
	}
}

int wifi_config_autoreconnect(__u8 mode)
{
	if ((mode == RTW_AUTORECONNECT_DISABLE) && rtw_reconn.b_enable) {
		rtos_timer_stop(rtw_reconn.timer, 1000);
		rtos_timer_delete(rtw_reconn.timer, 1000);
		rtw_reconn.timer = NULL;
		rtw_reconn.b_waiting = 0;
		rtw_reconn.b_enable = 0;
	} else if ((mode != RTW_AUTORECONNECT_DISABLE) && (rtw_reconn.b_enable == 0))  {
		if (rtos_timer_create(&(rtw_reconn.timer), "rtw_reconn_timer", NULL, wifi_user_config.auto_reconnect_interval * 1000, _FALSE,
							  rtw_reconn_timer_hdl) != SUCCESS) {
			RTK_LOGS(NOTAG, "rtw_reconn_timer create fail\n");
			return RTW_ERROR;
		}
		rtw_reconn.b_enable = 1;
		rtw_reconn.cnt = 0;
	}

	if (mode == RTW_AUTORECONNECT_INFINITE) {
		rtw_reconn.b_infinite = 1;
	}

	return RTW_SUCCESS;
}

int wifi_get_autoreconnect(__u8 *mode)
{
	if (mode == NULL) {
		return RTW_ERROR;
	} else {
		*mode = rtw_reconn.b_enable;
		return RTW_SUCCESS;
	}
}

#endif
