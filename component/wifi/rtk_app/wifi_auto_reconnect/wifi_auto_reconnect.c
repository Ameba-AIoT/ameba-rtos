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
#include "atcmd_service.h"
#include "wifi_api.h"
#include <wifi_auto_reconnect.h>
#include "os_wrapper.h"
#include <rtw_wakelock.h>
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#include "ethernetif.h"
#endif
#include "ameba_soc.h"

extern void eap_autoreconnect_hdl(u8 method_id);

#if CONFIG_AUTO_RECONNECT
struct rtw_auto_reconn_t  rtw_reconn;

void rtw_reconn_join_status_hdl(u8 *buf, s32 flags)
{
	u8 join_status = (u8)flags;
	static u8 join_status_last = RTW_JOINSTATUS_SUCCESS;
	int disconn_reason = -1;
	u8 need_reconn = 0;
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;
	struct rtw_event_join_fail *join_fail;
	struct rtw_event_disconnect *disconnect;

	if ((join_status_last == join_status) && (join_status_last > RTW_JOINSTATUS_4WAY_HANDSHAKING)) {
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "same joinstaus: %d\n", join_status);/*just for debug, delete when stable*/
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
		join_fail = &evt_info->private.fail;
		disconn_reason = join_fail->reason_or_status_code;
	} else if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconnect = &evt_info->private.disconnect;
		disconn_reason = disconnect->disconn_reason;
	}

	if (disconn_reason >= 0 && !(disconn_reason > RTW_DISCONN_RSN_APP_BASE &&
								 disconn_reason < RTW_DISCONN_RSN_APP_BASE_END)) {/*disconnect by APP no need do reconnect*/
		need_reconn = 1;
	}

	if (need_reconn == 0) {
		return;
	}

	if (rtw_reconn.eap_method != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "eap auto reconn\n");
		eap_autoreconnect_hdl(rtw_reconn.eap_method);
		return;
	}

	if (rtw_reconn.b_waiting) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "auto reconn ongoing\n");
		return;
	}

	rtw_reconn.cnt = rtw_reconn.b_infinite ? 0 : (rtw_reconn.cnt + 1);

	if (rtw_reconn.cnt > wifi_user_config.auto_reconnect_count) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "auto reconn max times\n");
		at_printf_indicate("wifi reconnect done\r\n");
	} else {
		rtw_reconn.b_waiting = 1;
		rtw_wakelock_timeout(wifi_user_config.auto_reconnect_interval * 1000 + 10);
		rtos_timer_start(rtw_reconn.timer, 1000);
	}
}

void rtw_reconn_task_hdl(void *param)
{
	(void) param;
	int ret = RTK_FAIL;

	ret = wifi_connect(&rtw_reconn.conn_param, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "reconn fail:-0x%x", -ret);
		if ((ret == -RTK_ERR_WIFI_CONN_INVALID_KEY)) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "(password format wrong)");
		} else if (ret == -RTK_ERR_WIFI_CONN_SCAN_FAIL) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "(not found AP)");
		} else if (ret == -RTK_ERR_BUSY) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "(busy)");
		}
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n");
	}

#ifdef CONFIG_LWIP_LAYER
	if (ret == RTK_SUCCESS) {
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
	if (rtos_task_create(NULL, ((const char *)"rtw_reconn_task_hdl"), rtw_reconn_task_hdl, NULL, WIFI_STACK_SIZE_AUTO_RECONN_TASKLET, 6) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Create reconn task failed\n");
	} else {
		if (rtw_reconn.b_infinite) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "auto reconn infinite\n");
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "auto reconn %d\n", rtw_reconn.cnt);
		}
		at_printf_indicate("wifi reconnecting\r\n");
	}
}

void rtw_reconn_new_conn(struct rtw_network_info *connect_param)
{
	if ((connect_param->by_reconn == 0) && (rtw_reconn.b_enable)) { /*a new wifi connect*/
		memcpy(&rtw_reconn.conn_param, connect_param, sizeof(struct rtw_network_info));
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

int wifi_stop_autoreconnect(void)
{
	if (rtw_reconn.b_enable) {
		rtos_timer_stop(rtw_reconn.timer, 1000);
		rtw_reconn.b_waiting = 0;
		rtw_reconn.cnt = 0;
	}
	return RTK_SUCCESS;
}

#endif

s32 wifi_set_autoreconnect(u8 enable)
{
#if CONFIG_AUTO_RECONNECT
	if ((enable == 0) && rtw_reconn.b_enable) {
		rtos_timer_stop(rtw_reconn.timer, 1000);
		rtos_timer_delete(rtw_reconn.timer, 1000);
		rtw_reconn.timer = NULL;
		rtw_reconn.b_waiting = 0;
		rtw_reconn.b_enable = 0;
	} else if ((enable != 0) && (rtw_reconn.b_enable == 0)) {
		if (rtos_timer_create(&(rtw_reconn.timer), "rtw_reconn_timer", NULL, wifi_user_config.auto_reconnect_interval * 1000, FALSE,
							  rtw_reconn_timer_hdl) != RTK_SUCCESS) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rtw_reconn_timer create fail\n");
			return RTK_FAIL;
		}
		rtw_reconn.b_enable = 1;
		rtw_reconn.cnt = 0;
	}

	rtw_reconn.b_infinite = (wifi_user_config.auto_reconnect_count == 0xff) ? 1 : 0;

	return RTK_SUCCESS;
#else
	UNUSED(enable);
	return RTK_FAIL;
#endif
}

s32 wifi_get_autoreconnect(u8 *enable)
{
#if CONFIG_AUTO_RECONNECT
	if (enable == NULL) {
		return RTK_FAIL;
	} else {
		*enable = rtw_reconn.b_enable;
		return RTK_SUCCESS;
	}
#else
	*enable = 0;
	return RTK_FAIL;
#endif
}
