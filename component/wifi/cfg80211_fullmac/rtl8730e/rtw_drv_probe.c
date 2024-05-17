// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>

/* Match table for of_platform binding */
static const struct of_device_id rtw_axi_of_match[] = {
	{ .compatible = "realtek,rtl8730e_nic", },
	{},
};

MODULE_DEVICE_TABLE(of, rtw_axi_of_match);

struct axi_data *paxi_data_global;

/* IPv4, IPv6 IP addr notifier */
static int rtw_inetaddr_notifier_call(struct notifier_block *nb, unsigned long action, void *data)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)data;
	struct net_device *ndev;

	if (!ifa || !ifa->ifa_dev || !ifa->ifa_dev->dev) {
		return NOTIFY_DONE;
	}

	ndev = ifa->ifa_dev->dev;

	switch (action) {
	case NETDEV_UP:
		memcpy(global_idev.ip_addr, &ifa->ifa_address, RTW_IP_ADDR_LEN);
		dev_dbg(global_idev.fullmac_dev, "%s[%s]: up IP: [%pI4]\n", __func__, ifa->ifa_label, global_idev.ip_addr);
		break;
	case NETDEV_DOWN:
		memset(global_idev.ip_addr, 0, RTW_IP_ADDR_LEN);
		dev_dbg(global_idev.fullmac_dev, "%s[%s]: down IP: [%pI4]\n", __func__, ifa->ifa_label, global_idev.ip_addr);
		break;
	default:
		dev_dbg(global_idev.fullmac_dev, "%s: default action\n", __func__);
		break;
	}
	return NOTIFY_DONE;
}

#ifdef CONFIG_IPV6
static int rtw_inet6addr_notifier_call(struct notifier_block *nb, unsigned long action, void *data)
{
	struct inet6_ifaddr *inet6_ifa = (struct inet6_ifaddr *)data;
	struct net_device *ndev;

	if (!inet6_ifa || !inet6_ifa->idev || !inet6_ifa->idev->dev) {
		return NOTIFY_DONE;
	}

	ndev = inet6_ifa->idev->dev;

	switch (action) {
	case NETDEV_UP:
		memcpy(global_idev.ipv6_addr, &inet6_ifa->addr, RTW_IPv6_ADDR_LEN);
		dev_dbg(global_idev.fullmac_dev, "%s: up IP: [%pI6]\n", __func__, global_idev.ipv6_addr);
		break;
	case NETDEV_DOWN:
		memset(global_idev.ipv6_addr, 0, RTW_IPv6_ADDR_LEN);
		dev_dbg(global_idev.fullmac_dev, "%s: down IP: [%pI6]\n", __func__, global_idev.ipv6_addr);
		break;
	default:
		dev_dbg(global_idev.fullmac_dev, "%s: default action\n", __func__);
		break;
	}
	return NOTIFY_DONE;
}
#endif

static struct notifier_block rtw_inetaddr_notifier = {
	.notifier_call = rtw_inetaddr_notifier_call
};

#ifdef CONFIG_IPV6
static struct notifier_block rtw_inet6addr_notifier = {
	.notifier_call = rtw_inet6addr_notifier_call
};
#endif

void rtw_inetaddr_notifier_register(void)
{
	register_inetaddr_notifier(&rtw_inetaddr_notifier);
#ifdef CONFIG_IPV6
	register_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

void rtw_inetaddr_notifier_unregister(void)
{
	unregister_inetaddr_notifier(&rtw_inetaddr_notifier);
#ifdef CONFIG_IPV6
	unregister_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

int rtw_netdev_probe(struct device *pdev)
{
	int ret = false;

	memset(&global_idev, 0, sizeof(struct inic_device));

	/* Initialize axi_priv */
	global_idev.fullmac_dev = pdev;

	dev_dbg(global_idev.fullmac_dev, "rtw_dev_probe start\n");

	/*step1: alloc and init wiphy */
	ret = rtw_wiphy_init();
	if (ret == false) {
		dev_err(global_idev.fullmac_dev, "wiphy init fail");
		goto exit;
	}

	/*step3: register wiphy */
	if (wiphy_register(global_idev.pwiphy_global) != 0) {
		dev_err(global_idev.fullmac_dev, "wiphy register fail");
		goto os_ndevs_deinit;
	}

	/*step4: register netdev */
	ret = rtw_ndev_alloc();
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "ndev alloc fail");
		goto os_ndevs_deinit;
	}

	ret = llhw_init();
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "llhw init fail");
		goto os_ndevs_deinit;
	}

	ret = rtw_ndev_register();
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "ndev register fail");
		goto os_ndevs_deinit;
	}

	rtw_regd_init();
	rtw_drv_proc_init();

