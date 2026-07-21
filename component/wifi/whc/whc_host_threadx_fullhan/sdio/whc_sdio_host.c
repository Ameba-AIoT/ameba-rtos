#include "rtw_whc_common.h"

struct whc_sdio whc_sdio_priv = {0};
extern void rtw_sdio_interrupt_handler(void);
extern void sdio_polling_task(void *arg1, void *arg2, void *arg3);
extern rtos_mutex_t hw_lock;

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
			whc_free(buf);
			return;
		}
		printf("%s: Alloc skb rx buf Err conuter %d \n", __func__, buf_counter);
		rt_thread_mdelay(1);
		goto retry;
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
#endif


void whc_sdio_host_init_drv(void)
{

	rtos_sema_create(&(whc_sdio_priv.host_send), 1, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_irq), 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_send_block_sema), 0, SEMA_MAX_COUNT);
	rtos_sema_create(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	rtos_mutex_create(&(whc_sdio_priv.lock));

	/* should higher than polling, polling 7 */
	if (rtos_task_create(NULL, ((const char *)"whc_host_sdio_recv_data_process"), (rtos_task_function_t)whc_host_sdio_recv_data_process, NULL,
						 WIFI_STACK_SIZE_RX_REQ_TASK, 0 + 6) != 0) {
		printf("create whc_host_sdio_recv_data_process fail \n");
	}

#ifndef WHC_SDIO_INT_MODE
	if (rtos_task_create(NULL, ((const char *)"sdioPollingTask"), (rtos_task_function_t)sdio_polling_task, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
	}
#else
	if (rtos_task_create(NULL, ((const char *)"sdio_int_hal_task"), (rtos_task_function_t)rtw_sdio_interrupt_handler, NULL, SDIO_POLLING_STACK_SIZE, 7) != 0) {
		printf("%s(), fail to create sdioPollingTask \r\n", __func__);
	}
#endif

}

/**
 * @brief  to initialize the whc host.
 * @param  none.
 * @return none.
 */
void whc_host_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (priv->whc_host_init_done == 1) {
		return;
	}

	rtos_mutex_create(&hw_lock);
	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_sdio_host_init_drv();
	lwip_module_init();

	priv->whc_host_init_done = 1;

	/* tell dev host type rtos */
	whc_host_set_host();
	/* init wifi when sdio done */
	whc_host_wifi_on();
}

