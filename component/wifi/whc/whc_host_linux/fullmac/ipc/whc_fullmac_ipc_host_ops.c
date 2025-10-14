// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

int whc_fullmac_ipc_host_send_msg(u32 id, u32 *param_buf, u32 buf_len)
{
	struct event_priv_t *event_priv = &global_idev.event_priv;
	int ret = 0;
	int cnt = 120000;

	if (!global_idev.host_init_done) {
		dev_err(global_idev.fullmac_dev, "Host api err: wifi not init\n");
		ret = -1;
		goto func_exit;
	}

	if (!global_idev.event_ch) {
		dev_err(global_idev.fullmac_dev, "%s: event ch is NULL when to send msg!\n",  "event");
		ret = -1;
		goto func_exit;
	}

	mutex_lock(&(event_priv->iiha_send_mutex));

	memset((u8 *)(event_priv->preq_msg), 0, sizeof(struct whc_ipc_host_req_msg));
	event_priv->preq_msg->api_id = id;
	if (param_buf) {
		memcpy(event_priv->preq_msg->param_buf, param_buf, buf_len * sizeof(u32));
	}

	memset((u8 *) & (event_priv->api_ipc_msg), 0, sizeof(ipc_msg_struct_t));
	event_priv->api_ipc_msg.msg = (u32)event_priv->req_msg_phy_addr;
	event_priv->api_ipc_msg.msg_type = IPC_USER_POINT;
	event_priv->api_ipc_msg.msg_len = sizeof(struct whc_ipc_host_req_msg);

	dev_dbg(global_idev.fullmac_dev, "-----LINUX SEND IPC\n");

	ret = ameba_ipc_channel_send(global_idev.event_ch, &(event_priv->api_ipc_msg));
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "LINUX IPC SEND FAIL!!!! ret = %d", ret);
	}

	while (event_priv->preq_msg->api_id != WHC_API_PROCESS_DONE) {
		cnt --;
		if (cnt == 0) {
			dev_warn(global_idev.fullmac_dev, "wait remsg has been 1.2s,id:%x\n", event_priv->preq_msg->api_id);
		}
		udelay(10);
	}

	mutex_unlock(&(event_priv->iiha_send_mutex));

	ret = event_priv->preq_msg->ret;

func_exit:
	return ret;
}

int whc_fullmac_host_set_user_config(struct wifi_user_conf *pwifi_usrcfg)
{
	u32 param_buf[1];
	dma_addr_t phy_addr;
	int ret = 0;
	struct device *pdev = global_idev.ipc_dev;
	struct wifi_user_conf *pusrcfg;

	pusrcfg = rtw_malloc(sizeof(struct wifi_user_conf), &phy_addr);
	if (!pusrcfg) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		return -1;
	}
	memcpy(pusrcfg, pwifi_usrcfg, sizeof(struct wifi_user_conf));

	param_buf[0] = (u32)phy_addr;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_USR_CFG, param_buf, 1);
	rtw_mfree(sizeof(struct wifi_user_conf), pusrcfg, phy_addr);

	return ret;
}

void whc_fullmac_host_wifi_on(void)
{
	u32 param_buf[1];

	param_buf[0] = 0;

	whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_ON, param_buf, 1);
}

int whc_fullmac_host_set_mac_addr(u32 wlan_idx, u8 *addr)
{
	dma_addr_t phy_addr;
	u8 *mac_addr = NULL;
	u32 param_buf[3];
	int ret = 0;
	struct device *pdev = global_idev.ipc_dev;

	mac_addr = rtw_malloc(ETH_ALEN, &phy_addr);
	if (mac_addr == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc error!\n", __func__);
		return -ENOMEM;
	}
	memcpy(mac_addr, addr, ETH_ALEN);

	param_buf[0] = wlan_idx;
	param_buf[1] = (u32)phy_addr;
	/* Set mac address to ram . */
	param_buf[2] = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_MAC_ADDR, param_buf, 3);
	rtw_mfree(ETH_ALEN, mac_addr, phy_addr);

	return ret;
}

int whc_fullmac_host_scan(struct rtw_scan_param *scan_param, u32 ssid_length, u8 block)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;
	u32 param_buf[2];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = sizeof(struct rtw_scan_param);
	size_t offset = 0;
	struct rtw_scan_param *scan_param_tmp = NULL;

	if (block) {
		block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
		if (!block_param) {
			ret = -ENOMEM;
			goto error;
		}
		/* initialize scan_sema. */
		init_completion(&block_param->sema);
	}

	buf_vir = rtw_malloc(size, &buf_phy);
	size += (ssid_length + scan_param->channel_list_num);
	if (!buf_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		ret = -ENOMEM;
		goto error;
	}
	memcpy(buf_vir, scan_param, size);

	scan_param_tmp = (struct rtw_scan_param *)buf_vir;
	offset += sizeof(struct rtw_scan_param);
	if (ssid_length) {
		scan_param_tmp->ssid = (u8 *)(buf_phy + offset);
		offset += ssid_length;
	}
	if (scan_param->channel_list_num) {
		scan_param_tmp->channel_list = (unsigned char *)(buf_phy + offset);
	}

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)size;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SCAN_NETWROKS, param_buf, 2);

	if (ret < 0) {
		goto error;
	}

	if (block) {
		global_idev.mlme_priv.scan_block_param = block_param;

		if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_SCAN_TIMEOUT) == 0) {
			dev_err(global_idev.fullmac_dev, "%s: Scan timeout!\n", __func__);
			ret = -EINVAL;
		}
		global_idev.mlme_priv.scan_block_param = NULL;
	}

error:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
	}

	if (buf_vir) {
		rtw_mfree(size, buf_vir, buf_phy);
	}
	return ret;
}

int whc_fullmac_host_scan_abort(void)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;

	block_param = (struct internal_block_param *)kzalloc(sizeof(struct internal_block_param), GFP_KERNEL);
	if (!block_param) {
		ret = -ENOMEM;
		goto exit;
	}
	/* initialize scan_abort_sema. */
	init_completion(&block_param->sema);

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SCAN_ABORT, NULL, 0);

	if (ret < 0) {
		ret = 0;
		goto exit;  /* there is no scan ongoing, just return SUCCESS*/
	}

	global_idev.mlme_priv.scan_abort_block_param = block_param;

	if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_SCAN_ABORT_TIMEOUT) == 0) {
		dev_err(global_idev.fullmac_dev, "%s: Scan abort wait timeout!\n", __func__);
		ret = -EINVAL;
	}
	global_idev.mlme_priv.scan_abort_block_param = NULL;

exit:
	if (block_param) {
		complete_release(&block_param->sema);
		kfree((u8 *)block_param);
	}

	return ret;
}

int whc_fullmac_host_event_connect(struct rtw_network_info *connect_param, unsigned char block)
{
	int ret = 0;
	struct internal_block_param *block_param = NULL;
	u32 param_buf[2];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = 0;
	size_t offset = 0;
	struct rtw_network_info *connect_param_tmp = NULL;

#ifdef CONFIG_SUPPLICANT_SME
	/* step1: check if auth is finished */
	if ((global_idev.mlme_priv.rtw_join_status > RTW_JOINSTATUS_UNKNOWN)
		&& (global_idev.mlme_priv.rtw_join_status < RTW_JOINSTATUS_SUCCESS)
		&& (global_idev.mlme_priv.rtw_join_status != RTW_JOINSTATUS_AUTHENTICATED)) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: auth is not finished! rtw_join_status=%d\n", global_idev.mlme_priv.rtw_join_status);
		return -EBUSY;
	}
#else
	/* step1: check if there's ongoing connect*/
	if ((global_idev.mlme_priv.rtw_join_status > RTW_JOINSTATUS_UNKNOWN)
		&& (global_idev.mlme_priv.rtw_join_status < RTW_JOINSTATUS_SUCCESS)) {
		dev_err(global_idev.fullmac_dev, "[fullmac]: there is ongoing wifi connect!rtw_join_status=%d\n", global_idev.mlme_priv.rtw_join_status);
		return -EBUSY;
	}
#endif

	/*clear for last connect status */
	global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_STARTING;
	whc_fullmac_host_connect_indicate(RTW_JOINSTATUS_STARTING, NULL);

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
	size = sizeof(struct rtw_network_info) + connect_param->password_len;
	buf_vir = rtw_malloc(size, &buf_phy);
	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
		ret = -ENOMEM;
		goto error;
	}
	memcpy(buf_vir, connect_param, size);
	connect_param_tmp = (struct rtw_network_info *)buf_vir;
	offset += sizeof(struct rtw_network_info);
	if (connect_param->password_len) {
		connect_param_tmp->password = (unsigned char *)(buf_phy + offset);
	}

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)size;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_CONNECT, param_buf, 2);

	if (ret != 0) {
		ret = -EINVAL;
		global_idev.mlme_priv.rtw_join_status = RTW_JOINSTATUS_FAIL;
		goto error;
	}

	/* step4: wait connect finished for synchronous connection*/
	if (block) {
		global_idev.mlme_priv.join_block_param = block_param;

		if (wait_for_completion_interruptible_timeout(&block_param->sema, RTW_JOIN_TIMEOUT) == 0) {
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
		whc_fullmac_host_connect_indicate(RTW_JOINSTATUS_FAIL, NULL);
	}
	if (buf_vir) {
		rtw_mfree(size, buf_vir, buf_phy);
	}
	return ret;
}

int whc_fullmac_host_event_disconnect(u16 reason_code)
{
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	u32 param_buf[4];
	u32 ie_len = 0;
	int ret = 0;

#ifdef CONFIG_SUPPLICANT_SME
	if (global_idev.sme_priv.deauth_ies && global_idev.sme_priv.deauth_ie_len) {
		ie_len = global_idev.sme_priv.deauth_ie_len;
		if (ie_len != 0) {
			buf_vir = rtw_malloc(ie_len, &buf_phy);
			if (!buf_vir) {
				dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
				return -ENOMEM;
			}

			memcpy(buf_vir, global_idev.sme_priv.deauth_ies, ie_len);
		}
	}
#endif

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)ie_len;
	param_buf[2] = (u32)reason_code;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_DISCONNECT, param_buf, 3);

	if (buf_vir != NULL) {
		rtw_mfree(ie_len, buf_vir, buf_phy);
	}

	return ret;
}

