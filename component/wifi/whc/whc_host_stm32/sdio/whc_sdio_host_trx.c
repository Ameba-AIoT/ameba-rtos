#include "rtw_whc_common.h"

extern struct whc_sdio whc_sdio_priv;

/* host tx */
int whc_host_sdio_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
					   int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
	(void)raw_para;
	(void)is_special_pkt;
	struct eth_drv_sg *psg_list;
	int ret = 0, i = 0;
	int len_send = 0;
	struct whc_msg_info *msg;
	uint8_t *ptr = whc_sdio_priv.tx_buf;
	uint8_t *buf;

	if (!whc_sdio_priv.whc_host_init_done) {
		printf("Host trx err: wifi not init\n");
		return -1;
	}

	if (total_len > MAX_SKB_BUF_SIZE_NORMAL) {
		printf("%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -1;
	}

	rtos_mutex_take(whc_sdio_priv.host_send, 0xFFFFFFFF);

	/* buf to sdio send */
	buf = ptr;
	ptr +=  SIZE_TX_DESC;
	msg = (struct whc_msg_info *)(ptr);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = 0;
	msg->data_len = 0;

	ptr += sizeof(struct whc_msg_info);
	len_send += SIZE_TX_DESC + sizeof(struct whc_msg_info);

	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(ptr, (void *)(psg_list->buf), psg_list->len);
		ptr += psg_list->len;
		msg->data_len += psg_list->len;
	}
	len_send += msg->data_len;

	whc_host_sdio_send_data(buf, len_send, NULL);
	rtos_mutex_give(whc_sdio_priv.host_send);

	return ret;
}

