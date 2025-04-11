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
 * @brief  Enable or disable LPS. LPS is the abbreviation of Leisure Power Save mode.
 * 	Wi-Fi automatically turns RF off during the association to AP if traffic is not busy, while
 *  it also automatically turns RF on to listen to the beacon of the associated AP.
 * @param[in]  enable: It could be TRUE(1) (means enable LPS) or FALSE(0) (means disable LPS).
 * @return
 *    - @ref RTK_SUCCESS : If setting LPS successful.
 *    - @ref RTK_FAIL : Otherwise.
 */
s32 wifi_set_lps_enable(u8 enable);

/**
 * @brief  Set the listen interval of LPS. LPS is the abbreviation of Leisure Power Save mode.
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
 * @return
 *    - @ref RTK_SUCCESS : If setting LPS listen interval successful.
 *    - @ref RTK_FAIL : Otherwise.
 */
s32 wifi_set_lps_listen_interval(u8 interval);


/**
 * @brief  Usually, user can set auto-reconnect in ameba_wificfg.c,
 * If you need to temporarily turn auto-reconnect on or off, this API can help.
 * @param[in]  enable:
 *                 - 1: enable auto reconnect mode.
 *                 - 0: disable auto reconnect mode.
 * @return
 *    - @ref RTK_SUCCESS
 *    - @ref RTK_FAIL : CONFIG_AUTO_RECONNECT in not defined or creating reconnect timer failed.
 */
s32 wifi_set_autoreconnect(u8 enable);

/**
 * @brief  Get the current auto-reconnect settings as a backup so you can restore the previous state after temporarily disabling it.
 * @param[out]  enable: Point to the result of setting reconnection mode.
 *                 - 1: enable the reconnection mode.
 *                 - 0: disable the reconnection mode.
 * @return
 *    - @ref RTK_SUCCESS
 *    - @ref RTK_FAIL : CONFIG_AUTO_RECONNECT in not defined.
 */
s32 wifi_get_autoreconnect(u8 *enable);

/**
 * @brief  Get the connected clients with SoftAP.
 * @param[out]  client_list_buffer: The location where the client list will be stored.
 * @return
 *    - @ref RTK_SUCCESS : The result is successfully got.
 *    - @ref RTK_FAIL : The result is not successfully got.
 */
s32 wifi_ap_get_connected_clients(struct rtw_client_list	*client_list_buffer);

/**
 * @brief  Delete a STA for softap.
 * @param[in]  hwaddr: The pointer to the MAC address of the STA which will be deleted.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note  This function should be used when operating as AP.
 */
s32 wifi_ap_del_client(u8 *hwaddr);

/**
 * @brief  Switch to a new channel in AP mode and using CSA to inform sta.
 * @param[in]  csa_param: Pointer to the csa config structure rtw_csa_parm.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL, only indicate whether channel switch cmd is
 * 	successfully set to wifi driver.
 * @note  This function should be used when operating as AP.
 */
s32 wifi_ap_switch_chl_and_inform(struct rtw_csa_parm *csa_param);

/**
  * @brief  For user to toggle softap whether can be discovered.
  * @param[in]  enable 1-invisible, 0-visible.
  * @return  None.
  */
void wifi_ap_set_invisible(u8 enable);

/**
 * @brief  Set channel.
 * @param[in]  wlan_idx: The wlan interface index, should be @ref SOFTAP_WLAN_INDEX or @ref STA_WLAN_INDEX.
 * @param[in]  channel: The current operating RF channel which will be switch to.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note There are two concepts of channels and they are generally consistent:
 *       1. Interafce(x) operating channel;
 *       2. Network channel: realAP's channel for interafce(0); softap channel for interface(1).
 *       - If you use a wifi_set_channel() to switch interafce(0) operating channel during or after wifi_connect(),
 *         the sta will connect fail or disconnect from realAP due to inconsistency with realAP channel caused by
 *         interface(0) operating channel change;
 *       - If you use a wifi_set_channel() to switch interafce(1) operating channel after softap start,
 *         other devices may not be able to connect to or disconnect from the softap because the interafce(1)
 *         operating channel has changed, resulting in inconsistency with the channel claimed by BCN.
 *         Softap channel switching recommendation: call wifi_ap_switch_chl_and_inform().
 */
