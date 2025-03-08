/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "whc_dev.h"
#ifdef CONFIG_WHC_BRIDGEB
#include "lwip_netconf.h"
#endif

extern int wifi_set_chplan(u8 chplan);
extern int wifi_get_chplan(u8 *chplan);

struct event_priv_t event_priv;

const struct event_func_t whc_dev_api_handlers[] = {
	{WHC_API_WIFI_CONNECT,	whc_event_wifi_connect},
	{WHC_API_WIFI_ADD_KEY,	whc_event_wifi_add_key},
	{WHC_API_WPA_PMKSA_OPS,	whc_event_wpa_pmksa_ops},
	{WHC_API_WIFI_GET_CHPLAN,	whc_event_wifi_get_chplan},
	{WHC_API_WIFI_GET_COUNTRY_CODE,	whc_event_wifi_get_countrycode},
	{WHC_API_WIFI_DISCONNECT,	whc_event_wifi_disconnect},
	{WHC_API_WIFI_IS_RUNNING,	whc_event_wifi_is_running},
	{WHC_API_WIFI_SET_CHANNEL,	whc_event_wifi_set_channel},
	{WHC_API_WIFI_ON,	whc_event_wifi_on},
	{WHC_API_WIFI_INIT_AP,	whc_event_wifi_init_ap},
	{WHC_API_WIFI_DEINIT_AP,	whc_event_wifi_off_ap},
	{WHC_API_WIFI_START_AP,	whc_event_wifi_start_ap},
	{WHC_API_WIFI_STOP_AP,	whc_event_wifi_stop_ap},
	{WHC_API_WIFI_SCAN_NETWROKS,	whc_event_wifi_scan_networks},
	{WHC_API_WIFI_SET_MAC_ADDR,	whc_event_wifi_set_mac_address},
	{WHC_API_WIFI_SET_PMF_MODE,	whc_event_wifi_set_pmf_mode},
	{WHC_API_WIFI_SET_LPS_EN,	whc_event_wifi_set_lps_enable},
	{WHC_API_WIFI_SAE_STATUS,	whc_event_wifi_set_sae_status},
	{WHC_API_WIFI_GET_PHY_STATISTIC,	whc_event_wifi_fetch_phy_statistic},
	{WHC_API_WIFI_SEND_MGNT,	whc_event_wifi_send_mgnt},
	{WHC_API_WIFI_SET_EDCA_PARAM,	whc_event_wifi_set_EDCA_param},
	{WHC_API_WIFI_DEL_STA,	whc_event_wifi_del_station},
	{WHC_API_WIFI_IWPRIV_INFO,	whc_event_wifi_iwpriv_info},
	{WHC_API_WIFI_IP_UPDATE,		whc_event_wifi_ip_update},
	{WHC_API_WIFI_AP_CH_SWITCH,		whc_event_wifi_ap_switch_ch},
	{WHC_API_WIFI_SET_CHPLAN,	whc_event_wifi_set_chplan},
	{WHC_API_WIFI_SET_COUNTRY_CODE,	whc_event_wifi_set_countrycode},
	{WHC_API_WIFI_SET_WPA_MODE,	whc_event_wifi_set_wpa_mode},
	{WHC_API_WIFI_SET_WPS_PHASE,	whc_event_wifi_set_wps_phase},
	{WHC_API_WIFI_SET_GEN_IE,	whc_event_wifi_set_gen_ie},
	{WHC_API_WIFI_SCAN_ABORT,	whc_event_wifi_scan_abort},
	{WHC_API_WIFI_CUS_IE,	whc_event_wifi_custom_ie_ops},
	{WHC_API_WIFI_SET_USR_CFG, whc_event_wifi_set_usr_config},
	{WHC_API_WIFI_SET_HOST_RTOS, whc_event_wifi_set_host_rtos},
	{WHC_API_WIFI_SET_EDCCA_MODE, whc_event_wifi_set_edcca_mode},
	{WHC_API_WIFI_GET_EDCCA_MODE, whc_event_wifi_get_edcca_mode},
	{WHC_API_WIFI_GET_ANTENNA_INFO, whc_event_wifi_get_ant_info},
#ifdef CONFIG_NAN
	{WHC_API_NAN_INIT,	whc_event_nan_init},
	{WHC_API_NAN_DEINIT,		whc_event_nan_deinit},
	{WHC_API_NAN_START,		whc_event_nan_start},
	{WHC_API_NAN_STOP,	whc_event_nan_stop},
	{WHC_API_NAN_ADD_FUNC,	whc_event_add_nan_func},
	{WHC_API_NAN_DEL_FUNC,	whc_event_del_nan_func},
	{WHC_API_NAN_CFGVENFOR,	whc_event_nan_cfgvenfor},
#endif
#ifdef CONFIG_MP_INCLUDED
	{WHC_API_WIFI_MP_CMD,	whc_event_mp_cmd},
#endif
	{WHC_API_WIFI_DRIVE_IS_MP,	whc_event_wifi_driver_is_mp},
#ifdef CONFIG_P2P
	{WHC_API_P2P_ROLE,	whc_event_p2p_role},
	{WHC_API_P2P_REMAIN_ON_CH,		whc_event_p2p_remain_on_ch},
#endif
	{WHC_API_WAR_OFFLOAD_CTRL, whc_event_war_offload_ctrl},
	{WHC_API_WAR_SET_MDNS_PARA, whc_event_war_set_mdns_para},
#if defined(CONFIG_WHC_BRIDGEB)
	{WHC_API_BRIDGE_DHCP, whc_event_bridge_DHCP},
	{WHC_API_BRIDGE_GET_IP, whc_event_bridge_get_ip},
	{WHC_API_WIFI_GET_SCANNED_AP_INFO, whc_event_bridge_get_scan_res},
	{WHC_API_WIFI_GET_MAC_ADDR, whc_event_bridge_get_dev_mac},
#else
	{WHC_API_WIFI_GET_SCANNED_AP_INFO, whc_event_get_scan_res},
	{WHC_API_WIFI_GET_SETTING, whc_event_wifi_get_setting},
	{WHC_API_WIFI_SEND_EAPOL, whc_event_send_eapol},
	{WHC_API_WIFI_GET_ASSOCIATED_CLIENT_LIST, whc_event_get_assoc_client_list},
	{WHC_API_WPA_4WAY_REPORT, whc_event_wpa_4way_rpt},
	{WHC_API_WIFI_GET_SW_STATISTIC, whc_event_get_sw_statistic},
#endif
};

