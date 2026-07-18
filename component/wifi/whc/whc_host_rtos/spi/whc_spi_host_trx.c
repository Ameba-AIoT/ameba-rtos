/**
  ******************************************************************************
  * @file    whc_spi_host_trx.c
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

#include "whc_host.h"

extern struct whc_spi_host_priv_t spi_host_priv;
extern int whc_host_init_done;

int whc_spi_host_send_pkt(int idx, struct eth_drv_sg *sg_list, int sg_len,
						  int total_len, struct skb_raw_para *raw_para, u8 is_special_pkt)
{
	(void)raw_para;
	(void)is_special_pkt;

	struct eth_drv_sg *psg_list;
	int i = 0;
	struct whc_msg_info *msg;
	u8 *ptr;

	if (!whc_host_init_done) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "Host trx err: wifi not init\n");
		return -RTK_ERR_WIFI_NOT_INIT;
	}

	if (total_len > MAXIMUM_ETHERNET_PACKET_SIZE) {
		RTK_LOGE(TAG_WLAN_INIC, "%s: len(%d) > MAXIMUM_ETHERNET_PACKET_SIZE !\n\r", __func__, total_len);
		return -RTK_ERR_BUFFER_OVERFLOW;
	}

	rtos_mutex_take(spi_host_priv.host_send, MUTEX_WAIT_TIMEOUT);

	ptr = &(spi_host_priv.tx_buf[spi_host_priv.used_buf_num][0]);

	msg = (struct whc_msg_info *)N_BYTE_ALIGMENT((u32)ptr, DEV_DMA_ALIGN);
	msg->event = WHC_WIFI_EVT_XIMT_PKTS;
	msg->wlan_idx = idx;
	msg->pad_len = 0;
	msg->data_len = 0;

	ptr = (u8 *)(msg + 1);

	for (i = 0; i < sg_len; i++) {
		psg_list = &sg_list[i];
		memcpy(ptr, (void *)(psg_list->buf), psg_list->len);
		ptr += psg_list->len;
		msg->data_len += psg_list->len;
	}

	whc_spi_host_send((u8 *)msg, msg->data_len + sizeof(struct whc_msg_info),
					  &(spi_host_priv.tx_buf[spi_host_priv.used_buf_num][0]), 1);

	spi_host_priv.used_buf_num = (spi_host_priv.used_buf_num + 1) % TX_BUF_NUM;

	rtos_mutex_give(spi_host_priv.host_send);
	return RTK_SUCCESS;
}

