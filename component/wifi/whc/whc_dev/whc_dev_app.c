
#include "whc_dev_app.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

rtos_sema_t whc_user_rx_sema;
u8 *whc_rx_msg = NULL;
/* spi add header before msg, different from others */
/* real addr needed for mem free */
u8 *whc_rx_msg_free_addr = NULL;

u16 rx_msg_size;
#ifndef CONFIG_MP_SHRINK
static struct rtw_network_info wifi = {0};
#endif
/* update from host in mode s1d */
struct whc_dev_network_info whc_network_info[2] = {0};

__weak int whc_dev_ip_in_table_indicate(u8 gate, u8 ip)
{
	(void)gate;
	(void)ip;

	//return 1 to forward all pkt now.
	return 1;
	//todo
}

__weak int whc_dev_get_lwip_info(u32 type, unsigned char *input, int index)
{
	int ret;
	(void)input;

	switch (type) {
	case WHC_WLAN_GET_IP:
		ret = (int)whc_network_info[index].ip;
		break;

	case WHC_WLAN_GET_GW:
		ret = (int)whc_network_info[index].gw;
		break;

	case WHC_WLAN_GET_GWMSK:
		ret = (int)whc_network_info[index].gw_mask;
		break;
	default:
		RTK_LOGE(TAG_WLAN_INIC, "%s, ERROR: unknown network info type\n", __func__);
		break;
	}

	return ret;
}

