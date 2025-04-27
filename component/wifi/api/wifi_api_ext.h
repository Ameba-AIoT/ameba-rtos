/**
  ******************************************************************************
  * @file    wifi_api_ext.h
  * @author
  * @version
  * @date
  * @brief   This file provides user interface for Wi-Fi station and AP mode
  *          configuration base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_API_EXT_H
#define __WIFI_API_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @defgroup WIFI_Exported_Functions Wi-Fi Exported Functions
 * @{
 */
/** @addtogroup WIFI_Exported_Basic_Functions Basic Functions
  * @{
  */
/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * @brief  Abort onoging wifi scan.
 * @param[in]  block: Set 1 for wait scan actually aborted.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note
 *     - If `block` set to 0, this will be an asynchronized function and will return immediately,
 * 	     return value only indicates whether the scan abort cmd is successfully notified to driver or not.
 *	   - If `block` set to 1, this will be a synchronized function and will return when scan is actually aborted.
 * 	     When scan is actually aborted, the user callback registered in wifi_scan_networks()
 * 	     will be executed. If there is no wifi scan in progress, this function will just return
 * 	     @ref RTK_SUCCESS and user callback won't be executed.
 */
s32 wifi_scan_abort(u8 block);

/**
 * @brief  Enable or disable LPS. LPS is the abbreviation of Legacy Power Save mode. When enable LPS,
 * 	Wi-Fi automatically turns RF off during the association to AP if traffic is not busy, while
 *  it also automatically turns RF on to listen to the beacon of the associated AP.
 * @param[in]  enable: It could be TRUE(1) (means enable LPS) or FALSE(0) (means disable LPS).
 * @return  @ref RTK_SUCCESS : The API executed successfully.
 * @note  If a SoftAP interface is active when this function is called, calling this API does not work.
 */
s32 wifi_set_lps_enable(u8 enable);

/**
 * @brief  Set the listen interval of LPS. LPS is the abbreviation of Legacy Power Save mode.
 * 	Wi-Fi automatically turns RF off during the association to AP	if traffic is not busy, while
 *  it also automatically turns RF on to listen to the beacon of the associated AP.
 *  The listen interval is used to indicate how often the STA wakes to listen to beacons of AP.
 * @param[in]  interval: The listen interval of LPS (unit: 102.4 ms).
 * @note If the specified interval is not a multiple of beacon interval, the actual listen interval
 *	     will be rounded to the largest multiple that <= the specified value. It will not be
 *	     rounded to 0 when the specified value < beacon interval. In this case, use 1 instead. e.g.
 *       Given that beacon interval = 2:
 *       - Calling wifi_set_lps_listen_interval(5) will make actual listen interval = 4.
 *       - Calling wifi_set_lps_listen_interval(1) will make actual listen interval = 1.
 * @return  @ref RTK_SUCCESS : The API executed successfully.
 */
s32 wifi_set_lps_listen_interval(u8 interval);


/**
 * @brief  Set the auto-reconnect mode for Wi-Fi connection.
 * This function allows temporarily enabling or disabling auto-reconnect mode,
 * overriding the default setting in ameba_wificfg.c.
 * @param[in]  enable:
 *                 - 1: Enable auto-reconnect mode.
 *                 - 0: Disable auto-reconnect mode.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : CONFIG_AUTO_RECONNECT in not defined or creating reconnect timer failed.
 */
s32 wifi_set_autoreconnect(u8 enable);

/**
 * @brief  Get current auto-reconnect setting which can be used to backup the
 * setting before temporarily changing it.
 * @param[out]  enable: Pointer to store auto-reconnect mode.
 *                 - 1: Enable auto-reconnect mode.
 *                 - 0: Disable auto-reconnect mode.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : CONFIG_AUTO_RECONNECT not defined.
 */
s32 wifi_get_autoreconnect(u8 *enable);

/**
 * @brief  Get the list of clients connected to the SoftAP.
 * @param[out]  client_list_buffer: Pointer to store the number of connected clients and their MAC addresses.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : SoftAP is not enabled.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `client_list_buffer`.
 */
