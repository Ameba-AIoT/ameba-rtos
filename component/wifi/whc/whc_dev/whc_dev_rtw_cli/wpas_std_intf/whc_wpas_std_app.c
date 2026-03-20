#include "whc_dev.h"
#include "os_wrapper.h"
#include "whc_dev_rtw_cli_cmd_define.h"
#include "whc_dev_rtw_cli_cmd_parse.h"

extern int start_wpa_supplicant(char *iface_name);
extern void whc_rtw_cli_send_to_host(u32 cmd_category, u8 cmd_id,
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
	memcpy(ptr, dev_mac.octet, 6);
	//6+4+1=11
	whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);

	return 0;

}

void whc_dev_rtw_cli_wpas_reply_hdl(char *test_rx, char *reply, size_t reply_len)
{
	(void)test_rx;

	whc_rtw_cli_send_to_host(WHC_WPA_OPS_UTIL, WHC_WPA_OPS_UTIL_OFLD_RESULT,
							 (u8 *)reply, reply_len);
}


