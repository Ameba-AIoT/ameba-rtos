#include "usbd.h"
#include "usbd_inic.h"
#include "whc_dev.h"
#if (defined(CONFIG_BT) && CONFIG_BT) && (defined(CONFIG_BT_INIC_USB) && CONFIG_BT_INIC_USB)
#include "bt_inic.h"
#endif

struct whc_usb_priv_t whc_usb_priv = {0};

u8 wifi_whc_usb_status = WIFI_WHC_USB_STATUS_ACTIVE;

static usbd_config_t whc_usb_wifi_cfg = {
	.speed = WIFI_WHC_USB_SPEED,
	.isr_priority = 4,
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 292U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 128U, },
#endif
};

/* host->device */
static int whc_usb_dev_rx_done_cb(usbd_inic_ep_t *out_ep, u32 len)
{
	usb_ep_info_t *info = &out_ep->ep.info;
	if (info->addr == WIFI_WHC_USB_BULKOUT_1 || info->addr == WIFI_WHC_USB_BULKOUT_2
		|| info->addr == WIFI_WHC_USB_BULKOUT_3) {
		if ((whc_usb_priv.irq_info.rxdone_epnum[whc_usb_priv.irq_info.intr_widx] == 0) &&
			(whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.intr_widx] == 0)) {
			whc_usb_priv.irq_info.rxdone_epnum[whc_usb_priv.irq_info.intr_widx] = info->addr;
			whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.intr_widx] = len;
			whc_usb_priv.irq_info.intr_widx = (whc_usb_priv.irq_info.intr_widx + 1) % WIFI_WHC_USB_BULKOUT_EP_NUM;
			rtos_sema_give(whc_usb_priv.usb_irq_sema);
		} else {
			usbd_inic_receive_data(info->addr, out_ep->ep.xfer_buf, USB_BUFSZ, NULL);
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "rx cb list full, drop this message!\n");
		}
	}
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	else if (info->addr == USBD_INIC_BT_EP2_BULK_OUT) {
		bt_inic_usb_hci_acl_hdl(out_ep->ep.xfer_buf, len);
	}
#endif
	return RTK_SUCCESS;
}

static void whc_usb_dev_irq_task(void)
{
	struct sk_buff *new_skb = NULL;
	struct sk_buff *skb_rcv = NULL;
	struct whc_msg_info *msg_info;
	struct whc_txbuf_info_t *whc_txbuf;
	u32 event = 0;
	u16 len = 0;
	u8 ep_num = 0;
	do {
		rtos_sema_take(whc_usb_priv.usb_irq_sema, MUTEX_WAIT_TIMEOUT);
		// tx callback
		if (whc_usb_priv.irq_info.txdone != 0) {
			whc_txbuf = (struct whc_txbuf_info_t *)whc_usb_priv.tx_buf;

			whc_dev_free_txbuf(whc_txbuf);
			whc_usb_priv.irq_info.txdone = 0;  // clear tx done flag
			whc_usb_priv.tx_buf = NULL;
			rtos_sema_give(whc_usb_priv.usb_tx_sema);
		}
		//rx callback
		while ((whc_usb_priv.irq_info.rxdone_epnum[whc_usb_priv.irq_info.task_ridx] != 0) &&
			   (whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx] != 0)) {

			ep_num = whc_usb_priv.irq_info.rxdone_epnum[whc_usb_priv.irq_info.task_ridx];
			skb_rcv = (struct sk_buff *)whc_usb_priv.rx_skb_addr[EPNUM_TO_IDX(ep_num)];
			len = whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx];

			event = *(u32 *)skb_rcv->data;

			if (event != WHC_WIFI_EVT_XIMT_PKTS) {
				whc_dev_dispatch_event_copy(skb_rcv->data, len);
				new_skb = skb_rcv;  // not need to malloc new skb
			} else {
				msg_info = (struct whc_msg_info *)skb_rcv->data;
				if (((skbpriv.skb_buff_num - skbpriv.skb_buff_used) < 3) ||
#ifdef CONFIG_WHCH
					(rtw_xmit_check_txbd(msg_info->wlan_hw_queue) == FALSE) ||
#endif
					((new_skb = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN)) == NULL)) {
					new_skb = skb_rcv;
					goto drop_pkt;
				}
				skb_reserve(skb_rcv, sizeof(struct whc_msg_info));

				skb_put(skb_rcv, msg_info->data_len);
				skb_rcv->dev = (void *)((u32)msg_info->wlan_idx);
				whc_dev_event_int_hdl((u8 *)msg_info, skb_rcv);
				whc_usb_priv.rx_skb_addr[EPNUM_TO_IDX(ep_num)] = (u8 *)new_skb;
			}
