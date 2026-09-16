// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2024 Realtek Corporation. All rights reserved.
 *
 * GSPI host bus glue: an SPI master plus a DEV_INT GPIO. The device pulls
 * DEV_INT low for both "RX ready" and "TX BD available"; the threaded IRQ
 * handler reads HISR to tell them apart. All register/FIFO access sleeps
 * (spi_sync), so the handler must be threaded.
 */
#include <whc_host_linux.h>

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>

struct whc_gspi whc_gspi_priv = {0};

static irqreturn_t whc_gspi_host_dev_irq_fn(int irq, void *context)
{
	struct whc_gspi *priv = &whc_gspi_priv;
	u32 value, himr;

	/* read HISR (32-bit) */
	priv->gspi_hisr = rtw_read32(priv, GSPI_REG_HISR);

	if (priv->gspi_hisr & priv->gspi_himr) {
		priv->gspi_hisr &= priv->gspi_himr;

		/* clear the write-1-to-clear status bits we handle */
		value = priv->gspi_hisr & MASK_GSPI_HISR_CLEAR;
		if (value) {
			gspi_write32(priv, GSPI_REG_HISR, value);
		}

#ifdef CONFIG_GSPI_TX_ENABLE_AVAL_INT
		if (priv->gspi_hisr & GSPI_HISR_TXBD_AVAL_INT) {
#ifdef CONFIG_AMEBADPLUS
			/* DP: read FREE_TXBD_NUM to clear the aval int */
			(void)rtw_read32(priv, GSPI_REG_FREE_TXBD_NUM);
#else
			/* green2 and later: reading TXBD never clears it, W1C instead */
			rtw_write32(priv, GSPI_REG_HISR, GSPI_HISR_TXBD_AVAL_INT);
#endif
			/* wake the TX waiter */
			priv->tx_avail_int_triggered = 1;
			wake_up(&priv->txbd_wq);
		}
#endif

		if (priv->gspi_hisr & GSPI_HISR_RX_REQUEST) {
			priv->gspi_hisr ^= GSPI_HISR_RX_REQUEST;

			/* mask RX_REQUEST until the recv path drains the FIFO */
			himr = priv->gspi_himr & (~GSPI_HIMR_RX_REQUEST_MSK);
			gspi_write32(priv, GSPI_REG_HIMR, himr);

			if (priv->rx_recv_notify) {
				priv->rx_recv_notify();
			}
		}
	}

	return IRQ_HANDLED;
}

static int whc_gspi_host_setup_gpio(struct whc_gspi *priv)
{
	struct spi_device *spi = priv->spi_dev;
	int status;

	status = gpio_request(DEV_INT_PIN, "DEV_INT_PIN");
	if (status) {
		dev_err(&spi->dev, "%s: gpio_request(%d) FAIL(%d)\n", __func__, DEV_INT_PIN, status);
		return status;
	}

	status = gpio_direction_input(DEV_INT_PIN);
	if (status) {
		goto free_dev_int_pin;
	}

	status = request_threaded_irq(DEV_INT_IRQ, NULL, whc_gspi_host_dev_irq_fn,
								  IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
								  "GSPI_DEV_INT_IRQ", priv);
	if (status) {
		dev_err(&spi->dev, "%s: request_irq FAIL(%d)\n", __func__, status);
		goto free_dev_int_pin;
	}

	priv->irq_alloc = 1;
	return 0;

free_dev_int_pin:
	gpio_free(DEV_INT_PIN);
	return status;
}

