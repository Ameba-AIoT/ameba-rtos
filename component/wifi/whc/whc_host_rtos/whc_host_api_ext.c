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
#include "whc_host.h"
#include <rtw_timer.h>
#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>

extern u8 rtw_join_status;

/******************************************************
 *                    Constants
 ******************************************************/
#define RTW_SCAN_ABORT_TIMEOUT (20)

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Variables Declarations
 ******************************************************/

struct internal_block_param *scan_abort_block_param = NULL;

#ifdef CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif

void (*p_ap_channel_switch_callback)(u8 channel, s8 ret) = NULL;


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
//----------------------------------------------------------------------------//
s32 wifi_set_channel(u8 wlan_idx, u8 channel)
{
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)channel;

	whc_host_api_message_send(WHC_API_WIFI_SET_CHANNEL, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_set_countrycode(u8 *cntcode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)cntcode;

	whc_host_api_message_send(WHC_API_WIFI_SET_COUNTRY_CODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_chplan(u8 chplan)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)chplan;

	whc_host_api_message_send(WHC_API_WIFI_SET_CHPLAN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_get_channel_list(struct rtw_channel_list *ch_list);
{
	int ret = 0;

	whc_host_api_message_send(WHC_API_WIFI_GET_CHANNEL_LIST, NULL, 0, (u8 *)ch_list, sizeof(struct rtw_channel_list));
	return ret;
}

//----------------------------------------------------------------------------//

_OPTIMIZE_NONE_
s32 wifi_get_scan_records(u32 *ap_num, struct rtw_scan_result *ap_list)
{
	int ret = 0;
	u32 param_buf[2];

	/* SPI bufsize can only 24 */
	if (*ap_num >= 20) {
		*ap_num = 20;
	}

	param_buf[0] = (u32)(*ap_num);
	param_buf[1] = (u32)ap_list;

	whc_host_api_message_send(WHC_API_WIFI_GET_SCANNED_AP_INFO, (u8 *)param_buf, 8, (u8 *)ap_list, (*ap_num)*sizeof(struct rtw_scan_result));

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

	whc_host_api_message_send(WHC_API_WIFI_SCAN_ABORT, NULL, 0, (u8 *)&ret, sizeof(ret));

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

	param_buf[0] = idx;
	param_buf[2] = efuse;
	whc_host_api_message_send(WHC_API_WIFI_GET_MAC_ADDR, (u8 *)param_buf, 8, (u8 *)mac, sizeof(struct rtw_mac));

	return ret;
}

int wifi_set_mac_address(int idx, unsigned char *mac, u8 efuse)
{
	int ret = 0;
	u32 param_buf[4];

	param_buf[0] = idx;
	param_buf[1] = (u32)efuse;
	memcpy((void *)&param_buf[2], (void *)mac, 6);
	whc_host_api_message_send(WHC_API_WIFI_SET_MAC_ADDR, (u8 *)param_buf, 16, (u8 *)&ret, sizeof(ret));

	return ret;
}
//----------------------------------------------------------------------------//
u8 wifi_driver_is_mp(void)
{
	int ret = 0;

	whc_host_api_message_send(WHC_API_WIFI_DRIVE_IS_MP, NULL, 0, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_ap_get_connected_clients(struct rtw_client_list *client_list_buffer)
{
	int ret = 0;

	whc_host_api_message_send(WHC_API_WIFI_AP_GET_CONNECTED_CLIENTS, NULL, 0, (u8 *)client_list_buffer, sizeof(struct rtw_client_list));
	return ret;
}

s32 wifi_ap_del_client(u8 *hwaddr)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)IFACE_PORT1;
	memcpy((void *)(&param_buf[1]), (void *)hwaddr, 6);
	whc_host_api_message_send(WHC_API_WIFI_AP_DEL_CLIENT, (u8 *)param_buf, 12, (u8 *)&ret, sizeof(ret));
	return ret;

}

s32 wifi_ap_switch_chl_and_inform(struct rtw_csa_parm *csa_param)
{
	int ret = 0;
	char *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_csa_parm));
	if (!param_buf) {
		return -1;
	}

	p_ap_channel_switch_callback = csa_param->callback;
	memcpy(param_buf, (void *)csa_param, sizeof(struct rtw_csa_parm));

	whc_host_api_message_send(WHC_API_WIFI_AP_CH_SWITCH, (u8 *)param_buf, sizeof(struct rtw_csa_parm), (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

//----------------------------------------------------------------------------//
s32 wifi_get_setting(u8 wlan_idx, struct rtw_wifi_setting *psetting)
{
	int ret = 0;
	u32 param_buf[2];


	param_buf[0] = (u32)wlan_idx;

	whc_host_api_message_send(WHC_API_WIFI_GET_SETTING, (u8 *)param_buf, 4, (u8 *)psetting, sizeof(struct rtw_wifi_setting));

	return ret;
}

int wifi_set_ips_internal(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	whc_host_api_message_send(WHC_API_WIFI_SET_IPS_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_set_lps_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	whc_host_api_message_send(WHC_API_WIFI_SET_LPS_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_set_lps_listen_interval(u8 interval)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)interval;
	whc_host_api_message_send(WHC_API_WIFI_SET_LPS_LISTEN_INTERVAL, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//

int wifi_set_mfp_support(unsigned char value)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)value;
	whc_host_api_message_send(WHC_API_WIFI_SET_MFP_SUPPORT, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
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
	whc_host_api_message_send(WHC_API_WIFI_SET_PMK_CACHE_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_get_traffic_stats(u8 wlan_idx, union rtw_traffic_stats *traffic_stats)
{
	int ret = 0;
	u32 param_buf[1] = {0};

	param_buf[0] = (u32)wlan_idx;

	whc_host_api_message_send(WHC_API_WIFI_GET_TRAFFIC_STATS, (u8 *)param_buf, 4, (u8 *)traffic_stats, sizeof(union rtw_traffic_stats));

	return ret;
}

s32 wifi_get_phy_stats(u8 wlan_idx, u8 *mac_addr, union rtw_phy_stats *phy_stats)
{
	int ret = 0;
	u32 param_buf[4] = {0};
	u32 len = 0;

	param_buf[0] = (u32)wlan_idx;
	len = 4;
	if (mac_addr) {
		param_buf[1] = 6;
		memcpy((void *)&param_buf[2], (void *)mac_addr, 6);
		len += 10;
	} else {
		param_buf[1] = 0;
		len += 4;
	}

	whc_host_api_message_send(WHC_API_WIFI_GET_PHY_STATS, (u8 *)param_buf, len, (u8 *)phy_stats, sizeof(union rtw_phy_stats));
	return ret;
}

s32 wifi_get_wireless_mode(u8 *wmode)
{
	int ret = 0;
	whc_host_api_message_send(WHC_API_WIFI_GET_WIRELESS_MODE, NULL, 0, (u8 *)wmode, sizeof(u8));
	return ret;
}

s32 wifi_set_wireless_mode(u32 wmode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)wmode;
	whc_host_api_message_send(WHC_API_WIFI_SET_WIRELESS_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_wps_phase(unsigned char is_trigger_wps)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = is_trigger_wps;

	whc_host_api_message_send(WHC_API_WIFI_SET_WPS_PHASE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 *param_buf  = rtos_mem_zmalloc(12 + buf_len);

	if (!param_buf) {
		return -1;
	}
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)buf_len;
	param_buf[2] = (u32)flags;

	memcpy((void *)&param_buf[3], buf, buf_len);
	whc_host_api_message_send(WHC_API_WIFI_SET_GEN_IE, (u8 *)param_buf, 12 + buf_len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

int wifi_set_eap_phase(unsigned char is_trigger_eap)
{
#ifdef CONFIG_EAP
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = is_trigger_eap;

#if CONFIG_AUTO_RECONNECT
	if (is_trigger_eap == 0) {
		rtw_reconn.eap_method = 0;
	}
#endif

	whc_host_api_message_send(WHC_API_WIFI_SET_EAP_PHASE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
#else
	return -1;
#endif
}

unsigned char wifi_get_eap_phase(void)
{
#ifdef CONFIG_EAP
	unsigned char eap_phase = 0;

	whc_host_api_message_send(WHC_API_WIFI_GET_EAP_PHASE, NULL, 0, &eap_phase, sizeof(eap_phase));
	return eap_phase;
#else
	return 0;
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

	whc_host_api_message_send(WHC_API_WIFI_SET_EAP_METHOD, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
#else
	return -1;
#endif
}

int wifi_if_send_eapol(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 *param_buf;
	int size = 12 + buf_len;
	param_buf = (u32 *)rtos_mem_zmalloc(size);

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)flags;
	param_buf[2] = buf_len;
	memcpy(&(param_buf[3]), buf, buf_len);

	whc_host_api_message_send(WHC_API_WIFI_SEND_EAPOL, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));
	rtos_mem_free(param_buf);
	return ret;
}

void wifi_wpa_4way_status_indicate(struct rtw_wpa_4way_status *rpt_4way)
{
	u32 *param_buf;
	int size = sizeof(struct rtw_wpa_4way_status);
	param_buf = (u32 *)rtos_mem_zmalloc(size);
	if (param_buf) {
		memcpy((void *)param_buf, (void *)rpt_4way, size);
		whc_host_api_message_send(WHC_API_WPA_4WAY_REPORT, (u8 *)param_buf, size, NULL, 0);
		rtos_mem_free(param_buf);
	}
}

//----------------------------------------------------------------------------//
/*
 * Example for custom ie
 *
 * u8 test_1[] = {221, 2, 2, 2};
 * u8 test_2[] = {221, 2, 1, 1};
 * struct rtw_custom_ie buf[2] = {{test_1, RTW_CUS_IE_BEACON},
 *		 {test_2, RTW_CUS_IE_PROBERSP}};
 * u8 buf_test2[] = {221, 2, 1, 3} ;
 * struct rtw_custom_ie buf_update = {buf_test2, RTW_CUS_IE_PROBERSP};
 *
 * add ie list
 * static void cmd_add_ie(int argc, char **argv)
 * {
 *	 wifi_add_custom_ie(buf, 2);
 * }
 *
 * update current ie
 * static void cmd_update_ie(int argc, char **argv)
 * {
 *	 wifi_update_custom_ie(&buf_update, 2);
 * }
 *
 * delete all ie for specific wlan interface
 * static void cmd_del_ie(int argc, char **argv)
 * {
 *	 wifi_del_custom_ie(SOFTAP_WLAN_INDEX);
 * }
 */

s32 wifi_add_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_num)
{
	int ret = 0;
	u8 *param_buf, *ptr;
	u32 size = 0;
	u8 i = 0;
	struct rtw_custom_ie *pcus_ie = cus_ie;

	size += 2;
	for (i = 0; i < ie_num; i++) {
		size += 3 + pcus_ie->ie[1];
		pcus_ie = pcus_ie + 1;
	}

	ptr = param_buf = (u8 *)rtos_mem_zmalloc(size);
	ptr[0] = 0;
	ptr[1] = ie_num;
	ptr += 2;

	for (i = 0; i < ie_num; i++) {
		ptr[0] = pcus_ie->type;
		//ptr[1] = pcus_ie->ie[0];
		//ptr[2] = pcus_ie->ie[1];
		memcpy((void *)(ptr + 1), pcus_ie->ie, pcus_ie->ie[1] + 2);
		ptr += 3 + pcus_ie->ie[1];
		pcus_ie = pcus_ie + 1;
	}

	whc_host_api_message_send(WHC_API_WIFI_CUS_IE, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

s32 wifi_update_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_index)
{
	int ret = 0;
	u8 *ptr, *param_buf;
	u32 size = 0;
	struct rtw_custom_ie *pcus_ie = cus_ie;

	size = 3 + 2 + pcus_ie->ie[1];
	ptr = param_buf = (u8 *)rtos_mem_zmalloc(size);

	ptr[0] = 1;
	ptr[1] = (u8)ie_index;
	ptr[2] = pcus_ie->type;
	ptr += 3;

	memcpy((void *)ptr, pcus_ie->ie, 2 + pcus_ie->ie[1]);

	whc_host_api_message_send(WHC_API_WIFI_CUS_IE, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

s32 wifi_del_custom_ie(u8 wlan_idx)
{
	int ret;
	u8 param_buf[2];

	param_buf[0] = 2;//type 2 means delete
	param_buf[1] = (u8)wlan_idx;
	whc_host_api_message_send(WHC_API_WIFI_CUS_IE, (u8 *)param_buf, 2, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_send_mgnt(struct _raw_data_desc_t *raw_data_desc)
{
	int ret = 0;
	u32 *param_buf;
	/* 3 param + buf len */
	int len = 12 + raw_data_desc->buf_len;

	param_buf = (u32 *)rtos_mem_zmalloc(len);

	if (!param_buf) {
		ret = -1;
		return ret;
	}

	param_buf[0] = (u32)raw_data_desc->wlan_idx;
	param_buf[1] = (u32)raw_data_desc->buf_len;
	param_buf[2] = (u32)raw_data_desc->flags;
	memcpy((void *)(&param_buf[3]), raw_data_desc->buf, raw_data_desc->buf_len);

	whc_host_api_message_send(WHC_API_WIFI_SEND_MGNT, (u8 *)param_buf, len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

s32 wifi_set_tx_rate_by_tos(u8 enable, u8 tos_precedence, u8 tx_rate)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)tos_precedence;
	param_buf[2] = (u32)tx_rate;
	whc_host_api_message_send(WHC_API_WIFI_SET_TXRATE_BY_TOS, (u8 *)param_buf, 12, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_set_edca_param(u32 ac_param)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = ac_param;
	whc_host_api_message_send(WHC_API_WIFI_SET_EDCA_PARAM, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_set_tx_cca_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;
	whc_host_api_message_send(WHC_API_WIFI_SET_TX_CCA_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

s32 wifi_set_cts2self_duration_and_send(u8 wlan_idx, u16 duration)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)duration;
	whc_host_api_message_send(WHC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
	return ret;

}

s32 wifi_get_antdiv_info(u8 *antdiv_mode, u8 *curr_ant)
{
#ifdef CONFIG_WIFI_ANTDIV
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = (u32)antdiv_mode;
	param_buf[1] = (u32)curr_ant;
	whc_host_api_message_send(WHC_API_WIFI_GET_ANTENNA_INFO, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
	return ret;
#else
	UNUSED(antdiv_mode);
	UNUSED(curr_ant);
	return -1;
#endif
}

/*
 * @brief get WIFI band type
 *@retval  the support band type.
 * 	RTW_SUPPORT_BAND_2_4G: only support 2.4G
 *	RTW_SUPPORT_BAND_5G: only support 5G
 *  RTW_SUPPORT_BAND_2_4G_5G_BOTH: support both 2.4G and 5G
 */
s32 wifi_get_band_type(u8 *band_type)
{
	whc_host_api_message_send(WHC_API_WIFI_GET_BAND_TYPE, NULL, 0, (u8 *)band_type, sizeof(band_type));
	return RTK_SUCCESS;
}

//----------------------------------------------------------------------------//
s32 wifi_csi_config(struct rtw_csi_action_parm *act_param)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)act_param;
	whc_host_api_message_send(WHC_API_WIFI_CONFIG_CSI, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//

int wifi_set_wpa_mode(u8 wpa_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)wpa_mode;
	whc_host_api_message_send(WHC_API_WIFI_SET_WPA_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_pmf_mode(u8 pmf_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)pmf_mode;
	whc_host_api_message_send(WHC_API_WIFI_SET_PMF_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

void wifi_wpa_add_key(struct rtw_crypt_info *crypt)
{
	char *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_crypt_info));
	memcpy(param_buf, (void *)crypt, sizeof(struct rtw_crypt_info));

	whc_host_api_message_send(WHC_API_WIFI_ADD_KEY, (u8 *)param_buf, sizeof(struct rtw_crypt_info), NULL, 0);
	rtos_mem_free(param_buf);
}

void wifi_wpa_pmksa_ops(struct rtw_pmksa_ops_t *pmksa_ops)
{
	char *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_pmksa_ops_t));
	memcpy(param_buf, (void *)pmksa_ops, sizeof(struct rtw_pmksa_ops_t));
	whc_host_api_message_send(WHC_API_WPA_PMKSA_OPS, (u8 *)param_buf, sizeof(struct rtw_pmksa_ops_t), NULL, 0);

	rtos_mem_free(param_buf);
}

int wifi_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	u32 param_buf[5] = {0};
	u32 len = 0;

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)status;
	len = 8;

	if (mac_addr) {
		param_buf[2] = 6;
		memcpy((void *)&param_buf[3], (void *)mac_addr, 6);
		len += 10;
	} else {
		param_buf[2] = 0;
		len += 4;
	}

	whc_host_api_message_send(WHC_API_WIFI_SAE_STATUS, (u8 *)param_buf, len, NULL, 0);
	return 0;
}

s32 wifi_send_raw_frame(struct rtw_raw_frame_desc *raw_frame_desc)
{
	int ret;
	int idx = 0;
	struct skb_raw_para raw_para;

	struct eth_drv_sg sg_list[2];
	int sg_len = 0;

	if (raw_frame_desc == NULL) {
		return -1;
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
	ret = whc_host_send(idx, sg_list, sg_len, raw_frame_desc->buf_len, &raw_para, 0);

	return ret;
}


void wifi_speaker_setting(u8 set_type, union rtw_speaker_set *settings)
{
#ifdef CONFIG_WIFI_SPEAKER_ENABLE
	u32 param_buf[2] = {0};

	param_buf[0] = (u32)set_type;

	DCache_Clean((u32)settings, sizeof(union rtw_speaker_set));
	param_buf[1] = (u32)settings;
	whc_host_api_message_send(WHC_API_WIFI_SPEAKER, (u8 *)param_buf, 8, NULL, 0);
#else
	UNUSED(set_type);
	UNUSED(settings);
#endif
}

void wifi_set_owe_param(struct rtw_owe_param_t *owe_param)
{
#ifdef CONFIG_OWE_SUPPORT
	u32 param_buf[1] = {0};

	DCache_Clean((u32)owe_param, sizeof(struct rtw_owe_param_t));
	param_buf[0] = (u32)owe_param;
	whc_host_api_message_send(WHC_API_WIFI_SET_OWE_PARAM, (u8 *)param_buf, 4, NULL, 0);
#else
	UNUSED(owe_param);
#endif
}

#endif	//#if CONFIG_WLAN

