/*
 *
 *  Generic Bluetooth SDIO driver
 *
 *  Copyright (C) 2007  Cambridge Silicon Radio Ltd.
 *  Copyright (C) 2007  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2024  Realtek Semiconductor Corp
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

#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/hci.h>

#include "btrtk_sdio.h"

#define    BTCOEX              1

#if defined(BTCOEX) && BTCOEX
#include "rtk_coex.h"
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
#define hci_skb_pkt_type(skb)                   bt_cb((skb))->pkt_type
#define hci_skb_opcode(skb)         bt_cb((skb))->hci.opcode
#endif

static const struct sdio_device_id btsdio_table[] = {
	/* Generic Bluetooth Type-A SDIO device */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_A) },

	/* Generic Bluetooth Type-B SDIO device */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_B) },

	/* Generic Bluetooth AMP controller */
	{ SDIO_DEVICE_CLASS(SDIO_CLASS_BT_AMP) },

	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE(sdio, btsdio_table);

struct sk_buff *hci_prepare_cmd(struct hci_dev *hdev, u16 opcode, u32 plen,
								const void *param)
{
	int len = HCI_COMMAND_HDR_SIZE + plen;
	struct hci_command_hdr *hdr;
	struct sk_buff *skb;

	skb = bt_skb_alloc(len, GFP_ATOMIC);
	if (!skb) {
		return NULL;
	}

	hdr = skb_put(skb, HCI_COMMAND_HDR_SIZE);
	hdr->opcode = cpu_to_le16(opcode);
	hdr->plen   = plen;

	if (plen) {
		memcpy(skb_put(skb, plen), param, plen);
	}

	bt_dev_dbg(hdev, "skb len %d", skb->len);

	hci_skb_pkt_type(skb) = HCI_COMMAND_PKT;
	hci_skb_opcode(skb) = opcode;

	return skb;
}

bool sdio_host_wakeup_CG(struct sdio_func *func)
{
	u8 dev_state;
	u8 reg_val;
	int err = 0;

	dev_state = sdio_readb(func, REG_CPU_IND, NULL);
	if (!dev_state) {
		BT_INFO("Wake up sdio device that maybe in CG Mode");
		reg_val = sdio_readb(func, REG_BT_CTRL, NULL);
		sdio_writeb(func, reg_val | 0x1, REG_BT_CTRL, &err); // wake up device, and will also trigger HOST_WAKE_CPU_INT interrupt to Device CPU
	} else {
		return true;
	}

	/* wait for device response */
	for (int i = 0; i < 100; i++) {
		dev_state = sdio_readb(func, REG_CPU_IND, NULL);
		if (dev_state) {
			return true;
		}
		msleep(1);
	}

	return false;
}

int btsdio_tx_packet(struct btsdio_data *data, struct sk_buff *skb)
{
	u8 *hdr;
	u8 reg_val;

	int err = 0;
	int pkt_type;
	struct sk_buff *txskb = skb;

	pkt_type = hci_skb_pkt_type(skb);

#if defined(BTCOEX) && BTCOEX
	bt_coex_process_tx_frame(pkt_type, skb->data, skb->len);
#endif
	BT_DBG("%s", data->hdev->name);

	/* just in case skb has no reserved buffer */
	if (((unsigned long)skb->data & (BTSDIO_DMA_ALIGN - 1)) || skb_headroom(skb) < HCI_SDIO_PKT_HDR_LEN) {
		if (skb_headroom(skb) < HCI_SDIO_PKT_HDR_LEN) {
			txskb = bt_skb_alloc(skb->len + HCI_SDIO_PKT_HDR_LEN, GFP_ATOMIC);
		} else {
			txskb = bt_skb_alloc(skb->len, GFP_ATOMIC);
		}

		if (!txskb) {
			kfree_skb(skb);
			return -ENOMEM;
		}

		skb_reserve(txskb, HCI_SDIO_PKT_HDR_LEN);
		memcpy(skb_put(txskb, skb->len), skb->data, skb->len);
		hci_skb_pkt_type(txskb) = pkt_type;
		kfree_skb(skb);
	}

	hdr = skb_push(txskb, HCI_SDIO_PKT_HDR_LEN);
	hdr[0] = (txskb->len) & 0xFF;
	hdr[1] = ((txskb->len) >> 8) & 0xFF;
	hdr[2] = ((txskb->len) >> 16) & 0xFF;
	hdr[3] = pkt_type;

	sdio_claim_host(data->func);

	if (!sdio_host_wakeup_CG(data->func)) {
		err = -EPERM;
		goto exit;
	}

	reg_val = sdio_readb(data->func, REG_FREE_TXBD_NUM, NULL);
	if (!reg_val) {
		BT_INFO("No availabe txbd \n");
		err = -EBUSY;
		goto exit;
	}

	// print_hex_dump(KERN_DEBUG, "TX Data: ", DUMP_PREFIX_OFFSET, 16, 1, txskb->data, txskb->len, true);

	err = sdio_memcpy_toio(data->func, REG_TDAT + ALIGN_UP(txskb->len, 4) / 4,
						   txskb->data, ALIGN_UP(txskb->len, 4));
	if (err) {
		BT_INFO("Couldn't write %u byptes to card error %d", txskb->len, err);
		goto exit;
	}

	data->hdev->stat.byte_tx += txskb->len;

exit:

	sdio_release_host(data->func);
	kfree_skb(txskb);

	return err;
}