#ifdef CONFIG_WAR_OFFLOAD
	rtw_proxy_init();
#endif

	return 0; /* probe success */

os_ndevs_deinit:
	rtw_ndev_unregister();
	rtw_wiphy_deinit();

exit:

	return -ENODEV;
}

int rtw_netdev_remove(struct device *pdev)
{
	dev_dbg(global_idev.fullmac_dev, "%s start.", __func__);

	rtw_ndev_unregister();
	dev_dbg(global_idev.fullmac_dev, "unregister netdev done.");

	rtw_regd_deinit();
	wiphy_unregister(global_idev.pwiphy_global);

	rtw_wiphy_deinit();
	dev_dbg(global_idev.fullmac_dev, "unregister and deinit wiphy done.");

	llhw_deinit();
	dev_dbg(global_idev.fullmac_dev, "remove llhw done.");

	rtw_drv_proc_deinit();

	pr_info("%s done\n", __func__);
	memset(&global_idev, 0, sizeof(struct inic_device));

	return 0;
}

#ifdef CONFIG_FULLMAC_HCI_IPC

static void platform_device_init(struct platform_device *pdev)
{
	u32 status = false;
	struct resource *res_mem = NULL;
	struct device_node *sys_node = NULL, *ocp_node = NULL;
	unsigned long pmem_len = 0;
	struct resource res_sys = {0};
	struct axi_data *axi_data;

	/* TODO: axi_data useless in fullmac, clear later. */
	axi_data = (struct axi_data *)kzalloc(sizeof(struct axi_data), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (!axi_data) {
		pr_err("Can't get axi_data\n");
		goto exit;
	}
	paxi_data_global = axi_data;

	ocp_node = of_get_parent(pdev->dev.of_node);
	sys_node = of_get_compatible_child(ocp_node, "realtek,amebad2-system-ctrl-ls");
	if (!sys_node) {
		pr_err("Can't get sys_node\n");
		goto free_dvobj;
	}

	axi_data->pdev = pdev;
	//platform_set_drvdata(pdev, dvobj); //LINUX_TODO

	/* TODO: SYS REG map, useless here. */
	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res_mem) {
		pr_err("Can't get axi IORESOURCE_MEM\n");
		goto free_dvobj;
	}

	pmem_len = res_mem->end - res_mem->start + 1;

	/* TODO: MAC REG map, useless here. */
	axi_data->axi_mem_start = (unsigned long)devm_ioremap_resource(&pdev->dev, res_mem);
	if (!axi_data->axi_mem_start) {
		pr_err("Can't map CTRL mem\n");
		goto exit;
	}
	axi_data->axi_mem_end = axi_data->axi_mem_start + pmem_len;

	pr_info("Memory mapped space start: 0x%08lx len:%08lx, after map:0x%08lx\n",
			(unsigned long)res_mem->start, pmem_len, axi_data->axi_mem_start);

	if (of_address_to_resource(sys_node, 0, &res_sys)) {
		pr_err("Can't get sys IORESOURCE_MEM\n");
		goto free_dvobj;
	}

	pmem_len = res_sys.end - res_sys.start + 1;

	axi_data->axi_sys_mem_start = 0;
	axi_data->axi_sys_mem_start = (unsigned long)ioremap(res_sys.start, pmem_len);
	if (!axi_data->axi_sys_mem_start) {
		pr_err("Can't map CTRL mem\n");
		goto exit;
	}
	axi_data->axi_sys_mem_end = axi_data->axi_sys_mem_start + pmem_len;

	pr_info("Memory mapped sys space start: 0x%08lx len:%08lx, after map:0x%08lx\n",
			(unsigned long)res_sys.start, pmem_len, axi_data->axi_sys_mem_start);

	status = true;

free_dvobj:
	if (status != true) {
		platform_set_drvdata(pdev, NULL);
	}
exit:
	return ;
}

