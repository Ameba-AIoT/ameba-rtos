#include <whc_host_linux.h>
#include <net/genetlink.h>
#include <whc_host_rtw_cli_api.h>
#include <whc_host_rtw_cli_custom_netlink.h>

int num_of_ap = 0;
int num_of_ap_index = 0;
struct rtw_scan_result *scan_results = NULL;
char join_event[256] = {0};


int whc_host_get_join_status(struct sk_buff *skb, struct genl_info *info)
{
	(void)skb;
	(void)info;

	whc_host_send_to_user_multi(join_event, sizeof(join_event),
								API_WIFI_GET_JOIN_EVENT, 0, 0);

	return 0;
}

int whc_host_do_mp_cmd(struct sk_buff *skb, struct genl_info *info)
{
	u8 *buf = NULL;
	u8 *string_data = NULL;
	u32 string_len = 0;
	u32 buf_len;
	u8 *ptr;

	printk("%s %d\n", __FUNCTION__, __LINE__);

	if (!info->attrs[WHC_ATTR_STRING]) {
		printk("Missing required string in Netlink message\n");
		return -EINVAL;
	}

	string_data = (u8 *)nla_data(info->attrs[WHC_ATTR_STRING]);
	string_len = nla_len(info->attrs[WHC_ATTR_STRING]);

	/* payload: WHC_WIFI_TEST(4B) + WHC_WIFI_TEST_MP(1B) + show_msg(1B) + cmd_string */
	buf_len = SIZE_TX_DESC + 6 + string_len;
	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf) {
		printk("Failed to allocate memory for buffer\n");
		return -ENOMEM;
	}

	ptr = buf + SIZE_TX_DESC;
	*(u32 *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	*ptr = WHC_WIFI_TEST_MP;
	ptr += 1;
	*ptr = 1; /* show_msg */
	ptr += 1;
	memcpy(ptr, string_data, string_len);

	whc_host_cmd_data_send_to_dev(buf, buf_len, 1);
	kfree(buf);

	return 0;
}

#if defined(CONFIG_WHC_WIFI_API_PATH)
int whc_host_do_dbg_cmd(struct sk_buff *skb, struct genl_info *info)
{
	u8 *buf = NULL;
	u8 *string_data = NULL;
	u32 string_len = 0;

	printk("%s %d\n", __FUNCTION__, __LINE__);

	string_data = (char *)nla_data(info->attrs[WHC_ATTR_STRING]);
	string_len = nla_len(info->attrs[WHC_ATTR_STRING]);

	buf = kzalloc(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!buf) {
		printk("Failed to allocate memory for buffer\n");
		return -ENOMEM;
	}

	whc_host_iwpriv_cmd((dma_addr_t)string_data, string_len,
						string_data, buf);

	whc_host_send_to_user(buf, NLMSG_DEFAULT_SIZE, 0);

	kfree(buf);

	return 0;
}
#endif

