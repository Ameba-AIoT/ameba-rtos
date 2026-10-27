#include "whc_dev.h"
#include "os_wrapper.h"
#include "whc_wpas_std_app.h"

extern int start_wpa_supplicant(char *iface_name);
extern int wpas_std_is_started(void);
extern void whc_rtw_cli_send_to_host(u8 idx, u32 cmd_category, u8 cmd_id,
									 u8 *user_data, u32 user_data_len);


int whc_wpa_ops_init_wpas_std(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	start_wpa_supplicant(NULL);

	return 0;
}


int whc_wpa_ops_get_status(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

int whc_dev_rtw_cli_remove_network(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}


int whc_dev_rtw_cli_wpas_cmd_hdl(char *ptr, u8 *buf, int msg_len)
{
	(void)buf;

	whc_dev_wpas_wpa_cli_cmd(ptr, msg_len);

	return 0;
}

int whc_wpa_ops_get_macaddr(u8 *ptr, u8 *buf)
{
	struct rtw_mac dev_mac = {0};
	u8 idx = 0;

	idx = *ptr;

	wifi_get_mac_address(idx, &dev_mac, 0);
	ptr = buf;
	*(u32 *)ptr = WHC_WPA_OPS_UTIL;
	ptr += 4;
	*ptr = WHC_WPA_OPS_UTIL_GET_MAC_ADDR;
	ptr += 1;
	*ptr = idx;
	ptr += 1;
	memcpy(ptr, dev_mac.octet, 6);
	//6+4+1+1=12
	whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE, NULL, 0);

	return 0;

}

int whc_wpa_ops_set_macaddr(u8 *ptr, u8 *buf)
{
	u8 idx = 0;
	u8 *mac = NULL;
	int ret = 0;

	(void)buf;

	idx = *ptr++;
	ptr++; /* = efuse byte from host; always write to RAM for safety */
	mac = ptr;

	RTK_LOGI(TAG_WLAN_INIC, "%s, idx: %d, mac: "
			 "%02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
			 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	if (idx != 0) {
		RTK_LOGW(TAG_WLAN_INIC, "%s, idx %d not supported, "
				 "wlan1 mac is derived from wlan0\n", __func__, idx);
		return 0;
	}

	/* wpa_supplicant caches the mac (incl. p2p device addr) at startup, set mac before start */
	if (wpas_std_is_started()) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, wpa_supplicant already started, set mac before start\n", __func__);
		return RTK_FAIL;
	}

	/*
	 * NOTE: efuse/OTP write is currently disabled for safety (RAM only).
	 * To enable it later:
	 *   1. Restore the efuse byte:  u8 efuse = *ptr++;
	 *   2. Pass it through:         wifi_set_mac_address(idx, mac, efuse);
	 *   3. Wire format is unchanged: [idx(1)][efuse(1)][mac(6)]
	 *      (userspace rtw_cli already accepts "setmac <idx> <mac> 1" and
	 *      warns that efuse writes are limited/irreversible).
	 */
	ret = wifi_set_mac_address(idx, mac, 0);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, set mac fail, ret: %d\n", __func__, ret);
	}

	return ret;
}

void whc_dev_rtw_cli_wpas_reply_info_hdl(u8 idx, const char *reply, size_t reply_len)
{

	whc_rtw_cli_send_to_host(idx, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_OFLD_RESULT,
							 (u8 *)reply, reply_len);
}


void whc_dev_rtw_cli_wpas_reply_event_hdl(u8 idx, const char *reply, size_t reply_len)
{

	whc_rtw_cli_send_to_host(idx, WHC_WPA_OPS_EVENT, WHC_WPA_OPS_EVENT_JOIN_STATUS,
							 (u8 *)reply, reply_len);
}

void whc_dev_rtw_cli_wpas_reply_scan_raw_hdl(u8 idx, const char *reply, size_t reply_len)
{

	whc_rtw_cli_send_to_host(idx, WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_SCAN_RAW_DATA,
							 (u8 *)reply, reply_len);
}


void whc_dev_rtw_cli_wpas_notify_event_hdl(u8 idx, u8 *notify_buf, u32 notify_len)
{
	/*
	 * notify_buf[0] is the WHC command ID (WHC_WPA_STD_EVENT_xxx).
	 * We strip it here because whc_rtw_cli_send_to_host prepends its own
	 * [cmd_id][idx] header.  The remaining bytes (sub-event + data) are
	 * forwarded verbatim so the host sees:
	 *   [category(4)][cmd_id][idx][sub_event][extra][extended_data...]
	 */
	if (!notify_buf || notify_len < 1) {
		return;
	}

	whc_rtw_cli_send_to_host(idx, WHC_WPA_STD_EVENT,
							 notify_buf[0],
							 notify_buf + 1, notify_len - 1);
}
