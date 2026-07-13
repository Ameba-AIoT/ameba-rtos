/**
  ******************************************************************************
  * @file    inic_ipc_host_api_ext.c
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

#include <os_wrapper.h>
#include <rtw_timer.h>

#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>
#endif

/******************************************************
 *                    Constants
 ******************************************************/
#define RTW_SCAN_ABORT_TIMEOUT (20)

/* Static buffer size for wifi_radar_send_data IPC transfer.
 * Near/Far: 44 IQ samples (s16 I + s16 Q) * 32 frames = 5632 bytes max.
 * AI_L_NEAR: 6 IQ samples * 128 frames = 3072 bytes max. */
#define RADAR_SEND_DATA_BUF_SIZE  5632

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Variables Declarations
 ******************************************************/

#ifdef RELEASE_WIFI
u8 wifi_dbg_log_enable = 0x00; // BIT(0): dp_log
#else
u8 wifi_dbg_log_enable = 0x01; // BIT(0): dp_log
#endif

struct internal_block_param *scan_abort_block_param = NULL;

void (*p_ap_channel_switch_callback)(u8 channel, s8 ret) = NULL;
#ifdef CONFIG_WIFI_TUNNEL
int (*rmesh_ota_user_callback)(u8 *, u16) = NULL;
#endif
/******************************************************
 *               Variables Declarations
 ******************************************************/
void *param_indicator;
struct task_struct wifi_autoreconnect_task = {0};

/******************************************************
 *               Variables Definitions
 ******************************************************/

/*NETMASK*/
#ifndef NETMASK_ADDR0
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
#endif

/*Gateway Address*/
#ifndef GW_ADDR0
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1
#endif


/******************************************************
 *               Function Definitions
 ******************************************************/
#if CONFIG_WLAN
//----------------------------------------------------------------------------//
s32 wifi_set_channel(u8 wlan_idx, u8 channel)
{
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)channel;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_CHANNEL, param_buf, 2);
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_set_countrycode(u8 *cntcode)
{
	int ret = 0;
	u32 param_buf[1];

	u8 *cntcode_temp = (u8 *)rtos_mem_zmalloc(2);
	if (cntcode_temp == NULL) {
		return -1;
	}
	memcpy(cntcode_temp, cntcode, 2);
	DCache_Clean((u32)cntcode_temp, 2);
	param_buf[0] = (u32)cntcode_temp;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_COUNTRY_CODE, param_buf, 1);

	rtos_mem_free(cntcode_temp);
	return ret;
}

s32 wifi_get_countrycode(struct rtw_country_code_table *pinfo)
{
	int ret = 0;
	u32 param_buf[1] = {0};
	struct rtw_country_code_table *tab = (struct rtw_country_code_table *)rtos_mem_zmalloc(sizeof(struct rtw_country_code_table));

	if ((pinfo == NULL) || (tab == NULL)) {
		return -1;
	}

	param_buf[0] = (u32)tab;
	DCache_CleanInvalidate((u32)tab, sizeof(struct rtw_country_code_table));
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_COUNTRY_CODE, param_buf, 1);
	DCache_Invalidate((u32)tab, sizeof(struct rtw_country_code_table));
	memcpy(pinfo, tab, sizeof(struct rtw_country_code_table));

	if (tab) {
		rtos_mem_free((u8 *)tab);
	}

	return ret;
}

int wifi_set_chplan(u8 chplan)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)chplan;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_CHPLAN, param_buf, 1);
	return ret;
}

s32 wifi_get_channel_list(struct rtw_channel_list *ch_list)
{
	int ret = 0;
	u32 param_buf[1];

	struct rtw_channel_list *ch_list_temp = (struct rtw_channel_list *)rtos_mem_zmalloc(sizeof(struct rtw_channel_list));
	if (ch_list_temp == NULL) {
		return -1;
	}

	param_buf[0] = (u32)ch_list_temp;
	DCache_CleanInvalidate((u32)ch_list_temp, sizeof(struct rtw_channel_list));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_CHANNEL_LIST, param_buf, 1);
	DCache_Invalidate((u32)ch_list_temp, sizeof(struct rtw_channel_list));
	memcpy(ch_list, ch_list_temp, sizeof(struct rtw_channel_list));
	rtos_mem_free((u8 *)ch_list_temp);
	return ret;
}

s32 wifi_get_scan_records(u32 *ap_num, struct rtw_scan_result *ap_list)
{
	int ret = 0;
	u32 param_buf[2];

	unsigned int *ap_num_temp = (unsigned int *)rtos_mem_zmalloc(sizeof(unsigned int));
	if (ap_num_temp == NULL) {
		return -1;
	}
	*ap_num_temp = *ap_num;

	struct rtw_scan_result *ap_list_temp = (struct rtw_scan_result *)rtos_mem_zmalloc((*ap_num) * sizeof(struct rtw_scan_result));
	if (ap_list_temp == NULL) {
		rtos_mem_free(ap_num_temp);
		return -1;
	}

	param_buf[0] = (u32)ap_num_temp;
	param_buf[1] = (u32)ap_list_temp;
	DCache_CleanInvalidate((u32)ap_num_temp, sizeof(unsigned int));
	DCache_CleanInvalidate((u32)ap_list_temp, (*ap_num)*sizeof(struct rtw_scan_result));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_SCANNED_AP_INFO, param_buf, 2);
	DCache_Invalidate((u32)ap_num_temp, sizeof(unsigned int));
	DCache_Invalidate((u32)ap_list_temp, (*ap_num)*sizeof(struct rtw_scan_result));
	*ap_num = *ap_num_temp;
	memcpy(ap_list, ap_list_temp, ((*ap_num)*sizeof(struct rtw_scan_result)));

	rtos_mem_free((u8 *)ap_num_temp);
	rtos_mem_free((u8 *)ap_list_temp);
	return ret;
}

s32 wifi_scan_abort(void)
{
	int ret = 0;

	struct internal_block_param *block_param = NULL;

	block_param = (struct internal_block_param *)rtos_mem_zmalloc(sizeof(struct internal_block_param));
	if (!block_param) {
		ret = -1;
		goto exit;
	}
	rtos_sema_create_static(&block_param->sema, 0, 0xFFFFFFFF);
	if (!block_param->sema) {
		ret = -1;
		goto exit;
	}

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SCAN_ABORT, NULL, 0);

	if (ret != RTK_SUCCESS) {
		ret = 0;  /* there is no scan ongoing, just return SUCCESS*/
		goto exit;
	}

	scan_abort_block_param = block_param;

	if (rtos_sema_take(block_param->sema, RTW_SCAN_ABORT_TIMEOUT) != RTK_SUCCESS) {
		RTK_LOGW(TAG_WLAN_DRV, "scan abort wait timeout!\n");
	}
	scan_abort_block_param = NULL;

