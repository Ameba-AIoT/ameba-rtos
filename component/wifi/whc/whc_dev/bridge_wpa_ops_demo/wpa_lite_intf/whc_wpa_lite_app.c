#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "whc_wpa_ops_cmd_define.h"
#include "whc_wpa_lite_app.h"

#include "cfg_parse_config_ssid.h"

extern rtos_sema_t whc_user_rx_sema;
extern u8 *whc_rx_msg;

struct wpa_ops_ssid ssid_a = {0};

int whc_wpa_ops_set_network(char *ptr)
{
	int msg_len = 0, ret = 0;
	char *var_name = NULL;
	char *value = NULL;
	char *space = NULL;
	struct wpa_ops_ssid *ssid = &ssid_a;

	msg_len = *ptr;
	ptr += 4;
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

void whc_dev_pkt_rx_to_user_task(void)
{
	u8 *ptr = NULL;
	u32 event = 0;
	u8 *buf, idx = 0;

	while (1) {
		rtos_sema_take(whc_user_rx_sema, RTOS_MAX_TIMEOUT);
		if (whc_rx_msg) {
			ptr = whc_rx_msg;
			event = *(u32 *)whc_rx_msg;
			ptr += 4;

			if (event == WHC_WPA_OPS_UTIL) {
				buf = rtos_mem_malloc(BRIDGE_WPA_OPS_BUF_SIZE);
				if (!buf) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
					return;
				}
				if (*ptr == WHC_WPA_OPS_UTIL_GET_MAC_ADDR) {
					struct rtw_mac dev_mac = {0};
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
						rtos_mem_free(buf);
						return;
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
				} else if (*ptr == WHC_WPA_OPS_UTIL_SET_NETWORK) {
					ptr += 1;

					whc_wpa_ops_set_network((char *)ptr);
				}
			} else if (event == WHC_WPA_OPS_CUSTOM_API) {
				buf = rtos_mem_malloc(BRIDGE_WPA_OPS_BUF_SIZE);
				if (!buf) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
					return;
				}
				if (*ptr == WHC_WPA_OPS_CUSTOM_API_SCAN) {
					struct rtw_scan_result *scanned_AP_list = NULL;
					struct rtw_scan_param scan_param;
					int ret = 0;
					u32 scanned_AP_num = 0;

					memset(&scan_param, 0, sizeof(struct rtw_scan_param));

					RTK_LOGE(TAG_WLAN_INIC, "%s, Scan!!!!!!\n", __func__);
					ret = wifi_scan_networks(&scan_param, 1);
					if (ret < RTK_SUCCESS) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, wifi_scan_networks ERROR\n", __func__);
					}

					//Return Scan done
					memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
					ptr = buf;
					*(u32 *)ptr = WHC_WPA_OPS_EVENT;
					ptr += 4;
					*ptr = WHC_WPA_OPS_EVENT_SCANING;
					ptr += 1;

					whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);


					/* get scan results and log them */
					scanned_AP_num = ret;

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

					//Return Scan done
					memset(buf, 0, BRIDGE_WPA_OPS_BUF_SIZE);
					ptr = buf;
					*(u32 *)ptr = WHC_WPA_OPS_EVENT;
					ptr += 4;
					*ptr = WHC_WPA_OPS_EVENT_SCAN_COMPLETE;
					ptr += 1;

					whc_dev_api_send_to_host(buf, BRIDGE_WPA_OPS_BUF_SIZE);
				}
				if (buf != NULL) {
					rtos_mem_free(buf);
				}
			}
			if (buf != NULL) {
				rtos_mem_free(buf);
			}
			whc_rx_msg = NULL;
		}
	}

}