s32 wifi_ap_get_connected_clients(struct rtw_client_list	*client_list_buffer);

/**
 * @brief  Disconnect a client from the SoftAP.
 * @param[in]  hwaddr: Pointer to the MAC address of the client to be disconnected.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : SoftAP is not enabled or no clients are currently connected to SoftAP.
 *    - -@ref RTK_ERR_BADARG :
 *      - `hwaddr` is brodcast MAC address (ff:ff:ff:ff:ff:ff).
 *      - The client with the specified MAC address `hwaddr` not connected to the SoftAP.
 */
s32 wifi_ap_del_client(u8 *hwaddr);

/**
 * @brief  Change the operating channel in AP mode and notify connected clients using CSA (Channel Switch Announcement).
 * @param[in]  csa_param: Pointer to the CSA configuration structure rtw_csa_parm.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : SoftAP is not enabled.
 *    - -@ref RTK_ERR_BADARG : The specified channel `csa_param->new_chl` is invalid.
 */
s32 wifi_ap_switch_chl_and_inform(struct rtw_csa_parm *csa_param);

/**
  * @brief  Toggle whether SoftAP can be discovered.
  * @param[in]  enable: 1-invisible, 0-visible.
  * @return  None
  * @note  SoftAP becomes invisible by pausing tx beacon and not responsing to probe request.
  */
void wifi_ap_set_invisible(u8 enable);

/**
 * @brief  Set the operating channel for a Wi-Fi interface.
 * @param[in]  wlan_idx: @ref SOFTAP_WLAN_INDEX or @ref STA_WLAN_INDEX.
 * @param[in]  channel: RF channel to switch to.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Wi-Fi is powered off in IPS(Inactive Power Save) mode, unable to access Wi-Fi registers.
 *    - -@ref RTK_ERR_BADARG : Invalid `channel` or `wlan_idx`(when set to @ref SOFTAP_WLAN_INDEX but SoftAP is not enabled).
 * @note
 *    - There are two channel concepts, generally consistent:
 *      1. The actual operating channel of the interface.
 *      2. The channel declared in the beacon of the connected AP on interface(0) or SoftAP on interface(1).
 *    - Recommended to call this function before the STA is connected or the SoftAP is started, beacuse:
 *      - Changing STA channel during/after connection may cause connection failure or disconnection due to
 *        channel inconsistencies with the AP.
 *      - Changing SoftAP channel after start may disrupt client connections due to beacon channel mismatch.
 *        **For SoftAP channel switching, use wifi_ap_switch_chl_and_inform() instead.**
 *    - When MCC (Multi-Channel Concurrent) feature is not enabled, both SoftAP (if enabled) and STA use the same operating channel:
 *      - Changing SoftAP channel while the STA is connected will have no effect.
 *      - Changing STA channel will also change SoftAP channel.
 */
s32 wifi_set_channel(u8 wlan_idx, u8 channel);

/**
 * @brief  Set the country code for Wi-Fi regulation.
 * @param[in]  cntcode:  Pointer to the country code to be set.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG : Invalid country code. Refer to ameba_wifi_country_code_table_usrcfg.c for valid codes.
 */
s32 wifi_set_countrycode(u8 *cntcode);

/**
 * @brief  Retrieve current country code information.
 * @param[in]  table: Pointer to store the country code table containing country code,
 *                    channel plan code, and TX power limit index.
 * @return  @ref RTK_SUCCESS : The API executed successfully.
 */
s32 wifi_get_countrycode(struct rtw_country_code_table *table);

/**
 * @brief  Get the list of available WiFi channels for the current regulatory domain.
 * @param[in]  ch_list: Pointer to store the available channel list.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `ch_list`.
 */
s32 wifi_get_channel_list(struct rtw_channel_list *ch_list);