exit:
	if (block_param) {
		if (block_param->sema) {
			rtos_sema_delete_static(block_param->sema);
		}
		rtos_mem_free((u8 *)block_param);
	}

	return ret;
}

//----------------------------------------------------------------------------//

s32 wifi_get_mac_address(s32 idx, struct rtw_mac *mac, u8 efuse)
{
	int ret = 0;
	u32 param_buf[3];

	struct rtw_mac *mac_temp = (struct rtw_mac *)rtos_mem_zmalloc(sizeof(struct rtw_mac));
	if (mac_temp == NULL) {
		return -1;
	}

	param_buf[0] = idx;
	param_buf[1] = (u32)mac_temp;
	param_buf[2] = efuse;
	DCache_CleanInvalidate((u32)mac_temp, sizeof(struct rtw_mac));
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_MAC_ADDR, param_buf, 3);

	DCache_Invalidate((u32)mac_temp, sizeof(struct rtw_mac));
	memcpy(mac, mac_temp, sizeof(struct rtw_mac));
	rtos_mem_free((u8 *)mac_temp);
	return ret;
}

int wifi_set_mac_address(int idx, unsigned char *mac, u8 efuse)
{
	int ret = 0;
	u32 param_buf[3];
	if (mac == NULL) {
		return -1;
	}
	unsigned char *mac_temp = (unsigned char *)rtos_mem_zmalloc(ETH_ALEN);
	if (mac_temp == NULL) {
		return -1;
	}
	memcpy(mac_temp, mac, ETH_ALEN);

	DCache_Clean((u32)mac_temp, ETH_ALEN);

	param_buf[0] = idx;
	param_buf[1] = (u32)mac_temp;
	param_buf[2] = efuse;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_MAC_ADDR, param_buf, 3);

	rtos_mem_free((u8 *)mac_temp);
	return ret;
}
//----------------------------------------------------------------------------//
s32 wifi_driver_is_mp(u8 *is_mp)
{
	int ret = 0;

	ret = (u8)whc_ipc_host_api_message_send(WHC_API_WIFI_DRIVE_IS_MP, NULL, 0);
	if (ret < 0) {
		*is_mp = 0;
	} else {
		*is_mp = ret;
		ret = 0;
	}
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_ap_get_connected_clients(struct rtw_client_list *client_list_buffer)
{
	int ret = 0;
	u32 param_buf[1];

	struct rtw_client_list *client_list_buffer_temp = (struct rtw_client_list *)rtos_mem_zmalloc(sizeof(struct rtw_client_list));
	if (client_list_buffer_temp == NULL) {
		return -1;
	}

	param_buf[0] = (u32)client_list_buffer_temp;
	DCache_CleanInvalidate((u32)client_list_buffer_temp, sizeof(struct rtw_client_list));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_AP_GET_CONNECTED_CLIENTS, param_buf, 1);
	DCache_Invalidate((u32)client_list_buffer_temp, sizeof(struct rtw_client_list));
	memcpy(client_list_buffer, client_list_buffer_temp, sizeof(struct rtw_client_list));
	rtos_mem_free((u8 *)client_list_buffer_temp);
	return ret;
}

s32 wifi_ap_del_client(u8 *hwaddr)
{
	int ret = 0;
	u32 param_buf[2];

	u8 *hwaddr_temp = (u8 *)rtos_mem_zmalloc(ETH_ALEN);
	if (hwaddr_temp == NULL) {
		return -1;
	}
	memcpy(hwaddr_temp, hwaddr, ETH_ALEN);

	DCache_Clean((u32)hwaddr_temp, ETH_ALEN);
	param_buf[0] = (u32)IFACE_PORT1;
	param_buf[1] = (u32)hwaddr_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_AP_DEL_CLIENT, param_buf, 2);

	rtos_mem_free(hwaddr_temp);
	return ret;
}

s32 wifi_ap_switch_chl_and_inform(struct rtw_csa_parm *csa_param)
{
	int ret = 0;
	u32 param_buf[3];
	p_ap_channel_switch_callback = csa_param->callback;

	struct rtw_csa_parm *csa_param_temp = (struct rtw_csa_parm *)rtos_mem_zmalloc(sizeof(struct rtw_csa_parm));
	if (csa_param_temp == NULL) {
		return -1;
	}
	memcpy(csa_param_temp, csa_param, sizeof(struct rtw_csa_parm));
	DCache_Clean((u32)csa_param_temp, sizeof(struct rtw_csa_parm));

	param_buf[0] = (u32)csa_param_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_AP_CH_SWITCH, param_buf, 1);

	rtos_mem_free(csa_param_temp);
	return ret;
}

void wifi_ap_set_invisible(u8 enable)
{
	u32 param_buf[1];
	param_buf[0] = (u32)enable;

	whc_ipc_host_api_message_send(WHC_API_WIFI_AP_TOGGLE_VISIBLE, param_buf, 1);
}

//----------------------------------------------------------------------------//
s32 wifi_get_setting(u8 wlan_idx, struct rtw_wifi_setting *psetting)
{
	int ret = 0;
	u32 param_buf[2];

	struct rtw_wifi_setting *setting_temp = (struct rtw_wifi_setting *)rtos_mem_zmalloc(sizeof(struct rtw_wifi_setting));
	if (setting_temp == NULL) {
		return -1;
	}

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)setting_temp;
	DCache_CleanInvalidate((u32)setting_temp, sizeof(struct rtw_wifi_setting));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_SETTING, param_buf, 2);
	DCache_Invalidate((u32)setting_temp, sizeof(struct rtw_wifi_setting));
	memcpy(psetting, setting_temp, sizeof(struct rtw_wifi_setting));
	rtos_mem_free((u8 *)setting_temp);

	return ret;
}

void wifi_set_dbg_dp_log(char *buf)
{
	u32 param = 0;
	_sscanf_ss(buf, "%*s%x", &param);

	if (param == 0) {
		wifi_dbg_log_enable &= ~BIT(0);
	} else {
		wifi_dbg_log_enable |= BIT(0);
	}
}

int wifi_set_ips_internal(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_IPS_EN, param_buf, 1);
	return ret;
}

