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
#include "inic_host.h"
#include <rtw_timer.h>
#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>

extern u8 rtw_join_status;

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Variables Declarations
 ******************************************************/

#ifdef CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif

void (*p_ap_channel_switch_callback)(unsigned char channel, s8 ret) = NULL;


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
int wifi_is_connected_to_ap(void)
{
	int ret = 0;

	inic_api_host_message_send(INIC_API_WIFI_IS_CONNECTED_TO_AP, NULL, 0, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
int wifi_set_channel(unsigned char wlan_idx, u8 channel)
{
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)channel;

	inic_api_host_message_send(INIC_API_WIFI_SET_CHANNEL, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_get_channel(unsigned char wlan_idx, u8 *channel)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)wlan_idx;
	inic_api_host_message_send(INIC_API_WIFI_GET_CHANNEL, (u8 *)param_buf, 4, channel, sizeof(u8));

	return ret;
}

//----------------------------------------------------------------------------//
int wifi_set_countrycode(char *cntcode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)cntcode;

	inic_api_host_message_send(INIC_API_WIFI_SET_COUNTRY_CODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_chplan(u8 chplan)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)chplan;

	inic_api_host_message_send(INIC_API_WIFI_SET_CHPLAN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//

_OPTIMIZE_NONE_
int wifi_get_scan_records(unsigned int *AP_num, char *scan_buf)
{
	int ret = 0;
	u32 param_buf[2];

	/* SPI bufsize can only 24 */
	if (*AP_num >= 20) {
		*AP_num = 20;
	}

	param_buf[0] = (u32)(*AP_num);
	param_buf[1] = (u32)scan_buf;

	inic_api_host_message_send(INIC_API_WIFI_GET_SCANNED_AP_INFO, (u8 *)param_buf, 8, (u8 *)scan_buf, (*AP_num)*sizeof(struct rtw_scan_result));

	return ret;
}

int wifi_scan_abort(u8 block)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)block;
	inic_api_host_message_send(INIC_API_WIFI_SCAN_ABORT, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));

	return ret;
}

//----------------------------------------------------------------------------//

int wifi_get_mac_address(int idx, struct _rtw_mac_t *mac, u8 efuse)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = idx;
	param_buf[2] = efuse;
	inic_api_host_message_send(INIC_API_WIFI_GET_MAC_ADDR, (u8 *)param_buf, 8, (u8 *)mac, sizeof(struct _rtw_mac_t));

	return ret;
}

