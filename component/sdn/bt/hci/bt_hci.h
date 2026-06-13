#ifndef _BT_HCI_H_
#define _BT_HCI_H_

#include <bt_hci_spec.h>

typedef uint8_t (*hci_cmd_func_t)(void *param, uint8_t *status);
struct bt_hci_cmd_func_hdl {
	uint16_t opcode;
	hci_cmd_func_t hdl;
};

//OGF: BT_OGF_LINK_CTRL
uint8_t bt_hci_cmd_ogf_linkctrl_ocf_disconnect(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_linkctrl_ocf_read_version(void *phci_cmd_param, uint8_t *rsp);

//OGF: BT_OGF_BASEBAND
uint8_t bt_hci_cmd_ogf_baseband_ocf_evt_mask(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ofg_baseband_ocf_reset(void *phci_cmd_param, uint8_t *rsp);

//OGF: BT_OGF_INFO
uint8_t bt_hci_cmd_ogf_info_ocf_read_local_version_info(void *phci_cmd_para, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_info_ocf_read_supported_commands(void *phci_cmd_para, uint8_t *prsp);
uint8_t bt_hci_cmd_ogf_info_ocf_read_local_features(void *phci_cmd_para, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_info_ocf_read_bd_address(void *phci_cmd_para, uint8_t *rsp);

//OGF: BT_OGF_STATUS
uint8_t bt_hci_cmd_ogf_status_ocf_read_rssi(void *phci_cmd_param, uint8_t *rsp);

//OGF: BT_OGF_LE
uint8_t bt_hci_ogf_le_ocf_set_evt_mask(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_read_buffer_size(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_read_local_feature(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_set_random_addr(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_adv_set_param(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_adv_chanl_tx_power(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_adv_data(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_scan_rsp_data(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_adv_enable(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_scan_param(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_scan_enable(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_create_connection(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_create_connection_cancel(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_local_rpa(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_add_dev_to_fal(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_remove_dev_from_fal(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_clear_all_fal(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_conn_udpate(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_host_chan_classif(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_channel_map(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_remote_feature(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ofg_le_ocf_le_encrypt(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_enable_encryption(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_rand(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ltk_req_reply(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ltk_req_neg_reply(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_supp_states(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_recv_test_v1(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_transmit_test_v1(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_test_end(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_conn_param_req_reply(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_conn_param_req_neg_reply(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_data_length(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_suggested_datalen(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_write_suggested_datalen(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_add_dev_to_rl(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_remove_dev_from_rl(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_clear_rl(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_rl_size(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_rpa_timeout(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_addr_res_enable(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_privacy_mode(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_max_datalen(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_read_phy(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_default_phy(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_phy(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_recv_test_v2(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_transmit_test_v2(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_read_max_adv_data_len(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_read_max_adv_set(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_set_ext_adv_random_addr(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_ext_adv_param_v1(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_adv_set_aux_data(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_adv_set_aux_scan_rsp_data(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_set_enable(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_set_remove(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_set_clear(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_set_ext_scan_param(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_ogf_le_ocf_set_ext_scan_enable(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_create_connection(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_ext_create_connection_v2(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_periodic_adv_param_v1(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_periodic_adv_data(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_le_ocf_set_periodic_adv_enable(void *phci_cmd_param, uint8_t *rsp);

//OGF:BT_OGF_VENDOR
uint8_t bt_hci_cmd_ogf_vendor_ocf_read_rtk_chip_id(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_read_vendor_reg(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_write_vendor_reg(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_get_eco_version(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_mp(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_phy(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_read_radio_reg_pi(void *phci_cmd_param, uint8_t *rsp);
uint8_t bt_hci_cmd_ogf_vendor_ocf_write_radio_reg_pi(void *phci_cmd_param, uint8_t *rsp);

void bt_hci_set_evt_mask(uint8_t *pevt_mask);
bool bt_hci_evt_mask_is_set(uint64_t evt);
void bt_hci_set_le_evt_mask(uint8_t *ple_evt_mask);
bool bt_hci_le_evt_mask_is_set(uint64_t le_evt);
void bt_hci_enable(uint8_t max_hci_cmd_num);
uint8_t bt_hci_get_rx_hci_cmd_num(void);

void bt_hci_tx_acl_data(void *pdu, uint8_t len, uint16_t conn_handle, bool is_start);
void bt_hci_rx_acl_data(uint8_t *pbuf);

void bt_hci_cmd_handler(uint8_t *pbuf);
void bt_hci_get_supported_hci_command(uint8_t *pcommands);
void bt_hci_get_le_local_feature(uint8_t *pfeature);

void bt_hci_evt_command_status(uint16_t cmd_opcode, uint8_t status);
void bt_hci_evt_command_complete(uint16_t opcode, uint8_t plen, void *param);
void bt_hci_evt_disconnect_complete(uint16_t conn_handle, uint8_t reason);
void bt_hci_evt_num_completed_pkts(uint16_t conn_handle, uint16_t count);
void bt_hci_evt_encryption_change(uint16_t conn_handle, uint8_t enc_enabled);
void bt_hci_evt_encryption_key_refresh_complete(uint16_t conn_handle);
void bt_hci_evt_phy_update_complete(uint16_t conn_handle, uint8_t tx_phy, uint8_t rx_phy);
void bt_hci_evt_le_advertising_report(uint8_t *pkt, struct bt_le_addr_t *adv_addr);
void bt_hci_evt_le_ext_adv_report(uint8_t *pri_pkt, uint8_t no_aux, uint8_t *sec_pkt, uint8_t chain_num,
								  uint8_t adv_mode, uint8_t directed, uint8_t scan_rsp_valid,
								  uint8_t data_status, struct bt_le_addr_t *a_addr, uint8_t prim_phy,
								  uint8_t sec_phy, uint8_t sid, int8_t tx_power,
								  uint16_t pa_interval, struct bt_le_addr_t *d_addr,
								  uint16_t data_total_len, uint16_t scan_rsp_total_len);
void bt_hci_evt_le_ext_adv_legacy_report(uint8_t *pkt,
		struct bt_le_addr_t *adv_addr, struct bt_le_addr_t *direct_addr);
void bt_hci_evt_le_connection_complete(uint8_t status, uint16_t conn_handle, uint8_t role, struct bt_le_addr_t *peer_addr,
									   uint8_t *local_rpa, uint8_t *peer_rpa, void *conn_param);
void bt_hci_evt_le_conn_update_complete(uint16_t conn_handle, uint8_t status, void *connparam);
void bt_hci_evt_le_conn_param_req(uint16_t conn_handle, uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout);
void bt_hci_evt_le_data_len_change(uint16_t conn_handle, uint16_t max_tx_octets, uint16_t max_tx_time,
								   uint16_t max_rx_octets, uint16_t max_rx_time);
void bt_hci_evt_le_phy_update_complete(uint8_t status, uint16_t conn_handle, uint8_t tx_phy, uint8_t rx_phy);
void bt_hci_evt_le_read_remote_features_complete(uint16_t conn_handle, void *features);
void bt_hci_evt_le_ltk_req(uint16_t conn_handle, uint8_t *rand_ediv);
void bt_hci_evt_le_adv_set_terminated(uint8_t status, uint8_t adv_handle, uint16_t conn_handle, uint8_t num_cmpl_ext_adv_evts);

uint8_t bt_hci_check_le_connhandle(uint16_t handle);

#endif