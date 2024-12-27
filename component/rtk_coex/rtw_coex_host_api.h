/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef RTW_COEX_HOST_API_H
#define RTW_COEX_HOST_API_H

#include "ameba_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TYPE_BITS   (8) // BIT15~BIT8
#define SUBTYPE_BITS (8) // BIT7~BIT0
#define TOTALTYPE_BITS (TYPE_BITS + SUBTYPE_BITS)

#define COEX_BT_TYPE(x)     (((RTK_COEX_TYPE_BT << SUBTYPE_BITS) + ((x) & ((1 << SUBTYPE_BITS) - 1))) & ((1 << TOTALTYPE_BITS) - 1))
#define COEX_EXT_TYPE(x)    (((RTK_COEX_TYPE_EXT << SUBTYPE_BITS) + ((x) & ((1 << SUBTYPE_BITS) - 1))) & ((1 << TOTALTYPE_BITS) - 1))
#define COEX_WP_TYPE(x)     (((RTK_COEX_TYPE_WPAN << SUBTYPE_BITS) + ((x) & ((1 << SUBTYPE_BITS) - 1))) & ((1 << TOTALTYPE_BITS) - 1))

#define COEX_TYPE_GET(x)    ((x & ((1 << TOTALTYPE_BITS) - 1)) >> SUBTYPE_BITS)
#define COEX_SUBTYPE_GET(x) ((x & ((1 << TOTALTYPE_BITS) - 1)) & ((1 << SUBTYPE_BITS) - 1))

enum coex_type {
	RTK_COEX_TYPE_BT = 0,
	RTK_COEX_TYPE_EXT,
	RTK_COEX_TYPE_WPAN,
	/* end */
	RTK_COEX_TYPE_INVALID = ((1 << TYPE_BITS) - 1),
};

enum coex_subtype_h2c_bt {
	COEX_H2C_BT_UNDEF = 0,
	COEX_H2C_BT_HCI_NOTIFY_HCI_EVENT,
	COEX_H2C_BT_HCI_NOTIFY_HCI_CMD,
	COEX_H2C_BT_HCI_NOTIFY_SW_MAILBOX,
	COEX_H2C_BT_VENDOR_INFO_SET,
	COEX_H2C_BT_RFK,
	COEX_H2C_BT_SET_BT_SEL,
	COEX_H2C_BT_SET_PTA,
	/* end */
	COEX_H2C_BT_INVALID = ((1 << SUBTYPE_BITS) - 1),
};

enum coex_subtype_h2c_ext {
	/* common info */
	COEX_H2C_EXT_UNDEF = 0,
	COEX_H2C_EXT_INIT,
	COEX_H2C_EXT_WL_PERFORMANCE_REQUEST,
	/* info for WPAN*/
	COEX_H2C_EXT_WPAN_STATE,
	COEX_H2C_EXT_WPAN_CHANNEL,
	/* info for BT*/
	COEX_H2C_EXT_BT_PROFILE,
	/* end */
	COEX_H2C_EXT_INVALID = ((1 << SUBTYPE_BITS) - 1),
};

enum coex_subtype_h2c_wpan {
	COEX_H2C_WPAN_UNDEF = 0,
	COEX_H2C_WPAN_ZB_RFK,
	COEX_H2C_WPAN_INVALID = ((1 << SUBTYPE_BITS) - 1),
};

enum coex_subtype_c2h_bt {
	COEX_C2H_BT_UNDEF = 0,
	COEX_C2H_BT_HCI_MSG_MAILBOX_TRIGGER,
	/* end */
	COEX_C2H_BT_INVALID = ((1 << SUBTYPE_BITS) - 1),
};

enum coex_subtype_c2h_ext {
	COEX_C2H_EXT_UNDEF = 0,
	/* end */
	COEX_C2H_EXT_INVALID = ((1 << SUBTYPE_BITS) - 1),
};

enum coex_subtype_c2h_wpan {
	COEX_C2H_WPAN_UNDEF = 0,
	COEX_C2H_WPAN_INVALID = ((1 << SUBTYPE_BITS) - 1),
};
//////////////////////////////////////////////////////////
///////// for BT Variables
//////////////////////////////////////////////////////////
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

struct rtk_coex_vendor_info {
	uint8_t  vendor_id;
	uint8_t  product_id;
};

struct pta_para_t {
	u8 type;
	u8 role;
	u8 process;
};

