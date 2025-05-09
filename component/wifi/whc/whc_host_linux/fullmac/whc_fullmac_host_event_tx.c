
#include <whc_host_linux.h>

void whc_fullmac_host_send_event(u32 id, u8 *param, u32 param_len, u8 *ret, u32 ret_len)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	struct whc_api_info *ret_msg;
	struct whc_api_info *api_info;
	unsigned long wait_ret;
	u8 *buf;
	u32 buf_len;
	u32 timeout;

	dev_dbg(global_idev.fullmac_dev, "-----HOST CALLING API %x START\n", id);

#ifdef CONFIG_FULLMAC_HCI_USB
	if (whc_usb_host_send_event_check(id) < 0) {
		return;
	}
#endif
	mutex_lock(&(event_priv->send_mutex));

	/* send TX_DESC + info + data(param, param_len) */
	buf_len = SIZE_TX_DESC + sizeof(struct whc_api_info) + param_len;
	buf = kzalloc(buf_len, GFP_KERNEL);
	if (buf) {
		/* fill whc_api_info_t */
		api_info = (struct whc_api_info *)(buf + SIZE_TX_DESC);
		api_info->event = WHC_WIFI_EVT_API_CALL;
		api_info->api_id = id;

		/* copy data */
		if (param) {
			memcpy((u8 *)(api_info + 1), param, param_len);
		}

		/* send */
		whc_host_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
		kfree(buf);
#endif
	} else {
		dev_err(global_idev.fullmac_dev, "%s can't alloc buffer!\n", __func__);
		goto exit;
	}

	timeout = 2000;

	/* wait for API calling done */
	event_priv->b_waiting_for_ret = 1;
	wait_ret = wait_for_completion_timeout(&event_priv->api_ret_sema, msecs_to_jiffies(timeout));
	event_priv->b_waiting_for_ret = 0;

	if (wait_ret == 0) {
		dev_err(global_idev.fullmac_dev, "wait ret value timeout!!\n");
		goto exit;
	}

	ret_msg = (struct whc_api_info *)(event_priv->rx_api_ret_msg->data + SIZE_RX_DESC);
	if (ret_msg != NULL) {
		/* check api_id of return msg */
		if (ret_msg->api_id != id) {
			dev_err(global_idev.fullmac_dev, "Host API return value id not match!\n");
		}

		/* copy return value*/
		if (ret != NULL && ret_len != 0) {
			memcpy(ret, (u8 *)(ret_msg + 1), ret_len);
		}

		/* free rx buffer */
		kfree_skb(event_priv->rx_api_ret_msg);
		event_priv->rx_api_ret_msg = NULL;
	} else {
		dev_err(global_idev.fullmac_dev, "Host API return value is NULL!\n");
	}

exit:
	mutex_unlock(&(event_priv->send_mutex));

	dev_dbg(global_idev.fullmac_dev, "-----HOST API %x CALLING DONE\n", id);

}

int whc_fullmac_host_set_user_config(struct wifi_user_conf *pwifi_usrcfg)
{
	u32 size;
	u32 param_buf[1];

	size = sizeof(struct wifi_user_conf);
	param_buf[0] = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_USR_CFG, (u8 *)param_buf, sizeof(param_buf), (u8 *)pwifi_usrcfg, size);

	return 0;
}

void whc_fullmac_host_wifi_on(void)
{
	u32 param_buf[1];

	param_buf[0] = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_ON, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}

