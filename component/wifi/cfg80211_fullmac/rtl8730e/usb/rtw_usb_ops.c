#include <rtw_cfg80211_fullmac.h>

struct rtw_usbreq *rtw_usb_dequeue(struct list_head *q, int *counter)
{
	struct inic_usb *priv = &inic_usb_priv;
	unsigned long flags;
	struct rtw_usbreq  *req;

	spin_lock_irqsave(&priv->usb_qlock, flags);
	if (list_empty(q)) {
		spin_unlock_irqrestore(&priv->usb_qlock, flags);
		return NULL;
	}
	req = list_entry(q->next, struct rtw_usbreq, list);
	list_del_init(q->next);
	if (counter) {
		(*counter)--;
	}
	spin_unlock_irqrestore(&priv->usb_qlock, flags);

	return req;

}
void rtw_usb_enqueue(struct list_head *q, struct rtw_usbreq *req, int *counter)
{
	struct inic_usb *priv = &inic_usb_priv;
	unsigned long flags;

	spin_lock_irqsave(&priv->usb_qlock, flags);
	list_add_tail(&req->list, q);
	if (counter) {
		(*counter)++;
	}
	spin_unlock_irqrestore(&priv->usb_qlock, flags);
}

static void rtw_usb_tx_complete_cb(struct urb *urb)
{
	struct inic_usb *priv = &inic_usb_priv;
	struct rtw_usbreq *req = (struct rtw_usbreq *)urb->context;

	if (req->skb) {
		dev_kfree_skb_any(req->skb);
	}

	req->skb = NULL;
	rtw_usb_enqueue(&priv->tx_freeq, req, NULL);
	//USB_TODO: check free urb count, may netif wake queue
}

void rtw_usb_send_data(u8 *buf, u32 len)
{
	struct inic_usb *priv = &inic_usb_priv;
	int actual_len = 0;
	char *zlp_buf;

	mutex_lock(&priv->lock);
	priv->tx_inflight++;
	usb_bulk_msg(priv->usb_dev, priv->tx_pipe[priv->out_pipe_idx], buf, len, &actual_len, 1000);
	printk("%s: acutul sen=%d\n", __FUNCTION__, actual_len);
	priv->out_pipe_idx = (priv->out_pipe_idx + 1) % WIFI_OUT_EP_NUM_TOTAL;
	if (len % priv->tx_mps == 0) {/*send ZLP*/
		zlp_buf = kmalloc(1, GFP_KERNEL);
		usb_bulk_msg(priv->usb_dev, priv->tx_pipe[priv->out_pipe_idx], zlp_buf, 0, &actual_len, 100);
		kfree(zlp_buf);

	}
	priv->tx_inflight--;
	//USB_TODO: if free urb is below thresh, need special process? =>netif stop queue?

	mutex_unlock(&priv->lock);
}

void rtw_usb_rx_complete(struct urb *urb)
{
	/*this will run in interrupt, thus just alloc a new skb and put it on usb_req,
	 enqueue the original skb and then notify other task to process */
	struct rtw_usbreq *req;
	struct sk_buff *skb;
	struct sk_buff *skb_copy;
	struct inic_usb *priv = &inic_usb_priv;
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
	printk("acutul rcv=%d\n", urb->actual_length);
	llhw_recv_notify();

	usb_fill_bulk_urb(req->urb, priv->usb_dev, priv->rx_pipe,
					  skb->data, RTW_USB_MAX_SKB_SIZE, rtw_usb_rx_complete, req);
	ret = usb_submit_urb(req->urb, GFP_ATOMIC);
	if (ret) {
		printk("%s: Failed to resubmit URB: %d\n", __func__, ret);
		return;
	}
}

void rtw_usb_recv_data_process(void *intf_priv)
{
	struct inic_usb *priv = (struct inic_usb *)intf_priv;
	struct sk_buff *skb;
	unsigned long flags;

	spin_lock_irqsave(&priv->usb_rxskb_lock, flags);
	skb = skb_dequeue(&priv->rxnode_list);
	spin_unlock_irqrestore(&priv->usb_rxskb_lock, flags);

	if (skb) {
		priv->rx_process_func(skb);
	}
}

struct hci_ops_t usb_intf_ops = {
	.send_data = rtw_usb_send_data,
	.recv_data_process = rtw_usb_recv_data_process,
};

