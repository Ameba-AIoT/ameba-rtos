/**
  ******************************************************************************
  * @file    inic_ipc_host_trx.c
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

#define __INIC_SPI_HOST_TRX_C__

/* -------------------------------- Includes -------------------------------- */
/* external head files */

/* internal head files */
#include "inic_host.h"
#define CONFIG_ENABLE_CACHE

#define fix_tx_buf_num	2
extern struct inic_spi_host_priv_t spi_host_priv;
u8 tx_buf[fix_tx_buf_num][4 + SPI_BUFSZ] = {0};
u8 used_buf_num = 0;

/* -------------------------------- spi --------------------------------- */
/**
 * @brief  to initialize the skb in host.
 * @param  none
 * @return none.
 */
void inic_host_init_skb(void)
{
	return;
}

void inic_host_deinit_skb(void)
{
	if (spi_host_priv.dummy_tx_buf) {
		rtos_mem_free((void *)spi_host_priv.dummy_tx_buf);
	}

	if (spi_host_priv.rx_buf) {
		rtos_mem_free((void *)spi_host_priv.rx_buf);
	}
	return;
}


/* host tx */
int inic_host_send(int idx, struct eth_drv_sg *sg_list, int sg_len,
				   int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
#ifndef SPI_TODO
#endif
	(void)raw_para;
	(void)is_special_pkt;
	struct eth_drv_sg *psg_list;
	int ret = 0, i = 0;
	int pad_len = 0;
	struct inic_msg_info *msg;
	struct inic_txbuf_info_t *inic_tx = rtos_mem_zmalloc(sizeof(struct inic_txbuf_info_t));
	rtos_mutex_take(spi_host_priv.host_send, 0xFFFFFFFF);

	u8 *ptr = &(tx_buf[used_buf_num][0]);

	if (*ptr != 0) {
		RTK_LOGE(TAG_WLAN_INIC, "%s fail buf busy !\n\r", __func__);
		return -1;
	}

	if (total_len > MAXIMUM_ETHERNET_PACKET_SIZE) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -1;
	}

	ptr += 4;
	pad_len = ((u32)ptr - sizeof(struct inic_msg_info)) % DEV_DMA_ALIGN;
	msg = (struct inic_msg_info *)(ptr);
	msg->event = INIC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = pad_len;
	msg->data_len = 0;
	/* allocate the skb buffer */

	ptr += sizeof(struct inic_msg_info);

	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(ptr, (void *)(psg_list->buf), psg_list->len);
		ptr += psg_list->len;
		msg->data_len += psg_list->len;
	}

	inic_tx->txbuf_info.buf_allocated = inic_tx->txbuf_info.buf_addr = (u32)msg;
	inic_tx->txbuf_info.size_allocated = inic_tx->txbuf_info.buf_size = msg->data_len + sizeof(struct inic_msg_info);

	inic_tx->ptr = &(tx_buf[used_buf_num][0]);
	inic_tx->is_skb = 1;

	inic_host_send_data(&inic_tx->txbuf_info);

	used_buf_num = (used_buf_num + 1) % fix_tx_buf_num;
	rtos_sema_give(spi_host_priv.host_send);

	//rtos_mem_free(inic_tx);
	return ret;
}


/* -------------------------------- spi done --------------------------------- */


