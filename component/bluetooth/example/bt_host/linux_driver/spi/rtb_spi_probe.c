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

// #include <asm/gpio.h>
#include <linux/of_gpio.h>
#include "rtb_spi_config.h"
#include "rtb_spi.h"

struct btspi_dev {
	struct spi_device *spi_dev;
	struct semaphore dev_rdy_sema;
	u8 dev_state;
};

struct btspi_dev btspi_priv = {0};

struct sk_buff *btspi_send_data(struct sk_buff *skb)
{
	struct btspi_dev *priv = &btspi_priv;
	struct spi_device *spidev = btspi_priv.spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *rxskb = NULL, *retskb = NULL;
	int rc;

	//dev_info(&data->spi_dev->dev, "%s", __func__);
	while (priv->dev_state == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(&spidev->dev, "%s: wait dev busy timeout\n\r", __func__);
			goto exit;
		}
	}

	if (skb->len > SPI_BUFSZ) {
		dev_err(&spidev->dev, "%s: len(%d) > SPI_BUFSZ\n\r", __func__, skb->len);
	}

	/* allocate rx skb */
	rxskb = bt_skb_alloc(SPI_BUFSZ, GFP_ATOMIC);
	if (rxskb == NULL) {
		dev_err(&spidev->dev, "%s: Alloc skb rx buf Err!!\n\r", __func__);
		goto exit;
	}

	/* initiate spi transaction */
	spimsg = spi_message_alloc(1, GFP_KERNEL);
	if (spimsg == NULL) {
		goto exit;
	}

	tr = list_first_entry(&spimsg->transfers, struct spi_transfer, transfer_list);
	tr->len = SPI_BUFSZ;
	tr->tx_buf = skb->data;
	tr->rx_buf = rxskb->data;
	tr->speed_hz = 100000;

	rc = spi_sync(spidev, spimsg);
	if (rc) {
		dev_err(&spidev->dev, "could not transfer : %d\n", rc);
		if (rxskb) {
			kfree_skb(rxskb);
		}
		goto exit;
	}

	//dev_info(&spidev->dev, "%s done", __func__);
	/* process rx msg */
	retskb = rxskb;

exit:
	kfree_skb(skb);
	if (spimsg) {
		spi_message_free(spimsg);
	}
	return retskb;
}

struct sk_buff *btspi_recv_data(void)
{
	struct btspi_dev *priv = &btspi_priv;
	struct spi_device *spidev = btspi_priv.spi_dev;
	struct spi_message *spimsg = NULL;
	struct spi_transfer *tr;
	struct sk_buff *pskb = NULL, *retskb = NULL;
	int rc;

	while (priv->dev_state == DEV_BUSY) {
		/* wait for sema*/
		if (down_timeout(&priv->dev_rdy_sema, msecs_to_jiffies(500))) {
			dev_err(&spidev->dev, "%s: wait dev busy timeout\n\r", __func__);
			goto exit;
		}
	}

	/* check RX_REQ level */
	if (gpio_get_value(RX_REQ_PIN)) {

		/* allocate skb */
		pskb = bt_skb_alloc(SPI_BUFSZ, GFP_ATOMIC);
		if (pskb == NULL) {
			dev_err(&spidev->dev, "%s: Alloc skb rx buf Err!!\n\r", __func__);
			goto exit;
		}

		/* initiate spi transaction */
		spimsg = spi_message_alloc(1, GFP_KERNEL);
		if (spimsg == NULL) {
			goto exit;
		}

		tr = list_first_entry(&spimsg->transfers, struct spi_transfer, transfer_list);
		tr->rx_buf = pskb->data;
		tr->len = SPI_BUFSZ;

		rc = spi_sync(spidev, spimsg);
		if (rc) {
			dev_err(&spidev->dev, "could not transfer : %d\n", rc);
			kfree_skb(pskb);

			goto exit;
		}

		retskb = pskb;
	}

exit:
	if (spimsg) {
		spi_message_free(spimsg);
	}

