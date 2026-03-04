#include "rtw_whc_common.h"

#define fix_tx_buf_num	1
#define MAX_SKB_BUF_SIZE_NORMAL	1664

#ifdef todo
#endif //use dynamic buf later, malloc when init host.
/* one buf for fullhan, host will not exit send task until data send done */
__attribute__((aligned(32)))  uint8_t tx_buf[fix_tx_buf_num][4 + SIZE_TX_DESC + MAX_SKB_BUF_SIZE_NORMAL] = {0};
uint8_t used_buf_num = 0;
extern struct whc_sdio whc_sdio_priv;
#ifdef CONFIG_WHC_WIFI_API_PATH
extern struct event_priv_t event_priv;
#endif
extern int whc_host_init_done;

/* host tx */
int whc_host_sdio_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
					   int total_len, uint8_t is_special_pkt)
{
	(void)is_special_pkt;
	struct eth_drv_sg *psg_list;
	int ret = 0, i = 0;
	int len_send = 0;
	struct whc_msg_info *msg;

	if (!whc_host_init_done) {
		printf("Host trx err: wifi not init\n");
		return -1;
	}

	rtos_sema_take(whc_sdio_priv.host_send, 0xFFFFFFFF);

	uint8_t *ptr = &(tx_buf[used_buf_num][0]);
	uint8_t *buf;

	if (total_len > MAX_SKB_BUF_SIZE_NORMAL) {
		printf("%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -1;
	}

	/* buf to sdio send */
	buf = ptr;
	ptr +=  SIZE_TX_DESC;
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

	rtos_sema_give(whc_sdio_priv.host_send);

	return ret;
}

