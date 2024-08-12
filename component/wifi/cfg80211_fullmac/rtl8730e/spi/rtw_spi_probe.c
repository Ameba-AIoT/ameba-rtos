#include <rtw_cfg80211_fullmac.h>

struct inic_spi inic_spi_priv = {0};

static irqreturn_t rtw_spi_rx_req_handler(int irq, void *context)
{
	if (global_idev.recv_priv.initialized == false) {
		goto exit;
	}

	llhw_recv_notify();

exit:
	return IRQ_HANDLED;
}

static irqreturn_t rtw_spi_dev_rdy_handler(int irq, void *context)
{
	if (gpio_get_value(DEV_READY_PIN)) {
		inic_spi_priv.dev_state = DEV_READY;

		/* wakeup wait task */
		up(&inic_spi_priv.dev_rdy_sema);
	} else {
		inic_spi_priv.dev_state = DEV_BUSY;
	}
	return IRQ_HANDLED;
}

int rtw_spi_setup_gpio(struct spi_device *spi)
{
	int status = 0;

	status = gpio_request(DEV_READY_PIN, "DEV_READY_PIN");
	if (status) {
		return status;
	}

	status = gpio_direction_input(DEV_READY_PIN);
	if (status) {
		goto free_dev_rdy_pin;
	}

	status = request_irq(DEV_READY_IRQ, rtw_spi_dev_rdy_handler,
						 IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						 "SPI_DEV_READY", spi);
	if (status) {
		goto free_dev_rdy_pin;
	}

	status = gpio_request(RX_REQ_PIN, "RX_REQ_PIN");
	if (status) {
		goto free_dev_rdy_irq;
	}

	status = gpio_direction_input(RX_REQ_PIN);
	if (status) {
		goto free_rx_req_pin;
	}

	status = request_irq(RX_REQ_IRQ, rtw_spi_rx_req_handler,
						 IRQF_SHARED | IRQF_TRIGGER_RISING,
						 "SPI_RX_REQ", spi);
	if (status) {
		goto free_rx_req_pin;
	}

#ifdef SPI_DEBUG
	gpio_request(DEBUG_PIN, "DEBUG_PIN");
	gpio_direction_output(DEBUG_PIN, 0);
#endif
	return status;

free_rx_req_pin:
	gpio_free(RX_REQ_PIN);

free_dev_rdy_irq:
	free_irq(DEV_READY_IRQ, spi);

free_dev_rdy_pin:
	gpio_free(DEV_READY_PIN);

	return status;
}

static int rtw_spi_probe(struct spi_device *spi)
{
	struct inic_spi *priv = &inic_spi_priv;
	int rc = 0;

	dev_info(&spi->dev, "%s\n", __func__);

	priv->spi_dev = spi;
	priv->rx_process_func = llhw_recv_process;

	mutex_init(&(priv->lock));
	sema_init(&priv->dev_rdy_sema, 0);

	/* Setup SPI parameters */
	dev_info(&spi->dev, "setup mode: %d, %u bits/w, %u Hz max\n",
			 (int)(spi->mode & (SPI_CPOL | SPI_CPHA)), spi->bits_per_word,
			 spi->max_speed_hz);
	dev_info(&spi->dev, "can_dma: %d\n", spi->master->can_dma != NULL);

	rc = spi_setup(spi);
	if (rc != 0) {
		goto exit;
	}

	/* setupGPIO pin */
	rc = rtw_spi_setup_gpio(spi);
	if (rc) {
		dev_err(&spi->dev, "fail to setup gpio\n");
	}

	if (gpio_get_value(DEV_READY_PIN)) {
		inic_spi_priv.dev_state = DEV_READY;
		dev_info(&spi->dev, "dev ready\n");
	} else {
		inic_spi_priv.dev_state = DEV_BUSY;
		dev_info(&spi->dev, "dev busy\n");
	}

	rtw_netdev_probe(&spi->dev);

exit:
	return rc;
}

static void rtw_spi_remove(struct spi_device *spi)
{
	struct inic_spi *priv = &inic_spi_priv;

	dev_dbg(&spi->dev, "%s\n", __func__);

	rtw_netdev_remove(&spi->dev);

	mutex_destroy(&(priv->lock));

	free_irq(DEV_READY_IRQ, spi);
	free_irq(RX_REQ_IRQ, spi);

	gpio_free(DEV_READY_PIN);
	gpio_free(RX_REQ_PIN);
#ifdef SPI_DEBUG
	gpio_free(DEBUG_PIN);
#endif

	return;
}

static const struct of_device_id inic_of_ids[] = {
	{ .compatible = "realtek,inic", .data = NULL },
	{},
};
MODULE_DEVICE_TABLE(of, inic_of_ids);

static const struct spi_device_id inic_spi_ids[] = {
	{ "inic", 0 },
	{},
};
MODULE_DEVICE_TABLE(spi, inic_spi_ids);

static struct spi_driver inic_spi_driver = {
	.driver = {
		.name = "inic",
		.of_match_table = of_match_ptr(inic_of_ids),
	},
	.id_table = inic_spi_ids,
	.probe = rtw_spi_probe,
	.remove = rtw_spi_remove,
};
module_spi_driver(inic_spi_driver);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek iNIC Fullmac");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");

