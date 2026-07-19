#include "whc_host.h"
#include "os_wrapper.h"
#include "atcmd_service.h"

/**
* @brief  send buf to dev using CMD path
* @param  buf: data buf to be sent.
* @param  len: length of buf in bytes.
* @return none.
*/
void whc_host_api_send_to_dev(u8 *buf, u32 len)
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

__weak void whc_host_pkt_rx_to_user(u8 *payload, u32 len)
{
	(void)len;
	u32 event = *(u32 *)payload;
	u8 *ptr = payload;
	u8 id;
	u32 ipaddr, netmask, gw;
	uint8_t idx = 0;
	if (event == WHC_WIFI_TEST) {
		ptr += 4;
		id = *ptr;
		ptr += 1;
		switch (id) {
		case WHC_WIFI_TEST_GET_MAC_ADDR:
			idx = *ptr;
			ptr += 1;
			lwip_wlan_set_netif_info(idx, NULL, ptr);
			break;

		case WHC_WIFI_TEST_GET_IP:
			ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
			netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			gw = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], 1);
			lwip_set_ip(NETIF_WLAN_STA_INDEX, ipaddr, netmask, gw);
			lwip_netif_set_link_up(NETIF_WLAN_STA_INDEX);
			break;

		case WHC_WIFI_TEST_SCAN_RESULT:
			whc_host_scan_result(payload + 4);
			break;

		default:
			break;
		}


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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);
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

	whc_host_api_send_to_dev(buf, buf_len);

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
		lwip_netif_set_up(NETIF_WLAN_STA_INDEX);
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