	return retskb;
}

static irqreturn_t rtw_spi_rx_req_handler(int irq, void *context)
{
	bt_recv_pkts(NULL);
	return IRQ_HANDLED;
}

static irqreturn_t rtw_spi_dev_rdy_handler(int irq, void *context)
{
	struct btspi_dev *priv = &btspi_priv;

	if (gpio_get_value(DEV_READY_PIN)) {
		priv->dev_state = DEV_READY;

		/* wakeup wait task */
		up(&priv->dev_rdy_sema);
	} else {
		priv->dev_state = DEV_BUSY;
	}

	return IRQ_HANDLED;
}

static int btspi_setup_gpio(struct spi_device *spi)
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

	status = request_irq(DEV_READY_IRQ, rtw_spi_dev_rdy_handler,
						 IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
						 "SPI_DEV_READY", spi);
	if (status) {
		goto free_dev_rdy_pin;
	}

	status = gpio_request(RX_REQ_PIN, "RX_REQ_PIN");
	if (status) {
		goto free_dev_rdy_irq;
	}

	status = gpio_direction_input(RX_REQ_PIN);
	if (status) {
		goto free_rx_req_pin;
	}

	status = request_irq(RX_REQ_IRQ, rtw_spi_rx_req_handler,
						 IRQF_SHARED | IRQF_TRIGGER_RISING,
						 "SPI_RX_REQ", spi);
	if (status) {
		goto free_rx_req_pin;
	}

	return status;

free_rx_req_pin:
	gpio_free(RX_REQ_PIN);

free_dev_rdy_irq:
	free_irq(DEV_READY_IRQ, spi);

free_dev_rdy_pin:
	gpio_free(DEV_READY_PIN);

	return status;
}

static int bt_spi_probe(struct spi_device *spi)
{
	struct btspi_dev *priv = &btspi_priv;
	struct spi_delay cs_setup_time = {
		.unit = SPI_DELAY_UNIT_USECS,
		.value = 7
	};
	int rc = 0;

	dev_info(&spi->dev, "%s\n", __func__);
	sema_init(&priv->dev_rdy_sema, 0);
	priv->spi_dev = spi;

	spi->cs_setup = cs_setup_time;

	rc = spi_setup(spi);
	if (rc != 0) {
		return rc;
	}

	/* setupGPIO pin */
	rc = btspi_setup_gpio(spi);
	if (rc) {
		dev_err(&spi->dev, "fail to setup gpio\n");
		return rc;
	}

	if (gpio_get_value(DEV_READY_PIN)) {
		priv->dev_state = DEV_READY;
		dev_info(&spi->dev, "dev ready\n");
	} else {
		priv->dev_state = DEV_BUSY;
		dev_info(&spi->dev, "dev busy\n");
	}

	bt_dev_probe(&spi->dev);
	return 0;
}

static void bt_spi_remove(struct spi_device *spi)
{
	bt_dev_remove(&spi->dev);

	free_irq(RX_REQ_IRQ, spi);
	free_irq(DEV_READY_IRQ, spi);
	gpio_free(RX_REQ_PIN);
	gpio_free(DEV_READY_PIN);
}

static const struct of_device_id inic_of_ids[] = {
	{ .compatible = "realtek,inic", .data = NULL },
	{},
};
MODULE_DEVICE_TABLE(of, inic_of_ids);

static const struct spi_device_id inic_spi_ids[] = {
	{ "inic", 0 },
	{},
};
MODULE_DEVICE_TABLE(spi, inic_spi_ids);

static struct spi_driver inic_spi_driver = {
	.driver = {
		.name = "inic",
		.of_match_table = of_match_ptr(inic_of_ids),
	},
	.id_table = inic_spi_ids,
	.probe = bt_spi_probe,
	.remove = bt_spi_remove,
};
module_spi_driver(inic_spi_driver);

MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("RealTek Bluetooth SPI Host");
MODULE_LICENSE("GPL");
MODULE_VERSION("rtl8721da");