/**
 * @brief  Retrieves the current Media Access Control (MAC) address
 *	(or Ethernet hardware address) of the 802.11 device.
 * @param[in]  idx: Index to specify STA or SoftAP MAC address (ignored if `efuse = 1`).
 * @param[in]  mac: Pointer to store the obtained MAC address.
 * @param[in]  efuse: 1 to read from EFUSE, 0 to read from RAM (RECOMMENDED).
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Driver internal error.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `mac`.
 * @note  Reading the runtime MAC address from RAM (`efuse = 0`) is recommended for most use cases.
 */
s32 wifi_get_mac_address(s32 idx, struct rtw_mac *mac, u8 efuse);

/**
 * @brief  Get the current Wi-Fi wireless mode for station mode when connected to an AP.
 * @param[out]  wmode: Pointer to store the wireless mode (e.g., @ref RTW_80211_B, @ref RTW_80211_A, etc.).
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : STA not connected to AP.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `wmode`.
 */
s32 wifi_get_wireless_mode(u8 *wmode);

/**
 * @brief  Set the wireless mode.
 * @param[in]  wmode: wireless mode to set. The value can be
 *               	- @ref RTW_80211_B
 *               	- @ref RTW_80211_A
 *               	- @ref RTW_80211_G
 *               	- @ref RTW_80211_N
 *               	- @ref RTW_80211_AC
 *               	- @ref RTW_80211_AX
 *               	- @ref RTW_80211_BG
 *               	- @ref RTW_80211_GN
 *               	- @ref RTW_80211_AN
 *               	- @ref RTW_80211_BN
 *               	- @ref RTW_80211_BGN
 *               	- @ref RTW_80211_BGAX
 *               	- @ref RTW_80211_GAX
 *               	- @ref RTW_80211_A_AC
 *               	- @ref RTW_80211_A_AX
 *               	- @ref RTW_80211_AGN
 *               	- @ref RTW_80211_ABGN
 *               	- @ref RTW_80211_ANAC
 *               	- @ref RTW_80211_24G_MIX
 *               	- @ref RTW_80211_5G_MIX
 *               	- @ref RTW_80211_MAX
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG : Invalid `wmode` provided.
 * @note
 *    - Default mode after driver initialization is @ref RTW_80211_24G_MIX (BGNAX).
 *    - Use this function to change wireless mode for station mode before connecting to AP.
 *    - Avoid using 2.4GHz modes without 11b or 5GHz modes without 11a to prevent compatibility issues.
 */
s32 wifi_set_wireless_mode(u32 wmode);

/** @} End of Basic_Functions group */


/** @addtogroup WIFI_Exported_Extended_Functions Extended Functions
  * @{
  */

/**
 * @brief  Enable or disable promisc mode.
 * @param[in] enable: 1 to enable, 0 to disable promisc mode.
 * @param[in] para: Pointer to a struct containing filter mode and callback if enable promisc mode.
 *                 - Filter mode: Receive all packets unconditionally or only packets from the connected AP.
 *                 - Callback: Provides details of the received packets. The return value of the callback determines
 *                   whether the driver should continue processing the packet.
 * @note  Enabling promisc mode temporarily disables LPS(Legacy Power Save) and IPS(Inactive Power Save).
 *        Original power save settings are restored when promisc mode is disabled.
 * @return  None.
 */
void wifi_promisc_enable(u32 enable, struct rtw_promisc_para *para);

/**
 * @brief  Check if the Wi-Fi driver is in Mass Production (MP) mode.
 * @note  MP mode is used for Wi-Fi & Bluetooth performance verification and parameter calibration during mass production.
 * @return
 *        - 1: Driver is MP mode.
 *        - 0: Driver is not in MP mode.
 */
u8 wifi_driver_is_mp(void);

/**
 * @brief  Get CCMP unicast and group keys.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  mac_addr: Pointer to client MAC address for SoftAP mode, NULL for STA mdoe.
 * @param[out]  uncst_key: Buffer to store CCMP unicast key.
 * @param[out]  group_key: Buffer to store CCMP group key.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Pairwise encryption cipher is not CCMP between STA and connected AP or SoftAP.
 *    - -@ref RTK_ERR_BADARG :
 *      - NULL pointer passed for `uncst_key` or `group_key`.
 *      - `wlan_idx` set to @ref SOFTAP_WLAN_INDEX but NULL pointer passed for `mac_addr`.
 *      - `wlan_idx` set to @ref STA_WLAN_INDEX but STA not connected to AP.
 */