s32 wifi_set_lps_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_LPS_EN, param_buf, 1);
	return ret;
}

s32 wifi_set_lps_listen_interval(u8 interval)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)interval;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_LPS_LISTEN_INTERVAL, param_buf, 1);
	return ret;
}

s32 wifi_set_lps_bcn_window(u8 enable, u8 bcn_ely_time, u8 rx_bcn_timeout)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)bcn_ely_time;
	param_buf[2] = (u32)rx_bcn_timeout;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_LPS_BCN_WINDOW_MANUAL_CONFIG, param_buf, 3);
	return ret;
}

s32 wifi_set_wowlan_rx_broadcast(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_WOWLAN_RX_BROADCAST, param_buf, 1);
	return ret;
}
//----------------------------------------------------------------------------//

s32 wifi_set_wowlan_ipv6_wake(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_WOWLAN_IPV6_WAKE, param_buf, 1);
	return ret;
}
//----------------------------------------------------------------------------//

int wifi_set_mfp_support(unsigned char value)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)value;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_MFP_SUPPORT, param_buf, 1);
	return ret;
}

int wifi_set_group_id(unsigned char value)
{
	rtw_sae_set_user_group_id(value);

	return RTK_SUCCESS;
}

int wifi_set_pmk_cache_enable(unsigned char value)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)value;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_PMK_CACHE_EN, param_buf, 1);
	return ret;
}

s32 wifi_get_traffic_stats(u8 wlan_idx, union rtw_traffic_stats *traffic_stats)
{
	u32 param_buf[2];
	int ret = 0;

	union rtw_traffic_stats *traffic_stats_temp = (union rtw_traffic_stats *)rtos_mem_zmalloc(sizeof(union rtw_traffic_stats));
	if (traffic_stats_temp == NULL) {
		return -1;
	}
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)traffic_stats_temp;
	DCache_CleanInvalidate((u32)traffic_stats_temp, sizeof(union rtw_traffic_stats));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_TRAFFIC_STATS, param_buf, 2);

	DCache_Invalidate((u32)traffic_stats_temp, sizeof(union rtw_traffic_stats));
	memcpy(traffic_stats, traffic_stats_temp, sizeof(union rtw_traffic_stats));
	rtos_mem_free((u8 *)traffic_stats_temp);
	return ret;
}

s32 wifi_get_phy_stats(u8 wlan_idx, u8 *mac_addr, union rtw_phy_stats *phy_stats)
{
	u32 param_buf[3];
	u8 *mac_addr_temp = NULL;
	int ret = 0;

	union rtw_phy_stats *phy_stats_temp = (union rtw_phy_stats *)rtos_mem_zmalloc(sizeof(union rtw_phy_stats));
	if (phy_stats_temp == NULL) {
		return -1;
	}

	if (mac_addr) {
		mac_addr_temp = (u8 *)rtos_mem_zmalloc(ETH_ALEN);
		if (mac_addr_temp == NULL) {
			return -1;
		}
		memcpy(mac_addr_temp, mac_addr, ETH_ALEN);
		DCache_Clean((u32)mac_addr_temp, ETH_ALEN);
	}
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)mac_addr_temp;
	param_buf[2] = (u32)phy_stats_temp;
	DCache_CleanInvalidate((u32)phy_stats_temp, sizeof(union rtw_phy_stats));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_PHY_STATS, param_buf, 3);

	DCache_Invalidate((u32)phy_stats_temp, sizeof(union rtw_phy_stats));
	memcpy(phy_stats, phy_stats_temp, sizeof(union rtw_phy_stats));
	rtos_mem_free((u8 *)phy_stats_temp);
	if (mac_addr_temp) {
		rtos_mem_free(mac_addr_temp);
	}
	return ret;
}

s32 wifi_get_wireless_mode(u8 *wmode)
{
	int ret = 0;
	u32 param_buf[1];
	u8 *wmode_temp = (u8 *)rtos_mem_zmalloc(sizeof(u8));
	if (wmode_temp == NULL) {
		return -1;
	}
	param_buf[0] = (u32)wmode_temp;
	DCache_CleanInvalidate((u32)wmode_temp, sizeof(u8));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_WIRELESS_MODE, param_buf, 1);
	DCache_Invalidate((u32)wmode_temp, sizeof(u8));
	*wmode = *wmode_temp;
	rtos_mem_free(wmode_temp);
	return ret;
}

s32 wifi_set_wireless_mode(u32 wmode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)wmode;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_WIRELESS_MODE, param_buf, 1);
	return ret;
}

int wifi_set_wps_phase(u8 wlan_idx, unsigned char is_trigger_wps)
{
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = wlan_idx;
	param_buf[1] = is_trigger_wps;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_WPS_PHASE, param_buf, 2);
	return ret;
}

int wifi_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 param_buf[4];
	char *buf_temp = NULL;

	if (buf_len != 0) {
		buf_temp = (char *)rtos_mem_zmalloc(buf_len);
		if (buf_temp == NULL) {
			return -1;
		}
		memcpy(buf_temp, buf, buf_len);
		DCache_Clean((u32)buf_temp, (u32)buf_len);
	}
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)buf_temp;
	param_buf[2] = (u32)buf_len;
	param_buf[3] = (u32)flags;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_GEN_IE, param_buf, 4);
	rtos_mem_free((u8 *)buf_temp);
	return ret;
}

int wifi_set_eap_phase(unsigned char is_trigger_eap)
{
#ifdef CONFIG_EAP
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = is_trigger_eap;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_EAP_PHASE, param_buf, 1);
	return ret;
#else
	return -1;
#endif
}

int wifi_get_eap_phase(u8 *eap_phase)
{
#ifdef CONFIG_EAP
	int ret = 0;

	ret = (u8)whc_ipc_host_api_message_send(WHC_API_WIFI_GET_EAP_PHASE, NULL, 0);
	if (ret < 0) {
		*eap_phase = 0;
	} else {
		*eap_phase = ret;
		ret = 0;
	}
	return ret;
#else
	return -1;
#endif
}

int wifi_set_eap_method(unsigned char eap_method)
{
#ifdef CONFIG_EAP
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = eap_method;

#if CONFIG_AUTO_RECONNECT
	rtw_reconn.eap_method = eap_method;
#endif

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_EAP_METHOD, param_buf, 1);
	return ret;
#else
	return -1;
#endif
}

