#ifndef __EXAMPLE_WIFI_RAW_H__
#define __EXAMPLE_WIFI_RAW_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include "platform_autoconf.h"
#include "wifi_intf_drv_to_upper.h"
#include "rtw_queue.h"
#include "os_wrapper.h"

#define WIFI_RAW_CH 1
#define RAW_TX_MGNT_FRAME_LEN  42
#define RAW_TX_DATA_FRAME_LEN  200

#define RX_PROMISC_BUF_NUM 1
#define RX_PROMISC_BUF_SIZE 1600

#define WifiGetFrameSubType(pbuf)	(*(unsigned short *)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2))

struct rx_promisc_data {
	struct list_head list;
	u8 *buffer;
	s8 rssi;               /**< Received signal strength indicator (RSSI) in dBm. */
	u8 data_rate;          /**< Data rate of the received packet. Values: @ref RTW_RATE_1M, @ref RTW_RATE_2M, etc. */
	u32 data_len;          /**< Length of the data in the buffer. */
};

struct rx_promisc_q_priv {
	struct rx_promisc_data rx_promisc_pkt[RX_PROMISC_BUF_NUM];
	struct list_head idle_q;
	struct list_head busy_q;
	rtos_mutex_t rx_promisc_q_mutex;  /* mutex */
	u32 idle_q_cnt;
	u32 busy_q_cnt;
};

void example_wifi_raw(void);
u8 wifi_rx_promisc_callback(struct rtw_rx_pkt_info *pkt_info);

#endif //#ifndef __EXAMPLE_WIFI_RAW_H__

