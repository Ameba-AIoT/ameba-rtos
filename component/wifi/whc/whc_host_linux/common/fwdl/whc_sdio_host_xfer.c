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
 ******************************************************************************/

#include <whc_host_linux.h>

#include "whc_host_xfer.h"
#include "whc_sdio_host.h"
#include "whc_sdio_host_reg.h"
#include "whc_sdio_host_drvio.h"

#include <linux/slab.h>

#define WHC_SDIO_REQ_RETRY_TIMES 2

/* Global XFER adapter pointer - used by interrupt handler */
static struct whc_xfer_adapter_t *g_xfer_adapter = NULL;

/**
 * @brief	Check if firmware download mode is needed
 * @param	priv: SDIO device handle
 * @retval	true: Firmware download is needed (device is in download mode)
 * 			false: Firmware download is not needed (firmware already running)
 */
bool whc_sdio_check_dl_mode(struct whc_sdio *priv)
{
	u8 reg8;

	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
	if (!(reg8 & SD_FW_DL_MODE)) {
		dev_info(&priv->func->dev, "%s: No need to update firmware!\n", __FUNCTION__);
		return false;
	}

	return true;
}

/**
 * @brief	Send data using TX descriptor
 * @param	adapter: XFER adapter
 * @param	pbuf: Data buffer
 * @param	size: Data size
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_send(struct whc_xfer_adapter_t *adapter, u8 *pbuf, u32 size)
{
	/* Use pre-allocated tx_buf, data goes after TX descriptor */
	INIC_TX_DESC *ptxdesc = (INIC_TX_DESC *)adapter->tx_buf;

	/* Actually pbuf is adapter->tx_buf */
	memmove(adapter->tx_buf + sizeof(INIC_TX_DESC), pbuf, size);

	/* Clear and setup TX descriptor */
	memset(ptxdesc, 0, sizeof(INIC_TX_DESC));

	whc_sdio_host_send_data((u8 *)ptxdesc, sizeof(INIC_TX_DESC) + size, NULL);

	return 0;
}

/**
 * @brief	SDIO XFER receive callback - called from interrupt handler
 * @param	adapter: XFER adapter
 * @note	This function is called when RX data is available during XFER download
 */
static void whc_sdio_recv_notify(void)
{
	struct whc_xfer_adapter_t *adapter = g_xfer_adapter;
	adapter->sdio_rx_ready = true;
	wake_up(&adapter->sdio_rx_wq);
}

/**
 * @brief	Receive data with timeout using interrupt-driven approach
 * @param	adapter: XFER adapter
 * @param	pbuf: Data buffer
 * @param	actual_size: Pointer to store actual received size
 * @param	timeout_ms: Timeout in milliseconds
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_recv_timeout(struct whc_xfer_adapter_t *adapter, u8 *pbuf, int *actual_size, int timeout_ms)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;
	INIC_RX_DESC *prxdesc;
	int ret;
	u32 himr;
	u32 rx_len;

	/* Clear RX ready flag before waiting */
	adapter->sdio_rx_ready = false;

	/* Wait for RX ready interrupt */
	ret = wait_event_timeout(adapter->sdio_rx_wq,
							 adapter->sdio_rx_ready == true,
							 msecs_to_jiffies(timeout_ms));

	if (ret == 0) {
		dev_err(&priv->func->dev, "RX timeout\n");
		return -ETIMEDOUT;
	}

	adapter->sdio_rx_ready = false;

	rx_len = rtw_sdio_get_rx_len(priv);
	if (rx_len > 0) {
		ret = sdio_read_port(priv, SDIO_RX_FIFO_DOMAIN_ID, rx_len, adapter->rx_buf);
		ret = (ret == true) ? 0 : -1;
	} else {
		dev_err(&priv->func->dev, "rx_len is 0\n");
		ret = -1;
	}

	if (ret == 0) {
		/* Parse RX descriptor from rx_buf */
		prxdesc = (INIC_RX_DESC *)adapter->rx_buf;
		*actual_size = prxdesc->pkt_len;

		/* Actually pbuf is adapter->rx_buf */
		memmove(pbuf, adapter->rx_buf + prxdesc->offset, prxdesc->pkt_len);
	}

	/* restore RX_REQ interrupt*/
	himr = cpu_to_le32(priv->sdio_himr);
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

	return ret;
}

/**
 * @brief	Start XFER mode - switch to XFER interrupt handler
 * @param	adapter: XFER adapter
 * @param	params: Optional parameters (unused)
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_init(struct whc_xfer_adapter_t *adapter, void *params)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;

	/* Set global adapter pointer */
	g_xfer_adapter = adapter;

	priv->rx_recv_notify = whc_sdio_recv_notify;

	if (false == rtw_sdio_init_common(priv)) {
		return -1;
	}

	init_waitqueue_head(&adapter->sdio_rx_wq);
	adapter->sdio_rx_ready = false;

	return 0;
}

/**
 * @brief	Stop XFER mode - restore original interrupt handler
 * @param	adapter: XFER adapter
 */
