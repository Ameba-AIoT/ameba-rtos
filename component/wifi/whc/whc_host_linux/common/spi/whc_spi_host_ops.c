#include <whc_host_linux.h>
#include <whc_host_cmd_path_api.h>

struct whc_msg_node *whc_host_dequeue_tx_packet(struct xmit_priv_t *xmit_priv);

void whc_spi_host_send_data(u8 *buf, u32 len, struct sk_buff *skb)
{
	struct whc_spi *priv = &whc_spi_priv;
	struct spi_device *spidev = priv->spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *pskb = NULL;
	int rc;

	(void) skb;

	mutex_lock(&priv->lock);

	while (atomic_read(&priv->dev_state) == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(global_idev.fullmac_dev, "%s: wait dev busy(%d) timeout, can't send data\n\r", __func__, gpio_get_value(DEV_READY_PIN));
			goto exit;
		}
	}

	if (len > SPI_BUFSZ) {
		dev_err(global_idev.fullmac_dev, "%s: len(%d) > SPI_BUFSZ\n\r", __func__, len);
	}

	/* allocate rx skb */
	pskb = netdev_alloc_skb(NULL, SPI_BUFSZ);
	if (pskb == NULL) {
		dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err!!\n\r", __func__);
		goto exit;
	}

	/* initiate spi transaction */
	spimsg = spi_message_alloc(1, GFP_KERNEL);
	if (spimsg == NULL) {
		kfree_skb(pskb);
		goto exit;
	}

	/* set dev busy if to start to send data, for case device can't drive ready pin. */
	atomic_set(&priv->dev_state, DEV_BUSY);

	tr = list_first_entry(&spimsg->transfers, struct spi_transfer, transfer_list);
	tr->len = SPI_BUFSZ;
	tr->tx_buf = buf;
	tr->rx_buf = pskb->data;

	rc = spi_sync(spidev, spimsg);
	if (rc) {
		dev_err(&spidev->dev, "could not transfer : %d\n", rc);
		if (pskb) {
			kfree_skb(pskb);
		}
		goto exit;
	}

	//print_hex_dump_bytes("whc_spi_host_send_data: ", DUMP_PREFIX_NONE, tr->tx_buf, tr->len);

	/* process rx msg */
	priv->rx_process_func(pskb);

exit:
	mutex_unlock(&priv->lock);

	if (spimsg) {
		spi_message_free(spimsg);
	}
}

void whc_spi_host_recv_data_process(void *intf_priv)
{
	struct whc_spi *priv = &whc_spi_priv;
	struct spi_device *spidev = priv->spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *pskb = NULL, *tx_skb = NULL;
	struct whc_msg_node *p_node;
	int rc;

	mutex_lock(&priv->lock);

	while (atomic_read(&priv->dev_state) == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(global_idev.fullmac_dev, "%s: wait dev busy(%d) timeout, can't send data\n\r", __func__, gpio_get_value(DEV_READY_PIN));
			goto exit;
		}
	}

	/* check RX_REQ level */
	if (gpio_get_value(RX_REQ_PIN)) {

		/* allocate skb */
		pskb = netdev_alloc_skb(NULL, SPI_BUFSZ);
		if (pskb == NULL) {
			dev_err(global_idev.fullmac_dev, "%s: Alloc skb rx buf Err!!\n\r", __func__);
			goto exit;
		}

		/* initiate spi transaction */
		spimsg = spi_message_alloc(1, GFP_KERNEL);
		if (spimsg == NULL) {
			kfree_skb(pskb);
			goto exit;
		}

		/* set dev busy if to start to send data, for case device can't drive ready pin. */
		atomic_set(&priv->dev_state, DEV_BUSY);

		tr = list_first_entry(&spimsg->transfers, struct spi_transfer, transfer_list);
		tr->rx_buf = pskb->data;
		tr->len = SPI_BUFSZ;

		p_node = whc_host_dequeue_tx_packet(&global_idev.xmit_priv);
		if (p_node != NULL) {
			tx_skb = p_node->msg;
			tr->tx_buf = tx_skb->data;
		}

		rc = spi_sync(spidev, spimsg);
		if (rc) {
			dev_err(&spidev->dev, "could not transfer : %d\n", rc);
			kfree_skb(pskb);

			goto exit;
		}

		//print_hex_dump_bytes("whc_spi_host_recv_data_process: ", DUMP_PREFIX_NONE, tr->rx_buf, tr->len);

		/* wake tx queue if need */
		if (p_node != NULL) {
			if (whc_host_xmit_pending_q_num() < QUEUE_WAKE_THRES) {
				netif_tx_wake_all_queues(global_idev.pndev[0]);
				if (global_idev.pndev[1]) {
					netif_tx_wake_all_queues(global_idev.pndev[1]);
				}
			}

			/* release the memory for this message. */
			dev_kfree_skb(tx_skb);
			kfree(p_node);
		}

		/* process rx msg */
		priv->rx_process_func(pskb);
	}

exit:
	mutex_unlock(&priv->lock);

	if (spimsg) {
		spi_message_free(spimsg);
	}
}

#ifdef CONFIG_WHC_CMD_PATH
void whc_host_send_cmd_data(u8 *buf, u32 len)
{
	struct whc_cmd_path_hdr *hdr = NULL;
	u32 event = *(u32 *)buf;
	u8 *txbuf = NULL;
	u32 txsize = len + sizeof(struct whc_cmd_path_hdr);

	if (event != WHC_WIFI_EVT_XIMT_PKTS) {
		txbuf = kzalloc(txsize, GFP_KERNEL);
		if (txbuf) {
			hdr = (struct whc_cmd_path_hdr *)txbuf;
			hdr->event = WHC_WIFI_EVT_BRIDGE;
			hdr->len = len;
			/* copy data */
			memcpy(txbuf + sizeof(struct whc_cmd_path_hdr), buf, len);
			whc_spi_host_send_data(txbuf, txsize, NULL);
			kfree(txbuf);
		} else {
			dev_err(global_idev.fullmac_dev, "%s can't alloc spi buffer!\n", __func__);
		}
	} else {
		whc_spi_host_send_data(buf, len, NULL);
	}
}

int whc_host_cmd_data_rx_to_user(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	u32 size = 0;
	struct whc_cmd_path_hdr *hdr = NULL;
	u8 *rxbuf = NULL;


	switch (event) {
	case WHC_WIFI_EVT_BRIDGE:
		hdr = (struct whc_cmd_path_hdr *)(pskb->data + SIZE_RX_DESC);
		size = hdr->len;
		rxbuf = (u8 *)pskb->data + SIZE_RX_DESC + sizeof(struct whc_cmd_path_hdr);
		whc_host_buf_rx_to_user(rxbuf, size);
		break;
	default:
		break;
	}

	return ret;
}
#endif

struct hci_ops_t whc_spi_host_intf_ops = {
	.send_data = whc_spi_host_send_data,
	.recv_data_process = whc_spi_host_recv_data_process,
};

