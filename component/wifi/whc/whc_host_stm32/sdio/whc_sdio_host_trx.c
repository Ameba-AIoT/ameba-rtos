#include "rtw_whc_common.h"
#include "lwip/pbuf.h"

extern struct whc_sdio whc_sdio_priv;
static int buf_counter = 0;

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

/* host rx */
void whc_host_sdio_recv_pkts(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	uint32_t len = msg_info->data_len;
	struct pbuf *temp_buf = 0;
	struct pbuf *p_buf;

	buf_counter = 0;
retry:
	p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL_RX);

	if (p_buf == NULL) {
		buf_counter++;
		vTaskDelay(1);
		goto retry;
		//just send rsp when pbuf alloc fail
		//return;
	}

	if (buf_counter == 1000) {
		printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
		buf_counter = 0;
	}

	/* copy data from skb(ipc data) to pbuf(ether net data) */
	temp_buf = p_buf;
	while (temp_buf) {
		/* If tot_len > PBUF_POOL_BUFSIZE_ALIGNED, the skb will be
		 * divided into several pbufs. Therefore, there is a while to
		 * use to assigne data to pbufs.
		 */

		memcpy(temp_buf->payload, data, temp_buf->len);
		data = data + temp_buf->len;
		temp_buf = temp_buf->next;
	}

	if (p_buf != NULL) {
		netif_adapter_wifi_recv_whc(msg_info->wlan_idx, p_buf);
	}

	whc_free(buf);
}