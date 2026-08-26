#include "whc_host.h"
#include "os_wrapper.h"
#include "atcmd_service.h"

struct whc_host_cmd_path_priv whc_host_cmdpath_data;

/**
* @brief  send buf to dev using CMD path
* @param  buf: data buf to be sent.
* @param  len: length of buf in bytes.
* @return none.
*/
void whc_host_send_cmd_data_to_dev(u8 *buf, u32 len)
{
	struct whc_cmd_path_hdr *hdr = NULL;
	u8 *txbuf = NULL;
	u32 txsize = len + sizeof(struct whc_cmd_path_hdr) + SIZE_TX_DESC;

	txbuf = rtos_mem_zmalloc(txsize);
	if (!txbuf) {
		RTK_LOGE(TAG_WLAN_INIC, "%s mem fail \r\n", __func__);
		return;
	}

	hdr = (struct whc_cmd_path_hdr *)(txbuf + SIZE_TX_DESC);
	hdr->event = WHC_WIFI_EVT_CMD;
	hdr->len = len;
	/* copy data */
	memcpy(txbuf + sizeof(struct whc_cmd_path_hdr) + SIZE_TX_DESC, buf, len);

	whc_host_send_data(txbuf, txsize, txbuf, 0);

#ifdef CONFIG_WHC_INTF_SDIO
	rtos_mem_free(txbuf);
#endif
}

void whc_host_scan_result(uint8_t *buf)
{
	uint32_t idx = buf[1];
	char *ap_info = (char *)(buf + 4);

	if (idx == 0) {
		return;
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02d %s", buf[1], ap_info);
}

/**
 * @brief  Deliver a CMD-path buffer to the dedicated cmd task.
 *         Ownership of the original buffer (including the RX descriptor
 *         and whc_cmd_path_hdr) is transferred to the cmd task, which
 *         will free it after processing.  This is a weak default; RTOS
 *         ports that define their own strong override bypass the task.
 * @param  buf:      pointer to the original buffer (including RX descriptor).
 * @param  buf_size: total size of buf in bytes.
 * @return none.
 */
void whc_host_deliver_rxbuf_to_user(u8 *buf, u32 buf_size)
{
	(void)buf_size;

	/* wait for the task to consume the previous message */
	while (whc_host_cmdpath_data.whc_rx_buf) {
		rtos_time_delay_ms(1);
	}

	whc_host_cmdpath_data.whc_rx_buf = buf;
	rtos_sema_give(whc_host_cmdpath_data.whc_user_rx_sema);
}

/**
 * @brief  Dedicated task that processes CMD-path messages received
 *         from the device (WHC_WIFI_TEST sub-commands).
 *         Never use blocking whc_dev_api_send_to_host calls here.
 */
static void whc_host_cmd_rx_to_user_task(void)
{
	u8 *ptr;
	u32 event;

	while (1) {
		rtos_sema_take(whc_host_cmdpath_data.whc_user_rx_sema, RTOS_MAX_TIMEOUT);

		if (whc_host_cmdpath_data.whc_rx_buf) {
			ptr = whc_host_cmdpath_data.whc_rx_buf + SIZE_RX_DESC + sizeof(struct whc_cmd_path_hdr);
			event = *(u32 *)ptr;

			if (event == WHC_WIFI_TEST) {
				ptr += 4;
				u8 id = *ptr;
				ptr += 1;
				u8 idx;
				u32 ipaddr, netmask, gw;

				switch (id) {
				case WHC_WIFI_TEST_GET_MAC_ADDR:
					idx = *ptr;
					lwip_wlan_set_netif_info(idx, NULL, ptr + 1);
					break;

				case WHC_WIFI_TEST_GET_IP:
					ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
					netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
					gw = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], 1);
					lwip_set_ip(NETIF_WLAN_STA_INDEX, ipaddr, netmask, gw);
					lwip_netif_set_link_up(NETIF_WLAN_STA_INDEX);
					lwip_netif_set_up(NETIF_WLAN_STA_INDEX);
					break;

				case WHC_WIFI_TEST_SCAN_RESULT:
					whc_host_scan_result(ptr - 1);
					break;

				default:
					break;
				}
			}

			rtos_mem_free(whc_host_cmdpath_data.whc_rx_buf);
			whc_host_cmdpath_data.whc_rx_buf = NULL;
		}
	}
}

