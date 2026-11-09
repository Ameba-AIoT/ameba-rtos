/**
 * @file      bt_le_iso_app_config.h
 * @author    sherman_sun@realsil.com.cn
 * @brief     Bluetooth LE ISO application default configuration definition.
 * @copyright Copyright (c) 2023. Realtek Semiconductor Corporation. All rights reserved.
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

#define RTK_BT_GAP_DEVICE_NAME_LEN (39+1)    /*!< Maximum length of the device name including the terminating NUL. A longer name is truncated. */

/**
 * @typedef   rtk_bt_le_iso_app_conf_t
 * @brief     Default LE ISO application configuration used when initializing the LE ISO module.
 */
typedef struct {
	uint8_t device_name[RTK_BT_GAP_DEVICE_NAME_LEN];    /*!< Device name. */
	uint8_t device_name_len;                            /*!< Length of the device name in bytes. */
	rtk_bt_le_iso_role_t iso_role;                      /*!< Default LE ISO role.
                                                         *   Selects whether the CIG and CIS resources or the BIG and BIS resources are initialized.
                                                         *   An unknown role makes the initialization fail. */
	uint8_t cig_num;                                    /*!< Number of CIGs to use. */
	uint8_t cis_num;                                    /*!< Number of CISes to use. */
	uint8_t big_num;                                    /*!< Number of BIGs to use. */
	uint8_t bis_num;                                    /*!< Number of BISes to use. */
} rtk_bt_le_iso_app_conf_t;

#endif

#ifdef __cplusplus
}
#endif
#endif /* __BT_LE_ISO_APP_CONFIG_H__ */
