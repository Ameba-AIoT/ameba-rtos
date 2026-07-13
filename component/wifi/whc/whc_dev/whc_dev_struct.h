/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_DEV_STRUCT_H__
#define __WHC_DEV_STRUCT_H__

#if defined(CONFIG_WHC_INTF_SDIO)
#include "spdio_api.h"
#endif

/* -------------------------------- Includes -------------------------------- */
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
	struct list_head	list;
	void				*msg;
};

#endif /* __WHC_DEV_STRUCT_H__ */

