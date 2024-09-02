#include <rtw_cfg80211_fullmac.h>
#include <linux/spi/spi.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>


#define RTK_USB_VID							0x0BDA
#define RTK_USB_PID							0x8722

#define RTW_INIC_WIFI_EP3_BULK_IN 						0x03U
#define RTW_INIC_WIFI_EP4_BULK_OUT						0x04U

#define RTW_USB_RXQ_NUM						0x0AU
#define RTW_USB_TXQ_NUM						0x20U


static struct usb_device_id inic_usb_ids[] = {
	{USB_DEVICE(RTK_USB_VID, RTK_USB_PID)},
	{}
};

MODULE_DEVICE_TABLE(usb, inic_usb_ids);
struct inic_usb inic_usb_priv = {0};

int rtw_usb_init(struct inic_usb *priv, struct usb_interface *intf)
{
	struct usb_interface_descriptor	*desc;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_device *usb;
	u32 num_of_eps;
	u8 endpoint_num, ep;

	desc = &intf->cur_altsetting->desc;
	num_of_eps = desc->bNumEndpoints;
	usb = interface_to_usbdev(intf);
	for (ep = 0; ep < num_of_eps; ep++) {
		endpoint = &intf->cur_altsetting->endpoint[ep].desc;
		endpoint_num = usb_endpoint_num(endpoint);
		if (!usb_endpoint_xfer_bulk(endpoint)) {
			continue;
		}

		if (usb_endpoint_dir_in(endpoint) && (endpoint_num == RTW_INIC_WIFI_EP3_BULK_IN)) {
			priv->rx_pipe = usb_rcvbulkpipe(usb, endpoint_num);
		} else if (usb_endpoint_dir_out(endpoint)) {
			if (endpoint_num == RTW_INIC_WIFI_EP4_BULK_OUT) {
				priv->tx_pipe[0] = usb_sndbulkpipe(usb, endpoint_num);
				priv->tx_mps = usb_endpoint_maxp(endpoint);
			}
		}
	}
	return true;
}

void rtw_usb_deinit(struct inic_usb *priv)
{
	int i;

	priv->rx_pipe = 0;
	for (i = 0; i < WIFI_OUT_EP_NUM_TOTAL; i++) {
		priv->tx_pipe[i] = 0;
	}
}

static int rtw_usb_trx_resource_init(struct list_head *q, int qsize, int pre_submit, u8 is_tx)
{
	int i;
	struct rtw_usbreq *req, *reqs;
	struct sk_buff	*skb;
	struct inic_usb *priv = &inic_usb_priv;
	int ret;
	unsigned long addr;

	reqs = kzalloc(qsize * sizeof(struct rtw_usbreq), GFP_ATOMIC);
	if (reqs == NULL) {
		return false;
	}
	if (is_tx) {
		priv->txreqs = (struct rtw_usbreq *)reqs;
	} else {
		priv->rxreqs = (struct rtw_usbreq *)reqs;
	}

	req = reqs;

	for (i = 0; i < qsize; i++) {
		req->urb = usb_alloc_urb(0, GFP_ATOMIC);
		if (!req->urb) {
			goto qinit_fail;
		}

		if (pre_submit) {
			skb = dev_alloc_skb(RTW_USB_MAX_SKB_SIZE);
			if (!skb) {
				goto qinit_fail;
			}
			req->skb = skb;

		}
		INIT_LIST_HEAD(&req->list);
		list_add_tail(&req->list, q);

		if (pre_submit) {
			usb_fill_bulk_urb(req->urb, priv->usb_dev, priv->rx_pipe,
							  skb->data, RTW_USB_MAX_SKB_SIZE, rtw_usb_rx_complete, req);

			ret = usb_submit_urb(req->urb, GFP_ATOMIC);
			if (ret) {
				list_del_init(&req->list);
				dev_kfree_skb_any(skb);
				req->skb = NULL;
				usb_free_urb(req->urb);
				printk("submit rx urb fail=%d\n", ret);
				goto qinit_fail;
			}
		}

		req++;
	}

	return true;
qinit_fail:
	while (!list_empty(q)) {
		req = list_entry(q->next, struct rtw_usbreq, list);
		if (req) {
			if (req->skb) {
				dev_kfree_skb_any(req->skb);
			}
			usb_free_urb(req->urb);
		}

		list_del(q->next);
	}
	kfree(reqs);
	if (is_tx) {
		priv->txreqs = NULL;
	} else {
		priv->rxreqs = NULL;
	}
	return false;
}

static int rtw_usb_init_phase2(struct inic_usb *priv)
{
	spin_lock_init(&priv->usb_qlock);
	spin_lock_init(&priv->usb_rxskb_lock);

	atomic_set(&priv->tx_inflight, 0);
	INIT_LIST_HEAD(&priv->tx_freeq);
	if (rtw_usb_trx_resource_init(&priv->tx_freeq, RTW_USB_TXQ_NUM, 0, 1) != true) {
		return false;
	}

	INIT_LIST_HEAD(&priv->rx_freeq);
	skb_queue_head_init(&priv->rxnode_list);
	if (rtw_usb_trx_resource_init(&priv->rx_freeq, RTW_USB_RXQ_NUM, 1, 0) != true) {
		return false;
	}

	return true;
}