static void whc_sdio_deinit(struct whc_xfer_adapter_t *adapter)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;
	u32 himr = 0;

	/* HIMR - turn all off */
	sdio_local_write(priv, SDIO_REG_HIMR, 4, (u8 *)&himr);

	g_xfer_adapter = NULL;
}

/**
 * @brief	Check whether the device firmware is ready
 * @param	adapter: XFER adapter
 * @return	Firmware type or negative error code
 */
static int whc_sdio_check_firmware(struct whc_xfer_adapter_t *adapter)
{
	u16 reg16, i;
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;

	for (i = 0; i < 200; i++) {
		reg16 = rtw_read16(priv, SDIO_REG_HCPWM2);

		if (priv->dev_state == WHC_XFER_FW_TYPE_ROM) {
			if (reg16 & HCPWM2_IMG1_BIT) {
				priv->dev_state = WHC_XFER_FW_TYPE_BOOTLOADER;
				return WHC_XFER_FW_TYPE_BOOTLOADER;
			}
		} else if (priv->dev_state == WHC_XFER_FW_TYPE_BOOTLOADER) {
			if (reg16 & HCPWM2_ACT_BIT) {
				priv->dev_state = WHC_XFER_FW_TYPE_APPLICATION;
				return WHC_XFER_FW_TYPE_APPLICATION;
			}
		} else {
			dev_err(&priv->func->dev, "%s: Not Support dev_state (%d)\n", __FUNCTION__, priv->dev_state);
			return -1;
		}

		msleep(1);
	}

	dev_err(&priv->func->dev, "%s: Wait SDIO_REG_HCPWM2 Timeout!!\n", __FUNCTION__);
	priv->dev_state = -1;
	return -1;
}

/**
 * @brief	Reset device
 * @param	adapter: XFER adapter
 * @return	Negative error code (not supported)
 */
static int whc_sdio_reset_device(struct whc_xfer_adapter_t *adapter)
{
	dev_err(&((struct whc_sdio *)adapter->interface)->func->dev, "whc_sdio_reset_device not support\n");
	return -1;
}

/**
 * @brief	XFER handshake function
 * @param	adapter: XFER adapter
 * @param	request: Request buffer
 * @param	request_size: Request size
 * @param	response: Response buffer
 * @param	response_size: Expected response size
 * @param	timeout_ms: Timeout in milliseconds
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_handshake(struct whc_xfer_adapter_t *adapter, u8 *request, int request_size, u8 *response, int response_size, int timeout_ms)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;
	int ret;
	int actual_length;
	int retry = 0;

	while (retry++ < WHC_SDIO_REQ_RETRY_TIMES) {
		ret = whc_sdio_send(adapter, request, request_size);
		if (ret == 0) {
			ret = whc_sdio_recv_timeout(adapter, response, &actual_length, timeout_ms);
			if (ret == 0) {
				if (actual_length != response_size) {
					dev_err(&priv->func->dev, "Protocol error: unexpected response length %d/%d\n", actual_length, response_size);
					ret = -1;
				} else {
					break;
				}
			} else {
				dev_err(&priv->func->dev, "Response error (%d)\n", ret);
			}
		} else {
			dev_err(&priv->func->dev, "Request error (%d)\n", ret);
		}
	}

	return ret;
}

/**
 * @brief	Boot image - SDIO specific handling for ROM mode
 * @param	adapter: XFER adapter
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_boot(struct whc_xfer_adapter_t *adapter)
{
	int ret;
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;

	if (priv->dev_state == WHC_XFER_FW_TYPE_ROM) {
		/* For ROM mode, use SDIO-specific ROM boot function */
		ret = whc_sdio_rom_boot(adapter);
	} else {
		/* For bootloader, use common XFER boot function */
		ret = whc_xfer_boot(adapter);
	}

	if (ret) {
		dev_err(&priv->func->dev, "%s fail (%d)\n", __FUNCTION__, ret);
		return ret;
	}

	// HIMR - turn all off, just polling
	rtw_write32(priv, SDIO_REG_HIMR, 0);

	ret = whc_sdio_check_firmware(adapter);
	return (ret < 0) ? ret : 0;
}

/**
 * @brief	Write data - SDIO specific handling for ROM mode
 * @param	adapter: XFER adapter
 * @param	buf: Data buffer
 * @param	addr: Target address
 * @param	size: Data size
 * @param	need_padding: Whether padding is needed
 * @return	0 on success, negative error code on failure
 */
static int whc_sdio_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding)
{
	int ret;
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;

	if (priv->dev_state == WHC_XFER_FW_TYPE_ROM) {
		/* For ROM mode, use SDIO-specific ROM write function */
		ret = whc_sdio_rom_write(adapter, buf, addr, size, need_padding);
	} else {
		/* For bootloader mode, use common XFER write function */
		ret = whc_xfer_write(adapter, buf, addr, size, need_padding);
	}

	return ret;
}

const struct whc_xfer_ops_t whc_sdio_xfer_ops = {
	.init = whc_sdio_init,
	.deinit = whc_sdio_deinit,
	.check_firmware = whc_sdio_check_firmware,
	.reset_device = whc_sdio_reset_device,
	.handshake = whc_sdio_handshake,
	.boot = whc_sdio_boot,
	.write = whc_sdio_write,
};