void whc_host_cmd_path_init(void)
{
	memset(&whc_host_cmdpath_data, 0, sizeof(struct whc_host_cmd_path_priv));

	rtos_sema_create(&whc_host_cmdpath_data.whc_user_rx_sema, 0, 0xFFFFFFFF);

	if (RTK_SUCCESS != rtos_task_create(NULL,
										(const char *const)"whc_host_cmd_rx_to_user_task",
										(rtos_task_function_t)whc_host_cmd_rx_to_user_task,
										NULL, WHC_HOST_CMD_USER_TASK_STACK_SIZE,
										CONFIG_WHC_HOST_CMD_USER_TASK_PRIO)) {
		RTK_LOGE(TAG_WLAN_INIC, "Create whc_host_cmd_rx_to_user_task Err!!\n");
	}
}

void whc_host_get_mac_addr(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_MAC_ADDR;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_get_ip(uint8_t idx)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_GET_IP;
	ptr += 1;
	buf_len += 1;
	*ptr = idx;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_update_network_info(u8 wlan_idx)
{
	u8 buf[18 + 16]; /* WHC_WIFI_TEST(4) + subcmd(1) + wlan_idx(1) + ip(4) + gw(4) + gw_mask(4) + ipv6(16) */
	u8 *ptr = buf;

	if (wlan_idx > 1) {
		return;
	}

	memset(buf, 0, sizeof(buf));
	*(u32 *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	*ptr = WHC_WIFI_TEST_NETWORK_INFO_UPDATE;
	ptr += 1;
	*ptr = wlan_idx;
	ptr += 1;
	*(u32 *)ptr = *((u32 *)lwip_get_ip(wlan_idx));
	ptr += 4;
	*(u32 *)ptr = *((u32 *)lwip_get_gw(wlan_idx));
	ptr += 4;
	*(u32 *)ptr = *((u32 *)lwip_get_mask(wlan_idx));

	whc_host_send_cmd_data_to_dev(buf, sizeof(buf));
}

void whc_host_set_rdy(uint8_t state)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;
	*ptr = WHC_WIFI_TEST_SET_READY;
	ptr += 1;
	buf_len += 1;
	*ptr = state;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_set_wifi_on(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_WIFION;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_dhcp(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_DHCP;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_set_host(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;
	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SET_HOST_RTOS;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_wifi_scan(void)
{
	uint8_t buf[12] = {0};
	uint8_t *ptr = buf;
	uint32_t buf_len = 0;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;
	buf_len += 4;

	*ptr = WHC_WIFI_TEST_SCAN;
	ptr += 1;
	buf_len += 1;

	whc_host_send_cmd_data_to_dev(buf, buf_len);
}

void whc_host_wifi_connect(char *ssid, char *pwd)
{
	uint8_t *buf;
	uint8_t *ptr;
	uint32_t buf_len = 0, len = 0;

	buf = rtos_mem_zmalloc(128);
	ptr = buf;

	*(uint32_t *)ptr = WHC_WIFI_TEST;
	ptr += 4;

	*ptr = WHC_WIFI_TEST_CONNECT;
	ptr += 1;

	len = strlen(ssid);
	*ptr = len;
	ptr += 1;

	memcpy(ptr, ssid, len);
	ptr += len;

	if (pwd) {
		len = strlen(pwd);
		*ptr = len;
		ptr += 1;

		memcpy(ptr, pwd, len);
		ptr += len;

	} else {
		*ptr = 0;
		ptr += 1;
	}

	buf_len = ptr - buf;

	whc_host_send_cmd_data_to_dev(buf, buf_len);

	rtos_mem_free(buf);
}

u32 cmd_whc_test(u16 argc, u8  *argv[])
{
	UNUSED(argc);
	char *pwd = NULL;
	u8 state = 1;

	if (_strcmp((const char *)argv[0], "getmac") == 0) {
		whc_host_get_mac_addr(0);
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "getip") == 0) {
		whc_host_get_ip(0);
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "setrdy") == 0) {
		if (argc > 1) {
			if (_strcmp((const char *)argv[1], "unready") == 0) {
				state = 0;
			}
		}

		whc_host_set_rdy(state);
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "wifion") == 0) {
		whc_host_set_wifi_on();
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "dhcp") == 0) {
		whc_host_dhcp();
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "hostrtos") == 0) {
		whc_host_set_host();
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "scan") == 0) {
		whc_host_wifi_scan();
		goto exit;
	}

	if (_strcmp((const char *)argv[0], "connect") == 0) {
		if (argc > 2) {
			pwd = (char *)argv[2];
		}
		whc_host_wifi_connect((char *)argv[1], pwd);
		goto exit;
	}
exit:
	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   whc_test_cmd_table[] = {
	{"whc", cmd_whc_test},
};