int whc_fullmac_host_wifi_get_join_status(void)
{
	return global_idev.mlme_priv.rtw_join_status;
}

int whc_fullmac_host_set_channel(u32 wlan_idx, u8 ch)
{
	int ret = -1;
	u32 param_buf[2];

	param_buf[0] = wlan_idx;
	param_buf[1] = (u32)ch;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_CHANNEL, param_buf, 2);

	return ret;
}

int whc_fullmac_host_init_ap(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_INIT_AP, NULL, 0);
	return ret;
}

int whc_fullmac_host_deinit_ap(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_DEINIT_AP, NULL, 0);
	return ret;
}

int whc_fullmac_host_ap_del_client(u8 wlan_idx, u8 *mac)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)mac;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_AP_DEL_CLIENT, param_buf, 2);
	return ret;
}

int whc_fullmac_host_start_ap(struct rtw_softap_info *softAP_config)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = 0;
	size_t offset = 0;
	struct rtw_softap_info *softAP_config_tmp = NULL;

	size = sizeof(struct rtw_softap_info) + softAP_config->password_len;
	buf_vir = rtw_malloc(size, &buf_phy);
	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		return -1;
	}
	memcpy(buf_vir, softAP_config, size);
	softAP_config_tmp = (struct rtw_softap_info *)buf_vir;
	offset += sizeof(struct rtw_softap_info);
	if (softAP_config->password_len) {
		softAP_config_tmp->password = (unsigned char *)(buf_phy + offset);
	}

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)size;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_START_AP, param_buf, 2);
	if (buf_vir) {
		rtw_mfree(size, buf_vir, buf_phy);
	}
	return ret;
}

int whc_fullmac_host_stop_ap(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_STOP_AP, NULL, 0);

	return ret;
}

int whc_fullmac_host_set_EDCA_params(struct rtw_edca_param *pedca_param)
{
	u32 param_buf[1];
	dma_addr_t phy_addr;
	int ret = 0;
	struct device *pdev = global_idev.ipc_dev;
	struct rtw_edca_param *pedca_param_temp;

	pedca_param_temp = rtw_malloc(sizeof(struct rtw_edca_param), &phy_addr);
	if (!pedca_param_temp) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		return -1;
	}
	memcpy(pedca_param_temp, pedca_param, sizeof(struct rtw_edca_param));

	param_buf[0] = (u32)phy_addr;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_EDCA_PARAM, param_buf, 1);
	rtw_mfree(sizeof(struct rtw_edca_param), pedca_param_temp, phy_addr);

	return ret;
}

int whc_fullmac_host_add_key(struct rtw_crypt_info *crypt)
{
	int ret = 0;
	u32 param_buf[1];
	dma_addr_t dma_addr_crypt = 0;
	struct rtw_crypt_info *crypt_temp = NULL;
	struct device *pdev = global_idev.ipc_dev;

	crypt_temp = rtw_malloc(sizeof(struct rtw_crypt_info), &dma_addr_crypt);
	if (!crypt_temp) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		return -ENOMEM;
	}
	memcpy(crypt_temp, crypt, sizeof(struct rtw_crypt_info));

	param_buf[0] = (u32)dma_addr_crypt;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_ADD_KEY, param_buf, 1);
	rtw_mfree(sizeof(struct rtw_crypt_info), crypt_temp, dma_addr_crypt);

	return ret;
}

int whc_fullmac_host_wpa_4way_status_indicate(struct rtw_wpa_4way_status *rpt_4way)
{
	int ret = 0;
	u32 param_buf[1];
	dma_addr_t dma_addr_4way = 0;
	struct rtw_wpa_4way_status *prpt_4way = NULL;

	prpt_4way = rtw_malloc(sizeof(struct rtw_wpa_4way_status), &dma_addr_4way);
	if (!prpt_4way) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		return -ENOMEM;
	}
	memcpy(prpt_4way, rpt_4way, sizeof(struct rtw_wpa_4way_status));

	param_buf[0] = (u32)dma_addr_4way;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WPA_4WAY_REPORT, param_buf, 1);

	rtw_mfree(sizeof(struct rtw_wpa_4way_status), prpt_4way, dma_addr_4way);
	return ret;
}

int whc_fullmac_host_tx_mgnt(u8 wlan_idx, const u8 *buf, size_t buf_len, u8 need_wait_ack)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = 0;
	size_t offset = 0;
	struct _raw_data_desc_t *raw_data_desc = NULL;

	size = sizeof(struct _raw_data_desc_t) + buf_len;
	buf_vir = rtw_malloc(size, &buf_phy);

	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -1;
	}

	raw_data_desc = (struct _raw_data_desc_t *)buf_vir;
	offset += sizeof(struct _raw_data_desc_t);
	raw_data_desc->buf_len = buf_len;
	raw_data_desc->wlan_idx = wlan_idx;
	if (need_wait_ack) {
		raw_data_desc->flags |= RTW_SEND_AND_WAIT_ACK;
	}
	memcpy(buf_vir + offset, buf, buf_len);
	raw_data_desc->buf = (u8 *)(buf_phy + offset);

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)size;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SEND_MGNT, param_buf, 2);

	rtw_mfree(size, buf_vir, buf_phy);
	return ret;
}

