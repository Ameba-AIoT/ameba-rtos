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
 * @brief  Register the event listener.
 * @param[in] event_cmds : Events indicated by wifi driver.
 *                    - @ref RTW_EVENT_STA_ASSOC
 *                    - @ref RTW_EVENT_STA_DISASSOC
 *                    - @ref RTW_EVENT_JOIN_STATUS
 *                    - @ref RTW_EVENT_CSI_DONE
 *                    - @ref RTW_EVENT_WPA_STA_WPS_START
 *                    - @ref RTW_EVENT_WPA_WPS_FINISH
 *                    - @ref RTW_EVENT_WPA_EAPOL_START
 *                    - @ref RTW_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function which will receive and process the event.
 *                    - \b buf: Event data transmitted from the driver layer to the application layer.
 *                    - \b len: Length of buf.
 *                    - \b flag: Flag set by wifi driver.
 *                    - \b user_data: Equal to `handler_user_data`.
 * @param[in] handler_user_data : User specific data that will be passed directly to the callback function, can set to NULL.
 * @return
 *    - @ref RTK_SUCCESS : If successfully registers the event.
 *    - @ref RTK_FAIL : If an error occurred.
 * @note  Set the same `event_cmds` with empty `handler_func` will unregister the `event_cmds`.
 */
void wifi_reg_event_handler(u32 event_cmds, void (*handler_func)(u8 *buf, s32 len, s32 flag, void *user_data), void *handler_user_data);

/**
 * @brief  Un-register the event listener.
 * @param[in] event_cmds : Events indicated by wifi driver.
 *                    - @ref RTW_EVENT_STA_ASSOC
 *                    - @ref RTW_EVENT_STA_DISASSOC
 *                    - @ref RTW_EVENT_JOIN_STATUS
 *                    - @ref RTW_EVENT_CSI_DONE
 *                    - @ref RTW_EVENT_WPA_STA_WPS_START
 *                    - @ref RTW_EVENT_WPA_WPS_FINISH
 *                    - @ref RTW_EVENT_WPA_EAPOL_START
 *                    - @ref RTW_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function which will receive and process the event.
 * @return
 *    - @ref RTK_SUCCESS : If successfully un-registers the event.
 *    - @ref RTK_FAIL : If an error occurred.
 */
void wifi_unreg_event_handler(u32 event_cmds, void (*handler_func)(u8 *buf, s32 len, s32 flag, void *user_data));

/** @} End of Event_Functions group*/
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/
#ifdef __cplusplus
}
#endif

#endif //__WIFI_API_EVENT_H