static void rtw_scan_result_to_string(struct rtw_scan_result *result, u8 *buffer, size_t buffer_size)
{
	char bssid_str[18];
	char security[36] = {0} ;
	snprintf(bssid_str, sizeof(bssid_str), "%02X:%02X:%02X:%02X:%02X:%02X",
			 result->bssid.octet[0], result->bssid.octet[1],
			 result->bssid.octet[2], result->bssid.octet[3],
			 result->bssid.octet[4], result->bssid.octet[5]);

	snprintf(security, sizeof(security), "[%s]", (result->security == RTW_SECURITY_OPEN) ? "Open" :
			 (result->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
			 (result->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
			 (result->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
			 (result->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
			 (result->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
			 (result->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
			 (result->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
			 (result->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
			 (result->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
			 (result->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
			 (result->security == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA TKIP Enterprise" :
			 (result->security == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA AES Enterprise" :
			 (result->security == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA Mixed Enterprise" :
			 (result->security == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA2 TKIP Enterprise" :
			 (result->security == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA2 AES Enterprise" :
			 (result->security == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA2 Mixed Enterprise" :
			 (result->security == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 TKIP Enterprise" :
			 (result->security == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 AES Enterprise" :
			 (result->security == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 Mixed Enterprise" :
#ifdef CONFIG_SAE_SUPPORT
			 (result->security == RTW_SECURITY_WPA3_AES_PSK) ? "WPA3-SAE AES" :
			 (result->security == RTW_SECURITY_WPA2_WPA3_MIXED) ? "WPA2/WPA3-SAE AES" :
#endif
			 (result->security == (WPA2_SECURITY | WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA2/WPA3 Enterprise" :
			 (result->security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA3 Enterprise" :
#ifdef CONFIG_OWE_SUPPORT
			 (result->security == RTW_SECURITY_WPA3_OWE) ? "WPA3-OWE" :
#endif
			 "Unknown");

	snprintf((char *)buffer, buffer_size, "%s %d %03u %.*s %.*s\r\n",
			 bssid_str,
			 result->signal_strength,
			 (unsigned int)result->channel,
			 (int)strlen(security), security,
			 (int)result->ssid.len, result->ssid.val);
}

s32 whc_dev_scan_callback(u32 scanned_AP_num, void *data)
{
	(void)data;
	struct rtw_scan_result *scanned_AP_list = NULL;
	u32 ap_num = scanned_AP_num;
	struct rtw_scan_result *scanned_AP_list_index;
	u32 idx = 1;
	/* set 1k temp, for ap info possible needed in host */
	u32 buf_size = 1024;
	u8 *result_buf;
	u8 *ptr;

	if (ap_num) {
		scanned_AP_list = (struct rtw_scan_result *)rtos_mem_zmalloc(scanned_AP_num * sizeof(struct rtw_scan_result));
		result_buf = rtos_mem_zmalloc(buf_size);
		if (wifi_get_scan_records(&ap_num, scanned_AP_list) < 0) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, ERROR: Get result failed\n", __func__);
			rtos_mem_free((void *)scanned_AP_list);
		}

		scanned_AP_list_index = scanned_AP_list;
		while (ap_num > 0) {
			memset(result_buf, 0, buf_size);
			ptr = result_buf;
			*(u32 *)ptr = WHC_WIFI_TEST;
			ptr += 4;
			*ptr = WHC_WIFI_TEST_SCAN_RESULT;
			ptr += 1;
			*ptr = idx++;
			ptr += 1;

			/* wrap to 4B aligned */
			ptr += 2;

			rtw_scan_result_to_string(scanned_AP_list_index, ptr, buf_size - 8);
			scanned_AP_list_index += 1;

			whc_dev_api_send_to_host(result_buf, buf_size);
			ap_num -= 1;
		}

		memset(result_buf, 0, buf_size);
		ptr = result_buf;
		*(u32 *)ptr = WHC_WIFI_TEST;
		ptr += 4;
		*ptr = WHC_WIFI_TEST_SCAN_RESULT;
		ptr += 1;
		/* 0 means end */
		*ptr = 0;
		ptr += 1;
		whc_dev_api_send_to_host(result_buf, buf_size);

		rtos_mem_free(result_buf);
		rtos_mem_free(scanned_AP_list);
	}

	return 0;
}

void whc_dev_cmd_scan(void)
{
	struct rtw_scan_param *scan_param;
	scan_param = rtos_mem_zmalloc(sizeof(struct rtw_scan_param));
	int ret;

	if (scan_param == NULL) {
		RTK_LOGE(TAG_WLAN_INIC, "%s mem fail!\n", __func__);
		return;
	}

	scan_param->scan_user_callback = whc_dev_scan_callback;
	ret = wifi_scan_networks(scan_param, 0);

	if (ret < RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_INIC, " wifi_scan_networks ERROR!\n");
		goto end;
	}


end:
	if (scan_param) {
		rtos_mem_free(scan_param);
	}

}

/* here in sdio rx done callback */
__weak void whc_dev_pkt_rx_to_user(u8 *rxbuf, u8 *real_buf, u16 size)
{
	while (whc_rx_msg) {
		/* waiting last msg done */
		rtos_time_delay_ms(1);
	}

	whc_rx_msg = rxbuf;
	whc_rx_msg_free_addr = real_buf;
	rx_msg_size = size;
	rtos_sema_give(whc_user_rx_sema);
}

__weak void whc_dev_pkt_rx_to_user_task(void)
{
	u8 *ptr = NULL;
	u32 event = 0;
	u8 *ip, *buf, idx = 0;
	(void)ip;
	u8 len;
	char *password = NULL;
	(void)len;
	(void)password;
	(void)idx;
	int ret;
	(void)ret;

	while (1) {
		rtos_sema_take(whc_user_rx_sema, RTOS_MAX_TIMEOUT);
		if (whc_rx_msg) {
			ptr = whc_rx_msg;
			event = *(u32 *)whc_rx_msg;
			ptr += 4;

			if (event == WHC_WIFI_TEST) {
				buf = rtos_mem_malloc(WHC_WIFI_TEST_BUF_SIZE);
				if (!buf) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
					return;
				}
#ifndef CONFIG_MP_SHRINK
				if (*ptr == WHC_WIFI_TEST_GET_MAC_ADDR) {
					struct rtw_mac dev_mac = {0};
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
					} else {
						wifi_get_mac_address(idx, &dev_mac, 0);
						ptr = buf;
						*(u32 *)ptr = WHC_WIFI_TEST;
						ptr += 4;
						*ptr = WHC_WIFI_TEST_GET_MAC_ADDR;
						ptr += 1;
						*ptr = idx;
						ptr += 1;
						memcpy(ptr, dev_mac.octet, 6);
						//6+4+1=11
						whc_dev_api_send_to_host(buf, WHC_WIFI_TEST_BUF_SIZE);
					}
				} else if (*ptr == WHC_WIFI_TEST_SCAN) {
					whc_dev_cmd_scan();
#ifdef CONFIG_LWIP_LAYER
				} else if (*ptr == WHC_WIFI_TEST_DHCP) {
					LwIP_netif_set_link_up(STA_WLAN_INDEX);
					/* Start DHCPClient */
					LwIP_IP_Address_Request(STA_WLAN_INDEX);
#endif
				} else if (*ptr == WHC_WIFI_TEST_CONNECT) {
					memset(&wifi, 0, sizeof(struct rtw_network_info));
					ptr += 1;
					len = *ptr;
					wifi.ssid.len = len;
					memcpy((char *)wifi.ssid.val, ptr + 1, len);

					ptr += len + 1;
					len = *ptr;
					if (len > 0) {
						password = rtos_mem_zmalloc(129);
						memcpy((char *)password, ptr + 1, len);
						wifi.password = (u8 *)password;
						wifi.password_len = len;
					}
					ret = wifi_connect(&wifi, 1);

					if (password) {
						rtos_mem_free(password);
					}
#ifdef CONFIG_LWIP_LAYER
					if (ret == RTK_SUCCESS) {
						/* Start DHCPClient */
						LwIP_IP_Address_Request(0);
					} else {
						RTK_LOGE(TAG_WLAN_INIC, "connect fail !\n");
					}
#endif

#ifdef CONFIG_LWIP_LAYER
				} else if (*ptr == WHC_WIFI_TEST_GET_IP) {
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
					} else {
						ip = LwIP_GetIP(idx);
						ptr = buf;
						*(u32 *)ptr = WHC_WIFI_TEST;
						ptr += 4;
						*ptr = WHC_WIFI_TEST_GET_IP;
						ptr += 1;
						memcpy(ptr, ip, 4);
						ptr += 4;
						ip = LwIP_GetGW(idx);
						memcpy(ptr, ip, 4);
						ptr += 4;
						whc_dev_api_send_to_host(buf, WHC_WIFI_TEST_BUF_SIZE);
					}
#endif
#ifdef CONFIG_WHC_DUAL_TCPIP
				} else if (*ptr == WHC_WIFI_TEST_SET_READY) {
					whc_dev_api_set_host_state(WHC_HOST_READY);
				} else if (*ptr == WHC_WIFI_TEST_SET_UNREADY) {
					whc_dev_api_set_host_state(WHC_HOST_UNREADY);
#endif
				} else if (*ptr == WHC_WIFI_TEST_SET_TICKPS_CMD) {
					whc_dev_tickps_cmd((struct whc_dev_ps_cmd *)(ptr + 1));
				} else if (*ptr == WHC_WIFI_TEST_WIFION) {
#ifdef CONFIG_WHC_DUAL_TCPIP
					whc_dev_api_set_host_state(WHC_HOST_READY);
#endif
					wifi_on(RTW_MODE_STA);
				} else if (*ptr == WHC_WIFI_TEST_SET_HOST_RTOS) {
					wifi_user_config.cfg80211 = 0;
				}
#endif
				rtos_mem_free(buf);
			}
			rtos_mem_free(whc_rx_msg_free_addr);
			whc_rx_msg = NULL;
			whc_rx_msg_free_addr = NULL;
		}
	}
}

__weak void whc_dev_init_cmd_path_task(void)
{
	/* initialize the semaphores */
	rtos_sema_create(&whc_user_rx_sema, 0, 0xFFFFFFFF);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_dev_pkt_rx_to_user_task", (rtos_task_function_t)whc_dev_pkt_rx_to_user_task,
										NULL,
										WHC_WHC_CMD_USER_TASK_STACK_SIZE, CONFIG_WHC_WHC_CMD_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_dev_pkt_rx_to_user_task Err!!\n");
	}

}