int whc_fullmac_host_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	int ret = 0;
	u32 param_buf[3];
	u8 *mac_addr_tmp = NULL;
	dma_addr_t dma_addr_mac_addr = 0;
	struct device *pdev = global_idev.ipc_dev;

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)status;

	if (mac_addr) {
		mac_addr_tmp = rtw_malloc(ETH_ALEN, &dma_addr_mac_addr);
		if (!mac_addr_tmp) {
			dev_err(global_idev.fullmac_dev, "%s: malloc mac_addr failed!\n", __func__);
			return -ENOMEM;
		}
		memcpy(mac_addr_tmp, mac_addr, ETH_ALEN);
		param_buf[2] = (u32)dma_addr_mac_addr;
	} else {
		param_buf[2] = 0;
	}

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SAE_STATUS, param_buf, 3);

	if (mac_addr_tmp) {
		rtw_mfree(ETH_ALEN, mac_addr_tmp, dma_addr_mac_addr);
	}

	return ret;
}

u32 whc_fullmac_host_update_ip_addr(void)
{
	int ret = 0;
	struct event_priv_t *event_priv = &global_idev.event_priv;
	u32 param_buf[1] = {0};
	u32 try_cnt = 5000;//wait 10ms
	u8 *ip_addr = NULL;
	dma_addr_t ip_addr_phy = 0;

	ip_addr = dma_alloc_coherent(global_idev.fullmac_dev, sizeof(u32), &ip_addr_phy, GFP_KERNEL);
	if (!ip_addr) {
		dev_err(global_idev.fullmac_dev, "%s: allloc ip_addr error.\n", __func__);
		return -ENOMEM;
	}
	memcpy(ip_addr, global_idev.ip_addr, 4);

	dev_dbg(global_idev.fullmac_dev, "%s ip=[%d.%d.%d.%d]\n", __func__, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);

	param_buf[0] = (u32)ip_addr_phy;

	mutex_lock(&(event_priv->iiha_send_mutex));

	memset((u8 *)(event_priv->preq_msg), 0, sizeof(struct whc_ipc_host_req_msg));
	event_priv->preq_msg->api_id = WHC_API_WIFI_IP_UPDATE;
	event_priv->preq_msg->ret = -1;
	memcpy(event_priv->preq_msg->param_buf, param_buf, sizeof(u32));

	memset((u8 *) & (event_priv->api_ipc_msg), 0, sizeof(ipc_msg_struct_t));
	event_priv->api_ipc_msg.msg = (u32)event_priv->req_msg_phy_addr;
	event_priv->api_ipc_msg.msg_type = IPC_USER_POINT;
	event_priv->api_ipc_msg.msg_len = sizeof(struct whc_ipc_host_req_msg);

	ameba_ipc_channel_send(global_idev.event_ch, &(event_priv->api_ipc_msg));

	while (try_cnt) {
		if (event_priv->preq_msg->api_id != WHC_API_PROCESS_DONE) {
			try_cnt --;
			udelay(2);
		} else {
			break;
		}
	}

	if (try_cnt == 0) {
		dev_err(global_idev.fullmac_dev, "wowlan update ip address fail");
	}

	mutex_unlock(&(event_priv->iiha_send_mutex));
	ret = event_priv->preq_msg->ret;

	if (ip_addr) {
		dma_free_coherent(global_idev.fullmac_dev, sizeof(u32), ip_addr, ip_addr_phy);
		ip_addr = NULL;
	}
	return ret;
}

int whc_fullmac_host_get_traffic_stats(u8 wlan_idx, dma_addr_t stats_traffic)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;

	/* ptr of statistics to fullfill. */
	param_buf[1] = (u32)stats_traffic;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_TRAFFIC_STATS, param_buf, 2);

	return ret;
}

int whc_fullmac_host_get_phy_stats(u8 wlan_idx, const u8 *mac_addr, union rtw_phy_stats *stats)
{
	int ret = 0;
	u32 param_buf[3];
	dma_addr_t dma_addr_mac_addr = 0;
	u8 *mac_addr_temp = NULL;
	union rtw_phy_stats *stats_vir = NULL;
	dma_addr_t stats_phy = 0;
	struct device *pdev = global_idev.ipc_dev;

	param_buf[0] = (u32)wlan_idx;

	stats_vir = rtw_malloc(sizeof(union rtw_phy_stats), &stats_phy);
	if (!stats_vir) {
		dev_dbg(global_idev.fullmac_dev, "%s: malloc stats failed.", __func__);
		return -ENOMEM;
	}

	if (mac_addr) {
		mac_addr_temp = rtw_malloc(6, &dma_addr_mac_addr);
		if (!mac_addr_temp) {
			rtw_mfree(sizeof(union rtw_phy_stats), stats_vir, stats_phy);
			dev_err(global_idev.fullmac_dev, "%s: malloc dma_addr failed!\n", __func__);
			return -ENOMEM;
		}
		memcpy(mac_addr_temp, mac_addr, 6);
		param_buf[1] = (u32)dma_addr_mac_addr;
	} else {
		param_buf[1] = 0;
	}

	/* ptr of statistics to fullfill. */
	param_buf[2] = (u32)stats_phy;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_PHY_STATS, param_buf, 3);
	memcpy(stats, stats_vir, sizeof(union rtw_phy_stats));

	rtw_mfree(sizeof(union rtw_phy_stats), stats_vir, stats_phy);
	if (mac_addr) {
		rtw_mfree(6, mac_addr_temp, dma_addr_mac_addr);
	}

	return ret;
}

