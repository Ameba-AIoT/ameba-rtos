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

#if GSPI_PM_TEST
static ssize_t gspi_pm_store(struct device *dev, struct device_attribute *attr,
							 const char *buf, size_t count)
{
	int rc;

	if (count && (buf[0] == '1')) {
		dev_info(dev, "gspi_pm: manual SUSPEND\n");
		rc = whc_gspi_host_suspend(dev);
	} else if (count && (buf[0] == '0')) {
		dev_info(dev, "gspi_pm: manual RESUME\n");
		rc = whc_gspi_host_resume(dev);
	} else {
		return -EINVAL;
	}

	dev_info(dev, "gspi_pm: flow returned %d\n", rc);
	return count;
}
static DEVICE_ATTR_WO(gspi_pm);
#endif /* GSPI_PM_TEST */

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

#if GSPI_PM_TEST
	if (device_create_file(&spi->dev, &dev_attr_gspi_pm)) {
		dev_warn(&spi->dev, "%s: create gspi_pm sysfs FAIL\n", __func__);
	}
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

#if GSPI_PM_TEST
	device_remove_file(&spi->dev, &dev_attr_gspi_pm);
#endif
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

u8 whc_gspi_host_rpwm_notify(struct whc_gspi *priv, enum RPWM2_EVENT event)
{
	u32 i;
	u8 payload;
	u8 target_cpu_rdy;
	u8 old_state = priv->dev_state;
	u8 ret = true;
	u32 cpu_ind;
	u32 hrpwm;

	switch (event) {
	case RPWM2_PWR_SUSPEND:
		if (priv->dev_state == PWR_STATE_SLEEP) {
			goto exit;
		}
		payload = GSPI_HRPWM2_CG_BIT;
		priv->dev_state = PWR_STATE_SLEEP;
		target_cpu_rdy = 0;
		break;

	case RPWM2_PWR_RESUME:
		if (priv->dev_state == PWR_STATE_ACTIVE) {
			goto exit;
		}
		payload = GSPI_HRPWM2_ACT_BIT;
		priv->dev_state = PWR_STATE_ACTIVE;
		target_cpu_rdy = 1;	/* expect CPU_RDY to be set again */
		break;

	default:
		dev_err(&priv->spi_dev->dev, "unknown rpwm event: %d\n", event);
		return false;
	}

	/* build the 32-bit HRPWM word: ACT/CG payload + a trigger edge on the toggle */
	hrpwm = ((u32)payload & GSPI_HRPWM2_PAYLOAD_MSK) << GSPI_HRPWM2_PAYLOAD_SHIFT;
#ifdef CONFIG_AMEBADPLUS
	/* DP: invert the current toggle bit to produce an edge */
	if ((rtw_read32(priv, GSPI_REG_HRPWM) & GSPI_HRPWM2_TOGGLE_W32) == 0) {
		hrpwm |= GSPI_HRPWM2_TOGGLE_W32;
	}
#else
	/* green2 and later: write 1 to trigger, device clears it */
	hrpwm |= GSPI_HRPWM2_TOGGLE_W32;
#endif

	gspi_write32(priv, GSPI_REG_HRPWM, hrpwm);

	/* wait for device response via CPU_RDY indication */
	for (i = 0; i < 1000; i++) {
		cpu_ind = (rtw_read32(priv, GSPI_REG_CPU_INDICATION) & GSPI_CPU_RDY_IND) ? 1 : 0;
		if (cpu_ind == target_cpu_rdy) {
			break;
		}
		msleep(1);
	}
	if (i == 1000) {
		priv->dev_state = old_state;
		dev_err(&priv->spi_dev->dev, "rpwm: wait device timeout, restore state %s\n",
				old_state ? "SLEEP" : "ACTIVE");
		ret = false;
	}

exit:
	return ret;
}

int whc_gspi_host_resume_common(struct whc_gspi *priv)
{
	/* wakeup device */
	if (!whc_gspi_host_rpwm_notify(priv, RPWM2_PWR_RESUME)) {
		dev_err(&priv->spi_dev->dev, "%s: wakeup device FAIL!\n", __func__);
		return -EPERM;
	}

	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	global_idev.wowlan_state = 0;

	return 0;
}

int whc_gspi_host_suspend_common(struct whc_gspi *priv)
{
	/* stop scheduling tx, mark wowlan */
	global_idev.wowlan_state = 1;
	netif_tx_stop_all_queues(global_idev.pndev[0]);

	/* suspend (clock-gate) the device */
	if (!whc_gspi_host_rpwm_notify(priv, RPWM2_PWR_SUSPEND)) {
		return -EPERM;
	}

	return 0;
}

int whc_gspi_host_suspend(struct device *dev)
{
	struct whc_gspi *priv = &whc_gspi_priv;

	dev_dbg(dev, "%s\n", __func__);

	if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		/* AP is up, refuse to suspend */
		return -EPERM;
	}

	if (whc_gspi_host_suspend_common(priv)) {
		goto FAIL;
	}

	return 0;

FAIL:
	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);
	global_idev.wowlan_state = 0;
	return -EPERM;
}

int whc_gspi_host_resume(struct device *dev)
{
	struct whc_gspi *priv = &whc_gspi_priv;

	dev_dbg(dev, "%s\n", __func__);

	/* wakeup device + restart queues */
	if (whc_gspi_host_resume_common(priv)) {
		return -EPERM;
	}

	return 0;
}

static struct dev_pm_ops whc_gspi_host_pm_ops = {
	.suspend = whc_gspi_host_suspend,
	.resume = whc_gspi_host_resume,
};

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
		.pm = &whc_gspi_host_pm_ops,
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
