#include "whc_dev.h"
#include "lwip/sys.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "sys_api.h"
#ifdef CONFIG_LOG_FWD
#include "log_forward.h"
#endif
#ifdef CONFIG_SUPPORT_ATCMD
#include "atcmd_service.h"
#endif

#ifdef CONFIG_NAN
extern u8 NAN_IPv6Parm[16];
#endif

struct whc_cmd_path_priv whc_cmdpath_data;
struct whc_dev_network_info whc_network_info[2] = {0};

#ifdef CONFIG_LOG_FWD
/* Reply to host that LOG_ENABLE/DISABLE has taken effect on the device;
 * host waits for this ACK before proceeding with bus suspend. */
static void whc_dev_log_fwd_send_ack(u8 op)
{
	u8 ack[6];
	*(u32 *)ack = WHC_WIFI_TEST;
	ack[4] = WHC_WIFI_TEST_LOG_ACK;
	ack[5] = op;
	whc_dev_api_send_to_host(ack, sizeof(ack), NULL, 0);
}
#endif

#ifdef CONFIG_SUPPORT_ATCMD
static void whc_at_output(char *buf, int len)
{
	u32 pkt_len = sizeof(u32) + 1 + (u32)len;
	u8 *pkt = rtos_mem_malloc(pkt_len);

	if (!pkt) {
		return;
	}
	*(u32 *)pkt = WHC_WIFI_TEST;
	pkt[4]      = WHC_WIFI_TEST_AT_RESP;
	memcpy(pkt + 5, buf, (u32)len);
	whc_dev_api_send_to_host(pkt, pkt_len, NULL, 0);
	rtos_mem_free(pkt);
}

void whc_at_resp_enable(void)
{
	out_buffer = whc_at_output;
}

void whc_at_resp_disable(void)
{
	out_buffer = NULL;
}
#else
void whc_at_resp_enable(void) {}
void whc_at_resp_disable(void) {}
#endif /* CONFIG_SUPPORT_ATCMD */

#ifdef CONFIG_RMESH_SOCKET_EN
__weak int whc_dev_wtn_socket_send(u8 *pframe, u32 len)
{
	u32 size;
	u32 *param;

	size = sizeof(u32) * 3 + len;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = WHC_RMESH_TEST;
	param[1] = WHC_RMESH_TEST_SOCK_SEND;
	param[2] = len;
	memcpy((void *)(param + 3), pframe, len);

	whc_dev_api_send_to_host((u8 *)param, size, NULL, 0);

	rtos_mem_free((u8 *)param);

	return RTK_SUCCESS;
}

__weak void whc_dev_wtn_socket_init(u8 enable, u8 rnat_ap_start)
{
	u32 size;
	u32 *param;

	size = sizeof(u32) * 4;
	param = (u32 *)rtos_mem_zmalloc(size);

	param[0] = WHC_RMESH_TEST;
	param[1] = WHC_RMESH_TEST_SOCK_INIT;
	param[2] = (u32)enable;
	param[3] = (u32)rnat_ap_start;

	whc_dev_api_send_to_host((u8 *)param, size, NULL, 0);

	rtos_mem_free((u8 *)param);
}
#endif

/**
  * @brief  Check if an IP address is already in the IP allocation table.
  * @param  gate: the third octet of the IP address (network segment).
  * @param  d: the fourth octet of the IP address (host ID).
  * @retval 0: IP is available; 1: IP is marked or network segment mismatch.
  */
int whc_dev_ip_in_table_indicate(u8 gate, u8 ip)
{
	(void)gate;
	(void)ip;

	/* TODO */

	/* return 1 to forward all pkt now.*/
	return 1;
}

/**
  * @brief  Get ip, gw, gw_mask information.
  * @param  type: info type, it can be WHC_WLAN_GET_IP/WHC_WLAN_GET_GW/WHC_WLAN_GET_GWMSK/WHC_WLAN_GET_HW_ADDR.
  * @param  input: ip address if needed.
  * @param  index: netif index
  * @retval the address where ip/gw/gw_mask info is stored.
  */