int wifi_set_mac_address(int idx, unsigned char *mac, u8 efuse)
{
	int ret = 0;
	u32 param_buf[4];

	param_buf[0] = idx;
	param_buf[1] = (u32)efuse;
	memcpy((void *)&param_buf[2], (void *)mac, 6);
	inic_api_host_message_send(INIC_API_WIFI_SET_MAC_ADDR, (u8 *)param_buf, 16, (u8 *)&ret, sizeof(ret));

	return ret;
}
//----------------------------------------------------------------------------//
u8 wifi_driver_is_mp(void)
{
	int ret = 0;

	inic_api_host_message_send(INIC_API_WIFI_DRIVE_IS_MP, NULL, 0, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
int wifi_get_associated_client_list(struct _rtw_client_list_t *client_list_buffer)
{
	int ret = 0;

	inic_api_host_message_send(INIC_API_WIFI_GET_ASSOCIATED_CLIENT_LIST, NULL, 0, (u8 *)client_list_buffer, sizeof(struct _rtw_client_list_t));
	return ret;
}
//----------------------------------------------------------------------------//
int wifi_get_setting(unsigned char wlan_idx, struct _rtw_wifi_setting_t *psetting)
{
	int ret = 0;
	u32 param_buf[2];


	param_buf[0] = (u32)wlan_idx;

	inic_api_host_message_send(INIC_API_WIFI_GET_SETTING, (u8 *)param_buf, 4, (u8 *)psetting, sizeof(struct _rtw_wifi_setting_t));

	return ret;
}

int wifi_set_ips_internal(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	inic_api_host_message_send(INIC_API_WIFI_SET_IPS_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_lps_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	inic_api_host_message_send(INIC_API_WIFI_SET_LPS_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_lps_listen_interval(u8 interval)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)interval;
	inic_api_host_message_send(INIC_API_WIFI_SET_LPS_LISTEN_INTERVAL, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//

int wifi_set_mfp_support(unsigned char value)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)value;
	inic_api_host_message_send(INIC_API_WIFI_SET_MFP_SUPPORT, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_group_id(unsigned char value)
{
	rtw_sae_set_user_group_id(value);

	return RTW_SUCCESS;
}

int wifi_set_pmk_cache_enable(unsigned char value)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)value;
	inic_api_host_message_send(INIC_API_WIFI_SET_PMK_CACHE_EN, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//
int wifi_get_sw_statistic(unsigned char idx, struct _rtw_sw_statistics_t *statistic)
{
	u32 param_buf[2];
	int ret = 0;

	param_buf[0] = (u32)idx;

	inic_api_host_message_send(INIC_API_WIFI_GET_SW_STATISTIC, (u8 *)param_buf, 4, (u8 *)statistic, sizeof(struct _rtw_sw_statistics_t));

	return ret;
}

int wifi_fetch_phy_statistic(struct _rtw_phy_statistics_t *phy_statistic)
{
	int ret = 0;

	inic_api_host_message_send(INIC_API_WIFI_GET_PHY_STATISTIC, NULL, 0, (u8 *)phy_statistic, sizeof(struct _rtw_phy_statistics_t));
	return ret;
}

int wifi_get_network_mode(void)
{
	int ret = 0;
	inic_api_host_message_send(INIC_API_WIFI_GET_NETWORK_MODE, NULL, 0, (u8 *)ret, sizeof(ret));
	return ret;
}

int wifi_set_network_mode(u32 mode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)mode;
	inic_api_host_message_send(INIC_API_WIFI_SET_NETWORK_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_wps_phase(unsigned char is_trigger_wps)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = is_trigger_wps;

	inic_api_host_message_send(INIC_API_WIFI_SET_WPS_PHASE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
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
	inic_api_host_message_send(INIC_API_WIFI_SET_GEN_IE, (u8 *)param_buf, 12 + buf_len, (u8 *)&ret, sizeof(ret));

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

	inic_api_host_message_send(INIC_API_WIFI_SET_EAP_PHASE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
#else
	return -1;
#endif
}

unsigned char wifi_get_eap_phase(void)
{
#ifdef CONFIG_EAP
	unsigned char eap_phase = 0;

	inic_api_host_message_send(INIC_API_WIFI_GET_EAP_PHASE, NULL, 0, &eap_phase, sizeof(eap_phase));
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

	inic_api_host_message_send(INIC_API_WIFI_SET_EAP_METHOD, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
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

	inic_api_host_message_send(INIC_API_WIFI_SEND_EAPOL, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));
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
		inic_api_host_message_send(INIC_API_WPA_4WAY_REPORT, (u8 *)param_buf, size, NULL, 0);
		rtos_mem_free(param_buf);
	}
}

//----------------------------------------------------------------------------//
/*
 * Example for custom ie
 *
 * u8 test_1[] = {221, 2, 2, 2};
 * u8 test_2[] = {221, 2, 1, 1};
 * struct custom_ie buf[2] = {{test_1, BEACON},
 *		 {test_2, PROBE_RSP}};
 * u8 buf_test2[] = {221, 2, 1, 3} ;
 * struct custom_ie buf_update = {buf_test2, PROBE_RSP};
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

int wifi_add_custom_ie(struct custom_ie *cus_ie, int ie_num)
{
	int ret = 0;
	u8 *param_buf, *ptr;
	u32 size = 0;
	u8 i = 0;
	struct custom_ie *pcus_ie = cus_ie;

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

	inic_api_host_message_send(INIC_API_WIFI_CUS_IE, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

int wifi_update_custom_ie(struct custom_ie *cus_ie, int ie_index)
{
	int ret = 0;
	u8 *ptr, *param_buf;
	u32 size = 0;
	struct custom_ie *pcus_ie = cus_ie;

	size = 3 + 2 + pcus_ie->ie[1];
	ptr = param_buf = (u8 *)rtos_mem_zmalloc(size);

	ptr[0] = 1;
	ptr[1] = (u8)ie_index;
	ptr[2] = pcus_ie->type;
	ptr += 3;

	memcpy((void *)ptr, pcus_ie->ie, 2 + pcus_ie->ie[1]);

	inic_api_host_message_send(INIC_API_WIFI_CUS_IE, (u8 *)param_buf, size, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

int wifi_del_custom_ie(unsigned char wlan_idx)
{
	int ret;
	u8 param_buf[2];

	param_buf[0] = 2;//type 2 means delete
	param_buf[1] = (u8)wlan_idx;
	inic_api_host_message_send(INIC_API_WIFI_CUS_IE, (u8 *)param_buf, 2, (u8 *)&ret, sizeof(ret));
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

	inic_api_host_message_send(INIC_API_WIFI_SEND_MGNT, (u8 *)param_buf, len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

int wifi_set_tx_rate_by_ToS(unsigned char enable, unsigned char ToS_precedence, unsigned char tx_rate)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)enable;
	param_buf[1] = (u32)ToS_precedence;
	param_buf[2] = (u32)tx_rate;
	inic_api_host_message_send(INIC_API_WIFI_SET_TXRATE_BY_TOS, (u8 *)param_buf, 12, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_EDCA_param(unsigned int AC_param)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = AC_param;
	inic_api_host_message_send(INIC_API_WIFI_SET_EDCA_PARAM, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_TX_CCA(unsigned char enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = enable;
	inic_api_host_message_send(INIC_API_WIFI_SET_TX_CCA, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_cts2self_duration_and_send(unsigned char wlan_idx, unsigned short duration)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)duration;
	inic_api_host_message_send(INIC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
	return ret;

}

int wifi_get_antdiv_info(unsigned char *antdiv_mode, unsigned char *curr_ant)
{
#ifdef CONFIG_WIFI_ANTDIV
	int ret = 0;
	u32 param_buf[2];
	param_buf[0] = (u32)antdiv_mode;
	param_buf[1] = (u32)curr_ant;
	inic_api_host_message_send(INIC_API_WIFI_GET_ANTENNA_INFO, (u8 *)param_buf, 8, (u8 *)&ret, sizeof(ret));
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
 * 	WL_BAND_2_4G: only support 2.4G
 *	WL_BAND_5G: only support 5G
 *      WL_BAND_2_4G_5G_BOTH: support both 2.4G and 5G
 */
u8 wifi_get_band_type(void)
{
	u8 ret;

	inic_api_host_message_send(INIC_API_WIFI_GET_BAND_TYPE, NULL, 0, (u8 *)&ret, sizeof(ret));

	if (ret == 0) {
		return WL_BAND_2_4G;
	} else if (ret == 1) {
		return WL_BAND_5G;
	} else {
		return WL_BAND_2_4G_5G_BOTH;
	}
}

int wifi_del_station(unsigned char *hwaddr)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = (u32)IFACE_PORT1;
	memcpy((void *)(&param_buf[1]), (void *)hwaddr, 6);
	inic_api_host_message_send(INIC_API_WIFI_DEL_STA, (u8 *)param_buf, 12, (u8 *)&ret, sizeof(ret));
	return ret;

}

int wifi_ap_switch_chl_and_inform(struct _rtw_csa_parm_t *csa_param)
{
	int ret = 0;
	char *param_buf = rtos_mem_zmalloc(sizeof(struct _rtw_csa_parm_t));
	if (!param_buf) {
		return -1;
	}

	p_ap_channel_switch_callback = csa_param->callback;
	memcpy(param_buf, (void *)csa_param, sizeof(struct _rtw_csa_parm_t));

	inic_api_host_message_send(INIC_API_WIFI_AP_CH_SWITCH, (u8 *)param_buf, sizeof(struct _rtw_csa_parm_t), (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);
	return ret;
}

//----------------------------------------------------------------------------//
int wifi_csi_config(struct _rtw_csi_action_parm_t *act_param)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)act_param;
	inic_api_host_message_send(INIC_API_WIFI_CONFIG_CSI, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

//----------------------------------------------------------------------------//

int wifi_set_wpa_mode(u8 wpa_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)wpa_mode;
	inic_api_host_message_send(INIC_API_WIFI_SET_WPA_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

int wifi_set_pmf_mode(u8 pmf_mode)
{
	u32 param_buf[1];
	int ret = 0;

	param_buf[0] = (u32)pmf_mode;
	inic_api_host_message_send(INIC_API_WIFI_SET_PMF_MODE, (u8 *)param_buf, 4, (u8 *)&ret, sizeof(ret));
	return ret;
}

void wifi_wpa_add_key(struct rtw_crypt_info *crypt)
{
	char *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_crypt_info));
	memcpy(param_buf, (void *)crypt, sizeof(struct rtw_crypt_info));

	inic_api_host_message_send(INIC_API_WIFI_ADD_KEY, (u8 *)param_buf, sizeof(struct rtw_crypt_info), NULL, 0);
	rtos_mem_free(param_buf);
}

void wifi_wpa_pmksa_ops(struct rtw_pmksa_ops_t *pmksa_ops)
{
	char *param_buf = rtos_mem_zmalloc(sizeof(struct rtw_pmksa_ops_t));
	memcpy(param_buf, (void *)pmksa_ops, sizeof(struct rtw_pmksa_ops_t));
	inic_api_host_message_send(INIC_API_WPA_PMKSA_OPS, (u8 *)param_buf, sizeof(struct rtw_pmksa_ops_t), NULL, 0);

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

	inic_api_host_message_send(INIC_API_WIFI_SAE_STATUS, (u8 *)param_buf, len, NULL, 0);
	return 0;
}

int wifi_send_raw_frame(struct raw_frame_desc_t *raw_frame_desc)
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

	idx = raw_frame_desc->wlan_idx;

	sg_list[sg_len].buf = (unsigned int)raw_frame_desc->buf;
	sg_list[sg_len++].len = raw_frame_desc->buf_len;
	ret = inic_host_send(idx, sg_list, sg_len, raw_frame_desc->buf_len, &raw_para, 0);

	return ret;
}


void wifi_speaker_setting(u8 set_type, union speaker_set *settings)
{
#ifdef CONFIG_WIFI_SPEAKER_ENABLE
	u32 param_buf[2] = {0};

	param_buf[0] = (u32)set_type;

	DCache_Clean((u32)settings, sizeof(union speaker_set));
	param_buf[1] = (u32)settings;
	inic_api_host_message_send(INIC_API_WIFI_SPEAKER, (u8 *)param_buf, 8, NULL, 0);
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
	inic_api_host_message_send(INIC_API_WIFI_SET_OWE_PARAM, (u8 *)param_buf, 4, NULL, 0);
#else
	UNUSED(owe_param);
#endif
}

#endif	//#if CONFIG_WLAN
