#include "usbd.h"
#include "usbd_inic.h"
#include "whc_dev.h"

struct whc_usb_priv_t whc_usb_priv = {0};

u8 wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;

#ifndef CONFIG_WHC_BRIDGE
void rtw_pending_q_resume(void);
#endif

static usbd_config_t whc_usb_wifi_cfg = {
	.speed = WIFI_INIC_USB_SPEED,
	.isr_priority = 4,
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 292U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 128U, },
#endif
};

/* host->device */
static int whc_usb_dev_rx_done_cb(usbd_inic_ep_t *out_ep, u16 len)
{
	if (out_ep->ep.addr == WIFI_INIC_USB_BULKOUT_1 || out_ep->ep.addr == WIFI_INIC_USB_BULKOUT_2
		|| out_ep->ep.addr == WIFI_INIC_USB_BULKOUT_3) {
		if ((whc_usb_priv.irq_info.rx_ep[whc_usb_priv.irq_info.intr_widx] == NULL) &&
			(whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.intr_widx] == 0)) {
			whc_usb_priv.irq_info.rx_ep[whc_usb_priv.irq_info.intr_widx] = (u8 *)out_ep;
			whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.intr_widx] = len;
			whc_usb_priv.irq_info.intr_widx = (whc_usb_priv.irq_info.intr_widx + 1) % WIFI_INIC_USB_BULKOUT_EP_NUM;
			rtos_sema_give(whc_usb_priv.usb_irq_sema);
		} else {
			usbd_inic_receive_data(out_ep->ep.addr, out_ep->ep.xfer_buf, out_ep->ep.xfer_len, out_ep->userdata);
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "rx cb list full, drop this message!\n");
		}
	}
	return RTK_SUCCESS;
}

static void whc_usb_dev_irq_task(void)
{
	u8 *buf = NULL;
	struct sk_buff *new_skb = NULL;
	struct sk_buff *skb_rcv = NULL;
	struct whc_msg_info *msg_info;
	usbd_inic_ep_t *inic_ep = NULL;
	struct whc_txbuf_info_t *whc_txbuf;
	u32 event = 0;
	u16 len = 0;
	do {
		rtos_sema_take(whc_usb_priv.usb_irq_sema, MUTEX_WAIT_TIMEOUT);
		// tx callback
		if (whc_usb_priv.irq_info.whc_txbuf != NULL) {
			whc_txbuf = (struct whc_txbuf_info_t *)whc_usb_priv.irq_info.whc_txbuf;

			if (whc_txbuf->is_skb) {
				dev_kfree_skb_any((struct sk_buff *) whc_txbuf->ptr);
			} else {
				rtos_mem_free((u8 *)whc_txbuf->ptr);
			}

			rtos_mem_free((u8 *)whc_txbuf);
			whc_usb_priv.irq_info.whc_txbuf = NULL;
			rtos_sema_give(whc_usb_priv.usb_tx_sema);
		}
		//rx callback
		while ((whc_usb_priv.irq_info.rx_ep[whc_usb_priv.irq_info.task_ridx] != NULL) &&
			   (whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx] != 0)) {
			inic_ep = (usbd_inic_ep_t *)whc_usb_priv.irq_info.rx_ep[whc_usb_priv.irq_info.task_ridx];
			len = whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx];

			event = *(u32 *)inic_ep->ep.xfer_buf;

			if (event != WHC_WIFI_EVT_XIMT_PKTS) {
				buf = rtos_mem_zmalloc(len);

				if (buf == NULL) {
					RTK_LOGE(TAG_WLAN_INIC, "%s, can't alloc buffer!!\n", __func__);
				} else {
					memcpy(buf, inic_ep->ep.xfer_buf, len);
					whc_usb_dev_event_int_hdl(buf, NULL);
				}
				/* buf will be freed later*/
			} else {
				if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 2) ||
					((new_skb = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN)) == NULL)) {
					whc_usb_priv.irq_info.wait_xmit_skb = 1;
#ifndef CONFIG_WHC_BRIDGE
					/* resume pending queue to release skb */
					rtw_pending_q_resume();
#endif
					break;
				}
				msg_info = (struct whc_msg_info *)inic_ep->ep.xfer_buf;
				skb_rcv = (struct sk_buff *)inic_ep->userdata;
				skb_reserve(skb_rcv, sizeof(struct whc_msg_info));

				skb_put(skb_rcv, msg_info->data_len);
				skb_rcv->dev = (void *)msg_info->wlan_idx;
				whc_usb_dev_event_int_hdl((u8 *)msg_info, skb_rcv);
				inic_ep->ep.xfer_buf = new_skb->data;
				inic_ep->userdata = new_skb;
			}

			whc_usb_priv.irq_info.rx_ep[whc_usb_priv.irq_info.task_ridx] = NULL;
			whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx] = 0;
			whc_usb_priv.irq_info.task_ridx = (whc_usb_priv.irq_info.task_ridx + 1) % (WIFI_INIC_USB_BULKOUT_EP_NUM);
			usbd_inic_receive_data(inic_ep->ep.addr, inic_ep->ep.xfer_buf, inic_ep->ep.xfer_len, inic_ep->userdata); //need config usb buffer after clear rx_ep list
		}

	} while (1);
}

