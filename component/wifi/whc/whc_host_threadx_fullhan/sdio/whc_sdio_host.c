#include "rtw_whc_common.h"

struct whc_sdio whc_sdio_priv = {0};
int whc_host_init_done = 0;
#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)
#define SDIO_POLLING_STACK_SIZE 1024
extern void rtw_sdio_interrupt_handler(void);
extern void sdio_polling_task(void *arg1, void *arg2, void *arg3);

void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s", info);
	for (uint32_t i = 0; i < len; i++) {
		printf("%s0x%02X%s", i % 16 == 0 ? "\n     " : ",", buf[i], i == len - 1 ? "\n" : "");
	}
}

/* compile in fullhan sdk now */
#ifdef TODO
void whc_host_sdio_recv_pkts(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	//uint32_t len = msg_info->data_len;
	struct pbuf *temp_buf = 0;
	struct pbuf *p_buf;

retry:
	p_buf = pbuf_alloc(PBUF_RAW, msg_info->data_len, PBUF_POOL);

	if (p_buf == NULL) {
		buf_counter++;
		if (buf_counter >= 1000) {
			printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
			buf_counter = 0;
			WHC_FREE(buf);
			return;
		}
		printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
		rt_thread_mdelay(1);
		goto retry;
	}

	/* cpoy data from skb(ipc data) to pbuf(ether net data) */
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

	WHC_FREE(buf);
}
#endif


void whc_sdio_host_init_drv(void)
{
	int ret;
	ret = rtos_sema_create(&whc_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&(whc_sdio_priv.host_irq), 0, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&whc_sdio_priv.host_send_block_sema, 0, SEMA_MAX_COUNT);
	ret += rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);

	/* should higher than polling, polling 7 */
	if (rtos_task_create(NULL, ((const char *)"whc_host_sdio_recv_data_process"), (rtos_task_function_t)whc_host_sdio_recv_data_process, NULL,
						 WIFI_STACK_SIZE_RX_REQ_TASK, 0 + 6) != 0) {
		printf("create whc_host_sdio_recv_data_process fail \n");
	}

	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), (rtos_task_function_t)sdio_polling_task, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
	}

}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_host_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (whc_host_init_done == 1) {
		return;
	}

	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();

	lwip_module_init();

	whc_host_init_done = 1;

	/* tell dev host type rtos */
	whc_host_set_host();
	/* init wifi when sdio done */
	whc_host_wifi_on();
}

void rtw_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb)
{
	uint32_t polling_num = 0;
	struct whc_sdio *priv = &whc_sdio_priv;
	struct INIC_TX_DESC *ptxdesc;

	(void) polling_num;
	(void) pskb;

	/* wakeup device if it's in power save mode before send msg */
	if (priv->dev_state == PWR_STATE_SLEEP) {
		printf("%s: wakeup device", __func__);
		//TODO wake
	}

	WHC_MUTEX_TAKE(priv->lock, MUTEX_WAIT_TIMEOUT);
	// check if hardware tx fifo page is enough
	while (priv->SdioTxBDFreeNum < 1) {
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		//TODO
		if (polling_num++ > 0) {
			priv->tx_avail_int_triggered = 1;
			if (rtos_sema_take(priv->txbd_wq, 1000) != 0) {
				goto exit;
			}
		}
		rtw_sdio_query_txbd_status(priv);
#else
		rtw_sdio_query_txbd_status(priv);
		polling_num++;
		if ((polling_num % 60) == 0) {
			WHC_MSLEEP(1);
		}
		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		printf("%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, (int)len);
		goto exit;
	}

	ptxdesc = (struct INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	//dump_buf("sdio send", buf + SIZE_TX_DESC, 32);
	//printf("send counter %d size %d buf %x\r\n", ++counter, len - SIZE_TX_DESC - sizeof(struct whc_msg_info), buf);

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}

	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;
exit:
	WHC_MUTEX_GIVE(priv->lock);

	return;
}

