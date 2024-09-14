/**
  ******************************************************************************
  * @file    wifi_auto_reconnect.h
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

#ifndef __WIFI_AUTO_RECONNECT_H__
#define __WIFI_AUTO_RECONNECT_H__
#if CONFIG_AUTO_RECONNECT
struct rtw_auto_reconn_t {
	rtw_network_info_t  conn_param;
	rtos_timer_t        timer;
	u8                  pwd[RTW_PASSPHRASE_MAX_SIZE + 1];
	u8                  cnt;
	u8                  eap_method;
	u8                  b_infinite: 1;
	u8                  b_enable: 1;
	u8                  b_waiting: 1;
};

#define WIFI_STACK_SIZE_AUTO_RECONN_TASKLET	(1056 + 128 + CONTEXT_SAVE_SIZE)	/* max 1056 in lite */

extern struct rtw_auto_reconn_t  rtw_reconn;
void rtw_reconn_new_conn(rtw_network_info_t *connect_param);
void rtw_reconn_join_status_hdl(char *buf, int flags);
#endif

#endif

