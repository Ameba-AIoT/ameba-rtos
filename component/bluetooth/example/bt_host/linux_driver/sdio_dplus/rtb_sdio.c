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

#ifdef CONFIG_BT_INIC
#include "whc_dev_struct.h"
#include "ameba_inic.h"
#include "bt_inic_defs.h"
#endif

struct btsdio_data *sdiodata = NULL;

struct bt_inic_sdio_hdr {
	uint32_t event;
	uint16_t len;
	uint8_t type;
} __attribute__((packed));

#if defined(CONFIG_WHC_HCI_SDIO)
extern void whc_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);
#else
extern int btsdio_tx_packet(struct btsdio_data *data, struct sk_buff *skb);
extern void btsdio_interrupt(struct sdio_func *func);
#endif

#ifndef CONFIG_BT_INIC
extern int btsdio_enable(struct sdio_func *func);
extern void btsdio_disable(struct sdio_func *func);
#endif

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
		skb_put_data(skb, param, plen);
	}

	bt_dev_dbg(hdev, "skb len %d", skb->len);

	hci_skb_pkt_type(skb) = HCI_COMMAND_PKT;
	hci_skb_opcode(skb) = opcode;

	return skb;
}

int bt_send_data(struct btsdio_data *data, struct sk_buff *skb)
{
	BT_DBG("%s", data->hdev->name);

	struct bt_inic_sdio_hdr *hdr;
	int err = 0;
	int pkt_type;
	struct sk_buff *tx_skb = skb;
	u32 tx_len;

	pkt_type = hci_skb_pkt_type(skb);
	tx_len = skb->len;

	/* just in case skb has no reserved buffer */
	if (skb_headroom(skb) < SIZE_TX_DESC + HCI_SDIO_PKT_HDR_LEN) {
		tx_skb = bt_skb_alloc(skb->len + SIZE_TX_DESC + HCI_SDIO_PKT_HDR_LEN, GFP_ATOMIC);
		if (!tx_skb) {
			kfree_skb(skb);
			return -1;
		}

		skb_reserve(tx_skb, SIZE_TX_DESC + HCI_SDIO_PKT_HDR_LEN);
		skb_put_data(tx_skb, skb->data, skb->len);
		kfree_skb(skb);
	}

	/* Prepend Type-A header */
	hdr = (struct bt_inic_sdio_hdr *)skb_push(tx_skb, HCI_SDIO_PKT_HDR_LEN);
	hdr->event = INIC_BT_HOST_TX;
	hdr->len = tx_len + HCI_SDIO_PKT_HDR_LEN;
	hdr->type = pkt_type;

	skb_push(tx_skb, SIZE_TX_DESC);

#if defined(CONFIG_WHC_HCI_SDIO)
	whc_host_send_data(tx_skb->data, tx_skb->len, NULL);
#else
	INIC_TX_DESC *ptxdesc;
	ptxdesc = (INIC_TX_DESC *)tx_skb->data;
	ptxdesc->txpktsize = tx_skb->len - SIZE_TX_DESC;
	ptxdesc->offset = SIZE_TX_DESC;
	ptxdesc->type = 0x00;
	ptxdesc->bus_agg_num = 1;
	err = btsdio_tx_packet(data, tx_skb);
#endif

	data->hdev->stat.byte_tx += tx_skb->len;

	return err;
}

static void btsdio_txwork(struct work_struct *work)
{
	struct btsdio_data *data = container_of(work, struct btsdio_data, work);
	struct sk_buff *skb;
	int err = 0;

	BT_DBG("%s", data->hdev->name);

	while ((skb = skb_dequeue(&data->txq))) {
		err = bt_send_data(data, skb);
		if (err < 0) {
			data->hdev->stat.err_tx++;
			skb_queue_head(&data->txq, skb);
			break;
		}
	}

}

