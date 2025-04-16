/**
  ******************************************************************************
  * @file    inic_ipc_host_api_basic.c
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
#include "whc_host.h"

#include <rtw_timer.h>
#include <rtw_wakelock.h>

#include <wifi_intf_ram_to_rom.h>
#include "wpa_lite_intf.h"
#include "rom_hal_rom_to_flash.h"
/******************************************************
 *                    Constants
 ******************************************************/
/* auth/assoc/key resnd limit can be configured, refer max >> RTW_JOIN_TIMEOUT
 * including auth + assoc + 4way handshake, no dhcp
 */
#define RTW_JOIN_TIMEOUT (3 * 12000 + 13100 + 20200 + 50) //(MAX_CNT_SCAN_TIMES * SCANNING_TIMEOUT + MAX_JOIN_TIMEOUT + KEY_EXCHANGE_TIMEOUT + 50)

/******************************************************
 *               Variables Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
struct internal_join_block_param *join_block_param = NULL;

s32(*scan_user_callback_ptr)(u32, void *) = NULL;
s32(*scan_each_report_user_callback_ptr)(struct rtw_scan_result *, void *) = NULL;

u8(*promisc_user_callback_ptr)(struct rtw_rx_pkt_info *pkt_info) = NULL;
s32(*scan_acs_report_user_callback_ptr)(struct rtw_acs_mntr_rpt *acs_mntr_rpt) = NULL;

extern void *param_indicator;
u8 rtw_join_status = RTW_JOINSTATUS_UNKNOWN;
int join_fail_reason = RTK_SUCCESS;

int (*p_wifi_do_fast_connect)(void) = NULL;
int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2) = NULL;
void (*p_wifi_join_info_free)(u8 iface_type) = NULL;
/* Give default value if not defined */
/******************************************************
 *               Function Definitions
 ******************************************************/

#if CONFIG_WLAN
s32 wifi_connect(struct rtw_network_info *connect_param, u8 block)
{
	int result = RTK_SUCCESS;
	struct internal_join_block_param *block_param = NULL;

	u8 *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_network_info) + connect_param->password_len);
	u8 *ptr;
	u8 no_need_indicate = 0;
	struct rtw_event_info_joinstatus_joinfail fail_info = {0};

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
	wifi_indication(RTW_EVENT_JOIN_STATUS, NULL, 0, RTW_JOINSTATUS_STARTING);

	/* step2: malloc and set synchronous connection related variables*/
	if (block) {
		block_param = (struct internal_join_block_param *)rtos_mem_zmalloc(sizeof(struct internal_join_block_param));
		if (!block_param) {
			result = -RTK_ERR_NOMEM;
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}
		block_param->block = block;
		rtos_sema_create_static(&block_param->join_sema, 0, 0xFFFFFFFF);
		if (!block_param->join_sema) {
			result = -RTK_ERR_NOMEM;
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}

	}


	ptr = param_buf;
	memcpy(ptr, &connect_param->ssid, sizeof(connect_param->ssid));
	ptr += sizeof(connect_param->ssid);

	memcpy(ptr, &connect_param->bssid, sizeof(connect_param->bssid));
	ptr += sizeof(connect_param->bssid);

	memcpy(ptr, &connect_param->security_type, sizeof(connect_param->security_type));
	ptr += sizeof(connect_param->security_type);

	memcpy(ptr, &connect_param->password_len, sizeof(connect_param->password_len));
	ptr += sizeof(connect_param->password_len);

	memcpy(ptr, &connect_param->key_id, sizeof(connect_param->key_id));
	ptr += sizeof(connect_param->key_id);

	memcpy(ptr, &connect_param->channel, sizeof(connect_param->channel));
	ptr += sizeof(connect_param->channel);

	memcpy(ptr, &connect_param->pscan_option, sizeof(connect_param->pscan_option));
	ptr += sizeof(connect_param->pscan_option);

	memcpy(ptr, &connect_param->is_wps_trigger, sizeof(connect_param->is_wps_trigger));
	ptr += sizeof(connect_param->is_wps_trigger);

	memcpy(ptr, &connect_param->wpa_supp, sizeof(connect_param->wpa_supp));
	ptr += sizeof(connect_param->wpa_supp);

	memcpy(ptr, &connect_param->prev_bssid, sizeof(connect_param->prev_bssid));
	ptr += sizeof(connect_param->prev_bssid);

	if (connect_param->password_len) {
		memcpy(ptr, connect_param->password, connect_param->password_len);
	}

	whc_host_api_message_send(WHC_API_WIFI_CONNECT, (u8 *)param_buf, sizeof(struct rtw_network_info) + connect_param->password_len, (u8 *)&result,
							  sizeof(result));

	if (result != RTK_SUCCESS) {
		rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	/* step4: wait connect finished for synchronous connection*/
	if (block) {
		join_block_param = block_param;

#ifdef CONFIG_ENABLE_EAP
		// for eap connection, timeout should be longer (default value in wpa_supplicant: 60s)
		if (wifi_get_eap_phase()) {
			block_param->join_timeout = 60000;
		} else
#endif
			block_param->join_timeout = RTW_JOIN_TIMEOUT;

		if (rtos_sema_take(block_param->join_sema, block_param->join_timeout) != RTK_SUCCESS) {
			RTK_LOGE(TAG_WLAN_INIC, "Join bss timeout\n");
			rtw_join_status = RTW_JOINSTATUS_FAIL;
			result = -RTK_ERR_TIMEOUT;
			goto error;
		} else {
			if (rtw_join_status != RTW_JOINSTATUS_SUCCESS) {
				result = join_fail_reason;
				no_need_indicate = 1;
				goto error;
			}
		}
	}

	/* ameba start dhcp in atcmd, check where start dhcp when connect success in no ameba ic */
#if defined(TODO) && defined(CONFIG_LWIP_LAYER)
	if (result == RTK_SUCCESS) {
		/* Start DHCPClient */
		LwIP_DHCP(0, DHCP_START);
	}
#endif

error:
	if (param_buf) {
		rtos_mem_free((u8 *)param_buf);
	}
	if (block_param) {
		if (block_param->join_sema) {
			rtos_sema_delete_static(block_param->join_sema);
		}
		rtos_mem_free((u8 *)block_param);
		join_block_param = NULL;
	}

	if (rtw_join_status == RTW_JOINSTATUS_FAIL && no_need_indicate == 0) {
		fail_info.fail_reason = result;
		wifi_indication(RTW_EVENT_JOIN_STATUS, (u8 *)&fail_info, sizeof(struct rtw_event_info_joinstatus_joinfail), RTW_JOINSTATUS_FAIL);
	}

	return result;
}