s32 wifi_set_channel(u8 wlan_idx, u8 channel);

/**
 * @brief  Set country code.
 * @param[in]  cntcode: Pointer to the country code which want to be set.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 */
s32 wifi_set_countrycode(s8 *cntcode);

/**
 * @brief  get country code info
 * @param[in]  table: Pointer to the currently obtained country code table
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_get_countrycode(struct rtw_country_code_table *table);

/**
 * @brief  get channel list
 * @param[in]  table: Pointer to the currently obtained available channel list
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_get_channel_list(struct rtw_channel_list *ch_list);

/**
 * @brief  Retrieves the current Media Access Control (MAC) address
 *	(or Ethernet hardware address) of the 802.11 device.
 * @param[in]  idx: Get STA or SoftAP mac address. Invalid parameter while setting efuse = 1.
 * @param[in]  mac: Pointer to the struct rtw_mac  which contain obtained mac address.
 * @param[in]  efuse: Get mac address from efuse or get from RAM.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note
 *     - Get mac address inside EFUSE(efuse = 1).
 *     - Get runtime mac address(efuse = 0). (RECOMMENDED)
 */
s32 wifi_get_mac_address(s32 idx, struct rtw_mac  *mac, u8 efuse);

/**
 * @brief  This function is used to get wifi wireless mode for station mode when connecting to AP.
 * @param[out]  wmode: the wireless mode, Val: @ref RTW_80211_B, @ref RTW_80211_A...
 * @return
 *    - @ref RTK_SUCCESS : The result is successfully got.
 *    - @ref RTK_FAIL : The result is not successfully got.
 */
s32 wifi_get_wireless_mode(u8 *wmode);

/**
 * @brief  Set the wireless mode according to the data rate its supported.
 * 	Driver works in @ref RTW_80211_24G_MIX (BGNAX) mode in default after driver initialization. This function is used
 *  to change wireless mode for station mode before connecting to AP.
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
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note  We do not recommend 2G without 11b mode and 5G without 11a mode, as this may lead to compatibility issues.
 */
s32 wifi_set_wireless_mode(u32 wmode);

/** @} End of Basic_Functions group */


/** @addtogroup WIFI_Exported_Extended_Functions Extended Functions
  * @{
  */

/**
 * @brief  Enable promisc mode and set promisc mode.
 * @param[in] enable: Enable or disable promisc mode.
 * @param[in] para: Promisc mode and callback if enable promisc mode.
 * @note Callback return value will decide whether driver need continue process this packet.
 * @return  None.
 */
void wifi_promisc_enable(u32 enable, struct rtw_promisc_para *para);

/**
 * @brief  Check whether current wifi driver is MP(mass production) mode or not.
 * @note  MP mode is used for Wi-Fi & BT performance verification and Wi-Fi & BT parameters
 *        calibration in mass production.
 * @return
 *        - 1: drv_mode is mass production (MP) mode.
 *        - 0: drv_mode is not MP.
 */
u8 wifi_driver_is_mp(void);

/**
 * @brief  Get ccmp unicast key and group key.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  mac_addr: Client mac addr for softap mode, set to NULL for sta mdoe.
 * @param[out]  uncst_key: CCMP unicast key.
 * @param[out]  group_key: CCMP group key.
 * @return
 *    - @ref RTK_SUCCESS : If get key successfully.
 *    - @ref RTK_FAIL : Otherwise.
 */
s32 wifi_get_ccmp_key(u8 wlan_idx, u8 *mac_addr, u8 *uncst_key, u8 *group_key);

/**
 * @brief  Fetch the traffic statistic about wifi.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  traffic_stats: The location where the statistic
 * 	info will be stored, for detail info, please refer to union rtw_traffic_stats .
 * @return
 *    - @ref RTK_SUCCESS : If the statistic info is successfully get.
 *    - @ref RTK_FAIL : If the statistic info is not successfully get.
 */
