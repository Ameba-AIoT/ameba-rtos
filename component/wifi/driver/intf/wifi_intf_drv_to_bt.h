/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_bt.h
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

#ifndef __BT_INTF_H__
#define __BT_INTF_H__

#ifdef	__cplusplus
extern "C" {
#endif
#include <rtw_autoconf.h>

enum pta_host_role {
	PTA_HOST_WIFI			= 0,
	PTA_HOST_BT				= 1
};

enum pta_process_action {
	COMMON_ACTION			= 0,
	CALIBRATION_START		= 1,
	CALIBRATION_STOP		= 2
};

enum pta_type {
	PTA_AUTO            = 0,
	PTA_WIFI			= 1,
	PTA_BT				= 2,
	PTA_DISABLE		    = 0xFF,
};

enum bt_rfk_type {
	BT_RX_DCK       = 0,
	BT_LOK             = 1,
	BT_LOK_RES      = 2,
	BT_DAC_DCK     = 3,
	BT_ADC_DCK     = 4,
};

struct bt_rfk_param {
	enum bt_rfk_type type;
	uint8_t  rfk_data1;
	uint8_t  rfk_data2;
	uint8_t  rfk_data3;
	uint8_t  rfk_data4;
};


//----- ------------------------------------------------------------------
// Coex Interface opened for upper layer
//----- ------------------------------------------------------------------
void rltk_coex_set_gnt_bt(enum pta_type gnt_bt, u8 role, u8 process);
void rltk_coex_set_gnt_bt_with_clk_source(enum pta_type gnt_bt, u8 role, u8 process);
int rltk_coex_bt_rfk(struct bt_rfk_param *rfk_param);
void rltk_coex_bt_hci_notify(u8 *pdata, u16 len, u8 dir);
#ifdef	__cplusplus
}
#endif

#endif //#ifndef __BT_INTF_H__