s32 wifi_get_ccmp_key(u8 wlan_idx, u8 *mac_addr, u8 *uncst_key, u8 *group_key);

/**
 * @brief  Fetch the traffic statistics for the specified Wi-Fi interface.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  traffic_stats: Pointer to union rtw_traffic_stats to store the statistics.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG :
 *      - `wlan_idx` set to @ref SOFTAP_WLAN_INDEX but SoftAP is not enabled.
 *      - NULL pointer passed for `traffic_stats`.
 */
s32 wifi_get_traffic_stats(u8 wlan_idx, union rtw_traffic_stats *traffic_stats);

/**
 * @brief  Fetch Wi-Fi related physical layer information.
 * @param[in]  wlan_idx: The Wi-Fi interface:
 *                      - @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX : Get RSSI for the interface.
 *                      - @ref NONE_WLAN_INDEX : Get channel loading measurement.
 * @param[in]  mac_addr: Client MAC address for SoftAP mode, NULL for STA and NONE mode.
 * @param[out]  phy_stats: Pointer to union rtw_phy_stats to store the information.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG :
 *      - NULL pointer passed for `traffic_stats`.
 *      - Set `wlan_idx` to @ref SOFTAP_WLAN_INDEX but SoftAP is not enabled.
 *      - Set `wlan_idx` to @ref SOFTAP_WLAN_INDEX but the STA with `mac_addr` not connected to SoftAP.
 * @note  For STA mode, all fields in `phy_stats->sta` will be 0 if the STA is not successfully connected to an AP.
 */
s32 wifi_get_phy_stats(u8 wlan_idx, u8 *mac_addr, union rtw_phy_stats *phy_stats);

/**
 * @brief  Get the number of packets remaining in the hardware TX buffer.
 * @param[out]  pkt_num: Pointer to store the number of packets.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Wi-Fi is powered off in IPS(Inactive Power Save) mode, unable to access Wi-Fi registers.
 * @note Supported only by RTL8730E chip. Returns @ref RTK_FAIL for other chips.
 */
s32 wifi_get_txbuf_pkt_num(s32 *pkt_num);

/**
 * @brief  Get antenna diversity infomation.
 * @param[out]  antdiv_mode: Pointer to store the antenna diversity mode.
 *                           Values: @ref RTW_ANTDIV_AUTO, @ref RTW_ANTDIV_FIX_MAIN, etc.
 * @param[out]  curr_ant: Pointer to store the current antenna in use.
 *                        0: Main antenna, 1: Auxiliary antenna.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : CONFIG_WIFI_ANTDIV is not defined.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `antdiv_mode` or `curr_ant`.
 */
s32 wifi_get_antdiv_info(u8 *antdiv_mode, u8 *curr_ant);

//-------------------------------------------------------------//
/**
 * @brief Get the supported frequency band type.
 * @param[out]  band_type: Pointer to store the supported band type. Values:
 *                       - @ref RTW_SUPPORT_BAND_2_4G : 2.4GHz only
 *                       - @ref RTW_SUPPORT_BAND_5G : 5GHz only
 *                       - @ref RTW_SUPPORT_BAND_2_4G_5G_BOTH : Both 2.4GHz and 5GHz
 * @return  @ref RTK_SUCCESS : The API executed successfully.
 */
s32 wifi_get_band_type(u8 *band_type);

/**
 * @brief	Get wifi TSF register value (64-bit).
 * @param[in]	wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[out] tsf: Pointer to store the 64-bit TSF value.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Wi-Fi is powered off in IPS(Inactive Power Save) mode, unable to access Wi-Fi registers.
 */
