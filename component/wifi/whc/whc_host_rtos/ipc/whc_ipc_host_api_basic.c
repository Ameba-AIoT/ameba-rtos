/**
  ******************************************************************************
  * @file    whc_ipc_host_api_basic.c
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

#include "rtw_inic_common.h"
#ifndef ZEPHYR_WIFI
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif
#ifdef CONFIG_P2P
#include "wifi_wps_config.h"
#include "wifi_p2p_supplicant.h"
#endif
#include <os_wrapper.h>
#include <rtw_timer.h>
#include <rtw_wakelock.h>

#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>
#include "wifi_fast_connect.h"
#endif

/******************************************************
 *               Variables Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
struct internal_block_param *join_block_param = NULL;
struct internal_block_param *scan_block_param = NULL;

s32(*scan_user_callback_ptr)(u32, void *) = NULL;
s32(*scan_each_report_user_callback_ptr)(struct rtw_scan_result *, void *, u8 *, u32) = NULL;
s32(*scan_acs_report_user_callback_ptr)(struct rtw_acs_mntr_rpt *acs_mntr_rpt) = NULL;

u8(*promisc_user_callback_ptr)(struct rtw_rx_pkt_info *pkt_info) = NULL;

extern void *param_indicator;
u8 rtw_join_status = RTW_JOINSTATUS_UNKNOWN;
int join_fail_reason = RTK_SUCCESS;
u8 rtw_scan_api_inprocess = 0;
u16 scanned_ap_cnt = 0;

int (*p_wifi_do_fast_connect)(void) = NULL;
int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2) = NULL;
void (*p_wifi_join_info_free)(u8 iface_type) = NULL;
/* Give default value if not defined */
/******************************************************
 *               Function Definitions
 ******************************************************/
#if CONFIG_WLAN
//----------------------------------------------------------------------------//
s32 wifi_connect(struct rtw_network_info *connect_param, u8 block)
{
	int result = RTK_SUCCESS;
	struct internal_block_param *block_param = NULL;
	u32 param_buf[2];
	u8 no_need_indicate = 0;
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_network_info *connect_param_temp = NULL;
	struct rtw_event_join_status_info join_status_info;
	struct rtw_event_join_fail *join_fail;
	unsigned int timeout;
	u8 eap_phase = 0;

	/* check if SoftAP is running */
	if ((wifi_user_config.concurrent_enabled == FALSE) && wifi_is_running(SOFTAP_WLAN_INDEX)) {
		RTK_LOGE(TAG_WLAN_INIC, "ap running, please set concurrent_enabled in wifi_set_user_config\n");
		return RTK_FAIL;
	}

	if (connect_param == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "connect param not set!\n");
		return RTK_FAIL;
	}

	/* step1: check if there's ongoing connect*/
	if ((rtw_join_status > RTW_JOINSTATUS_UNKNOWN) && (rtw_join_status < RTW_JOINSTATUS_SUCCESS)) {
		RTK_LOGD(TAG_WLAN_INIC, "on wifi connect\n");
		return -RTK_ERR_BUSY;
	}

	/*clear for last connect status */
	rtw_join_status = RTW_JOINSTATUS_STARTING;
	memset(&join_status_info, 0, sizeof(struct rtw_event_join_status_info));
	join_status_info.status = RTW_JOINSTATUS_STARTING;
	wifi_indication(RTW_EVENT_JOIN_STATUS, (u8 *)&join_status_info, sizeof(struct rtw_event_join_status_info));

	/* step2: malloc and set synchronous connection related variables*/
	if (block) {
		block_param = (struct internal_block_param *)rtos_mem_zmalloc(sizeof(struct internal_block_param));
		if (!block_param) {
			result = -RTK_ERR_NOMEM;
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}
		rtos_sema_create_static(&block_param->sema, 0, 0xFFFFFFFF);
		if (!block_param->sema) {
			result = -RTK_ERR_NOMEM;
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}

	}

#if CONFIG_AUTO_RECONNECT
	rtw_reconn_new_conn(connect_param);/*auto reconn backup connnect parameters*/