int whc_host_do_scan(struct sk_buff *skb, struct genl_info *info)
{
	u8 *buf;
	u8 *payload;
	u32 buf_len = SIZE_TX_DESC;
	u32 payload_len;
	u8 *ptr;

	if (!info->attrs[WHC_ATTR_PAYLOAD]) {
		printk("Missing required payload in Netlink message\n");
		return -EINVAL;
	}
	payload = (char *)nla_data(info->attrs[WHC_ATTR_PAYLOAD]);
	payload_len = nla_len(info->attrs[WHC_ATTR_PAYLOAD]);

	buf_len += payload_len;

	buf = kzalloc(buf_len, GFP_KERNEL);

	if (!buf) {
		printk("Failed to allocate memory for buffer\n");
		return -ENOMEM;
	}

	ptr = buf;
	ptr += SIZE_TX_DESC;

	memcpy(ptr, payload, payload_len);
	whc_host_cmd_data_send_to_dev(buf, buf_len, 1);
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

int whc_host_scan_result(void)
{
	char *buf;
	size_t total_len = 0;
	int chuck_index = 1;
	int last_chuck = 0;
	struct rtw_scan_result *scan_results_index = NULL;

	printk("whc_host_scan_result\n");

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

		whc_host_send_to_user_multi(buf, total_len, API_WIFI_SCAN_RESULT, chuck_index, last_chuck);

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

/*
 * Binary scan raw data format (device -> host via WHC):
 *
 * Offset  Size  Field             Description
 * ────────────────────────────────────────────────
 *  0       2    total_entries      Total number of BSS entries
 *  2       2    entry_index        0-based index (0xFFFF = done marker)
 *  4       2    data_len           Bytes following this header
 * ────────────────────────────────────────────────
 *  6       6    bssid              MAC address
 * 12       2    beacon_interval    Beacon interval (TU)
 * 14       2    capability_info    Capability info bits
 * 16       4    frequency          MHz
 * 20       4    signal             RSSI (dBm, as s32)
 * 24       1    ssid_len           SSID length
 * 25    ssid_len ssid              SSID string (not null-terminated)
 * 25+ssid  2    ie_len             Total IE length
 * 25+ssid+2 ie_len ies            Information elements
 */
static int scan_raw_data_cb(u8 *buf_p)
{
	u16 total_entries, entry_index, data_len;
	u8 *bssid;
	u16 beacon_interval, cap_info, ie_len;
	u32 freq, signal;
	u8 ssid_len;
	int chan;

	total_entries = buf_p[0] | (buf_p[1] << 8);
	entry_index   = buf_p[2] | (buf_p[3] << 8);
	data_len      = buf_p[4] | (buf_p[5] << 8);

	/* Done marker: entry_index == 0xFFFF */
	if (entry_index == 0xFFFF) {
		printk("[scan_raw] Scan complete -- %d AP(s) total\n",
			   total_entries);
		return 1;
	}

	/* Parse BSS entry fields */
	bssid           = buf_p + 6;
	beacon_interval = buf_p[12] | (buf_p[13] << 8);
	cap_info        = buf_p[14] | (buf_p[15] << 8);
	freq            = buf_p[16] | (buf_p[17] << 8)
					  | (buf_p[18] << 16) | (buf_p[19] << 24);
	signal          = buf_p[20] | (buf_p[21] << 8)
					  | (buf_p[22] << 16) | (buf_p[23] << 24);
	ssid_len        = buf_p[24];
	ie_len          = buf_p[25 + ssid_len]
					  | (buf_p[25 + ssid_len + 1] << 8);

	/* Convert frequency (MHz) to channel number */
	if (freq >= 2412 && freq <= 2484) {
		chan = (freq - 2412) / 5 + 1;
	} else if (freq >= 5180 && freq <= 5825) {
		chan = (freq - 5180) / 5 + 36;
	} else {
		chan = 0;
	}

	printk("[scan_raw] [%d/%d] %pM | %.*s | Ch %d (%d MHz) | "
		   "%d dBm | beacon %d | caps 0x%04x | ie_len %d\n",
		   entry_index + 1, total_entries,
		   bssid,
		   ssid_len, buf_p + 25,
		   chan, freq, (s32)signal,
		   beacon_interval, cap_info, ie_len);

	return 1;
}

static int getmac_address_cb(u8 *buf)
{
	u8 idx = 0;
	const u8 *macaddr;

	idx = buf[0];
	macaddr = buf + 1;

	if (!global_idev.pndev[idx]) {
		return 1;
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0))
	memcpy((void *)global_idev.pndev[idx]->dev_addr, macaddr, ETH_ALEN);
#else
	eth_hw_addr_set(global_idev.pndev[idx], macaddr);
#endif

	return 0;

}

static int join_event_cb(u8 *buf_p)
{
	static const char *const event_success    = "RTW_JOINSTATUS_SUCCESS";
	static const char *const event_disconnect = "RTW_JOINSTATUS_DISCONNECT";
	static const char *const event_p2p_group_started = "P2P-GROUP-STARTED";
	static const char *const event_p2p_group_removed = "P2P-GROUP-REMOVED";
	int idx = 0, ret = 0;

	if (buf_p == NULL) {
		return 0;
	}

	idx = *buf_p;

	buf_p += 1;

	printk("EVENT(idx: %d): %s\n", idx, buf_p);

	memset(join_event, 0, sizeof(join_event));

	ret = strscpy(join_event, buf_p, sizeof(join_event));

	if (strstr(join_event, event_success)) {
		netif_carrier_on(global_idev.pndev[idx]);
		netif_tx_wake_all_queues(global_idev.pndev[idx]);
	} else if (strstr(join_event, event_disconnect)) {
		netif_carrier_off(global_idev.pndev[idx]);
		netif_tx_stop_all_queues(global_idev.pndev[idx]);
	} else if (strstr(join_event, event_p2p_group_removed)) {
		netif_carrier_off(global_idev.pndev[idx]);
		netif_tx_stop_all_queues(global_idev.pndev[idx]);
	} else if (strstr(join_event, event_p2p_group_started)) {
		netif_carrier_on(global_idev.pndev[idx]);
		netif_tx_wake_all_queues(global_idev.pndev[idx]);
	}

	return 0;
}


int pre_process_buf_data(u8 *buf, u16 size)
{
	u32 whc_cmd_catg;
	u8 *buf_p = buf;
	int ret = FALSE;

	whc_cmd_catg = *(u32 *)buf_p;
	printk("%s event: %x\n", __FUNCTION__, whc_cmd_catg);

	buf_p = buf_p + sizeof(u32);

	//TODO need to lock/unlock the user command?
	if (whc_cmd_catg == WHC_WPA_OPS_SOC_CB) {
		printk("WHC_WPA_OPS_SOC_CB cmd_id: %d\n", *buf_p);
		switch (*buf_p) {
		case WHC_WPA_OPS_SOC_CB_SCAN_RESULT:

			buf_p += 1;
			scan_result_cb(buf_p);
			ret = TRUE;

			break;
		default:
			break;
		}

	} else if (whc_cmd_catg == WHC_WPA_OPS_UTIL) {
		printk("WHC_WPA_OPS_UTIL cmd_id: %d\n", *buf_p);
		switch (*buf_p) {
		case WHC_WPA_OPS_UTIL_GET_MAC_ADDR:
			printk("%s GET MAC ADDR\n", __FUNCTION__);

			buf_p += 1;
			getmac_address_cb(buf_p);

			ret = FALSE;

			break;
		default:
			break;
		}
	} else if (whc_cmd_catg == WHC_WPA_OPS_EVENT) {
		printk("WHC_WPA_OPS_EVENT cmd_id: %d\n", *buf_p);
		switch (*buf_p) {
		case WHC_WPA_OPS_EVENT_JOIN_STATUS:

			buf_p += 1;
			join_event_cb(buf_p);

			ret = FALSE;

			break;
		case WHC_WPA_OPS_EVENT_SCAN_RAW_DATA:

			buf_p += 2;  /* skip cmd_id + idx, now points to scan_raw payload */
			scan_raw_data_cb(buf_p);
			ret = TRUE;

			break;
		default:
			break;
		}
	}

	return ret;
}
