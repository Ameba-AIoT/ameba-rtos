#include "inic_dev.h"
#include "usbd.h"
#include "usbd_inic.h"

//temporarily add here
#if defined (CONFIG_AMEBADPLUS)
#define WIFI_INIC_USB_BULKIN_EP  USBD_INIC_WIFI_EP3_BULK_IN
#define WIFI_INIC_USB_BULKOUT_1  USBD_INIC_WIFI_EP2_BULK_OUT
#define WIFI_INIC_USB_BULKOUT_2  USBD_INIC_WIFI_EP4_BULK_OUT
#define WIFI_INIC_USB_BULKOUT_3  0xFF /*invalid*/
#define WIFI_INIC_USB_BULKOUT_EP_NUM 2
#define WIFI_INIC_USB_SPEED      USB_SPEED_FULL
#elif defined(CONFIG_AMEBAGREEN2)
#define WIFI_INIC_USB_BULKIN_EP  USBD_INIC_WIFI_EP4_BULK_IN
#define WIFI_INIC_USB_BULKOUT_1  USBD_INIC_WIFI_EP5_BULK_OUT
#define WIFI_INIC_USB_BULKOUT_2  USBD_INIC_WIFI_EP6_BULK_OUT
#define WIFI_INIC_USB_BULKOUT_3  USBD_INIC_WIFI_EP7_BULK_OUT
#define WIFI_INIC_USB_BULKOUT_EP_NUM 3
#define WIFI_INIC_USB_SPEED      USB_SPEED_HIGH
#endif

struct inic_usb_priv_t inic_usb_priv = {0};

u8 wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;

static usbd_config_t inic_usb_wifi_cfg = {
	.speed = WIFI_INIC_USB_SPEED,
	.dma_enable = 1U,
	.isr_priority = 1,
};

/* host->device */
static int inic_usb_rx_done_cb(usbd_inic_ep_t *ep, u16 len)
{
	u8 *buf = NULL;
	struct sk_buff *new_skb = NULL;
	struct sk_buff *skb_rcv = NULL;
	struct inic_msg_info *msg_info;
	u32 event = 0;

	if (ep->addr == WIFI_INIC_USB_BULKOUT_1 || ep->addr == WIFI_INIC_USB_BULKOUT_2 || ep->addr == WIFI_INIC_USB_BULKOUT_3)  {
		event = *(u32 *)ep->buf;

		if (event != INIC_WIFI_EVT_XIMT_PKTS) {
			buf = rtos_mem_zmalloc(len);

			if (buf == NULL) {
				RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
				return FAIL;
			}

			memcpy(buf, ep->buf, len);
			inic_dev_event_int_hdl(buf, NULL);
			/* buf will be freed later*/
		} else {
			if ((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 2) {
				/*todo: if return fail, pkt will be droped??*/
				//RTK_LOGS(NOTAG, RTK_LOG_ERROR, "SKB FAIL\n");
				return FAIL;
			}

			msg_info = (struct inic_msg_info *) ep->buf;
			skb_rcv = (struct sk_buff *)ep->userdata;
			skb_reserve(skb_rcv, sizeof(struct inic_msg_info));

			skb_put(skb_rcv, msg_info->data_len);
			new_skb = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
			if (!new_skb) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rxdone cb alloc fail\n");
				return FAIL;
			}

			skb_rcv->dev = (void *)msg_info->wlan_idx;
			inic_dev_event_int_hdl((u8 *)msg_info, skb_rcv);
			ep->buf = new_skb->data;
			ep->userdata = new_skb;

		}
	}

	return HAL_OK;
}

static void inic_usb_tx_done_cb(usbd_inic_ep_t *ep, u8 status)
{
	UNUSED(status);
	struct inic_txbuf_info_t *inic_tx;

	switch (ep->addr) {
	case WIFI_INIC_USB_BULKIN_EP:

		inic_tx = (struct inic_txbuf_info_t *)ep->userdata;

		if (inic_tx->is_skb) {
			dev_kfree_skb_any((struct sk_buff *) inic_tx->ptr);
		} else {
			rtos_mem_free((u8 *)inic_tx->ptr);
		}

		rtos_mem_free((u8 *)inic_tx);

		rtos_sema_give(inic_usb_priv.usb_tx_sema);
		break;
	default:
		break;
	}
}

static void inic_usb_suspend_cb(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "USB SUSPEND\n");
	wifi_inic_usb_status = WIFI_INIC_USB_STATUS_SUSPEND;
}

static void inic_usb_resume_cb(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "USB RESUME\n");
	wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;
}