static int btsdio_rx_packet(struct btsdio_data *data)
{
	struct sk_buff *skb;
	int err = 0;
	u16 opcode;
	unsigned int rxreq_len;
	unsigned int pkt_len;

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

	skb->len = rxreq_len;

	hci_skb_pkt_type(skb) = skb->data[3];
	pkt_len = *((uint16_t *)(skb->data));

	skb_trim(skb, pkt_len);
	skb_pull(skb, HCI_SDIO_PKT_HDR_LEN);

	// BT_DBG("Received packet, length: %d", skb->len);
	// print_hex_dump(KERN_DEBUG, "RX Data: ", DUMP_PREFIX_OFFSET, 16, 1, skb->data, skb->len, true);

#if defined(BTCOEX) && BTCOEX
	bt_coex_process_rx_frame(hci_skb_pkt_type(skb), skb->data, skb->len);
#endif

	data->hdev->stat.byte_rx += skb->len;

	err = hci_recv_frame(data->hdev, skb);

exit:
	return err;
}

void btsdio_interrupt(struct sdio_func *func)
{
	struct btsdio_data *data = sdio_get_drvdata(func);
	int err;
	u32 intrd = 0;

	BT_DBG("%s", data->hdev->name);

	//  BT_INFO("%s: got btsdio interrupt\n", __func__);

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

static int btsdio_open(struct hci_dev *hdev)
{
	BT_INFO("[BT_SDIO] Enter: %s\n", __func__);
	struct btsdio_data *data = hci_get_drvdata(hdev);
	int err = 0;

	u8 reg_val;

	BT_DBG("%s", hdev->name);

	sdio_claim_host(data->func);

	if (!sdio_host_wakeup_CG(data->func)) {
		err = -EPERM;
		BT_ERR("%s, sdio device inative \n", hdev->name);
		goto release;
	}

	err = sdio_enable_func(data->func);
	if (err < 0) {
		goto release;
	}

	err = sdio_claim_irq(data->func, btsdio_interrupt);
	if (err < 0) {
		sdio_disable_func(data->func);
		goto release;
	}

#if defined(BTCOEX) && BTCOEX
	bt_coex_open(hdev);
#endif

release:
	sdio_release_host(data->func);

	return err;
}

static int btsdio_close(struct hci_dev *hdev)
{
	BT_INFO("[BT_SDIO] Enter: %s\n", __func__);
	BT_DBG("%s", hdev->name);

	struct btsdio_data *data = hci_get_drvdata(hdev);
	int err = 0;

	sdio_claim_host(data->func);

	sdio_release_irq(data->func);
	sdio_disable_func(data->func);

	sdio_release_host(data->func);

#if defined(BTCOEX) && BTCOEX
	bt_coex_close();
#endif

	return err;
}

static int btsdio_flush(struct hci_dev *hdev)
{
	BT_INFO("flush %s", __func__);
	struct btsdio_data *data = hci_get_drvdata(hdev);

	BT_DBG("%s", hdev->name);

	skb_queue_purge(&data->txq);

	return 0;
}

static int btsdio_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
{

	struct btsdio_data *data = hci_get_drvdata(hdev);

	BT_DBG("%s", hdev->name);

	switch (hci_skb_pkt_type(skb)) {
	case HCI_COMMAND_PKT:
		hdev->stat.cmd_tx++;
		break;

	case HCI_ACLDATA_PKT:
		hdev->stat.acl_tx++;
		break;

	case HCI_SCODATA_PKT:
		hdev->stat.sco_tx++;
		break;

	default:
		return -EILSEQ;
	}

	skb_queue_tail(&data->txq, skb);

	schedule_work(&data->work);
	return 0;
}

static void btsdio_txwork(struct work_struct *work)
{
	struct btsdio_data *data = container_of(work, struct btsdio_data, work);
	struct sk_buff *skb = NULL;
	int err;

	BT_DBG("%s", data->hdev->name);

	while ((skb = skb_dequeue(&data->txq))) {
		err = btsdio_tx_packet(data, skb);
		if (err < 0) {
			data->hdev->stat.err_tx++;
			skb_queue_head(&data->txq, skb);
			break;
		}
	}

}

static int btsdio_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	struct btsdio_data *data;
	struct hci_dev *hdev;
	struct sdio_func_tuple *tuple = func->tuples;
	int err;

	BT_DBG("func %p id %p class 0x%04x", func, id, func->class);

	while (tuple) {
		BT_DBG("code 0x%x size %d", tuple->code, tuple->size);
		tuple = tuple->next;
	}

	data = devm_kzalloc(&func->dev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}

	data->func = func;

	data->int_count = 0;

	INIT_WORK(&data->work, btsdio_txwork);

	skb_queue_head_init(&data->txq);

	hdev = hci_alloc_dev();
	if (!hdev) {
		BT_ERR("Couldn't alloc hdev");
		err = -ENOMEM;
	}

	hdev->bus = HCI_SDIO;
	hci_set_drvdata(hdev, data);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
	hdev->dev_type = HCI_BREDR;
#elif LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
	hdev->dev_type = HCI_PRIMARY;
#endif
	data->hdev = hdev;

	SET_HCIDEV_DEV(hdev, &func->dev);

	hdev->open     = btsdio_open;
	hdev->close    = btsdio_close;
	hdev->flush    = btsdio_flush;
	hdev->send     = btsdio_send_frame;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
	set_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks);