int whc_fullmac_host_set_mac_addr(u32 wlan_idx, u8 *addr)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 2 * sizeof(u32) + ETH_ALEN;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = wlan_idx;
	param[1] = 0;
	memcpy((void *)(param + 2), addr, ETH_ALEN);

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_MAC_ADDR, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_scan(struct rtw_scan_param *scan_param, u32 ssid_length, u8 block)
{
	int ret = 0;
	u32 size;
	u8 *ptr, *param;

	size = sizeof(block) + sizeof(ssid_length) +
		   sizeof(scan_param->options) +
		   sizeof(scan_param->channel_list_num) +
		   sizeof(scan_param->chan_scan_time) +
		   sizeof(scan_param->max_ap_record_num) +
		   sizeof(u32) +
		   ssid_length + scan_param->channel_list_num +
		   sizeof(u32);

	ptr = param = kzalloc(size, GFP_KERNEL);

	memcpy(ptr, &block, sizeof(block));
	ptr += sizeof(block);

	memcpy(ptr, &ssid_length, sizeof(ssid_length));
	ptr += sizeof(ssid_length);

	memcpy(ptr, &scan_param->options, sizeof(scan_param->options));
	ptr += sizeof(scan_param->options);

	memcpy(ptr, &scan_param->channel_list_num, sizeof(scan_param->channel_list_num));
	ptr += sizeof(scan_param->channel_list_num);

	memcpy(ptr, &scan_param->chan_scan_time, sizeof(scan_param->chan_scan_time));
	ptr += sizeof(scan_param->chan_scan_time);

	memcpy(ptr, &scan_param->max_ap_record_num, sizeof(scan_param->max_ap_record_num));
	ptr += sizeof(scan_param->max_ap_record_num);

	/* Note: for PC/raspi, function pointer would be 8-bytes. So force to 4 bytes to sync with NP. */
	memcpy(ptr, &scan_param->scan_user_callback, sizeof(u32));
	ptr += sizeof(u32);

	memcpy(ptr, scan_param->ssid, ssid_length);
	ptr += ssid_length;

	memcpy(ptr, scan_param->channel_list, scan_param->channel_list_num);
	ptr += scan_param->channel_list_num;

	memcpy(ptr, &scan_param->scan_user_data, sizeof(u32)); /* for P2P roch related indicate */
	ptr += sizeof(u32);

	whc_fullmac_host_send_event(WHC_API_WIFI_SCAN_NETWROKS, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_scan_abort(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_SCAN_ABORT, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_event_connect(struct rtw_network_info *connect_param, unsigned char block)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;
	u32 size;
	u8 *param, *ptr;

	/* step1: check if there's ongoing connect*/
	if ((global_idev.mlme_priv.rtw_join_status > RTW_JOINSTATUS_UNKNOWN)
		&& (global_idev.mlme_priv.rtw_join_status < RTW_JOINSTATUS_SUCCESS)) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: there is ongoing wifi connect!rtw_join_status=%d\n", global_idev.mlme_priv.rtw_join_status);
		return -EBUSY;
	}

	/*clear for last connect status */
	global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_STARTING;
	whc_fullmac_host_connect_indicate(RTW_JOINSTATUS_STARTING, NULL, 0);

	/* step2: malloc and set synchronous connection related variables*/
	if (block) {
		block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
		if (!block_param) {
			ret = -ENOMEM;
			global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
			goto error;
		}
		/* initialize join_sema. */
		init_completion(&block_param->sema);
	}

	/* step3: set connect cmd to driver*/
	size = sizeof(connect_param->ssid) +
		   sizeof(connect_param->bssid) +
		   sizeof(connect_param->security_type) +
		   sizeof(connect_param->password_len) +
		   sizeof(connect_param->key_id) +
		   sizeof(connect_param->channel) +
		   sizeof(connect_param->pscan_option) +
		   sizeof(connect_param->is_wps_trigger) +
		   sizeof(connect_param->wpa_supp) +
		   sizeof(connect_param->prev_bssid) +
		   connect_param->password_len;

	ptr = param = kzalloc(size, GFP_KERNEL);

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

	memcpy(ptr, connect_param->password, connect_param->password_len);
	ptr += connect_param->password_len;

	//print_hex_dump_bytes("connect_param: ", DUMP_PREFIX_NONE, param, size);

	whc_fullmac_host_send_event(WHC_API_WIFI_CONNECT, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	if (ret != 0) {
		ret = -EINVAL;
		global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	/* step4: wait connect finished for synchronous connection*/
	if (block) {
		global_idev.mlme_priv.join_block_param = block_param;

		if (wait_for_completion_timeout(&block_param->sema, RTW_JOIN_TIMEOUT) == 0) {
			dev_err(global_idev.fullmac_dev, "%s: Join bss timeout!\n", __func__);
			global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
			ret = -EINVAL;
			goto error;
		} else {
			if (whc_fullmac_host_wifi_get_join_status() != RTW_JOINSTATUS_SUCCESS) {
				ret = -EINVAL;
				global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
				goto error;
			}
		}
	}

error:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
		global_idev.mlme_priv.join_block_param = NULL;
	}

	if (global_idev.mlme_priv.rtw_join_status == RTW_JOINSTATUS_FAIL) {
		whc_fullmac_host_connect_indicate(RTW_JOINSTATUS_FAIL, NULL, 0);
	}

	return ret;
}

int whc_fullmac_host_event_disconnect(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_DISCONNECT, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_wifi_get_join_status(void)
{
	return global_idev.mlme_priv.rtw_join_status;
}

int whc_fullmac_host_set_channel(u32 wlan_idx, u8 ch)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = wlan_idx;
	param_buf[1] = (u32)ch;
	whc_fullmac_host_send_event(WHC_API_WIFI_SET_CHANNEL, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_init_ap(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_INIT_AP, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_deinit_ap(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_DEINIT_AP, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_ap_del_client(u8 wlan_idx, u8 *mac)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 1 * sizeof(u32) + ETH_ALEN;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = wlan_idx;
	memcpy((void *)(param + 1), mac, ETH_ALEN);

	whc_fullmac_host_send_event(WHC_API_WIFI_AP_DEL_CLIENT, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_start_ap(struct rtw_softap_info *softAP_config)
{
	int ret = 0;
	u32 size;
	u8 *param, *ptr;

	size = sizeof(softAP_config->ssid) +
		   sizeof(softAP_config->hidden_ssid) +
		   sizeof(softAP_config->security_type) +
		   sizeof(softAP_config->password_len) +
		   sizeof(softAP_config->channel) +
		   softAP_config->password_len;

	ptr = param = kzalloc(size, GFP_KERNEL);

	memcpy(ptr, &softAP_config->ssid, sizeof(softAP_config->ssid));
	ptr += sizeof(softAP_config->ssid);

	memcpy(ptr, &softAP_config->hidden_ssid, sizeof(softAP_config->hidden_ssid));
	ptr += sizeof(softAP_config->hidden_ssid);

	memcpy(ptr, &softAP_config->security_type, sizeof(softAP_config->security_type));
	ptr += sizeof(softAP_config->security_type);

	memcpy(ptr, &softAP_config->password_len, sizeof(softAP_config->password_len));
	ptr += sizeof(softAP_config->password_len);

	memcpy(ptr, &softAP_config->channel, sizeof(softAP_config->channel));
	ptr += sizeof(softAP_config->channel);

	memcpy(ptr, softAP_config->password, softAP_config->password_len);
	ptr += softAP_config->password_len;

	whc_fullmac_host_send_event(WHC_API_WIFI_START_AP, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_stop_ap(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_STOP_AP, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_set_EDCA_params(unsigned int *AC_param)
{
	int ret = 0;
	u32 param_buf[1] = {0};

	param_buf[0] = *AC_param;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_EDCA_PARAM, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_add_key(struct rtw_crypt_info *crypt)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_ADD_KEY, (u8 *)crypt, sizeof(struct rtw_crypt_info), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_tx_mgnt(u8 wlan_idx, const u8 *buf, size_t buf_len, u8 need_wait_ack)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 3 * sizeof(u32) + buf_len;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = wlan_idx;
	param[1] = buf_len;
	if (need_wait_ack) {
		param[2] |= RTW_SEND_AND_WAIT_ACK;
	} else {
		param[2] = 0;	/* flags*/
	}
	memcpy((void *)(param + 3), buf, buf_len);

	whc_fullmac_host_send_event(WHC_API_WIFI_SEND_MGNT, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	int ret = 0;
	u32 size, mac_len = 0;
	u32 *param;

	size = 3 * sizeof(u32);
	if (mac_addr != NULL) {
		mac_len = ETH_ALEN;
		size += mac_len;
	}

	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = (u32)wlan_idx;
	param[1] = (u32)status;
	param[2] = (u32)mac_len;

	if (mac_addr != NULL) {
		memcpy((void *)(param + 3), mac_addr, ETH_ALEN);
	}

	whc_fullmac_host_send_event(WHC_API_WIFI_SAE_STATUS, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

u32 whc_fullmac_host_update_ip_addr(void)
{
	u32 ret = 0;
	u8 param[RTW_IP_ADDR_LEN + RTW_IPv6_ADDR_LEN];

	memcpy(param, global_idev.ip_addr, RTW_IP_ADDR_LEN);
	memcpy(param + RTW_IP_ADDR_LEN, global_idev.ipv6_addr, RTW_IPv6_ADDR_LEN);

	whc_fullmac_host_send_event(WHC_API_WIFI_IP_UPDATE, param, sizeof(param), (u8 *)&ret, sizeof(u32));

	return ret;
}

int whc_fullmac_host_get_traffic_stats(u8 wlan_idx, dma_addr_t traffic_stats_addr)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 1 * sizeof(u32);

	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = (u32)wlan_idx;

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_TRAFFIC_STATS, (u8 *)param, size, (u8 *)traffic_stats_addr, sizeof(union rtw_traffic_stats));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_get_phy_stats(u8 wlan_idx, u8 *mac_addr, dma_addr_t phy_stats_addr)
{
	int ret = 0;
	u32 size, mac_len = 0;
	u32 *param;

	size = 2 * sizeof(u32);
	if (mac_addr != NULL) {
		mac_len = ETH_ALEN;
		size += mac_len;
	}

	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = (u32)wlan_idx;
	param[1] = (u32)mac_len;

	if (mac_addr != NULL) {
		memcpy((void *)(param + 2), mac_addr, ETH_ALEN);
	}

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_PHY_STATS, (u8 *)param, size, (u8 *)phy_stats_addr, sizeof(union rtw_phy_stats));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_get_setting(unsigned char wlan_idx, dma_addr_t setting_addr)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = wlan_idx;

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_SETTING, (u8 *)param_buf, sizeof(param_buf), (u8 *)setting_addr, sizeof(struct rtw_wifi_setting));

	return ret;
}

int whc_fullmac_host_channel_switch(dma_addr_t csa_param_addr)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_AP_CH_SWITCH, (u8 *)csa_param_addr, sizeof(struct rtw_csa_parm), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_pmksa_ops(dma_addr_t pmksa_ops_addr)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WPA_PMKSA_OPS, (u8 *)pmksa_ops_addr, sizeof(struct rtw_pmksa_ops_t), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_set_lps_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_LPS_EN, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_mp_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 2 * sizeof(u32) + cmd_len;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = 1;	/* show_msg*/
	param[1] = cmd_len;
	memcpy((void *)(param + 2), (void *)cmd_addr, cmd_len);

	whc_fullmac_host_send_event(WHC_API_WIFI_MP_CMD, (u8 *)param, size, (u8 *)user_addr, WIFI_MP_MSG_BUF_SIZE);

	/* free buffer */
	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_iwpriv_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, unsigned char *cmd, unsigned char *user_buf)
{
	int ret = 0;
	u32 size;
	u32 *param;
	u8 *output = NULL;

	size = 2 * sizeof(u32) + cmd_len;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = 1;	/* show_msg*/
	param[1] = cmd_len;
	memcpy((void *)(param + 2), (void *)cmd_addr, cmd_len);

	whc_fullmac_host_send_event(WHC_API_WIFI_IWPRIV_INFO, (u8 *)param, size, (u8 *)param, size);
	memcpy((u8 *)&ret, (u8 *)param, sizeof(int));
	if (ret == 0) {
		output = (u8 *)param + sizeof(int);
		memcpy((u8 *)user_buf, output, strlen(output));
		printk("%s", output);
	}
	kfree((void *)param);

	return ret;
}

u64 whc_fullmac_host_get_tsft(u8 iface_type)
{
	dev_err(global_idev.fullmac_dev, "%s sdio cannot access wifi reg directly!\n", __func__);
	return 0;
}

#ifdef CONFIG_NAN
int whc_fullmac_host_init_nan(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_NAN_INIT, NULL, 0, (u8 *)&ret, sizeof(int));
	return ret;
}

int whc_fullmac_host_deinit_nan(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_NAN_DEINIT, NULL, 0, (u8 *)&ret, sizeof(int));
	return ret;
}

int whc_fullmac_host_start_nan(u8 master_pref, u8 band_support)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)master_pref;
	param_buf[1] = (u32)band_support;

	whc_fullmac_host_send_event(WHC_API_NAN_START, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_stop_nan(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_NAN_STOP, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_add_nan_func(struct rtw_nan_func_info_t *func, void *nan_func_pointer)
{
	int ret = 0;
	u32 size;
	u8 *ptr, *param;

	size = sizeof(struct rtw_nan_func_info_t) + func->serv_spec_info_len + func->srf_bf_len + func->srf_num_macs * sizeof(struct mac_address)
		   + (func->num_tx_filters + func->num_rx_filters) * sizeof(struct cfg80211_nan_func_filter) + sizeof(void *);
	param = (u8 *)kzalloc(size, GFP_KERNEL);
	ptr = param;

	memcpy(ptr, func, sizeof(struct rtw_nan_func_info_t));
	ptr += sizeof(struct rtw_nan_func_info_t);

	memcpy(ptr, func->serv_spec_info, func->serv_spec_info_len);
	ptr += func->serv_spec_info_len;

	memcpy(ptr, func->srf_bf, func->srf_bf_len);
	ptr += func->srf_bf_len;

	memcpy(ptr, func->srf_macs, func->srf_num_macs * sizeof(struct mac_address));
	ptr += func->srf_num_macs * sizeof(struct mac_address);

	memcpy(ptr, func->tx_filters, func->num_tx_filters * sizeof(struct cfg80211_nan_func_filter));
	ptr += func->num_tx_filters * sizeof(struct cfg80211_nan_func_filter);

	memcpy(ptr, func->rx_filters, func->num_rx_filters * sizeof(struct cfg80211_nan_func_filter));
	ptr += func->num_rx_filters * sizeof(struct cfg80211_nan_func_filter);

	memcpy(ptr, &nan_func_pointer, sizeof(nan_func_pointer));

	whc_fullmac_host_send_event(WHC_API_NAN_ADD_FUNC, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_del_nan_func(u64 cookie)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)(cookie & 0xFFFFFFFF);
	param_buf[1] = (u32)((cookie >> 32) & 0xFFFFFFFF);

	whc_fullmac_host_send_event(WHC_API_NAN_DEL_FUNC, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_nan_cfgvendor_cmd(u16 vendor_cmd, const void *data, int len)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 2 * sizeof(u32) + len;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = (u32)vendor_cmd;
	param[1] = (u32)len;
	memcpy((void *)(param + 2), data, len);

	whc_fullmac_host_send_event(WHC_API_NAN_CFGVENFOR, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}
#endif

int whc_fullmac_host_set_pmf_mode(u8 pmf_mode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)pmf_mode;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_PMF_MODE, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_set_ch_plan(u8 ch_plan)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)ch_plan;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_CHPLAN, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));
	return ret;
}

int whc_fullmac_host_set_wps_phase(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)enable;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_WPS_PHASE, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));
	return ret;
}

int whc_fullmac_host_set_wpa_mode(u8 wpa_mode)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)wpa_mode;

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_WPA_MODE, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));
	return ret;
}

