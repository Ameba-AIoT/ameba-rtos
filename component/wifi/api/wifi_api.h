/**
  ******************************************************************************
  * @file    wifi_api.h
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

#ifndef __WIFI_API_H
#define __WIFI_API_H

#include "wifi_api_types.h"
#include "wifi_api_event.h"
#include "wifi_api_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/**********************************************************************************************
 *                                     Function Declarations
 *********************************************************************************************/
/** @defgroup WIFI_Exported_Functions Wi-Fi Exported Functions
 * @{
 */
/** @addtogroup WIFI_Exported_Basic_Functions Basic Functions
  * @{
  */
/**
 * @brief  Enable Wi-Fi, i.e., bring the Wireless interface 0 "Up".
 * @param[in]  mode: Should always set to RTW_MODE_STA.
 * @return
 *    - @ref RTK_SUCCESS : If the WiFi chip initialized successfully.
 *    - @ref RTK_FAIL : If the WiFi chip initialization failed.
 * @note
 *    - It is called by default in wifi_init.c.
 *    - In WHC mode as a host, first call whc_host_init to initialize the interface between host and device.
 *    - In WHC mode as a device, this API is automatically called by the host’s wifi_on, so users don’t need to call it again.
 *    - This API performs general initialization and sets interface 0 to STA mode. To start AP mode, call wifi_start_ap after this API.
 */
int wifi_on(u8 mode);

/**
 * @brief  Check if the specified wlan interface  is running.
 * @param[in]  wlan_idx: Can be set as STA_WLAN_INDEX or SOFTAP_WLAN_INDEX.
 * @return  If the function succeeds, the return value is 1.
 * 	Otherwise, return 0.
 * @note  For STA mode, use STA_WLAN_INDEX; for AP mode, use SOFTAP_WLAN_INDEX.
 */
int wifi_is_running(unsigned char wlan_idx);

/**
 * @brief  Join a Wi-Fi network.
 * 	Scan for, associate and authenticate with a Wi-Fi network.
 * @param[in]  connect_param: The pointer of a struct which store the connection
 * 	info, including ssid, bssid, password, etc, for details, please refer to struct
 * 	_rtw_network_info_t in wifi_api_types.h.
 * @param[in]  block:
 *                  - If block is set to 1, it means synchronized wifi connect, and this
 * 	                  API will return until connect is finished;
 *                  - If block is set to 0, it means asynchronized wifi connect, and this
 *                    API will return immediately.
 * @return
 *    - @ref RTK_SUCCESS : Join successfully for synchronized wifi connect,
 *  						or connect cmd is set successfully for asynchronized wifi connect.
 *    - @ref RTK_FAIL : An error occurred.
 *    - @ref RTW_BUSY : Wifi connect or scan is ongoing.
 *    - @ref RTW_NOMEM : Malloc fail during wifi connect.
 *    - @ref RTW_TIMEOUT : More than RTW_JOIN_TIMEOUT(~70s) without successful connection.
 *    - @ref RTW_CONNECT_INVALID_KEY : Password format wrong.
 *    - @ref RTW_CONNECT_SCAN_FAIL : Scan fail.
 *    - @ref RTW_CONNECT_AUTH_FAIL : Auth fail.
 *    - @ref RTW_CONNECT_AUTH_PASSWORD_WRONG : Password error causing auth failure, not entirely accurate.
 *    - @ref RTW_CONNECT_ASSOC_FAIL : Assoc fail.
 *    - @ref RTW_CONNECT_4WAY_HANDSHAKE_FAIL : 4 way handshake fail.
 *    - @ref RTW_CONNECT_4WAY_PASSWORD_WRONG : Password error causing 4 way handshake failure,not entirely accurate.
 * @note
 *      - Please make sure the Wi-Fi is enabled (wifi_on()) before invoking this function.
 *      - If bssid in connect_param is set, then bssid will be used for connect, otherwise ssid is used for connect.
 */
int wifi_connect(struct _rtw_network_info_t *connect_param, unsigned char block);

/**
 * @brief  Disassociates from current Wi-Fi network.
 * @return
 *    - @ref RTK_SUCCESS : On successful disassociation from the AP.
 *    - @ref RTK_FAIL : If an error occurred.
 */
int wifi_disconnect(void);

/**
 * @brief  Get join status during wifi connectection.
 * @param[out] join_status the join status during wifi connectection.
 *    - @ref RTW_JOINSTATUS_UNKNOWN : Unknown.
 *    - @ref RTW_JOINSTATUS_STARTING :	Starting phase.
 *    - @ref RTW_JOINSTATUS_SCANNING : Scanning phase.
 *    - @ref RTW_JOINSTATUS_AUTHENTICATING : Authenticating phase.
 *    - @ref RTW_JOINSTATUS_AUTHENTICATED : Authenticated phase.
 *    - @ref RTW_JOINSTATUS_ASSOCIATING : Associating phase.
 *    - @ref RTW_JOINSTATUS_ASSOCIATED : Associated phase.
 *    - @ref RTW_JOINSTATUS_4WAY_HANDSHAKING : 4 way handshaking phase.
 *    - @ref RTW_JOINSTATUS_4WAY_HANDSHAKE_DONE : 4 way handshake done phase.
 *    - @ref RTW_JOINSTATUS_SUCCESS : Join success.
 *    - @ref RTW_JOINSTATUS_FAIL :	Join fail.
 *    - @ref RTW_JOINSTATUS_DISCONNECT : Disconnect.
 * @return
 *    - @ref RTK_SUCCESS : If the join_status is successfully get.
 *    - @ref RTK_FAIL : If the join_status is not successfully get.
 */