s32 wifi_disconnect(void)
{
	int ret = 0;

	whc_host_api_message_send(WHC_API_WIFI_DISCONNECT, NULL, 0, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_is_running(u8 wlan_idx)
{
	int ret;
	u32 param_buf[1];
	param_buf[0] = wlan_idx;

	whc_host_api_message_send(WHC_API_WIFI_IS_RUNNING, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
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
	u32 wifi_mode;
	static u32 wifi_boot = 0;

	/* tell dev host linux or rtos */
	whc_host_api_message_send(WHC_API_WIFI_SET_HOST_RTOS, NULL, 0, NULL, 0);

	/* for common dev code, cfg follow np */
	whc_host_api_message_send(WHC_API_WIFI_SET_USR_CFG, NULL, 0, (void *)(&wifi_user_config), sizeof(struct wifi_user_conf));

	wifi_mode = (u32)mode;
	whc_host_api_message_send(WHC_API_WIFI_ON, (u8 *)&wifi_mode, 4, (u8 *)&ret, sizeof(ret));

	if (wifi_boot == 0) {
		wifi_boot = 1;
		init_timer_wrapper();
#ifndef CONFIG_MP_SHRINK
		rtw_wpa_init(STA_WLAN_INDEX);
#endif
		if (p_wifi_do_fast_connect && (mode == RTW_MODE_STA)) {
			p_wifi_do_fast_connect();
		}
	}

	if (ret == RTK_SUCCESS) { //wifi on success
#if defined(CONFIG_LWIP_LAYER)
		if (mode == RTW_MODE_STA) {
			LwIP_netif_set_up(0);
		}
#endif
	}

	return ret;
}

s32 wifi_start_ap(struct rtw_softap_info *softap_config)
{
	int ret = 0;
	u8 *param_buf;
	u8 *ptr;
	struct psk_info *PSK_INFO = NULL;

	/* check if softap is running */
	if (wifi_is_running(SOFTAP_WLAN_INDEX)) {
		RTK_LOGW(TAG_WLAN_DRV, "already an AP running\n");
		return ret;
	}

	/* check if STA is running */
	if ((wifi_user_config.concurrent_enabled == FALSE) &&
		(rtw_join_status > RTW_JOINSTATUS_UNKNOWN) && (rtw_join_status <= RTW_JOINSTATUS_SUCCESS)) {
		RTK_LOGE(TAG_WLAN_INIC, "need ap? please set concurrent_enabled to TRUE in wifi_set_user_config() !!\n");
		ret = RTK_FAIL;
		goto exit;
	}

	DCache_Clean((u32)softap_config->password, softap_config->password_len);
	DCache_Clean((u32)softap_config, sizeof(struct rtw_softap_info));

	param_buf = rtos_mem_zmalloc(sizeof(struct rtw_softap_info) + softap_config->password_len);

	if (!param_buf) {
		goto exit;
	}
	ptr = param_buf;

	memcpy(ptr, &softap_config->ssid, sizeof(softap_config->ssid));
	ptr += sizeof(softap_config->ssid);

	memcpy(ptr, &softap_config->hidden_ssid, sizeof(softap_config->hidden_ssid));
	ptr += sizeof(softap_config->hidden_ssid);

	memcpy(ptr, &softap_config->security_type, sizeof(softap_config->security_type));
	ptr += sizeof(softap_config->security_type);

	memcpy(ptr, &softap_config->password_len, sizeof(softap_config->password_len));
	ptr += sizeof(softap_config->password_len);

	memcpy(ptr, &softap_config->channel, sizeof(softap_config->channel));
	ptr += sizeof(softap_config->channel);

	if (softap_config->password_len) {
		memcpy(ptr, softap_config->password, sizeof(softap_config->password_len));
	}

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

	whc_host_api_message_send(WHC_API_WIFI_START_AP, (u8 *)param_buf, sizeof(struct rtw_softap_info) + softap_config->password_len, (u8 *)&ret, sizeof(ret));

	if (ret == RTK_SUCCESS) {
#ifdef CONFIG_LWIP_LAYER
		LwIP_netif_set_up(SOFTAP_WLAN_INDEX);
		LwIP_netif_set_link_up(SOFTAP_WLAN_INDEX);
#endif
	}

exit:
	if (PSK_INFO) {
		rtos_mem_free(PSK_INFO);
	}

	if (param_buf) {
		rtos_mem_free(param_buf);
	}
	return ret;
}

s32 wifi_stop_ap(void)
{
	int ret = 0;

	if (wifi_is_running(SOFTAP_WLAN_INDEX) == 0) {
		RTK_LOGA(TAG_WLAN_INIC, "WIFI no run\n");
		return 0;
	}

#ifdef CONFIG_LWIP_LAYER
	dhcps_deinit();
	LwIP_netif_set_down(1);
	LwIP_netif_set_link_down(1);
#endif

	whc_host_api_message_send(WHC_API_WIFI_STOP_AP, NULL, 0, (u8 *)&ret, sizeof(ret));
	rtw_psk_wpa_deinit(SOFTAP_WLAN_INDEX);

	return ret;
}

s32 wifi_scan_networks(struct rtw_scan_param *scan_param, u8 block)
{
	assert_param(scan_param);
	int ret = 0;
	u8 *param_buf;
	u8 *ptr;
	u32 ssid_len;
	u32 buf_len = 0;
	/* lock 2s to forbid suspend under scan */
	rtw_wakelock_timeout(2 * 1000);
	scan_user_callback_ptr = scan_param->scan_user_callback;
	scan_each_report_user_callback_ptr = scan_param->scan_report_each_mode_user_callback;
	scan_acs_report_user_callback_ptr = scan_param->scan_report_acs_user_callback;

	if (scan_param->ssid)  {
		ssid_len = strlen((char *)scan_param->ssid);
		buf_len += ssid_len;
	} else {
		ssid_len = 0;
	}

	if (scan_param->channel_list) {
		buf_len += scan_param->channel_list_num;
	}

	buf_len += sizeof(struct rtw_scan_param);

	param_buf = rtos_mem_zmalloc(buf_len);

	if (!param_buf) {
		ret = -1;
		return ret;
	}
	ptr = param_buf;

	memcpy(ptr, &block, sizeof(block));
	ptr += 4;

	memcpy(ptr, &ssid_len, sizeof(ssid_len));
	ptr += sizeof(ssid_len);

	memcpy(ptr, &scan_param->options, sizeof(scan_param->options));
	ptr += sizeof(scan_param->options);

	memcpy(ptr, &scan_param->channel_list_num, sizeof(scan_param->channel_list_num));
	ptr += sizeof(scan_param->channel_list_num);

	memcpy(ptr, &scan_param->chan_scan_time, sizeof(scan_param->chan_scan_time));
	ptr += sizeof(scan_param->chan_scan_time);

	memcpy(ptr, &scan_param->max_ap_record_num, sizeof(scan_param->max_ap_record_num));
	ptr += sizeof(scan_param->max_ap_record_num);

	memcpy(ptr, &scan_param->scan_user_callback, sizeof(u32));
	ptr += sizeof(u32);

	if (ssid_len) {
		memcpy(ptr, &scan_param->ssid, sizeof(ssid_len));
		ptr += ssid_len;
	}

	if (scan_param->channel_list) {
		memcpy(ptr, &scan_param->channel_list, scan_param->channel_list_num);
		ptr += scan_param->channel_list_num;
	}

	memcpy(ptr, &scan_param->scan_user_data, sizeof(u32));
	ptr += sizeof(u32);

	whc_host_api_message_send(WHC_API_WIFI_SCAN_NETWROKS, (u8 *)param_buf, buf_len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

void wifi_promisc_enable(u32 enable, struct rtw_promisc_para *para)
{
	u32 buf[3] = {0};
	buf[0] = enable;
	buf[1] = (u32)para->filter_mode;
	if (para->callback) {
		promisc_user_callback_ptr = para->callback;
		buf[2] = ENABLE;
	}
	whc_host_api_message_send(WHC_API_WIFI_PROMISC_INIT, (u8 *)buf, 12, NULL, 0);
}

#endif	//#if CONFIG_WLAN