s32 wifi_get_tsf(u8 wlan_idx, u64 *tsf);

/**
 * @brief  Set up custom Information Element (IE) list.
 * @warning  This API cannot be called twice without deleting the previous custom IE list.
 * @param[in]  ie_list: Buffer containing custom IE list. Each element should be of type struct rtw_custom_ie.
 * @param[in]  ie_num: Number of custom IEs in `ie_list`.
 * @code
 *  u8 ie1[] = {221, 2, 2, 2};
 *  u8 ie2[] = {221, 2, 1, 1};
 *  struct rtw_custom_ie ie_list[2] = {{ie1, RTW_CUS_IE_BEACON|RTW_CUS_IE_PROBERSP}, {ie2, RTW_CUS_IE_PROBERSP}};
 *  wifi_add_custom_ie(ie_list, 2);
 * @endcode
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Previous custom IE list not deleted.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `ie_list` or incompatible frame types specified
 *      (e.g. set @ref RTW_CUS_IE_BEACON | @ref RTW_CUS_IE_PROBEREQ to `ie_list->type`).
 * @note The Wi-Fi interface is determined by the sepecified frame type in `ie_list->type`:
 *       - For SoftAP: Use @ref RTW_CUS_IE_BEACON or @ref RTW_CUS_IE_PROBERSP.
 *       - For STA: Use @ref RTW_CUS_IE_PROBEREQ or @ref RTW_CUS_IE_ASSOCREQ.
 */
s32 wifi_add_custom_ie(struct rtw_custom_ie *ie_list, s32 ie_num);

/**
 * @brief  Update an item in Wi-Fi custom Information Element (IE) list.
 * @param[in]  cus_ie: Pointer to the struct containing details of the IE to be updated.
 * @param[in]  ie_index: Index of the IE to be updated in the custom IE list.
 * @code
 *  u8 ie[] = {221, 2, 1, 3} ;
 *  struct rtw_custom_ie ie_update = {ie, RTW_CUS_IE_PROBERSP};
 *  wifi_update_custom_ie(&ie_update, 2);
 * @endcode
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - -@ref RTK_ERR_BADARG :
 *      - NULL pointer passed for `cus_ie`.
 *      - Incompatible frame types specified (e.g. set @ref RTW_CUS_IE_BEACON | @ref RTW_CUS_IE_PROBEREQ to `cus_ie->type`).
 *      - The IE at `ie_index` does not exist in the list.
 * @note  Ensure the IE is added by wifi_add_custom_ie() before calling this function.
 */
s32 wifi_update_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_index);

/**
 * @brief  Delete Wi-Fi custom Information Element (IE) list.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : No IE list has been added before.
 */
s32 wifi_del_custom_ie(u8 wlan_idx);

/**
 * @brief  Send raw frame without upper layer protocol processing or encapsulation.
 * @param[in]  raw_frame_desc: Pointer of struct rtw_raw_frame_desc, describing raw frame
 *             details such as raw data pointer and transmission rate.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Driver internal error.
 *    - -@ref RTK_ERR_BADARG : NULL pointer passed for `raw_frame_desc`.
 */
s32 wifi_send_raw_frame(struct rtw_raw_frame_desc *raw_frame_desc);

/**
 * @brief  Control initial tx rate based on ToS value in IP header.
 * @param[in]  enable: Set to 1 to enable, 0 to disable initial rate control for this ToS value.
 * @param[in]  tos_precedence: IP precedence in TOS field (bits 7~5), range 0~7.
 * @param[in]  tx_rate: Initial tx rate for packets which with matching ToS. Values: @ref RTW_RATE_1M, @ref RTW_RATE_2M, etc.
 * @return  @ref RTK_SUCCESS : The API executed successfully.
 * @note
 *      - Only considers IP Precedence (bits 7~5 in ToS field).
 *      - Applies to both SoftAP (if enabled) and STA modes.
 */
s32 wifi_set_tx_rate_by_tos(u8 enable, u8 tos_precedence, u8 tx_rate);