drop_pkt:
			whc_usb_priv.irq_info.rxdone_epnum[whc_usb_priv.irq_info.task_ridx] = 0;
			whc_usb_priv.irq_info.len[whc_usb_priv.irq_info.task_ridx] = 0;
			whc_usb_priv.irq_info.task_ridx = (whc_usb_priv.irq_info.task_ridx + 1) % (WIFI_WHC_USB_BULKOUT_EP_NUM);
			usbd_inic_receive_data(ep_num, new_skb->data, USB_BUFSZ, NULL); //need config usb buffer after clear rxdone_epnum list
		}

	} while (1);
}

/* device->host  .transmitted*/
static void whc_usb_dev_tx_done_cb(usbd_inic_ep_t *in_ep, u8 status)
{
	UNUSED(status);
	usb_ep_info_t *info = &in_ep->ep.info;
	if (info->addr == WIFI_WHC_USB_BULKIN_EP) {
		if (whc_usb_priv.irq_info.txdone == 0) {
			whc_usb_priv.irq_info.txdone = 1;
			rtos_sema_give(whc_usb_priv.usb_irq_sema);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "irq task not process last tx done!\n");
		}
	}
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	else if (info->addr == USBD_INIT_BT_EP1_INTR_IN) {
		bt_inic_usb_evt_txdone_cb(in_ep->ep.xfer_buf);
	} else if (info->addr == USBD_INIC_BT_EP2_BULK_IN) {
		bt_inic_usb_acl_txdone_cb(in_ep->ep.xfer_buf);
	}
#endif
}

void whc_usb_dev_status_changed_cb(u8 old_status, u8 new_status)
{
	UNUSED(old_status);
	if (new_status == USBD_ATTACH_STATUS_ATTACHED) {
		wifi_whc_usb_status = WIFI_WHC_USB_STATUS_ACTIVE;
	} else if (new_status == USBD_ATTACH_STATUS_DETACHED) {
#if defined(CONFIG_USBD_WHC_HOTPLUG)
		wifi_whc_usb_status = WIFI_WHC_USB_STATUS_DISABLED;
#else
		wifi_whc_usb_status = WIFI_WHC_USB_STATUS_SUSPEND;
#endif
	}
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	bt_inic_status_change_cb(old_status, new_status);
#endif
#if defined(CONFIG_USBD_WHC_HOTPLUG)
	rtos_sema_give(whc_usb_priv.usb_attach_status_sema);
#endif
}