void inic_usb_status_changed_cb(u8 status)
{
	if (status == USBD_ATTACH_STATUS_ATTACHED) {
		wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;
	}
}

int inic_usb_rxbuf_config(void)
{
	struct sk_buff *skb_new = NULL;

	skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
	usbd_inic_receive_data(WIFI_INIC_USB_BULKOUT_1, skb_new->data, USB_BUFSZ, skb_new);
	skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
	usbd_inic_receive_data(WIFI_INIC_USB_BULKOUT_2, skb_new->data, USB_BUFSZ, skb_new);
	if (WIFI_INIC_USB_BULKOUT_EP_NUM == 3) {
		skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
		usbd_inic_receive_data(WIFI_INIC_USB_BULKOUT_3, skb_new->data, USB_BUFSZ, skb_new);
	}

	return 0;
}

static usbd_inic_cb_t inic_usb_wifi_cb = {
	.set_config = inic_usb_rxbuf_config,
	.received = inic_usb_rx_done_cb,
	.transmitted = inic_usb_tx_done_cb,
	.status_changed = inic_usb_status_changed_cb,
	.suspend = inic_usb_suspend_cb,
	.resume = inic_usb_resume_cb,
};


static void inic_usb_init(void)
{
	int ret;

	ret = usbd_init(&inic_usb_wifi_cfg);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USBdevice init fail!\n");
	}

	ret = usbd_inic_init(&inic_usb_wifi_cb);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USB inic init fail!\n");
	}

	rtos_mutex_create_static(&inic_usb_priv.tx_lock);
	rtos_sema_create_static(&inic_usb_priv.usb_tx_sema, 0, 0xFFFFFFFF);
	rtos_sema_give(inic_usb_priv.usb_tx_sema);
	RTK_LOGI(TAG_WLAN_INIC, "USB device init done!\n");
}


/**
 * @brief  to haddle the inic message interrupt. If the message queue is
 * 	initialized, it will enqueue the message and wake up the message
 * 	task to haddle the message. If last send message cannot be done, I will
 * 	set pending for next sending message.
 * @param  rxbuf: rx data.
 * @return none.
 */
void inic_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct inic_api_info *ret_msg;

	switch (event) {
	case INIC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case INIC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct inic_api_info *)rxbuf;
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_WARN, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
	case INIC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (inic_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == FAIL) {
			break;
		}
		/* wakeup task */
		rtos_sema_give(dev_xmit_priv.xmit_sema);

		break;
	default:
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Event(%ld) unknown!\n", event);
		break;
	}

}


/*wifi recv pkt, send to host*/
void inic_dev_send(struct inic_buf_info *pbuf)
{
	int i = 0;
	struct inic_txbuf_info_t *inic_tx;

	if (wifi_inic_usb_status == WIFI_INIC_USB_STATUS_SUSPEND) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "DEV Send when suspend\n");
		usbd_wake_host();
		for (i = 0; i < WIFI_INIC_USB_WAKE_HOST_MAX_WAIT_CNT; i++) {
			rtos_time_delay_ms(WIFI_INIC_USB_WAKE_HOST_UNIT);
			if (wifi_inic_usb_status == WIFI_INIC_USB_STATUS_ACTIVE) {
				break;
			}
		}
		//rtos_time_delay_ms(500); /*todo: need confirm host already resumed before send data to host*/
	}

	rtos_mutex_take(inic_usb_priv.tx_lock, MUTEX_WAIT_TIMEOUT);
	inic_tx = container_of(pbuf, struct inic_txbuf_info_t, txbuf_info);

	rtos_sema_take(inic_usb_priv.usb_tx_sema, MUTEX_WAIT_TIMEOUT);

	usbd_inic_transmit_data((u8)WIFI_INIC_USB_BULKIN_EP, (u8 *)pbuf->buf_addr, (u16)pbuf->buf_size, (void *)inic_tx);

	rtos_mutex_give(inic_usb_priv.tx_lock);

	return;
}

u8 inic_dev_tx_path_avail(void)
{
	u32 delay_cnt = 0;
	u8 ret = FALSE;

	while (delay_cnt++ < 100) {
		/* wait for skb allocatable */
		if ((skbpriv.skb_buff_num - skbpriv.skb_buff_used) > 3) {
			ret = TRUE;
			break;
		} else {
			rtos_time_delay_ms(1);
		}
	}

	return ret;
}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void inic_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	inic_usb_init();

	/* initialize the dev priv */
	inic_dev_init_priv();

	inic_api_init_dev();
}
