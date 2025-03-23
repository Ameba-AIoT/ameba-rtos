/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_app_basic.h
  * @author
  * @version
  * @date
  * @brief   This file provides user interface for Wi-Fi station and AP mode configuration
  *             base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_CONF_TYPES_H
#define __WIFI_CONF_TYPES_H

#ifndef CONFIG_FULLMAC
#include "section_config.h"
#include "platform_stdlib.h"
#include "ameba.h"
#include "rtw_wifi_common.h"
#include "rtw_byteorder.h"

#include "rtw_wifi_defs.h"
#include "rtw_80211spec_macro.h"
#include "rtw_80211spec_enum.h"
#include "rtw_80211spec_he.h"
#include "rtw_80211spec_ht.h"
#include "rtw_80211spec_twt.h"
#include "rtw_80211spec_vht.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @addtogroup WIFI_Exported_Constants Wi-Fi Exported Constants
 * @{
 */
#define WHC_MAX_SSID_LENGTH (33) /**< Refer to 802.11 spec, the max length of ssid is 32 bytes. */
#define SCAN_LONGEST_WAIT_TIME	(12000) /**< Scan longest wait time. */

/** Set to this means do fast survey on the specified channel with scan time set to 25ms,
 * otherwise do normal scan on the specified channel with scan time set to 110ms. */
#define PSCAN_FAST_SURVEY	0x02

/** @ref wifi_user_conf::rf_calibration_disable set to this means disable DPK(Digital Pre-Distortion Calibration) of rf calibration. */
#define DIS_DPK BIT(0)

/** @} End of WIFI_Exported_Constants group*/


/** @addtogroup WIFI_Exported_Types Wi-Fi Exported Types
* @{
*/
/** @addtogroup WIFI_Exported_Enumeration_Types Enumeration Type
 * @{
 */
/**
 * @brief The enumeration lists the results of the function, size int.
 */
enum {
	RTW_SUCCESS                      = 0,	/**< Success. */

	RTW_ERROR                        = -1,	/**< Generic Error. */
	RTW_BADARG                       = -2,	/**< Bad Argument. */
	RTW_BUSY                         = -3,	/**< Busy. */
	RTW_NOMEM                        = -4,	/**< No Memory. */
	RTW_TIMEOUT                      = -5,	/**< Timeout. */

	RTW_CONNECT_INVALID_KEY	         = -11,	/**< Invalid key. */
	RTW_CONNECT_SCAN_FAIL            = -12,
	RTW_CONNECT_AUTH_FAIL            = -13,
	RTW_CONNECT_AUTH_PASSWORD_WRONG  = -14,
	RTW_CONNECT_ASSOC_FAIL           = -15,
	RTW_CONNECT_4WAY_HANDSHAKE_FAIL  = -16,
	RTW_CONNECT_4WAY_PASSWORD_WRONG  = -17,
};

/**
 * @brief  The enumeration is wl band type.
 */
enum {
	WL_BAND_2_4G = 0,   /**< 2.4g band. */
	WL_BAND_5G,            /**< 5g band. */
	WL_BAND_2_4G_5G_BOTH, /**< 2.4g&5g band. */
	WL_BANDMAX  /**< Max band. */
};

/**
 * @brief The enumeration lists the type of pmksa operations.
 */
enum {
	PMKSA_SET = 0,
	PMKSA_DEL = 1,
	PMKSA_FLUSH = 2,
};

/**
 * @brief The enumeration lists the type of speaker related settings.
 */
enum {
	SPEAKER_SET_INIT = 0,
	SPEAKER_SET_LATCH_I2S_COUNT = 1,
	SPEAKER_SET_TSF_TIMER = 2,
};

/**
 * @brief  The enumeration is transmission type for wifi custom ie.
 */
enum {
	PROBE_REQ = BIT(0),  /**< Probe request. */
	PROBE_RSP = BIT(1),  /**< Probe response. */
	BEACON	  = BIT(2),     /**< Beacon. */
	ASSOC_REQ = BIT(3), /**< Assocation request. */
};

/**
 * @brief The enumeration lists rcr mode under promisc.
 */
enum {
	RCR_ALL_PKT,  /**< Receive all packets. */
	RCR_AP_ALL     /**< Receive all packtets send by connected ap. */
};

/**
 * @brief The enumeration lists promisc callback return value.
 */
enum {
	NEED_DRIVER_HANDLE,  /**< Driver will continue process this pkt. */
	BYPASS_DRIVER_HANDLE     /**< Driver will bypass this pkt. */
};

/** @} End of WIFI_Exported_Enumeration_Types group*/

/** @addtogroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */

/** @} End of WIFI_Exported_Structure_Types group*/

/** @} End of WIFI_Exported_Types group*/
/** @} End of WIFI_API group */
#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_H
