// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_FUNCTIONS_H__
#define __RTW_FUNCTIONS_H__

int rtw_wiphy_init(void);
void rtw_wiphy_deinit(void);
int rtw_wiphy_init_params(struct wiphy *pwiphy);
int rtw_wiphy_band_init(struct wiphy *pwiphy, u32 band_type);
int llhw_wifi_set_user_config(struct wifi_user_conf *pwifi_usrcfg);
void llhw_wifi_on(void);
int llhw_wifi_scan(struct _rtw_scan_param_t *scan_param, u32 ssid_len, u32 block);
int llhw_wifi_scan_abort(u8 block);
int llhw_wifi_connect(struct _rtw_network_info_t *connect_param, unsigned char block);
int llhw_wifi_disconnect(void);
int llhw_wifi_is_connected_to_ap(void);
int llhw_wifi_get_channel(u32 wlan_idx, u8 *ch);
int llhw_init(void);
void llhw_deinit(void);
int llhw_wifi_del_sta(u8 wlan_idx, u8 *mac);
int llhw_wifi_init_ap(void);
int llhw_wifi_deinit_ap(void);
int llhw_wifi_start_ap(struct _rtw_softap_info_t *softAP_config);
int llhw_wifi_stop_ap(void);
int llhw_wifi_add_key(struct rtw_crypt_info *crypt);
int llhw_wifi_set_EDCA_params(unsigned int *AC_param);
int llhw_wifi_tx_mgnt(u8 wlan_idx, const u8 *buf, size_t buf_len, u8 need_wait_ack);
int llhw_wifi_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr);
int llhw_wifi_pmksa_ops(dma_addr_t pmksa_ops_addr);
int llhw_wifi_channel_switch(dma_addr_t csa_param_addr);
u32 llhw_wifi_update_ip_addr(void);
int llhw_wifi_get_statistics(dma_addr_t statistic_addr);
int llhw_wifi_set_lps_enable(u8 enable);
int llhw_wifi_mp_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr);
int llhw_wifi_iwpriv_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr);
int llhw_wifi_set_mac_addr(u32 wlan_idx, u8 *addr);
int rtw_ndev_alloc(void);
int rtw_ndev_register(void);
void rtw_ndev_unregister(void);
void *rtw_get_ethtool_ops(void);
void cfg80211_rtw_ops_sta_init(void);
void cfg80211_rtw_ops_ap_init(void);
void cfg80211_rtw_ops_key_init(void);
void cfg80211_rtw_inform_bss(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len);
void cfg80211_rtw_disconnect_indicate(u16 reason, u8 locally_generated);
void cfg80211_rtw_sta_assoc_indicate(char *buf, int buf_len);
void cfg80211_rtw_external_auth_request(char *buf, int buf_len);
void cfg80211_rtw_update_owe_info_event(char *buf, int buf_len);
void cfg80211_rtw_connect_indicate(unsigned int join_status, void *user_data, size_t user_data_len);
int cfg80211_rtw_scan_done_indicate(unsigned int scanned_AP_num, void *user_data);
u64 llhw_wifi_get_tsft(u8 iface_type);
void rtw_reg_notifier(struct wiphy *wiphy, struct regulatory_request *request);
void rtw_regd_deinit(void);
int rtw_regd_init(void);
void rtw_ethtool_ops_init(void);

#ifdef CONFIG_NAN
void cfg80211_rtw_ops_nan_init(void);
void cfg80211_rtw_nan_handle_sdf(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie);
void cfg80211_rtw_nan_func_free(u64 os_dep_data);
int llhw_wifi_init_nan(void);
int llhw_wifi_deinit_nan(void);
int llhw_wifi_start_nan(u8 master_pref, u8 band_support);
int llhw_wifi_stop_nan(void);
int llhw_wifi_add_nan_func(struct rtw_nan_func_info_t *func, void *nan_func_pointer);
int llhw_wifi_del_nan_func(u64 cookie);
int rtw_nan_iface_alloc(struct wiphy *wiphy, const char *name, struct wireless_dev **nan_wdev, struct vif_params *params);
void rtw_nan_iface_free(struct wiphy *wiphy);
int llhw_wifi_nan_cfgvendor_cmd(u16 vendor_cmd, const void *data, int len);
#endif
#ifdef CONFIG_P2P
void cfg80211_rtw_ops_p2p_init(void);
void llhw_wifi_set_p2p_role(enum rtw_p2p_role role);
int llhw_wifi_set_p2p_remain_on_ch(unsigned char wlan_idx, u8 enable);
void rtw_p2p_gc_intf_revert(u8 need_if2_deinit);
void rtw_p2p_driver_macaddr_switch(void);
int rtw_p2p_iface_alloc(struct wiphy *wiphy, const char *name,
						struct wireless_dev **p2p_wdev, enum nl80211_iftype type);
