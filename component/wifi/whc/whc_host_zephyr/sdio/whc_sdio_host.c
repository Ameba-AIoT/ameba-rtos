#include "rtw_whc_common.h"
#include "whc_sdio_host_app.h"

struct whc_sdio whc_sdio_priv = {0};

#define whc_host_rx_req_task_prio	2

#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)

K_THREAD_STACK_DEFINE(whc_sdio_rx_req_stack, WIFI_STACK_SIZE_RX_REQ_TASK);

static struct k_thread ameba_host_rx_req_thread;

void (*tx_read_pkt_ptr)(void *pkt_addr, void *data, size_t length);
int (*rx_callback_ptr)(uint8_t idx, void *buffer, uint16_t len);

static void whc_host_sdio_init_drv(void)
{
	k_sem_init(&whc_sdio_priv.host_send, 1, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.host_recv_wake), 0, SEMA_MAX_COUNT);
	k_sem_init(&(whc_sdio_priv.host_recv_done), 1, 1);
	k_sem_init(&(whc_sdio_priv.txbd_wq), 0, SEMA_MAX_COUNT);
	k_mutex_init(&whc_sdio_priv.lock);

	/* init host<->dev synchronous request/return */
	k_sem_init(&(whc_sdio_priv.api_ret_sema), 0, SEMA_MAX_COUNT);
	k_mutex_init(&(whc_sdio_priv.send_mutex));

	k_thread_create(&ameba_host_rx_req_thread, whc_sdio_rx_req_stack,
					WIFI_STACK_SIZE_RX_REQ_TASK,
					(k_thread_entry_t)whc_host_sdio_recv_data_process, NULL, NULL, NULL,
					whc_host_rx_req_task_prio, K_USER,
					K_NO_WAIT);
}

/**
 * @brief  to initialize the device.
 * @param  none.
 * @return none.
 */
void whc_host_sdio_init(void)
{
	struct whc_sdio *priv = &whc_sdio_priv;

	if (rtw_sdio_init(priv) != TRUE) {
		printf("%s: initialize SDIO Failed!\n", __FUNCTION__);
		return;
	}

	/* init sdio */
	whc_host_sdio_init_drv();
}

void whc_host_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb)
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
		priv->tx_avail_int_triggered = 1;
		if (rtos_sema_take(&(priv->txbd_wq), 1000) != 0) {
			LOG_INF("take sem fail %s %d\r\n", __func__, __LINE__);
		} else {
			priv->tx_avail_int_triggered = 0;
		}
		rtw_sdio_query_txbd_status(priv);
#else
		polling_num++;
		if ((polling_num % 60) == 0) {
			WHC_MSLEEP(1);
		}
		// Total number of TXBD is NOT available, so update current TXBD status
		rtw_sdio_query_txbd_status(priv);
#endif
	}

	if (len > priv->SdioTxMaxSZ) {
		printf("%s: PKT SIZE ERROR, total size: %d\n", __FUNCTION__, len);
		goto exit;
	}

	ptxdesc = (struct INIC_TX_DESC *)buf;
	ptxdesc->txpktsize = len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = TX_PACKET_802_3;
	ptxdesc->bus_agg_num = 1;

	rtw_write_port(priv, SDIO_TX_FIFO_DOMAIN_ID, len, buf);

	if (priv->SdioTxBDFreeNum > 0) {
		priv->SdioTxBDFreeNum -= 1;
	}
	priv->txbd_wptr = (priv->txbd_wptr + 1) % priv->txbd_size;

exit:
	WHC_MUTEX_GIVE(priv->lock);

	return;
}

void whc_host_sdio_recv_pkts(uint8_t *buf)
{
	uint8_t *ptr = buf + SIZE_RX_DESC;
	struct whc_msg_info *msg_info = (struct whc_msg_info *)ptr;
	char *data = (char *)(ptr + sizeof(struct whc_msg_info) + msg_info->pad_len);
	uint32_t len = msg_info->data_len;

	// direct up to zephyr network, free after done
	if (rx_callback_ptr) {
		rx_callback_ptr(msg_info->wlan_idx, data, len);
	}

	WHC_FREE(buf);
}

