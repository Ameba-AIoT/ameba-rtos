#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "whc_wpa_ops_cmd_define.h"
#include "whc_wpa_ops_cmd_parse.h"
#include "whc_wpa_lite_app.h"

#include "cfg_parse_config_ssid.h"

struct wpa_ops_ssid ssid_a = {0};
static struct rtw_network_info wifi = {0};

static void init_wifi_struct(void)
{
	memset(wifi.ssid.val, 0, sizeof(wifi.ssid.val));
	memset(wifi.bssid.octet, 0, ETH_ALEN);
	wifi.ssid.len = 0;
	wifi.password = NULL;
	wifi.password_len = 0;
	wifi.key_id = -1;
	wifi.channel = 0;
	wifi.pscan_option = 0;
	wifi.security_type = RTW_SECURITY_OPEN;
}

int whc_wpa_ops_get_status(char *ptr, u8 *buf)
{
	(void)ptr;
	struct rtw_wifi_setting *p_wifi_setting = NULL;
	struct whc_rtw_cli_wifi_status *p_wifi_status = NULL;
	u8 *buf_p = NULL;

	p_wifi_setting = (struct rtw_wifi_setting *)rtos_mem_zmalloc(sizeof(struct rtw_wifi_setting));
	if (p_wifi_setting == NULL) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, alloc p_wifi_setting fail\n", __FUNCTION__);
		return 0;
	}

	//TODO default set STA 0
	wifi_get_setting(0, p_wifi_setting);

	p_wifi_status = (struct whc_rtw_cli_wifi_status *)rtos_mem_zmalloc(sizeof(struct whc_rtw_cli_wifi_status));
	if (p_wifi_status == NULL) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, alloc p_wifi_status fail\n", __FUNCTION__);
		rtos_mem_free((void *)p_wifi_setting);
		return 0;
	}

	memcpy(p_wifi_status->ssid, p_wifi_setting->ssid, sizeof(p_wifi_status->ssid));
	memcpy(p_wifi_status->bssid, p_wifi_setting->bssid, sizeof(p_wifi_status->bssid));
	p_wifi_status->channel = p_wifi_setting->channel;

	memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
	buf_p = buf;
	*(u32 *)buf_p = WHC_WPA_OPS_UTIL;
	buf_p += 4;
	*buf_p = WHC_WPA_OPS_UTIL_GET_STATUS;
	buf_p += 1;

	memcpy(buf_p, p_wifi_status, sizeof(struct whc_rtw_cli_wifi_status));
	whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);

	rtos_mem_free((void *)p_wifi_setting);
	rtos_mem_free((void *)p_wifi_status);

	return 0;
}

int whc_wpa_ops_disconnect(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;
	int ret = 0;
	struct rtw_wifi_setting wifi_setting = {0};


	if (wifi_get_setting(STA_WLAN_INDEX, &wifi_setting) >= 0) {
		if (wifi_setting.mode == RTW_MODE_AP) {
			RTK_LOGI(TAG_WLAN_INIC, "%s, No need do disconnect for softap mode\n", __FUNCTION__);
			goto end;
		}
	} else {
		RTK_LOGI(TAG_WLAN_INIC, "%s, No need do disconnect when netif is down\n", __FUNCTION__);
		goto end;
	}

	/* Disconnecting ...... */
	ret = wifi_disconnect();
	if (ret < 0) {
		RTK_LOGI(TAG_WLAN_INIC, "%s, Disconnect ERROR\n", __FUNCTION__);
		goto end;
	}


end:
	init_wifi_struct();

	return 0;

}

int whc_wpa_ops_select_network(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;
	struct wpa_ops_ssid *ssid = &ssid_a;
	int ret = 0;

	RTK_LOGI(TAG_WLAN_INIC, "%s\n", __func__);

	if (ssid->ssid == NULL) {
		return 0;
	}

	init_wifi_struct();

	if (ssid->ssid != NULL) {
		strncpy((char *)wifi.ssid.val, (char *)ssid->ssid, ssid->ssid_len);
		wifi.ssid.len = ssid->ssid_len;
	}

	if (ssid->passphrase != NULL) {
		wifi.password = (u8 *)ssid->passphrase;
		wifi.password_len = strlen(ssid->passphrase);
	}

	//TODO key_mgmt

	ret = wifi_connect(&wifi, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGI(TAG_WLAN_INIC, "Fail:-0x%x", -ret);
		if ((ret == -RTK_ERR_WIFI_CONN_INVALID_KEY)) {
			RTK_LOGI(TAG_WLAN_INIC, "(password format wrong)\n");
		} else if (ret == -RTK_ERR_WIFI_CONN_SCAN_FAIL) {
			RTK_LOGI(TAG_WLAN_INIC, "(not found AP)\n");
		} else if (ret == -RTK_ERR_BUSY) {
			RTK_LOGI(TAG_WLAN_INIC, "(busy)\n");
		}
	}

	return 0;
}


