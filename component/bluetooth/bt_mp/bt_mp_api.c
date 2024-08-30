/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "bt_debug.h"
#include "hci_common.h"
#include "hci_uart.h"
#include <bt_mp_api.h>
#if defined(CONFIG_BT_ENABLE_FAST_MP) && CONFIG_BT_ENABLE_FAST_MP
#include "hci_dtm.h"
#endif

/* ---------------------------------- define -------------------------------*/
#define BT_MP_ARG_LOGE(str)  BT_LOGE("%s (line: %d) arg NOT IN RANGE! (arg: %s)\r\n", __func__, __LINE__, str)

/**
 * @brief     BT power on for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_on(void)
{
	hci_set_mp(true);
	if (hci_controller_enable()) {
		BT_LOGA("Patch download End!\r\n");
		BT_LOGA("After download patch, deinit HCI driver & HCI uart!\r\n");
		/* In order to keep controller powerd on, do not use hci_controller_disable() */
		hci_uart_close();
		hci_transport_close();
		hci_controller_free();
	}
}

/**
 * @brief     BT power off for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_off(void)
{
	/* just power off controller, uart & transport are already disabled & freed */
	hci_controller_disable();
	hci_set_mp(false);
}

#if defined(CONFIG_BT_ENABLE_FAST_MP) && CONFIG_BT_ENABLE_FAST_MP
void rtk_bt_mp_dtm_power_on(void)
{
	hci_set_mp(true);
	hci_controller_enable();
}

void rtk_bt_mp_dtm_power_off(void)
{
	hci_controller_disable();
	hci_controller_free();
	hci_set_mp(false);
}

uint8_t rtk_bt_mp_dtm_rx_test_v1(uint8_t rx_chann)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return 0;
	}

	return hci_dtm_reveiver_test_v1(rx_chann);
}

uint8_t rtk_bt_mp_dtm_rx_test_v2(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return 0;
	}
	if (phy < RTK_BT_MP_RX_PHYS_1M || phy > RTK_BT_MP_RX_PHYS_CODED) {
		BT_MP_ARG_LOGE("phy");
		return 0;
	}
	if (mod_idx > RTK_BT_MP_MODULATION_INDEX_STABLE) {
		BT_MP_ARG_LOGE("mod_idx");
		return 0;
	}

	return hci_dtm_reveiver_test_v2(rx_chann, phy, mod_idx);
}

uint8_t rtk_bt_mp_dtm_rx_test_v3(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx,
								 uint8_t exp_cte_len, uint8_t exp_cte_type, uint8_t slot_durations,
								 uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return 0;
	}
	if (phy < RTK_BT_MP_RX_PHYS_1M || phy > RTK_BT_MP_RX_PHYS_CODED) {
		BT_MP_ARG_LOGE("phy");
		return 0;
	}
	if (mod_idx > RTK_BT_MP_MODULATION_INDEX_STABLE) {
		BT_MP_ARG_LOGE("mod_idx");
		return 0;
	}
	if (exp_cte_len == 0x01 || exp_cte_len > 0x14) {
		BT_MP_ARG_LOGE("exp_cte_len");
		return 0;
	}
	if (exp_cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("exp_cte_type");
		return 0;
	}
	if (slot_durations < RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_1US ||
		slot_durations > RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_2US) {
		BT_MP_ARG_LOGE("slot_durations");
		return 0;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return 0;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return 0;
	}

	return hci_dtm_receiver_test_v3(rx_chann, phy, mod_idx,
									exp_cte_len, exp_cte_type, slot_durations,
									sw_pattern_len, p_antenna_ids);
}

uint8_t rtk_bt_mp_dtm_tx_test_v1(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return 0;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return 0;
	}

	return hci_dtm_transmitter_test_v1(tx_chann, data_len, pkt_pl);
}

uint8_t rtk_bt_mp_dtm_tx_test_v2(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return 0;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return 0;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return 0;
	}

	return hci_dtm_transmitter_test_v2(tx_chann, data_len, pkt_pl, phy);
}

uint8_t rtk_bt_mp_dtm_tx_test_v3(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
								 uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return 0;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return 0;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return 0;
	}
	if (cte_len == 0x01 || cte_len > 0x14) {
		BT_MP_ARG_LOGE("cte_len");
		return 0;
	}
	if (cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("cte_type");
		return 0;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return 0;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return 0;
	}

	return hci_dtm_transmitter_test_v3(tx_chann, data_len, pkt_pl, phy,
									   cte_len, cte_type, sw_pattern_len, p_antenna_ids);
}

uint8_t rtk_bt_mp_dtm_tx_test_v4(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
								 uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids,
								 uint8_t tx_power_level)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return 0;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return 0;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return 0;
	}
	if (cte_len == 0x01 || cte_len > 0x14) {
		BT_MP_ARG_LOGE("cte_len");
		return 0;
	}
	if (cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("cte_type");
		return 0;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return 0;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return 0;
	}

	return hci_dtm_transmitter_test_v4(tx_chann, data_len, pkt_pl, phy, cte_len,
									   cte_type, sw_pattern_len, p_antenna_ids, tx_power_level);
}

uint8_t rtk_bt_mp_dtm_test_end(uint16_t *p_num_pkts)
{
	if (p_num_pkts == NULL) {
		BT_MP_ARG_LOGE("p_num_pkts");
		return 0;
	}

	return hci_dtm_test_end(p_num_pkts);
}

uint8_t rtk_bt_mp_dtm_get_rx_report(int8_t *p_rssi)
{
	if (p_rssi == NULL) {
		BT_MP_ARG_LOGE("p_rssi");
		return 0;
	}

	return hci_dtm_vendor_get_receiver_report(NULL, NULL, NULL, NULL, NULL, p_rssi);
}

uint8_t rtk_bt_mp_dtm_set_tx_count(uint8_t tx_pkt_cnt)
{
	return hci_dtm_vendor_set_transmitter_count(0x01, tx_pkt_cnt, 0x00, NULL, 0x00);
}

uint8_t rtk_bt_mp_dtm_set_tx_power_index(uint8_t ble_1m, uint8_t ble_2m)
{
	return hci_dtm_vendor_ctrl_tx_power(0xFF, 0xFF, 0xFF, ble_1m, ble_2m);
}

uint8_t rtk_bt_mp_dtm_set_tx_gaink(uint8_t tx_gain_k)
{
	return hci_dtm_vendor_k_power_setting(0x01, (uint32_t)tx_gain_k, NULL, NULL, NULL);
}

uint8_t rtk_bt_mp_dtm_set_tx_flatnessk(uint32_t tx_flastness_k)
{
	return hci_dtm_vendor_k_power_setting(0x02, tx_flastness_k, NULL, NULL, NULL);
}

uint8_t rtk_bt_mp_dtm_read_thermal(uint8_t *p_thermal_value)
{
	if (p_thermal_value == NULL) {
		BT_MP_ARG_LOGE("p_thermal_value");
		return 0;
	}

	return hci_dtm_vendor_read_thermal_meter_data(p_thermal_value);
}

uint8_t rtk_bt_mp_dtm_set_disable_tx_power_tracking(void)
{
	return hci_dtm_vendor_enable_tx_power_tracking(0x00, 0x00, NULL);
}
#endif /* CONFIG_BT_ENABLE_FAST_MP */