s32 wifi_get_traffic_stats(u8 wlan_idx, union rtw_traffic_stats *traffic_stats);

/**
 * @brief  Fetch the phy statistic info about wifi.
 * @param[in]  wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX or @ref NONE_WLAN_INDEX.
 * @param[in]  mac_addr: Client mac addr for softap mode, set to NULL for sta mode and none mode.
 * @param[out]  phy_stats: The location where the statistic
 * 	info will be stored, for detail info, please refer to union rtw_phy_stats .
 * @return
 *    - @ref RTK_SUCCESS : If the statistic info is successfully get.
 *    - @ref RTK_FAIL : If the statistic info is not successfully get.
 * @note  The `phy_stats->sta.rssi` and `phy_stats->sta.snr` info will only be valid after connected to AP successfully.
 */
s32 wifi_get_phy_stats(u8 wlan_idx, u8 *mac_addr, union rtw_phy_stats *phy_stats);

/**
 * @brief  Get current remaining number of packets in HW TX buffer.
 * @param[out]  pkt_num: The location where the number of packets will be stored.
 * @return
 *    - @ref RTK_SUCCESS : If the number of packets is successfully get.
 *    - @ref RTK_FAIL : If the number of packets is not successfully get.
 */
s32 wifi_get_txbuf_pkt_num(s32 *pkt_num);

/**
 * @brief  Get antdiv infomation.
 * @param[in]  antdiv_mode: Point to the antdiv_mode value obtained from driver.
 * @param[in]  curr_ant: Point to the antenna value obtained from driver,
 * 	0 means main antenna, 1 means aux antenna.
 * @return  0 if get successfully, otherwise return -1.
 */

s32 wifi_get_antdiv_info(u8 *antdiv_mode, u8 *curr_ant);

//-------------------------------------------------------------//
/**
 * @brief Get band type.
 * @param[out]  band_type: The location where the support band type info will be stored.
 * The support band type:
 *    - @ref RTW_SUPPORT_BAND_2_4G : Only support 2.4G.
 *    - @ref RTW_SUPPORT_BAND_5G : Only support 5G.
 *    - @ref RTW_SUPPORT_BAND_2_4G_5G_BOTH : Support both 2.4G and 5G.
 * @return
 *    - @ref RTK_SUCCESS : If the band type info is successfully get.
 *    - @ref RTK_FAIL : If the band type info is not successfully get.
 */
s32 wifi_get_band_type(u8 *band_type);

/**
 * @brief	Get wifi TSF register[63:32]&[31:0].
 * @param[in]	wlan_idx: @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[out] tsf: The location where the tsf[63:0] will be stored.
 * @return
 *    - @ref RTK_SUCCESS : If the tsf is successfully get.
 *    - @ref RTK_FAIL : If the tsf is not successfully get.
 */
s32 wifi_get_tsf(u8 wlan_idx, u64 *tsf);

/**
 * @brief  Setup custom IE(Information Element) list.
 * @warning  This API can't be executed twice before deleting the previous custom ie list.
 * @param[in]  ie_list: A buffer stores custom IE list, format of custom ie is struct rtw_custom_ie. e.g.
 * @code
 *  u8 ie1[] = {221, 2, 2, 2};
 *  u8 ie2[] = {221, 2, 1, 1};
 *  struct rtw_custom_ie ie_list[2] = {{ie1, RTW_BEACON|RTW_PROBE_RSP}, {ie2, RTW_PROBE_RSP}};
 *  wifi_add_custom_ie(ie_list, 2);
 * @endcode
 * @param[in]  ie_num: The number of custom IEs in `ie_list`.
 * @return  0 if success, otherwise return -1.
 */
s32 wifi_add_custom_ie(struct rtw_custom_ie *ie_list, s32 ie_num);

