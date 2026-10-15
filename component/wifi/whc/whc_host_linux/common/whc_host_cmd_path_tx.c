#include <whc_host_linux.h>
#include <whc_host_cmd_path_api.h>
#include "whc_host_netlink.h"

/* SIZE_TX_DESC not included in len  */
/* pbuf needed to be freed by user if need */
void whc_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc)
{
	u8 *buf;
	u32 buf_len = len;

	if (with_txdesc) {
		whc_host_send_cmd_data(pbuf, buf_len);
	} else {
		/* SIZE_TX_DESC is needed by dev hw */
		buf_len += SIZE_TX_DESC;
		buf = kzalloc(buf_len, GFP_KERNEL);
		if (buf) {
			/* copy data */
			memcpy((u8 *)(buf + SIZE_TX_DESC), pbuf, len);

			/* send */
			whc_host_send_cmd_data(buf, buf_len);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
			kfree(buf);
#endif
		} else {
			dev_err(global_idev.pwhc_dev, "%s can't alloc buffer!\n", __func__);
		}
	}

}

void whc_host_update_network_info(u8 wlan_idx)
{
	struct {
		u32 event;
		u8 subcmd;
		u8 wlan_idx;
		u8 ip[4];
		u8 gw[4];
		u8 gw_mask[4];
		u8 ipv6[16];
	} __packed info;
	struct net_device *ndev;

	if (wlan_idx >= WHC_MAX_NET_PORT_NUM) {
		return;
	}

	memset(&info, 0, sizeof(info));
	info.event = WHC_WIFI_TEST;
	info.subcmd = WHC_WIFI_TEST_NETWORK_INFO_UPDATE;
	info.wlan_idx = wlan_idx;

	ndev = global_idev.pndev[wlan_idx];
	if (!ndev) {
		return;
	}

	rcu_read_lock();
	{
		struct in_device *in_dev = __in_dev_get_rcu(ndev);
		if (in_dev) {
			struct in_ifaddr *ifa = in_dev->ifa_list;
			if (ifa) {
				memcpy(info.ip,      &ifa->ifa_address, 4);
				memcpy(info.gw_mask, &ifa->ifa_mask,    4);
			}
		}
	}
	rcu_read_unlock();

	memcpy(info.ipv6, global_idev.ipv6_addr, 16);

	whc_host_send_data_to_dev((u8 *)&info, sizeof(info), 0);
}

/* compatible with IPC mode */
u32 whc_host_update_ip_addr(void)
{
	whc_host_update_network_info(STA_WLAN_INDEX);

	return 0;
}