static int rtw_usb_probe(struct usb_interface *intf, const struct usb_device_id *id_table)
{
	int i;
	int id_cnt = sizeof(inic_usb_ids) / sizeof(struct usb_device_id);
	struct inic_usb *priv = &inic_usb_priv;
	struct usb_device *usb = interface_to_usbdev(intf);
	int status;

	printk("rtw usb probe start\n");
	for (i = 0; i < id_cnt; i++) {
		if ((id_table->idVendor == inic_usb_ids[i].idVendor) &&
			(id_table->idProduct == inic_usb_ids[i].idProduct)) {
			break;
		}
	}
	if (i == id_cnt) {
		printk("USB device descriptor not match\n");
		goto exit;
	}

	memset(priv, 0, sizeof(struct inic_usb));
	mutex_init(&(priv->lock));

	priv->usb_dev = usb;
	priv->dev = &usb->dev;
	priv->rx_process_func = llhw_recv_process;

	usb_set_intfdata(intf, (void *)priv);

	dev_info(priv->dev, "rtw_usb_probe: vendor=0x%04x product=0x%04x\n", id_table->idVendor, id_table->idProduct);

	if (rtw_usb_init(priv, intf) != true) {
		dev_err(priv->dev, "%s: initialize usb Failed!\n", __FUNCTION__);
		goto exit;
	}

	status = rtw_usb_init_phase2(priv);
	if (status != true) {
		goto exit;
	}

	device_wakeup_enable(&usb->dev);

	rtw_netdev_probe(priv->dev);

	status = true;

exit:
	if (status != true) {
		usb_set_intfdata(intf, NULL);
		rtw_usb_deinit(priv);
	}

	return status == true ? 0 : -ENODEV;
}

static void rtw_usb_disconnect(struct usb_interface *intf)
{
	struct rtw_usbreq *req, *next;
	struct inic_usb *priv = &inic_usb_priv;

	printk("%s\n", __func__);

	priv->usb_disconnecting = 1;
	if (priv->dev && (global_idev.pndev[0] != NULL)) {
		rtw_netdev_remove(priv->dev);
	}

	/*free tx related resource*/
	while ((req = rtw_usb_dequeue(&priv->tx_freeq, NULL)) != NULL) {
		usb_kill_urb(req->urb);
		usb_free_urb(req->urb);
	}
	kfree(priv->txreqs);

	/*free rx related resource*/
	req = NULL;
	list_for_each_entry_safe(req, next, &priv->rx_freeq, list) {
		if (!req->urb) {
			break;
		}

		if (req->skb) {
			dev_kfree_skb_any(req->skb);
			req->skb = NULL;
		}
		usb_kill_urb(req->urb);
		usb_free_urb(req->urb);
		list_del_init(&req->list);
	}
	kfree(priv->rxreqs);
	skb_queue_purge(&priv->rxnode_list);

	usb_set_intfdata(intf, NULL);
}

static int rtw_usb_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct inic_usb *priv = &inic_usb_priv;
	struct rtw_usbreq *req, *next;

	netif_tx_stop_all_queues(global_idev.pndev[0]);
	if (atomic_read(&priv->tx_inflight)) {
		netif_tx_start_all_queues(global_idev.pndev[0]);
		netif_tx_wake_all_queues(global_idev.pndev[0]);
		return -1;
	}

	req = NULL;
	list_for_each_entry_safe(req, next, &priv->rx_freeq, list) {
		if (!req->urb) {
			break;
		}

		usb_kill_urb(req->urb);
	}

	return 0;
}

static int rtw_usb_resume(struct usb_interface *intf)
{
	struct inic_usb *priv = &inic_usb_priv;
	struct rtw_usbreq *req, *next;
	struct sk_buff	*skb;
	int ret = 0;

	list_for_each_entry_safe(req, next, &priv->rx_freeq, list) {
		if (!req->urb) {
			break;
		}

		skb = req->skb;
		usb_fill_bulk_urb(req->urb, priv->usb_dev, priv->rx_pipe,
						  skb->data, RTW_USB_MAX_SKB_SIZE, rtw_usb_rx_complete, req);

		ret = usb_submit_urb(req->urb, GFP_ATOMIC);
		if (ret) {
			printk("resume submit urb fail\n");
		}
	}

	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	return 0;
}

static struct usb_driver inic_usb_driver = {
	.name = "INIC_USB",
	.probe = rtw_usb_probe,
	.disconnect = rtw_usb_disconnect,
	.id_table = inic_usb_ids,
	.suspend = rtw_usb_suspend,
	.resume = rtw_usb_resume,
	.soft_unbind = 1,
};

static int __init rtw_usb_init_module(void)
{
	int ret = 0;

	printk("%s\n", __func__);

	ret = usb_register(&inic_usb_driver);
	if (ret != 0) {
		printk("usb register driver Failed!\n");
	}

	rtw_inetaddr_notifier_register();

	return ret;
}

static void __exit rtw_usb_cleanup_module(void)
{
	struct inic_usb *priv = &inic_usb_priv;
	priv->usb_deregistering = 1;
	rtw_inetaddr_notifier_unregister();

	usb_deregister(&inic_usb_driver);
}

module_init(rtw_usb_init_module);
module_exit(rtw_usb_cleanup_module);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek iNIC Fullmac");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");