static int whc_gspi_host_probe(struct spi_device *spi)
{
	struct whc_gspi *priv = &whc_gspi_priv;
	int rc = 0;
#if (KERNEL_VERSION(5, 5, 0) <= LINUX_VERSION_CODE)
	struct spi_delay cs_setup_time = {
		.unit = SPI_DELAY_UNIT_USECS,
		.value = 7
	};
#endif

	dev_info(&spi->dev, "%s\n", __func__);

	priv->spi_dev = spi;
	mutex_init(&priv->lock);
	init_waitqueue_head(&priv->txbd_wq);

#if (KERNEL_VERSION(5, 15, 0) <= LINUX_VERSION_CODE)
	spi->cs_setup = cs_setup_time;
#elif (KERNEL_VERSION(5, 5, 0) <= LINUX_VERSION_CODE)
	spi->controller->cs_setup = cs_setup_time;
#endif

	dev_info(&spi->dev, "setup mode: %d, %u bits/w, %u Hz max\n",
			 (int)(spi->mode & (SPI_CPOL | SPI_CPHA)), spi->bits_per_word, spi->max_speed_hz);

	rc = spi_setup(spi);
	if (rc != 0) {
		dev_err(&spi->dev, "spi_setup FAIL(%d)\n", rc);
		goto err_setup;
	}

	rc = whc_gspi_host_setup_gpio(priv);
	if (rc) {
		goto err_setup;
	}

	if (rtw_gspi_init(priv) != true) {
		dev_err(&spi->dev, "%s: initialize GSPI Failed!\n", __func__);
		rc = -ENODEV;
		goto err_init;
	}

	rtw_netdev_probe(&spi->dev);
#ifdef CONFIG_BT_INIC
	bt_dev_probe(&spi->dev);
#endif

	return 0;

err_init:
	if (priv->irq_alloc) {
		free_irq(DEV_INT_IRQ, priv);
		gpio_free(DEV_INT_PIN);
		priv->irq_alloc = 0;
	}
err_setup:
	mutex_destroy(&priv->lock);
	return rc;
}

#if (KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE)
static void whc_gspi_host_remove(struct spi_device *spi)
#else
static int whc_gspi_host_remove(struct spi_device *spi)
#endif
{
	struct whc_gspi *priv = &whc_gspi_priv;

	dev_dbg(&spi->dev, "%s\n", __func__);

#ifdef CONFIG_BT_INIC
	bt_dev_remove(&spi->dev);
#endif
	rtw_netdev_remove(&spi->dev);

	rtw_gspi_deinit(priv);

	if (priv->irq_alloc) {
		free_irq(DEV_INT_IRQ, priv);
		gpio_free(DEV_INT_PIN);
		priv->irq_alloc = 0;
	}

	mutex_destroy(&priv->lock);

#if (KERNEL_VERSION(5, 18, 0) <= LINUX_VERSION_CODE)
	return;
#else
	return 0;
#endif
}

static const struct of_device_id whc_gspi_of_ids[] = {
	{ .compatible = "realtek,inic", .data = NULL },
	{},
};
MODULE_DEVICE_TABLE(of, whc_gspi_of_ids);

static const struct spi_device_id whc_gspi_ids[] = {
	{ "inic", 0 },
	{},
};
MODULE_DEVICE_TABLE(spi, whc_gspi_ids);

static struct spi_driver whc_gspi_host_driver = {
	.driver = {
		.name = "WHC_GSPI",
		.of_match_table = of_match_ptr(whc_gspi_of_ids),
	},
	.id_table = whc_gspi_ids,
	.probe = whc_gspi_host_probe,
	.remove = whc_gspi_host_remove,
};

static int __init whc_gspi_host_init_module(void)
{
	int ret;

	ret = spi_register_driver(&whc_gspi_host_driver);
	if (ret) {
		pr_err("%s: spi_register_driver FAIL(%d)\n", __func__, ret);
		return ret;
	}

	rtw_inetaddr_notifier_register();

	return ret;
}

static void __exit whc_gspi_host_cleanup_module(void)
{
	rtw_inetaddr_notifier_unregister();
	spi_unregister_driver(&whc_gspi_host_driver);
}

module_init(whc_gspi_host_init_module);
module_exit(whc_gspi_host_cleanup_module);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek Ameba WHC");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