int whc_fullmac_host_get_setting(unsigned char wlan_idx, dma_addr_t setting_phy)
{
	int ret = 0;
	u32 param_buf[2];

	/* ptr of settings to fullfill. */
	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)setting_phy;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_SETTING, param_buf, 2);

	return ret;
}

int whc_fullmac_host_channel_switch(dma_addr_t csa_param_phy)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)csa_param_phy;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_AP_CH_SWITCH, param_buf, 1);
	return ret;
}

int whc_fullmac_host_pmksa_ops(dma_addr_t pmksa_ops_phy)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)pmksa_ops_phy;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WPA_PMKSA_OPS, param_buf, 1);

	return ret;
}

int whc_fullmac_host_set_lps_enable(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)enable;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_LPS_EN, param_buf, 1);
	return ret;
}

int whc_fullmac_host_mp_cmd(dma_addr_t cmd_phy, unsigned int cmd_len, dma_addr_t user_phy)
{
	u32 param_buf[4];

	param_buf[0] = (u32)cmd_phy;
	param_buf[1] = (u32)cmd_len;
	param_buf[2] = (u32)1;
	param_buf[3] = (u32)user_phy;

	return whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_MP_CMD, param_buf, 4);
}

int whc_fullmac_host_iwpriv_cmd(dma_addr_t cmd_phy, unsigned int cmd_len, unsigned char *cmd, unsigned char *user_buf)
{
	u32 param_buf[3];
	int ret = 0;

	param_buf[0] = (u32)cmd_phy;
	param_buf[1] = (u32)cmd_len;
	param_buf[2] = (u32)1;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_IWPRIV_INFO, param_buf, 3);
	memcpy((u8 *)user_buf, cmd, strlen(cmd));

	return ret;
}

void whc_fullmac_ipc_host_send_packet(struct whc_ipc_ex_msg *p_ipc_msg)
{
	struct ipc_msg_q_priv *msg_priv = &global_idev.msg_priv;
	struct whc_ipc_ex_msg *pmsg = msg_priv->p_inic_ipc_msg;
	ipc_msg_struct_t ipc_msg = {0};
	int try_cnt = 100000;

	/* Get the warning of queue's depth not enough in peer, delay send the
	 * the next message.
	 */
	if (p_ipc_msg->msg_queue_status == IPC_WIFI_MSG_MEMORY_NOT_ENOUGH) {
		dev_err(global_idev.fullmac_dev, "[AP]%s: p_ipc_msg->msg_queue_status not enough. Do not msleep here.\n", __func__);
		/* TX SKBBUF 10 + RX DONE 10 = MSG POOL 20. If msg memory is still not enough, need debug.  */
	}

	spin_lock_bh(&msg_priv->ipc_send_msg_lock);

	/* Wait for another port ack acknowledgement last message sending */
	while (pmsg->event_num != IPC_WIFI_MSG_READ_DONE) {
		udelay(2);
		try_cnt--;
		if (try_cnt == 0) {
			dev_warn(global_idev.fullmac_dev, "[AP] inic ipc wait timeout %d\n", pmsg->event_num);
			break;
		}
	}

	pmsg->event_num = p_ipc_msg->event_num;
	pmsg->msg_addr = p_ipc_msg->msg_addr;
	pmsg->msg_queue_status = p_ipc_msg->msg_queue_status;
	pmsg->wlan_idx = p_ipc_msg->wlan_idx;

	/* Send the new message after last one acknowledgement */
	ipc_msg.msg_type = IPC_USER_POINT;
	ipc_msg.msg = msg_priv->ipc_msg_phy_addr;
	ipc_msg.msg_len = sizeof(struct whc_ipc_ex_msg);

	ameba_ipc_channel_send(global_idev.data_ch, &ipc_msg);
	spin_unlock_bh(&msg_priv->ipc_send_msg_lock);
}

u64 whc_fullmac_host_get_tsft(u8 iface_type)
{
	u32 reg_tsf_low = 0, reg_tsf_high = 0;
	u64 tsft_val = 0;

	if (iface_type == 0) {
		reg_tsf_low = llhw_ipc_wifi_reg_read32(0x560);
		reg_tsf_high = llhw_ipc_wifi_reg_read32(0x564);
	} else if (iface_type == 1) {
		reg_tsf_low = llhw_ipc_wifi_reg_read32(0x568);
		reg_tsf_high = llhw_ipc_wifi_reg_read32(0x56C);
	} else {
		dev_warn(global_idev.fullmac_dev, "[AP] unknown port(%d)!\n", iface_type);
	}

	tsft_val = ((u64)reg_tsf_high << 32) | reg_tsf_low;
	return tsft_val;
}