int whc_fullmac_host_set_owe_param(struct rtw_owe_param_t *owe_param)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = sizeof(struct rtw_owe_param_t);
	param = (u32 *)kzalloc(size, GFP_KERNEL);
	memcpy((void *)param, (void *)owe_param, size);
	whc_fullmac_host_send_event(WHC_API_WIFI_SET_OWE_PARAM, (u8 *)param, size, (u8 *)&ret, sizeof(int));
	kfree((void *)param);
	return ret;
}

int whc_fullmac_host_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 size;
	u32 *param;

	size = 3 * sizeof(u32) + buf_len;
	param = (u32 *)kzalloc(size, GFP_KERNEL);

	param[0] = (u32)wlan_idx;
	param[1] = (u32)buf_len;
	param[2] = (u32)flags;
	memcpy((void *)(param + 3), (void *)buf, buf_len);

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_GEN_IE, (u8 *)param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_add_custom_ie(const struct element **elem, u8 num, u16 type)
{
	int ret = 0;
	u32 size = 0;
	u8 *ptr, *param;
	u8 i = 0;

	size += 2;
	for (i = 0; i < num; i++) {
		size += 3 + elem[i]->datalen;
	}

	ptr = param = (u8 *)kzalloc(size, GFP_KERNEL);

	ptr[0] = 0;
	ptr[1] = num;
	ptr += 2;

	for (i = 0; i < num; i++) {
		ptr[0] = (u8)type;
		ptr[1] = elem[i]->id;
		ptr[2] = elem[i]->datalen;
		memcpy(ptr + 3, elem[i]->data, elem[i]->datalen);

		ptr += 3 + elem[i]->datalen;
	}

	whc_fullmac_host_send_event(WHC_API_WIFI_CUS_IE, param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_del_custom_ie(unsigned char wlan_idx)
{
	int ret = 0;
	u8 param[2];

	param[0] = 2;
	param[1] = wlan_idx;
	whc_fullmac_host_send_event(WHC_API_WIFI_CUS_IE, param, 2, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_update_custom_ie(u8 *ie, int ie_index, u8 type)
{
	int ret = 0;
	u32 size = 0;
	u8 *ptr, *param;

	size = 3 + 2 + ie[1];
	ptr = param = (u8 *)kzalloc(size, GFP_KERNEL);

	ptr[0] = 1;
	ptr[1] = (u8)ie_index;
	ptr[2] = type;
	ptr += 3;

	memcpy(ptr, ie, 2 + ie[1]);

	whc_fullmac_host_send_event(WHC_API_WIFI_CUS_IE, param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_set_edcca_mode(u8 edcca_mode)
{
	int ret = 0;
	u32 param_buf[1];

	switch (edcca_mode) {
	case RTW_EDCCA_NORM:
	case RTW_EDCCA_ADAPT:
	case RTW_EDCCA_CS:
	case RTW_EDCCA_DISABLE:
		param_buf[0] = (u32)edcca_mode;
		whc_fullmac_host_send_event(WHC_API_WIFI_SET_EDCCA_MODE, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));
		break;
	default:
		dev_info(global_idev.fullmac_dev, "Wrong EDCCA mode %d!", edcca_mode);
		dev_info(global_idev.fullmac_dev, "0: normal; 1: ETSI; 2: Japan; 9: Disable.");
		ret = -EINVAL;
		break;
	}
	return ret;
}

int whc_fullmac_host_get_edcca_mode(u8 *edcca_mode)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_EDCCA_MODE, NULL, 0, edcca_mode, sizeof(u8));

	return ret;
}

int whc_fullmac_host_get_ant_info(u8 *antdiv_mode, u8 *curr_ant)
{
	int ret = 0;
	u8 value[2];

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_ANTENNA_INFO, NULL, 0, value, sizeof(value));
	*antdiv_mode = value[0];
	*curr_ant = value[1];

	return ret;
}

int whc_fullmac_host_set_country_code(char *cc)
{
	int ret = 0;
	u8 param[2];

	if (strlen(cc) != 2) {
		dev_err(global_idev.fullmac_dev, "%s: the length of country is not 2.\n", __func__);
		return -EINVAL;
	}

	if ((cc[0] == '0') && (cc[1] == '0')) {
		param[0] = 0xff;
		param[1] = 0xff;
	} else {
		memcpy(param, cc, 2);
	}

	whc_fullmac_host_send_event(WHC_API_WIFI_SET_COUNTRY_CODE, param, sizeof(param), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_get_country_code(struct rtw_country_code_table *table)
{
	int ret = 0;

	if (table == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: input is NULL.\n", __func__);
		return -EINVAL;
	}

	whc_fullmac_host_send_event(WHC_API_WIFI_GET_COUNTRY_CODE, NULL, 0, (u8 *)table, sizeof(struct rtw_country_code_table));

	if ((table->char2[0] == 0xff) && (table->char2[1] == 0xff)) {
		table->char2[0] = '0';
		table->char2[1] = '0';
	}

	return ret;
}


#ifdef CONFIG_P2P
void whc_fullmac_host_set_p2p_role(enum rtw_p2p_role role)
{
	u32 param_buf[1];

	param_buf[0] = (u32)role;

	whc_fullmac_host_send_event(WHC_API_P2P_ROLE, (u8 *)param_buf, sizeof(param_buf), NULL, 0);
}

int whc_fullmac_host_set_p2p_remain_on_ch(unsigned char wlan_idx, u8 enable)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)enable;

	whc_fullmac_host_send_event(WHC_API_P2P_REMAIN_ON_CH, (u8 *)param_buf, sizeof(param_buf), (u8 *)&ret, sizeof(int));
	return ret;
}
#endif

int whc_fullmac_host_war_offload_ctrl(u8 offload_en, u32 offload_ctrl)
{
	int ret = 0;
	struct H2C_WAROFFLOAD_PARM param;

	param.offload_en = offload_en;
	param.offload_ctrl = offload_ctrl;

	whc_fullmac_host_send_event(WHC_API_WAR_OFFLOAD_CTRL, (u8 *)&param, sizeof(struct H2C_WAROFFLOAD_PARM), (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_set_mdns_param(u8 *pframe, u32 len)
{
	int ret = 0;
	u32 size = 0;
	u8 *param;

	size = 4 + len;
	param = (u8 *)kzalloc(size, GFP_KERNEL);

	memcpy(param, &len, sizeof(len));
	memcpy(param + 4, pframe, len);

	whc_fullmac_host_send_event(WHC_API_WAR_SET_MDNS_PARA, param, size, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

int whc_fullmac_host_dev_driver_is_mp(void)
{
	int ret = 0;

	whc_fullmac_host_send_event(WHC_API_WIFI_DRIVE_IS_MP, NULL, 0, (u8 *)&ret, sizeof(int));

	return ret;
}

int whc_fullmac_host_set_promisc_enable(u32 enable, u8 mode)
{
	(void) enable;
	(void) mode;

	/* Only smart customer needs promisc function in Linux.
	 * So to set this function to null. */
	return 0;
}
