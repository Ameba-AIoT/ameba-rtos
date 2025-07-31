#include "whc_host.h"

#define fix_tx_buf_num	2

u8 tx_buf[fix_tx_buf_num][4 + SIZE_TX_DESC + MAX_SKB_BUF_SIZE_NORMAL] = {0};
u8 used_buf_num = 0;
extern struct whc_sdio whc_sdio_priv;
extern struct event_priv_t event_priv;

/* host tx */
int whc_sdio_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
					   int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
	(void)raw_para;
	(void)is_special_pkt;
	struct eth_drv_sg *psg_list;
	int ret = RTK_SUCCESS, i = 0;
	int len_send = 0;
	struct whc_msg_info *msg;

	if (!event_priv.host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host trx err: wifi not init\n");
		return -RTK_ERR_WIFI_NOT_INIT;
	}

	rtos_mutex_take(whc_sdio_priv.host_send, 0xFFFFFFFF);

	uint8_t *ptr = &(tx_buf[used_buf_num][0]);
	uint8_t *buf;

	if (*ptr != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s fail buf busy !\n\r", __func__);
		return -RTK_ERR_WIFI_TX_BUF_FULL;
	}

	if (total_len > MAX_SKB_BUF_SIZE_NORMAL) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -RTK_ERR_BUFFER_OVERFLOW;
	}

	/* buf to sdio send */
	buf = ptr + 4;
	ptr +=  SIZE_TX_DESC + 4;
	msg = (struct whc_msg_info *)(ptr);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = 0;
	msg->data_len = 0;
	/* allocate the skb buffer */

	ptr += sizeof(struct whc_msg_info);
	len_send += SIZE_TX_DESC + sizeof(struct whc_msg_info);

	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(ptr, (void *)(psg_list->buf), psg_list->len);
		ptr += psg_list->len;
		msg->data_len += psg_list->len;
	}
	len_send += msg->data_len;

	rtw_sdio_send_data(buf, len_send, NULL);

	used_buf_num = (used_buf_num + 1) % fix_tx_buf_num;
	rtos_sema_give(whc_sdio_priv.host_send);

	return ret;
}

/**
* @brief  send buf to dev
* @param  buf: data buf to be sent.
* @param  len: real buf address, to be freed after sent.
* @return none.
*/
void whc_bridge_sdio_host_send_to_dev(u8 *buf, u32 len)
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
