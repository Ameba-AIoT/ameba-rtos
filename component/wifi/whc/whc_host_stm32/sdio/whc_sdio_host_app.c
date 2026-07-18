#include "rtw_whc_common.h"

/* subtype */
#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3

#define WHC_WIFI_TEST_BUF_SIZE     16

__weak void whc_host_pkt_rx_to_user(u8 *pbuf)
{
	/* pbuf = buf + SIZE_RX_DESC, so pbuf[0..7] is whc_cmd_path_hdr */
	u8 *ptr = pbuf + sizeof(struct whc_cmd_path_hdr);
	uint32_t event = *(uint32_t *)ptr;
	u8 id;
	u32 ipaddr, netmask, gw;
	uint8_t idx;

	ptr += 4;

	if (event == WHC_WIFI_TEST) {
		id = *ptr;
		ptr += 1;
		switch (id) {
		case WHC_WIFI_TEST_GET_MAC_ADDR:
			idx = *ptr;
			ptr += 1;
			lwip_wlan_set_netif_info(STA_WLAN_INDEX, NULL, ptr);
			break;
		case WHC_WIFI_TEST_GET_IP:
			ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
			netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			gw = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], 1);
			lwip_set_ip(STA_WLAN_INDEX, ipaddr, netmask, gw);
			lwip_netif_set_link_up(0);
			break;

		default:
			break;
		}
	}
}

/**
* @brief  send buf to dev
* @param  buf: data buf to be sent.
* @param  len: length of buf in bytes.
* @return none.
*/
void whc_sdio_host_send_to_dev(u8 *buf, u32 len)
{
	struct whc_cmd_path_hdr *hdr;
	u8 *txbuf = NULL;
	u32 txsize = len + sizeof(struct whc_cmd_path_hdr) + SIZE_TX_DESC;

	txbuf = WHC_MALLOC(txsize);
	if (txbuf == NULL) {
		printf("%s mem fail \r\n", __func__);
		return;
	}

	hdr = (struct whc_cmd_path_hdr *)(txbuf + SIZE_TX_DESC);
	hdr->event = WHC_WIFI_EVT_CMD;
	hdr->len = len;
	memcpy(txbuf + SIZE_TX_DESC + sizeof(struct whc_cmd_path_hdr), buf, len);

	whc_host_sdio_send_data(txbuf, txsize, NULL);

	WHC_FREE(txbuf);
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

	whc_sdio_host_send_to_dev(buf, buf_len);
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

	whc_sdio_host_send_to_dev(buf, buf_len);
}

void whc_host_set_rdy(void)
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

	whc_sdio_host_send_to_dev(buf, buf_len);
}