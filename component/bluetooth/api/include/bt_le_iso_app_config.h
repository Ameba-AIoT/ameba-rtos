/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_LE_ISO_APP_CONFIG_H__
#define __BT_LE_ISO_APP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <rtk_bt_def.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_gap.h>
#include <rtk_bt_le_iso.h>

#define RTK_BT_GAP_DEVICE_NAME_LEN (39+1)//!< Max length of device name, if device name length exceeds it, it will be truncated.

/**
 * @typedef rtk_bt_le_iso_app_conf_t
 * @brief   structure of default LE iso configuration
 */
typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];             /*!< Device name */
	uint8_t device_name_len;                                     /*!< Device name length */
	rtk_bt_le_iso_role_t iso_role;                               /*!< Default le iso role */
	uint8_t cig_num;                                             /*!< Default cig number */
	uint8_t cis_num;                                             /*!< Default cis number */
	uint8_t big_num;                                             /*!< Default big number */
	uint8_t bis_num;                                             /*!< Default bis number */
} rtk_bt_le_iso_app_conf_t;

#endif

#ifdef __cplusplus
}
#endif
#endif  /* __BT_LE_ISO_APP_CONFIG_H__ */
