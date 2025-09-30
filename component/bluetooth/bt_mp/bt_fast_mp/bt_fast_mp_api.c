/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "bt_debug.h"
#include "hci_controller.h"
#include "hci_transport.h"
#include "hci_uart.h"
#include "bt_fast_mp_api.h"
#include "hci_if_dtm.h"

#define DTM_ARRAY_TO_UINT16(_array)           \
        (((uint16_t)(*(_array + 0)) <<  0) |  \
         ((uint16_t)(*(_array + 1)) <<  8))

#define DTM_ARRAY_TO_UINT32(_array)           \
        (((uint32_t)(*(_array + 0)) <<  0) |  \
         ((uint32_t)(*(_array + 1)) <<  8) |  \
         ((uint32_t)(*(_array + 2)) << 16) |  \
         ((uint32_t)(*(_array + 3)) << 24))

/* ---------------------------------- define -------------------------------*/
#define BT_MP_ARG_LOGE(str)  BT_LOGE("%s (line: %d) arg NOT IN RANGE! (arg: %s)\r\n", __func__, __LINE__, str)

void rtk_bt_mp_dtm_power_on(void)
{
	hci_dtm_open();
}

void rtk_bt_mp_dtm_power_off(void)
{
	hci_dtm_close();
}

bool rtk_bt_mp_dtm_rx_test_v1(uint8_t rx_chann)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return false;
	}

	/* OpCode: 0x201D, Data Len: Cmd(1+3), Event(6) */
	uint8_t buf[6] = {0};

	buf[2] = (uint8_t)(1);
	buf[3] = rx_chann;

	return hci_dtm_send(0x201D, buf, 4);
}

bool rtk_bt_mp_dtm_rx_test_v2(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return false;
	}
	if (phy < RTK_BT_MP_RX_PHYS_1M || phy > RTK_BT_MP_RX_PHYS_CODED) {
		BT_MP_ARG_LOGE("phy");
		return false;
	}
	if (mod_idx > RTK_BT_MP_MODULATION_INDEX_STABLE) {
		BT_MP_ARG_LOGE("mod_idx");
		return false;
	}

	/* OpCode: 0x2033, Data Len: Cmd(3+3), Event(6) */
	uint8_t buf[6] = {0};

	buf[2] = (uint8_t)(3);
	buf[3] = rx_chann;
	buf[4] = phy;
	buf[5] = mod_idx;

	return hci_dtm_send(0x2033, buf, 6);
}

bool rtk_bt_mp_dtm_rx_test_v3(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx,
							  uint8_t exp_cte_len, uint8_t exp_cte_type, uint8_t slot_durations,
							  uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	if (rx_chann > 0x27) {
		BT_MP_ARG_LOGE("rx_chann");
		return false;
	}
	if (phy < RTK_BT_MP_RX_PHYS_1M || phy > RTK_BT_MP_RX_PHYS_CODED) {
		BT_MP_ARG_LOGE("phy");
		return false;
	}
	if (mod_idx > RTK_BT_MP_MODULATION_INDEX_STABLE) {
		BT_MP_ARG_LOGE("mod_idx");
		return false;
	}
	if (exp_cte_len == 0x01 || exp_cte_len > 0x14) {
		BT_MP_ARG_LOGE("exp_cte_len");
		return false;
	}
	if (exp_cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("exp_cte_type");
		return false;
	}
	if (slot_durations < RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_1US ||
		slot_durations > RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_2US) {
		BT_MP_ARG_LOGE("slot_durations");
		return false;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return false;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return false;
	}

	/* OpCode: 0x204F, Data Len: Cmd(7+sw_pattern_len+3), Event(6) */
	uint8_t buf[10 + sw_pattern_len];
	uint8_t i = 0;

	buf[2] = (uint8_t)(7 + sw_pattern_len);
	buf[3] = rx_chann;
	buf[4] = phy;
	buf[5] = mod_idx;
	buf[6] = exp_cte_len;
	buf[7] = exp_cte_type;
	buf[8] = slot_durations;
	buf[9] = sw_pattern_len;
	if (p_antenna_ids != NULL) {
		for (i = 0; i < buf[9]; i++) {
			buf[10 + i] = p_antenna_ids[i];
		}
	}

	return hci_dtm_send(0x204F, buf, 10 + sw_pattern_len);
}

