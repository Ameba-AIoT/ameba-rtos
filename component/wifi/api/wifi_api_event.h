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
  * @brief Flags of @ref RTW_EVENT_JOIN_STATUS.
  */
enum rtw_join_status {
	RTW_JOINSTATUS_UNKNOWN = 0,

	/* The intermediate states of Linking should be added in front of @ref RTW_JOINSTATUS_SUCCESS */
	RTW_JOINSTATUS_STARTING,				        /**< Starting phase. */
	RTW_JOINSTATUS_SCANNING,				        /**< Scanning phase. */
	RTW_JOINSTATUS_AUTHENTICATING,	    		/**< Authenticating phase. */
	RTW_JOINSTATUS_AUTHENTICATED,			      /**< Authenticated phase. */
	RTW_JOINSTATUS_ASSOCIATING,			       	/**< Associating phase. */
	RTW_JOINSTATUS_ASSOCIATED,			      	/**< Associated phase. */
	RTW_JOINSTATUS_4WAY_HANDSHAKING,	    	/**< 4 way handshaking phase. */
	RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE,	    /**< 4 way handshake done phase. */
	RTW_JOINSTATUS_SUCCESS,				         	/**< Join success.  */

	/* The other result states of Linking should be added in back of @ref RTW_JOINSTATUS_SUCCESS */
	RTW_JOINSTATUS_FAIL,						        /**< Join fail during wifi connect. */
	RTW_JOINSTATUS_DISCONNECT,			      	/**< Disconnect after wifi connected.*/
};

/**
  * @brief  event_cmds
  */
enum rtw_event_id {
	RTW_EVENT_STA_ASSOC = 0, 			    	  /**< A station associate to softAP.*/
	RTW_EVENT_STA_DISASSOC,			        	/**< A station disassociate to softAP.*/
	RTW_EVENT_JOIN_STATUS,					        /**< Status change during STA connect to AP, see exampe_wifi_event.c.*/
	RTW_EVENT_CSI_DONE,                    /**< Indicate CSI data is ready, see exampe_wifi_csi.c.*/
	RTW_EVENT_WPA_STA_WPS_START,           /**< STA WPS start.*/
	RTW_EVENT_WPA_WPS_FINISH,              /**< STA WPS finish.*/
	RTW_EVENT_WPA_EAPOL_START,             /**< STA WPA enterprise start.*/
	RTW_EVENT_WPA_EAPOL_RECVD,             /**< STA WPA enterprise receive a eapol packet.*/
	RTW_EVENT_MAX,
};
/** @} End of WIFI_Exported_Enumeration_Types group*/

/** @defgroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */
/**
  * @brief Buf of @ref RTW_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_DISCONNECT.
  */
struct rtw_event_info_joinstatus_disconn {
	u16 disconn_reason;      /**< Val: @ref RTW_DISCONN_RSN_80211_UNSPECIFIED, @ref RTW_DISCONN_RSN_80211_PREV_AUTH_NOT_VALID...*/
	u8	bssid[6];            /**< AP's MAC address.*/
};

/**
  * @brief Buf of @ref RTW_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_FAIL
  */
struct rtw_event_info_joinstatus_joinfail {
	s32					fail_reason;           /**< Value: @ref RTK_FAIL, -@ref RTK_ERR_WIFI_CONN_INVALID_KEY...*/
	u16					reason_or_status_code; /**< From AP, define in 802.11 spec.*/
	u8					bssid[6];              /**< AP's MAC address.*/
};
/** @} End of WIFI_Exported_Structure_Types group*/
/** @} End of WIFI_Exported_Types group*/

/**********************************************************************************************
 *                                     Function Declarations
 *********************************************************************************************/
/** @defgroup WIFI_Exported_Functions Wi-Fi Exported Functions
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