#endif

	err = hci_register_dev(hdev);
	if (err < 0) {
		BT_ERR("Couldn't register hdev");
		hci_free_dev(hdev);
		return err;
	}

	bt_dev_dbg(hdev, "hci register dev ok");

#if defined(BTCOEX) && BTCOEX
	bt_coex_probe(hdev);
#endif

	sdio_set_drvdata(func, data);

	return 0;
}

static void btsdio_remove(struct sdio_func *func)
{
	struct btsdio_data *data = sdio_get_drvdata(func);
	struct hci_dev *hdev;

	BT_DBG("func %p", func);

	if (!data) {
		return;
	}

	cancel_work_sync(&data->work);
	hdev = data->hdev;

	sdio_set_drvdata(func, NULL);

	hci_unregister_dev(hdev);

	bt_dev_dbg(hdev, "hci unregister dev ok");

	hci_free_dev(hdev);

	bt_dev_dbg(hdev, "hci free dev ok");
}

static int btsdio_suspend(struct device *dev)
{
	u8 reg_val;
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct btsdio_data *data = sdio_get_drvdata(func);

	BT_DBG("func %p", func);

	sdio_claim_host(func);

	reg_val = sdio_readb(func, REG_CPU_IND, NULL);
	sdio_writeb(func, reg_val | 0x2, REG_CPU_IND, NULL);

	sdio_release_host(func);

	return 0;
}

static int btsdio_resume(struct device *dev)
{
	int err;
	u8 reg_val;
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct btsdio_data *data = sdio_get_drvdata(func);

	BT_DBG("func %p", func);

	sdio_claim_host(func);
	err = sdio_enable_func(data->func);
	err = sdio_release_irq(data->func);
	err = sdio_claim_irq(data->func, btsdio_interrupt);
	if (err < 0) {
		return err;
	}
	sdio_release_host(func);

	sdio_claim_host(func);

	reg_val = sdio_readb(func, REG_BT_CTRL, NULL);
	sdio_writeb(func, reg_val | 0x1, REG_BT_CTRL, NULL);

	sdio_release_host(func);

	//hci_resume_dev(data->hdev);

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
#if defined(BTCOEX) && BTCOEX
	bt_coex_init();
#endif
	return sdio_register_driver(&btsdio_driver);
}

static void __exit btsdio_exit(void)
{
	sdio_unregister_driver(&btsdio_driver);
#if defined(BTCOEX) && BTCOEX
	bt_coex_deinit();
#endif
}

module_init(btsdio_init);
module_exit(btsdio_exit);

MODULE_DESCRIPTION("Realtek Bluetooth SDIO driver ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