int wifi_if_send_eapol(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 param_buf[4];
	char *buf_temp = (char *)rtos_mem_zmalloc(buf_len);
	if (buf_temp == NULL) {
		return -1;
	}
	memcpy(buf_temp, buf, buf_len);

	DCache_Clean((u32)buf_temp, (u32)buf_len);
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)buf_temp;
	param_buf[2] = buf_len;
	param_buf[3] = flags;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SEND_EAPOL, param_buf, 4);
	rtos_mem_free((u8 *)buf_temp);
	return ret;
}

//----------------------------------------------------------------------------//
/*
 * Example for custom ie
 *
 * u8 ie1[] = {221, 2, 2, 2};
 * u8 ie2[] = {221, 2, 1, 1};
 * struct rtw_custom_ie ie_list[2] = {{ie1, RTW_CUS_IE_BEACON | RTW_CUS_IE_PROBERSP}, {ie2, RTW_CUS_IE_PROBERSP}};
 * u8 ie2_new[] = {221, 2, 1, 3} ;
 * struct rtw_custom_ie ie_update = {ie2_new, RTW_CUS_IE_PROBERSP};
 *
 * add ie list
 * static void cmd_add_ie(int argc, char **argv)
 * {
 *	 wifi_add_custom_ie(ie_list, 2);
 * }
 *
 * update current ie
 * static void cmd_update_ie(int argc, char **argv)
 * {
 *	 wifi_update_custom_ie(&ie_update, 2);
 * }
 *
 * delete all ie for specific wlan interface
 * static void cmd_del_ie(int argc, char **argv)
 * {
 *	 wifi_del_custom_ie(SOFTAP_WLAN_INDEX);
 * }
 */
s32 wifi_add_custom_ie(struct rtw_custom_ie *ie_list, s32 ie_num)
{
	int ret = 0;
	u32 param_buf[4];
	u8 ie_len = 0;
	int cnt = 0;
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_custom_ie *dst_ie = NULL;

	size += ie_num * sizeof(struct rtw_custom_ie);
	for (cnt = 0; cnt < ie_num; cnt++) {
		ie_len = ie_list[cnt].ie[1];
		size += (ie_len + 2);
	}

	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		return FALSE;
	}
	memcpy(buf, ie_list, ie_num * sizeof(struct rtw_custom_ie));
	dst_ie = (struct rtw_custom_ie *)buf;
	ptr += ie_num * sizeof(struct rtw_custom_ie);

	for (cnt = 0; cnt < ie_num; cnt++) {
		ie_len = dst_ie[cnt].ie[1];
		memcpy(ptr, dst_ie[cnt].ie, (u32)(ie_len + 2));
		dst_ie[cnt].ie = (u8 *)ptr;
		ptr += (ie_len + 2);
	}

	DCache_Clean((u32)buf, size);
	param_buf[0] = 0;//type 0 means add
	param_buf[1] = (u32)buf;
	param_buf[2] = (u32)size;
	param_buf[3] = ie_num;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_CUS_IE, param_buf, 4);

	rtos_mem_free(buf);
	return ret;
}

s32 wifi_set_broadcast_port_wakeup_white_list(u16 *port_list, u8 list_count)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *buf = NULL;

	buf = (u8 *)rtos_mem_zmalloc(list_count * 2);
	if (buf == NULL) {
		return FALSE;
	}
	memcpy(buf, port_list, (list_count * 2));

	DCache_Clean((u32)buf, (list_count * 2));

	param_buf[0] = (u32)buf;
	param_buf[1] = list_count;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_BROADCAST_WHITE_LIST, param_buf, 2);
	rtos_mem_free(buf);
	return ret;
}

s32 wifi_update_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_index)
{
	int ret = 0;
	u32 param_buf[4];
	u8 ie_len = 0;
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct rtw_custom_ie *cus_ie_temp = NULL;

	struct rtw_custom_ie ie_t = *(struct rtw_custom_ie *)(cus_ie);
	ie_len = *(u8 *)(ie_t.ie + 1) + 2;
	size = sizeof(struct rtw_custom_ie) + ie_len;
	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		return FALSE;
	}
	memcpy(ptr, cus_ie, sizeof(struct rtw_custom_ie));
	cus_ie_temp = (struct rtw_custom_ie *)ptr;
	ptr += sizeof(struct rtw_custom_ie);

	memcpy(ptr, ie_t.ie, ie_len);
	cus_ie_temp->ie = (u8 *)ptr;

	DCache_Clean((u32)buf, size);
	param_buf[0] = 1;//type 1 means update
	param_buf[1] = (u32)buf;
	param_buf[2] = (u32)size;
	param_buf[3] = ie_index;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_CUS_IE, param_buf, 4);
	rtos_mem_free(buf);
	return ret;
}

s32 wifi_del_custom_ie(u8 wlan_idx)
{
	u32 param_buf[2];

	param_buf[0] = 2;//type 2 means delete
	param_buf[1] = (u32)wlan_idx;
	return whc_ipc_host_api_message_send(WHC_API_WIFI_CUS_IE, param_buf, 2);
}

int wifi_send_mgnt(struct _raw_data_desc_t *raw_data_desc)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *buf = NULL, *ptr = NULL;
	size_t size = 0;
	struct _raw_data_desc_t *raw_data_desc_temp = NULL;

	size = sizeof(struct _raw_data_desc_t) + raw_data_desc->buf_len;
	ptr = buf = (u8 *)rtos_mem_zmalloc(size);
	if (buf == NULL) {
		return FALSE;
	}
	memcpy(ptr, raw_data_desc, sizeof(struct _raw_data_desc_t));
	raw_data_desc_temp = (struct _raw_data_desc_t *)ptr;
	ptr += sizeof(struct _raw_data_desc_t);

	memcpy(ptr, raw_data_desc->buf, (u32)raw_data_desc->buf_len);
	raw_data_desc_temp->buf = ptr;

	DCache_Clean((u32)buf, (u32)size);
	param_buf[0] = (u32)buf;
	param_buf[1] = (u32)size;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SEND_MGNT, param_buf, 2);
	rtos_mem_free(buf);
	return ret;
}

s32 wifi_set_tx_rate_by_tos(u8 enable, u8 tos_precedence, u8 tx_rate)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)tos_precedence;
	param_buf[2] = (u32)tx_rate;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_TXRATE_BY_TOS, param_buf, 3);
	return ret;
}

