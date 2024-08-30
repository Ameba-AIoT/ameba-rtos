#include <rtw_cfg80211_fullmac.h>
#include <net/genetlink.h>
#include <rtw_sdio_bridge_netlink.h>

#ifdef CONFIG_SDIO_BRIDGE

u8 *g_scan_buf = NULL;
u32 g_scan_ap_num = 0;
struct genl_info wifi_event_user_genl_info;

void rtw_sdio_bridge_getip_rsp(struct genl_info *info);
void rtw_sdio_bridge_scanres_rsp(struct genl_info *info);
void rtw_sdio_bridge_connect_rsp(struct genl_info *info, int connect_ret);
void rtw_sdio_bridge_wifi_event_indicate(struct genl_info *info, int event);
extern void llhw_send_event(u32 id, u8 *param, u32 param_len, u8 *ret, u32 ret_len);

int llhw_sdio_bridge_connect(char *ssid, char *pwd)
{
	rtw_network_info_t wifi = {0};
	unsigned char password[129] = {0};
	int ret = 0;

	dev_info(global_idev.fullmac_dev, "%s\n", __func__);

	/* SSID */
	wifi.ssid.len = strlen(ssid);
	if ((ssid == NULL) || (wifi.ssid.len >= 33)) {
		return -1;
	}
	strncpy((char *)wifi.ssid.val, ssid, sizeof(wifi.ssid.val) - 1);

	/* Password (maybe not exist) */
	if (pwd) {
		strncpy((char *)password, pwd, sizeof(password) - 1);
		wifi.password = password;
		wifi.password_len = strlen(pwd);
	}

	wifi.key_id = -1;

	/* check password */
	if (wifi.password != NULL) {
		wifi.security_type = ((wifi.key_id >= 0) && (wifi.key_id <= 3)) ? RTW_SECURITY_WEP_SHARED : RTW_SECURITY_WPA2_AES_PSK;
	} else {
		wifi.security_type = RTW_SECURITY_OPEN;
	}

	ret = llhw_wifi_connect(&wifi, 1);
	return ret;
}

int llhw_sdio_bridge_dhcp(void)
{
	int ret;

	ret = 0;
	llhw_send_event(INIC_API_BRIDGE_DHCP, NULL, 0, (u8 *)&ret, sizeof(int));
	return ret;
}

void llhw_sdio_bridge_get_scan_result(u32 ap_num)
{
	u32 param_buf[1];
	u8 *scan_buf = NULL;
	struct event_priv_t *event_priv = &global_idev.event_priv;

	param_buf[0] = (u32)ap_num;
	if (g_scan_buf && scan_buf) {
		kfree((void *)scan_buf);
	}
	scan_buf = (u8 *)kzalloc(ap_num * sizeof(rtw_scan_result_t), GFP_KERNEL);

	llhw_send_event(INIC_API_WIFI_GET_SCANNED_AP_INFO, (u8 *)param_buf, sizeof(param_buf), (u8 *)scan_buf, ap_num * sizeof(rtw_scan_result_t));

	g_scan_buf = scan_buf;
	g_scan_ap_num = ap_num;
	complete(&event_priv->bridge_scan_done_sema);
	//kfree((void *)scan_buf); /*buf will be freed when user get scanres or new scan finished*/
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
		} else if (flags == RTW_JOINSTATUS_DISCONNECT) {
			if (global_idev.mlme_priv.b_in_disconnect) {
				complete(&global_idev.mlme_priv.disconnect_done_sema);
				global_idev.mlme_priv.b_in_disconnect = false;
			}
			netif_carrier_off(global_idev.pndev[0]);
			rtw_sdio_bridge_wifi_event_indicate(&wifi_event_user_genl_info, BRIDGE_WIFI_EVENT_DISCONNECT);
		}
	}

	return;
}

/* netlink cmd handler */
static int rtw_sdio_bridge_cmd_process(struct sk_buff *skb, struct genl_info *info)
{
	char *ssid = NULL;
	char *pwd = NULL;
	int ret;
	struct _rtw_scan_param_t scan_param;
	u32 cmd = nla_get_u32(info->attrs[BRIDGE_ATTR_API_ID]);
	struct event_priv_t	*event_priv = &global_idev.event_priv;
	u8 dev_mac[ETH_ALEN] = {0};

	if (cmd == CMD_WIFI_CONNECT) {
		if (info->attrs[BRIDGE_ATTR_SSID]) {
			ssid = (char *)nla_data(info->attrs[BRIDGE_ATTR_SSID]);
		}
		if (info->attrs[BRIDGE_ATTR_PWD]) {
			pwd = (char *)nla_data(info->attrs[BRIDGE_ATTR_PWD]);
		}
		ret = llhw_sdio_bridge_connect(ssid, pwd);
		if (ret == 0) {
			llhw_send_event(INIC_API_WIFI_GET_MAC_ADDR, NULL, 0, (u8 *)dev_mac, ETH_ALEN);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
			memcpy((void *)global_idev.pndev[0]->dev_addr, dev_mac, ETH_ALEN);
#else
			eth_hw_addr_set(global_idev.pndev[0], dev_mac);
#endif
			llhw_sdio_bridge_dhcp();
		}
		rtw_sdio_bridge_connect_rsp(info, ret);
	} else if (cmd == CMD_GET_IP) {
		rtw_sdio_bridge_getip_rsp(info);
	} else if (cmd == CMD_WIFI_SCAN) {
		memset(&scan_param, 0, sizeof(struct _rtw_scan_param_t));
		scan_param.scan_user_callback = (enum _rtw_result_t(*)(unsigned int, void *))0xFFFFFFFF;
		llhw_wifi_scan(&scan_param, 0, 0);
		if (wait_for_completion_timeout(&event_priv->bridge_scan_done_sema, msecs_to_jiffies(20000)) == 0) {
			dev_err(global_idev.fullmac_dev, "wait scan done timeout\n");
		}
	} else if (cmd == CMD_GET_SCAN_RES) {
		rtw_sdio_bridge_scanres_rsp(info);
	} else if (cmd == CMD_WIFI_DISCONNECT) {
		llhw_wifi_disconnect();
	} else if (cmd == CMD_EVENT_INIT) {
		memcpy(&wifi_event_user_genl_info, info, sizeof(struct genl_info));
	}
	return 0;
}