#ifdef CONFIG_NAN
int whc_fullmac_host_init_nan(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_INIT, NULL, 0);
	return ret;
}

int whc_fullmac_host_deinit_nan(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_DEINIT, NULL, 0);
	return ret;
}

int whc_fullmac_host_start_nan(u8 master_pref, u8 band_support)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)master_pref;
	param_buf[1] = (u32)band_support;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_START, param_buf, 2);
	return ret;
}

int whc_fullmac_host_stop_nan(void)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_STOP, NULL, 0);
	return ret;
}

int whc_fullmac_host_add_nan_func(struct rtw_nan_func_t *func, void *nan_func_pointer)
{
	int ret = 0;
	size_t size = 0;
	size_t offset = 0;
	u32 param_buf[4];
	u64 func_pointer_addr = (u64)(uintptr_t)nan_func_pointer;
	struct rtw_nan_func_t *nan_param = NULL;
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;

	size = sizeof(struct rtw_nan_func_t) + func->serv_spec_info_len + func->srf_bf_len +
		   (func->srf_num_macs * sizeof(struct rtw_mac)) +
		   (func->num_rx_filters * sizeof(struct rtw_nan_func_filter)) +
		   (func->num_tx_filters * sizeof(struct rtw_nan_func_filter));

	buf_vir = rtw_malloc(size, &buf_phy);
	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: mapping dma error!\n", __func__);
		return -1;
	}
	memcpy(buf_vir, func, size);
	nan_param = (struct rtw_nan_func_t *)buf_vir;
	offset += sizeof(struct rtw_nan_func_t);
	if (func->serv_spec_info_len) {
		func->serv_spec_info = (unsigned char *)(buf_phy + offset);
		offset += func->serv_spec_info_len;
	}
	if (func->srf_bf_len) {
		func->srf_bf = (unsigned char *)(buf_phy + offset);
		offset += func->srf_bf_len;
	}
	if (func->srf_macs) {
		nan_param->srf_macs = (struct rtw_mac *)(buf_phy + offset);
		offset += func->srf_num_macs * sizeof(struct rtw_mac);
	}
	if (func->rx_filters) {
		nan_param->rx_filters = (struct rtw_nan_func_filter *)(buf_phy + offset);
		offset += func->num_rx_filters * sizeof(struct rtw_nan_func_filter);
	}
	if (func->tx_filters) {
		nan_param->tx_filters = (struct rtw_nan_func_filter *)(buf_phy + offset);
		offset += func->num_tx_filters * sizeof(struct rtw_nan_func_filter);
	}

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)(func_pointer_addr & 0xFFFFFFFF);
	param_buf[2] = (u32)((func_pointer_addr >> 32) & 0xFFFFFFFF);
	param_buf[3] = (u32)size;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_ADD_FUNC, param_buf, 4);
	if (buf_vir) {
		rtw_mfree(size, buf_vir, buf_phy);
	}
	return ret;
}

int whc_fullmac_host_del_nan_func(u64 cookie)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)(cookie & 0xFFFFFFFF);
	param_buf[1] = (u32)((cookie >> 32) & 0xFFFFFFFF);

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_DEL_FUNC, param_buf, 2);
	return ret;
}

int whc_fullmac_host_nan_cfgvendor_cmd(u16 vendor_cmd, const void *data, int len)
{
	int ret = 0;
	u32 param_buf[3];
	void *data_vir = NULL;
	dma_addr_t dma_data = 0;
	struct device *pdev = global_idev.ipc_dev;

	data_vir = rtw_malloc(len, &dma_data);
	if (data_vir == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		return -ENOMEM;
	}
	memcpy(data_vir, data, len);

	param_buf[0] = (u32)vendor_cmd;
	param_buf[1] = (u32)dma_data;
	param_buf[2] = (u32)len;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_NAN_CFGVENFOR, param_buf, 3);
	rtw_mfree(len, data_vir, dma_data);
	return ret;
}
#endif

#ifdef CONFIG_P2P
void whc_fullmac_host_set_p2p_role(enum rtw_p2p_role role)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)role;

	whc_fullmac_ipc_host_send_msg(WHC_API_P2P_ROLE, param_buf, 1);
}

int whc_fullmac_host_set_p2p_remain_on_ch(unsigned char wlan_idx, u8 enable)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)enable;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_P2P_REMAIN_ON_CH, param_buf, 2);
	return ret;
}
#endif

#ifdef CONFIG_SUPPLICANT_SME
void whc_fullmac_host_sme_auth(dma_addr_t auth_data_phy)
{
	u32 param_buf[1];

	param_buf[0] = auth_data_phy;
	whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SME_AUTH, param_buf, 1);
}

int whc_fullmac_host_sme_set_assocreq_ie(u8 *ie, size_t ie_len, u8 wpa_rsn_exist)
{
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	u32 param_buf[3];
	int ret = 0;

	buf_vir = rtw_malloc(ie_len, &buf_phy);
	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	memcpy(buf_vir, ie, ie_len);

	param_buf[0] = (u32)buf_phy;
	param_buf[1] = (u32)ie_len;
	param_buf[2] = (u32)wpa_rsn_exist;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SME_SET_ASSOCREQ_IE, param_buf, 3);

	rtw_mfree(ie_len, buf_vir, buf_phy);

	return ret;
}