int whc_usb_dev_rxbuf_config_cb(void)
{
	struct sk_buff *skb_new = NULL;

	skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
	whc_usb_priv.rx_skb_addr[EPNUM_TO_IDX(WIFI_WHC_USB_BULKOUT_1)] = (u8 *)skb_new;
	usbd_inic_receive_data(WIFI_WHC_USB_BULKOUT_1, skb_new->data, USB_BUFSZ, NULL);

	skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
	whc_usb_priv.rx_skb_addr[EPNUM_TO_IDX(WIFI_WHC_USB_BULKOUT_2)] = (u8 *)skb_new;
	usbd_inic_receive_data(WIFI_WHC_USB_BULKOUT_2, skb_new->data, USB_BUFSZ, NULL);

	if (WIFI_WHC_USB_BULKOUT_EP_NUM == 3) {
		skb_new = dev_alloc_skb(USB_BUFSZ, USB_SKB_RSVD_LEN);
		whc_usb_priv.rx_skb_addr[EPNUM_TO_IDX(WIFI_WHC_USB_BULKOUT_3)] = (u8 *)skb_new;
		usbd_inic_receive_data(WIFI_WHC_USB_BULKOUT_3, skb_new->data, USB_BUFSZ, NULL);
	}

#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	bt_inic_set_config();
#endif
	return 0;
}

static int whc_usb_dev_init_cb(void)
{
	int ret = HAL_OK;

	// wifi use skb, not need to malloc buffer

#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	if (usbd_inic_is_bt_en()) {
		ret = bt_inic_usb_init();
	}
#endif
	return ret;
}

static int whc_usb_dev_deinit_cb(void)
{
	struct sk_buff *skb_rcv = NULL;
	struct whc_txbuf_info_t *whc_txbuf = NULL;
	u8 i;

	// free tx buffer
	if (whc_usb_priv.tx_buf) {
		whc_txbuf = (struct whc_txbuf_info_t *)whc_usb_priv.tx_buf;

		if (whc_txbuf->is_skb) {
			dev_kfree_skb_any((struct sk_buff *) whc_txbuf->ptr);
		} else {
			rtos_mem_free((u8 *)whc_txbuf->ptr);
		}

		rtos_mem_free((u8 *)whc_txbuf);

		whc_usb_priv.tx_buf = NULL;
		whc_usb_priv.irq_info.txdone = 0;
		rtos_sema_give(whc_usb_priv.usb_tx_sema);
	}

	// free rx skb
	for (i = 0; i < WIFI_WHC_USB_BULKOUT_EP_NUM; i++) {
		skb_rcv = (struct sk_buff *)whc_usb_priv.rx_skb_addr[i];
		dev_kfree_skb_any((struct sk_buff *) skb_rcv);
		whc_usb_priv.rx_skb_addr[i] = NULL;
		whc_usb_priv.irq_info.rxdone_epnum[i] = 0;
		whc_usb_priv.irq_info.len[i] = 0;
	}

	whc_usb_priv.irq_info.intr_widx = 0;
	whc_usb_priv.irq_info.task_ridx = 0;

#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	if (usbd_inic_is_bt_en()) {
		bt_inic_usb_deinit();
	}
#endif

	return HAL_OK;
}

static int whc_usb_dev_setup_cb(usb_setup_req_t *req, u8 *buf)
{
#if !(defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB))
	UNUSED(buf);
#endif
	int ret = HAL_OK;
	switch (req->bRequest) {
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	case USBD_INIC_VENDOR_REQ_BT_HCI_CMD:
		ret = bt_inic_usb_hci_cmd_hdl(buf, req->wLength);
		break;
#endif
	default:
		break;
	}
	return ret;
}

static int whc_usb_dev_clear_config_cb(void)
{
	return HAL_OK;
}

static usbd_inic_cb_t whc_usb_dev_cb = {
	.init = whc_usb_dev_init_cb,
	.deinit = whc_usb_dev_deinit_cb,
	.setup = whc_usb_dev_setup_cb,
	.set_config = whc_usb_dev_rxbuf_config_cb,
	.clear_config = whc_usb_dev_clear_config_cb,
	.received = whc_usb_dev_rx_done_cb,
	.transmitted = whc_usb_dev_tx_done_cb,
	.status_changed = whc_usb_dev_status_changed_cb,
};