s32 wifi_set_edca_param(struct rtw_edca_param *pedca_param)
{
	int ret = 0;
	u32 param_buf[1] = {0};
	struct rtw_edca_param *pedca_param_temp = (struct rtw_edca_param *)rtos_mem_zmalloc(sizeof(struct rtw_edca_param));
	if (pedca_param_temp == NULL) {
		return -1;
	}

	memcpy(pedca_param_temp, pedca_param, sizeof(struct rtw_edca_param));
	DCache_Clean((u32)pedca_param_temp, sizeof(struct rtw_edca_param));
	param_buf[0] = (u32)pedca_param_temp;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_EDCA_PARAM, param_buf, 1);
	rtos_mem_free((u8 *)pedca_param_temp);

	return ret;
}

s32 wifi_set_tx_cca_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = enable;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_TX_CCA_EN, param_buf, 1);
	return ret;
}

s32 wifi_set_cts2self_duration_and_send(u8 wlan_idx, u16 duration)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)duration;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND, param_buf, 2);
	return ret;

}

s32 wifi_get_antdiv_info(u8 *antdiv_mode, u8 *curr_ant)
{
#ifdef CONFIG_WIFI_ANTDIV
	int ret = 0;
	u32 param_buf[1];
	u8 *antInfo_temp = (u8 *)rtos_mem_zmalloc(sizeof(int));

	if (antInfo_temp == NULL) {
		return -1;
	}
	param_buf[0] = (u32)antInfo_temp;
	DCache_CleanInvalidate((u32)antInfo_temp, sizeof(int));
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_ANTENNA_INFO, param_buf, 1);
	DCache_Invalidate((u32)antInfo_temp, sizeof(int));
	*antdiv_mode = *(u8 *)antInfo_temp;
	*curr_ant = *(u8 *)(antInfo_temp + sizeof(u8));
	rtos_mem_free(antInfo_temp);
	return ret;
#else
	UNUSED(antdiv_mode);
	UNUSED(curr_ant);
	return -1;
#endif
}

s32 wifi_set_antdiv_info(u8 antdiv_mode)
{
#ifdef CONFIG_WIFI_ANTDIV
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = antdiv_mode;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_ANTENNA_INFO, param_buf, 1);
	return ret;

#else
	UNUSED(antdiv_mode);
	return -1;
#endif
}

s32 wifi_get_band_type(u8 *band_type)
{
	u8 ret = 0;
	u32 param_buf[1];
	u8 *band_type_temp = (u8 *)rtos_mem_zmalloc(sizeof(u8));
	if (band_type_temp == NULL) {
		return -1;
	}
	param_buf[0] = (u32)band_type_temp;
	DCache_CleanInvalidate((u32)band_type_temp, sizeof(u8));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_BAND_TYPE, param_buf, 1);
	DCache_Invalidate((u32)band_type_temp, sizeof(u8));
	*band_type = *band_type_temp;
	rtos_mem_free(band_type_temp);
	return ret;
}

s32 wifi_get_tsf(u8 wlan_idx, u64 *tsf)
{
	*tsf = whc_ipc_host_api_get_wifi_tsf(wlan_idx);
	return RTK_SUCCESS;
}

s32 wifi_get_latched_tsf_i2s(struct rtw_speaker_read_latch_req *req, struct rtw_speaker_read_latch_rpt *rpt)
{
	return whc_ipc_host_api_get_wifi_latched_tsf_i2s(req, rpt);
}

s32 wifi_get_txbuf_pkt_num(s32 *pkt_num)
{
	*pkt_num = whc_ipc_host_api_get_txbuf_pkt_num();
	return RTK_SUCCESS;
}

//----------------------------------------------------------------------------//
s32 wifi_csi_config(struct rtw_csi_action_parm *act_param)
{
	int ret = 0;
	u32 param_buf[1];

	struct rtw_csi_action_parm *act_param_temp = (struct rtw_csi_action_parm *)rtos_mem_zmalloc(sizeof(struct rtw_csi_action_parm));
	if (act_param_temp == NULL) {
		return -1;
	}
	memcpy(act_param_temp, act_param, sizeof(struct rtw_csi_action_parm));
	DCache_Clean((u32)act_param_temp, sizeof(struct rtw_csi_action_parm));

	param_buf[0] = (u32)act_param_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_CONFIG_CSI, param_buf, 1);

	rtos_mem_free(act_param_temp);
	return ret;
}

s32 wifi_radar_config(struct rtw_radar_action_parm *act_param)
{
	int ret = 0;
	u32 param_buf[1];

	struct rtw_radar_action_parm *act_param_temp = (struct rtw_radar_action_parm *)rtos_mem_zmalloc(sizeof(struct rtw_radar_action_parm));
	if (act_param_temp == NULL) {
		return -1;
	}
	memcpy(act_param_temp, act_param, sizeof(struct rtw_radar_action_parm));
	DCache_Clean((u32)act_param_temp, sizeof(struct rtw_radar_action_parm));

	param_buf[0] = (u32)act_param_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_CONFIG_RADAR, param_buf, 1);

	rtos_mem_free(act_param_temp);
	return ret;
}

s32 wifi_radar_send_data(u16 frame_num, u8 frame_type, u8 *data)
{
	int ret = 0;
	u32 param_buf[3];
	u32 data_size;
	static u8 radar_send_data_buf[RADAR_SEND_DATA_BUF_SIZE];

	/* Calculate actual data size based on frame type.
	 * Near/Far: 44 IQ samples (s16 I + s16 Q) per frame.
	 * AI_L_NEAR: 6 IQ samples per frame. */
	if (frame_type == RTW_RADAR_TYPE_AI_L_NEAR) {
		data_size = 6 * 4 * frame_num;
	} else {
		data_size = 44 * 4 * frame_num;
	}

	if (data_size > RADAR_SEND_DATA_BUF_SIZE) {
		RTK_LOGE(NOTAG, "[radar] send_data buf overflow: %u > %u\n",
				 data_size, RADAR_SEND_DATA_BUF_SIZE);
		return -1;
	}

	memcpy(radar_send_data_buf, data, data_size);
	DCache_Clean((u32)radar_send_data_buf, data_size);

	param_buf[0] = (u32)frame_num;
	param_buf[1] = (u32)frame_type;
	param_buf[2] = (u32)radar_send_data_buf;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_RADAR_SEND_DATA, param_buf, 3);

	return ret;
}

//----------------------------------------------------------------------------//

int wifi_set_wpa_mode(u8 wpa_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)wpa_mode;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_WPA_MODE, param_buf, 1);
	return ret;
}

int wifi_set_pmf_mode(u8 pmf_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)pmf_mode;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_PMF_MODE, param_buf, 1);
	return ret;
}