int whc_wpa_ops_list_network(char *ptr, u8 *buf)
{
	(void)ptr;
	u8 *buf_p;
	struct wpa_ops_ssid *ssid = &ssid_a;

	memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);

	buf_p = buf;
	*(u32 *)buf_p = WHC_WPA_OPS_UTIL;
	buf_p += 4;
	*buf_p = WHC_WPA_OPS_UTIL_LIST_NETWORK;
	buf_p += 1;

	if (ssid->ssid != NULL) {
		memcpy(buf_p, ssid->ssid, ssid->ssid_len);
		whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);
	}


	return 0;
}

int whc_wpa_ops_set_network(char *ptr, u8 *buf, int msg_len)
{
	int ret = 0;
	char *var_name = NULL;
	char *value = NULL;
	char *space = NULL;
	struct wpa_ops_ssid *ssid = &ssid_a;
	(void)buf;

	RTK_LOGE(TAG_WLAN_INIC, "%s, len: %d; set_network: %s\n", __func__, msg_len, ptr);

	var_name = ptr;

	space = memchr(ptr, ' ', msg_len);
	if (space == NULL) {
		return -1;
	}

	*space = '\0';
	value = space + 1;

	RTK_LOGE(TAG_WLAN_INIC, "%s, host input name: %s, value: %s\n", __func__, var_name, value);

	ret = wpa_config_set(ssid, var_name, value, 0);
	if (ret < 0) {
		RTK_LOGE(TAG_WLAN_INIC, "CTRL_IFACE: Failed to set network "
				 "variable '%s'\n", var_name);
	}

	if (ssid->ssid) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, ssid: %s\n", __func__, ssid->ssid);
	}
	if (ssid->passphrase) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, passphrase: %s\n", __func__, ssid->passphrase);
	}
	if (ssid->key_mgmt) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, key_mgmt: %x\n", __func__, ssid->key_mgmt);
	}

	return ret;

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

s32 whc_rtw_cli_scan_callback(u32 scanned_AP_num, void *data)
{
	u8 *buf = NULL;
	u8 *ptr = NULL;
	struct rtw_scan_result *scanned_AP_list = NULL;
	(void)data;

	buf = rtos_mem_malloc(BRIDGE_WPA_OPS_BUF_SIZE);
	if (!buf) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
		return 0;
	}

	//Return Scan done
	memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
	ptr = buf;
	*(u32 *)ptr = WHC_WPA_OPS_EVENT;
	ptr += 4;
	*ptr = WHC_WPA_OPS_EVENT_SCAN_COMPLETE;
	ptr += 1;

	whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);


	if (scanned_AP_num != 0) {
		scanned_AP_list = (struct rtw_scan_result *)
						  rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));

		if (scanned_AP_list == NULL) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, ERROR: Cannot malloc memory for scan result\n", __func__);
		}

		if (wifi_get_scan_records(&scanned_AP_num, scanned_AP_list) < 0) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, ERROR: Get result failed\n", __func__);
			rtos_mem_free((void *)scanned_AP_list);
		}

		//Construct Return info
		int ix = scanned_AP_num;
		struct rtw_scan_result *scanned_AP_list_index = scanned_AP_list;
		while (ix > 0) {
			memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
			ptr = buf;
			*(u32 *)ptr = WHC_WPA_OPS_SOC_CB;
			ptr += 4;
			*ptr = WHC_WPA_OPS_SOC_CB_SCAN_RESULT;
			ptr += 1;
			*ptr = ix;
			ptr += 1;

			memcpy(ptr, scanned_AP_list_index, sizeof(struct rtw_scan_result));
			scanned_AP_list_index += 1;

			whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);

			ix -= 1;
		}
		//Free Scan AP list
		rtos_mem_free((void *)scanned_AP_list);
	}

	if (buf != NULL) {
		rtos_mem_free(buf);
	}

	return 0;

}


int whc_wpa_ops_do_scan(u8 *input, u8 *buf)
{
	struct rtw_scan_param scan_param;
	int ret = 0;
	u8 *ptr = NULL;

	(void) input;

	memset(&scan_param, 0, sizeof(struct rtw_scan_param));
	scan_param.scan_user_callback = whc_rtw_cli_scan_callback;

	RTK_LOGE(TAG_WLAN_INIC, "%s, Scan!!!!!!\n", __func__);
	ret = wifi_scan_networks(&scan_param, 0);
	if (ret < RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, wifi_scan_networks ERROR\n", __func__);
	}

	memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
	ptr = buf;
	*(u32 *)ptr = WHC_WPA_OPS_EVENT;
	ptr += 4;
	*ptr = WHC_WPA_OPS_EVENT_SCANING;
	ptr += 1;

	whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);


	return 0;

}

