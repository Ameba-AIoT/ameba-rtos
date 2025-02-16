#include <rtw_cfg80211_fullmac.h>
#include <net/genetlink.h>
#include <rtw_bridge_netlink.h>

#if defined(CONFIG_FULLMAC_BRIDGE)

void rtw_bridge_setstate(void)
{
	u8 *ptr;
	/* send TX_DESC + info + data(param, param_len) */
	u32 buf_len = SIZE_TX_DESC + 16;
	u8 *buf = kzalloc(buf_len, GFP_KERNEL);

	ptr = buf;

	ptr += SIZE_TX_DESC;
	*(u32 *)ptr = INIC_WIFI_TEST;
	ptr += 4;
	*ptr = INIC_WIFI_TEST_SET_READY;
	ptr += 1;

	llhw_send_data_to_dev(buf, buf_len, 1);

	kfree(buf);
}

void rtw_bridge_getip(void)
{
	u8 *ptr;
	/* send TX_DESC + info + data(param, param_len) */
	u32 buf_len = SIZE_TX_DESC + 16;
	u8 *buf = kzalloc(buf_len, GFP_KERNEL);

	ptr = buf;

	ptr += SIZE_TX_DESC;
	*(u32 *)ptr = INIC_WIFI_TEST;
	ptr += 4;
	*ptr = INIC_WIFI_TEST_GET_IP;
	ptr += 1;

	llhw_send_data_to_dev(buf, buf_len, 1);

	kfree(buf);
}

void rtw_bridge_getmacaddr(void)
{
	u8 *ptr;
	/* send TX_DESC + info + data(param, param_len) */
	u32 buf_len = SIZE_TX_DESC + 16;
	u8 *buf = kzalloc(buf_len, GFP_KERNEL);

	ptr = buf;

	ptr += SIZE_TX_DESC;
	*(u32 *)ptr = INIC_WIFI_TEST;
	ptr += 4;
	*ptr = INIC_WIFI_TEST_GET_MAC_ADDR;
	ptr += 1;

	llhw_send_data_to_dev(buf, buf_len, 1);

	kfree(buf);
}

/* netlink cmd handler */
static int rtw_bridge_cmd_process(struct sk_buff *skb, struct genl_info *info)
{
	u32 cmd = nla_get_u32(info->attrs[BRIDGE_ATTR_API_ID]);
	u8 mac[6];
	u8 *dev_mac;

	if (cmd == CMD_GET_IP) {
		netif_carrier_on(global_idev.pndev[0]);
		rtw_bridge_getip();
	} else if (cmd == CMD_GET_MAC_ADDR) {
		rtw_bridge_getmacaddr();
	} else if (cmd == CMD_SET_HOST_STATE) {
		rtw_bridge_setstate();
	} else if (cmd == CMD_WIFI_SET_MAC) {
		if (info->attrs[BRIDGE_ATTR_MAC]) {
			dev_mac = (char *)nla_data(info->attrs[BRIDGE_ATTR_MAC]);
			sscanf(dev_mac, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
		memcpy((void *)global_idev.pndev[0]->dev_addr, mac, ETH_ALEN);
#else
		eth_hw_addr_set(global_idev.pndev[0], dev_mac);
#endif

	}
	return 0;
}

static const struct nla_policy rtw_bridge_cmd_policy[6] = {
	[BRIDGE_ATTR_API_ID] = {.type = NLA_U32},
	[BRIDGE_ATTR_MAC] = {.type = NLA_STRING},
};

/* netlink operation definition */
static struct genl_ops rtw_bridge_cmd_gnl_ops[] = {
	{
		.cmd = BRIDGE_CMD_ECHO,
		.doit = rtw_bridge_cmd_process,
	}
};

/* netlink family definition */
static struct genl_family rtw_bridge_gnl_family = {
	.name = BRIDGE_GENL_NAME,
	.version = 1,
	.maxattr = 6,
	.policy = rtw_bridge_cmd_policy,
	.ops = rtw_bridge_cmd_gnl_ops,
	.n_ops = ARRAY_SIZE(rtw_bridge_cmd_gnl_ops),
	.netnsok = true,
	.module = THIS_MODULE,
};

void rtw_bridge_register_genl_family(void)
{
	int ret = 0;
	ret = genl_register_family(&rtw_bridge_gnl_family);
	if (ret < 0) {
		printk("register genl family fail\n");
	}
}

void rtw_bridge_unregister_genl_family(void)
{
	int ret = 0;
	ret = genl_unregister_family(&rtw_bridge_gnl_family);
}

#endif
