#ifndef _INIC_DEV_API_H_
#define _INIC_DEV_API_H_

/* -------------------------------- Defines --------------------------------- */
#define CONFIG_INIC_DEV_API_PRIO 3
#ifdef CONFIG_PHYDM_CMD/*halbb debug cmd need bigger stack size*/
#define INIC_API_STACK 1024
#else
#define INIC_API_STACK 512
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

extern u8 inic_ipc_ip_addr[4];

/* ---------------------------- Private Functions --------------------------- */
void inic_event_wifi_connect(u32 api_id, u32 *param_buf);
void inic_event_wifi_add_key(u32 api_id, u32 *param_buf);
void inic_event_wpa_pmksa_ops(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_chplan(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_countrycode(u32 api_id, u32 *param_buf);
void inic_event_wifi_disconnect(u32 api_id, u32 *param_buf);
void inic_event_wifi_is_connected_to_ap(u32 api_id, u32 *param_buf);
void inic_event_wifi_is_running(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_channel(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_channel(u32 api_id, u32 *param_buf);
void inic_event_wifi_on(u32 api_id, u32 *param_buf);
void inic_event_wifi_init_ap(u32 api_id, u32 *param_buf);
void inic_event_wifi_off_ap(u32 api_id, u32 *param_buf);
void inic_event_wifi_start_ap(u32 api_id, u32 *param_buf);
void inic_event_wifi_stop_ap(u32 api_id, u32 *param_buf);
void inic_event_wifi_scan_networks(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_mac_address(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_pmf_mode(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_lps_enable(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_sae_status(u32 api_id, u32 *param_buf);
void inic_event_wifi_fetch_phy_statistic(u32 api_id, u32 *param_buf);
void inic_event_wifi_send_mgnt(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_EDCA_param(u32 api_id, u32 *param_buf);
void inic_event_wifi_del_station(u32 api_id, u32 *param_buf);
void inic_event_wifi_iwpriv_info(u32 api_id, u32 *param_buf);
void inic_event_wifi_ip_update(u32 api_id, u32 *param_buf);
void inic_event_wifi_ap_switch_ch(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_chplan(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_countrycode(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_wpa_mode(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_wps_phase(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_gen_ie(u32 api_id, u32 *param_buf);
void inic_event_wifi_scan_abort(u32 api_id, u32 *param_buf);
void inic_event_wifi_custom_ie_ops(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_usr_config(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_host_rtos(u32 api_id, u32 *param_buf);
void inic_event_wifi_set_edcca_mode(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_edcca_mode(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_ant_info(u32 api_id, u32 *param_buf);
void inic_event_war_offload_ctrl(u32 api_id, u32 *param_buf);
void inic_event_war_set_mdns_para(u32 api_id, u32 *param_buf);
void inic_event_wifi_driver_is_mp(u32 api_id, u32 *param_buf);


#ifdef CONFIG_NAN
void inic_event_nan_init(u32 api_id, u32 *param_buf);
void inic_event_nan_deinit(u32 api_id, u32 *param_buf);
void inic_event_nan_start(u32 api_id, u32 *param_buf);
void inic_event_nan_stop(u32 api_id, u32 *param_buf);
void inic_event_add_nan_func(u32 api_id, u32 *param_buf);
void inic_event_del_nan_func(u32 api_id, u32 *param_buf);
void inic_event_nan_cfgvenfor(u32 api_id, u32 *param_buf);
#endif

#ifdef CONFIG_WIFI_TUNNEL
void inic_event_wtn_cmd(u32 api_id, u32 *param_buf);
#endif

#ifdef CONFIG_MP_INCLUDED
void inic_event_mp_cmd(u32 api_id, u32 *param_buf);
#endif
#ifdef CONFIG_P2P
void inic_event_p2p_role(u32 api_id, u32 *param_buf);
void inic_event_p2p_remain_on_ch(u32 api_id, u32 *param_buf);
#endif

#ifndef CONFIG_SDIO_BRIDGE
void inic_event_get_scan_res(u32 api_id, u32 *param_buf);
void inic_event_wifi_get_setting(u32 api_id, u32 *param_buf);
void inic_event_send_eapol(u32 api_id, u32 *param_buf);
void inic_event_get_assoc_client_list(u32 api_id, u32 *param_buf);
void inic_event_wpa_4way_rpt(u32 api_id, u32 *param_buf);
void inic_event_get_sw_statistic(u32 api_id, u32 *param_buf);
#endif

/* ---------------------------- Public Functions ---------------------------- */
int wext_private_command(char *cmd, int show_msg, char *user_buf);
int wifi_set_ips_internal(u8 enable);

void inic_api_init_dev(void);
void inic_wifi_event_indicate(int event_cmd, char *buf, int buf_len, int flags);
void inic_scan_user_callback_indicate(unsigned int ap_num, void *user_data);
void inic_acs_info_indicate(struct acs_mntr_rpt *acs_mntr_rpt);
void inic_scan_each_report_user_callback_indicate(struct rtw_scan_result *scanned_ap_info, void *user_data);
u8 inic_promisc_callback_indicate(struct rx_pkt_info *pkt_info);
void inic_ap_ch_switch_callback_indicate(unsigned char channel, s8 ret);
void inic_dev_set_netif_info(int idx_wlan, unsigned char *dev_addr);
int inic_get_lwip_info(u32 type, unsigned char *input, int index);
void inic_cfg80211_indicate_scan_report(u32 channel, u32 frame_is_bcn, s32 rssi, u8 *mac_addr, u8 *IEs, u32 ie_len);

#ifdef CONFIG_NAN
void inic_cfg80211_indicate_nan_match(u8 type, u8 inst_id, u8 peer_inst_id, u8 *addr, u32 info_len, u8 *info, u64 cookie);
void inic_cfg80211_nan_func_free(u64 data);
void inic_cfg80211_nan_cfgvendor_event_report(u16 event_id, void *event, int event_len);
void inic_cfg80211_cfgvendor_send_cmd_reply(void *data, int len);
#endif
#ifdef CONFIG_P2P
void inic_cfg80211_indicate_channel_ready(void *scan_userdata);
#endif

int inic_ip_in_table_indicate(u8 gate, u8 ip);

#endif
