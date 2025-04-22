/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_HOST_H__
#define __WHC_HOST_H__

/* -------------------------------- Includes -------------------------------- */
#include "rtw_inic_common.h"
#include "whc_host_api.h"
#include "whc_host_cust_evt.h"

#ifdef CONFIG_WHC_INTF_SPI
#include "whc_spi_host.h"
#include "whc_spi_host_trx.h"
#elif CONFIG_WHC_INTF_SDIO
#include "whc_sdio_host.h"
#include "whc_sdio_host_trx.h"
#include "rtw_sdio_drvio.h"
#endif

#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif

#ifdef CONFIG_WHC_BRIDGE_HOST
#include "whc_bridge_host_app.h"
#endif

#include "bt_inic_defs.h"
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

enum WHC_WIFI_CTRL_TYPE {
	WHC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	WHC_WIFI_EVT_RECV_PKTS,
	WHC_WIFI_EVT_API_CALL,
	WHC_WIFI_EVT_API_RETURN,
	WHC_WIFI_EVT_BRIDGE,
	WHC_WIFI_EVT_MAX,
	WHC_CUST_EVT, /* the ID to transmit data for the customer. */

	WHC_BT_EVT_BASE = WHC_BT_ID_BASE,
	WHC_BT_EVT_MAX = WHC_BT_ID_BASE + 0x1000000
};

struct whc_api_info {
	u32	event;
	u32	api_id;
};

/* the header for customer to send or receive the data between host and device. */
struct whc_cust_hdr {
	u32	event;
	u32	len;
};

struct whc_msg_info {
	u32	event;
	u32	wlan_idx;
	u32	data_len;
	u32	pad_len;
};

#if defined(CONFIG_WHC_BRIDGE_HOST)
struct whc_bridge_hdr {
	u32	event;
	u32	len;
};
#endif

struct event_func_t {
	u32 api_id;
	void (*func)(u32 api_id, u32 *param_buf);
};

struct event_priv_t {
	rtos_task_t api_dev_task;

	rtos_sema_t task_wake_sema;
	rtos_sema_t send_mutex;
	rtos_sema_t api_ret_sema;

	u8 *rx_api_msg;
	u8 *rx_ret_msg;

	u8 b_waiting_for_ret: 1;
};

struct whc_buf_info {
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries
};

struct whc_txbuf_info_t {
	struct whc_buf_info txbuf_info;
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb: 1;	/* the original buffer is skb or not */
};


#endif /* __INIC_HOST_H__ */