#endif

int whc_fullmac_host_set_pmf_mode(u8 pmf_mode)
{
	int ret = 0;
	u32 param_buf[1];

	param_buf[0] = (u32)pmf_mode;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_PMF_MODE, param_buf, 1);
	return ret;
}

int whc_fullmac_host_set_wps_phase(u8 enable)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)enable;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_WPS_PHASE, param_buf, 1);
	return ret;
}

int whc_fullmac_host_set_wpa_mode(u8 wpa_mode)
{
	int ret = 0;
	u32 param_buf[1];
	param_buf[0] = (u32)wpa_mode;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_WPA_MODE, param_buf, 1);
	return ret;
}

int whc_fullmac_host_set_owe_param(struct rtw_owe_param_t *owe_param)
{
	int ret = 0;
	u32 param_buf[1];
	dma_addr_t dma_data = 0;
	struct rtw_owe_param_t *powe = NULL;
	struct device *pdev = global_idev.ipc_dev;

	powe = rtw_malloc(sizeof(struct rtw_owe_param_t), &dma_data);
	if (powe == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc error!\n", __func__);
		return -ENOMEM;
	}
	memcpy(powe, owe_param, sizeof(struct rtw_owe_param_t));
	param_buf[0] = (u32)dma_data;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_OWE_PARAM, param_buf, 1);
	rtw_mfree(sizeof(struct rtw_owe_param_t), powe, dma_data);
	return ret;
}

int whc_fullmac_host_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	int ret = 0;
	u32 param_buf[4];
	dma_addr_t dma_data = 0;
	char *data = NULL;
	struct device *pdev = global_idev.ipc_dev;

	data = rtw_malloc(buf_len, &dma_data);
	if (data == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc error!\n", __func__);
		return -ENOMEM;
	}
	memcpy(data, buf, buf_len);

	param_buf[0] = (u32)wlan_idx;
	param_buf[1] = (u32)dma_data;
	param_buf[2] = (u32)buf_len;
	param_buf[3] = (u32)flags;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_GEN_IE, param_buf, 4);
	rtw_mfree(buf_len, data, dma_data);
	return ret;
}

int whc_fullmac_host_add_custom_ie(const struct element **elem, u8 num, u16 type)
{
	int ret = 0;
	u32 param_buf[4];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = 0;
	size_t offset = 0;
	u8 *ie_vir = NULL;
	struct rtw_custom_ie *cus_ie_array = NULL;
	u8 i = 0;

	size = sizeof(struct rtw_custom_ie) * num;
	for (i = 0; i < num; i++) {
		size += elem[i]->datalen + 2;
	}

	buf_vir = rtw_malloc(size, &buf_phy);

	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}

	cus_ie_array = (struct rtw_custom_ie *)buf_vir;
	offset += sizeof(struct rtw_custom_ie) * num;

	for (i = 0; i < num; i++) {
		cus_ie_array[i].type = type;
		ie_vir = buf_vir + offset;
		ie_vir[0] = elem[i]->id;
		ie_vir[1] = elem[i]->datalen;
		memcpy(ie_vir + 2, elem[i]->data, elem[i]->datalen);
		cus_ie_array[i].ie = (u8 *)(buf_phy + offset);
		offset += elem[i]->datalen + 2;
	}

	param_buf[0] = (u32)0;
	param_buf[1] = (u32)buf_phy;
	param_buf[2] = (u32)size;
	param_buf[3] = (u32)num;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_CUS_IE, param_buf, 4);

	rtw_mfree(size, buf_vir, buf_phy);
	return ret;
}

int whc_fullmac_host_del_custom_ie(unsigned char wlan_idx)
{
	int ret = 0;
	u32 param_buf[2];

	param_buf[0] = (u32)2;
	param_buf[1] = (u32)wlan_idx;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_CUS_IE, param_buf, 2);

	return ret;
}

int whc_fullmac_host_update_custom_ie(u8 *ie, int ie_index, u8 type)
{
	int ret = 0;
	u32 param_buf[4];
	u8 *buf_vir = NULL;
	dma_addr_t buf_phy = 0;
	size_t size = 0;
	size_t offset = 0;
	struct rtw_custom_ie *cus_ie_array = NULL;

	size = sizeof(struct rtw_custom_ie) + ie[1] + 2;
	buf_vir = rtw_malloc(size, &buf_phy);

	if (!buf_vir) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed.", __func__);
		return -ENOMEM;
	}
	cus_ie_array = (struct rtw_custom_ie *)buf_vir;
	offset += sizeof(struct rtw_custom_ie);
	cus_ie_array->type = type;
	memcpy(buf_vir + offset, ie, ie[1] + 2);
	cus_ie_array->ie = (u8 *)(buf_phy + offset);

	param_buf[0] = (u32)1;
	param_buf[1] = (u32)buf_phy;
	param_buf[2] = (u32)size;
	param_buf[3] = (u32)ie_index;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_CUS_IE, param_buf, 4);

	rtw_mfree(size, buf_vir, buf_phy);

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
		ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_EDCCA_MODE, param_buf, 1);
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
	u32 param_buf[1];
	u8 *virt_addr = NULL;
	dma_addr_t dma_addr = 0;
	struct device *pdev = global_idev.ipc_dev;

	virt_addr = rtw_malloc(sizeof(u8), &dma_addr);
	if (virt_addr == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		ret = -ENOMEM;
		goto func_exit;
	}
	param_buf[0] = dma_addr;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_EDCCA_MODE, param_buf, 1);
	/* need do cache invalidate before get value */
	*edcca_mode = *virt_addr;
	rtw_mfree(sizeof(u8), virt_addr, dma_addr);

