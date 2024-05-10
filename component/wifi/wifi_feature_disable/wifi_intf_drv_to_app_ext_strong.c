#include <wifi_conf.h>

int wifi_get_disconn_reason_code(unsigned short *reason_code)
{
	UNUSED(reason_code);
	return -1;
}

int wifi_get_scan_records(unsigned int *AP_num, char *scan_buf)
{
	UNUSED(AP_num);
	UNUSED(scan_buf);
	return -1;
}

int wifi_scan_abort(u8 block)
{
	UNUSED(block);
	return -1;
}

int wifi_btcoex_set_ble_scan_duty(u8 duty)
{
	(void)duty;
	return -1;
}

int wifi_get_ccmp_key(u8 wlan_idx, u8 *mac_addr, unsigned char *uncst_key, unsigned char *group_key)
{
	UNUSED(wlan_idx);
	UNUSED(mac_addr);
	UNUSED(uncst_key);
	UNUSED(group_key);
	return -1;
}

int wifi_get_network_mode(void)
{
	return 0;//unconnected
}


int wifi_set_network_mode(enum wlan_mode mode)
{
	UNUSED(mode);
	return RTW_ERROR;
}

u64 wifi_get_tsf(unsigned char port_id)
{
	UNUSED(port_id);
	return 0;
}

int wifi_get_txbuf_pkt_num(void)
{
	return -1;
}

int wifi_set_tx_rate_by_ToS(unsigned char enable, unsigned char ToS_precedence, unsigned char tx_rate)
{
	UNUSED(enable);
	UNUSED(ToS_precedence);
	UNUSED(tx_rate);
	return -1;
}

int wifi_set_EDCA_param(unsigned int AC_param)
{
	UNUSED(AC_param);
	return -1;
}

int wifi_set_TX_CCA(unsigned char enable)
{
	UNUSED(enable);
	return -1;
}

int wifi_ap_switch_chl_and_inform(struct _rtw_csa_parm_t *csa_param)
{
	UNUSED(csa_param);
	return -1;
}

u8 wifi_set_countrycode(char *cntcode)
{
	UNUSED(cntcode);
	return 0;
}

u8 wifi_get_countrycode(struct country_code_table_t *table)
{
	UNUSED(table);
	return 0;
}

u8 wifi_set_chplan(u8 chplan)
{
	UNUSED(chplan);
	return 0;
}

int wifi_get_chplan(u8 *chplan)
{
	UNUSED(chplan);
	return 0;
}


int wifi_set_cts2self_duration_and_send(unsigned char wlan_idx, unsigned short duration)
{
	UNUSED(wlan_idx);
	UNUSED(duration);
	return -1;
}

int wifi_get_mac_address(int idx, struct _rtw_mac_t *mac, u8 efuse)
{
	UNUSED(idx);
	UNUSED(mac);
	UNUSED(efuse);
	return -1;
}

int wifi_set_bt_mac_address(u8 *bt_mac)
{
	UNUSED(bt_mac);
	return -1;
}

int wifi_get_bt_mac_address(u8 *mac)
{
	UNUSED(mac);
	return -1;
}

int wifi_csi_config(struct _rtw_csi_action_parm_t *act_param)
{
	(void)act_param;
	return -1;
}

int wifi_csi_report(u32 buf_len, u8 *csi_buf, u32 *len)
{
	(void)buf_len;
	(void)csi_buf;
	(void)len;
	return -1;
}

int wifi_set_wps_phase(unsigned char is_trigger_wps)
{
	UNUSED(is_trigger_wps);
	return -1;
}

int wifi_set_eap_phase(unsigned char is_trigger_eap)
{
	UNUSED(is_trigger_eap);
	return -1;
}

int wifi_set_eap_method(unsigned char eap_method)
{
	UNUSED(eap_method);
	return -1;
}

int wifi_get_sw_statistic(unsigned char idx, struct _rtw_sw_statistics_t *sw_statistics)
{
	UNUSED(idx);
	UNUSED(sw_statistics);
	return -1;
}

