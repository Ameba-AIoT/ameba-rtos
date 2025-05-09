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
/**
  * @brief Data structure for @ref RTW_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_DISCONNECT.
  */
struct rtw_event_info_joinstatus_disconn {
	u16 disconn_reason;      /**< Disconnect reason, refer to @ref rtw_disconn_reason. */
	u8	bssid[6];            /**< MAC address of the AP.*/
};

/**
  * @brief Data structure for @ref RTW_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_FAIL
  */
struct rtw_event_info_joinstatus_joinfail {
	s32					fail_reason;           /**< Failure reason, refer to @ref RTK_FAIL, -@ref RTK_ERR_WIFI_CONN_INVALID_KEY, etc. */
	u16					reason_or_status_code; /**< 802.11 reason code or status code from AP.*/
	u8					bssid[6];              /**< MAC address of the AP.*/
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

/**
 * @brief  Registers an event listener for specific Wi-Fi events.
 * @param[in] event_id : The events from the Wi-Fi driver to listen for:
 *                    - @ref RTW_EVENT_STA_ASSOC
 *                    - @ref RTW_EVENT_STA_DISASSOC
 *                    - @ref RTW_EVENT_JOIN_STATUS
 *                    - @ref RTW_EVENT_CSI_DONE
 *                    - @ref RTW_EVENT_WPA_STA_WPS_START
 *                    - @ref RTW_EVENT_WPA_WPS_FINISH
 *                    - @ref RTW_EVENT_WPA_EAPOL_START
 *                    - @ref RTW_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function to process the events. It has the following parameters:
 *                    - \b buf: Event data passed from the driver to the application layer.
 *                    - \b len: Length of the `buf`.
 *                    - \b flag: Flag set by Wi-Fi driver, used in conjunction with `buf`.
 *                         The content of `buf` and meaning of `flag` depend on the `event_id`:
 *                         <table>
 *                         <tr><th>event id</th><th>buf</th><th>flag</th></tr>
 *                         <tr><td>RTW_EVENT_JOIN_STATUS</td><td>rtw_event_info_joinstatus_joinfail</td><td>RTW_JOINSTATUS_FAIL</td></tr>
 *                         <tr><td>RTW_EVENT_JOIN_STATUS</td><td>rtw_event_info_joinstatus_disconn</td><td>RTW_JOINSTATUS_DISCONNECT</td></tr>
 *                         <tr><td>RTW_EVENT_JOIN_STATUS</td><td>NULL</td><td>Other join status in @ref rtw_join_status</td></tr>
 *                         <tr><td>RTW_EVENT_CSI_DONE</td><td>NULL</td><td>CSI header and raw data length</td></tr>
 *                         <tr><td>RTW_EVENT_STA_ASSOC</td><td>Association request frame</td><td>0</td></tr>
 *                         <tr><td>RTW_EVENT_STA_DISASSOC</td><td>STA's MAC SoftAP will disassoc</td><td>0</td></tr>
 *                         <tr><td>RTW_EVENT_WPA_STA_WPS_START</td><td>Source MAC of assoc response</td><td>0</td></tr>
 *                         <tr><td>RTW_EVENT_WPA_WPS_FINISH</td><td>NULL</td><td>0</td></tr>
 *                         <tr><td>RTW_EVENT_WPA_EAPOL_START</td><td>Source MAC of assoc response</td><td>0</td></tr>
 *                         <tr><td>RTW_EVENT_WPA_EAPOL_RECVD</td><td>EAPOL message</td><td>0</td></tr>
 *                         </table>
 *                    - \b user_data: User-provided data (see `handler_user_data`).
 * @param[in] handler_user_data :  Optional user-defined data passed to the callback function. Can be NULL.
 * @return  None.
 * @note  Re-registering an `event_id` with a NULL `handler_func` will unregister that event command.
 */
void wifi_reg_event_handler(u32 event_id, void (*handler_func)(u8 *buf, s32 len, s32 flag, void *user_data), void *handler_user_data);

/**
 * @brief  Unregisters an event listener for specific Wi-Fi events.
 * @param[in] event_id : The events from the WiFi driver to stop listening for:
 *                    - @ref RTW_EVENT_STA_ASSOC
 *                    - @ref RTW_EVENT_STA_DISASSOC
 *                    - @ref RTW_EVENT_JOIN_STATUS
 *                    - @ref RTW_EVENT_CSI_DONE
 *                    - @ref RTW_EVENT_WPA_STA_WPS_START
 *                    - @ref RTW_EVENT_WPA_WPS_FINISH
 *                    - @ref RTW_EVENT_WPA_EAPOL_START
 *                    - @ref RTW_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function previously registered for event processing.
 * @return None.
 */
void wifi_unreg_event_handler(u32 event_id, void (*handler_func)(u8 *buf, s32 len, s32 flag, void *user_data));

/** @} End of Event_Functions group*/
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/
#ifdef __cplusplus
}
#endif

#endif //__WIFI_API_EVENT_H

