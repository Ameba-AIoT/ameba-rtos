/******************************************************************************
 *
 * Copyright(c) Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#include <whc_host_linux.h>
#include "whc_host_xfer.h"

#define WHC_USB_REQ_RETRY_TIMES		3

#define WHC_USB_BULK_OUT_TIMEOUT		5000
#define WHC_USB_BULK_IN_TIMEOUT			5000
#define WHC_USB_CTRL_XFER_TIMEOUT		500

/* USB control request types */
#define WHC_USB_CTRL_REQ_TYPE			0xF0
#define WHC_USB_CTRL_READ				0xC0  /* USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE */
#define WHC_USB_CTRL_WRITE				0x40

/* USB control request/ack codes */
#define WHC_USB_CTRL_QUERY_REQ			0x01
#define WHC_USB_CTRL_RESET_REQ			0x02
#define WHC_USB_CTRL_QUERY_ACK			0x81
#define WHC_USB_CTRL_RESET_ACK			0x82

static int whc_usb_send(struct whc_usb *priv, u8 *pbuf, u32 size);
static int whc_usb_recv(struct whc_usb *priv, u8 *pbuf, u32 size);
static int whc_usb_recv_timeout(struct whc_usb *priv, u8 *pbuf, u32 size, int timeout_ms);

/**
 * @brief	Boot image - USB uses common XFER boot
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_usb_boot(struct whc_xfer_adapter_t *adapter)
{
	/* USB uses common XFER boot function */
	return whc_xfer_boot(adapter);
}

/**
 * @brief	Write data - USB uses common XFER write
 * @param	adapter: Adapter handler
 * @param	buf: Data buffer
 * @param	addr: Target address
 * @param	size: Data size
 * @param	need_padding: Whether padding is needed
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_usb_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding)
{
	/* USB uses common XFER write function */
	return whc_xfer_write(adapter, buf, addr, size, need_padding);
}

static int whc_usb_handshake(struct whc_xfer_adapter_t *adapter, u8 *request, int request_size, u8 *response, int response_size, int timeout_ms)
{
	struct whc_usb *priv = (struct whc_usb *)adapter->interface;
	int ret;
	int retry = 0;

	while ((retry++ < WHC_USB_REQ_RETRY_TIMES)) {
		if (request && request_size > 0) {
			ret = whc_usb_send(priv, request, request_size);
			if (ret) {
				dev_err(priv->dev, "Request error (%d)\n", ret);
				continue;
			}
		}

		if (response && response_size > 0) {
			ret = whc_usb_recv_timeout(priv, response, response_size, timeout_ms);
			if (ret) {
				dev_err(priv->dev, "Response error (%d)\n", ret);
				continue;
			}
		}
		break;
	}

	return ret;
}

/*
 * @brief	Check whether the device firmware is ready.
 * @param	adapter: Adapter handler
 * @retval	Firmware type or error
 */
static int whc_usb_check_firmware(struct whc_xfer_adapter_t *adapter)
{
	int retry = 0;
	int ret = -1;
	unsigned int pipe;
	struct whc_usb *priv = (struct whc_usb *)adapter->interface;
	struct usb_device *udev = priv->usb_dev;
	u8 *data = priv->ctrl_in_buf;
	struct whc_xfer_desc_t *rx_desc;

	if (priv == NULL || udev == NULL) {
		dev_err(priv->dev, "%s: USB device not ready\n", __FUNCTION__);
		return -1;
	}

	mutex_lock(&priv->ctrl_mutex);

	pipe = usb_rcvctrlpipe(udev, 0);

	while ((!priv->bSurpriseRemoved) && (retry++ < WHC_USB_REQ_RETRY_TIMES)) {
		dev_dbg(priv->dev, "Query firmware type\n");
		memset(data, 0, WHC_XFER_DESC_SIZE);
		ret = usb_control_msg(udev, pipe, WHC_USB_CTRL_REQ_TYPE, WHC_USB_CTRL_READ,
							  0, WHC_USB_CTRL_QUERY_REQ, data, WHC_XFER_DESC_SIZE, WHC_USB_CTRL_XFER_TIMEOUT);
		if (ret == WHC_XFER_DESC_SIZE) {
			rx_desc = (struct whc_xfer_desc_t *)data;
			dev_dbg(priv->dev, "Query response: len %d type 0x%02X mode 0x%02X\n",
					rx_desc->data_len, rx_desc->type, rx_desc->mode);
			if ((rx_desc->status == WHC_XFER_STAUS_OK) && (rx_desc->type == WHC_USB_CTRL_QUERY_ACK)) {
				ret = rx_desc->mode;
				break;
			} else {
				dev_dbg(priv->dev, "Query response error\n");
				ret = -1;
			}
		} else {
			if (ret < 0) {
				dev_err(priv->dev, "Query control msg error (%d)\n", ret);
			} else {
				dev_err(priv->dev, "Query response length error (%d)\n", ret);
			}
			dev_err(priv->dev, "Fail to query firmware type (%d), retry %d#\n", ret, retry);
		}
	}

	mutex_unlock(&priv->ctrl_mutex);

	return ret;
}

