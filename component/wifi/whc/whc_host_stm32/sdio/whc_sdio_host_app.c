#include "rtw_whc_common.h"

#define WHC_WIFI_TEST 0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3

#define WHC_WIFI_TEST_BUF_SIZE     16

struct event_priv_t event_priv;

__weak void whc_host_pkt_rx_to_user(u8 *pbuf)
{
	int counter = 0;
	uint32_t event = *(uint32_t *)(pbuf);
	u8 *ptr = pbuf;
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
			LwIP_wlan_set_netif_info(STA_WLAN_INDEX, NULL, ptr);
			break;
		case WHC_WIFI_TEST_GET_IP:
			ipaddr = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], ptr[3]);
			netmask = CONCAT_TO_UINT32(NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			gw = CONCAT_TO_UINT32(ptr[0], ptr[1], ptr[2], 1);
			LwIP_SetIP(STA_WLAN_INDEX, ipaddr, netmask, gw);
			LwIP_netif_set_link_up(0);
			break;

		default:
			break;
		}
	}
}

/**
* @brief  send buf to dev
* @param  buf: data buf to be sent.
* @param  len: real buf address, to be freed after sent.
* @return none.
*/
void whc_sdio_host_send_to_dev(u8 *buf, u32 len)
{
	u8 *txbuf = NULL;
	u32 txsize = len + SIZE_TX_DESC;

	/* construct struct whc_buf_info & whc_buf_info_t */
	txbuf = rtos_mem_zmalloc(txsize);

	if (txbuf == NULL) {
		printf("%s mem fail \r\n", __func__);
		if (txbuf) {
			rtos_mem_free(txbuf);
		}
		return;
	}

	/* copy data */
	memcpy(txbuf + SIZE_TX_DESC, buf, len);

	/* send ret_msg + ret_val(buf, len) */
	rtw_sdio_send_data(txbuf, txsize, NULL);
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