int whc_dev_get_lwip_info(u32 type, unsigned char *input, int index)
{
	(void) input;

	u8 *ptr = NULL;

	switch (type) {
	case WHC_WLAN_GET_IP:
		ptr = whc_network_info[index].ip;
		break;
	case WHC_WLAN_GET_GW:
		ptr = whc_network_info[index].gw;
		break;
	case WHC_WLAN_GET_GWMSK:
		ptr = whc_network_info[index].gw_mask;
		break;
	default:
		break;
	}

	return (int)ptr;
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
		if (!scanned_AP_list) {
			return -1;
		}
		result_buf = rtos_mem_zmalloc(buf_size);
		if (!result_buf) {
			rtos_mem_free(scanned_AP_list);
			return -1;
		}
		if (wifi_get_scan_records(&ap_num, scanned_AP_list) < 0) {
			RTK_LOGE(TAG_WLAN_INIC, "%s, ERROR: Get result failed\n", __func__);
			rtos_mem_free((void *)scanned_AP_list);
			rtos_mem_free(result_buf);
			return -1;
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

			whc_dev_api_send_to_host(result_buf, buf_size, NULL, 0);
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
		whc_dev_api_send_to_host(result_buf, buf_size, NULL, 0);

		rtos_mem_free(result_buf);
		rtos_mem_free(scanned_AP_list);
	}

	return 0;
}