/**
 * @brief  handle the inic message.
 * @param  none.
 * @return none.
 */
void whc_dev_api_task(void)
{
	struct whc_api_info *p_recv_msg;
	u32 *param_buf;
	void (*api_hdl)(u32 api_id, u32 * param_buf);
	u32 i = 0;

	do {
		rtos_sema_take(event_priv.task_wake_sema, 0xFFFFFFFF);

		p_recv_msg = (struct whc_api_info *)event_priv.rx_api_msg;
		param_buf = (u32 *)(p_recv_msg + 1);

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "Host CALL API(%x)\n", p_recv_msg->api_id);

		api_hdl = NULL;
		for (i = 0; i < sizeof(whc_dev_api_handlers) / sizeof(struct event_func_t); i++) {
			if (whc_dev_api_handlers[i].api_id == p_recv_msg->api_id) {
				api_hdl = whc_dev_api_handlers[i].func;
				break;
			}
		}

		if (api_hdl != NULL) {
			api_hdl(p_recv_msg->api_id, param_buf);
		} else {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host Unknown API(%x)\n", p_recv_msg->api_id);
		}

		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "Host CALL API(%x) done\n", p_recv_msg->api_id);

		/* free rx_api_msg */
		rtos_mem_free((u8 *)p_recv_msg);
	} while (1);
}

void whc_send_api_ret_value(u32 api_id, u8 *pbuf, u32 len)
{
	u8 *buf = NULL;
	struct whc_api_info *ret_msg;
	struct whc_txbuf_info_t *inic_tx;

	buf = rtos_mem_zmalloc(sizeof(struct whc_api_info) + len + DEV_DMA_ALIGN);
	if (!buf) {
		goto exit;
	}
	ret_msg = (struct whc_api_info *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);

	/* notify NP that event is finished */
	ret_msg->event = WHC_WIFI_EVT_API_RETURN;
	ret_msg->api_id = api_id;

	memcpy((void *)(ret_msg + 1), pbuf, len);

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		goto exit;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)ret_msg;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct whc_api_info) + len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(pbuf, len) */
	whc_dev_send(&inic_tx->txbuf_info);

	RTK_LOGD(TAG_WLAN_INIC, "Host API %x return\n", api_id);

	return;

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (inic_tx) {
		rtos_mem_free((u8 *)inic_tx);
	}
	return;

}

#if !defined(CONFIG_WHC_BRIDGEB)
void whc_event_get_scan_res(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	char *scan_buf = NULL;
	unsigned int scanned_AP_num = 0;
	int ret = 0;

	scanned_AP_num = param_buf[0];
	if (scanned_AP_num == 0) {/* scanned no AP*/
		goto error_exit;
	}
	scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
	if (scan_buf == NULL) {
		goto error_exit;
	}
	if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
		ret = -1;
		rtos_mem_free((void *)scan_buf);
		goto error_exit;
	} else {
		whc_send_api_ret_value(api_id, (u8 *)scan_buf, scanned_AP_num * sizeof(struct rtw_scan_result));
		rtos_mem_free(scan_buf);
		return;
	}

error_exit:
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));

}