void rtw_p2p_iface_free(struct wiphy *wiphy, struct wireless_dev *wdev);
void rtw_p2p_pdwdev_free(void);
int rtw_p2p_get_wdex_idx(struct wireless_dev *wdev);
#endif
int llhw_wifi_set_pmf_mode(u8 pmf_mode);
int llhw_wifi_set_wps_phase(u8 enable);
int llhw_wifi_set_wpa_mode(rtw_wpa_mode wpa_mode);
int llhw_wifi_set_owe_param(struct rtw_owe_param_t *owe_param);
int llhw_wifi_set_gen_ie(unsigned char wlan_idx, char *buf, __u16 buf_len, __u16 flags);
int llhw_wifi_add_custom_ie(const struct element **elem, u8 num, u16 type);
int llhw_wifi_del_custom_ie(unsigned char wlan_idx);
int llhw_wifi_update_custom_ie(u8 *ie, int ie_index, u8 type);
int llhw_wifi_set_edcca_mode(u8 edcca_mode);
int llhw_wifi_get_edcca_mode(u8 *edcca_mode);
int llhw_wifi_get_ant_info(u8 *antdiv_mode, u8 *curr_ant);
int llhw_wifi_set_country_code(char *cc);
int llhw_wifi_get_country_code(struct country_code_table_t *table);
int llhw_wifi_driver_is_mp(void);
int wifi_btcoex_bt_hci_notify(uint8_t *pdata, uint16_t len, uint8_t dir);

void *rtw_malloc(size_t size, dma_addr_t *paddr);
void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr);

/* trx, message related function */
#ifdef CONFIG_FULLMAC_HCI_IPC
int inic_msg_q_init(struct device *pdev, void (*task_hdl)(u8 event_num, u32 msg_addr, u8 wlan_idx));
int inic_msg_enqueue(struct inic_ipc_ex_msg *p_ipc_msg);
void inic_msg_q_deinit(void);
void llhw_send_packet(struct inic_ipc_ex_msg *p_ipc_msg);
void llhw_recv_task_from_msg(u8 event_num, u32 msg_addr, u8 wlan_idx);
void llhw_event_task(unsigned long data);
int llhw_xmit_init(void);
void llhw_xmit_deinit(void);
void llhw_recv_task_from_msg_recv_pkts(int idx_wlan, struct dev_sk_buff *skb);
int llhw_xmit_entry(int idx, struct sk_buff *pskb);
void llhw_xmit_done(int idx_wlan);
int llhw_event_init(struct inic_device *idev);
void llhw_event_deinit(void);
int llhw_ipc_send_msg(u32 id, u32 *param_buf, u32 buf_len);

#else
void llhw_send_data(u8 *buf, u32 len, struct sk_buff *pskb);
void llhw_recv_data_process(void *intf_priv);
void llhw_event_task(struct work_struct *data);
int llhw_xmit_entry(int idx, struct sk_buff *pskb);
int llhw_xmit_init(void);
int llhw_xmit_deinit(void);
int llhw_xmit_pending_q_num(void);
void llhw_xmit_wakeup_thread(void);
void llhw_recv_notify(void);
int llhw_recv_process(struct sk_buff *pskb);
void llhw_recv_init(void);
void llhw_recv_deinit(void);
int llhw_event_init(struct inic_device *idev);
void llhw_event_deinit(void);
int llhw_war_offload_ctrl(u8 offload_en, u32 offload_ctrl);
int llhw_war_set_mdns_param(u8 *pframe, u32 len);
void rtw_set_wowlan_offload_ctrl(u32 value);
void rtw_proxy_init(void);
void rtw_proxy_mdns_parms_init(u8 is_set_default);
void rtw_wow_prepare_mdns_para(u8 *pframe, u32 *plen);

#ifdef CONFIG_FULLMAC_HCI_SDIO
u32 rtw_sdio_init(struct inic_sdio *priv);
void rtw_sdio_deinit(struct inic_sdio *priv);
int rtw_sdio_suspend(struct device *dev);
int rtw_sdio_resume(struct device *dev);
int rtw_resume_common(struct inic_sdio *priv);
void rtw_sdio_init_txavailbd_threshold(struct inic_sdio *priv);
u8 rtw_sdio_query_txbd_status(struct inic_sdio *priv);
int rtw_sdio_alloc_irq(struct inic_sdio *priv);
#elif defined (CONFIG_FULLMAC_HCI_SPI)


#endif

#ifdef CONFIG_SDIO_BRIDGE
void llhw_sdio_bridge_get_scan_result(u32 ap_num);
void llhw_sdio_bridge_event_join_status_indicate(void *event_priv, u32 *param_buf);
void rtw_sdio_bridge_register_genl_family(void);
void rtw_sdio_bridge_unregister_genl_family(void);
#endif
#endif
#endif // __RTW_FUNCTIONS_H__