#endif

	/* step3: set connect cmd to driver*/
	size = sizeof(struct rtw_network_info) + connect_param->password_len;
	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		result = -RTK_ERR_NOMEM;
		rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	memcpy(ptr, connect_param, sizeof(struct rtw_network_info));
	connect_param_temp = (struct rtw_network_info *)ptr;
	ptr += sizeof(struct rtw_network_info);

	if (connect_param->password_len) {
		memcpy(ptr, connect_param->password, connect_param->password_len);
		connect_param_temp->password = ptr;
	}

	DCache_Clean((u32)buf, size);
	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)size;
	result = whc_host_api_message_send(WHC_API_WIFI_CONNECT, param_buf, 2);

	if (result != RTK_SUCCESS) {
		rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	/* step4: wait connect finished for synchronous connection*/
	if (block) {
		join_block_param = block_param;

		// for eap connection, timeout should be longer (default value in wpa_supplicant: 60s)
		wifi_get_eap_phase(&eap_phase);
		if (eap_phase == 1) {
			timeout = 60000;
		} else {
			timeout = RTW_JOIN_TIMEOUT;
		}

		if (rtos_sema_take(block_param->sema, timeout) != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "Join bss timeout\n");
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			result = -RTK_ERR_TIMEOUT;
			goto error;
		} else {
			if (rtw_join_status != RTW_JOINSTATUS_SUCCESS) {
				result = join_fail_reason;
				no_need_indicate = 1;/*already indicated in join fail event handle*/
				goto error;
			}
		}
	}

error:
	if (block_param) {
		if (block_param->sema) {
			rtos_sema_delete_static(block_param->sema);
		}
		rtos_mem_free((u8 *)block_param);
		join_block_param = NULL;
	}

	if (rtw_join_status == RTW_JOINSTATUS_FAIL && no_need_indicate == 0) {
		join_status_info.status = RTW_JOINSTATUS_FAIL;
		join_fail = &join_status_info.priv.fail;
		join_fail->fail_reason = result;
		wifi_indication(RTW_EVENT_JOIN_STATUS, (u8 *)&join_status_info, sizeof(struct rtw_event_join_status_info));
	}

	rtos_mem_free(buf);
	return result;
}

s32 wifi_disconnect(void)
{
	int ret = 0;
#if CONFIG_AUTO_RECONNECT
	wifi_stop_autoreconnect();
#endif
	ret = whc_host_api_message_send(WHC_API_WIFI_DISCONNECT, NULL, 0);
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_is_running(unsigned char wlan_idx)
{
	int ret;
	u32 param_buf[1];
	param_buf[0] = wlan_idx;

	ret = whc_host_api_message_send(WHC_API_WIFI_IS_RUNNING, param_buf, 1);
	if (ret < 0) {
		ret = 0;
	}
	return ret;
}

s32 wifi_get_join_status(u8 *join_status)
{
	*join_status = rtw_join_status;
	return RTK_SUCCESS;
}

s32 wifi_on(u8 mode)
{
	int ret = 1;
	u32 param_buf[1];
	static u32 wifi_boot = 0;

	wifi_event_init();
	wifi_set_user_config();
	/* initialize after user config */
	whc_msg_q_init();
#ifdef CONFIG_WIFI_XMESH
	wifi_fast_connect_enable(0);
#else
	wifi_fast_connect_enable(wifi_user_config.fast_reconnect_en);
#endif
	wifi_set_autoreconnect(wifi_user_config.auto_reconnect_en);
	DCache_Clean((u32)(&wifi_user_config), sizeof(struct wifi_user_conf));
	param_buf[0] = (u32)(&wifi_user_config);
	ret = whc_host_api_message_send(WHC_API_WIFI_SET_USR_CFG, param_buf, 1);

	whc_ipc_host_init_skb();
	whc_ipc_host_init_rtwpbuf_pool();

	param_buf[0] = mode;
	ret = whc_host_api_message_send(WHC_API_WIFI_ON, param_buf, 1);

	if (wifi_boot == 0) {
		wifi_boot = 1;
		init_timer_wrapper();
#if !defined (CONFIG_MP_SHRINK) && !defined(ZEPHYR_WIFI)
		rtw_wpa_init(STA_WLAN_INDEX);
#endif
		if (p_wifi_do_fast_connect && (mode == RTW_MODE_STA)) {
			p_wifi_do_fast_connect();
		}
	}

	if (ret == RTK_SUCCESS) { //wifi on success
#ifdef CONFIG_LWIP_LAYER
		if (mode == RTW_MODE_STA) {
			lwip_netif_set_up(NETIF_WLAN_STA_INDEX);
		}
#endif
	}

	whc_ipc_host_heap_statistics(0);
	return ret;
}

int wifi_init_ap(void)
{
	int ret;

	ret = whc_host_api_message_send(WHC_API_WIFI_INIT_AP, NULL, 0);
	return ret;
}

s32 wifi_start_ap(struct rtw_softap_info *softap_config)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_softap_info *softap_config_temp = NULL;
	struct psk_info *PSK_INFO = NULL;

#ifdef CONFIG_P2P
	if (wifi_p2p_check_role(P2P_R_GO) == FALSE)
#endif
	{
		/* not GO, then check if softap is running */
		if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
			RTK_LOGW(TAG_WLAN_DRV, "already an AP running\n");
			return ret;
		}
	}

	/* check if STA is running */
	if ((wifi_user_config.concurrent_enabled == FALSE) &&
		(rtw_join_status > RTW_JOINSTATUS_UNKNOWN) && (rtw_join_status <= RTW_JOINSTATUS_SUCCESS)) {
		RTK_LOGE(TAG_WLAN_INIC, "need ap? please set concurrent_enabled to TRUE in wifi_set_user_config() !!\n");
		ret = RTK_FAIL;
		goto exit;
	}

	size = sizeof(struct rtw_softap_info) + softap_config->password_len;
	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		ret = RTK_FAIL;
		goto exit;
	}

	memcpy(ptr, softap_config, sizeof(struct rtw_softap_info));
	softap_config_temp = (struct rtw_softap_info *)ptr;
	ptr += sizeof(struct rtw_softap_info);

	if (softap_config->password_len) {
		memcpy(ptr, softap_config->password, softap_config->password_len);
		softap_config_temp->password = ptr;
	}
	DCache_Clean((u32)buf, size);

	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)size;

