/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_DEV_MEM_H__
#define __WHC_DEV_MEM_H__

#if defined(CONFIG_WHC_INTF_SDIO)
#include "spdio_api.h"
#ifndef CONFIG_FULLMAC
#include "ameba_inic.h"
#endif
#endif
/* -------------------------------- Includes -------------------------------- */

struct whc_api_info {
	u32	event;
	u32	api_id;
	u32	data_len;
	u32	rsvd;	/* API payload has no pad; kept at @12 to match whc_msg_info */
};

/* the header for customer to send or receive the data between host and device. */
struct whc_cust_hdr {
	u32	event;
	u32	len;
};

/* for spi, due to full duplex communication in spi. Also for usb, due to the lack of length info. */
struct whc_cmd_path_hdr {
	u32	event;
	u32	len;
};

struct whc_buf_info {
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries
};

struct whc_txbuf_info_t {
#if defined(CONFIG_WHC_INTF_SDIO)
	struct spdio_buf_t txbuf_info;
#else
	struct whc_buf_info txbuf_info;
#endif
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb: 1;	/* the original buffer is skb or not */
};

struct whc_msg_node {
	struct list_head		list;
	void	*msg;
};

struct whc_msg_info {
	u32	event;
	u8	wlan_idx: 2;
	u8	flow_ctrl_en: 1;
	u8	rsvd1 : 5;
	u8	rsvd2[3];
	u32	data_len;
	u32	pad_len;
};

/*
 * Common view over every device->host header, used by the host SDIO RX
 * de-aggregation splitter to walk concatenated units: each unit's on-wire
 * length is derived from its header (event + the per-family length fields).
 *
 * The splitter sizes each segment by header type: whc_msg_info uses
 * data_len@8 + pad_len@12; whc_api_info uses data_len@8 only (@12 is an unused
 * placeholder, its payload has no pad); whc_cmd_path_hdr / whc_cust_hdr use
 * len@4. BT INIC (INIC_BT_HOST_RX) is not part of this union: its packed
 * bt_inic_sdio_hdr carries a u16 total length at @4, which the SDIO splitter
 * reads directly.
 */
union whc_hdr {
	struct whc_msg_info		msg;	/* RECV_PKTS / FLOWCTRL */
	struct whc_api_info		api;	/* API_CALL / API_RETURN */
	struct whc_cmd_path_hdr	cmd;	/* CMD */
	struct whc_cust_hdr		cust;	/* custom path */
};

#define SPI_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32
#define SPI_BUFSZ		(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
#define SPI_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), 4)

#define SPDIO_DEVICE_RX_BUFSZ	(SPDIO_RX_BUFSZ_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info) + sizeof(INIC_TX_DESC))) //n*64, must be rounded to 64
#define SPDIO_SKB_RSVD_LEN		N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info) - sizeof(INIC_TX_DESC), SPDIO_DMA_ALIGN_4)

#define UART_DMA_ALIGN(x)	((((x-1)>>5)+1)<<5) //alignement to 32 for cache line
#define UART_BUFSZ		(UART_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
/* uart as flow controller when rx */
#define UART_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), WHC_UART_RX_BURST_SIZE)

#endif /* __INIC_SDIO_H__ */