bool rtk_bt_mp_dtm_tx_test_v1(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return false;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return false;
	}

	/* OpCode: 0x201E, Data Len: Cmd(3+3), Event(6) */
	uint8_t buf[6] = {0};

	buf[2] = (uint8_t)(3);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;

	return hci_dtm_send(0x201E, buf, 6);
}

bool rtk_bt_mp_dtm_tx_test_v2(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return false;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return false;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return false;
	}

	/* OpCode: 0x2034, Data Len: Cmd(4+3), Event(6) */
	uint8_t buf[7] = {0};

	buf[2] = (uint8_t)(4);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;
	buf[6] = phy;

	return hci_dtm_send(0x2034, buf, 7);
}

bool rtk_bt_mp_dtm_tx_test_v3(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
							  uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return false;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return false;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return false;
	}
	if (cte_len == 0x01 || cte_len > 0x14) {
		BT_MP_ARG_LOGE("cte_len");
		return false;
	}
	if (cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("cte_type");
		return false;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return false;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return false;
	}

	/* OpCode: 0x2050, Data Len: Cmd(7+sw_pattern_len+3), Event(6) */
	uint8_t buf[10 + sw_pattern_len];
	uint8_t i = 0;

	buf[2] = (uint8_t)(7 + sw_pattern_len);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;
	buf[6] = phy;
	buf[7] = cte_len;
	buf[8] = cte_type;
	buf[9] = sw_pattern_len;
	if (p_antenna_ids != NULL) {
		for (i = 0; i < buf[9]; i++) {
			buf[10 + i] = p_antenna_ids[i];
		}
	}

	return hci_dtm_send(0x2050, buf, 10 + sw_pattern_len);
}

bool rtk_bt_mp_dtm_tx_test_v4(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
							  uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids,
							  int8_t tx_power_level)
{
	if (tx_chann > 0x27) {
		BT_MP_ARG_LOGE("tx_chann");
		return false;
	}
	if (pkt_pl > RTK_BT_MP_PACKET_PAYLOAD_01) {
		BT_MP_ARG_LOGE("pkt_pl");
		return false;
	}
	if (phy < RTK_BT_MP_TX_PHYS_1M || phy > RTK_BT_MP_TX_PHYS_CODED_S2) {
		BT_MP_ARG_LOGE("phy");
		return false;
	}
	if (cte_len == 0x01 || cte_len > 0x14) {
		BT_MP_ARG_LOGE("cte_len");
		return false;
	}
	if (cte_type > RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT) {
		BT_MP_ARG_LOGE("cte_type");
		return false;
	}
	if (sw_pattern_len < 0x02 || sw_pattern_len > 0x4B) {
		BT_MP_ARG_LOGE("sw_pattern_len");
		return false;
	}
	if (p_antenna_ids == NULL) {
		BT_MP_ARG_LOGE("p_antenna_ids");
		return false;
	}

	/* OpCode: 0x207B, Data Len: Cmd(8+sw_pattern_len+3), Event(6) */
	uint8_t buf[11 + sw_pattern_len];
	uint8_t i = 0;

	buf[2] = (uint8_t)(8 + sw_pattern_len);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;
	buf[6] = phy;
	buf[7] = cte_len;
	buf[8] = cte_type;
	buf[9] = sw_pattern_len;
	if (p_antenna_ids != NULL) {
		for (i = 0; i < buf[9]; i++) {
			buf[10 + i] = p_antenna_ids[i];
		}
	}
	buf[10 + i] = (uint8_t)tx_power_level;

	return hci_dtm_send(0x207B, buf, 11 + sw_pattern_len);
}

bool rtk_bt_mp_dtm_test_end(uint16_t *p_num_pkts)
{
	if (p_num_pkts == NULL) {
		BT_MP_ARG_LOGE("p_num_pkts");
		return false;
	}

	/* OpCode: 0x201F, Data Len: Cmd(3), Event(8) */
	uint8_t buf[8] = {0};

	buf[2] = (uint8_t)(0);

	if (false == hci_dtm_send(0x201F, buf, 3)) {
		return false;
	}

	if (p_num_pkts != NULL) {
		*p_num_pkts = DTM_ARRAY_TO_UINT16(&buf[6]);
	}

	return true;
}

