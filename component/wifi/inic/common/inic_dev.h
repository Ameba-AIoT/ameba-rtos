/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __INIC_SDIO_H__
#define __INIC_SDIO_H__

/* -------------------------------- Includes -------------------------------- */

#ifndef CONFIG_FULLMAC
#include "platform_autoconf.h"
#endif

struct inic_buf_info;

#include "inic_def.h"

#ifndef CONFIG_FULLMAC
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "rtw_skbuff.h"
#include "rtw_queue.h"
#include "wifi_conf.h"
#include "wifi_intf_drv_to_lwip.h"
#include "wifi_intf_drv_to_upper.h"

#include "inic_dev_api.h"
#include "inic_dev_trx.h"
#include "inic_dev_msg_queue.h"

#ifdef CONFIG_INIC_INTF_SDIO
#include "ameba_inic.h"
#include "spdio_api.h"
#include "inic_sdio_dev.h"
#elif defined (CONFIG_INIC_INTF_SPI)
#include "inic_spi_dev.h"
#endif
#ifdef CONFIG_SDIO_BRIDGE
#include "inic_sdio_dev_bridge.h"
#endif
#endif

#ifndef CONFIG_SDIO_BRIDGE
#include "inic_dev_protocal_offload.h"
#endif

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

struct inic_msg_info {
	u32	event;
	u32	wlan_idx;
	u32	data_len;
	u32	pad_len;
};

struct inic_msg_node {
	struct list_head		list;
	void	*msg;
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
	u8 is_skb;	/* the original buffer is skb or not */
};

#define SPI_DMA_ALIGN(x)	((((x-1)>>2)+1)<<2) //alignement to 4
#define SPI_BUFSZ		(SPI_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct inic_msg_info)))
#define SPI_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct inic_msg_info), 4)

#endif /* __INIC_SDIO_H__ */
