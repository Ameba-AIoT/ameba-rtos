
#include <rtw_cfg80211_fullmac.h>
#include <rtw_sdio_bridge.h>

#ifdef CONFIG_SDIO_BRIDGE

struct bridge_cmd_entry bridge_cmd_handlers[ ] = {
	{"wifi_connect", llhw_sdio_bridge_connect},
	{"disconnect", llhw_sdio_bridge_disconnect},
	{"scan", llhw_sdio_bridge_scan},
	{"dhcp", llhw_sdio_bridge_dhcp},
};

int parse_param(char *buf, char **argv)
{
	/* The argv[0] is ignored, so that the argc is 1 at least. */
	int argc = 1;
	static char temp_buf[100];
	char *buf_pos = temp_buf;
	memset(temp_buf, 0, sizeof(temp_buf));

	if (buf == NULL) {
		goto exit;
	}
	strncpy(temp_buf, buf, 100 - 1);

	while ((argc < 10) && (*buf_pos != '\0')) {
		/* The segmentation is comma or square bracket. */
		while ((*buf_pos == ',') || (*buf_pos == '[') || (*buf_pos == ']')) {
			*buf_pos = '\0';
			buf_pos++;
		}

		if (*buf_pos == '\0') {
			break;
		} else {
			/* argv[argc] is a part of temp_buf. */
			argv[argc] = buf_pos;
		}
		argc++;
		buf_pos++;

		while ((*buf_pos != ',') && (*buf_pos != '\0') && (*buf_pos != '[') && (*buf_pos != ']')) {
			buf_pos++;
		}
	}
exit:
	return argc;
}

int llhw_sdio_bridge_connect(char *param)
{
	const int ssid_idx = 1, password_idx = 2, keyid_idx = 3, bssid_idx = 4, channel_idx = 5;
	rtw_network_info_t wifi = {0};
	unsigned char password[129] = {0};
	unsigned int mac[ETH_ALEN];
	int ret = 0;
	int channel = 0;
	char *argv[10] = {0};
	int argc = 0;
	int i = 0;

	dev_info(global_idev.fullmac_dev, "%s\n", __func__);
	argc = parse_param(param, argv);
	if (argc < 2) {
		return -1;
	}

	/* SSID */
	wifi.ssid.len = strlen(argv[ssid_idx]);
	if ((argv[ssid_idx] == NULL) || (wifi.ssid.len >= 33)) {
		return -1;
	}
	strncpy((char *)wifi.ssid.val, argv[ssid_idx], wifi.ssid.len);

	/* Password (maybe not exist) */
	if ((argc > password_idx) && (argv[password_idx] != NULL)) {
		strncpy((char *)password, argv[password_idx], sizeof(password) - 1);
		wifi.password = password;
		wifi.password_len = strlen(argv[password_idx]);
		printk("passwrd=%s\n", password);
	}

	/* KEYID (maybe not exist) */
	wifi.key_id = -1;
	if ((argc > keyid_idx) && (argv[keyid_idx] != NULL)) {
		if ((strlen(argv[keyid_idx]) != 1) || (*(char *)argv[keyid_idx] < '0' || *(char *)argv[keyid_idx] > '3')) {
			return -1;
		}
		sscanf(argv[keyid_idx], "%d", &wifi.key_id);
	}

	/* BSSID (maybe not exist) */
	if ((argc > bssid_idx) && (argv[bssid_idx] != NULL)) {
		if (strlen(argv[bssid_idx]) != 17) {
			return -1;
		}
		sscanf(argv[bssid_idx], "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		for (i = 0; i < ETH_ALEN; i++) {
			wifi.bssid.octet[i] = mac[i] & 0xFF;
		}
	}

	/* Channel (maybe not exist) */
	if ((argc > channel_idx) && (argv[channel_idx] != NULL)) {
		//channel = (unsigned char)atoi(argv[channel_idx]);
		sscanf(argv[channel_idx], "%d", &channel);
		printk("channel=%d\n", channel);
	}
	wifi.channel = channel;

	/* check password */
	if (wifi.password != NULL) {
		wifi.security_type = ((wifi.key_id >= 0) && (wifi.key_id <= 3)) ? RTW_SECURITY_WEP_SHARED : RTW_SECURITY_WPA2_AES_PSK;
	} else {
		wifi.security_type = RTW_SECURITY_OPEN;
	}

	ret = llhw_wifi_connect(&wifi, 1);
	return ret;
}

