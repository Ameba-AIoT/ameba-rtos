/*
*
*  Generic Bluetooth SDIO driver
*
*  Copyright (C) 2007  Cambridge Silicon Radio Ltd.
*  Copyright (C) 2007  Marcel Holtmann <marcel@holtmann.org>
*  Copyright (C) 2018  Realtek Semiconductor Corp
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*/

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/timer.h>

#include <linux/mmc/host.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/hci.h>

#include "rtb_sdio.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
#define hci_skb_pkt_type(skb)               bt_cb((skb))->pkt_type
#endif

extern struct btsdio_data *sdiodata;

static const struct sdio_device_id btsdio_table[] = {
	/* Bluetooth dplus SDIO device */
	{ SDIO_DEVICE(0x024c, 0x8722) },

	/* Generic Bluetooth Type-A SDIO device */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_A) },

	/* Generic Bluetooth Type-B SDIO device */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_B) },

	/* Generic Bluetooth AMP controller */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_AMP) },

	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(sdio, btsdio_table);

int btsdio_tx_packet(struct btsdio_data *data, struct sk_buff *skb)
{
	int err = 0;
	u8 reg_val;

	sdio_claim_host(data->func);

	reg_val = sdio_readb(data->func, REG_BT_CTRL, NULL);
	sdio_writeb(data->func, reg_val | 0x1, REG_BT_CTRL, &err); // wake up device, and will also trigger HOST_WAKE_CPU_INT interrupt to Device CPU

	sdio_release_host(data->func);

	BT_INFO("[BT_SDIO] TX data: ");
	for (int i = SIZE_TX_DESC + sizeof(struct whc_api_info); i < skb->len; i++) {
		BT_INFO("0x%02x ", skb->data[i]);
	}

	sdio_claim_host(data->func);
	reg_val = sdio_readb(data->func, REG_FREE_TXBD_NUM, NULL);
	if (!reg_val) {
		BT_INFO("No availabe txbd \n");
		err = -EBUSY;
		goto exit;
	}
	err = sdio_memcpy_toio(data->func, REG_TDAT + ALIGN_UP(skb->len, 4) / 4,
						   skb->data, ALIGN_UP(skb->len, 4));
	if (err) {
		BT_INFO("Couldn't write %u byptes to card error %d", skb->len, err);
		goto exit;
	}
	data->hdev->stat.byte_tx += skb->len;

exit:
	sdio_release_host(data->func);
	kfree_skb(skb);
	return err;
}

static int btsdio_rx_packet(struct btsdio_data *data)
{
	struct sk_buff *skb;
	int err = 0;
	unsigned int rxreq_len;

	BT_DBG("%s", data->hdev->name);

	sdio_claim_host(data->func);

	rxreq_len = sdio_readl(data->func, 0x1c, &err) & 0xFFFF;

	skb = bt_skb_alloc(rxreq_len, GFP_KERNEL);

	if (!skb) {
		BT_ERR("Couldn't alloc skb for rx");
		return -ENOMEM;
	}

	err = sdio_memcpy_fromio(data->func, skb->data, REG_RDAT, rxreq_len);

	sdio_release_host(data->func);

	if (err) {
		BT_ERR("Read payload error %d", err);
		goto exit;
	}

	//skb->len = rxreq_len;
	/*if bt sdio device use spdio API, BT_SDIO_DPLUS must be defined*/
	err = bt_recv_pkts(skb);

exit:
	return err;
}

void btsdio_interrupt(struct sdio_func *func)
{
	struct btsdio_data *data = sdio_get_drvdata(func);
	int err;
	u32 intrd = 0;

	BT_DBG("%s", data->hdev->name);

	BT_INFO("%s: got btsdio interrupt\n", __func__);

	intrd = sdio_readl(func, REG_HISR, &err);

	if (!err && (intrd & 0x01)) {
		sdio_writel(func, intrd, REG_HISR, &err);
		if (btsdio_rx_packet(data) < 0) {
			data->hdev->stat.err_tx++;
		}
	} else {
		BT_ERR("Unknown interrupt, err %d, intrd 0x%08x", err, intrd);
	}
}

int btsdio_enable(struct sdio_func *func)
{
	int err = 0;
	sdio_claim_host(func);

	err = sdio_enable_func(func);
	if (err < 0) {
		goto release;
	}

	err = sdio_claim_irq(func, btsdio_interrupt);
	if (err < 0) {
		sdio_disable_func(func);
		goto release;
	}

release:
	sdio_release_host(func);
	return err;
}

void btsdio_disable(struct sdio_func *func)
{

	sdio_claim_host(func);

	sdio_release_irq(func);
	sdio_disable_func(func);

	sdio_release_host(func);

}

static int btsdio_probe(struct sdio_func *func,
						const struct sdio_device_id *id)
{
	struct sdio_func_tuple *tuple = func->tuples;
	int err;

	BT_DBG("func %p id %p class 0x%04x", func, id, func->class);

	while (tuple) {
		BT_DBG("code 0x%x size %d", tuple->code, tuple->size);
		tuple = tuple->next;
	}

	err = bt_dev_probe(&func->dev);

	sdio_set_drvdata(func, sdiodata);

	return err;
}

static void btsdio_remove(struct sdio_func *func)
{

	sdio_set_drvdata(func, NULL);

	bt_dev_remove(&func->dev);

}

static int btsdio_suspend(struct device *dev)
{
	u8 reg_val;
	struct sdio_func *func = dev_to_sdio_func(dev);

	BT_DBG("func %p", func);

	sdio_claim_host(func);

	reg_val = sdio_readb(func, REG_CPU_IND, NULL);
	sdio_writeb(func, reg_val | 0x2, REG_CPU_IND, NULL);

	sdio_release_host(func);

	return 0;
}

static int btsdio_resume(struct device *dev)
{
	u8 reg_val;
	struct sdio_func *func = dev_to_sdio_func(dev);

	sdio_claim_host(func);

	reg_val = sdio_readb(func, REG_BT_CTRL, NULL);
	sdio_writeb(func, reg_val | 0x1, REG_BT_CTRL, NULL);

	sdio_release_host(func);

	return 0;
}

static const struct dev_pm_ops btsdio_pm_ops = {
	.suspend     = btsdio_suspend,
	.resume      = btsdio_resume,
};

static struct sdio_driver btsdio_driver = {
	.name        = "btstdsdio",
	.probe       = btsdio_probe,
	.remove      = btsdio_remove,
	.id_table    = btsdio_table,
	.drv = {
		.owner = THIS_MODULE,
		.pm = &btsdio_pm_ops,
	}
};

static int __init btsdio_init(void)
{
	BT_INFO("Realtek Bluetooth SDIO driver ver %s", VERSION);
	return sdio_register_driver(&btsdio_driver);
}

static void __exit btsdio_exit(void)
{
	sdio_unregister_driver(&btsdio_driver);
}

module_init(btsdio_init);
module_exit(btsdio_exit);

MODULE_AUTHOR("Realsil tim_liu");
MODULE_DESCRIPTION("Realtek Bluetooth SDIO driver ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");