#if !(defined WIFI_ZEPHYR_TODO)
	rtw_wpa_init(SOFTAP_WLAN_INDEX);
	if ((PSK_INFO = rtos_mem_zmalloc(sizeof(struct psk_info))) == NULL) {
		ret = FALSE;
		goto exit;
	}
	if (softap_config->password && softap_config->password_len) {
		PSK_INFO->index = SOFTAP_WLAN_INDEX;
		PSK_INFO->security_type = softap_config->security_type;
		memcpy(PSK_INFO->psk_essid, softap_config->ssid.val, softap_config->ssid.len);
		memcpy(PSK_INFO->psk_passphrase, softap_config->password, softap_config->password_len);
		rtw_psk_set_psk_info(PSK_INFO);
	}
#endif
	ret = whc_host_api_message_send(WHC_API_WIFI_START_AP, param_buf, 2);

#ifdef CONFIG_P2P
	if (wifi_p2p_check_role(P2P_R_GO)) {
		wpas_wps_init(SOFTAP_WLAN_INDEX);
	}
#endif

	if (ret == RTK_SUCCESS) {
#ifdef CONFIG_LWIP_LAYER
		lwip_netif_set_up(NETIF_WLAN_AP_INDEX);
		lwip_netif_set_link_up(SOFTAP_WLAN_INDEX);
#endif
	}
#ifdef CONFIG_DFS_MASTER
	else if (ret == RTW_DFS_CAC_PENDING) {
		/* DFS master: the NP core armed CAC on a DFS channel and is not beaconing yet.
		 * The AP netif link-up is deferred until CAC passes, driven by the
		 * RTW_EVENT_DFS_CAC_DONE handler (rtw_dfs_cac_done_hdl in rtw_event.c). */
		ret = RTK_SUCCESS;
	}
#endif

exit:
	if (PSK_INFO) {
		rtos_mem_free(PSK_INFO);
	}
	if (buf) {
		rtos_mem_free(buf);
	}
	return ret;
}

s32 wifi_stop_ap(void)
{
	int ret = 0;

	if (wifi_is_running(SOFTAP_WLAN_INDEX) == FALSE) {
		RTK_LOGA(TAG_WLAN_INIC, "WIFI no run\n");
		return 0;
	}

#ifdef CONFIG_LWIP_LAYER
	lwip_netif_set_down(NETIF_WLAN_AP_INDEX);
	lwip_netif_set_link_down(NETIF_WLAN_AP_INDEX);
#endif

	ret = whc_host_api_message_send(WHC_API_WIFI_STOP_AP, NULL, 0);
	rtw_psk_wpa_deinit(SOFTAP_WLAN_INDEX);

	return ret;
}