static const struct nla_policy rtw_sdio_bridge_cmd_policy[6] = {
	[BRIDGE_ATTR_API_ID] = {.type = NLA_U32},
	[BRIDGE_ATTR_SSID] = {.type = NLA_STRING},
	[BRIDGE_ATTR_PWD] = {.type = NLA_STRING},
	[BRIDGE_ATTR_SCAN_RES_ADDR] = {.type = NLA_U64},
	[BRIDGE_ATTR_API_RET] = {.type = NLA_S32},
};

/* netlink operation definition */
static struct genl_ops rtw_sdio_bridge_cmd_gnl_ops[] = {
	{
		.cmd = BRIDGE_CMD_ECHO,
		.doit = rtw_sdio_bridge_cmd_process,
	}
};

/* netlink family definition */
static struct genl_family rtw_sdio_bridge_gnl_family = {
	.name = SDIO_BRIDGE_GENL_NAME,
	.version = 1,
	.maxattr = 6,
	.policy = rtw_sdio_bridge_cmd_policy,
	.ops = rtw_sdio_bridge_cmd_gnl_ops,
	.n_ops = ARRAY_SIZE(rtw_sdio_bridge_cmd_gnl_ops),
	.netnsok = true,
	.module = THIS_MODULE,
};

void rtw_sdio_bridge_register_genl_family(void)
{
	int ret = 0;
	ret = genl_register_family(&rtw_sdio_bridge_gnl_family);
	if (ret < 0) {
		printk("register genl family fail\n");
	}
}

void rtw_sdio_bridge_unregister_genl_family(void)
{
	int ret = 0;
	ret = genl_unregister_family(&rtw_sdio_bridge_gnl_family);
}

void rtw_sdio_bridge_connect_rsp(struct genl_info *info, int connect_ret)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;


	skb = genlmsg_new(nla_total_size(sizeof(s32)), GFP_KERNEL);
	if (!skb) {
		return;
	}
	reply = genlmsg_put_reply(skb, info, &rtw_sdio_bridge_gnl_family, 0, BRIDGE_CMD_REPLY);
	if (reply == NULL) {
		nlmsg_free(skb);
		return;
	}
	nla_put_u32(skb, BRIDGE_ATTR_API_RET, (s32)connect_ret);
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	genlmsg_reply(skb, info);
}

void rtw_sdio_bridge_getip_rsp(struct genl_info *info)
{
	struct sk_buff *skb;
	void *reply;
	int ip;
	struct genlmsghdr *genlhdr;

	ip = 0;
	llhw_send_event(INIC_API_BRIDGE_GET_IP, NULL, 0, (u8 *)&ip, sizeof(int));

	skb = genlmsg_new(nla_total_size(sizeof(u32)), GFP_KERNEL);
	if (!skb) {
		return;
	}
	reply = genlmsg_put_reply(skb, info, &rtw_sdio_bridge_gnl_family, 0, BRIDGE_CMD_REPLY);
	if (reply == NULL) {
		nlmsg_free(skb);
		return;
	}
	nla_put_u32(skb, BRIDGE_ATTR_API_ID, (u32)ip);
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	genlmsg_reply(skb, info);
}

void rtw_sdio_bridge_scanres_rsp(struct genl_info *info)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;
	u64 userbuf;

	if (info->attrs[BRIDGE_ATTR_SCAN_RES_ADDR]) {
		userbuf = nla_get_u64(info->attrs[BRIDGE_ATTR_SCAN_RES_ADDR]);
		if (g_scan_buf) {
			copy_to_user((void *)userbuf, g_scan_buf, g_scan_ap_num * sizeof(rtw_scan_result_t));
		}
	}

	skb = genlmsg_new(nla_total_size(sizeof(u32)), GFP_KERNEL);
	if (!skb) {
		return;
	}
	reply = genlmsg_put_reply(skb, info, &rtw_sdio_bridge_gnl_family, 0, BRIDGE_CMD_REPLY);
	if (reply == NULL) {
		nlmsg_free(skb);
		return;
	}
	nla_put_u32(skb, BRIDGE_ATTR_API_ID, (u32)g_scan_ap_num);
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	genlmsg_reply(skb, info);

	if (g_scan_buf) {
		kfree(g_scan_buf);
		g_scan_ap_num = 0;
	}
}

void rtw_sdio_bridge_wifi_event_indicate(struct genl_info *info, int event)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;

	skb = genlmsg_new(nla_total_size(sizeof(u32)), GFP_KERNEL);
	if (!skb) {
		return;
	}

	reply = genlmsg_put_reply(skb, info, &rtw_sdio_bridge_gnl_family, 0, BRIDGE_CMD_EVENT);

	if (reply == NULL) {
		nlmsg_free(skb);
		return;
	}
	nla_put_u32(skb, BRIDGE_ATTR_API_ID, (u32)event);
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	genlmsg_reply(skb, info);
}
#endif
