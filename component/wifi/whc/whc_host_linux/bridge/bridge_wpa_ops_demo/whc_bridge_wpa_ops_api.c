#include <whc_host_linux.h>
#include <net/genetlink.h>
#include <whc_host_netlink.h>
#include <whc_bridge_wpa_ops_api.h>
#include <whc_bridge_wpa_ops_custom_netlink.h>

#if defined(CONFIG_WHC_BRIDGE)

int num_of_ap = 0;
int num_of_ap_index = 0;
struct rtw_scan_result *scan_results = NULL;

int whc_bridge_host_do_scan(struct sk_buff *skb, struct genl_info *info)
{
	u8 *buf;
	u8 *payload;
	u32 buf_len = SIZE_TX_DESC;
	u32 payload_len;
	u8 *ptr;

	if (!info->attrs[BRIDGE_ATTR_PAYLOAD]) {
		printk("Missing required payload in Netlink message\n");
		return -EINVAL;
	}
	payload = (char *)nla_data(info->attrs[BRIDGE_ATTR_PAYLOAD]);

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

	//TODO Free scan_result
	if (scan_results != NULL) {
		kfree(scan_results);
		scan_results = NULL;
	}
	num_of_ap_index = 0;
	num_of_ap = 0;


	return 0;
}

int whc_bridge_host_scan_result(void)
{
	char *buf;
	size_t total_len = 0;
	int chuck_index = 1;
	int last_chuck = 0;
	struct rtw_scan_result *scan_results_index = NULL;

	printk("whc_bridge_host_scan_result\n");

	if ((scan_results == NULL) || (num_of_ap == 0)) {
		return 0;
	}

	scan_results_index = scan_results;

	total_len = sizeof(struct rtw_scan_result);

	buf = kzalloc(total_len, GFP_KERNEL);
	if (buf == NULL) {
		return 0;
	}


	while (chuck_index <= num_of_ap) {

		if (chuck_index == num_of_ap) {
			last_chuck = 1;
		}

		memcpy(buf, scan_results_index, sizeof(struct rtw_scan_result));

		whc_bridge_host_send_to_user_multi(buf, total_len, CMD_WIFI_SCAN_RESULT, chuck_index, last_chuck);

		memset(buf, 0, sizeof(struct rtw_scan_result));
		scan_results_index++;
		chuck_index++;

	}

	kfree(buf);

	return 0;
}

int scan_result_cb(u8 *scan_data)
{
	//int ix = 0, ap_list_index = 0;
	struct rtw_scan_result *scanned_AP_info;

	if ((scan_data[0] > 0) && (scan_results == NULL)) {
		num_of_ap_index = 0;
		num_of_ap = scan_data[0];
		scan_results = kzalloc(sizeof(struct rtw_scan_result) * num_of_ap, GFP_KERNEL);
	}

	while (scan_data[0] > 0) {
		scan_data += 1;

		scanned_AP_info = (struct rtw_scan_result *)scan_data;
		scanned_AP_info->ssid.val[scanned_AP_info->ssid.len] = 0; /* Ensure the SSID is null terminated */

		memcpy(&scan_results[num_of_ap_index], scanned_AP_info, sizeof(struct rtw_scan_result));

		scan_data += sizeof(struct rtw_scan_result);

		num_of_ap_index++;
	}


	return 0;

}

int getmac_address_cb(u8 *macaddr)
{
	//TODO idx
	int idx = 0;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[idx]->dev_addr, macaddr, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[idx], macaddr);
#endif

	return 0;

}



int pre_process_buf_data(u8 *buf, u16 size)
{
	u32 bridge_event;
	u8 *buf_p = buf;
	int ret = FALSE;


	bridge_event = *(u32 *)buf_p;
	printk("%s event: %x\n", __FUNCTION__, bridge_event);

	buf_p = buf_p + sizeof(u32);

	//TODO need to lock/unlock the user command?
	if (bridge_event == WHC_WPA_OPS_SOC_CB) {
		printk("bridge_event: %x cmd_id: %d\n", bridge_event, *buf_p);
		switch (*buf_p) {
		case WHC_WPA_OPS_SOC_CB_SCAN_RESULT:

			buf_p += 1;
			scan_result_cb(buf_p);
			ret = TRUE;

			break;
		default:
			printk("Unknow command: %x\n", bridge_event);
			break;
		}

	} else if (bridge_event == WHC_WIFI_TEST) {
		printk("%s bridge_event: %x cmd_id: %d\n", __FUNCTION__, bridge_event, *buf_p);
		switch (*buf_p) {
		case WHC_WIFI_TEST_GET_MAC_ADDR:
			printk("%s GET MAC ADDR\n", __FUNCTION__);

			buf_p += 1;
			getmac_address_cb(buf_p);

			ret = FALSE;

			break;
		default:
			printk("Unknow command: %x\n", bridge_event);
			break;
		}


	}

	return ret;

}

#endif
