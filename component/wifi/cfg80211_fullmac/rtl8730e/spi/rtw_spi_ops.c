#include <rtw_cfg80211_fullmac.h>

void rtw_spi_send_data(u8 *buf, u32 len)
{
	struct inic_spi *priv = &inic_spi_priv;
	struct spi_device *spidev = priv->spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *pskb = NULL;
	int rc;

	mutex_lock(&priv->lock);

	while (priv->dev_state == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(global_idev.fullmac_dev, "%s: wait dev busy timeout, can't send data\n\r", __func__);
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
		goto exit;
	}

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

	//print_hex_dump_bytes("rtw_spi_send_data: ", DUMP_PREFIX_NONE, tr->tx_buf, tr->len);

	/* process rx msg */
	priv->rx_process_func(pskb);

exit:
	mutex_unlock(&priv->lock);

	if (spimsg) {
		spi_message_free(spimsg);
	}
}

void rtw_spi_recv_data_process(void *intf_priv)
{
	struct inic_spi *priv = &inic_spi_priv;
	struct spi_device *spidev = priv->spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *pskb = NULL;
	int rc;

	mutex_lock(&priv->lock);

	while (priv->dev_state == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(global_idev.fullmac_dev, "%s: wait dev busy timeout, can't send data\n\r", __func__);
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
			goto exit;
		}

		tr = list_first_entry(&spimsg->transfers, struct spi_transfer, transfer_list);
		tr->rx_buf = pskb->data;
		tr->len = SPI_BUFSZ;

		rc = spi_sync(spidev, spimsg);
		if (rc) {
			dev_err(&spidev->dev, "could not transfer : %d\n", rc);
			kfree_skb(pskb);

			goto exit;
		}

		//print_hex_dump_bytes("rtw_spi_recv_data_process: ", DUMP_PREFIX_NONE, tr->rx_buf, tr->len);

		/* process rx msg */
		priv->rx_process_func(pskb);
	}

exit:
	mutex_unlock(&priv->lock);

	if (spimsg) {
		spi_message_free(spimsg);
	}
}

struct hci_ops_t spi_intf_ops = {
	.send_data = rtw_spi_send_data,
	.recv_data_process = rtw_spi_recv_data_process,
};

