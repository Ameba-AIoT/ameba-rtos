// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************
 *
 * Copyright(c) Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 ******************************************************************************/

#include <whc_host_linux.h>
#include <linux/slab.h>

#include "whc_host_xfer.h"
#include "whc_gspi_host.h"

#define WHC_GSPI_REQ_RETRY_TIMES 2

/* g_xfer_adapter: the notify callback takes no argument, so a global is required */
static struct whc_xfer_adapter_t *g_xfer_adapter;

static void whc_gspi_recv_notify(void)
{
	struct whc_xfer_adapter_t *adapter = g_xfer_adapter;

	adapter->sdio_rx_ready = true;
	wake_up(&adapter->sdio_rx_wq);
}

static int whc_gspi_send(struct whc_xfer_adapter_t *adapter, u8 *pbuf, u32 size)
{
	/* Prepend a zeroed INIC_TX_DESC before the XFER descriptor + payload */
	memmove(adapter->tx_buf + sizeof(INIC_TX_DESC), pbuf, size);
	memset(adapter->tx_buf, 0, sizeof(INIC_TX_DESC));

	whc_gspi_host_send_data(adapter->tx_buf, sizeof(INIC_TX_DESC) + size, NULL);
	return 0;
}

static int whc_gspi_recv_timeout(struct whc_xfer_adapter_t *adapter, u8 *pbuf, int *actual_size, int timeout_ms)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;
	u32 rx_len, frame_len;
	u8 *frame_buf;
	int ret = 0;

	ret = wait_event_timeout(adapter->sdio_rx_wq,
							 adapter->sdio_rx_ready == true,
							 msecs_to_jiffies(timeout_ms));
	if (ret == 0) {
		dev_err(&priv->spi_dev->dev, "RX timeout\n");
		ret = -ETIMEDOUT;
		goto out;
	}

	adapter->sdio_rx_ready = false;

	rx_len = rtw_gspi_get_rx_len(priv);
	if (rx_len == 0) {
		dev_err(&priv->spi_dev->dev, "rx_len is 0\n");
		ret = -1;
		goto out;
	}

	/* gspi_read_port() places payload at frame_buf + GSPI_CMD_PHASE_LEN (4B) */
	frame_len = GSPI_FIFO_FRAME_LEN(rx_len);
	frame_buf = kmalloc(frame_len, GFP_KERNEL);
	if (!frame_buf) {
		ret = -ENOMEM;
		goto out;
	}

	if (!gspi_read_port(priv, frame_buf, rx_len)) {
		dev_err(&priv->spi_dev->dev, "read port FAIL\n");
		kfree(frame_buf);
		ret = -1;
		goto out;
	}

	memcpy(pbuf, frame_buf + GSPI_CMD_PHASE_LEN, rx_len);
	*actual_size = (int)rx_len;
	kfree(frame_buf);

out:
	gspi_write32(priv, GSPI_REG_HIMR, priv->gspi_himr);
	return ret;
}

static int whc_gspi_init(struct whc_xfer_adapter_t *adapter, void *params)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;

	/* Initialise wait queue before enabling HIMR to avoid wake_up on uninitialised queue */
	init_waitqueue_head(&adapter->sdio_rx_wq);
	adapter->sdio_rx_ready = false;

	g_xfer_adapter = adapter;
	priv->rx_recv_notify = whc_gspi_recv_notify;

	if (false == rtw_gspi_init_common(priv)) {
		return -1;
	}

	return 0;
}

static void whc_gspi_deinit(struct whc_xfer_adapter_t *adapter)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;

	/* Disable all interrupts; rtw_gspi_init_common() will re-enable after fwdl */
	gspi_write32(priv, GSPI_REG_HIMR, 0);
	priv->rx_recv_notify = whc_host_recv_notify;
	g_xfer_adapter = NULL;
}

/*
 * Poll HCPWM2 until device signals bootloader (IMG1_BIT=BIT2) or application
 * (ACT_BIT=BIT0) ready. IMG1_BIT is only treated as terminal in ROM phase to
 * avoid a race with HCI_DeInit in bootloader phase.
 */
static int whc_gspi_check_firmware(struct whc_xfer_adapter_t *adapter)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;
	u16 reg16;
	int i;

	for (i = 0; i < 200; i++) {
		reg16 = rtw_read16(priv, GSPI_REG_HCPWM2);
		if (reg16 & GSPI_HCPWM2_ACT_BIT) {
			priv->dev_state = WHC_XFER_FW_TYPE_APPLICATION;
			return WHC_XFER_FW_TYPE_APPLICATION;
		} else if (reg16 & GSPI_HCPWM2_IMG1_BIT) {
			if (priv->dev_state == WHC_XFER_FW_TYPE_ROM) {
				priv->dev_state = WHC_XFER_FW_TYPE_BOOTLOADER;
				return WHC_XFER_FW_TYPE_BOOTLOADER;
			}
		} else {
			if ((priv->dev_state != WHC_XFER_FW_TYPE_ROM) && (priv->dev_state != WHC_XFER_FW_TYPE_BOOTLOADER)) {
				dev_err(&priv->spi_dev->dev, "%s: unsupported dev_state (%d)\n", __FUNCTION__, priv->dev_state);
				return -1;
			}
		}
		msleep(1);
	}

	dev_err(&priv->spi_dev->dev, "%s: HCPWM2 timeout\n", __FUNCTION__);
	priv->dev_state = -1;
	return -1;
}

static int whc_gspi_reset_device(struct whc_xfer_adapter_t *adapter)
{
	dev_err(&((struct whc_gspi *)adapter->interface)->spi_dev->dev, "reset not supported\n");
	return -1;
}

static int whc_gspi_handshake(struct whc_xfer_adapter_t *adapter, u8 *request, int request_size,
							  u8 *response, int response_size, int timeout_ms)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;
	int ret, actual_length, retry = 0;

	while (retry++ < WHC_GSPI_REQ_RETRY_TIMES) {
		adapter->sdio_rx_ready = false;
		ret = whc_gspi_send(adapter, request, request_size);
		if (ret == 0) {
			ret = whc_gspi_recv_timeout(adapter, response, &actual_length, timeout_ms);
			if (ret == 0) {
				if (actual_length != response_size) {
					dev_err(&priv->spi_dev->dev, "Protocol error: response length %d/%d\n", actual_length, response_size);
					ret = -1;
				} else {
					break;
				}
			} else {
				dev_err(&priv->spi_dev->dev, "Response error (%d)\n", ret);
			}
		} else {
			dev_err(&priv->spi_dev->dev, "Request error (%d)\n", ret);
		}
	}

	return ret;
}

static int whc_gspi_boot(struct whc_xfer_adapter_t *adapter)
{
	struct whc_gspi *priv = (struct whc_gspi *)adapter->interface;
	int ret;

	ret = whc_xfer_boot(adapter);
	if (ret) {
		dev_err(&priv->spi_dev->dev, "%s fail (%d)\n", __FUNCTION__, ret);
		return ret;
	}

	/* Switch to polling mode while waiting for firmware to come up */
	gspi_write32(priv, GSPI_REG_HIMR, 0);

	ret = whc_gspi_check_firmware(adapter);
	return (ret < 0) ? ret : 0;
}

const struct whc_xfer_ops_t whc_gspi_xfer_ops = {
	.init = whc_gspi_init,
	.deinit = whc_gspi_deinit,
	.check_firmware = whc_gspi_check_firmware,
	.reset_device = whc_gspi_reset_device,
	.handshake = whc_gspi_handshake,
	.boot = whc_gspi_boot,
	.write = whc_xfer_write,
};