static void whc_usb_dev_tx_done_cb(usbd_inic_ep_t *in_ep, u8 status)
{
	UNUSED(status);

	if (in_ep->ep.addr == WIFI_INIC_USB_BULKIN_EP) {
		if (whc_usb_priv.irq_info.whc_txbuf == NULL) {
			whc_usb_priv.irq_info.whc_txbuf = (u8 *)in_ep->userdata;
			rtos_sema_give(whc_usb_priv.usb_irq_sema);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "irq task not process last tx done!\n");
		}
	}
}

static void whc_usb_dev_suspend_cb(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "USB SUSPEND\n");
	wifi_inic_usb_status = WIFI_INIC_USB_STATUS_SUSPEND;
}

static void whc_usb_dev_resume_cb(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "USB RESUME\n");
	wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;
}

void whc_usb_dev_status_changed_cb(u8 old_status, u8 new_status)
{
	UNUSED(old_status);
	if (new_status == USBD_ATTACH_STATUS_ATTACHED) {
		wifi_inic_usb_status = WIFI_INIC_USB_STATUS_ACTIVE;
	}
}

int whc_usb_dev_rxbuf_config(void)
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

static usbd_inic_cb_t whc_usb_wifi_cb = {
	.set_config = whc_usb_dev_rxbuf_config,
	.received = whc_usb_dev_rx_done_cb,
	.transmitted = whc_usb_dev_tx_done_cb,
	.status_changed = whc_usb_dev_status_changed_cb,
	.suspend = whc_usb_dev_suspend_cb,
	.resume = whc_usb_dev_resume_cb,
};


static void whc_usb_dev_device_init(void)
{
	int ret;

	ret = usbd_init(&whc_usb_wifi_cfg);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USBdevice init fail!\n");
	}

	ret = usbd_inic_init(&whc_usb_wifi_cb);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USB inic init fail!\n");
	}

	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_usb_dev_irq_task", (rtos_task_function_t)whc_usb_dev_irq_task,
										NULL, 512, 1)) {
		RTK_LOGI(NOTAG, "Create whc_usb_dev_irq_task Err!!\n");
	}
	rtos_mutex_create_static(&whc_usb_priv.tx_lock);
	rtos_sema_create_static(&whc_usb_priv.usb_tx_sema, 0, 0xFFFFFFFF);
	rtos_sema_create_static(&whc_usb_priv.usb_irq_sema, 0, 0xFFFFFFFF);
	rtos_sema_give(whc_usb_priv.usb_tx_sema);
	rtos_sema_give(whc_usb_priv.usb_irq_sema);
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
void whc_usb_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb)
{
	u32 event = *(u32 *)rxbuf;
	struct whc_api_info *ret_msg;

	switch (event) {
	case WHC_WIFI_EVT_API_CALL:
		event_priv.rx_api_msg = rxbuf;
		rtos_sema_give(event_priv.task_wake_sema);

		break;
	case WHC_WIFI_EVT_API_RETURN:
		if (event_priv.b_waiting_for_ret) {
			event_priv.rx_ret_msg = rxbuf;
			rtos_sema_give(event_priv.api_ret_sema);
		} else {
			ret_msg = (struct whc_api_info *)rxbuf;
			RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_WARN, "too late to receive API ret, ID: 0x%x!\n", ret_msg->api_id);

			/* free rx buffer */
			rtos_mem_free((u8 *)ret_msg);
		}

		break;
	case WHC_WIFI_EVT_XIMT_PKTS:
		/* put the inic message to the queue */
		if (whc_msg_enqueue(skb, &dev_xmit_priv.xmit_queue) == RTK_FAIL) {
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
void whc_usb_dev_send(struct whc_buf_info *pbuf)
{
	int i = 0;
	struct whc_txbuf_info_t *whc_txbuf;

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

	rtos_mutex_take(whc_usb_priv.tx_lock, MUTEX_WAIT_TIMEOUT);
	whc_txbuf = container_of(pbuf, struct whc_txbuf_info_t, txbuf_info);

	rtos_sema_take(whc_usb_priv.usb_tx_sema, MUTEX_WAIT_TIMEOUT);

	usbd_inic_transmit_data((u8)WIFI_INIC_USB_BULKIN_EP, (u8 *)pbuf->buf_addr, (u16)pbuf->buf_size, (void *)whc_txbuf);

	rtos_mutex_give(whc_usb_priv.tx_lock);

	return;
}

u8 whc_usb_dev_tx_path_avail(void)
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

void whc_usb_dev_trigger_rx_handle(void)
{
	if (whc_usb_priv.irq_info.wait_xmit_skb == 1) {
		whc_usb_priv.irq_info.wait_xmit_skb = 0;
		rtos_sema_give(whc_usb_priv.usb_irq_sema);
	}
}

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_usb_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	whc_usb_dev_device_init();

	/* initialize the dev priv */
	whc_dev_init_priv();

	whc_dev_api_init();
}

