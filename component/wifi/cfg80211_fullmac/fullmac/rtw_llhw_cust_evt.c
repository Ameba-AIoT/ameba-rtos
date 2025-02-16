// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2025, Realtek Corporation. All rights reserved.
*/

#include "rtw_llhw_cust_evt.h"

/**
* @brief  This is a function implemented by customer. It shuld to used
* to receive the data from device.
* @param  buf: the buffer sent from the device.
* @param  len: length of the transmitted data.
* @return none.
*/
static inline void llhw_recv_cust_evt_hdl(u8 *buf, u32 len)
{
	(void) buf;
	(void) len;

	/* implemented by customer */

	return;
}

void llhw_recv_cust_evt(u8 *rx_buf)
{
	struct inic_cust_hdr *head = (struct inic_cust_hdr *)rx_buf;
	u8 *buf = rx_buf + sizeof(struct inic_cust_hdr);

	llhw_recv_cust_evt_hdl(buf, head->len);

	return;
}

/**
* @brief  This is a function to send the date to device from host.
* @param  buf: the buffer sent to the device from host.
* @param  len: length of the transmitted data.
* @return none.
*/
void llhw_send_cust_evt(u8 *buf, u32 len)
{
	struct inic_cust_hdr *head = NULL;
	u8 *buf_send = NULL;
	u32 len_send;

	/* send TX_DESC + info + data(param, param_len) */
	len_send = SIZE_TX_DESC + sizeof(struct inic_cust_hdr) + len;
	buf_send = kzalloc(len_send, GFP_KERNEL);
	if (buf_send) {
		/* fill inic_api_info_t */
		head = (struct inic_cust_hdr *)(buf_send + SIZE_TX_DESC);
		head->event = INIC_CUST_EVT;
		head->len = len;

		/* copy data */
		if (buf) {
			memcpy(buf_send + SIZE_TX_DESC + sizeof(struct inic_cust_hdr), buf, len);
		}

		/* send */
		llhw_send_data(buf_send, len_send, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
		kfree(buf_send);
#endif
	} else {
		dev_err(global_idev.fullmac_dev, "%s can't alloc buffer!\n", __func__);
	}
}