int wifi_get_join_status(u8 *join_status);

/**
 * @brief  Initiate a scan to search for 802.11 networks. There are two different scan type can be confgiured
 *         by the input param block:
 *         - Synchronized scan.
 *         - Asynchronized scan. There are two different ways about how the scan result will be reported:
 *           - The first way is that when scan is done ,the total number of scanned APs will be reported through
 *             scan_user_callback, and the detailed scanned AP infos can be get by calling wifi_get_scan_records(),
 *             so in this way, scan_user_callback need to be registered in scan_param.
 *           - The second way is that every time a AP is scanned, this AP info will be directly reported by
 *             scan_report_each_mode_user_callback, and when scan is done, scan_report_each_mode_user_callback will
 *             report a NULL pointer for notification. So in this way, scan_report_each_mode_user_callback need to
 *             be registered in scan_param, and RTW_SCAN_REPORT_EACH need to be set in scan_param->options.Also in
 *             this mode, scan_user_callback is no need to be registered.
 * @param[in]  scan_param: Refer to struct struct _rtw_scan_param_t in wifi_api_types.h.
 * @param[in]  block:
 * 					- If set to 1, it's synchronized scan and this API will return after scan is done.
 * 					- If set to 0, it's asynchronized scan and this API will return immediately.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL for asynchronized scan, return @ref RTK_FAIL or
 * 	scanned AP number for Synchronized scan.
 * @note  If this API is called, the scanned APs are stored in WiFi driver dynamic
 * 	allocated memory, for synchronized scan or asynchronized scan which not use RTW_SCAN_REPORT_EACH,
 * 	these memory will be freed when wifi_get_scan_records() is called.
 */
int wifi_scan_networks(struct _rtw_scan_param_t *scan_param, unsigned char block);

/**
 * @brief  Get scan results
 * @param[inout]  ap_num: Input the pointer to the number of scanned ap info which
 * 	want to get, output the number of scanned ap info whicn can actually get.
 * @param[in]  ap_list: A buffer stores scanned ap list, the scanned AP info
 * 	will be stored one by one in form of struct rtw_scan_result.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note
 *     - For synchronized scan or asynchronized scan which do not config RTW_SCAN_REPORT_EACH,
 *       if once called wifi_scan_networks() but not use this API to get scanned AP info,
 * 	     driver memory for these scanned AP will not be freed until next time
 * 	     wifi_scan_networks() is called.
 *     - For asynchronized scan which config RTW_SCAN_REPORT_EACH, every time a
 * 	     AP is scanned, the AP info will be directly reported through scan_report_each_mode_user_callback
 * 	     and freed after user callback executed, thus there is no need to use this function to get scan result.
 */
int wifi_get_scan_records(unsigned int *ap_num, struct rtw_scan_result *ap_list);

/**
 * @brief  Trigger Wi-Fi driver to start an infrastructure Wi-Fi network.
 * @param[in]  softap_config: The pointer of a struct which store the softAP
 * 	configuration, please refer to struct _rtw_softap_info_t in wifi_api_types.h.
 * @warning  If a STA interface is active when this function is called,
 * 	the softAP will start on the same channel as the STA.
 * 	It will NOT use the channel provided!
 * @return
 *    - @ref RTK_SUCCESS : If successfully creates an AP.
 *    - @ref RTK_FAIL : If an error occurred.
 * @note
 *     - If hidden_ssid in softap_config is set to 1, then this softAP will start
 * 	     with hidden ssid.
 *     - Please make sure the Wi-Fi is enabled (wifi_on()) before invoking this function.
 */
int wifi_start_ap(struct _rtw_softap_info_t *softap_config);

/**
 * @brief  Disable Wi-Fi interface-2.
 * @return
 *    - @ref RTK_SUCCESS : Deinit success, wifi mode is changed to RTW_MODE_STA.
 *    - @ref RTK_FAIL : Otherwise.
 */
int wifi_stop_ap(void);

/**
 * @brief  Get current Wi-Fi setting from driver.
 * @param[in]  wlan_idx: STA_WLAN_IDX or SOFTAP_WLAN_IDX.
 * @param[out]  psetting: Points to the struct _rtw_wifi_setting_t structure which information is gotten.
 * @return
 *    - @ref RTK_SUCCESS : The result is successfully got.
 *    - @ref RTK_FAIL : The result is not successfully got.
 * @note  The mode in struct _rtw_wifi_setting_t corresponding to the wifi mode of current wlan_idx:
 *      - If in station mode, the info in struct _rtw_wifi_setting_t except mode will correspond to the AP it connected,
 *      - If in AP mode, the info in struct _rtw_wifi_setting_t will correspond to the softAP itself.
 */
int wifi_get_setting(unsigned char wlan_idx, struct _rtw_wifi_setting_t *psetting);

/** @} End of Basic_Functions group */
/** @} End of WIFI_Exported_Functions group */
/** @} End of WIFI_API group */
#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_H
