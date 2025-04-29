#include <whc_host_linux.h>

struct rtw_usbreq *whc_usb_host_dequeue(struct list_head *q, int *counter)
{
	struct whc_usb *priv = &whc_usb_host_priv;
	unsigned long flags;
	struct rtw_usbreq  *req;

	spin_lock_irqsave(&priv->usb_qlock, flags);
	if (list_empty(q)) {
		spin_unlock_irqrestore(&priv->usb_qlock, flags);
		return NULL;
	}
	req = list_entry(q->next, struct rtw_usbreq, list);
	list_del(&req->list);
	if (counter) {
		(*counter)--;
	}
	spin_unlock_irqrestore(&priv->usb_qlock, flags);

	return req;

}
void whc_usb_host_enqueue(struct list_head *q, struct rtw_usbreq *req, int *counter)
{
	struct whc_usb *priv = &whc_usb_host_priv;
	unsigned long flags;

	spin_lock_irqsave(&priv->usb_qlock, flags);
	list_add_tail(&req->list, q);
	if (counter) {
		(*counter)++;
	}
	spin_unlock_irqrestore(&priv->usb_qlock, flags);
}

static void whc_usb_host_tx_complete_cb(struct urb *urb)
{
	struct whc_usb *priv = &whc_usb_host_priv;
	struct rtw_usbreq *req = (struct rtw_usbreq *)urb->context;

	if (urb->status != 0) {
		printk("tx urb sts=%d\n", urb->status);
		return;
	}

	if (req->skb) {
		if (!req->is_buf) {
			dev_kfree_skb_any(req->skb);
		} else {
			kfree(req->skb);
		}
	}

	req->skb = NULL;
	req->is_buf = 0;
	whc_usb_host_enqueue(&priv->tx_freeq, req, NULL);
	atomic_dec(&priv->tx_inflight);
	priv->txreq_available = 1;
	wake_up(&priv->txreq_wq);

	//USB_TODO: check free urb count, may netif wake queue
}

void whc_usb_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
#ifdef CONFIG_INIC_USB_ASYNC_SEND
	struct whc_usb *priv = &whc_usb_host_priv;
	struct rtw_usbreq *req;
	int ret = 0;

	if (len > RTW_USB_MAX_SKB_SIZE) {
		printk("usb send data size overflow!\n");
		len = RTW_USB_MAX_SKB_SIZE;
	}

	if (priv->usb_disconnecting && (!priv->usb_deregistering)) {
		if (pskb) {
			dev_kfree_skb(pskb);
		} else {
			kfree(buf);
		}
	}
	mutex_lock(&priv->lock);

dequeue_again:
	req = whc_usb_host_dequeue(&priv->tx_freeq, NULL);
	if (req == NULL) {
		priv->txreq_available = 0;
		if (!wait_event_timeout(priv->txreq_wq, priv->txreq_available == 1, msecs_to_jiffies(500))) {
			printk("wait txreq available timeout\n");
			goto exit_free_buf;
		} else {
			goto dequeue_again;
		}
	}
	if (pskb) {
		req->skb = pskb;
		req->is_buf = 0;
	} else {
		req->skb = buf;
		req->is_buf = 1;
	}

	req->urb->transfer_flags |= URB_ZERO_PACKET;
	usb_fill_bulk_urb(req->urb, priv->usb_dev, priv->tx_pipe[priv->out_pipe_idx],
					  buf, len, whc_usb_host_tx_complete_cb, req);
	priv->out_pipe_idx = (priv->out_pipe_idx + 1) % WIFI_OUT_EP_NUM_TOTAL;
	ret = usb_submit_urb(req->urb, GFP_ATOMIC);
	if (ret) {
		printk("%s: Failed to resubmit TX URB: %d\n", __func__, ret);
		req->skb = NULL;
		req->is_buf = 0;
		whc_usb_host_enqueue(&priv->tx_freeq, req, NULL);
		goto exit_free_buf;
	}
	atomic_inc(&priv->tx_inflight);
	mutex_unlock(&priv->lock);
	return;

exit_free_buf:
	if (pskb) {
		dev_kfree_skb(pskb);
	} else {
		kfree(buf);
	}
	mutex_unlock(&priv->lock);