void whc_event_wifi_get_setting(u32 api_id, u32 *param_buf)
{
	int ret = -1;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	struct _rtw_wifi_setting_t *ap_info = (struct _rtw_wifi_setting_t *)rtos_mem_zmalloc(sizeof(struct _rtw_wifi_setting_t));
	if (!ap_info) {
		goto error_exit;
	}

	ret = wifi_get_setting(wlan_idx, ap_info);
	if (ret == 0) {
		whc_send_api_ret_value(api_id, (u8 *)ap_info, sizeof(struct _rtw_wifi_setting_t));
		rtos_mem_free(ap_info);
		return;
	} else {
		rtos_mem_free(ap_info);
		goto error_exit;
	}

error_exit:
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_send_eapol(u32 api_id, u32 *param_buf)
{
	int ret = -1;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	u16 flags = (u16)param_buf[1];
	u16 buf_len = (u16)param_buf[2];
	char *buf = (char *) & (param_buf[3]);

	ret = wifi_if_send_eapol(wlan_idx, buf, buf_len, flags);

	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_get_assoc_client_list(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret = -1;
	struct _rtw_client_list_t *client_list_buffer = (struct _rtw_client_list_t *)rtos_mem_zmalloc(sizeof(struct _rtw_client_list_t));

	if (!client_list_buffer) {
		goto error_exit;
	}
	ret = wifi_get_associated_client_list(client_list_buffer);

	if (ret != 0) {
		goto error_exit;
	}

	whc_send_api_ret_value(api_id, (u8 *)client_list_buffer, sizeof(struct _rtw_client_list_t));
	goto exit;

error_exit:
	RTK_LOGE(TAG_WLAN_INIC, "%s fail\n");
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));

exit:
	if (client_list_buffer)  {
		rtos_mem_free(client_list_buffer);
	}
}

