/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __INIC_HOST_H__
#define __INIC_HOST_H__

/* -------------------------------- Includes -------------------------------- */
#include "rtw_inic_common.h"
#include "inic_host_api.h"

#ifdef CONFIG_INIC_INTF_SPI
#include "inic_spi_host.h"
#include "inic_spi_host_trx.h"
#endif
#ifdef CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif

#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

enum INIC_WIFI_CTRL_TYPE {
	INIC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	INIC_WIFI_EVT_RECV_PKTS,
	INIC_WIFI_EVT_API_CALL,
	INIC_WIFI_EVT_API_RETURN,
	INIC_WIFI_EVT_MAX,
	//TODO host need BT confirm file
	//INIC_BT_EVT_BASE = INIC_BT_ID_BASE,
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

struct inic_buf_info {
	u32 buf_allocated; //The spdio buffer allocated address
	u16 size_allocated; //The actual allocated size
	u32 buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	u16 buf_size;
	u8 type; //The type of the data which this buffer carries
};

struct inic_txbuf_info_t {
	struct inic_buf_info txbuf_info;
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb: 1;	/* the original buffer is skb or not */
};

void inic_host_init_skb(void);

#endif /* __INIC_HOST_H__ */