/**
 * @brief  Set EDCA parameters for STA.
 * @param[in]  ac_param: EDCA parameters format (as per 802.11 spec):
 * <table>
 *   <tr><th>BIT31~16</th><th>BIT15~8</th><th>BIT7~0</th></tr>
 *   <tr><td>TXOP Limit</td><td>ECWmin/ECWmax</td><td>ACI/AIFSN</td></tr>
 * </table>
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Wi-Fi is powered off in IPS(Inactive Power Save) mode,
 *                      unable to access Wi-Fi registers.
 */
s32 wifi_set_edca_param(u32 ac_param);

/**
 * @brief  Enable or disable CCA/EDCCA for TX.
 * @param[in]  enable: 1 to enable, 0 to disable.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Wi-Fi is powered off in IPS(Inactive Power Save) mode,
 *                      unable to access Wi-Fi registers.
 * @note
 *      - Both CCA and EDCCA will be simultaneously enabled or disabled.
 *      - Applies to both SoftAP (if enabled) and STA modes.
 */
s32 wifi_set_tx_cca_enable(u8 enable);

/**
 * @brief  Set duration by sending a CTS2SELF frame.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  duration: Duration value for the CTS2SELF frame.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Driver internal error.
 */
s32 wifi_set_cts2self_duration_and_send(u8 wlan_idx, u16 duration);

/**
 * @brief   Configure and control the CSI (Channel State Information) functionality for STA or SoftAP.
 * @param[in]  act_param: Pointer to the configuration parameters.
 *                      - Set `act_param->act` to @ref RTW_CSI_ACT_CFG to configure CSI parameters.
 *                      - Set `act_param->act` to @ref RTW_CSI_ACT_EN to enable/disable CSI.
 *                        If disabled, reconfigure parameters before re-enabling.
 * @code
 *    // config csi parameters and enable wifi csi
 *    struct rtw_csi_action_parm act_param = {0};
 *    act_param.mode = RTW_CSI_MODE_RX_RESP;
 *    // ... (set other parameters)
 *    act_param.act = RTW_CSI_ACT_CFG;  //csi cfg
 *    wifi_csi_config(&act_param);
 *    act_param.act = RTW_CSI_ACT_EN;  //csi en
 *    act_param.enable = 1;
 *    wifi_csi_config(&act_param);
 * @endcode
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : CONFIG_CSI is not defined or driver internal error.
 *    - -@ref RTK_ERR_BADARG :
 *      - Invalid `act_param->mac_addr`, such as not set `mac_addr` when not use active CSI broadcast mode
 *        or the specified STA with `mac_addr` not connected the SoftAP when SoftAP enabled.
 *      - Invalid `act_param->data_rate`, such as use non-OFDM/HT rate or set `act_param->data_bw = 1`(i.e. 40M) while use OFDM rate.
 *      - SoftAP is not enabled, nor is the STA connected to an AP.
 * @note
 *     - If the SoftAP is enabled, Wi-Fi CSI function will only be initiated by the SoftAP.
 *     - 40MHz bandwidth (`act_param->data_bw = 1`) is supported only on RTL8721Dx chip.
 */
s32 wifi_csi_config(struct rtw_csi_action_parm *act_param);

/**
 * @brief  Get CSI raw data and header information.
 * @param[in]  buf_len: Size of the buffer to store CSI information.
 * @param[in]  csi_buf: Pointer to the CSI data buffer.
 * @param[in]  len:  Pointer to the size of CSI raw data.
 * @code
 *    u8 *csi_buf = NULL;
 *    u32 len;
 *    csi_buf = rtos_mem_zmalloc(buf_len);
 *    wifi_csi_report(buf_len,csi_buf,&len);
 * @endcode
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : CONFIG_CSI is not defined or driver internal error.
 *    - -@ref RTK_ERR_BADARG :
 *      - NULL pointer passed for `csi_buf`.
 *      - CSI packet length (CSI header + raw data) exceeds `buf_len`.
 */
s32 wifi_csi_report(u32 buf_len, u8 *csi_buf, u32 *len);

