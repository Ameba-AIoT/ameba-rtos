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
int whc_fullmac_host_set_user_config(struct wifi_user_conf *pwifi_usrcfg);
void whc_fullmac_host_wifi_on(void);
int whc_fullmac_host_scan(struct _rtw_scan_param_t *scan_param, u32 ssid_len, u8 block);
int whc_fullmac_host_scan_abort(u8 block);
int whc_fullmac_host_event_connect(struct _rtw_network_info_t *connect_param, unsigned char block);
int whc_fullmac_host_event_disconnect(void);
int whc_fullmac_host_wifi_is_connected_to_ap(void);
int whc_fullmac_host_get_channel(u32 wlan_idx, u8 *ch);
int whc_fullmac_host_set_channel(u32 wlan_idx, u8 ch);
int whc_host_init(void);
void whc_host_deinit(void);
int whc_fullmac_host_del_sta(u8 wlan_idx, u8 *mac);
int whc_fullmac_host_init_ap(void);
int whc_fullmac_host_deinit_ap(void);
int whc_fullmac_host_start_ap(struct _rtw_softap_info_t *softAP_config);
int whc_fullmac_host_stop_ap(void);
int whc_fullmac_host_add_key(struct rtw_crypt_info *crypt);
int whc_fullmac_host_set_EDCA_params(unsigned int *AC_param);
int whc_fullmac_host_tx_mgnt(u8 wlan_idx, const u8 *buf, size_t buf_len, u8 need_wait_ack);
int whc_fullmac_host_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr);
int whc_fullmac_host_pmksa_ops(dma_addr_t pmksa_ops_addr);
int whc_fullmac_host_channel_switch(dma_addr_t csa_param_addr);
u32 whc_fullmac_host_update_ip_addr(void);
int whc_fullmac_host_get_statistics(dma_addr_t statistic_addr);
int whc_fullmac_host_set_lps_enable(u8 enable);
int whc_fullmac_host_mp_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr);
int whc_fullmac_host_iwpriv_cmd(dma_addr_t cmd_addr, unsigned int cmd_len, dma_addr_t user_addr);
int whc_fullmac_host_set_mac_addr(u32 wlan_idx, u8 *addr);
int rtw_ndev_alloc(void);
int rtw_ndev_register(void);
void rtw_ndev_unregister(void);
void *rtw_get_ethtool_ops(void);
void whc_fullmac_host_ops_sta_init(void);
void whc_fullmac_host_ops_ap_init(void);
void whc_fullmac_host_ops_key_init(void);
void whc_fullmac_host_inform_bss(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len);
void whc_fullmac_host_disconnect_indicate(u16 reason, u8 locally_generated);
void whc_fullmac_host_sta_assoc_indicate(char *buf, int buf_len);
void whc_fullmac_host_external_auth_request(char *buf, int buf_len);
void whc_fullmac_host_update_owe_info_event(char *buf, int buf_len);
void whc_fullmac_host_connect_indicate(unsigned int join_status, void *user_data, size_t user_data_len);
int whc_fullmac_host_scan_done_indicate(unsigned int scanned_AP_num, void *user_data);
u64 whc_fullmac_host_get_tsft(u8 iface_type);
void rtw_reg_notifier(struct wiphy *wiphy, struct regulatory_request *request);
int rtw_regd_init(void);
void rtw_ethtool_ops_init(void);

#ifdef CONFIG_NAN
void whc_fullmac_host_nan_init(void);
void whc_fullmac_host_nan_handle_sdf(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie);
void whc_fullmac_host_nan_func_free(u64 os_dep_data);
int whc_fullmac_host_init_nan(void);
int whc_fullmac_host_deinit_nan(void);
int whc_fullmac_host_start_nan(u8 master_pref, u8 band_support);
int whc_fullmac_host_stop_nan(void);
int whc_fullmac_host_add_nan_func(struct rtw_nan_func_info_t *func, void *nan_func_pointer);
int whc_fullmac_host_del_nan_func(u64 cookie);
int rtw_nan_iface_alloc(struct wiphy *wiphy, const char *name, struct wireless_dev **nan_wdev, struct vif_params *params);
void rtw_nan_iface_free(struct wiphy *wiphy);
int whc_fullmac_host_nan_cfgvendor_cmd(u16 vendor_cmd, const void *data, int len);
#endif
#ifdef CONFIG_P2P
void whc_host_ops_p2p_init(void);
void whc_fullmac_host_set_p2p_role(enum rtw_p2p_role role);
int whc_fullmac_host_set_p2p_remain_on_ch(unsigned char wlan_idx, u8 enable);
void whc_fullmac_host_p2p_gc_intf_revert(u8 need_if2_deinit);
void whc_fullmac_host_p2p_driver_macaddr_switch(void);
int whc_fullmac_host_p2p_iface_alloc(struct wiphy *wiphy, const char *name,
									 struct wireless_dev **p2p_wdev, enum nl80211_iftype type);
