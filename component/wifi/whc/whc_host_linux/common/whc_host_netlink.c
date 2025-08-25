#include <whc_host_linux.h>
#include <net/genetlink.h>
#include <whc_host_netlink.h>
#include <whc_host_cmd_path_api.h>

struct genl_info wifi_event_user_genl_info;


__attribute__((weak))  int whc_host_nl_custom_api(struct sk_buff *skb, struct genl_info *info)
{
	(void)skb;
	(void)info;

	return 0;
}

/* netlink cmd handler */
static int whc_host_nl_cmd_process(struct sk_buff *skb, struct genl_info *info)
{
	u32 cmd = nla_get_u32(info->attrs[WHC_ATTR_API_ID]);
	u8 mac[6];
	u8 *dev_mac;
	u8 *ptr;
	u8 *buf;
	u8 *payload;
	u8 idx = 0;
	u32 buf_len = SIZE_TX_DESC;
	u32 payload_len;
#ifndef CONFIG_WHC_BRIDGE
	char *user_buf;
#endif

	if (!info->attrs[WHC_ATTR_API_ID]) {
		printk("Missing required attributes in Netlink message\n");
		return -EINVAL;
	}

	if (cmd == CMD_WIFI_SEND_BUF) {
		if (!info->attrs[WHC_ATTR_PAYLOAD]) {
			printk("Missing required payload in Netlink message\n");
			return -EINVAL;
		}
		payload = (char *)nla_data(info->attrs[WHC_ATTR_PAYLOAD]);
		//payload_len = nla_len(info->attrs[WHC_ATTR_PAYLOAD]) - NLA_HDRLEN;
		payload_len = *(u32 *)payload;
		buf_len += payload_len;
		payload += 4;

		buf = kzalloc(buf_len, GFP_KERNEL);

		if (!buf) {
			printk("Failed to allocate memory for buffer\n");
			return -ENOMEM;
		}
		ptr = buf;
		ptr += SIZE_TX_DESC;

		memcpy(ptr, payload, payload_len);
		whc_host_send_data_to_dev(buf, buf_len, 1);
		kfree(buf);

	} else if (cmd == CMD_WIFI_SET_MAC) {
		if (info->attrs[WHC_ATTR_WLAN_IDX]) {
			idx = *(u8 *)nla_data(info->attrs[WHC_ATTR_WLAN_IDX]);
		} else {
			idx = 0;
		}
		if (info->attrs[WHC_ATTR_STRING]) {
			dev_mac = (char *)nla_data(info->attrs[WHC_ATTR_STRING]);
			sscanf(dev_mac, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
		memcpy((void *)global_idev.pndev[idx]->dev_addr, mac, ETH_ALEN);
#else
		eth_hw_addr_set(global_idev.pndev[idx], mac);
#endif

	} else if (cmd == CMD_WIFI_NETIF_ON) {
		if (info->attrs[WHC_ATTR_WLAN_IDX]) {
			idx = *(u8 *)nla_data(info->attrs[WHC_ATTR_WLAN_IDX]);
		} else {
			idx = 0;
		}
		netif_carrier_on(global_idev.pndev[idx]);
	} else if (cmd == CMD_WIFI_INFO_INIT) {
		memcpy(&wifi_event_user_genl_info, info, sizeof(struct genl_info));
#ifndef CONFIG_WHC_BRIDGE
	} else if (cmd == CMD_WIFI_MP) {
		buf = (char *)nla_data(info->attrs[WHC_ATTR_STRING]);
		buf_len = strlen(buf) + 1;
		user_buf = kzalloc(WHC_WIFI_MP_MSG_BUF_SIZE, GFP_KERNEL);
		whc_fullmac_host_mp_cmd((dma_addr_t)buf, buf_len, (dma_addr_t)user_buf);
		whc_host_buf_rx_to_user(user_buf, WHC_WIFI_MP_MSG_BUF_SIZE);

		kfree(user_buf);
	} else if (cmd == CMD_WIFI_DBG) {
		buf = (char *)nla_data(info->attrs[WHC_ATTR_STRING]);
		buf_len = strlen(buf) + 1;
		user_buf = kzalloc(WHC_WIFI_MP_MSG_BUF_SIZE, GFP_KERNEL);
		whc_fullmac_host_iwpriv_cmd((dma_addr_t)buf, buf_len, buf, user_buf);
		whc_host_buf_rx_to_user(user_buf, WHC_WIFI_MP_MSG_BUF_SIZE);
		kfree(user_buf);
#endif
	}

	return 0;
}

static const struct nla_policy whc_nl_cmd_policy[NUM_WHC_ATTR] = {
	[WHC_ATTR_API_ID] = {.type = NLA_U32},
	[WHC_ATTR_WLAN_IDX] = {.type = NLA_U8},
	[WHC_ATTR_STRING] = {.type = NLA_STRING},
	[WHC_ATTR_PAYLOAD] = {.type = NLA_BINARY},
	[WHC_ATTR_CHUNK_INDEX] = {.type = NLA_U32},
	[WHC_ATTR_LAST_CHUNK] = {.type = NLA_U8},
};

static const struct genl_multicast_group whc_mcgrps[] = {
	[WHC_MCGRP_EVENT] = { .name = "whc_event" },
};

/* netlink operation definition */
static struct genl_ops whc_nl_cmd_ops[] = {
	{
		.cmd = WHC_CMD_ECHO,
		.doit = whc_host_nl_cmd_process,
	},
	{
		.cmd = WHC_CMD_CUSTOM_API,
		.doit = whc_host_nl_custom_api,
	},
};

/* netlink family definition */
struct genl_family whc_nl_family = {
	.name = WHC_CMD_GENL_NAME,
	.version = 1,
	.maxattr = WHC_ATTR_MAX,
	.policy = whc_nl_cmd_policy,
	.ops = whc_nl_cmd_ops,
	.n_ops = ARRAY_SIZE(whc_nl_cmd_ops),
	.mcgrps = whc_mcgrps,
	.n_mcgrps = ARRAY_SIZE(whc_mcgrps),
	.netnsok = true,
	.module = THIS_MODULE,
};

__attribute__((weak)) int whc_host_buf_rx_to_user(u8 *buf, u16 size)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;
	int ret = 0;

	if (size == 0) {
		printk("whc_host_buf_rx_to_user: check pkt size");
		return -1;
	}

	skb = genlmsg_new(nla_total_size(size + 4), GFP_KERNEL);
	if (!skb) {
		return -1;
	}

	reply = genlmsg_put_reply(skb, &wifi_event_user_genl_info, &whc_nl_family, 0, WHC_CMD_EVENT);

	if (reply == NULL) {
		nlmsg_free(skb);
		return -1;
	}
	if (nla_put(skb, WHC_ATTR_PAYLOAD, size, buf)) {
		nlmsg_free(skb);
		printk("fail whc_host_buf_rx_to_user");
		return -EMSGSIZE;
	}
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	ret = genlmsg_reply(skb, &wifi_event_user_genl_info);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

void whc_host_register_genl_family(void)
{
	int ret = 0;
	ret = genl_register_family(&whc_nl_family);
	if (ret < 0) {
		printk("register genl family fail\n");
	}
}

void whc_host_unregister_genl_family(void)
{
	int ret = 0;
	ret = genl_unregister_family(&whc_nl_family);
}
