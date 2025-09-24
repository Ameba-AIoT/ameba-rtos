#ifndef _WHC_DEV_API_PATH_H_
#define _WHC_DEV_API_PATH_H_

/* -------------------------------- Defines --------------------------------- */
#define CONFIG_WHC_DEV_API_PRIO 3
#if defined(CONFIG_PHYDM_CMD) || defined(NAN_CUSTOMER_NANDOW) /*halbb debug cmd need bigger stack size*/
#define WHC_API_STACK 1024
#else
#define WHC_API_STACK 512
#endif

#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

/* ------------------------------- Data Types ------------------------------- */
struct event_priv_t {
	rtos_task_t api_dev_task;

	rtos_sema_t task_wake_sema;
	rtos_mutex_t send_mutex;
	rtos_sema_t api_ret_sema;

	u8 *rx_api_msg;
	u8 *rx_ret_msg;

	u8 dev_req_network_info[DEV_REQ_NETWORK_INFO_MAX_LEN];

	u8 b_waiting_for_ret: 1;
};

struct event_func_t {
	u32 api_id;
	void (*func)(u32 api_id, u32 *param_buf);
};

/* ---------------------------- Global Variables ---------------------------- */
extern struct event_priv_t event_priv;

extern u8 whc_ipc_ip_addr[4];

/* ---------------------------- Private Functions --------------------------- */
void whc_event_wifi_connect(u32 api_id, u32 *param_buf);
void whc_event_wifi_add_key(u32 api_id, u32 *param_buf);
void whc_event_wpa_pmksa_ops(u32 api_id, u32 *param_buf);
void whc_event_wifi_get_countrycode(u32 api_id, u32 *param_buf);
void whc_event_wifi_disconnect(u32 api_id, u32 *param_buf);
void whc_event_wifi_is_running(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_channel(u32 api_id, u32 *param_buf);
void whc_event_wifi_on(u32 api_id, u32 *param_buf);
void whc_event_wifi_init_ap(u32 api_id, u32 *param_buf);
void whc_event_wifi_off_ap(u32 api_id, u32 *param_buf);
void whc_event_wifi_start_ap(u32 api_id, u32 *param_buf);
void whc_event_wifi_stop_ap(u32 api_id, u32 *param_buf);
void whc_event_wifi_scan_networks(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_mac_address(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_pmf_mode(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_lps_enable(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_sae_status(u32 api_id, u32 *param_buf);
void whc_event_wifi_fetch_phy_stats(u32 api_id, u32 *param_buf);
void whc_event_wifi_send_mgnt(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_EDCA_param(u32 api_id, u32 *param_buf);
void whc_event_wifi_ap_del_client(u32 api_id, u32 *param_buf);
void whc_event_wifi_iwpriv_info(u32 api_id, u32 *param_buf);
void whc_event_wifi_ip_update(u32 api_id, u32 *param_buf);
void whc_event_wifi_ap_switch_ch(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_chplan(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_countrycode(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_wpa_mode(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_wps_phase(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_gen_ie(u32 api_id, u32 *param_buf);
void whc_event_wifi_scan_abort(u32 api_id, u32 *param_buf);
void whc_event_wifi_custom_ie_ops(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_usr_config(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_host_rtos(u32 api_id, u32 *param_buf);
void whc_event_wifi_set_edcca_mode(u32 api_id, u32 *param_buf);
void whc_event_wifi_get_edcca_mode(u32 api_id, u32 *param_buf);
void whc_event_wifi_get_ant_info(u32 api_id, u32 *param_buf);
void whc_event_war_offload_ctrl(u32 api_id, u32 *param_buf);
void whc_event_war_set_mdns_para(u32 api_id, u32 *param_buf);
void whc_event_wifi_driver_is_mp(u32 api_id, u32 *param_buf);


#ifdef CONFIG_NAN
void whc_event_nan_init(u32 api_id, u32 *param_buf);
void whc_event_nan_deinit(u32 api_id, u32 *param_buf);
void whc_event_nan_start(u32 api_id, u32 *param_buf);
void whc_event_nan_stop(u32 api_id, u32 *param_buf);
void whc_event_add_nan_func(u32 api_id, u32 *param_buf);
void whc_event_del_nan_func(u32 api_id, u32 *param_buf);
void whc_event_nan_cfgvenfor(u32 api_id, u32 *param_buf);
#endif

#ifdef CONFIG_WIFI_TUNNEL
void whc_event_wtn_cmd(u32 api_id, u32 *param_buf);
#endif

#ifdef CONFIG_MP_INCLUDED
void whc_event_mp_cmd(u32 api_id, u32 *param_buf);
#endif
#ifdef CONFIG_P2P
void whc_event_p2p_role(u32 api_id, u32 *param_buf);
void whc_event_p2p_remain_on_ch(u32 api_id, u32 *param_buf);
#endif

#if defined(CONFIG_WHC_WIFI_API_PATH)
void whc_event_get_scan_res(u32 api_id, u32 *param_buf);
void whc_event_wifi_get_setting(u32 api_id, u32 *param_buf);
void whc_event_send_eapol(u32 api_id, u32 *param_buf);
void whc_event_wifi_ap_get_connected_clients(u32 api_id, u32 *param_buf);
void whc_event_wpa_4way_rpt(u32 api_id, u32 *param_buf);
void whc_event_wifi_dhcp_success(u32 api_id, u32 *param_buf);
void whc_event_get_traffic_stats(u32 api_id, u32 *param_buf);
void whc_event_start_join_cmd(u32 api_id, u32 *param_buf);
void whc_event_get_eap_phase(u32 api_id, u32 *param_buf);
#endif

#ifdef CONFIG_SUPPLICANT_SME
void whc_event_sme_auth(u32 api_id, u32 *param_buf);
void whc_event_sme_set_assocreq_ie(u32 api_id, u32 *param_buf);
#endif

/* ---------------------------- Public Functions ---------------------------- */
int wext_private_command(char *cmd, int show_msg, char *user_buf);
int wifi_set_ips_internal(u8 enable);

void whc_dev_api_init(void);
void whc_dev_wifi_event_indicate(u32 event_cmd, u8 *evt_info, s32 evt_len);
void whc_dev_scan_user_callback_indicate(unsigned int ap_num, void *user_data);
void whc_dev_acs_info_indicate(struct rtw_acs_mntr_rpt *acs_mntr_rpt);
void whc_dev_scan_each_report_user_callback_indicate(struct rtw_scan_result *scanned_ap_info, void *user_data);
u8 whc_dev_promisc_callback_indicate(struct rtw_rx_pkt_info *pkt_info);
void whc_dev_ap_ch_switch_callback_indicate(unsigned char channel, s8 ret);
void whc_dev_set_netif_info(int idx_wlan, unsigned char *dev_addr);
int whc_dev_get_lwip_info(u32 type, unsigned char *input, int index);
void whc_dev_cfg80211_indicate_scan_report(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len);

#ifdef CONFIG_NAN
void whc_dev_cfg80211_indicate_nan_match(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie);
void whc_dev_cfg80211_nan_func_free(u64 data);
void whc_dev_cfg80211_nan_cfgvendor_event_report(u16 event_id, void *event, int event_len);
void whc_dev_cfg80211_cfgvendor_send_cmd_reply(void *data, int len);
#endif

int whc_dev_ip_in_table_indicate(u8 gate, u8 ip);

#endif
