/**
  ******************************************************************************
  * @file    wifi_api_event.h
  * @author
  * @version
  * @date
  * @brief   This file provides the functions related to event handler mechanism.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_API_EVENT_H
#define __WIFI_API_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/**********************************************************************************************
 *                                     wifi event structures
 *********************************************************************************************/
/** @addtogroup WIFI_Exported_Types Wi-Fi Exported Types
* @{
*/

/** @addtogroup WIFI_Exported_Enumeration_Types Enumeration Type
 * @{
 */

/**
  * @brief Join status flags for @ref RTW_EVENT_JOIN_STATUS.
  */
enum rtw_join_status {
	RTW_JOINSTATUS_UNKNOWN = 0,

	/* Intermediate connection states (new states should be added before RTW_JOINSTATUS_SUCCESS). */
	RTW_JOINSTATUS_STARTING,                  /**< Connection initiation. */
	RTW_JOINSTATUS_SCANNING,                  /**< Network scanning. */
	RTW_JOINSTATUS_AUTHENTICATING,            /**< Authentication in progress. */
	RTW_JOINSTATUS_AUTHENTICATED,             /**< Authentication completed. */
	RTW_JOINSTATUS_ASSOCIATING,               /**< Association in progress. */
	RTW_JOINSTATUS_ASSOCIATED,                /**< Association completed. */
	RTW_JOINSTATUS_4WAY_HANDSHAKING,          /**< 4-way handshake in progress. */
	RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE,       /**< 4-way handshake completed. */
	RTW_JOINSTATUS_SUCCESS,                   /**< Connection successful. */

	/* Other connection result states (new states should be added after RTW_JOINSTATUS_SUCCESS). */
	RTW_JOINSTATUS_FAIL,                      /**< Connection failed. */
	RTW_JOINSTATUS_DISCONNECT,                /**< Disconnected after successful connection. */
};

/**
  * @brief  Wi-Fi event identifiers.
  */
enum rtw_event_id {
	RTW_EVENT_STA_ASSOC = 0,          /**< SoftAP mode: Station associated */
	RTW_EVENT_STA_DISASSOC,           /**< SoftAP mode: Station disassociated */
	RTW_EVENT_JOIN_STATUS,	          /**< STA mode: Connection status change (see example_wifi_event.c) */
	RTW_EVENT_CSI_DONE,               /**< CSI data ready (see example_wifi_csi.c) */
	RTW_EVENT_WPA_STA_WPS_START,      /**< STA mode: WPS procedure started */
	RTW_EVENT_WPA_WPS_FINISH,         /**< STA mode: WPS procedure completed */
	RTW_EVENT_WPA_EAPOL_START,        /**< STA mode: WPA enterprise authentication started */
	RTW_EVENT_WPA_EAPOL_RECVD,        /**< STA mode: EAPOL packet received during WPA enterprise authentication */
	RTW_EVENT_MAX,
};
/** @} End of WIFI_Exported_Enumeration_Types group*/

/** @addtogroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */

struct rtw_event_hdl_func_t {
	u16 evt_id;
	void (*handler)(u8 *buf, s32 len);
};

struct rtw_event_join_status_info {
	/* common paras */
	u8 status;  /* refer to @ref rtw_join_status */
	u8 channel;
	u8 bssid[ETH_ALEN];
	s8 rssi;

	/* private paras */
	union {
		struct rtw_event_authenticating {

		} authenticating;  /* RTW_JOINSTATUS_AUTHENTICATING */
		struct rtw_event_authenticated {
			u16 reason_or_status_code;  /**< 802.11 reason code or status code from AP.*/
		} authenticated;  /* RTW_JOINSTATUS_AUTHENTICATED */
		struct rtw_event_associating {
			u8 is_reassoc;
		} associating;  /* RTW_JOINSTATUS_ASSOCIATING */
		struct rtw_event_associated {
			u16 reason_or_status_code;
			u8 is_reassoc;
		} associated;  /* RTW_JOINSTATUS_ASSOCIATED */
		struct rtw_event_join_fail {
			s32 fail_reason;  /**< Failure reason, refer to @ref RTK_FAIL, -@ref RTK_ERR_WIFI_CONN_INVALID_KEY, etc. */
			u16 reason_or_status_code;
		} fail;  /* RTW_JOINSTATUS_FAIL */
		struct rtw_event_disconnect {
			u16 disconn_reason;  /**< Disconnect reason, refer to @ref rtw_disconn_reason. */
		} disconnect;  /* RTW_JOINSTATUS_DISCONNECT */
	} private;
};

/** @} End of WIFI_Exported_Structure_Types group*/
/** @} End of WIFI_Exported_Types group*/

/**********************************************************************************************
 *                                     Function Declarations
 *********************************************************************************************/
/** @addtogroup WIFI_Exported_Functions Wi-Fi Exported Functions
 * @{
 */
/** @addtogroup WIFI_Exported_Event_Functions Event Functions
  * @{
  */

/** @} End of Event_Functions group*/
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/
#ifdef __cplusplus
}
#endif

#endif //__WIFI_API_EVENT_H