void whc_event_wpa_4way_rpt(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	wifi_wpa_4way_status_indicate((struct rtw_wpa_4way_status *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_get_sw_statistic(u32 api_id, u32 *param_buf)
{
	unsigned char wlan_idx = (unsigned char)param_buf[0];

	struct _rtw_sw_statistics_t *statistic = (struct _rtw_sw_statistics_t *)rtos_mem_zmalloc(sizeof(struct _rtw_sw_statistics_t));
	wifi_get_sw_statistic(wlan_idx, statistic);
	whc_send_api_ret_value(api_id, (u8 *)statistic, sizeof(struct _rtw_sw_statistics_t));
	rtos_mem_free(statistic);
}

#endif

void whc_event_wifi_connect(u32 api_id, u32 *param_buf)
{
	int ret = -1;
	u8 *ptr = (u8 *)param_buf;
	struct _rtw_network_info_t connect_param;

	memcpy(&connect_param.ssid, ptr, sizeof(connect_param.ssid));
	ptr += sizeof(connect_param.ssid);

	memcpy(&connect_param.bssid, ptr, sizeof(connect_param.bssid));
	ptr += sizeof(connect_param.bssid);

	memcpy(&connect_param.security_type, ptr, sizeof(connect_param.security_type));
	ptr += sizeof(connect_param.security_type);

	memcpy(&connect_param.password_len, ptr, sizeof(connect_param.password_len));
	ptr += sizeof(connect_param.password_len);

	memcpy(&connect_param.key_id, ptr, sizeof(connect_param.key_id));
	ptr += sizeof(connect_param.key_id);

	memcpy(&connect_param.channel, ptr, sizeof(connect_param.channel));
	ptr += sizeof(connect_param.channel);

	memcpy(&connect_param.pscan_option, ptr, sizeof(connect_param.pscan_option));
	ptr += sizeof(connect_param.pscan_option);

	memcpy(&connect_param.is_wps_trigger, ptr, sizeof(connect_param.is_wps_trigger));
	ptr += sizeof(connect_param.is_wps_trigger);

	memcpy(&connect_param.wpa_supp, ptr, sizeof(connect_param.wpa_supp));
	ptr += sizeof(connect_param.wpa_supp);

	memcpy(&connect_param.prev_bssid, ptr, sizeof(connect_param.prev_bssid));
	ptr += sizeof(connect_param.prev_bssid);

	if (connect_param.password_len) {
		connect_param.password = ptr;
	}

	if (wifi_user_config.cfg80211) {
		/* KVR Roaming from cfg80211. */
		rtw_joinbss_by_roaming(&connect_param);
	}
	ret = rtw_joinbss_start_api(&connect_param);

	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));

}

void whc_event_wifi_add_key(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	wifi_wpa_add_key((struct rtw_crypt_info *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wpa_pmksa_ops(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	wifi_wpa_pmksa_ops((struct rtw_pmksa_ops_t *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_get_chplan(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	u8 chplan;

	wifi_get_chplan(&chplan);
	whc_send_api_ret_value(api_id, &chplan, sizeof(chplan));
}

void whc_event_wifi_get_countrycode(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	struct country_code_table_t countryCode_t;

	wifi_get_countrycode(&countryCode_t);
	whc_send_api_ret_value(api_id, (u8 *)&countryCode_t, sizeof(countryCode_t));
}

void whc_event_wifi_disconnect(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret;
	ret = wifi_disconnect();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_is_running(u32 api_id, u32 *param_buf)
{
	int ret;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	ret = wifi_is_running(wlan_idx);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_channel(u32 api_id, u32 *param_buf)
{
	int ret;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	u8 channel = (u8)param_buf[1];
	ret = wifi_set_channel(wlan_idx, channel);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_on(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 mode = (u8)param_buf[0];
	ret = wifi_on(mode);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_init_ap(u32 api_id, u32 *param_buf)
{
	(void)param_buf;

	int ret;
	ret = _wifi_on_ap();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_off_ap(u32 api_id, u32 *param_buf)
{
	(void)param_buf;

	int ret;
	ret = _wifi_off_ap();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_start_ap(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 *ptr = (u8 *)param_buf;
	struct _rtw_softap_info_t softAP_config;

	memcpy(&softAP_config.ssid, ptr, sizeof(softAP_config.ssid));
	ptr += sizeof(softAP_config.ssid);

	memcpy(&softAP_config.hidden_ssid, ptr, sizeof(softAP_config.hidden_ssid));
	ptr += sizeof(softAP_config.hidden_ssid);

	memcpy(&softAP_config.security_type, ptr, sizeof(softAP_config.security_type));
	ptr += sizeof(softAP_config.security_type);

	memcpy(&softAP_config.password_len, ptr, sizeof(softAP_config.password_len));
	ptr += sizeof(softAP_config.password_len);

	memcpy(&softAP_config.channel, ptr, sizeof(softAP_config.channel));
	ptr += sizeof(softAP_config.channel);

	softAP_config.password = ptr;

	ret = wifi_start_ap(&softAP_config);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_stop_ap(u32 api_id, u32 *param_buf)
{
	(void)param_buf;

	int ret;
	ret = wifi_stop_ap();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_scan_networks(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 *ptr = (u8 *)param_buf;
	u8 block;
	u32 ssid_length;
	struct _rtw_scan_param_t scan_param = {0};
	char *ssid = NULL;

	memcpy(&block, ptr, sizeof(block));
	ptr += sizeof(block);

	memcpy(&ssid_length, ptr, sizeof(ssid_length));
	ptr += sizeof(ssid_length);

	memcpy(&scan_param.options, ptr, sizeof(scan_param.options));
	ptr += sizeof(scan_param.options);

	memcpy(&scan_param.channel_list_num, ptr, sizeof(scan_param.channel_list_num));
	ptr += sizeof(scan_param.channel_list_num);

	memcpy(&scan_param.chan_scan_time, ptr, sizeof(scan_param.chan_scan_time));
	ptr += sizeof(scan_param.chan_scan_time);

	memcpy(&scan_param.max_ap_record_num, ptr, sizeof(scan_param.max_ap_record_num));
	ptr += sizeof(scan_param.max_ap_record_num);

	memcpy(&scan_param.scan_user_callback, ptr, sizeof(u32));
	ptr += sizeof(u32);

	if (ssid_length) {
		ssid = rtos_mem_zmalloc(ssid_length + 1);	/* need '\0' at the end */
		memcpy(ssid, (char *)ptr, ssid_length);
		scan_param.ssid = ssid;

		ptr += ssid_length;
	}

	scan_param.channel_list = ptr;
	ptr += scan_param.channel_list_num;

	memcpy(&scan_param.scan_user_data, ptr, sizeof(u32));
	ptr += sizeof(u32);

	/* wait for xmit done to release skb, otherwise API/RET msg can't be received.
		https://jira.realtek.com/browse/WQCCE-2914 */
	if (!whc_dev_tx_path_avail()) {
		RTK_LOGD(TAG_WLAN_INIC, "sdio tx path busy, scan fail\n");
		ret = -1;
	} else {
		ret = wifi_scan_networks(&scan_param, block);
	}

	if (ssid) {
		rtos_mem_free(ssid);
	}
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_mac_address(u32 api_id, u32 *param_buf)
{
	int ret;
	int idx = (int)param_buf[0];
	u8 efuse = (u8)param_buf[1];
	unsigned char *mac = (unsigned char *)(param_buf + 2);

	ret = wifi_set_mac_address(idx, mac, efuse);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_pmf_mode(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 pmf_mode = (u8)param_buf[0];
	ret = wifi_set_pmf_mode(pmf_mode);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_lps_enable(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 lps_en = (u8)param_buf[0];
	ret = wifi_set_lps_enable(lps_en);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_sae_status(u32 api_id, u32 *param_buf)
{
	int ret;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	u16 value = (u16)param_buf[1];
	u32 mac_len = param_buf[2];
	u8 *mac_addr = NULL;

	if (mac_len) {
		mac_addr = (u8 *)(param_buf + 3);
	}

	ret = wifi_sae_status_indicate(wlan_idx, value, mac_addr);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_fetch_phy_statistic(u32 api_id, u32 *param_buf)
{
	(void)param_buf;

	struct _rtw_phy_statistics_t statistic;
	wifi_fetch_phy_statistic(&statistic);
	whc_send_api_ret_value(api_id, (u8 *)&statistic, sizeof(struct _rtw_phy_statistics_t));
}

void whc_event_wifi_send_mgnt(u32 api_id, u32 *param_buf)
{
	int ret;
	struct _raw_data_desc_t raw_data_desc;

	raw_data_desc.wlan_idx = (u8)param_buf[0];
	raw_data_desc.buf_len = (u16)param_buf[1];
	raw_data_desc.flags = (u16)param_buf[2];
	raw_data_desc.buf = (u8 *)(param_buf + 3);
	ret = wifi_send_mgnt(&raw_data_desc);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_EDCA_param(u32 api_id, u32 *param_buf)
{
	int ret;
	unsigned int ac_param = param_buf[0];

	ret = wifi_set_EDCA_param(ac_param);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_del_station(u32 api_id, u32 *param_buf)
{
	int ret;
	unsigned char *hwaddr = (unsigned char *)(param_buf + 1);

	ret = wifi_del_station(hwaddr);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_iwpriv_info(u32 api_id, u32 *param_buf)
{
	int ret;
	int show_msg = (int)param_buf[0];
	//int cmd_len = (int)param_buf[1];
	char *cmd = (char *)(param_buf + 2);

	ret = rtw_iwpriv_command(STA_WLAN_INDEX, cmd, show_msg);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_ip_update(u32 api_id, u32 *param_buf)
{
#if defined(CONFIG_WHC_BRIDGEB)
	(void) api_id;
	(void) param_buf;
#else
	int ret = 0;
	u8 *p_ip_addr = (u8 *)param_buf;

	memcpy(whc_ipc_ip_addr, p_ip_addr, IPv4_ALEN);

	memcpy(IPv4Parm.IP, p_ip_addr, IPv4_ALEN);
	memcpy(IPv6Parm.IP, p_ip_addr + IPv4_ALEN, IPv6_ALEN);

	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
#endif
}

#ifdef CONFIG_NAN
void whc_event_nan_init(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret = 0;

	rtw_nan_intfs_init();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_nan_deinit(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret = 0;

	rtw_nan_intfs_deinit();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_nan_start(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u8 master_pref = (u8)param_buf[0];
	u8 band_support = (u8)param_buf[1];

	rtw_nan_start_api(master_pref, band_support);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_nan_stop(u32 api_id, u32 *param_buf)
{
	(void)param_buf;

	int ret = 0;
	rtw_nan_stop_api();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_add_nan_func(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	void *nan_func_pointer;

	rtw_nan_func_set_parameter((void *)param_buf, &nan_func_pointer);
	ret = rtw_nan_func_add((void *)param_buf, nan_func_pointer);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_del_nan_func(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u64 cookie = 0;

	cookie = (u64)(param_buf[1]);
	cookie = cookie << 32;
	cookie |= param_buf[0];
	ret = rtw_nan_func_del(cookie);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_nan_cfgvenfor(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u16 vendor_cmd = param_buf[0];
	u32 len = param_buf[1];
	void *data = (void *)(param_buf + 2);

	rtw_cfgvendor_cmd_process(vendor_cmd, data, len);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}
#endif

#ifdef CONFIG_WIFI_TUNNEL
void whc_event_wtn_cmd(u32 api_id, u32 *param_buf)
{
	int ret;
	char *cmd = (char *)(param_buf[0]);

	ret = rtw_wltunnel_command(cmd);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}
#endif

#ifdef CONFIG_MP_INCLUDED
void whc_event_mp_cmd(u32 api_id, u32 *param_buf)
{
	rtw_sdio_mp_cmd_dispatch(api_id, param_buf);
}
#endif

#ifdef CONFIG_P2P
void whc_event_p2p_role(u32 api_id, u32 *param_buf)
{
	int ret;
	enum rtw_p2p_role p2p_role = (enum rtw_p2p_role)param_buf[0];
	rtw_p2p_set_role(p2p_role);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_p2p_remain_on_ch(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 wlan_idx = (u8)param_buf[0];
	u8 enable = (u8)param_buf[1];
	ret = rtw_p2p_remain_on_ch(wlan_idx, enable);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}
#endif

void whc_event_wifi_ap_switch_ch(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	ret = wifi_ap_switch_chl_and_inform((struct _rtw_csa_parm_t *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_chplan(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u8 chplan = (u8)param_buf[0];
	ret = wifi_set_chplan(chplan);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_countrycode(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	ret = wifi_set_countrycode((char *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}


void whc_event_wifi_set_wpa_mode(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	u8 wpa_mode = (u8)param_buf[0];
	ret = wifi_set_wpa_mode(wpa_mode);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_wps_phase(u32 api_id, u32 *param_buf)
{
	int ret = 0;

	unsigned char is_trigger_wps = (unsigned char)param_buf[0];
	ret = wifi_set_wps_phase(is_trigger_wps);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_gen_ie(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	unsigned char wlan_idx = (unsigned char)param_buf[0];
	u16 buf_len = (u16)param_buf[1];
	u16 flags = (u16)param_buf[2];

	ret = wifi_set_gen_ie(wlan_idx, (char *)&param_buf[3], buf_len, flags);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_scan_abort(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u8 block = (u8)param_buf[0];
	ret = wifi_scan_abort(block);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_custom_ie_ops(u32 api_id, u32 *param_buf)
{
	int ret = 0;
	u8 *ptr = (u8 *)param_buf;
	u8 type = ptr[0];

	if (type == 2) {
		unsigned char wlan_idx = ptr[1];
		ret = wifi_del_custom_ie(wlan_idx);
	} else if (type == 1) {
		int ie_index = (int)ptr[1];
		struct custom_ie *cus_ie = rtos_mem_zmalloc(sizeof(struct custom_ie));

		cus_ie->type = ptr[2];
		ptr += 3;
		cus_ie->ie = ptr;

		ret = wifi_update_custom_ie(cus_ie, ie_index);
		rtos_mem_free((u8 *)cus_ie);

	} else if (type == 0) {
		int ie_num = (int)ptr[1];
		u8 i = 0;

		ptr += 2;
		if (ie_num) {
			struct custom_ie *cus_ie_array = rtos_mem_zmalloc(sizeof(struct custom_ie) * ie_num);
			for (i = 0; i < ie_num; i++) {
				cus_ie_array[i].type = ptr[0];/*ptr[0]: type*/
				cus_ie_array[i].ie = &ptr[1];/*ptr[1]: id*/
				ptr += 3 + ptr[2];			/*ptr[2]: datalen*/
			}
			ret = wifi_add_custom_ie(cus_ie_array, ie_num);
			rtos_mem_free((u8 *)cus_ie_array);
		} else {
			ret = 0;
		}
	}
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_set_host_rtos(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret = 0;
	wifi_user_config.cfg80211 = 0;
	whc_send_api_ret_value(api_id, (u8 *)(&ret), sizeof(ret));
}

void whc_event_wifi_set_usr_config(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	u8 *pwifi_usrcfg = rtos_mem_zmalloc(sizeof(struct wifi_user_conf));

	memcpy(pwifi_usrcfg, &wifi_user_config, sizeof(struct wifi_user_conf));
	wifi_set_rom2flash_user_config();
	whc_send_api_ret_value(api_id, pwifi_usrcfg, sizeof(struct wifi_user_conf));
	rtos_mem_free(pwifi_usrcfg);
}

void whc_event_wifi_set_edcca_mode(u32 api_id, u32 *param_buf)
{
	int ret;
	u8 edcca_mode = (u8)param_buf[0];

	switch (edcca_mode) {
	case RTW_EDCCA_NORM:
	case RTW_EDCCA_ADAPT:
	case RTW_EDCCA_CS:
	case RTW_EDCCA_DISABLE:
		wifi_set_edcca_mode(edcca_mode);
		ret = 0;
		break;
	default:
		ret = -1;
	}
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_wifi_get_edcca_mode(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	u8 edcca_mode = wifi_user_config.rtw_edcca_mode;

	whc_send_api_ret_value(api_id, &edcca_mode, sizeof(edcca_mode));
}

void whc_event_wifi_get_ant_info(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	u8 value[2];

	wifi_get_antdiv_info(&value[0], &value[1]);

	whc_send_api_ret_value(api_id, (u8 *)value, sizeof(value));
}

void whc_event_war_offload_ctrl(u32 api_id, u32 *param_buf)
{
#if defined(CONFIG_WHC_BRIDGEB)
	(void) api_id;
	(void) param_buf;
#else
	int ret = 0;

	ret = rtw_war_offload_ctrl((u8 *)param_buf);
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
#endif
}

void whc_event_war_set_mdns_para(u32 api_id, u32 *param_buf)
{
#if defined(CONFIG_WHC_BRIDGEB)
	(void) api_id;
	(void) param_buf;
#else
	int ret = 0;
	u32 mdns_param_sz;
	u8 *ptr = (u8 *)param_buf;

	memcpy(&mdns_param_sz, ptr, 4);
	ptr += 4;

	if (WAROffloadParm.offload_en) {
		if (WAROffloadParm.offload_ctrl & WAR_MDNS_EN) {
			GetMDNSInfo(ptr, mdns_param_sz);
		}
	}

	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
#endif
}

void whc_event_wifi_driver_is_mp(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	int ret = 0;

	ret = wifi_driver_is_mp();
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

/**
 * @brief  send a inic message and wait resut.
 * @param  ID[in]: api_id.
 * @param  param_buf[inout]: pointer to API parameter.
 * @return result of API.
 */
void whc_dev_api_message_send(u32 id, u8 *param, u32 param_len, u8 *ret, u32 ret_len)
{
	u8 *buf = NULL;
	struct whc_api_info *info;
	struct whc_api_info *ret_msg;
	struct whc_txbuf_info_t *inic_tx;

	RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "Device Call API %ld\n", id);

	rtos_mutex_take(event_priv.send_mutex, MUTEX_WAIT_TIMEOUT);

	buf = rtos_mem_zmalloc(sizeof(struct whc_api_info) + param_len + DEV_DMA_ALIGN);
	if (!buf) {
		goto exit;
	}
	info = (struct whc_api_info *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);
	info->event = WHC_WIFI_EVT_API_CALL;
	info->api_id = id;

	memcpy((void *)(info + 1), param, param_len);

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	if (!inic_tx) {
		goto exit;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)info;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = sizeof(struct whc_api_info) + param_len;

	inic_tx->ptr = buf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_dev_send(&inic_tx->txbuf_info);

	/* wait for API calling done */
	event_priv.b_waiting_for_ret = 1;
	rtos_sema_take(event_priv.api_ret_sema, 0xFFFFFFFF);
	event_priv.b_waiting_for_ret = 0;

	RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_DEBUG, "Device API %ld return\n", id);

	/* get return value */
	ret_msg = (struct whc_api_info *)event_priv.rx_ret_msg;
	if (ret_msg != NULL) {
		/* check api_id of return msg */
		if (ret_msg->api_id != id) {
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Linux API return value id not match!\n");
		}

		/* copy return value*/
		if (ret != NULL && ret_len != 0) {
			memcpy(ret, (u8 *)(ret_msg + 1), ret_len);
		}

		/* free rx buffer */
		rtos_mem_free((u8 *)ret_msg);

		event_priv.rx_ret_msg = NULL;
	} else {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Linux API return value is NULL!\n");
	}

	rtos_mutex_give(event_priv.send_mutex);
	return;

exit:
	if (buf) {
		rtos_mem_free(buf);
	}
	if (inic_tx) {
		rtos_mem_free((u8 *)inic_tx);
	}

	rtos_mutex_give(event_priv.send_mutex);
	return;

}

void whc_dev_wifi_event_indicate(int event_cmd, char *buf, int buf_len, int flags)
{
	/*not indicate Beacon event since it is too frequent*/
	/*not indicate scan event since it is handled in device*/
	u32 size;
	u32 *param;

	size = 3 * sizeof(u32) + buf_len;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = event_cmd;
	param[1] = flags;
	param[2] = buf_len;
	memcpy((void *)(param + 3), buf, buf_len);

	whc_dev_api_message_send(WHC_API_HDL, (u8 *)param, size, NULL, 0);

	/* free buffer */
	rtos_mem_free((u8 *)param);

}

void whc_dev_scan_user_callback_indicate(unsigned int ap_num, void *user_data)
{
	u32 param_buf[2];
	param_buf[0] = ap_num;
	param_buf[1] = (u32)user_data;

	whc_dev_api_message_send(WHC_API_SCAN_USER_CALLBACK, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}

void whc_dev_scan_each_report_user_callback_indicate(struct rtw_scan_result *scanned_ap_info, void *user_data)
{
	/* TODO for Linux */
	(void) scanned_ap_info;
	(void) user_data;
}

u8 whc_dev_promisc_callback_indicate(struct rx_pkt_info *pkt_info)
{
	/* TODO for Linux */
	(void) pkt_info;
	return NEED_DRIVER_HANDLE;
}

int whc_dev_ip_in_table_indicate(u8 gate, u8 ip)
{
	u32 param_buf[2];
	int ret;
	param_buf[0] = gate;
	param_buf[1] = ip;

	whc_dev_api_message_send(WHC_API_IP_TABLE_CHK, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(ret));

	return ret;
}

void whc_dev_ap_ch_switch_callback_indicate(unsigned char channel, s8 ret)
{
	u32 param_buf[2];

	param_buf[0] = (u32)channel;
	param_buf[1] = (u32)ret;
	whc_dev_api_message_send(WHC_API_AP_CH_SWITCH, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}

int whc_dev_get_lwip_info(u32 type, unsigned char *input, int index)
{
	u32 size;
	u32 *param;
	u8 res_size = 0;

	size = 2 * sizeof(u32) + 4;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = type;
	param[1] = index;

	if (input) {
		memcpy((void *)(param + 2), input, 4);
	}

	if (type == WHC_WLAN_GET_IP || type == WHC_WLAN_GET_GW || type == WHC_WLAN_GET_GWMSK) {
		res_size = 4;
	} else if (type == WHC_WLAN_GET_HW_ADDR) {
		res_size = 6;
	}

	whc_dev_api_message_send(WHC_API_GET_LWIP_INFO, (u8 *)param, size, event_priv.dev_req_network_info, res_size);

	/* free buffer */
	rtos_mem_free((u8 *)param);

	return (int)event_priv.dev_req_network_info;

}

void whc_dev_set_netif_info(int idx_wlan, unsigned char *dev_addr)
{
	u32 size;
	u32 *param;

	size = sizeof(u32) + ETH_ALEN;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = idx_wlan;
	if (dev_addr) {
		memcpy((void *)(param + 1), dev_addr, ETH_ALEN);
	}

	whc_dev_api_message_send(WHC_API_SET_NETIF_INFO, (u8 *)param, size, NULL, 0);

	/* free buffer */
	rtos_mem_free((u8 *)param);
}

void whc_dev_acs_info_indicate(struct acs_mntr_rpt *acs_mntr_rpt)
{
	u32 *param = NULL;
	u32 size = sizeof(struct acs_mntr_rpt);

	param = (u32 *)rtos_mem_zmalloc(size);
	memcpy((void *)(param), acs_mntr_rpt, size);

	whc_dev_api_message_send(WHC_API_IP_ACS, (u8 *)param, size, NULL, 0);

	rtos_mem_free((u8 *)param);
}

void whc_dev_cfg80211_indicate_scan_report(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len)
{
	u32 size;
	u32 *param = NULL;

	size = 4 * sizeof(u32) + ETH_ALEN + ie_len;

	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = channel;
	param[1] = frame_is_bcn;
	param[2] = (u32)rssi;
	param[3] = ie_len;
	memcpy((void *)(param + 4), mac_addr, ETH_ALEN);

	if (IEs && (ie_len > 0)) {
		memcpy((void *)((u8 *)(param + 4) + ETH_ALEN), IEs, ie_len);
	}

	RTK_LOGD(TAG_WLAN_INIC, "CH%ld, %02x:%02x:%02x:%02x:%02x:%02x\n", channel,
			 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

	whc_dev_api_message_send(WHC_API_CFG80211_SCAN_REPORT, (u8 *)param, size, NULL, 0);

	rtos_mem_free((u8 *)param);
}

/**
 * @brief  to initialize the host for WIFI api.
 * @param  none.
 * @return none.
 */
void whc_dev_api_init(void)
{
	/* initialize the semaphores */
	rtos_sema_create_static(&event_priv.api_ret_sema, 0, 0xFFFFFFFF);
	rtos_sema_create_static(&event_priv.task_wake_sema, 0, 0xFFFFFFFF);
	rtos_mutex_create_static(&event_priv.send_mutex);

	event_priv.b_waiting_for_ret = 0;

	/* Initialize the event task */
	if (SUCCESS != rtos_task_create(&event_priv.api_dev_task, (const char *const)"whc_dev_api_task", (rtos_task_function_t)whc_dev_api_task, NULL,
									WHC_API_STACK * 4, CONFIG_WHC_DEV_API_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_dev_api_task Err!!\n");
	}
}

#ifdef CONFIG_NAN
void whc_dev_cfg80211_indicate_nan_match(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u32 size;
	u32 *param;

	size = 6 * sizeof(u32) + ETH_ALEN + info_len;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = type;
	param[1] = inst_id;
	param[2] = peer_inst_id;
	param[3] = info_len;
	param[4] = cookie & 0xFFFFFFFF;
	param[5] = cookie >> 32 & 0xFFFFFFFF;
	memcpy((void *)(param + 6), addr, ETH_ALEN);
	memcpy((void *)((u8 *)(param + 6) + ETH_ALEN), info, info_len);

	whc_dev_api_message_send(WHC_API_CFG80211_NAN_REPORT_MATCH_EVENT, (u8 *)param, size, NULL, 0);

	rtos_mem_free((u8 *)param);
}

void whc_dev_cfg80211_nan_func_free(u64 data)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u64 param_buf[1];
	param_buf[0] = data;

	whc_dev_api_message_send(WHC_API_CFG80211_NAN_DEL_FUNC, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}

void whc_dev_cfg80211_nan_cfgvendor_event_report(u16 event_id, void *event, int event_len)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u32 size;
	u32 *param;

	size = 2 * sizeof(u32) + event_len;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = event_id;
	param[1] = event_len;
	if (event && (event_len > 0)) {
		memcpy((void *)(param + 2), event, event_len);
	}

	whc_dev_api_message_send(WHC_API_CFG80211_NAN_CFGVENDOR_EVENT, (u8 *)param, size, NULL, 0);

	/* free buffer */
	rtos_mem_free((u8 *)param);

}

void whc_dev_cfg80211_cfgvendor_send_cmd_reply(void *data, int len)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	u32 size;
	u32 *param;

	size = sizeof(u32) + len;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = len;
	memcpy((void *)(param + 1), data, len);

	whc_dev_api_message_send(WHC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY, (u8 *)param, size, NULL, 0);

	/* free buffer */
	rtos_mem_free((u8 *)param);

}
#endif

#ifdef CONFIG_P2P
void whc_dev_cfg80211_indicate_channel_ready(void *scan_userdata)
{
	if (!wifi_user_config.cfg80211) {
		return;
	}

	/* record wlan_idx directly in scan userdata, and use it for search wdev when indicate */
	u32 param_buf[1];
	param_buf[0] = (u32)scan_userdata;

	whc_dev_api_message_send(WHC_API_CFG80211_P2P_CH_RDY, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}
#endif

#if defined(CONFIG_WHC_BRIDGEB)
/* for bridgeb mode */
void whc_event_bridge_get_dev_mac(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	struct _rtw_mac_t dev_mac = {0};
	wifi_get_mac_address(STA_WLAN_INDEX, &dev_mac, 0);
	whc_send_api_ret_value(api_id, dev_mac.octet, 6);
}

void whc_event_bridge_DHCP(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	int ret = -1;
	LwIP_netif_set_link_up(0);
	if (LwIP_DHCP(0, DHCP_START) == DHCP_ADDRESS_ASSIGNED) {
		ret = 0;
	}
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));
}

void whc_event_bridge_get_ip(u32 api_id, u32 *param_buf)
{
	(void) param_buf;
	u32 ip_ret = 0;
	u8 *ip;
	ip = LwIP_GetIP(0);
	memcpy((u8 *)&ip_ret, ip, 4);
	whc_send_api_ret_value(api_id, (u8 *)&ip_ret, sizeof(ip_ret));
}

void whc_event_bridge_get_scan_res(u32 api_id, u32 *param_buf)
{
	(void)param_buf;
	char *scan_buf = NULL;
	unsigned int scanned_AP_num = 0;
	int ret = 0;

	scanned_AP_num = param_buf[0];
	if (scanned_AP_num == 0) {/* scanned no AP*/
		goto error_exit;
	}

	scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
	if (scan_buf == NULL) {
		goto error_exit;
	}
	if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
		rtos_mem_free((void *)scan_buf);
		goto error_exit;
	} else {
		whc_send_api_ret_value(api_id, (u8 *)scan_buf, scanned_AP_num * sizeof(struct rtw_scan_result));
		rtos_mem_free(scan_buf);
		return;
	}

error_exit:
	whc_send_api_ret_value(api_id, (u8 *)&ret, sizeof(ret));

}
#endif