int bt_rx_process(struct btsdio_data *data, struct sk_buff *skb)
{
	unsigned int pkt_len;
	int err = 0;
	u16 opcode;

	struct bt_inic_sdio_hdr *hdr = (struct bt_inic_sdio_hdr *)(skb->data + SIZE_RX_DESC);

	skb->len = SIZE_RX_DESC + hdr->len;

	skb_pull(skb, SIZE_RX_DESC);

	hci_skb_pkt_type(skb) = hdr->type;

	pkt_len = hdr->len;
	skb_trim(skb, pkt_len);
	skb_pull(skb, HCI_SDIO_PKT_HDR_LEN);

	if (hdr->type == HCI_EVENT_PKT && *(skb->data) == 0xe && *(skb->data + 4) == 0xfe) {
		opcode = *(skb->data + 3) | 0xfe00;
		if (opcode == BT_HCI_CMD_VS_BT_ON || opcode == BT_HCI_CMD_VS_BT_OFF) {
			kfree_skb(skb);
			return err;
		}
	}

	data->hdev->stat.byte_rx += skb->len;

	err = hci_recv_frame(data->hdev, skb);

	return err;
}

int bt_recv_pkts(struct sk_buff *skb)
{
	int err = 0;
	if (skb) {
		err = bt_rx_process(sdiodata, skb);
	}
	return err;
}

static int btsdio_open(struct hci_dev *hdev)
{
	BT_INFO("BT SDIO Open Start %s", __func__);
	struct btsdio_data *data = hci_get_drvdata(hdev);
	int err = 0;

	BT_DBG("%s", hdev->name);

#ifndef CONFIG_BT_INIC
	err = btsdio_enable(data->func);

	if (err < 0) {
		return err;
	}
#endif

	struct sk_buff *skb;

	skb = hci_prepare_cmd(hdev, BT_HCI_CMD_VS_BT_ON, 0, NULL);

	err = bt_send_data(data, skb);

	BT_INFO("BT SDIO Open End %s", __func__);

	return err;
}

static int btsdio_close(struct hci_dev *hdev)
{
	BT_INFO("BT SDIO Close Start %s", __func__);
	struct btsdio_data *data = hci_get_drvdata(hdev);

	BT_DBG("%s\n", hdev->name);

	int err = 0;
	struct sk_buff *skb;

	skb = hci_prepare_cmd(hdev, BT_HCI_CMD_VS_BT_OFF, 0, NULL);

	err = bt_send_data(data, skb);

#ifndef CONFIG_BT_INIC
	btsdio_disable(data->func);
#endif

	BT_INFO("BT SDIO Close End %s", __func__);

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
	BT_INFO("send frame %s", __func__);

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

int bt_dev_probe(struct device *pdev)
{
	BT_INFO("BT SDIO Driver Probe Start %s", __func__);
	struct btsdio_data *data;
	struct hci_dev *hdev;
	struct sdio_func *func = dev_to_sdio_func(pdev);
	int err = 0;

	data = devm_kzalloc(pdev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}

	data->func = func;

	INIT_WORK(&data->work, btsdio_txwork);

	skb_queue_head_init(&data->txq);

	hdev = hci_alloc_dev();
	if (!hdev) {
		BT_ERR("Couldn't alloc hdev");
		return -ENOMEM;
	}

	hdev->bus = HCI_SDIO;
	hci_set_drvdata(hdev, data);

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 6, 0)
	hdev->dev_type = HCI_PRIMARY;
#endif

	data->hdev = hdev;

	SET_HCIDEV_DEV(hdev, pdev);

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

	sdiodata = data;

	BT_INFO("BT SDIO Driver Probe End %s", __func__);
	return 0;
}

void bt_dev_remove(struct device *pdev)
{
	BT_INFO("BT SDIO Driver Remove Start %s", __func__);
	struct btsdio_data *data = sdiodata;

	struct hci_dev *hdev;

	if (!data) {
		return;
	}

	cancel_work_sync(&data->work);
	hdev = data->hdev;

	sdiodata = NULL;

	hci_unregister_dev(hdev);

	bt_dev_dbg(hdev, "hci unregister dev ok");

	hci_free_dev(hdev);

	bt_dev_dbg(hdev, "hci free dev ok");

	BT_INFO("BT SDIO Driver Remove End %s", __func__);
}