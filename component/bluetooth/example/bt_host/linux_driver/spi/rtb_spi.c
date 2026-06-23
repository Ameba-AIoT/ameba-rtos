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

#include <linux/spi/spi.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/hci.h>

#include "rtb_spi.h"

#if defined(CONFIG_WHC_HCI_SPI)
extern void whc_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);
#else
extern struct sk_buff *btspi_send_data(struct sk_buff *skb);
extern struct sk_buff *btspi_recv_data(void);
#endif

struct btspi_data *spidata = NULL;

void bt_recv_pkts(struct sk_buff *skb)
{
	// dev_info(spidata->dev, "%s", __func__);
	if (skb) {
		spin_lock(&spidata->lock);
		skb_queue_tail(&spidata->rxq, skb);
		spin_unlock(&spidata->lock);
	}

	up(&spidata->trx_sema);
}

static void bt_rx_process(struct btspi_data *data, struct sk_buff *skb)
{
	u8 *hdr;
	uint16_t opcode, len;
	u8 type;

	if (!skb) {
		return;
	}

	hdr = skb->data;
	len = hdr[4] | (hdr[5] << 8) | (hdr[6] << 8);
	type = hdr[HCI_SPI_PKT_HDR_LEN - 1];

	/* error packet */
	if (len > skb_end_offset(skb)) {
		kfree_skb(skb);
		return;
	}

	skb_put(skb, len);
	skb_pull(skb, HCI_SPI_PKT_HDR_LEN);
	//dev_info(data->dev, "content: %x %x %x %x %x", skb->data[0], skb->data[1], skb->data[2], skb->data[3], skb->data[4]);

	/* command complete event of vendor command */
	if (type == HCI_EVENT_PKT &&
		*(skb->data) == 0xe &&
		*(skb->data + 4) == 0xfe) {
		opcode = *(skb->data + 3) | 0xfe00;
		if (opcode == BT_HCI_CMD_VS_BT_ON || opcode == BT_HCI_CMD_VS_BT_OFF) {
			data->vendor_status = *(skb->data + 5);
			up(&data->vendor_sema);
			kfree_skb(skb);
			return;
		}
	}

	data->hdev->stat.byte_rx += skb->len;
	hci_skb_pkt_type(skb) = type;
	hci_recv_frame(data->hdev, skb);
}

static void bt_send_data(struct btspi_data *data, struct sk_buff *skb)
{
	struct bt_inic_spi_hdr *hdr;
	struct sk_buff *txskb = skb;
	u8 type;

	if (!skb) {
		return;
	}

	type = hci_skb_pkt_type(skb);

	/* just in case skb has no reserved buffer */
	if (skb_headroom(skb) < HCI_SPI_PKT_HDR_LEN || !IS_ALIGNED((unsigned long)skb->data, 4)) {
		txskb = alloc_skb(skb->len + HCI_SPI_PKT_HDR_LEN, GFP_ATOMIC);
		if (!txskb) {
			kfree_skb(skb);
			return;
		}

		skb_reserve(txskb, HCI_SPI_PKT_HDR_LEN);
		skb_put_data(txskb, skb->data, skb->len);
		kfree_skb(skb);
	} else {

	}

	hdr = (struct bt_inic_spi_hdr *)skb_push(txskb, HCI_SPI_PKT_HDR_LEN);
	hdr->event = INIC_BT_HOST_TX;
	hdr->len = txskb->len;
	hdr->type = type;

#if defined(CONFIG_WHC_HCI_SPI)
	whc_host_send_data(txskb->data, txskb->len, NULL);
#else
	bt_rx_process(data, btspi_send_data(txskb));
#endif

	data->hdev->stat.byte_tx += txskb->len;

}

static struct sk_buff *_alloc_vendor_cmd(u16 opcode, u16 plen, u8 *param)
{
	int len = plen + HCI_COMMAND_HDR_SIZE;
	struct hci_command_hdr *hdr;
	struct sk_buff *skb;

	skb = bt_skb_alloc(len, GFP_ATOMIC);
	if (!skb) {
		return NULL;
	}

	hdr = skb_put(skb, HCI_COMMAND_HDR_SIZE);
	hdr->opcode = cpu_to_le16(opcode);
	hdr->plen = plen;

	if (plen) {
		skb_put_data(skb, param, plen);
	}

	hci_skb_pkt_type(skb) = HCI_COMMAND_PKT;
	hci_skb_opcode(skb) = opcode;

	return skb;
}

