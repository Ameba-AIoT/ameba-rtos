/**
  ******************************************************************************
  * @file    whc_uart_host_trx.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#define __WHC_UART_HOST_TRX_C__

/* -------------------------------- Includes -------------------------------- */
/* external head files */

/* internal head files */
#include "whc_host.h"

#define fix_tx_buf_num	2

u8 used_buf_num = 0;

extern struct event_priv_t event_priv;
extern int whc_host_init_done;
extern struct whc_uart_host_priv_t uart_host_priv;
u8 tx_buf[fix_tx_buf_num][4 + UART_BUFSZ] = {0};

/**
 * @brief  to initialize the skb in host.
 * @param  none
 * @return none.
 */
void whc_host_init_skb(void)
{
	return;
}

void whc_host_deinit_skb(void)
{

	if (uart_host_priv.rx_buf) {
		rtos_mem_free((void *)uart_host_priv.rx_buf);
	}
	return;
}

/* host tx */
int whc_uart_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
					   int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
	(void)raw_para;
	(void)is_special_pkt;
	struct eth_drv_sg *psg_list;
	int ret = RTK_SUCCESS, i = 0;
	int pad_len = 0;
	struct whc_msg_info *msg;
	struct whc_txbuf_info_t *whc_tx = rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));

	if (!whc_host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host trx err: wifi not init\n");
		return -RTK_ERR_WIFI_NOT_INIT;
	}

	rtos_mutex_take(uart_host_priv.host_send, 0xFFFFFFFF);

	u8 *ptr = &(tx_buf[used_buf_num][0]);

	if (*ptr != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s fail buf busy !\n\r", __func__);
		return -RTK_ERR_WIFI_TX_BUF_FULL;
	}

	if (total_len > MAXIMUM_ETHERNET_PACKET_SIZE) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -RTK_ERR_BUFFER_OVERFLOW;
	}

	ptr += 4;
	pad_len = ((u32)ptr - sizeof(struct whc_msg_info)) % DEV_DMA_ALIGN;
	msg = (struct whc_msg_info *)(ptr);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = pad_len;
	msg->data_len = 0;
	/* allocate the skb buffer */

	ptr += sizeof(struct whc_msg_info);

	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(ptr, (void *)(psg_list->buf), psg_list->len);
		ptr += psg_list->len;
		msg->data_len += psg_list->len;
	}

	whc_tx->txbuf_info.buf_allocated = whc_tx->txbuf_info.buf_addr = (u32)msg;
	whc_tx->txbuf_info.size_allocated = whc_tx->txbuf_info.buf_size = msg->data_len + sizeof(struct whc_msg_info);

	whc_tx->ptr = &(tx_buf[used_buf_num][0]);
	whc_tx->is_skb = 1;

	whc_uart_host_send_data(&whc_tx->txbuf_info);

	used_buf_num = (used_buf_num + 1) % fix_tx_buf_num;
	rtos_sema_give(uart_host_priv.host_send);

	//rtos_mem_free(whc_tx);
	return ret;
}

/**
* @brief  send buf to dev
* @param  buf: data buf to be sent.
* @param  len: real buf address, to be freed after sent.
* @return none.
*/
void whc_uart_host_send_to_dev(u8 *buf, u32 len)
{
	u8 *txbuf = NULL;
	struct whc_txbuf_info_t *whc_tx;

	/* construct struct whc_buf_info & whc_buf_info_t */
	whc_tx = (struct whc_txbuf_info_t *)rtos_mem_zmalloc(sizeof(struct whc_txbuf_info_t));
	txbuf = rtos_mem_zmalloc(len);

	if ((txbuf == NULL) || (whc_tx == NULL)) {
		RTK_LOGE(TAG_WLAN_INIC, "%s mem fail \r\n", __func__);
		if (whc_tx) {
			rtos_mem_free(whc_tx);
		}

		if (txbuf) {
			rtos_mem_free(txbuf);
		}
		return;
	}

	/* copy data */
	memcpy(txbuf, buf, len);

	whc_tx->txbuf_info.buf_allocated = whc_tx->txbuf_info.buf_addr = (u32)txbuf;
	whc_tx->txbuf_info.size_allocated = whc_tx->txbuf_info.buf_size = len;

	whc_tx->ptr = txbuf;
	whc_tx->is_skb = 0;

	/* send ret_msg + ret_val(buf, len) */
	whc_uart_host_send_data(&whc_tx->txbuf_info);
}