#include <whc_host_linux.h>
#include <net/genetlink.h>
#include <whc_host_netlink.h>
#include <whc_host_cmd_path_api.h>
#include <whc_bridge_wpa_ops_api.h>
#include <whc_bridge_wpa_ops_custom_netlink.h>

#if defined(CONFIG_WHC_BRIDGE)

static u32 whc_user_portid;

/* netlink cmd handler */
int whc_host_nl_custom_api(struct sk_buff *skb, struct genl_info *info)
{
	u32 cmd = nla_get_u32(info->attrs[WHC_ATTR_API_ID]);
	u8 *ptr;
	u8 *buf;
	u8 *payload;
	u32 buf_len = SIZE_TX_DESC;
	u32 payload_len;

	whc_user_portid = info->snd_portid;

	if (!info->attrs[WHC_ATTR_API_ID]) {
		printk("Missing required attributes in Netlink message\n");
		return -EINVAL;
	}

	printk("%s cmd: %x\n", __FUNCTION__, cmd);

	if (cmd == CMD_WIFI_SEND_BUF) {
		if (!info->attrs[WHC_ATTR_PAYLOAD]) {
			printk("Missing required payload in Netlink message\n");
			return -EINVAL;
		}
		payload = (char *)nla_data(info->attrs[WHC_ATTR_PAYLOAD]);
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

	} else if (cmd == CMD_WIFI_DO_SCAN) {
		whc_bridge_host_do_scan(skb, info);
	} else if (cmd == CMD_WIFI_SCAN_RESULT) {
		whc_bridge_host_scan_result();
	}
	return 0;
}

int whc_bridge_host_send_to_user_multi(u8 *buf, u16 size, u16 api_id, u32 chuck_index, u8 last_chuck)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;
	int ret = 0;

	if (size == 0) {
		printk("%s: check pkt size\n", __FUNCTION__);
		return -1;
	}

	skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb) {
		return -1;
	}

	reply = genlmsg_put(skb, 0, 0, &whc_nl_family, 0, WHC_CMD_EVENT);

	if (reply == NULL) {
		nlmsg_free(skb);
		return -1;
	}

	nla_put_u32(skb, WHC_ATTR_API_ID, api_id);
	nla_put_u32(skb, WHC_ATTR_CHUNK_INDEX, chuck_index);
	nla_put_u8(skb, WHC_ATTR_LAST_CHUNK, last_chuck);


	if (nla_put(skb, WHC_ATTR_PAYLOAD, size, buf)) {
		nlmsg_free(skb);
		printk("%s fail\n", __FUNCTION__);
		return -EMSGSIZE;
	}
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	ret = genlmsg_unicast(&init_net, skb, whc_user_portid);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int whc_bridge_host_send_to_user(u8 *buf, u16 size, u16 api_id)
{
	struct sk_buff *skb;
	void *reply;
	struct genlmsghdr *genlhdr;
	int ret = 0;

	if (size == 0) {
		printk("%s: check pkt size\n", __FUNCTION__);
		return -1;
	}

	if (pre_process_buf_data(buf, size) == TRUE) {
		return ret;
	}

	skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb) {
		return -1;
	}

	reply = genlmsg_put(skb, 0, 0, &whc_nl_family, 0, WHC_CMD_EVENT);

	if (reply == NULL) {
		nlmsg_free(skb);
		return -1;
	}

	nla_put_u32(skb, WHC_ATTR_API_ID, api_id);


	if (nla_put(skb, WHC_ATTR_PAYLOAD, size, buf)) {
		nlmsg_free(skb);
		printk("%s fail\n", __FUNCTION__);
		return -EMSGSIZE;
	}
	genlhdr = nlmsg_data(nlmsg_hdr(skb));
	reply = genlmsg_data(genlhdr);
	genlmsg_end(skb, reply);

	ret = genlmsg_unicast(&init_net, skb, whc_user_portid);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int whc_host_buf_rx_to_user(u8 *buf, u16 size)
{
	int ret = 0;

	ret = whc_bridge_host_send_to_user(buf, size, 0);

	return ret;
}


#endif
