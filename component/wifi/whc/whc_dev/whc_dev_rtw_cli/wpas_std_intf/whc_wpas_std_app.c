#include "whc_dev.h"
#include "os_wrapper.h"
#include "whc_wpas_std_app.h"

extern int start_wpa_supplicant(char *iface_name);
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
	if (!wifi_is_running(idx)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
		rtos_mem_free(buf);
		return 0;
	}

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

void whc_dev_rtw_cli_wpas_reply_info_hdl(u8 idx, char *reply, size_t reply_len)
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


static u8 whc_dev_wpas_std_event_map(u8 supp_event)
{
	switch (supp_event) {
	case WPAS_AMEBA_EVENT_SCAN_RESULTS:
		return WHC_WPA_STD_EVENT_SCAN_RESULTS;
	case WPAS_AMEBA_EVENT_WIFI_DRV:
		return WHC_WPA_STD_EVENT_WIFI_DRV;
	default:
		return 0xFF;
	}
}

void whc_dev_rtw_cli_wpas_notify_event_hdl(u8 idx, u32 notify_event, u32 notify_sub_event, u32 size)
{
	u8 whc_cmd_id;
	u8 payload[4];

	(void)size;

	if (!notify_event) {
		return;
	}

	whc_cmd_id = whc_dev_wpas_std_event_map((u8)notify_event);
	if (whc_cmd_id == 0xFF) {
		return;
	}

	payload[0] = (u8)(notify_sub_event & 0xFF);		/* sub_event */
	payload[1] = (u8)((notify_sub_event >> 8) & 0xFF);	/* extra_data */
	payload[2] = 0;					/* reserved */
	payload[3] = 0;					/* reserved */

	whc_rtw_cli_send_to_host(idx, WHC_WPA_STD_EVENT,
							 whc_cmd_id, payload, sizeof(payload));
}
