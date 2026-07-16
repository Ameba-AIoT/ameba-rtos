#include "rtw_whc_common.h"

extern void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
extern struct whc_sdio whc_sdio_priv;

int whc_host_sdio_send(int idx, void *pkt_addr, uint32_t len)
{
	struct whc_msg_info *msg;

	uint8_t *ptr = whc_sdio_priv.tx_buf;
	uint8_t *buf;
	uint32_t total_len = 0;

	if (len > MAX_SKB_BUF_SIZE_NORMAL) {
		printf("%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, len);
		return -1;
	}

	k_sem_take(&whc_sdio_priv.host_send, MUTEX_WAIT_TIMEOUT);

	buf = ptr;

	/* rsvd for txdesc */
	ptr += SIZE_TX_DESC;
	total_len += SIZE_TX_DESC;

	msg = (struct whc_msg_info *)(ptr);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = 0;
	msg->data_len = len;
	ptr += sizeof(struct whc_msg_info);
	total_len += sizeof(struct whc_msg_info);

	tx_read_pkt_ptr(pkt_addr, ptr, len);
	total_len += len;

	whc_host_sdio_send_data(buf, total_len, NULL);

	k_sem_give(&whc_sdio_priv.host_send);

	return 0;
}