#if defined(CONFIG_USBD_WHC_HOTPLUG)
static void whc_usb_hotplug_task(void)
{
	do {
		rtos_sema_take(whc_usb_priv.usb_attach_status_sema, 0xFFFFFFFF);
		if (wifi_whc_usb_status == WIFI_WHC_USB_STATUS_DISABLED) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "DETACHED, so reboot\n");
			rtos_time_delay_ms(20);
			System_Reset();
		} else if (wifi_whc_usb_status == WIFI_WHC_USB_STATUS_ACTIVE) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "ATTACHED\n");
		}
	} while (1);

	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_WHC_HOTPLUG

void whc_usb_dev_init(void)
{
	int ret;

	ret = usbd_init(&whc_usb_wifi_cfg);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USBdevice init fail!\n");
	}

	ret = usbd_inic_init(&whc_usb_dev_cb);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG_WLAN_INIC, "USB whc init fail!\n");
	}

	rtos_mutex_create_static(&whc_usb_priv.tx_lock);
	rtos_sema_create_static(&whc_usb_priv.usb_tx_sema, 1, 0xFFFFFFFF);
	rtos_sema_create_static(&whc_usb_priv.usb_irq_sema, 0, 0xFFFFFFFF);

#if defined(CONFIG_USBD_WHC_HOTPLUG)
	rtos_sema_create_static(&whc_usb_priv.usb_attach_status_sema, 0, 1);
	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_usb_hotplug_task", (rtos_task_function_t)whc_usb_hotplug_task,
										NULL, 1024, 8)) {
		RTK_LOGI(NOTAG, "Create whc_usb_dev_irq_task Err!!\n");
	}
#endif

	if (RTK_SUCCESS != rtos_task_create(NULL, (const char *const)"whc_usb_dev_irq_task", (rtos_task_function_t)whc_usb_dev_irq_task,
										NULL, 512, 1)) {
		RTK_LOGI(NOTAG, "Create whc_usb_dev_irq_task Err!!\n");
	}

	RTK_LOGI(TAG_WLAN_INIC, "USB device init done!\n");
}

/*wifi recv pkt, send to host*/
void whc_usb_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb)
{
	int i = 0;
	struct whc_txbuf_info_t *buf_info = NULL;
	struct whc_buf_info *pbuf;

	buf_info = whc_dev_alloc_buf_info(buf, len, buf_alloc, is_skb);
	if (!buf_info) {
		goto drop;
	}
	pbuf = &buf_info->txbuf_info;

	if (wifi_whc_usb_status == WIFI_WHC_USB_STATUS_SUSPEND) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "DEV Send when suspend\n");
		usbd_wake_host();
		for (i = 0; i < WIFI_WHC_USB_WAKE_HOST_MAX_WAIT_CNT; i++) {
			rtos_time_delay_ms(WIFI_WHC_USB_WAKE_HOST_UNIT);
			if (wifi_whc_usb_status == WIFI_WHC_USB_STATUS_ACTIVE) {
				break;
			}
		}
		//rtos_time_delay_ms(500); /*todo: need confirm host already resumed before send data to host*/
	}

	rtos_mutex_take(whc_usb_priv.tx_lock, MUTEX_WAIT_TIMEOUT);

	rtos_sema_take(whc_usb_priv.usb_tx_sema, MUTEX_WAIT_TIMEOUT);
	whc_usb_priv.tx_buf = (u8 *)buf_info;
	usbd_inic_transmit_data((u8)WIFI_WHC_USB_BULKIN_EP, (u8 *)pbuf->buf_addr, (u16)pbuf->buf_size, NULL);
	rtos_mutex_give(whc_usb_priv.tx_lock);

	return;

drop:
	/* Dev drop tx, free tx skb or buffer */
	if (is_skb) {
		dev_kfree_skb_any((struct sk_buff *) buf_alloc);
	} else {
		rtos_mem_free((u8 *)buf_alloc);
	}
}

u8 whc_usb_dev_bus_is_idle(void)
{
	/*Not yet implemented*/
	return TRUE;
}