void wifi_wpa_4way_status_indicate(struct rtw_wpa_4way_status *rpt_4way)
{
	u32 param_buf[1] = {0};
	struct rtw_wpa_4way_status *rpt_4way_temp = (struct rtw_wpa_4way_status *)rtos_mem_zmalloc(sizeof(struct rtw_wpa_4way_status));

	if (rpt_4way_temp == NULL) {
		return;
	}
	memcpy(rpt_4way_temp, rpt_4way, sizeof(struct rtw_wpa_4way_status));

	DCache_Clean((u32)rpt_4way_temp, sizeof(struct rtw_wpa_4way_status));
	param_buf[0] = (u32)rpt_4way_temp;
	whc_ipc_host_api_message_send(WHC_API_WPA_4WAY_REPORT, param_buf, 1);

	rtos_mem_free((u8 *)rpt_4way_temp);
}

void wifi_dhcp_success_indicate(void)
{
	whc_ipc_host_api_message_send(WHC_API_WIFI_DHCP_SUCCESS_IND, NULL, 0);
}

int wifi_wpa_add_key(struct rtw_crypt_info *crypt)
{
	u32 param_buf[1] = {0};
	int ret = 0;

	struct rtw_crypt_info *crypt_temp = (struct rtw_crypt_info *)rtos_mem_zmalloc(sizeof(struct rtw_crypt_info));

	if (crypt_temp == NULL) {
		return -1;
	}
	memcpy(crypt_temp, crypt, sizeof(struct rtw_crypt_info));

	DCache_Clean((u32)crypt_temp, sizeof(struct rtw_crypt_info));
	param_buf[0] = (u32)crypt_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_ADD_KEY, param_buf, 1);

	rtos_mem_free((u8 *)crypt_temp);
	return ret;
}

void wifi_wpa_pmksa_ops(struct rtw_pmksa_ops_t *pmksa_ops)
{
	u32 param_buf[1] = {0};
	struct rtw_pmksa_ops_t *pmksa_ops_temp = (struct rtw_pmksa_ops_t *)rtos_mem_zmalloc(sizeof(struct rtw_pmksa_ops_t));
	if (pmksa_ops_temp == NULL) {
		return;
	}
	memcpy(pmksa_ops_temp, pmksa_ops, sizeof(struct rtw_pmksa_ops_t));

	DCache_Clean((u32)pmksa_ops_temp, sizeof(struct rtw_pmksa_ops_t));
	param_buf[0] = (u32)pmksa_ops_temp;
	whc_ipc_host_api_message_send(WHC_API_WPA_PMKSA_OPS, param_buf, 1);

	rtos_mem_free((u8 *)pmksa_ops_temp);
}

int wifi_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	u32 param_buf[3] = {0};
	u8 *mac_addr_temp = NULL;

	if (mac_addr) {
		mac_addr_temp = (u8 *)rtos_mem_zmalloc(ETH_ALEN);
		if (mac_addr_temp == NULL) {
			return -1;
		}
		memcpy(mac_addr_temp, mac_addr, ETH_ALEN);
		DCache_Clean((u32)mac_addr_temp, ETH_ALEN);
	}

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)status;
	param_buf[2] = (u32)mac_addr_temp;

	whc_ipc_host_api_message_send(WHC_API_WIFI_SAE_STATUS, param_buf, 3);

	if (mac_addr_temp) {
		rtos_mem_free(mac_addr_temp);
	}
	return 0;
}

int wifi_start_join_cmd(void)
{
	int ret;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_START_JOIN_CMD, NULL, 0);

	return ret;
}

int wifi_ft_status_indicate(struct rtw_kvr_param_t *kvr_param, u16 status)
{
#ifdef CONFIG_IEEE80211R
	u32 param_buf[2] = {0};
	struct rtw_kvr_param_t *kvr_param_temp = NULL;

	if (kvr_param) {
		kvr_param_temp = (struct rtw_kvr_param_t *)rtos_mem_zmalloc(sizeof(struct rtw_kvr_param_t));
		if (kvr_param_temp == NULL) {
			return -1;
		}
		memcpy(kvr_param_temp, kvr_param, sizeof(struct rtw_kvr_param_t));
		DCache_Clean((u32)kvr_param_temp, sizeof(struct rtw_kvr_param_t));
		param_buf[0] = (u32)kvr_param_temp;
		param_buf[1] = (u32)status;
	}
	whc_ipc_host_api_message_send(WHC_API_WIFI_FT_STATUS, param_buf, 2);

	if (kvr_param_temp) {
		rtos_mem_free((u8 *)kvr_param_temp);
	}
	return 0;
#else
	UNUSED(kvr_param);
	UNUSED(status);
	return -1;
#endif
}

s32 wifi_send_raw_frame(struct rtw_raw_frame_desc *raw_frame_desc)
{
	int ret;
	int idx = 0;
	struct skb_raw_para raw_para;

	struct eth_drv_sg sg_list[2];
	int sg_len = 0;

	if (raw_frame_desc == NULL) {
		return -RTK_ERR_BADARG;
	}

	raw_para.rate = raw_frame_desc->tx_rate;
	raw_para.retry_limit = raw_frame_desc->retry_limit;
	raw_para.ac_queue = raw_frame_desc->ac_queue;
	raw_para.sgi = raw_frame_desc->sgi;
	raw_para.agg_en = raw_frame_desc->agg_en;
	raw_para.device_id = raw_frame_desc->device_id;
	raw_para.bw_40_en = raw_frame_desc->bw_40_en;

	idx = raw_frame_desc->wlan_idx;

	sg_list[sg_len].buf = (unsigned int)raw_frame_desc->buf;
	sg_list[sg_len++].len = raw_frame_desc->buf_len;
	ret = whc_ipc_host_send(idx, sg_list, sg_len, raw_frame_desc->buf_len, &raw_para, 0);

	if (ret == -2) { /* convert to definition of WIFI_ERR */
		ret = -RTK_ERR_WIFI_TX_BUF_FULL;
	}

	return ret;
}