bool rtk_bt_mp_dtm_get_rx_report(int8_t *p_rssi)
{
	if (p_rssi == NULL) {
		BT_MP_ARG_LOGE("p_rssi");
		return false;
	}

	uint32_t *p_rx_pkts = NULL;
	uint32_t *p_rx_bits = NULL;
	uint32_t *p_rx_error_bits = NULL;
	uint16_t *p_ber = NULL;
	uint16_t *p_cfo = NULL;

	/* OpCode: 0xFCE5, Data Len: Cmd(3), Event(23) */
	uint8_t buf[23] = {0};

	buf[2] = (uint8_t)(0);

	if (false == hci_dtm_send(0xFCE5, buf, 3)) {
		return false;
	}

	if (p_rx_pkts != NULL) {
		*p_rx_pkts = DTM_ARRAY_TO_UINT32(&buf[6]);
	}
	if (p_rx_bits != NULL) {
		*p_rx_bits = DTM_ARRAY_TO_UINT32(&buf[10]);
	}
	if (p_rx_error_bits != NULL) {
		*p_rx_error_bits = DTM_ARRAY_TO_UINT32(&buf[14]);
	}
	if (p_ber != NULL) {
		*p_ber = DTM_ARRAY_TO_UINT16(&buf[18]);
	}
	if (p_cfo != NULL) {
		*p_cfo = DTM_ARRAY_TO_UINT16(&buf[20]);
	}
	if (p_rssi != NULL) {
		*p_rssi = buf[22];
	}

	return true;
}

bool rtk_bt_mp_dtm_set_tx_count(uint8_t tx_pkt_cnt)
{
	uint8_t tx_cnt_mode_en = 0x01;
	uint8_t chg_aa = 0x00;
	uint8_t *p_access_addr = NULL;
	uint8_t prbs_fix = 0x00;

	/* OpCode: 0xFCDC, Data Len: Cmd(8+3), Event(6) */
	uint8_t buf[11] = {0};
	uint8_t i = 0;

	buf[2] = (uint8_t)(8);
	buf[3] = tx_cnt_mode_en;
	buf[4] = tx_pkt_cnt;
	buf[5] = chg_aa;
	if (p_access_addr != NULL) {
		for (i = 0; i < 4; i++) {
			buf[6 + i] = p_access_addr[i];
		}
	}
	buf[10] = prbs_fix;

	return hci_dtm_send(0xFCDC, buf, 11);
}

bool rtk_bt_mp_dtm_set_tx_power_index(uint8_t ble_1m, uint8_t ble_2m)
{
	uint8_t bdr_1m = 0xFF;
	uint8_t edr_2m = 0xFF;
	uint8_t edr_3m = 0xFF;

	/* OpCode: 0xFCE7, Data Len: Cmd(5+3), Event(6) */
	uint8_t buf[8] = {0};

	buf[2] = (uint8_t)(5);
	buf[3] = bdr_1m;
	buf[4] = edr_2m;
	buf[5] = edr_3m;
	buf[6] = ble_1m;
	buf[7] = ble_2m;

	return hci_dtm_send(0xFCE7, buf, 8);
}

bool rtk_bt_mp_dtm_set_tx_gaink(uint8_t tx_gain_k)
{
	uint8_t sub_index = 0x01;
	uint32_t value = (uint32_t)tx_gain_k;
	uint8_t *p_tx_gaink = NULL;
	uint32_t *p_tx_flatnessk = NULL;
	uint8_t *p_tx_path_lose = NULL;

	/* OpCode: 0xFD14, Data Len: Cmd(5+3), Event(12) */
	uint8_t buf[12] = {0};
	uint16_t len = 0;

	buf[3] = sub_index;
	if (sub_index == 0x00) {         /* get TX power */
		buf[2] = (uint8_t)(1);
		len = 4;
	} else if (sub_index == 0x01) {  /* set TX Gain K */
		buf[2] = (uint8_t)(2);
		buf[4] = (uint8_t)(value & 0xFF);
		len = 5;
	} else if (sub_index == 0x02) {  /* set TX Flatness K */
		buf[2] = (uint8_t)(5);
		buf[4] = (uint8_t)(value >> 0);
		buf[5] = (uint8_t)(value >> 8);
		buf[6] = (uint8_t)(value >> 16);
		buf[7] = (uint8_t)(value >> 24);
		len = 8;
	}

	if (false == hci_dtm_send(0xFD14, buf, len)) {
		return false;
	}

	if (sub_index == 0x00) {
		if (p_tx_gaink != NULL) {
			*p_tx_gaink = buf[6];
		}
		if (p_tx_flatnessk != NULL) {
			*p_tx_flatnessk = DTM_ARRAY_TO_UINT32(&buf[7]);
		}
		if (p_tx_path_lose != NULL) {
			*p_tx_path_lose = buf[11];
		}
	}

	return true;
}