static int btspi_send_frame(struct hci_dev *hdev, struct sk_buff *skb);
static int _send_vendor_cmd(struct hci_dev *hdev, u16 opcode, u16 plen, u8 *param, u32 timeout)
{
	struct btspi_data *data = hci_get_drvdata(hdev);
	struct sk_buff *skb;

	skb = _alloc_vendor_cmd(opcode, plen, param);
	if (!skb) {
		dev_info(data->dev, "no memory for vendor command");
		return -ENOMEM;
	}

	btspi_send_frame(hdev, skb);
	// dev_info(data->dev, "%s wait", __func__);
	if (down_timeout(&data->vendor_sema, timeout)) {
		dev_info(data->dev, "%s timeout", __func__);
		return -ETIMEDOUT;
	}
	// dev_info(data->dev, "%s", __func__);

	return data->vendor_status;
}

static int btspi_open(struct hci_dev *hdev)
{
	int err = 0;
	struct btspi_data *data = hci_get_drvdata(hdev);

	dev_info(data->dev, "%s open", hdev->name);

	err = _send_vendor_cmd(hdev, BT_HCI_CMD_VS_BT_ON, 0, NULL, HCI_INIT_TIMEOUT);
	if (err) {
		dev_err(data->dev, "Open controller failed (%d)", err);
	}

	return err;
}

static int btspi_close(struct hci_dev *hdev)
{
	int err = 0;
	struct btspi_data *data = hci_get_drvdata(hdev);

	dev_info(data->dev, "%s close", hdev->name);
	/* send 0xFC02 to power off remote device */
	err = _send_vendor_cmd(hdev, BT_HCI_CMD_VS_BT_OFF, 0, NULL, HCI_CMD_TIMEOUT);
	if (err) {
		dev_err(data->dev, "Close controller failed (%d)", err);
	}

	return err;
}

static int btspi_flush(struct hci_dev *hdev)
{
	struct btspi_data *data = hci_get_drvdata(hdev);

	dev_info(data->dev, "%s flush", hdev->name);

	spin_lock(&(data->lock));
	skb_queue_purge(&data->txq);
	spin_unlock(&(data->lock));

	return 0;
}

static int btspi_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct btspi_data *data = hci_get_drvdata(hdev);

	// dev_info(data->dev, "%s send_frame", hdev->name);
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

	spin_lock(&data->lock);
	skb_queue_tail(&data->txq, skb);
	spin_unlock(&data->lock);
	up(&data->trx_sema);
	return 0;
}

static int rtkbt_main_thread(void *arg)
{
	struct btspi_data *data = arg;
	struct sk_buff *skb;

	while (!kthread_should_stop()) {

		/* wait for smea */
		if (down_interruptible(&data->trx_sema)) {
			dev_dbg(data->dev, "%s is interrupted", __func__);
			continue;
		}

		spin_lock(&data->lock);
		skb = skb_dequeue(&data->txq);
		spin_unlock(&data->lock);
		bt_send_data(data, skb);

#if defined(CONFIG_WHC_HCI_SPI)
		spin_lock(&data->lock);
		skb = skb_dequeue(&data->rxq);
		spin_unlock(&data->lock);
#else
		/* trigger SPI to receive packet*/
		skb = btspi_recv_data();
#endif
		bt_rx_process(data, skb);

	}

	return 0;
}

int bt_dev_probe(struct device *pdev)
{
	struct hci_dev *hdev;
	int rc = 0;
	struct btspi_data *data;

	data = devm_kzalloc(pdev, sizeof(*data), GFP_KERNEL);
	if (!data) {
		return -ENOMEM;
	}

	dev_info(pdev, "%s\n", __func__);
	spin_lock_init(&(data->lock));
	sema_init(&data->trx_sema, 0);
	sema_init(&data->vendor_sema, 0);
	skb_queue_head_init(&data->txq);
	skb_queue_head_init(&data->rxq);

	data->thread = kthread_run(rtkbt_main_thread, data, "btrtk");

	data->dev = pdev;

	hdev = hci_alloc_dev();
	if (!hdev) {
		dev_err(pdev, "Couldn't alloc hdev");
		rc = -ENOMEM;
		goto alloc_dev_err;
	}

	hdev->bus = HCI_SPI;
	hci_set_drvdata(hdev, data);

	data->hdev = hdev;

	SET_HCIDEV_DEV(hdev, pdev);

	hdev->open     = btspi_open;
	hdev->close    = btspi_close;
	hdev->flush    = btspi_flush;
	hdev->send     = btspi_send_frame;

	rc = hci_register_dev(hdev);
	if (rc < 0) {
		dev_err(pdev, "Couldn't register hdev");
		goto err_hci_reg;
	}

	spidata = data;
	return 0;

err_hci_reg:
	hci_free_dev(hdev);
alloc_dev_err:
	kthread_stop(data->thread);
	return rc;
}

void bt_dev_remove(struct device *pdev)
{
	struct btspi_data *data = spidata;
	struct hci_dev *hdev;

	dev_info(pdev, "%s\n", __func__);

	hdev = data->hdev;
	hci_unregister_dev(hdev);
	hci_free_dev(hdev);

	//up(&spidata->trx_sema);
	kthread_stop(data->thread);
	dev_info(pdev, "%s exit\n", __func__);
}