s32 wifi_get_ccmp_key(u8 wlan_idx, u8 *mac_addr, u8 *uncst_key, u8 *group_key)
{
	int ret;
	u32 param_buf[4] = {0};
	unsigned char *uncst_key_temp = NULL;
	unsigned char *group_key_temp = NULL;

	uncst_key_temp = (unsigned char *)rtos_mem_zmalloc(16);
	group_key_temp = (unsigned char *)rtos_mem_zmalloc(16);
	if (uncst_key_temp == NULL || group_key_temp == NULL) {
		return -1;
	}
	DCache_Clean((u32)uncst_key_temp, 16);
	DCache_Clean((u32)group_key_temp, 16);
	if (mac_addr) {
		DCache_Clean((u32)mac_addr, 6);
	}
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)mac_addr;
	param_buf[2] = (u32)uncst_key_temp;
	param_buf[3] = (u32)group_key_temp;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_CCMP_KEY, param_buf, 4);
	DCache_Invalidate((u32)uncst_key_temp, 16);
	DCache_Invalidate((u32)group_key_temp, 16);
	memcpy(uncst_key, uncst_key_temp, 16);
	memcpy(group_key, group_key_temp, 16);

	rtos_mem_free(uncst_key_temp);
	rtos_mem_free(group_key_temp);
	return ret;
}

void wifi_speaker_setting(u8 set_type, union rtw_speaker_set *settings)
{
	u32 param_buf[2] = {0};
	union rtw_speaker_set *settings_temp = (union rtw_speaker_set *)rtos_mem_zmalloc(sizeof(union rtw_speaker_set));
	if (settings_temp == NULL) {
		return;
	}
	memcpy(settings_temp, settings, sizeof(union rtw_speaker_set));
	DCache_Clean((u32)settings_temp, sizeof(union rtw_speaker_set));

	param_buf[0] = (u32)set_type;
	param_buf[1] = (u32)settings_temp;
	whc_ipc_host_api_message_send(WHC_API_WIFI_SPEAKER, param_buf, 2);

	rtos_mem_free((u8 *)settings_temp);
}

void wifi_set_owe_param(struct rtw_owe_param_t *owe_param)
{
#ifdef CONFIG_OWE_SUPPORT
	u32 param_buf[1] = {0};

	struct rtw_owe_param_t *owe_param_temp = (struct rtw_owe_param_t *)rtos_mem_zmalloc(sizeof(struct rtw_owe_param_t));
	if (owe_param_temp == NULL) {
		return;
	}
	memcpy(owe_param_temp, owe_param, sizeof(struct rtw_owe_param_t));

	DCache_Clean((u32)owe_param_temp, sizeof(struct rtw_owe_param_t));
	param_buf[0] = (u32)owe_param_temp;
	whc_ipc_host_api_message_send(WHC_API_WIFI_SET_OWE_PARAM, param_buf, 1);

	rtos_mem_free((u8 *)owe_param_temp);
#else
	UNUSED(owe_param);
#endif
}

s32 wifi_set_tx_power(struct rtw_tx_power_ctl_info *txpwr_ctrl_info)
{
	int ret = 0;
	u32 param_buf[1];
	struct rtw_tx_power_ctl_info *txpwr_temp = (struct rtw_tx_power_ctl_info *)rtos_mem_zmalloc(sizeof(struct rtw_tx_power_ctl_info));

	if (txpwr_temp == NULL) {
		return -1;
	}
	memcpy(txpwr_temp, txpwr_ctrl_info, sizeof(struct rtw_tx_power_ctl_info));

	DCache_Clean((u32)txpwr_temp, sizeof(struct rtw_tx_power_ctl_info));
	param_buf[0] = (u32)txpwr_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_TX_POWER, param_buf, 1);

	rtos_mem_free((u8 *)txpwr_temp);
	return ret;
}

s32 wifi_get_tx_power(u8 rate, s8 *txpwr)
{
	int ret = 0;
	u32 param_buf[2];
	s8 *txpwr_temp = (s8 *)rtos_mem_zmalloc(sizeof(int));

	if (txpwr_temp == NULL) {
		return -1;
	}

	param_buf[0] = (u32)rate;
	param_buf[1] = (u32)txpwr_temp;
	DCache_CleanInvalidate((u32)txpwr_temp, sizeof(int));

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_GET_TX_POWER, param_buf, 2);
	DCache_Invalidate((u32)txpwr_temp, sizeof(int));
	*txpwr = *txpwr_temp;
	rtos_mem_free(txpwr_temp);

	return ret;
}

void wifi_set_conn_step_try_limit(struct rtw_conn_step_retries *conn_step_retries)
{
	u32 param_buf[1] = {0};
	struct rtw_conn_step_retries *conn_step_retries_temp = (struct rtw_conn_step_retries *)rtos_mem_zmalloc(sizeof(struct rtw_conn_step_retries));
	if (conn_step_retries_temp == NULL) {
		return;
	}
	memcpy(conn_step_retries_temp, conn_step_retries, sizeof(struct rtw_conn_step_retries));

	DCache_Clean((u32)conn_step_retries_temp, sizeof(struct rtw_conn_step_retries));
	param_buf[0] = (u32)conn_step_retries_temp;
	whc_ipc_host_api_message_send(WHC_API_WIFI_SET_CONN_STEP_TRY_LIMIT, param_buf, 1);

	rtos_mem_free((u8 *)conn_step_retries_temp);
}

int wifi_wake_pll_rdy_in_ps_state(u8 need)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)need;

	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_WAKE_PLL_RDY_IN_PS, param_buf, 1);

	return ret;
}

void wifi_ps_en_by_bt_state(u8 enable)
{
#ifndef CONFIG_MP_SHRINK
	u32 param_buf[1];
	param_buf[0] = (u32)enable;

	whc_ipc_host_api_message_send(WHC_API_WIFI_PS_EN_BY_BT_ON, param_buf, 1);
#else
	UNUSED(enable);
#endif
}

s32 wifi_set_tx_advanced_config(struct rtw_tx_advanced_cfg *tx_setting)
{
	s32 ret = 0;
	u32 param_buf[1] = {0};
	struct rtw_tx_advanced_cfg *settings_temp = (struct rtw_tx_advanced_cfg *)rtos_mem_zmalloc(sizeof(struct rtw_tx_advanced_cfg));
	if (settings_temp == NULL) {
		return -RTK_ERR_NOMEM;
	}

	memcpy(settings_temp, tx_setting, sizeof(struct rtw_tx_advanced_cfg));
	DCache_Clean((u32)settings_temp, sizeof(struct rtw_tx_advanced_cfg));

	param_buf[0] = (u32)settings_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_TX_ADVANCED_CFG, param_buf, 1);

	rtos_mem_free((u8 *)settings_temp);
	return ret;
}

