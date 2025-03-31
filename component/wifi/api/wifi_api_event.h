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
  * @brief  The enumeration lists the disconnet reasons in rtw_event_disconn_info_t when @ref RTW_JOINSTATUS_DISCONNECT happenned.
  */
enum {
#ifndef CONFIG_FULLMAC
	/*Reason code in 802.11 spec, Receive AP's deauth or disassoc after wifi connected*/
	WLAN_REASON_UNSPECIFIED                         = 1,
	WLAN_REASON_PREV_AUTH_NOT_VALID 			        	= 2,
	WLAN_REASON_DEAUTH_LEAVING                      = 3,
	WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY          = 4,
	WLAN_REASON_DISASSOC_AP_BUSY                    = 5,
	WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA       = 6,
	WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA      = 7,
	WLAN_REASON_DISASSOC_STA_HAS_LEFT               = 8,
	WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH          = 9,
	WLAN_REASON_PWR_CAPABILITY_NOT_VALID            = 10,
	WLAN_REASON_SUPPORTED_CHANNEL_NOT_VALID         = 11,
	WLAN_REASON_INVALID_IE                          = 13,
	WLAN_REASON_MICHAEL_MIC_FAILURE                 = 14,
	WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT              = 15,
	WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT            = 16,
	WLAN_REASON_IE_IN_4WAY_DIFFERS                  = 17,
	WLAN_REASON_GROUP_CIPHER_NOT_VALID              = 18,
	WLAN_REASON_PAIRWISE_CIPHER_NOT_VALID           = 19,
	WLAN_REASON_AKMP_NOT_VALID                      = 20,
	WLAN_REASON_UNSUPPORTED_RSN_IE_VERSION          = 21,
	WLAN_REASON_INVALID_RSN_IE_CAPAB                = 22,
	WLAN_REASON_IEEE_802_1X_AUTH_FAILED             = 23,
	WLAN_REASON_CIPHER_SUITE_REJECTED               = 24,
#endif
	/*RTK defined, Driver disconenct from AP after wifi connected and detect something wrong*/
	WLAN_REASON_DRV_BASE                            = 60000,
	WLAN_REASON_DRV_AP_LOSS                         = 60001,
	WLAN_REASON_DRV_AP_CHANGE                       = 60002,
	WLAN_REASON_DRV_BASE_END                        = 60099,

	/*RTK defined, Application layer call some API to cause wifi disconnect*/
	WLAN_REASON_APP_BASE                            = 60100,
	WLAN_REASON_APP_DISCONN                         = 60101,
	WLAN_REASON_APP_CONN_WITHOUT_DISCONN            = 60102,
	WLAN_REASON_APP_BASE_END                        = 60199,

	WLAN_REASON_MAX                                 = 65535,/*0xffff*/
};

/**
  * @brief Flags of @ref WIFI_EVENT_JOIN_STATUS.
  */
enum {
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
enum {
	WIFI_EVENT_STA_ASSOC = 0, 			    	  /**< A station associate to softAP.*/
	WIFI_EVENT_STA_DISASSOC,			        	/**< A station disassociate to softAP.*/
	WIFI_EVENT_JOIN_STATUS,					        /**< Status change during STA connect to AP, see exampe_wifi_event.c.*/
	WIFI_EVENT_CSI_DONE,                    /**< Indicate CSI data is ready, see exampe_wifi_csi.c.*/
	WIFI_EVENT_WPA_STA_WPS_START,           /**< STA WPS start.*/
	WIFI_EVENT_WPA_WPS_FINISH,              /**< STA WPS finish.*/
	WIFI_EVENT_WPA_EAPOL_START,             /**< STA WPA enterprise start.*/
	WIFI_EVENT_WPA_EAPOL_RECVD,             /**< STA WPA enterprise receive a eapol packet.*/
	WIFI_EVENT_MAX,
};
/** @} End of WIFI_Exported_Enumeration_Types group*/

/** @defgroup WIFI_Exported_Structure_Types Structure Type
 * @{
 */
/**
  * @brief Buf of @ref WIFI_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_DISCONNECT.
  */
struct rtw_event_disconn_info_t {
	u16 disconn_reason;      /**< Val: @ref WLAN_REASON_UNSPECIFIED, @ref WLAN_REASON_PREV_AUTH_NOT_VALID...*/
	u8	bssid[6];            /**< AP's MAC address.*/
};

/**
  * @brief Buf of @ref WIFI_EVENT_JOIN_STATUS when flag is @ref RTW_JOINSTATUS_FAIL
  */
struct rtw_event_join_fail_info_t {
	int					fail_reason;           /**< Value: @ref RTK_FAIL, -@ref RTK_ERR_WIFI_CONN_INVALID_KEY...*/
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
 *                    - @ref WIFI_EVENT_STA_ASSOC
 *                    - @ref WIFI_EVENT_STA_DISASSOC
 *                    - @ref WIFI_EVENT_JOIN_STATUS
 *                    - @ref WIFI_EVENT_CSI_DONE
 *                    - @ref WIFI_EVENT_WPA_STA_WPS_START
 *                    - @ref WIFI_EVENT_WPA_WPS_FINISH
 *                    - @ref WIFI_EVENT_WPA_EAPOL_START
 *                    - @ref WIFI_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function which will receive and process the event.
 *                    - \b buf: Event data transmitted from the driver layer to the application layer.
 *                    - \b len: Length of buf.
 *                    - \b flag: Flag set by wifi driver.
 *                    - \b user_data: Equal handler_user_data.
 * @param[in] handler_user_data : User specific data that will be passed directly to the callback function, can set to NULL.
 * @return
 *    - @ref RTK_SUCCESS : If successfully registers the event.
 *    - @ref RTK_FAIL : If an error occurred.
 * @note  Set the same event_cmds with empty handler_func will unregister the event_cmds.
 */
void wifi_reg_event_handler(unsigned int event_cmds, void (*handler_func)(char *buf, int len, int flag, void *user_data), void *handler_user_data);

/**
 * @brief  Un-register the event listener.
 * @param[in] event_cmds : Events indicated by wifi driver.
 *                    - @ref WIFI_EVENT_STA_ASSOC
 *                    - @ref WIFI_EVENT_STA_DISASSOC
 *                    - @ref WIFI_EVENT_JOIN_STATUS
 *                    - @ref WIFI_EVENT_CSI_DONE
 *                    - @ref WIFI_EVENT_WPA_STA_WPS_START
 *                    - @ref WIFI_EVENT_WPA_WPS_FINISH
 *                    - @ref WIFI_EVENT_WPA_EAPOL_START
 *                    - @ref WIFI_EVENT_WPA_EAPOL_RECVD
 * @param[in] handler_func : The callback function which will receive and process the event.
 * @return
 *    - @ref RTK_SUCCESS : If successfully un-registers the event.
 *    - @ref RTK_FAIL : If an error occurred.
 */
void wifi_unreg_event_handler(unsigned int event_cmds, void (*handler_func)(char *buf, int len, int flag, void *user_data));

/** @} End of Event_Functions group*/
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/
#ifdef __cplusplus
}
#endif

#endif //__WIFI_API_EVENT_H

