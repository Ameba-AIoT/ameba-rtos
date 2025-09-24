/**
  ******************************************************************************
  * @file    wtn_APP_zrpp.h
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

#ifndef WTN_APP_ZRPP_H
#define WTN_APP_ZRPP_H

enum zrpp_scan_control_code {
	ZRPP_CONTROL_ALLOW,
	ZRPP_CONTROL_PAUSE,
	ZRPP_CONTROL_STOP,
};

enum zrpp_status_code {
	ZRPP_STATUS_START,
	ZRPP_STATUS_SCAN_ONGOING,
	ZRPP_STATUS_GET_AP_INFO,
};

/* -------------------------------- Includes -------------------------------- */
int wtn_zrpp_sync_state_with_ble_config(enum zrpp_scan_control_code zrpp_control_op);
#endif