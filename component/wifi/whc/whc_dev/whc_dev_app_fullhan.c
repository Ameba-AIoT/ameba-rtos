
#include "whc_dev_app.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"

#define WHC_WIFI_TEST_DISCONN        0x10
#define WHC_WIFI_TEST_SOFTAP         0x11
#define WHC_WIFI_TEST_CONN_STATUS    0x12

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
static struct rtw_softap_info ap = {0};
static unsigned char password[129] = {0};

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
void rtw_scan_result_for_fullhan(struct rtw_scan_result *result, u8 *buffer, size_t buffer_size)
{
	u8 *ptr = buffer;
	u8 len;
	(void)buffer_size;
	int value;

	memcpy(ptr, result->bssid.octet, 6);
	ptr += 6;

	//int in fullhan
	value = (int)result->signal_strength;
	memcpy(ptr, (void *)&value, sizeof(value));
	ptr += 4;

	//int in fullhan
	value = (int)result->channel;
	memcpy(ptr, (void *)&value, sizeof(value));
	ptr += 4;

	//u32 in fullhan
	memcpy(ptr, (void *)&result->security, sizeof(result->security));
	ptr += 4;

	len = (u8)result->ssid.len;
	*ptr = len;
	ptr += 1;
	memcpy(ptr, result->ssid.val, len);
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

			rtw_scan_result_for_fullhan(scanned_AP_list_index, ptr, buf_size - 8);
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

void whc_dev_enable_ap(u8 *buf)
{
	u8 *ptr = buf;
	u8 len;
	int value;
#ifdef CONFIG_LWIP_LAYER
	u32 ip_addr, netmask, gw;
	//struct ip_addr start_ip, end_ip;
#endif

	len = *ptr;
	ap.ssid.len = len;
	ptr += 1;
	memcpy(ap.ssid.val, ptr, len);
	ptr += len;

	memcpy(&value, ptr, 4);
	ap.channel = (u8)value;
	ptr += 4;

	memcpy(&value, ptr, 4);
	ptr += 4;
#ifdef CONFIG_WHC_DUAL_TCPIP
	memcpy(ap_ip, &value, 4);
#endif
	len = *ptr;
	ptr += 1;
	if (len > 0) {
		ap.password_len = len;
		memcpy(password, ptr, len);
		ap.password = password;
		ap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	} else {
		ap.security_type = RTW_SECURITY_OPEN;
	}
	//DiagPrintf("ap info: ssid %s ip %x psk %x %s \r\n", ap.ssid.val, value, ap.password, password);

	wifi_stop_ap();
	wifi_start_ap(&ap);
#ifdef CONFIG_LWIP_LAYER
	ip_addr = CONCAT_TO_UINT32(AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
	netmask = CONCAT_TO_UINT32(AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
	LwIP_SetIP(NETIF_WLAN_AP_INDEX, ip_addr, netmask, gw);
	extern void dhcps_init(struct netif * pnetif);
	dhcps_init(pnetif_ap);
#endif

	memset(ap.ssid.val, 0, sizeof(ap.ssid.val));
	ap.ssid.len = 0;
	ap.password = NULL;
	ap.password_len = 0;
	ap.channel = 0;
}

void whc_dev_cmd_connect_status(void)
{
	u8 status;
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_CONN_STATUS;
	ptr += 1;
	buf_len += 1;

	wifi_get_join_status(&status);
	*ptr = status;
	ptr += 1;
	buf_len += 1;

	whc_dev_api_send_to_host(buf, buf_len);
}

void whc_dev_cmd_get_macaddr(u8 idx)
{
	u8 *ptr = NULL;
	u8 *buf = rtos_mem_malloc(WHC_WIFI_TEST_BUF_SIZE);
	struct rtw_mac dev_mac = {0};

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
	rtos_mem_free(buf);
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
					idx = *(ptr + 1);
					if (!wifi_is_running(idx)) {
						RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
					} else {
						whc_dev_cmd_get_macaddr(idx);
					}
				} else if (*ptr == WHC_WIFI_TEST_DISCONN) {
					wifi_disconnect();
				}  else if (*ptr == WHC_WIFI_TEST_SOFTAP) {
					ptr++;
					if (*ptr == 0) {
						wifi_stop_ap();
					} else {
						ptr += 1;
						whc_dev_enable_ap(ptr);
					}
				} else if (*ptr == WHC_WIFI_TEST_CONN_STATUS) {
					whc_dev_cmd_connect_status();
				} else if (*ptr == WHC_WIFI_TEST_SCAN) {
					whc_dev_cmd_scan();
#ifdef CONFIG_LWIP_LAYER
				} else if (*ptr == WHC_WIFI_TEST_DHCP) {
					LwIP_netif_set_link_up(NETIF_WLAN_STA_INDEX);
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
						LwIP_IP_Address_Request(NETIF_WLAN_STA_INDEX);
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
						whc_dev_api_set_host_state(WHC_HOST_READY);
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