bool rtk_bt_mp_dtm_set_tx_flatnessk(uint32_t tx_flastness_k)
{
	uint8_t sub_index = 0x02;
	uint32_t value = tx_flastness_k;
	uint8_t *p_tx_gaink = NULL;
	uint32_t *p_tx_flatnessk = NULL;
	uint8_t *p_tx_path_lose = NULL;

	/* OpCode: 0xFD14, Data Len: Cmd(5+3), Event(12) */
	uint8_t buf[12] = {0};
	uint16_t len = 0;

	buf[3] = sub_index;
	if (sub_index == 0x00) {         /* get TX power */
		buf[2] = (uint8_t)(1);
		len = 4;
	} else if (sub_index == 0x01) {  /* set TX Gain K */
		buf[2] = (uint8_t)(2);
		buf[4] = (uint8_t)(value & 0xFF);
		len = 5;
	} else if (sub_index == 0x02) {  /* set TX Flatness K */
		buf[2] = (uint8_t)(5);
		buf[4] = (uint8_t)(value >> 0);
		buf[5] = (uint8_t)(value >> 8);
		buf[6] = (uint8_t)(value >> 16);
		buf[7] = (uint8_t)(value >> 24);
		len = 8;
	}

	if (false == hci_dtm_send(0xFD14, buf, len)) {
		return false;
	}

	if (sub_index == 0x00) {
		if (p_tx_gaink != NULL) {
			*p_tx_gaink = buf[6];
		}
		if (p_tx_flatnessk != NULL) {
			*p_tx_flatnessk = DTM_ARRAY_TO_UINT32(&buf[7]);
		}
		if (p_tx_path_lose != NULL) {
			*p_tx_path_lose = buf[11];
		}
	}

	return true;
}

bool rtk_bt_mp_dtm_read_thermal(uint8_t *p_thermal_value)
{
	if (p_thermal_value == NULL) {
		BT_MP_ARG_LOGE("p_thermal_value");
		return false;
	}

	/* OpCode: 0xFC40, Data Len: Cmd(3), Event(7) */
	uint8_t buf[7] = {0};

	buf[2] = (uint8_t)(0);

	if (false == hci_dtm_send(0xFC40, buf, 3)) {
		return false;
	}

	if (p_thermal_value != NULL) {
		*p_thermal_value = buf[6];
	}

	return true;
}

bool rtk_bt_mp_dtm_set_disable_tx_power_tracking(void)
{
	uint8_t is_get_or_set = 0x00;
	uint8_t is_set_enable = 0x00;
	uint8_t *p_is_get_enable = NULL;

	/* OpCode: 0xFCE8, Data Len: Cmd(2+3), Event(7) */
	uint8_t buf[7] = {0};
	uint16_t len = 0;

	buf[3] = is_get_or_set;
	if (is_get_or_set == 0x00) {         /* set TX power tracking */
		buf[2] = (uint8_t)(2);
		buf[4] = is_set_enable;
		len = 5;
	} else if (is_get_or_set == 0x01) {  /* get TX power tracking */
		buf[2] = (uint8_t)(1);
		len = 4;
	}

	if (false == hci_dtm_send(0xFCE8, buf, len)) {
		return false;
	}

	if (is_get_or_set == 0x01 && p_is_get_enable != NULL) {
		*p_is_get_enable = buf[6];
	}

	return true;
}