func_exit:
	return ret;
}

int whc_fullmac_host_get_ant_info(u8 *antdiv_mode, u8 *curr_ant)
{
	int ret = 0;
	u32 param_buf[2];
	u8 *virt_addr = NULL;
	dma_addr_t dma_addr = 0;
	struct device *pdev = global_idev.ipc_dev;

	virt_addr = rtw_malloc(2 * sizeof(u8), &dma_addr);
	if (virt_addr == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: malloc failed!\n", __func__);
		ret = -ENOMEM;
		goto func_exit;
	}
	param_buf[0] = dma_addr;
	param_buf[1] = dma_addr + 1;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_ANTENNA_INFO, param_buf, 2);
	*antdiv_mode = virt_addr[0];
	*curr_ant = virt_addr[1];

	rtw_mfree(2 * sizeof(u8), virt_addr, dma_addr);

func_exit:
	return ret;
}

int whc_fullmac_host_set_country_code(char *cc)
{
	int ret = 0;
	u32 param_buf[1];
	dma_addr_t phy_addr = 0;
	char *country_code = NULL;
	struct device *pdev = global_idev.ipc_dev;

	if (strlen(cc) != 2) {
		dev_err(global_idev.fullmac_dev, "%s: the length of country is not 2.\n", __func__);
		return -EINVAL;
	}

	country_code = rtw_malloc(2, &phy_addr);
	if (!country_code) {
		dev_err(global_idev.fullmac_dev, "%s: maolloc failed\n", __func__);
		return -ENOMEM;
	}
	memcpy(country_code, cc, 2);

	param_buf[0] = (u32)phy_addr;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_SET_COUNTRY_CODE, param_buf, 1);
	rtw_mfree(2, country_code, phy_addr);

	return ret;
}

int whc_fullmac_host_get_country_code(struct rtw_country_code_table *table)
{
	int ret = 0;
	u32 param_buf[1];
	dma_addr_t phy_addr = 0;
	struct rtw_country_code_table *virt_addr = NULL;
	struct device *pdev = global_idev.ipc_dev;

	if (table == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: input is NULL.\n", __func__);
		return -EINVAL;
	}

	virt_addr = rtw_malloc(sizeof(struct rtw_country_code_table), &phy_addr);
	if (virt_addr == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: allocate memory failed!\n", __func__);
		return -ENOMEM;
	}

	param_buf[0] = (u32)phy_addr;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_GET_COUNTRY_CODE, param_buf, 1);
	memcpy(table, virt_addr, sizeof(struct rtw_country_code_table));
	if ((virt_addr->char2[0] == 0xff) && (virt_addr->char2[1] == 0xff)) {
		table->char2[0] = '0';
		table->char2[1] = '0';
	}
	rtw_mfree(sizeof(struct rtw_country_code_table), virt_addr, phy_addr);

	return ret;
}

int whc_fullmac_host_dev_driver_is_mp(u8 *is_mp)
{
	int ret = 0;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_DRIVE_IS_MP, NULL, 0);
	if (ret < 0) {
		*is_mp = 0;
	} else {
		*is_mp = ret;
		ret = 0;
	}
	return ret;
}

int whc_fullmac_host_set_promisc_enable(u32 enable, u8 mode)
{
	int ret = 0;
	u32 param_buf[3];

	param_buf[0] = enable;
	param_buf[1] = (u32)mode;
	param_buf[2] = (u32)0xffffffff;
	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_PROMISC_INIT, param_buf, 3);

	return ret;
}

int whc_fullmac_host_ft_status_indicate(struct rtw_kvr_param_t *kvr_param, u16 status)
{
	dma_addr_t phy_addr = 0;
	u8 *param = NULL;
	u32 param_buf[2];
	int ret = 0;
	struct device *pdev = global_idev.ipc_dev;

	if (kvr_param) {
		param = rtw_malloc(sizeof(struct rtw_kvr_param_t), &phy_addr);
		if (param == NULL) {
			dev_err(global_idev.fullmac_dev, "%s: malloc error!\n", __func__);
			return -ENOMEM;
		}
		memcpy(param, kvr_param, sizeof(struct rtw_kvr_param_t));
	}

	param_buf[0] = phy_addr;
	param_buf[1] = (u32)status;

	ret = whc_fullmac_ipc_host_send_msg(WHC_API_WIFI_FT_STATUS, param_buf, 2);
	if (phy_addr) {
		rtw_mfree(sizeof(struct rtw_kvr_param_t), param, phy_addr);
	}

	return ret;
}