static void platform_device_deinit(struct platform_device *pdev)
{
	struct axi_data *axi_data = paxi_data_global;

	platform_set_drvdata(pdev, NULL);
	if (axi_data) {
		if (axi_data->axi_sys_mem_start != 0) {
			iounmap((void *)axi_data->axi_sys_mem_start);
			axi_data->axi_sys_mem_start = 0;
		}

		if (axi_data->axi_mem_start != 0) {
			devm_iounmap(&pdev->dev, (void *)axi_data->axi_mem_start);
			axi_data->axi_mem_start = 0;
		}

		kfree((u8 *)axi_data);
		paxi_data_global = NULL;
	}
}

static int rtw_dev_probe(struct platform_device *pdev)
{
	int err = 0;

	platform_device_init(pdev);

	err = rtw_netdev_probe(&pdev->dev);
	if (err != 0) {
		platform_device_deinit(pdev);
	}

	return err;
}

static int rtw_dev_remove(struct platform_device *pdev)
{
	rtw_netdev_remove(&pdev->dev);
	platform_device_deinit(pdev);

	return 0;
}

static void rtw_dev_shutdown(struct platform_device *pdev)
{
	dev_dbg(global_idev.fullmac_dev, "%s", __func__);
	rtw_dev_remove(pdev);
}

static int rtw_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
	u32 ret = 0;

	dev_dbg(global_idev.fullmac_dev, "%s", __func__);

	if (rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		/* AP is up, stop to suspend */
		return -EPERM;
	}

	/* staion mode */
	if (llhw_wifi_is_connected_to_ap() == 0) {
		/* wowlan */
		ret = llhw_wifi_update_ip_addr_in_wowlan();
		if (ret == 0) {
			/* update ip address success, to suspend */
			/* set wowlan_state, to not schedule rx work */
			global_idev.wowlan_state = 1;
			netif_tx_stop_all_queues(global_idev.pndev[0]);
		} else {
			/* not suspend */
		}
	}

	return ret;
}

static int rtw_dev_resume(struct platform_device *pdev)
{
	dev_dbg(global_idev.fullmac_dev, "%s", __func__);

	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	global_idev.wowlan_state = 0;

	/* wakeup recv work */
	if (!work_pending(&(global_idev.msg_priv.msg_work))) {
		schedule_work(&(global_idev.msg_priv.msg_work));
	}

	return 0;
}

static struct axi_drv_priv axi_drvpriv = {
	.rtw_axi_drv.probe = rtw_dev_probe,
	.rtw_axi_drv.remove = rtw_dev_remove,
	.rtw_axi_drv.shutdown = rtw_dev_shutdown,
	.rtw_axi_drv.suspend = rtw_dev_suspend,
	.rtw_axi_drv.resume = rtw_dev_resume,
	.rtw_axi_drv.driver = {
		.name = "fullmac-8730e",
		.owner = THIS_MODULE,
		.of_match_table = rtw_axi_of_match
	},
};

int __init rtw_drv_entry(void)
{
	int ret = 0;

	rtw_inetaddr_notifier_register();

	ret = platform_driver_register(&axi_drvpriv.rtw_axi_drv);

	if (ret != 0) {
		rtw_inetaddr_notifier_unregister();
		goto exit;
	}

exit:
	pr_info("Fullmac module init ret=%d\n", ret);
	return ret;
}

void __exit rtw_drv_halt(void)
{
	pr_info("Fullmac module exit start\n");
	dev_dbg(global_idev.fullmac_dev, "%s", __func__);
	platform_driver_unregister(&axi_drvpriv.rtw_axi_drv);
	pr_info("Fullmac module exit success\n");
}

module_init(rtw_drv_entry);
module_exit(rtw_drv_halt);
#endif