void whc_fullmac_host_p2p_iface_free(struct wiphy *wiphy, struct wireless_dev *wdev);
void whc_fullmac_host_p2p_pdwdev_free(void);
int whc_fullmac_host_p2p_get_wdex_idx(struct wireless_dev *wdev);
#endif
int whc_fullmac_host_set_pmf_mode(u8 pmf_mode);
int whc_fullmac_host_set_wps_phase(u8 enable);
int whc_fullmac_host_set_wpa_mode(u8 wpa_mode);
int whc_fullmac_host_set_owe_param(struct rtw_owe_param_t *owe_param);
int whc_fullmac_host_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags);
int whc_fullmac_host_add_custom_ie(const struct element **elem, u8 num, u16 type);
int whc_fullmac_host_del_custom_ie(unsigned char wlan_idx);
int whc_fullmac_host_update_custom_ie(u8 *ie, int ie_index, u8 type);
int whc_fullmac_host_set_edcca_mode(u8 edcca_mode);
int whc_fullmac_host_get_edcca_mode(u8 *edcca_mode);
int whc_fullmac_host_get_ant_info(u8 *antdiv_mode, u8 *curr_ant);
int whc_fullmac_host_set_country_code(char *cc);
int whc_fullmac_host_get_country_code(struct country_code_table_t *table);
int whc_fullmac_host_dev_driver_is_mp(void);
int whc_fullmac_host_set_promisc_enable(u32 enable, u8 mode);

void *rtw_malloc(size_t size, dma_addr_t *paddr);
void rtw_mfree(size_t size, void *vaddr, dma_addr_t paddr);

/* trx, message related function */
#ifdef CONFIG_FULLMAC_HCI_IPC
int whc_fullmac_ipc_host_msg_q_init(struct device *pdev, void (*task_hdl)(u8 event_num, u32 msg_addr, u8 wlan_idx));
int whc_fullmac_ipc_host_msg_enqueue(struct whc_ipc_ex_msg *p_ipc_msg);
void whc_fullmac_ipc_host_msg_q_deinit(void);
void whc_fullmac_ipc_host_send_packet(struct whc_ipc_ex_msg *p_ipc_msg);
void whc_fullmac_ipc_host_recv_task_from_msg(u8 event_num, u32 msg_addr, u8 wlan_idx);
void whc_fullmac_host_event_task(unsigned long data);
int whc_host_xmit_init(void);
void whc_host_xmit_deinit(void);
void whc_fullmac_ipc_host_recv_task_from_msg_recv_pkts(int idx_wlan, struct dev_sk_buff *skb);
int whc_host_xmit_entry(int idx, struct sk_buff *pskb);
void whc_fullmac_ipc_host_xmit_done(int idx_wlan);
int whc_fullmac_host_event_init(struct whc_device *idev);
void whc_fullmac_host_event_deinit(void);
int whc_fullmac_ipc_host_send_msg(u32 id, u32 *param_buf, u32 buf_len);

#else
void whc_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);
void whc_host_recv_data_process(void *intf_priv);
void whc_fullmac_host_event_task(struct work_struct *data);
int whc_host_xmit_entry(int idx, struct sk_buff *pskb);
int whc_host_xmit_init(void);
int whc_host_xmit_deinit(void);
int whc_host_xmit_pending_q_num(void);
void whc_host_xmit_wakeup_thread(void);
void whc_host_recv_notify(void);
int whc_host_recv_process(struct sk_buff *pskb);
void whc_host_recv_init(void);
void whc_host_recv_deinit(void);
int whc_fullmac_host_event_init(struct whc_device *idev);
void whc_fullmac_host_event_deinit(void);
int whc_fullmac_host_war_offload_ctrl(u8 offload_en, u32 offload_ctrl);
int whc_fullmac_host_set_mdns_param(u8 *pframe, u32 len);
void rtw_set_wowlan_offload_ctrl(u32 value);
void rtw_proxy_init(void);
void rtw_proxy_mdns_parms_init(u8 is_set_default);
void rtw_wow_prepare_mdns_para(u8 *pframe, u32 *plen);

#ifdef CONFIG_FULLMAC_HCI_SDIO
u32 rtw_sdio_init(struct whc_sdio *priv);
void rtw_sdio_deinit(struct whc_sdio *priv);
int whc_sdio_host_suspend(struct device *dev);
int whc_sdio_host_resume(struct device *dev);
int whc_sdio_host_resume_common(struct whc_sdio *priv);
void rtw_sdio_init_txavailbd_threshold(struct whc_sdio *priv);
u8 rtw_sdio_query_txbd_status(struct whc_sdio *priv);
int rtw_sdio_alloc_irq(struct whc_sdio *priv);
#elif defined (CONFIG_FULLMAC_HCI_SPI)


#endif

#if defined(CONFIG_FULLMAC_BRIDGE)
void whc_bridge_host_register_genl_family(void);
void whc_bridge_host_unregister_genl_family(void);
void whc_bridge_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc);
int whc_bridge_host_recv_process(struct sk_buff *pskb);
#else
int whc_fullmac_host_recv_process(struct sk_buff *pskb);
#endif


#endif
#endif // __RTW_FUNCTIONS_H__