void whc_dev_cmd_scan(void)
{
	struct rtw_scan_param *scan_param;
	int ret;

	scan_param = rtos_mem_zmalloc(sizeof(struct rtw_scan_param));

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

#ifdef CONFIG_MP_INCLUDED
void whc_dev_mp_cmd(char *cmd, int show_msg)
{
	/* response layout per fragment: WHC_WIFI_TEST(4B) + WHC_WIFI_TEST_MP(1B) + frag_idx(1B) + data(WHC_MP_FRAG_SIZE) */
	u8 *outbuf = rtos_mem_zmalloc(WHC_MP_FRAG_NUM * WHC_MP_FRAG_SIZE);
	u8 *resp = rtos_mem_malloc(6 + WHC_MP_FRAG_SIZE);
	u8 i;

	if (!outbuf || !resp) {
		RTK_LOGE(TAG_WLAN_INIC, "%s Malloc fail!\n", __func__);
		rtos_mem_free(outbuf);
		rtos_mem_free(resp);
		return;
	}

	wext_private_command(cmd, show_msg, (char *)outbuf);

	for (i = 0; i < WHC_MP_FRAG_NUM; i++) {
		*(u32 *)resp = WHC_WIFI_TEST;
		resp[4] = WHC_WIFI_TEST_MP;
		resp[5] = i; //frag_idx
		memcpy(resp + 6, outbuf + i * WHC_MP_FRAG_SIZE, WHC_MP_FRAG_SIZE);
		whc_dev_api_send_to_host(resp, 6 + WHC_MP_FRAG_SIZE, NULL, 0);
	}

	rtos_mem_free(outbuf);
	rtos_mem_free(resp);
}
#endif

/* here in sdio rx done callback */
__weak void whc_dev_cmd_rx_to_user(u8 *rxbuf)
{
	while (whc_cmdpath_data.whc_rx_msg) {
		/* waiting last msg done */
		rtos_time_delay_ms(1);
	}

	whc_cmdpath_data.whc_rx_msg = rxbuf;
	rtos_sema_give(whc_cmdpath_data.whc_user_rx_sema);
}

/* ---------- Handler implementations ---------- */
#ifndef CONFIG_MP_SHRINK

static void whc_dev_handle_get_mac_addr(u8 *ptr)
{
	struct rtw_mac dev_mac = {0};
	u8 idx = *(ptr + 1);
	u8 buf[WHC_WIFI_TEST_BUF_SIZE];
	u8 *dst;

	if (!wifi_is_running(idx)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
		return;
	}
	wifi_get_mac_address(idx, &dev_mac, 0);

	memset(buf, 0, sizeof(buf));
	dst = buf;
	*(u32 *)dst = WHC_WIFI_TEST;
	dst += 4;
	*dst = WHC_WIFI_TEST_GET_MAC_ADDR;
	dst += 1;
	*dst = idx;
	dst += 1;
	memcpy(dst, dev_mac.octet, 6);
	whc_dev_api_send_to_host(buf, WHC_WIFI_TEST_BUF_SIZE, NULL, 0);
}

static void whc_dev_handle_scan(u8 *ptr)
{
	(void)ptr;
	whc_dev_cmd_scan();
}

static void whc_dev_handle_connect(u8 *ptr)
{
	u8 len;
	char *password = NULL;
	int ret = RTK_FAIL;
	struct rtw_network_info *wifi = rtos_mem_zmalloc(sizeof(struct rtw_network_info));

	if (!wifi) {
		goto connect_fail;
	}
	ptr += 1;
	len = *ptr;
	wifi->ssid.len = len;
	memcpy((char *)wifi->ssid.val, ptr + 1, len);

	ptr += len + 1;
	len = *ptr;
	if (len > 0) {
		password = rtos_mem_zmalloc(129);
		if (!password) {
			goto connect_fail;
		}
		memcpy((char *)password, ptr + 1, len);
		wifi->password = (u8 *)password;
		wifi->password_len = len;
	}
	ret = wifi_connect(wifi, 1);

connect_fail:
	if (password) {
		rtos_mem_free(password);
	}
	if (wifi) {
		rtos_mem_free(wifi);
	}
#ifdef CONFIG_LWIP_LAYER
	if (ret == RTK_SUCCESS) {
		lwip_request_ip(NETIF_WLAN_STA_INDEX);
	} else {
		RTK_LOGE(TAG_WLAN_INIC, "connect fail !\n");
	}
#else
	(void) ret;
#endif
}

#ifdef CONFIG_LWIP_LAYER
static void whc_dev_handle_dhcp(u8 *ptr)
{
	(void)ptr;
	lwip_netif_set_link_up(NETIF_WLAN_STA_INDEX);
	lwip_request_ip(STA_WLAN_INDEX);
}

static void whc_dev_handle_get_ip(u8 *ptr)
{
	u8 idx = *(ptr + 1);
	u8 *ip;
	u8 buf[WHC_WIFI_TEST_BUF_SIZE];
	u8 *dst;

	if (!wifi_is_running(idx)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s, port %d is not running!\n", __func__, idx);
		return;
	}

	memset(buf, 0, sizeof(buf));
	ip = lwip_get_ip(idx);
	dst = buf;
	*(u32 *)dst = WHC_WIFI_TEST;
	dst += 4;
	*dst = WHC_WIFI_TEST_GET_IP;
	dst += 1;
	memcpy(dst, ip, 4);
	dst += 4;
	ip = lwip_get_gw(idx);
	memcpy(dst, ip, 4);
	dst += 4;
	whc_dev_api_send_to_host(buf, WHC_WIFI_TEST_BUF_SIZE, NULL, 0);
}
#endif /* CONFIG_LWIP_LAYER */

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
static void whc_dev_handle_set_ready(u8 *ptr)
{
	(void)ptr;
	whc_dev_api_set_host_state(WHC_HOST_READY);
}

static void whc_dev_handle_set_unready(u8 *ptr)
{
	(void)ptr;
	whc_dev_api_set_host_state(WHC_HOST_UNREADY);
}
#endif /* CONFIG_WHC_DEV_TCPIP_KEEPALIVE */

static void whc_dev_handle_tickps_cmd(u8 *ptr)
{
	whc_dev_tickps_cmd((struct whc_ps_cmd *)(ptr + 1));
}

static void whc_dev_handle_wifion(u8 *ptr)
{
	(void)ptr;
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	whc_dev_api_set_host_state(WHC_HOST_READY);
#endif
	wifi_on(RTW_MODE_STA);
}

static void whc_dev_handle_set_host_rtos(u8 *ptr)
{
	(void)ptr;
	wifi_user_config.cfg80211 = 0;
}

static void whc_dev_handle_ota(u8 *ptr)
{
	whc_dev_ota_process(ptr);
}

static void whc_dev_handle_network_info_update(u8 *ptr)
{
	u8 idx = *(ptr + 1);

	memcpy(whc_network_info[idx].ip, ptr + 2, 4);
	memcpy(whc_network_info[idx].gw, ptr + 6, 4);
	memcpy(whc_network_info[idx].gw_mask, ptr + 10, 4);

	if (idx == 0) {
		memcpy(whc_ipc_ip_addr, ptr + 2, 4);
	}
#ifdef CONFIG_NAN
	memcpy(NAN_IPv6Parm, ptr + 14, 16);
#endif
}

#endif /* !CONFIG_MP_SHRINK */

/* These subcmds are outside CONFIG_MP_SHRINK guard in v03_9 */
static void whc_dev_handle_clear_ota(u8 *ptr)
{
	sys_clear_ota_signature(*(ptr + 1));
}

static void whc_dev_handle_shell_cmd(u8 *ptr)
{
	u8 *cmdstr = ptr + 1;
	shell_cmd_inject((const char *)cmdstr, _strlen((const char *)cmdstr));
}

#ifdef CONFIG_LOG_FWD
static void whc_dev_handle_log_enable(u8 *ptr)
{
	(void)ptr;
	rtk_log_forward_enable();
	whc_at_resp_enable();
	whc_dev_log_fwd_send_ack(WHC_WIFI_TEST_LOG_ENABLE);
}

static void whc_dev_handle_log_disable(u8 *ptr)
{
	(void)ptr;
	rtk_log_forward_disable();
	whc_at_resp_disable();
	whc_dev_log_fwd_send_ack(WHC_WIFI_TEST_LOG_DISABLE);
}
#endif /* CONFIG_LOG_FWD */

#ifdef CONFIG_MP_INCLUDED
static void whc_dev_handle_mp(u8 *ptr)
{
	whc_dev_mp_cmd((char *)(ptr + 2), *(ptr + 1));
}
#endif /* CONFIG_MP_INCLUDED */


/* note: Should never use dev block send in handlers, may cause deadlock */
static const struct whc_dev_subcmd_entry whc_dev_subcmd_entries[] = {
#ifndef CONFIG_MP_SHRINK
	{WHC_WIFI_TEST_GET_MAC_ADDR,		whc_dev_handle_get_mac_addr},
	{WHC_WIFI_TEST_SCAN,			whc_dev_handle_scan},
	{WHC_WIFI_TEST_CONNECT,			whc_dev_handle_connect},
#ifdef CONFIG_LWIP_LAYER
	{WHC_WIFI_TEST_DHCP,			whc_dev_handle_dhcp},
	{WHC_WIFI_TEST_GET_IP,			whc_dev_handle_get_ip},
#endif
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	{WHC_WIFI_TEST_SET_READY,		whc_dev_handle_set_ready},
	{WHC_WIFI_TEST_SET_UNREADY,		whc_dev_handle_set_unready},
#endif
	{WHC_WIFI_TEST_SET_TICKPS_CMD,		whc_dev_handle_tickps_cmd},
	{WHC_WIFI_TEST_WIFION,			whc_dev_handle_wifion},
	{WHC_WIFI_TEST_SET_HOST_RTOS,		whc_dev_handle_set_host_rtos},
	{WHC_WIFI_TEST_OTA,			whc_dev_handle_ota},
	{WHC_WIFI_TEST_NETWORK_INFO_UPDATE,	whc_dev_handle_network_info_update},
#endif /* !CONFIG_MP_SHRINK */

	/* Outside CONFIG_MP_SHRINK */
	{WHC_WIFI_TEST_CLEAR_OTA,		whc_dev_handle_clear_ota},
	{WHC_WIFI_TEST_SHELL_CMD,		whc_dev_handle_shell_cmd},
#ifdef CONFIG_LOG_FWD
	{WHC_WIFI_TEST_LOG_ENABLE,		whc_dev_handle_log_enable},
	{WHC_WIFI_TEST_LOG_DISABLE,		whc_dev_handle_log_disable},
#endif
#ifdef CONFIG_MP_INCLUDED
	{WHC_WIFI_TEST_MP,			whc_dev_handle_mp},
#endif
};

__weak void whc_dev_cmd_rx_to_user_task(void)
{
	u8 *ptr;
	u32 event;
	u32 i;

	while (1) {
		rtos_sema_take(whc_cmdpath_data.whc_user_rx_sema, RTOS_MAX_TIMEOUT);

		if (whc_cmdpath_data.whc_rx_msg) {
			ptr = whc_cmdpath_data.whc_rx_msg + sizeof(struct whc_cmd_path_hdr);
			event = *(u32 *)(ptr);
			ptr += 4;

			if (event == WHC_WIFI_TEST) {
				u8 subcmd = *ptr;

				for (i = 0; i < sizeof(whc_dev_subcmd_entries) / sizeof(whc_dev_subcmd_entries[0]); i++) {
					if (whc_dev_subcmd_entries[i].subcmd == subcmd) {
						whc_dev_subcmd_entries[i].handler(ptr);
						break;
					}
				}
			}
			rtos_mem_free(whc_cmdpath_data.whc_rx_msg);
			whc_cmdpath_data.whc_rx_msg = NULL;
		}
	}
}

__weak void whc_dev_init_cmd_path(void)
{
	memset(&whc_cmdpath_data, 0, sizeof(struct whc_cmd_path_priv));

	/* initialize the semaphores */
	rtos_sema_create(&(whc_cmdpath_data.whc_user_rx_sema), 0, 0xFFFFFFFF);
	rtos_sema_create(&(whc_cmdpath_data.whc_user_blocksend_sema), 0, 0xFFFFFFFF);
	rtos_mutex_create(&whc_cmdpath_data.whc_user_blocksend_mutex);

	/* Initialize the event task */
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_dev_cmd_rx_to_user_task", (rtos_task_function_t)whc_dev_cmd_rx_to_user_task,
										NULL, WHC_WHC_CMD_USER_TASK_STACK_SIZE, CONFIG_WHC_WHC_CMD_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_dev_cmd_rx_to_user_task Err!!\n");
	}

}

