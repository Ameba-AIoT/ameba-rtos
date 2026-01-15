#include <whc_host_linux.h>

struct whc_spi whc_spi_priv = {0};

static irqreturn_t whc_spi_host_dev_txreq_handler(int irq, void *context)
{
	if (global_idev.recv_priv.initialized == false) {
		goto exit;
	}

	whc_host_recv_notify();

exit:
	return IRQ_HANDLED;
}

static irqreturn_t whc_spi_host_dev_rdy_handler(int irq, void *context)
{
	atomic_set(&whc_spi_priv.dev_state, DEV_READY);
	/* wakeup wait task */
	up(&whc_spi_priv.dev_rdy_sema);

	return IRQ_HANDLED;
}

int whc_spi_host_setup_gpio(struct spi_device *spi)
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

	status = request_irq(DEV_READY_IRQ, whc_spi_host_dev_rdy_handler,
						 IRQF_SHARED | IRQF_TRIGGER_RISING,
						 "SPI_DEV_READY", spi);
	if (status) {
		goto free_dev_rdy_pin;
	}

	status = gpio_request(DEV_TX_REQ_PIN, "DEV_TX_REQ_PIN");
	if (status) {
		goto free_dev_rdy_irq;
	}

	status = gpio_direction_input(DEV_TX_REQ_PIN);
	if (status) {
		goto free_rx_req_pin;
	}

	status = request_irq(DEV_TX_REQ_IRQ, whc_spi_host_dev_txreq_handler,
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
	gpio_free(DEV_TX_REQ_PIN);

free_dev_rdy_irq:
	free_irq(DEV_READY_IRQ, spi);

free_dev_rdy_pin:
	gpio_free(DEV_READY_PIN);

	return status;
}

static int whc_spi_host_probe(struct spi_device *spi)
{
	struct whc_spi *priv = &whc_spi_priv;
	int rc = 0;
#if (KERNEL_VERSION(5, 5, 0) <= LINUX_VERSION_CODE)
	struct spi_delay cs_setup_time = {
		.unit = SPI_DELAY_UNIT_USECS,
		.value = 7
	};
#endif
	dev_info(&spi->dev, "%s\n", __func__);

	priv->spi_dev = spi;
	priv->rx_process_func = whc_host_recv_process;

	mutex_init(&(priv->lock));
	sema_init(&priv->dev_rdy_sema, 0);
#if (KERNEL_VERSION(5, 15, 0) <= LINUX_VERSION_CODE)
	/* Setup SPI parameters */
	spi->cs_setup = cs_setup_time;
#elif (KERNEL_VERSION(5, 5, 0) <= LINUX_VERSION_CODE)
	/* Setup SPI parameters */
	spi->controller->cs_setup = cs_setup_time;
#endif

	dev_info(&spi->dev, "setup mode: %d, %u bits/w, %u Hz max\n",
			 (int)(spi->mode & (SPI_CPOL | SPI_CPHA)), spi->bits_per_word,
			 spi->max_speed_hz);
#if (KERNEL_VERSION(6, 12, 0) <= LINUX_VERSION_CODE)
	dev_info(&spi->dev, "can_dma: %d\n", spi->controller->can_dma != NULL);
#else
	dev_info(&spi->dev, "can_dma: %d\n", spi->master->can_dma != NULL);
#endif

	rc = spi_setup(spi);
	if (rc != 0) {
		goto exit;
	}

	/* setupGPIO pin */
	rc = whc_spi_host_setup_gpio(spi);
	if (rc) {
		dev_err(&spi->dev, "fail to setup gpio\n");
	}

	if (gpio_get_value(DEV_READY_PIN)) {
		atomic_set(&whc_spi_priv.dev_state, DEV_READY);
		dev_info(&spi->dev, "dev ready\n");
	} else {
		atomic_set(&whc_spi_priv.dev_state, DEV_BUSY);
		dev_info(&spi->dev, "dev busy\n");
	}

	rtw_netdev_probe(&spi->dev);
#ifdef CONFIG_BT_INIC
	bt_dev_probe(&spi->dev);
#endif

exit:
	return rc;
}
#if (KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE)
static void whc_spi_host_remove(struct spi_device *spi)
#else
static int whc_spi_host_remove(struct spi_device *spi)
#endif
{
	struct whc_spi *priv = &whc_spi_priv;

	dev_dbg(&spi->dev, "%s\n", __func__);

#ifdef CONFIG_BT_INIC
	bt_dev_remove(&spi->dev);
#endif
	rtw_netdev_remove(&spi->dev);

	mutex_destroy(&(priv->lock));

	free_irq(DEV_READY_IRQ, spi);
	free_irq(DEV_TX_REQ_IRQ, spi);

	gpio_free(DEV_READY_PIN);
	gpio_free(DEV_TX_REQ_PIN);
#ifdef SPI_DEBUG
	gpio_free(DEBUG_PIN);
#endif

#if (KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE)
	return;
#else
	return 0;
#endif
}

static const struct of_device_id whc_of_ids[] = {
	{ .compatible = "realtek,inic", .data = NULL },
	{},
};
MODULE_DEVICE_TABLE(of, whc_of_ids);

static const struct spi_device_id whc_spi_ids[] = {
	{ "inic", 0 },
	{},
};
MODULE_DEVICE_TABLE(spi, whc_spi_ids);

static struct spi_driver whc_spi_host_driver = {
	.driver = {
		.name = "whc",
		.of_match_table = of_match_ptr(whc_of_ids),
	},
	.id_table = whc_spi_ids,
	.probe = whc_spi_host_probe,
	.remove = whc_spi_host_remove,
};
module_spi_driver(whc_spi_host_driver);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek iNIC Fullmac");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");