s32 wifi_scan_networks(struct rtw_scan_param *scan_param, u8 block)
{
	assert_param(scan_param);
	struct internal_block_param *block_param = NULL;
	int ret = 0;
	u32 param_buf[2];
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_scan_param *scan_param_temp = NULL;
	u8 wait_cnt = 50;

	while (rtw_scan_api_inprocess) {
		rtos_time_delay_ms(2);  //For case: callback excuted or block sema released but rtw_scan_api_inprocess not cleared yet
		wait_cnt--;
		if (wait_cnt == 0) {
			return -RTK_ERR_BUSY;
		}
	}

	rtw_scan_api_inprocess = 1;

	if (block) {
		block_param = (struct internal_block_param *)rtos_mem_zmalloc(sizeof(struct internal_block_param));
		if (!block_param) {
			ret = -RTK_ERR_NOMEM;
			goto error;
		}
		rtos_sema_create_static(&block_param->sema, 0, 0xFFFFFFFF);
		if (!block_param->sema) {
			ret = -RTK_ERR_NOMEM;
			goto error;
		}

	}

	/* lock 2s to forbid suspend under scan */
	rtw_wakelock_timeout(2 * 1000);
	scan_user_callback_ptr = scan_param->scan_user_callback;
	scan_each_report_user_callback_ptr = scan_param->scan_report_each_mode_user_callback;
	scan_acs_report_user_callback_ptr = scan_param->scan_report_acs_user_callback;

	size = sizeof(struct rtw_scan_param)
		   + (scan_param->ssid ? strlen((char *)scan_param->ssid) + 1 : 0)
		   + (scan_param->channel_list ? scan_param->channel_list_num : 0);

	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto error;
	}

	memcpy(ptr, scan_param, sizeof(struct rtw_scan_param));
	scan_param_temp = (struct rtw_scan_param *)ptr;
	ptr += sizeof(struct rtw_scan_param);

	if (scan_param->ssid) {
		strcpy((char *)ptr, (char *)scan_param->ssid);
		scan_param_temp->ssid = ptr;
		ptr += strlen((char *)scan_param->ssid) + 1;
	}

	if (scan_param->channel_list) {
		memcpy(ptr, scan_param->channel_list, scan_param->channel_list_num);
		scan_param_temp->channel_list = ptr;
	}

	DCache_Clean((u32)buf, size);

	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)size;

	ret = whc_host_api_message_send(WHC_API_WIFI_SCAN_NETWROKS, param_buf, 2);

	if (ret != RTK_SUCCESS) {
		goto error;
	}

	if (block) {
		scan_block_param = block_param;
		if (rtos_sema_take(block_param->sema, RTW_SCAN_TIMEOUT) != RTK_SUCCESS) {
			RTK_LOGW(TAG_WLAN_DRV, "Scan timeout!\n");
			ret = -RTK_ERR_TIMEOUT;
		} else {
			ret = scanned_ap_cnt;
			scanned_ap_cnt = 0;
		}
		scan_block_param = NULL;
	}

	goto exit;

error:
	rtw_scan_api_inprocess = 0;

exit:
	if (block_param) {
		if (block_param->sema) {
			rtos_sema_delete_static(block_param->sema);
		}
		rtos_mem_free((u8 *)block_param);
	}

	if (buf) {
		rtos_mem_free(buf);
	}
	return ret;
}

u8 promisc_callback_default(struct rtw_rx_pkt_info *pkt_info)
{
	(void) pkt_info;

	return RTW_PROMISC_NEED_DRV_HDL;
}

void wifi_promisc_enable(u32 enable, struct rtw_promisc_para *para)
{
	u32 buf[3] = {0};

	if (enable && para == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "promisc param not set!\n");
		return;
	}

	buf[0] = enable;
	if (enable) {
		buf[1] = (u32)para->filter_mode;
		if (para->callback) {
			promisc_user_callback_ptr = para->callback;
			buf[2] = ENABLE;
		}
	} else {
		promisc_user_callback_ptr = promisc_callback_default;
	}

	whc_host_api_message_send(WHC_API_WIFI_PROMISC_INIT, buf, 3);
}

#endif	//#if CONFIG_WLAN