/**
 * @brief  Update the item in WIFI CUSTOM IE(Information Element) list.
 * @param[in]  cus_ie: Pointer to WIFI CUSTOM IE address.
 * @code
 *  u8 ie[] = {221, 2, 1, 3} ;
 *  struct rtw_custom_ie ie_update = {ie, RTW_PROBE_RSP};
 *  wifi_update_custom_ie(&ie_update, 2);
 * @endcode
 * @param[in]  ie_index: Index of WIFI CUSTOM IE list.
 * @return  0 if success, otherwise return -1.
 */
s32 wifi_update_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_index);

/**
 * @brief  Delete WIFI CUSTOM IE(Information Element) list.
 * @param[in]  wlan_idx: Should be @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @return  0 if success, otherwise return -1.
 */
s32 wifi_del_custom_ie(u8 wlan_idx);

/**
 * @brief  Send raw frame.
 * @param[in]  raw_frame_desc: The pointer of struct rtw_raw_frame_desc,
 * 	which describe related information, include the pointer of raw frame and so on.
 * @return  @ref RTK_FAIL or @ref RTK_SUCCESS.
 */
s32 wifi_send_raw_frame(struct rtw_raw_frame_desc *raw_frame_desc);

/**
 * @brief  Control initial tx rate by different ToS value in IP header.
 * @param[in]  enable: Set 1 to add control for specified tx_rate for
 * 	corresponding ToS_value, set 0 to disable initial rate control for this ToS_value.
 * @param[in]  tos_precedence: Range from 0 to 7, corresponding to IP precedence in TOS field of IP header(BIT7~5).
 * @param[in]  tx_rate: Initial tx rate for packet which has the same ToS value as setted. val: @ref RTW_RATE_1M, @ref RTW_RATE_2M...
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note This function only take IP Precedence(BIT 7~5 in ToS field) into consideration.
 */
s32 wifi_set_tx_rate_by_tos(u8 enable, u8 tos_precedence, u8 tx_rate);

/**
 * @brief  Set EDCA parameter.
 * @param[in]  ac_param: Format is shown as:
 * <table>
 *   <tr><th>BIT31~16</th><th>BIT15~8</th><th>BIT7~0</th></tr>
 *   <tr><td>TXOP Limit</td><td>ECWmin/ECWmax</td><td>ACI/AIFSN</td></tr>
 * </table>
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_set_edca_param(u32 ac_param);

/**
 * @brief  Enable or disable CCA/EDCCA for TX.
 * @param[in]  enable: 1 for enable, 0 for disable.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 * @note Both CCA and EDCCA will be enabled or disabled.
 */
s32 wifi_set_tx_cca_enable(u8 enable);

/**
 * @brief  Set duration and send a CTS2SELF frame.
 * @param[in]  wlan_idx: The wlan interface index, can be @ref STA_WLAN_INDEX or @ref SOFTAP_WLAN_INDEX.
 * @param[in]  duration: The duration value for the CTS2SELF frame.
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
 */
s32 wifi_set_cts2self_duration_and_send(u8 wlan_idx, u16 duration);

/**
 * @brief  Set the csi parameters and enable or disable csi func(sta or softap).
 * @param[in]  act_param: A pointer to the param.
 * @code
 *    struct rtw_csi_action_parm act_param = {0};
 *    act_param.mode = 2;
 *    ...
 *    wifi_csi_config(&act_param);
 * @endcode
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL
 */
s32 wifi_csi_config(struct rtw_csi_action_parm *act_param);

/**
 * @brief  Get csi raw data and csi hdr info.
 * @param[in]  buf_len: Buffer size for getting csi info.
 * @param[in]  csi_buf: The pointer to csi data buffer.
 * @param[in]  len: The size of csi raw data.
 * @code
 *    u8 *csi_buf = NULL;
 *    u32 len;
 *    csi_buf = rtos_mem_zmalloc(buf_len);
 *    wifi_csi_report(buf_len,csi_buf,&len);
 * @endcode
 * @return  @ref RTK_SUCCESS or @ref RTK_FAIL.
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

/** @} End of Extended_Functions group */
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/

#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_EXT_H

