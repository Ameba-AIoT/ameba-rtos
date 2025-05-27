#include <whc_host_linux.h>

struct whc_sdio whc_sdio_priv = {0};
extern u32 rtw_sdio_enable_func(struct whc_sdio *priv);

static int whc_sdio_host_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	struct whc_sdio *priv = &whc_sdio_priv;
	int status = false;

	dev_info(&func->dev, "whc_sdio_host_probe: vendor=0x%04x device=0x%04x class=0x%02x\n", func->vendor, func->device, func->class);

	mutex_init(&(priv->lock));
	init_waitqueue_head(&priv->txbd_wq);

	priv->func = func;
	priv->rx_process_func = whc_host_recv_process;
	atomic_set(&priv->continual_io_error, 0);

	sdio_set_drvdata(func, (void *)priv);

	if (rtw_sdio_init(priv) != true) {
		dev_err(&priv->func->dev, "%s: initialize SDIO Failed!\n", __FUNCTION__);
		goto exit;
	}

	rtw_netdev_probe(&func->dev);
#ifdef CONFIG_BT_INIC
	bt_dev_probe(&func->dev);
#endif
	status = true;

exit:
	if (status != true) {
		sdio_set_drvdata(func, NULL);
		rtw_sdio_deinit(priv);
	}

	return status == true ? 0 : -ENODEV;
}

static void whc_sdio_host_remove(struct sdio_func *func)
{
	int err;
	struct whc_sdio *priv = (struct whc_sdio *) sdio_get_drvdata(func);

	dev_info(&func->dev, "whc_sdio_host_remove: vendor=0x%04x device=0x%04x class=0x%02x\n", func->vendor, func->device, func->class);
#ifdef CONFIG_BT_INIC
	bt_dev_remove(&func->dev);
#endif
	rtw_netdev_remove(&func->dev);

	if (priv->bSurpriseRemoved == false) {

		/* test surprise remove */
		sdio_claim_host(func);
		sdio_readb(func, 0, &err);
		sdio_release_host(func);
		if (err == -ENOMEDIUM) {
			priv->bSurpriseRemoved = true;
			dev_info(&priv->func->dev, "%s: device had been removed!\n", __func__);
		}
	}

	sdio_set_drvdata(func, NULL);
	rtw_sdio_deinit(priv);
}

u8 whc_sdio_host_rpwm_notify(struct whc_sdio *priv, enum RPWM2_EVENT event)
{
	u16 rpwm2;
	u32 i = 0;
	u8 target_cpu_rdy_bit;
	u8 old_state = priv->dev_state;
	u8 ret = true, fw_ready;

#ifdef CONFIG_AMEBADPLUS
	/* toggele bit inverse to trigger a RPWM int in DP */
	rpwm2 = rtw_read16(priv, SDIO_REG_HRPWM2) & RPWM2_TOGGLE_BIT;	// get the toggle bit
	/* inverse toggle bit */
	rpwm2 = rpwm2 ^ RPWM2_TOGGLE_BIT;
#else
	/* host write 1 to trigger interrupt to device, cleared by device */
	rpwm2 = RPWM2_TOGGLE_BIT;
#endif

	switch (event) {
	case RPWM2_PWR_SUSPEND:
		if (priv->dev_state == PWR_STATE_SLEEP) {
			dev_dbg(&priv->func->dev, "Device is already in SLEEP state!\n");
			goto exit;
		} else {
			rpwm2 |= RPWM2_CG_BIT;
			priv->dev_state = PWR_STATE_SLEEP;
			target_cpu_rdy_bit = 0;
			dev_dbg(&priv->func->dev, "Device is in ACTIVE state, suspend\n");
		}
		break;

	case RPWM2_PWR_RESUME:
		if (priv->dev_state == PWR_STATE_ACTIVE) {
			dev_dbg(&priv->func->dev, "Device is already in ACTIVE state!\n");
			goto exit;
		} else {
			rpwm2 |= RPWM2_ACT_BIT;
			priv->dev_state = PWR_STATE_ACTIVE;
			target_cpu_rdy_bit = SDIO_SYSTEM_TRX_RDY_IND;
			dev_dbg(&priv->func->dev, "Device is in SLEEP state, resume\n");
		}
		break;

	default:
		dev_err(&priv->func->dev, "unknown rpwm event: %d\n", event);
		ret = false;
		goto exit;
	}

	rtw_write16(priv, SDIO_REG_HRPWM2, rpwm2);

	/* wait for device response */
	for (i = 0; i < 1000; i++) {
		fw_ready = rtw_read8(priv, SDIO_REG_CPU_IND);
		if ((fw_ready & SDIO_SYSTEM_TRX_RDY_IND) == target_cpu_rdy_bit) {
			break;
		}
		msleep(1);
	}
	if (i == 100) {
		priv->dev_state = old_state;
		dev_err(&priv->func->dev, "wait for device response timeout, restore dev state to %s\n", old_state ? "SLEEP" : "ACTIVE");
		ret = false;
		goto exit;
	}

exit:
	return ret;
}