//////////////////////////////////////////////////////////
///////// for EXT Variables
//////////////////////////////////////////////////////////
enum EXT_STATE {
	EXT_DISABLE = 0,
	EXT_ENABLE,
} ;

enum EXT_PROTOCOL {
	EXT_PTA_PROTOCOL_UNDEF = 0,
	EXT_PTA_PROTOCOL_WPAN,
	EXT_PTA_PROTOCOL_BT,
	EXT_PTA_PROTOCOL_BT_WPAN,
} ;

enum EXT_PERFORMANCE {
	EXT_PERF_DEFAULT,
	EXT_PERF_HIGH,
	EXT_PERF_MED,
	EXT_PERF_LOW,
};

struct extchip_para_t {
	u8 pri_det_time;
	u8 trx_det_time;
	u8 pri_mode: 1;
	u8 req_polar: 1;
	u8 gnt_polar: 1;
	u8 rsvd: 5;
	enum EXT_PROTOCOL active_protocol;
	u8 pta_pad_req;
	u8 pta_pad_pri;
	u8 pta_pad_gnt;
};

//////////////////////////////////////////////////////////
///////// for Internal Wpan Variables
//////////////////////////////////////////////////////////
// ADD HERE


//////////////////////////////////////////////////////////
///////// for BT Function Declare
//////////////////////////////////////////////////////////
/**
 * @brief  Hci notify.
 * @param[in]  pdata  	A pointer to hci notify data.
 * @param[in]  len 		size of hci notify data.
 * @param[in]  type		type for hci notify.
 * @return  None.
 */
void rtk_coex_btc_bt_hci_notify(u8 *pdata, u16 len, u8 type);
/**
 * @brief  Vendor info set.
 * @param[in]  p_rfk_param  A pointer to struct rtk_coex_vendor_info.
 * @param[in]  length 		size of struct rtk_coex_vendor_info.
 * @return  None.
 */
void rtk_coex_btc_vendor_info_set(void *p_vendor_info, u8 length);
/**
 * @brief  RFK for BT.
 * @param[in]  p_rfk_param  A pointer to struct bt_rfk_param.
 * @param[in]  length 		size of struct bt_rfk_param.
 * @return  None.
 */
int rtk_coex_btc_bt_rfk(void *p_rfk_param, u16 length);
/**
 * @brief  Set BTS0 or BTS1.
 * @param[in]  bt_ant: the BT ant(BTS0/BTS1).
 * @return  None.
 */
void rtk_coex_btc_set_bt_ant(u8 bt_ant);
/**
 * @brief  Set PTA.
 * @param[in]  type  refer to enum pta_type.
 * @param[in]  role  refer to enum enum pta_host_role.
 * @param[in]  process refer to enum enum pta_process_action.
 * @return  None.
 */
void rtk_coex_btc_set_pta(u8 type, u8 role, u8 process);

//////////////////////////////////////////////////////////
///////// for EXT Function Declare
//////////////////////////////////////////////////////////
/**
 * @brief  ext chip pta parameter get.
 * @param[in]  None.
 * @return  A pointer to Internal struct extchip_para_t.
 */
struct extchip_para_t *rtk_coex_extc_para_get(void);
/**
 * @brief  ext chip pta parameter set.
 * @param[in]  para_t	A pointer to struct extchip_para_t.
 * @return  None.
 */
void rtk_coex_extc_para_set(struct extchip_para_t *para_t);
/**
 * @brief  ext chip init notification.
 * @param[in]  p_extchip_para	A pointer to struct extchip_para_t.
 * @param[in]  length	size of struct extchip_para_t.
 * @return  None.
 */
void rtk_coex_extc_ntfy_init(struct extchip_para_t *p_extchip_para, u16 length);
/**
 * @brief  ext wpan state notification.
 * @param[in] state  wpan status state, refer to enum EXT_STATE.
 * @return  None.
 */
void rtk_coex_extc_ntfy_wpan_state(u8 state);
/**
 * @brief  ext wpan channel notification.
 * @param[in] channel  802.15.4 channel number [11-26].
 * @return  None.
 */
void rtk_coex_extc_ntfy_wpan_channel(u8 channel);

//////////////////////////////////////////////////////////
///////// for Internal wpan Function Declare
//////////////////////////////////////////////////////////
/**
 * @brief  RFK for Internal wpan module.
 * @param[in] None.
 * @return  0 if Sucess, else if Fail.
 */
int rtk_coex_wpc_zb_rfk(void);

#ifdef __cplusplus
}
#endif
#endif
