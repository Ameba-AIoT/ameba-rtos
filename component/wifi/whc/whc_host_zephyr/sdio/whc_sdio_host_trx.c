#include "rtw_whc_common.h"

#define fix_tx_buf_num	1
#ifdef ZEPHYR_TODO
#endif
#define MAX_SKB_BUF_SIZE_NORMAL	1664
uint8_t tx_buf[fix_tx_buf_num][4 + SIZE_TX_DESC + MAX_SKB_BUF_SIZE_NORMAL] = {0};
uint8_t used_buf_num = 0;

extern void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
extern struct inic_sdio inic_sdio_priv;

int whc_bridge_host_send(int idx, void *pkt_addr, uint32_t len)
{
	struct whc_msg_info *msg;

	uint8_t *ptr = &(tx_buf[used_buf_num][0]);
	uint8_t *buf;
	uint32_t total_len = 0;

	k_sem_take(&inic_sdio_priv.host_send, MUTEX_WAIT_TIMEOUT);

	if (*ptr != 0) {
		printf("%s fail buf busy !\n\r", __func__);
		return -1;
	}

	if (len > 1600) {
		printf("%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, len);
		return -1;
	}

	ptr += 4;
	/* force to 4 bytes aligned */
	//pad_len = 0;
	buf = ptr;

	/* rsvd for txdesc */
	ptr += SIZE_TX_DESC;
	total_len += SIZE_TX_DESC;

	msg = (struct whc_msg_info *)(ptr);
	msg->event = INIC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = 0;
	msg->data_len = len;
	ptr += sizeof(struct whc_msg_info);
	total_len += sizeof(struct whc_msg_info);

	tx_read_pkt_ptr(pkt_addr, ptr, len);
	total_len += len;

	whc_bridge_sdio_host_send_data(buf, total_len, NULL);

	used_buf_num = (used_buf_num + 1) % fix_tx_buf_num;

	k_sem_give(&inic_sdio_priv.host_send);

	return 0;
}