int whc_sdio_host_suspend_common(struct whc_sdio *priv)
{
#ifndef CONFIG_WHC_BRIDGE
	/* staion mode */
	if (whc_fullmac_host_wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) {
		/* update ip address success */
		if (whc_fullmac_host_update_ip_addr()) {
			return -EPERM;
		}
	}

	/* set wowlan_state, stop schedule rx/tx work */
	global_idev.wowlan_state = 1;
	netif_tx_stop_all_queues(global_idev.pndev[0]);
#endif

	/* suspend device */
	if (!whc_sdio_host_rpwm_notify(priv, RPWM2_PWR_SUSPEND)) {
		return -EPERM;
	}
	return 0;
}

int whc_sdio_host_suspend(struct device *dev)
{
	struct sdio_func *func = container_of(dev, struct sdio_func, dev);
	struct whc_sdio *priv = &whc_sdio_priv;

	dev_dbg(dev, "%s", __func__);

	if (global_idev.pndev[1] && rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		/* AP is up, stop to suspend */
		return -EPERM;
	}

	if (whc_sdio_host_suspend_common(priv)) {
		goto FAIL;
	}

	if (func) {
		mmc_pm_flag_t pm_flag = sdio_get_host_pm_caps(func);
		dev_dbg(dev, "pm_flag:0x%x\n", pm_flag);
		if (!(pm_flag & MMC_PM_KEEP_POWER)) {
			dev_dbg(dev, "%s: can't remain alive while host is suspended\n", sdio_func_id(func));
			return -ENOSYS;
		} else {
			sdio_set_host_pm_flags(func,  MMC_PM_WAKE_SDIO_IRQ); //MMC_PM_KEEP_POWER
			dev_dbg(dev, "suspend with MMC_PM_WAKE_SDIO_IRQ\n");
		}
#ifndef SDIO_HOST_FAKE_SLEEP
		/* release irq */
		if (priv->irq_alloc) {
			sdio_claim_host(func);
			if (sdio_release_irq(func)) {
				dev_err(dev, "%s: sdio_release_irq fail\n", __func__);
			} else {
				priv->irq_alloc = 0;
			}
			sdio_release_host(func);
		}
#endif
	}

	return 0;

FAIL:
	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);
	global_idev.wowlan_state = 0;

	return -EPERM;
}

int whc_sdio_host_resume_common(struct whc_sdio *priv)
{
	/* wakeup device */
	if (!whc_sdio_host_rpwm_notify(priv, RPWM2_PWR_RESUME)) {
		dev_err(&priv->func->dev, "%s: wakeup device FAIL!\n", __func__);
		return -EPERM;
	}

#ifndef CONFIG_WHC_BRIDGE
	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	global_idev.wowlan_state = 0;
#endif

	return 0;
}

int whc_sdio_host_resume(struct device *dev)
{
	int ret;
	struct sdio_func *func = container_of(dev, struct sdio_func, dev);
	struct whc_sdio *priv = &whc_sdio_priv;
	u32 himr;
	u8 value;

	dev_dbg(dev, "%s", __func__);

	/* some sdio local registers and CCCR would be reset after kernel resume from suspend */
	/* enable func and set block size */
	if (rtw_sdio_enable_func(priv) == false) {
		return false;
	}

	value = rtw_read8(priv, SDIO_REG_TX_CTRL) | SDIO_EN_HISR_MASK_TIMER;
	rtw_write8(priv, SDIO_REG_TX_CTRL, value);

	rtw_write16(priv, SDIO_REG_STATIS_RECOVERY_TIMOUT, 0x10); //500us

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	rtw_sdio_init_txavailbd_threshold(priv);
#endif

#ifdef CONFIG_SDIO_RX_AGGREGATION
	if (rtw_sdio_init_agg_setting(priv) == false) {
		return -EPERM;
	}
#endif
	rtw_sdio_query_txbd_status(priv);

	/* alloc irq */
	rtw_sdio_alloc_irq(priv);

	/* Enable interrupt */
	himr = cpu_to_le32(priv->sdio_himr);
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

	/* Wakeup device */
	if (whc_sdio_host_resume_common(priv)) {
		return -EPERM;
	}

	return 0;
}

struct dev_pm_ops whc_sdio_host_pm_ops = {
	.suspend = whc_sdio_host_suspend,
	.resume = whc_sdio_host_resume,
};

static const struct sdio_device_id whc_sdio_host_ids[] = {
	{ SDIO_DEVICE(0x024c, 0x8006), .driver_data = 0, .class = SDIO_CLASS_WLAN},	//green2
	{ SDIO_DEVICE(0x024c, 0x8722), .driver_data = 0},	//Dplus
};

static struct sdio_driver whc_sdio_host_driver = {
	.probe	= whc_sdio_host_probe,
	.remove	= whc_sdio_host_remove,
	.name	= "INIC_SDIO",
	.id_table	= whc_sdio_host_ids,
	.drv = {
		.pm = &whc_sdio_host_pm_ops,
	},
};

static int __init whc_sdio_host_init_module(void)
{
	int ret = 0;

	printk("%s\n", __func__);

	ret = sdio_register_driver(&whc_sdio_host_driver);
	if (ret != 0) {
		printk("sdio register driver Failed!\n");
	}

	rtw_inetaddr_notifier_register();

	return ret;
}

static void __exit whc_sdio_host_cleanup_module(void)
{
	rtw_inetaddr_notifier_unregister();

	sdio_unregister_driver(&whc_sdio_host_driver);

	printk("%s\n", __func__);
}

module_init(whc_sdio_host_init_module);
module_exit(whc_sdio_host_cleanup_module);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek iNIC Fullmac");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");

