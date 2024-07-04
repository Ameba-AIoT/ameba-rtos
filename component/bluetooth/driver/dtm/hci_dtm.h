#ifndef _HCI_DTM_H_
#define _HCI_DTM_H_

/* -------------------------- Spec functions -------------------------- */
uint8_t hci_dtm_reveiver_test_v1(uint8_t rx_chann);
uint8_t hci_dtm_reveiver_test_v2(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx);
uint8_t hci_dtm_receiver_test_v3(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx,
								 uint8_t exp_cte_len, uint8_t exp_cte_type, uint8_t slot_durations,
								 uint8_t sw_pattern_len, uint8_t *p_antenna_ids);
uint8_t hci_dtm_transmitter_test_v1(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl);
uint8_t hci_dtm_transmitter_test_v2(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy);
uint8_t hci_dtm_transmitter_test_v3(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl,
									uint8_t phy, uint8_t cte_len, uint8_t cte_type,
									uint8_t sw_pattern_len, uint8_t *p_antenna_ids);
uint8_t hci_dtm_transmitter_test_v4(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl,
									uint8_t phy, uint8_t cte_len, uint8_t cte_type,
									uint8_t sw_pattern_len, uint8_t *p_antenna_ids, uint8_t tx_power_level);
uint8_t hci_dtm_test_end(uint16_t *p_num_pkts);

/* -------------------------- Vendor functions -------------------------- */
uint8_t hci_dtm_vendor_get_receiver_report(uint32_t *p_rx_pkts, uint32_t *p_rx_bits, uint32_t *p_rx_error_bits,
										   uint16_t *p_ber, uint16_t *p_cfo, int8_t *p_rssi);
uint8_t hci_dtm_vendor_set_transmitter_count(uint8_t tx_cnt_mode_en, uint8_t tx_pkt_cnt,
											 uint8_t chg_aa, uint8_t *p_access_addr, uint8_t prbs_fix);
uint8_t hci_dtm_vendor_ctrl_tx_power(uint8_t bdr_1m, uint8_t edr_2m, uint8_t edr_3m,
									 uint8_t ble_1m, uint8_t ble_2m);
uint8_t hci_dtm_vendor_k_power_setting(uint8_t sub_index, uint32_t value, uint8_t *p_tx_gaink,
									   uint32_t *p_tx_flatnessk, uint8_t *p_tx_path_lose);
uint8_t hci_dtm_vendor_read_thermal_meter_data(uint8_t *p_thermal_value);
uint8_t hci_dtm_vendor_enable_tx_power_tracking(uint8_t is_get_or_set, uint8_t is_set_enable,
												uint8_t *p_is_get_enable);

#endif /* _HCI_DTM_H_ */