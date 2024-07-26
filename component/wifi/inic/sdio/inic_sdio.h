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

#include "spdio_api.h"
#include "ameba_inic.h"

#include "inic_sdio_dev_api.h"
#include "inic_sdio_dev_trx.h"
#include "inic_sdio_msg_queue.h"
#include "inic_sdio_dev.h"
#ifdef CONFIG_SDIO_BRIDGE
#include "inic_sdio_dev_bridge.h"
#endif
#endif

#ifndef CONFIG_SDIO_BRIDGE
#include "inic_sdio_protocal_offload.h"
#endif
/* -------------------------------- Macros ---------------------------------- */
#define FLAG_WLAN_IF_NOT_RUNNING		0xFFFFFFFF

#define HOST_MSG_PARAM_NUM		(9)
#define HOST_MSG_DUMY_NUM		(64 - (HOST_MSG_PARAM_NUM + 2) * 4)
#define DEV_MSG_PARAM_NUM		(7)
#define DEV_MSG_DUMY_NUM		(64 - (DEV_MSG_PARAM_NUM + 2) * 4)

/* ------------------------------- Data Types ------------------------------- */
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

#endif /* __INIC_SDIO_H__ */