int wifi_del_station(unsigned char wlan_idx, unsigned char *hwaddr)
{
	UNUSED(wlan_idx);
	UNUSED(hwaddr);
	return -1;
}

u8 wifi_driver_is_mp(void)
{
	return 0;
}

int wifi_get_associated_client_list(struct _rtw_client_list_t *client_list_buf)
{
	UNUSED(client_list_buf);
	return -1;
}

int wifi_fetch_phy_statistic(struct _rtw_phy_statistics_t *phy_statistic)
{
	UNUSED(phy_statistic);
	return -1;
}

int wifi_set_mfp_support(unsigned char value)
{
	UNUSED(value);
	return -1;
}

int wifi_set_group_id(unsigned char value)
{
	UNUSED(value);
	return -1;
}

int wifi_get_autoreconnect(__u8 *mode)
{
	UNUSED(mode);
	return -1;
}

int wifi_add_custom_ie(void *cus_ie, int ie_num)
{
	UNUSED(cus_ie);
	UNUSED(ie_num);
	return -1;
}


int wifi_update_custom_ie(void *cus_ie, int ie_index)
{
	UNUSED(cus_ie);
	UNUSED(ie_index);
	return -1;
}



int wifi_del_custom_ie(unsigned char wlan_idx)
{
	UNUSED(wlan_idx);
	return -1;
}


void wifi_set_indicate_mgnt(int enable)
{
	UNUSED(enable);
	return;
}

int wifi_send_raw_frame(struct raw_frame_desc_t *raw_frame_desc)
{
	UNUSED(raw_frame_desc);
	return -1;
}

int wifi_get_antdiv_info(unsigned char *antdiv_mode, unsigned char *curr_ant)
{
	UNUSED(antdiv_mode);
	UNUSED(curr_ant);
	return -1;
}

int wifi_set_pmk_cache_enable(unsigned char value)
{
	UNUSED(value);
	return -1;
}

int wifi_set_lps_listen_interval(u8 interval)
{
	UNUSED(interval);
	return -1;
}

void wifi_wpa_pmksa_ops(struct rtw_pmksa_ops_t *pmksa_ops)
{
	UNUSED(pmksa_ops);
}

int wifi_sae_status_indicate(u8 wlan_idx, u16 status, u8 *mac_addr)
{
	UNUSED(wlan_idx);
	UNUSED(status);
	UNUSED(mac_addr);

	return -1;
}

int wifi_ft_status_indicate(struct rtw_kvr_param_t *kvr_param, u16 status)
{
	UNUSED(kvr_param);
	UNUSED(status);

	return -1;
}

int wifi_set_gen_ie(unsigned char wlan_idx, char *buf, __u16 buf_len, __u16 flags)
{
	UNUSED(wlan_idx);
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);

	return -1;
}

enum _WL_BAND_TYPE wifi_get_band_type(void)
{
	return WL_BAND_2_4G;
}

void wifi_btcoex_set_pta(enum pta_type type, u8 role, u8 process)
{
	(void)type;
	(void)role;
	(void)process;
}

void wifi_btcoex_set_bt_ant(u8 bt_ant)
{
	(void)bt_ant;
}

int wifi_set_pmf_mode(u8 pmf_mode)
{
	UNUSED(pmf_mode);
	return -1;
}

int wifi_btcoex_bt_rfk(struct bt_rfk_param *rfk_param)
{
	UNUSED(rfk_param);
	return -1;
}

int wifi_zigbee_coex_zb_rfk(void)
{
	return -1;
}

void wifi_wpa_sta_4way_fail_notify(void)
{
}

void wifi_wpa_add_key(struct rtw_crypt_info *crypt)
{
	UNUSED(crypt);
}

void wifi_promisc_enable(u32 enable, struct _promisc_para_t *para)
{
	UNUSED(enable);
	UNUSED(para);
}

void wifi_speaker_setting(u8 mode, u8 nav_thresh, u8 relay_en)
{
	UNUSED(mode);
	UNUSED(nav_thresh);
	UNUSED(relay_en);
}

void wifi_set_owe_param(struct rtw_owe_param_t *owe_param)
{
	UNUSED(owe_param);
}