#else
	struct whc_usb *priv = &whc_usb_host_priv;
	int actual_len = 0;
	char *zlp_buf;

	if (priv->usb_disconnecting && (!priv->usb_deregistering)) {
		return;
	}

	if (len > RTW_USB_MAX_SKB_SIZE) {
		printk("usb send data size overflow!\n");
		len = RTW_USB_MAX_SKB_SIZE;
	}

	mutex_lock(&priv->lock);
	atomic_inc(&priv->tx_inflight);

	usb_bulk_msg(priv->usb_dev, priv->tx_pipe[priv->out_pipe_idx], buf, len, &actual_len, 1000);
	//printk("%s: acutul sen=%d\n", __FUNCTION__, actual_len);
	if (len % priv->tx_mps == 0) {/*send ZLP*/
		zlp_buf = kmalloc(1, GFP_KERNEL);
		usb_bulk_msg(priv->usb_dev, priv->tx_pipe[priv->out_pipe_idx], zlp_buf, 0, &actual_len, 100);
		kfree(zlp_buf);
	}
	atomic_dec(&priv->tx_inflight);
	mutex_unlock(&priv->lock);
#endif
}

void whc_usb_host_rx_complete(struct urb *urb)
{
	/*this will run in interrupt, thus just alloc a new skb and put it on usb_req,
	 enqueue the original skb and then notify other task to process */
	struct rtw_usbreq *req;
	struct sk_buff *skb;
	struct sk_buff *skb_copy;
	struct whc_usb *priv = &whc_usb_host_priv;
	int	 ret;
	unsigned long flags;
	unsigned long addr;

	req = (struct rtw_usbreq *)urb->context;


	if (urb->status != 0) {
		printk("urb sts=%d\n", urb->status);
		return;
	}

	skb = dev_alloc_skb(RTW_USB_MAX_SKB_SIZE);
	addr = (unsigned long)(skb->data);

	if (!skb) {
		printk("cannot alloc skb, drop this packet\n");
		return;
	}

	skb_copy = req->skb;
	req->skb = skb;

	spin_lock_irqsave(&priv->usb_rxskb_lock, flags);
	skb_queue_tail(&priv->rxnode_list, skb_copy);
	spin_unlock_irqrestore(&priv->usb_rxskb_lock, flags);
	//printk("acutul rcv=%d\n", urb->actual_length);
	whc_host_recv_notify();

	usb_fill_bulk_urb(req->urb, priv->usb_dev, priv->rx_pipe,
					  skb->data, RTW_USB_MAX_SKB_SIZE, whc_usb_host_rx_complete, req);
	ret = usb_submit_urb(req->urb, GFP_ATOMIC);
	if (ret) {
		printk("%s: Failed to resubmit URB: %d\n", __func__, ret);
		return;
	}
}

void whc_usb_host_recv_data_process(void *intf_priv)
{
	struct whc_usb *priv = (struct whc_usb *)intf_priv;
	struct sk_buff *skb;
	unsigned long flags;

	spin_lock_irqsave(&priv->usb_rxskb_lock, flags);
	skb = skb_dequeue(&priv->rxnode_list);
	spin_unlock_irqrestore(&priv->usb_rxskb_lock, flags);

	if (skb) {
		priv->rx_process_func(skb);
	}
}


struct hci_ops_t whc_usb_host_intf_ops = {
	.send_data = whc_usb_host_send_data,
	.recv_data_process = whc_usb_host_recv_data_process,
};

int whc_usb_host_send_event_check(u32 event_id)
{
	struct whc_usb *priv = &whc_usb_host_priv;

	/*when kernel's usb_disconnect triggered, endpoint will be disabled, thus cannot send event*/
	/*but when driver->disconnect called by unregister_usb, since soft_unbind=1,
	endpoint will not be disabled, event can still be send*/
	if (priv->usb_disconnecting && (!priv->usb_deregistering)) {
		if (event_id == WHC_API_WIFI_DISCONNECT) {
			/*due to whc_fullmac_host_event_disconnect will wait for sema*/
			whc_fullmac_host_disconnect_indicate(0, 1);
			complete(&global_idev.mlme_priv.disconnect_done_sema);
		}
		return -1;
	}
	return 0;
}

