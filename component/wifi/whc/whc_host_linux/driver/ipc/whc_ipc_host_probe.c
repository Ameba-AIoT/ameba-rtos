// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <whc_host_linux.h>

/* Match table for of_platform binding */
static const struct of_device_id whc_ipc_of_match[] = {
	{ .compatible = "realtek,rtl8730e_nic", },
	{},
};

MODULE_DEVICE_TABLE(of, whc_ipc_of_match);

static void platform_device_init(struct platform_device *pdev)
{
	unsigned long pmem_len = 0;
	struct axi_data *axi_data;

	axi_data = (struct axi_data *)kzalloc(sizeof(struct axi_data), in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (!axi_data) {
		pr_err("Can't get axi_data\n");
		goto exit;
	}
	paxi_data_global = axi_data;
	axi_data->pdev = pdev;

	axi_data->wifi_reg_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!axi_data->wifi_reg_res) {
		pr_err("Can't get axi IORESOURCE_MEM\n");
		goto free_dvobj;
	}

	pmem_len = axi_data->wifi_reg_res->end - axi_data->wifi_reg_res->start + 1;

	axi_data->wifi_reg_start = (unsigned long)devm_ioremap_resource(&pdev->dev, axi_data->wifi_reg_res);
	if (!axi_data->wifi_reg_start) {
		pr_err("Can't map CTRL mem\n");
		goto free_dvobj;
	}
	axi_data->wifi_reg_end = axi_data->wifi_reg_start + pmem_len;

	pr_info("wifi reg mapped space start: 0x%08lx len:%08lx, after map:0x%08lx\n",
			(unsigned long)axi_data->wifi_reg_res->start, pmem_len, axi_data->wifi_reg_start);

	axi_data->fw_mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!axi_data->fw_mem_res) {
		pr_err("Can't get np IORESOURCE_MEM\n");
		goto free_dvobj;
	}

	pmem_len = axi_data->fw_mem_res->end - axi_data->fw_mem_res->start + 1;

	axi_data->fw_mem_start = (unsigned long)devm_ioremap_resource(&pdev->dev, axi_data->fw_mem_res);
	if (!axi_data->fw_mem_start) {
		pr_err("Can't map np mem\n");
		goto free_dvobj;
	}
	axi_data->fw_mem_end = axi_data->fw_mem_start + pmem_len;

	pr_info("fw memory mapped space start: 0x%08lx len:%08lx, after map:0x%08lx\n",
			(unsigned long)axi_data->fw_mem_res->start, pmem_len, axi_data->fw_mem_start);

	return;

free_dvobj:
	kfree((u8 *)axi_data);
	paxi_data_global = NULL;
	platform_set_drvdata(pdev, NULL);

exit:
	return ;
}

static void platform_device_deinit(struct platform_device *pdev)
{
	struct axi_data *axi_data = paxi_data_global;

	platform_set_drvdata(pdev, NULL);
	if (axi_data) {
		kfree((u8 *)axi_data);
		paxi_data_global = NULL;
	}
}

static int whc_ipc_host_probe(struct platform_device *pdev)
{
	int err = 0;

	platform_device_init(pdev);

	err = rtw_netdev_probe(&pdev->dev);
	if (err != 0) {
		platform_device_deinit(pdev);
	}

	return err;
}

#if (KERNEL_VERSION(6, 18, 0) <= LINUX_VERSION_CODE)
static void whc_ipc_host_remove(struct platform_device *pdev)
#else
static int whc_ipc_host_remove(struct platform_device *pdev)
#endif
{
	rtw_netdev_remove(&pdev->dev);
	platform_device_deinit(pdev);

#if (KERNEL_VERSION(6, 18, 0) <= LINUX_VERSION_CODE)
	return;
#else
	return 0;
#endif
}

static void whc_ipc_host_shutdown(struct platform_device *pdev)
{
	dev_dbg(global_idev.pwhc_dev, "%s", __func__);
	whc_ipc_host_remove(pdev);
}

static int whc_ipc_host_suspend(struct platform_device *pdev, pm_message_t state)
{
	u32 ret = 0;

	dev_dbg(global_idev.pwhc_dev, "%s", __func__);

	if (rtw_netdev_priv_is_on(global_idev.pndev[1])) {
		/* AP is up, stop to suspend */
		return -EPERM;
	}

	/* staion mode */
	if (global_idev.mlme_priv.rtw_join_status == RTW_JOINSTATUS_SUCCESS) {
		/* wowlan */
		ret = whc_host_update_ip_addr();
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

static int whc_ipc_host_resume(struct platform_device *pdev)
{
	dev_dbg(global_idev.pwhc_dev, "%s", __func__);

	netif_tx_start_all_queues(global_idev.pndev[0]);
	netif_tx_wake_all_queues(global_idev.pndev[0]);

	global_idev.wowlan_state = 0;

	/* wakeup recv work */
	if (!work_pending(&(global_idev.msg_priv.msg_work))) {
		schedule_work(&(global_idev.msg_priv.msg_work));
	}

	return 0;
}

static struct axi_drv_priv whc_ipc_host_driver = {
	.rtw_axi_drv.probe = whc_ipc_host_probe,
	.rtw_axi_drv.remove = whc_ipc_host_remove,
	.rtw_axi_drv.shutdown = whc_ipc_host_shutdown,
	.rtw_axi_drv.suspend = whc_ipc_host_suspend,
	.rtw_axi_drv.resume = whc_ipc_host_resume,
	.rtw_axi_drv.driver = {
		.name = "fullmac-8730e",
		.owner = THIS_MODULE,
		.of_match_table = whc_ipc_of_match
	},
};

int __init whc_ipc_host_init_module(void)
{
	int ret = 0;

	rtw_inetaddr_notifier_register();

	ret = platform_driver_register(&whc_ipc_host_driver.rtw_axi_drv);

	if (ret != 0) {
		rtw_inetaddr_notifier_unregister();
		goto exit;
	}

exit:
	pr_info("WHC module init ret=%d\n", ret);
	return ret;
}

void __exit whc_ipc_host_cleanup_module(void)
{
	pr_info("WHC module exit start\n");
	dev_dbg(global_idev.pwhc_dev, "%s", __func__);
	platform_driver_unregister(&whc_ipc_host_driver.rtw_axi_drv);
	rtw_inetaddr_notifier_unregister();
	pr_info("WHC module exit success\n");
}

module_init(whc_ipc_host_init_module);
module_exit(whc_ipc_host_cleanup_module);