/**
 * @brief  For wifi speaker setting.
 * @param[in]  set_type: Wifi speaker setting type:
 *                - @ref RTW_SPEAKER_SET_INIT
 *                - @ref RTW_SPEAKER_SET_LATCH_I2S_COUNT
 *                - @ref RTW_SPEAKER_SET_TSF_TIMER
 * @param[in]  settings: A pointer to the params:
 *                     - When `set_type` == @ref RTW_SPEAKER_SET_INIT
 *                       - `settings->init.mode`: 0 for slave, 1 for master.
 *                       - `settings->init.thresh`: Unit 128us.
 *                       - `settings->init.relay_en`: Relay control.
 *                     - When `set_type` == @ref RTW_SPEAKER_SET_LATCH_I2S_COUNT
 *                       - `settings->latch_i2s_count.port`: 0 for select port 0's TSFT to trigger audio latch count, 1 for port 1.
 *                       - `settings->latch_i2s_count.latch_period`: 0 for trigger audio latch period is 4.096ms, 1 for 8.192ms.
 *                     - When `set_type` == @ref RTW_SPEAKER_SET_TSF_TIMER
 *                       - `settings->tsf_timer.enable`: 1 for enable twt timer, 0 for disable.
 *                       - `settings->tsf_timer.tsft`: Absolute value for twt timer, unit ms.
 *                       - `settings->tsf_timer.port`: 0 for select port 0's TSFT to trigger twt timer interrupt, 1 for port 1.
 * @return  None.
 */
void wifi_speaker_setting(u8 set_type, union rtw_speaker_set *settings);

/**
 * @brief  For user to set tx power.
 * 1. Currently will TX with the set power,  regardless of power by rate and power by limit.
 * 2. Afterwards, it can be extended to specify rate, or power by limit needs to be considered.
 * @param[in]  txpwr_ctrl_info: The pointer of rtw_tx_power_ctl_info:
 *                            - \b b_tx_pwr_force_enbale: 1 for enable, 0 for disable.
 *                            - \b tx_pwr_force: Unit 0.25dBm.
 * @note
 *    - For amebadplus, the power range varies for different channels or IC, the recommended power range is -2 ~ 23 dBm,
 *      if exceeds the power range, the power may be inaccurate, and will be changed to the boundary value.
 *    - For amebasmart&amebalite, the recommended power range is -24 ~ 24 dBm.
 *    - For both, we suggest setting the power not to exceed the power by rate table.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_set_tx_power(struct rtw_tx_power_ctl_info *txpwr_ctrl_info);

/**
 * @brief  For user to get tx power.
 * @param[in]  rate: Phy rate, val: @ref RTW_RATE_1M, @ref RTW_RATE_2M...
 *                 - CCK rate 1M,2M,5.5M,11M
 *                 - OFDM rate 6M,9M,12M,18M,24M,36M,48M,54M
 *                 - HT rate MCS0~MCS7
 *                 - VHT rate MCS0~MCS8
 *                 - HE rate MCS0~MCS9
 * @param[out]  txpwr: The current tx power, unit 0.25dBm.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_get_tx_power(u8 rate, s8 *txpwr);

/**
  * @brief  For user to config retry limit for different stages of join.
  * @param[in]  conn_step_retries: The pointer of rtw_conn_step_retries.
  * @return  None.
  */
void wifi_set_conn_step_try_limit(struct rtw_conn_step_retries *conn_step_retries);

/**
  * @brief  Automatically select a better channel based on environmental conditions.
  * @param[in]  acs_config: configure acs parameters
  * @param[out] ideal_ch
  * @return
  *    - @ref RTK_SUCCESS : The API executed successfully.
  *    - @ref RTK_FAIL :
  *      - Scan fail.
  *      - Get acs report fail
  */
s32 wifi_acs_find_ideal_channel(struct rtw_acs_config *acs_config, u8 *ideal_ch);

/** @} End of Extended_Functions group */
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/

#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_EXT_H