int llhw_sdio_bridge_disconnect(char *param)
{
	int ret;

	ret = llhw_wifi_disconnect();
	return ret;
}

int llhw_sdio_bridge_scan(char *param)
{
	int ret;
	struct _rtw_scan_param_t scan_param;

	memset(&scan_param, 0, sizeof(struct _rtw_scan_param_t));
	scan_param.scan_user_callback = (enum _rtw_result_t(*)(unsigned int, void *))0xFFFFFFFF;
	ret = llhw_wifi_scan(&scan_param, 0, 0);
	return ret;
}

int llhw_sdio_bridge_dhcp(char *param)
{
	int ret;

	ret = 0;
	llhw_send_msg(INIC_API_BRIDGE_DHCP, NULL, 0, (u8 *)&ret, sizeof(int));
	return ret;
}

int llhw_sdio_bridge_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr)
{
	char *in_cmd = (char *)(cmd_addr);
	char *token = NULL;
	char *param = NULL;
	u32 i = 0;
	int ret = 0;
	int (*cmd_hdl)(char *param_buf);

	token = strsep(&in_cmd, "=");
	param = strsep(&in_cmd, "\0");
	printk("bridge cmd: %s:%s\n", token, param);
	cmd_hdl = NULL;
	for (i = 0; i < sizeof(bridge_cmd_handlers) / sizeof(struct bridge_cmd_entry); i++) {
		if (strcmp(token, bridge_cmd_handlers[i].bridge_cmd) == 0) {
			cmd_hdl = bridge_cmd_handlers[i].func;
			break;
		}
	}
	if (cmd_hdl != NULL) {
		ret = cmd_hdl(param);
	}

	return ret;
}

void llhw_sdio_bridge_get_scan_result(u32 ap_num)
{
	u32 param_buf[1];
	int i = 0;
	u8 *scan_buf;
	rtw_scan_result_t *ap_info;

	param_buf[0] = (u32)ap_num;
	scan_buf = (u8 *)kzalloc(ap_num * sizeof(rtw_scan_result_t), GFP_KERNEL);

	llhw_send_msg(INIC_API_WIFI_GET_SCANNED_AP_INFO, (u8 *)param_buf, sizeof(param_buf), (u8 *)scan_buf, ap_num * sizeof(rtw_scan_result_t));

	kfree((void *)scan_buf);
}

int llhw_sdio_bridge_sync_host_mac(u8 *addr)
{
	int ret = 0;
	u32 *param;

	param = (u32 *)kzalloc(ETH_ALEN, GFP_KERNEL);

	memcpy((void *)(param), addr, ETH_ALEN);

	llhw_send_msg(INIC_API_BRIDGE_SYNC_HOST_MAC, (u8 *)param, ETH_ALEN, (u8 *)&ret, sizeof(int));

	kfree((void *)param);

	return ret;
}

void llhw_sdio_bridge_event_join_status_indicate(void *event_priv, u32 *param_buf)
{
	enum rtw_event_indicate event = (enum rtw_event_indicate)param_buf[0];
	int flags = (int)param_buf[1];
	struct mlme_priv_t *mlme_priv = &global_idev.mlme_priv;

	if (event == WIFI_EVENT_JOIN_STATUS) {
		mlme_priv->rtw_join_status = flags;
		/* Merge from wifi_join_status_indicate. */
		if (flags == RTW_JOINSTATUS_SUCCESS) {
			/* if Synchronous connection, up sema when connect success */
			if (mlme_priv->join_block_param && mlme_priv->join_block_param->block) {
				complete(&mlme_priv->join_block_param->join_sema);
			}
			netif_carrier_on(global_idev.pndev[0]);
		} else if (flags == RTW_JOINSTATUS_FAIL) {
			dev_dbg(global_idev.fullmac_dev, "[fullmac] --- %s --- join failed up sema.", __func__);
			/* merge from wifi_join_status_indicate if synchronous connection, up sema when connect fail*/
			if (mlme_priv->join_block_param && mlme_priv->join_block_param->block) {
				complete(&mlme_priv->join_block_param->join_sema);
			}
		}
	} else if (event == WIFI_EVENT_DISCONNECT) {
		if (global_idev.mlme_priv.rtw_join_status == RTW_JOINSTATUS_DISCONNECT) {
			netif_carrier_off(global_idev.pndev[0]);
		}
	}

	return;
}
#endif
