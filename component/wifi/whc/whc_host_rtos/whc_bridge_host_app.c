#include "whc_host.h"
#include "os_wrapper.h"
#include "atcmd_service.h"

__weak void whc_bridge_host_pkt_rx_to_user(u8 *payload, u32 len)
{
	(void)len;
	u32 event = *(u32 *)payload;
	u8 *ptr = payload;
	u8 id;
	u32 ipaddr, netmask, gw;

	if (event == WHC_WIFI_TEST) {
		ptr += 4;
		id = *ptr;
		ptr += 1;
		switch (id) {
		case WHC_WIFI_TEST_GET_MAC_ADDR:
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
void whc_bridge_spi_host_send_to_dev(u8 *buf, u32 len)
{
	struct whc_bridge_hdr *hdr = NULL;
	u8 *txbuf = NULL;
	u32 txsize = len + sizeof(struct whc_bridge_hdr);
	struct whc_txbuf_info_t *inic_tx;

	/* construct struct whc_buf_info & whc_buf_info_t */
	inic_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	txbuf = rtos_mem_zmalloc(txsize);

	if ((txbuf == NULL) || (inic_tx == NULL)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s mem fail \r\n", __func__);
		if (inic_tx) {
			rtos_mem_free(inic_tx);
		}

		if (txbuf) {
			rtos_mem_free(txbuf);
		}
		return;
	}

	hdr = (struct whc_bridge_hdr *)txbuf;
	hdr->event = WHC_WIFI_EVT_BRIDGE;
	hdr->len = len;
	/* copy data */
	memcpy(txbuf + sizeof(struct whc_bridge_hdr), buf, len);

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)txbuf;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = txsize;

	inic_tx->ptr = txbuf;
	inic_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_spi_host_send_data(&inic_tx->txbuf_info);
}


void whc_bridge_host_get_mac_addr(uint8_t idx)
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

	whc_bridge_spi_host_send_to_dev(buf, buf_len);
}

void whc_bridge_host_get_ip(uint8_t idx)
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

	whc_bridge_spi_host_send_to_dev(buf, buf_len);
}

void whc_bridge_host_set_rdy(void)
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

	whc_bridge_spi_host_send_to_dev(buf, buf_len);
}

u32 cmd_bridge_test(u16 argc, u8  *argv[])
{
	UNUSED(argc);

	if (_strcmp((const char *)argv[0], "getmac") == 0) {
		whc_bridge_host_get_mac_addr(0);
	}

	if (_strcmp((const char *)argv[0], "getip") == 0) {
		whc_bridge_host_get_ip(0);
	}

	if (_strcmp((const char *)argv[0], "setrdy") == 0) {
		LwIP_netif_set_up(0);
		whc_bridge_host_set_rdy();
	}
	return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   spi_bridge_test_cmd_table[] = {
	{
		(const u8 *)"BRIDGE", 8, cmd_bridge_test, (const u8 *)"\tBRIDGE \n"
		"\t\t BRIDGE Test \n"
	},
};