/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __INIC_DEV_MEM_H__
#define __INIC_DEV_MEM_H__

#ifdef CONFIG_INIC_INTF_SDIO
#include "spdio_api.h"
#endif
#ifndef CONFIG_FULLMAC
#include "ameba_inic.h"
#endif

/* -------------------------------- Includes -------------------------------- */
enum INIC_WIFI_CTRL_TYPE {
	INIC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	INIC_WIFI_EVT_RECV_PKTS,
	INIC_WIFI_EVT_API_CALL,
	INIC_WIFI_EVT_API_RETURN,

	INIC_WIFI_EVT_MAX
};

struct inic_api_info {
	u32	event;
	u32	api_id;
};

struct inic_buf_info {
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries
};

struct inic_txbuf_info_t {
#ifdef CONFIG_INIC_INTF_SDIO
	struct spdio_buf_t txbuf_info;
#else
	struct inic_buf_info txbuf_info;
#endif
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb: 1;	/* the original buffer is skb or not */
};

struct inic_msg_node {
	struct list_head		list;
	void	*msg;
};

struct inic_msg_info {
	u32	event;
	u32	wlan_idx;
	u32	data_len;
	u32	pad_len;
};

#define SPI_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32
#define SPI_BUFSZ		(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct inic_msg_info)))
#define SPI_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct inic_msg_info), 4)

#define SPDIO_RX_BUFSZ	(SPDIO_RX_BUFSZ_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct inic_msg_info) + sizeof(INIC_TX_DESC))) //n*64, must be rounded to 64
#define SPDIO_SKB_RSVD_LEN		N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct inic_msg_info) - sizeof(INIC_TX_DESC), SPDIO_DMA_ALIGN_4)


#endif /* __INIC_SDIO_H__ */
