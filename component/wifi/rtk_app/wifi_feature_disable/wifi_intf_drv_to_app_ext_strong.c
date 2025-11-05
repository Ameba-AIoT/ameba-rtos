#include <wifi_api.h>
#include "wifi_intf_drv_to_app_internal.h"
u16 call_noused = 0;

s32 wifi_get_scan_records(u32 *ap_num, struct rtw_scan_result *ap_list)
{
	UNUSED(ap_num);
	UNUSED(ap_list);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_scan_abort(void)
{
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_ccmp_key(u8 wlan_idx, u8 *mac_addr, u8 *uncst_key, u8 *group_key)
{
	UNUSED(wlan_idx);
	UNUSED(mac_addr);
	UNUSED(uncst_key);
	UNUSED(group_key);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_wireless_mode(u8 *wmode)
{
	UNUSED(wmode);
	call_noused = __LINE__;
	return RTK_FAIL;
}


s32 wifi_set_wireless_mode(u32 wmode)
{
	UNUSED(wmode);
	call_noused = __LINE__;
	return RTK_FAIL;
}

s32 wifi_get_tsf(u8 wlan_idx, u64 *tsf)
{
	UNUSED(wlan_idx);
	UNUSED(tsf);
	call_noused = __LINE__;
	return 0;
}

s32 wifi_get_txbuf_pkt_num(s32 *pkt_num)
{
	UNUSED(pkt_num);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_tx_rate_by_tos(u8 enable, u8 tos_precedence, u8 tx_rate)
{
	UNUSED(enable);
	UNUSED(tos_precedence);
	UNUSED(tx_rate);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_edca_param(struct rtw_edca_param *pedca_param)
{
	UNUSED(pedca_param);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_tx_cca_enable(u8 enable)
{
	UNUSED(enable);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_ap_switch_chl_and_inform(struct rtw_csa_parm *csa_param)
{
	UNUSED(csa_param);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_countrycode(u8 *cntcode)
{
	UNUSED(cntcode);
	call_noused = __LINE__;
	return RTK_FAIL;
}

s32 wifi_get_countrycode(struct rtw_country_code_table *table)
{
	UNUSED(table);
	call_noused = __LINE__;
	return RTK_FAIL;
}

s32 wifi_get_channel_list(struct rtw_channel_list *ch_list)
{
	UNUSED(ch_list);
	call_noused = __LINE__;
	return RTK_FAIL;
}

int wifi_set_chplan(u8 chplan)
{
	UNUSED(chplan);
	call_noused = __LINE__;
	return RTK_FAIL;
}

s32 wifi_set_cts2self_duration_and_send(u8 wlan_idx, u16 duration)
{
	UNUSED(wlan_idx);
	UNUSED(duration);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_mac_address(s32 idx, struct rtw_mac *mac, u8 efuse)
{
	UNUSED(idx);
	UNUSED(mac);
	UNUSED(efuse);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_bt_mac_address(u8 *bt_mac)
{
	UNUSED(bt_mac);
	call_noused = __LINE__;
	return -1;
}

int wifi_get_bt_mac_address(u8 *mac)
{
	UNUSED(mac);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_csi_config(struct rtw_csi_action_parm *act_param)
{
	(void)act_param;
	call_noused = __LINE__;
	return -1;
}

int wifi_set_wps_phase(unsigned char is_trigger_wps)
{
	UNUSED(is_trigger_wps);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_eap_phase(unsigned char is_trigger_eap)
{
	UNUSED(is_trigger_eap);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_eap_method(unsigned char eap_method)
{
	UNUSED(eap_method);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_driver_is_mp(u8 *is_mp)
{
	UNUSED(is_mp);
	call_noused = __LINE__;
	return 0;
}

s32 wifi_ap_get_connected_clients(struct rtw_client_list *client_list_buf)
{
	UNUSED(client_list_buf);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_traffic_stats(u8 wlan_idx, union rtw_traffic_stats *traffic_stats)
{
	UNUSED(wlan_idx);
	UNUSED(traffic_stats);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_phy_stats(u8 wlan_idx, u8 *mac_addr, union rtw_phy_stats *phy_stats)
{
	UNUSED(wlan_idx);
	UNUSED(mac_addr);
	UNUSED(phy_stats);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_mfp_support(unsigned char value)
{
	UNUSED(value);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_group_id(unsigned char value)
{
	UNUSED(value);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_autoreconnect(u8 *enable)
{
	UNUSED(enable);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_add_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_num)
{
	UNUSED(cus_ie);
	UNUSED(ie_num);
	call_noused = __LINE__;
	return -1;
}


s32 wifi_update_custom_ie(struct rtw_custom_ie *cus_ie, s32 ie_index)
{
	UNUSED(cus_ie);
	UNUSED(ie_index);
	call_noused = __LINE__;
	return -1;
}



s32 wifi_del_custom_ie(u8 wlan_idx)
{
	UNUSED(wlan_idx);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_send_raw_frame(struct rtw_raw_frame_desc *raw_frame_desc)
{
	UNUSED(raw_frame_desc);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_get_antdiv_info(u8 *antdiv_mode, u8 *curr_ant)
{
	UNUSED(antdiv_mode);
	UNUSED(curr_ant);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_pmk_cache_enable(unsigned char value)
{
	UNUSED(value);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_lps_listen_interval(u8 interval)
{
	UNUSED(interval);
	call_noused = __LINE__;
	return -1;
}

void wifi_wpa_pmksa_ops(struct rtw_pmksa_ops_t *pmksa_ops)
{
	UNUSED(pmksa_ops);
	call_noused = __LINE__;
}

int wifi_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	UNUSED(wlan_idx);
	UNUSED(status);
	UNUSED(mac_addr);
	call_noused = __LINE__;

	return -1;
}

int wifi_ft_status_indicate(struct rtw_kvr_param_t *kvr_param, u16 status)
{
	UNUSED(kvr_param);
	UNUSED(status);
	call_noused = __LINE__;

	return -1;
}

int wifi_set_gen_ie(unsigned char wlan_idx, char *buf, u16 buf_len, u16 flags)
{
	UNUSED(wlan_idx);
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
	call_noused = __LINE__;

	return -1;
}

s32 wifi_get_band_type(u8 *band_type)
{
	UNUSED(band_type);
	call_noused = __LINE__;
	return -1;
}

int wifi_set_pmf_mode(u8 pmf_mode)
{
	UNUSED(pmf_mode);
	call_noused = __LINE__;
	return -1;
}

void wifi_wpa_4way_status_indicate(struct rtw_wpa_4way_status *rpt_4way)
{
	UNUSED(rpt_4way);
	call_noused = __LINE__;
}

void wifi_dhcp_success_indicate(void)
{
	call_noused = __LINE__;
}

int wifi_wpa_add_key(struct rtw_crypt_info *crypt)
{
	UNUSED(crypt);
	call_noused = __LINE__;
	return -1;
}

void wifi_promisc_enable(u32 enable, struct rtw_promisc_para *para)
{
	UNUSED(enable);
	UNUSED(para);
	call_noused = __LINE__;
}

void wifi_speaker_setting(u8 set_type, union rtw_speaker_set *settings)
{
	UNUSED(set_type);
	UNUSED(settings);
	call_noused = __LINE__;
}

void wifi_set_owe_param(struct rtw_owe_param_t *owe_param)
{
	UNUSED(owe_param);
	call_noused = __LINE__;
}

void wifi_set_conn_step_try_limit(struct rtw_conn_step_retries *conn_step_retries)
{
	UNUSED(conn_step_retries);
	call_noused = __LINE__;
}

void wifi_ap_set_invisible(u8 enable)
{
	UNUSED(enable);
	call_noused = __LINE__;
}

int wifi_wake_pll_rdy_in_ps_state(u8 need)
{
	UNUSED(need);
	call_noused = __LINE__;
	return -1;
}

s32 wifi_set_tx_advanced_config(struct rtw_tx_advanced_cfg *tx_setting)
{
	UNUSED(tx_setting);
	call_noused = __LINE__;
	return -1;
}

void wifi_rmesh_register_ota_callback(int (*ota_callback)(u8 *buf, u16 len))
{
	UNUSED(ota_callback);
	call_noused = __LINE__;
	return;
}

int wifi_rmesh_get_node_info(enum rtw_rmesh_node_type type, struct rtw_rmesh_node_info *node_info)
{
	UNUSED(type);
	UNUSED(node_info);
	call_noused = __LINE__;
	return RTK_FAIL;
}

u8 wifi_rmesh_get_child_num(void)
{
	call_noused = __LINE__;
	return 0;
}

int wifi_rmesh_get_child_info_list(u8 *child_num, struct rtw_rmesh_node_info *child_info_list)
{
	UNUSED(child_num);
	UNUSED(child_info_list);
	call_noused = __LINE__;
	return RTK_FAIL;
}

int wifi_rmesh_update_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, u8 ota_ongoing)
{
	UNUSED(ota_ver);
	UNUSED(ota_ver_len);
	UNUSED(ota_ongoing);
	call_noused = __LINE__;
	return RTK_FAIL;
}

int wifi_rmesh_check_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, enum rtw_rmesh_node_type node_type)
{
	UNUSED(ota_ver);
	UNUSED(ota_ver_len);
	UNUSED(node_type);
	call_noused = __LINE__;
	return RTK_FAIL;
}