s32 wifi_tsf_sync_to_user_target(u8 enable, u8 *mac_addr)
{
	s32 ret = 0;
	u32 param_buf[2] = {0};

	unsigned char *mac_temp = (unsigned char *)rtos_mem_zmalloc(ETH_ALEN);
	if (mac_temp == NULL) {
		return -RTK_ERR_NOMEM;
	}
	memcpy(mac_temp, mac_addr, ETH_ALEN);
	DCache_Clean((u32)mac_temp, ETH_ALEN);

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)mac_temp;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_TSF_SYNC_TO_TARGET, param_buf, 2);

	rtos_mem_free((u8 *)mac_temp);
	return ret;
}

s32 wifi_set_p1_to_p0_tsf_offset(u16 offset_us)
{
	s32 ret = 0;
	u32 param_buf[1] = {0};

	param_buf[0] = (u32)offset_us;
	ret = whc_ipc_host_api_message_send(WHC_API_WIFI_SET_P1_TO_P0_TSF_OFFSET, param_buf, 2);

	return ret;
}

#ifdef CONFIG_WIFI_TUNNEL
void wifi_rmesh_register_ota_callback(int (*ota_callback)(u8 *buf, u16 len))
{
	u32 param_buf[1];
	param_buf[0] = (u32)ota_callback;
	rmesh_ota_user_callback = ota_callback;

	whc_host_api_message_send(WHC_API_WTN_OTA_CALLBACK_REGISTER, param_buf, 1);
}

int wifi_rmesh_get_node_info(enum rtw_rmesh_node_type type, struct rtw_rmesh_node_info *node_info)
{
	int ret = RTK_SUCCESS;
	u32 param_buf[2];
	struct rtw_rmesh_node_info *node_info_tmp = NULL;

	node_info_tmp = (struct rtw_rmesh_node_info *)rtos_mem_zmalloc(sizeof(struct rtw_rmesh_node_info));
	if (node_info_tmp == NULL) {
		return RTK_FAIL;
	}
	DCache_Clean((u32)node_info_tmp, sizeof(struct rtw_rmesh_node_info));

	param_buf[0] = (u32)type;
	param_buf[1] = (u32)node_info_tmp;
	ret = whc_ipc_host_api_message_send(WHC_API_WTN_GET_NODE_INFO, param_buf, 2);
	DCache_Invalidate((u32)node_info_tmp, sizeof(struct rtw_rmesh_node_info));
	memcpy(node_info, node_info_tmp, sizeof(struct rtw_rmesh_node_info));
	rtos_mem_free(node_info_tmp);

	return ret;
}

u8 wifi_rmesh_get_child_num(void)
{
	return whc_ipc_host_api_message_send(WHC_API_WTN_GET_CHILD_NUM, NULL, 0);
}


int wifi_rmesh_get_child_info_list(u8 *child_num, struct rtw_rmesh_node_info *child_info_list)
{
	int ret = RTK_SUCCESS;
	u32 param_buf[2];

	u8 *child_num_temp = (u8 *)rtos_mem_zmalloc(sizeof(u8));
	if (child_num_temp == NULL) {
		return RTK_FAIL;
	}
	*child_num_temp = *child_num;

	u8 *child_list_temp = (u8 *)rtos_mem_zmalloc((*child_num) * sizeof(struct rtw_rmesh_node_info));
	if (child_list_temp == NULL) {
		rtos_mem_free(child_num_temp);
		return RTK_FAIL;
	}

	param_buf[0] = (u32)child_num_temp;
	param_buf[1] = (u32)child_list_temp;
	DCache_CleanInvalidate((u32)child_num_temp, sizeof(u8));
	DCache_CleanInvalidate((u32)child_list_temp, (*child_num) * sizeof(struct rtw_rmesh_node_info));

	ret = whc_ipc_host_api_message_send(WHC_API_WTN_GET_CHILD_MAC_LIST, param_buf, 2);
	DCache_Invalidate((u32)child_num_temp, sizeof(u8));
	DCache_Invalidate((u32)child_list_temp, (*child_num)*sizeof(struct rtw_rmesh_node_info));
	*child_num = *child_num_temp;
	memcpy(child_info_list, child_list_temp, ((*child_num)*sizeof(struct rtw_rmesh_node_info)));

	rtos_mem_free((u8 *)child_num_temp);
	rtos_mem_free((u8 *)child_list_temp);
	return ret;
}

int wifi_rmesh_update_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, u8 ota_ongoing)
{
	int ret = RTK_FAIL;
	u32 param_buf[3];

	u8 *ota_ver_tmp = (u8 *)rtos_mem_zmalloc(ota_ver_len);
	if (ota_ver_tmp == NULL) {
		return RTK_FAIL;
	}
	memcpy(ota_ver_tmp, ota_ver, ota_ver_len);
	DCache_CleanInvalidate((u32)ota_ver_tmp, ota_ver_len);
	param_buf[0] = (u32)ota_ver_tmp;
	param_buf[1] = (u32)ota_ver_len;
	param_buf[2] = (u32)ota_ongoing;

	ret = whc_ipc_host_api_message_send(WHC_API_WTN_UPDATE_OTA_VER, param_buf, 3);

	rtos_mem_free(ota_ver_tmp);
	return ret;
}

int wifi_rmesh_check_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, enum rtw_rmesh_node_type type)
{
	int ret = RTK_FAIL;
	u32 param_buf[3];

	u8 *ota_ver_tmp = (u8 *)rtos_mem_zmalloc(ota_ver_len);
	if (ota_ver_tmp == NULL) {
		return RTK_FAIL;
	}
	memcpy(ota_ver_tmp, ota_ver, ota_ver_len);
	DCache_CleanInvalidate((u32)ota_ver_tmp, ota_ver_len);
	param_buf[0] = (u32)ota_ver_tmp;
	param_buf[1] = (u32)ota_ver_len;
	param_buf[2] = (u32)type;

	ret = whc_ipc_host_api_message_send(WHC_API_WTN_CHECK_OTA_VER, param_buf, 3);

	rtos_mem_free(ota_ver_tmp);
	return ret;
}
#endif

#ifdef CONFIG_P2P
void wifi_p2p_set_role(u32 role)
{
	u32 param_buf[1];

	param_buf[0] = role;

	whc_ipc_host_api_message_send(WHC_API_P2P_ROLE, param_buf, 1);
}

int wifi_p2p_set_remain_on_ch(unsigned char wlan_idx, u8 enable)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)enable;

	ret = whc_ipc_host_api_message_send(WHC_API_P2P_REMAIN_ON_CH, param_buf, 2);
	return ret;
}

#endif

#endif	//#if CONFIG_WLAN