/*
 * @brief	Reset device
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_usb_reset_device(struct whc_xfer_adapter_t *adapter)
{
	int retry = 0;
	int ret = -1;
	unsigned int pipe;
	struct whc_usb *priv = (struct whc_usb *)adapter->interface;
	struct usb_device *udev = priv->usb_dev;
	u8 *data = priv->ctrl_in_buf;
	struct whc_xfer_desc_t *rx_desc;

	if (priv == NULL || udev == NULL) {
		dev_err(priv->dev, "%s: USB device not ready\n", __FUNCTION__);
		return -1;
	}

	mutex_lock(&priv->ctrl_mutex);

	pipe = usb_rcvctrlpipe(udev, 0);

	while ((!priv->bSurpriseRemoved) && (retry++ < WHC_USB_REQ_RETRY_TIMES)) {
		dev_info(priv->dev, "Reset device...\n");
		memset(data, 0, WHC_XFER_DESC_SIZE);
		ret = usb_control_msg(udev, pipe, WHC_USB_CTRL_REQ_TYPE, WHC_USB_CTRL_READ,
							  0, WHC_USB_CTRL_RESET_REQ, data, WHC_XFER_DESC_SIZE, WHC_USB_CTRL_XFER_TIMEOUT);
		if (ret == WHC_XFER_DESC_SIZE) {
			rx_desc = (struct whc_xfer_desc_t *)data;
			dev_dbg(priv->dev, "Reset response: len %d type 0x%02X status 0x%02X\n",
					rx_desc->data_len, rx_desc->type, rx_desc->status);
			if ((rx_desc->status == WHC_XFER_STAUS_OK) && (rx_desc->type == WHC_USB_CTRL_RESET_ACK)) {
				ret = 0;
				dev_info(priv->dev, "Reset response OK\n");
				break;
			} else {
				dev_err(priv->dev, "Reset response error\n");
				ret = -1;
			}
		} else {
			if (ret < 0) {
				dev_err(priv->dev, "Reset control msg error (%d)\n", ret);
			} else {
				dev_err(priv->dev, "Reset response length error (%d)\n", ret);
				ret = -1;
			}
			dev_err(priv->dev, "Fail to reset device (%d), retry %d#\n", ret, retry);
		}
	}

	mutex_unlock(&priv->ctrl_mutex);

	return ret;
}
static int whc_usb_send(struct whc_usb *priv, u8 *pbuf, u32 size)
{
	u32 tx_len;
	int err = 0;
	int actual_length;

	tx_len = size;

	usb_lock_device(priv->usb_dev);
	err = usb_bulk_msg(priv->usb_dev, priv->tx_pipe[0], pbuf, tx_len, &actual_length, WHC_USB_BULK_OUT_TIMEOUT);
	usb_unlock_device(priv->usb_dev);

	if (err) {
		dev_err(priv->dev, "%s: FAIL! %d\n", __FUNCTION__, err);
	}

	return err;
}

static int whc_usb_recv(struct whc_usb *priv, u8 *pbuf, u32 size)
{
	u32 rx_len;
	int err = 0;
	int actual_length;

	rx_len = size;

	usb_lock_device(priv->usb_dev);
	err = usb_bulk_msg(priv->usb_dev, priv->rx_pipe, pbuf, rx_len, &actual_length, WHC_USB_BULK_IN_TIMEOUT);
	usb_unlock_device(priv->usb_dev);

	if (err) {
		dev_err(priv->dev, "%s: FAIL! %d\n", __FUNCTION__, err);
	}

	return err;
}

static int whc_usb_recv_timeout(struct whc_usb *priv, u8 *pbuf, u32 size, int timeout_ms)
{
	unsigned long start_time = jiffies;
	unsigned long timeout_jiffies = msecs_to_jiffies(timeout_ms);
	int ret;
	int actual_length;

	while (time_before(jiffies, start_time + timeout_jiffies)) {
		ret = usb_bulk_msg(priv->usb_dev, priv->rx_pipe, pbuf, size, &actual_length, timeout_ms);
		if (ret == 0) {
			return ret;
		}
		msleep(1);
	}

	return -ETIMEDOUT;
}

const struct whc_xfer_ops_t whc_usb_xfer_ops = {
	.init = NULL, /* Will be set by whc_usb_xfer_init */
	.deinit = NULL,
	.check_firmware = whc_usb_check_firmware,
	.reset_device = whc_usb_reset_device,
	.handshake = whc_usb_handshake,
	.boot = whc_usb_boot,
	.write = whc_usb_write,
};
