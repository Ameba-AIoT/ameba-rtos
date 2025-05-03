#include <stdio.h>
#include <stdbool.h>
#include "hci/hci_common.h"
#include "hci/hci_transport.h"

#define RESERVE_LEN 1

#define DTM_ARRAY_TO_UINT16(_array)           \
        (((uint16_t)(*(_array + 0)) <<  0) |  \
         ((uint16_t)(*(_array + 1)) <<  8))

#define DTM_ARRAY_TO_UINT32(_array)           \
        (((uint32_t)(*(_array + 0)) <<  0) |  \
         ((uint32_t)(*(_array + 1)) <<  8) |  \
         ((uint32_t)(*(_array + 2)) << 16) |  \
         ((uint32_t)(*(_array + 3)) << 24))

uint8_t hci_dtm_reveiver_test_v1(uint8_t rx_chann)
{
	/* OpCode: 0x201D, Data Len: Cmd(1+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(1);
	buf[3] = rx_chann;

	return hci_sa_send_cmd_sync(0x201D, buf, buf[2] + 3);
}

uint8_t hci_dtm_reveiver_test_v2(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx)
{
	/* OpCode: 0x2033, Data Len: Cmd(3+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(3);
	buf[3] = rx_chann;
	buf[4] = phy;
	buf[5] = mod_idx;

	return hci_sa_send_cmd_sync(0x2033, buf, buf[2] + 3);
}

uint8_t hci_dtm_receiver_test_v3(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx,
								 uint8_t exp_cte_len, uint8_t exp_cte_type, uint8_t slot_durations,
								 uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	/* OpCode: 0x204F, Data Len: Cmd(7+sw_pattern_len+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 10 + sw_pattern_len];
	uint8_t *buf = buf_raw + RESERVE_LEN;
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

	return hci_sa_send_cmd_sync(0x204F, buf, buf[2] + 3);
}

uint8_t hci_dtm_transmitter_test_v1(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl)
{
	/* OpCode: 0x201E, Data Len: Cmd(3+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 6] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(3);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;

	return hci_sa_send_cmd_sync(0x201E, buf, buf[2] + 3);
}

uint8_t hci_dtm_transmitter_test_v2(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy)
{
	/* OpCode: 0x2034, Data Len: Cmd(4+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 7] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(4);
	buf[3] = tx_chann;
	buf[4] = data_len;
	buf[5] = pkt_pl;
	buf[6] = phy;

	return hci_sa_send_cmd_sync(0x2034, buf, buf[2] + 3);
}

uint8_t hci_dtm_transmitter_test_v3(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl,
									uint8_t phy, uint8_t cte_len, uint8_t cte_type,
									uint8_t sw_pattern_len, uint8_t *p_antenna_ids)
{
	/* OpCode: 0x2050, Data Len: Cmd(7+sw_pattern_len+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 10 + sw_pattern_len];
	uint8_t *buf = buf_raw + RESERVE_LEN;
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

	return hci_sa_send_cmd_sync(0x2050, buf, buf[2] + 3);
}

uint8_t hci_dtm_transmitter_test_v4(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl,
									uint8_t phy, uint8_t cte_len, uint8_t cte_type,
									uint8_t sw_pattern_len, uint8_t *p_antenna_ids, uint8_t tx_power_level)
{
	/* OpCode: 0x207B, Data Len: Cmd(8+sw_pattern_len+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 11 + sw_pattern_len];
	uint8_t *buf = buf_raw + RESERVE_LEN;
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
	buf[10 + i] = tx_power_level;

	return hci_sa_send_cmd_sync(0x207B, buf, buf[2] + 3);
}

uint8_t hci_dtm_test_end(uint16_t *p_num_pkts)
{
	/* OpCode: 0x201F, Data Len: Cmd(3), Event(8) */
	uint8_t buf_raw[RESERVE_LEN + 8] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send_cmd_sync(0x201F, buf, buf[2] + 3)) {
		return HCI_FAIL;
	}

	if (p_num_pkts != NULL) {
		*p_num_pkts = DTM_ARRAY_TO_UINT16(&buf[6]);
	}

	return HCI_SUCCESS;
}

uint8_t hci_dtm_vendor_get_receiver_report(uint32_t *p_rx_pkts, uint32_t *p_rx_bits, uint32_t *p_rx_error_bits,
										   uint16_t *p_ber, uint16_t *p_cfo, int8_t *p_rssi)
{
	/* OpCode: 0xFCE5, Data Len: Cmd(3), Event(23) */
	uint8_t buf_raw[RESERVE_LEN + 23] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send_cmd_sync(0xFCE5, buf, buf[2] + 3)) {
		return HCI_FAIL;
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

	return HCI_SUCCESS;
}

uint8_t hci_dtm_vendor_set_transmitter_count(uint8_t tx_cnt_mode_en, uint8_t tx_pkt_cnt,
											 uint8_t chg_aa, uint8_t *p_access_addr, uint8_t prbs_fix)
{
	/* OpCode: 0xFCDC, Data Len: Cmd(8+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 11] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;
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

	return hci_sa_send_cmd_sync(0xFCDC, buf, buf[2] + 3);
}

uint8_t hci_dtm_vendor_ctrl_tx_power(uint8_t bdr_1m, uint8_t edr_2m, uint8_t edr_3m,
									 uint8_t ble_1m, uint8_t ble_2m)
{
	/* OpCode: 0xFCE7, Data Len: Cmd(5+3), Event(6) */
	uint8_t buf_raw[RESERVE_LEN + 8] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(5);
	buf[3] = bdr_1m;
	buf[4] = edr_2m;
	buf[5] = edr_3m;
	buf[6] = ble_1m;
	buf[7] = ble_2m;

	return hci_sa_send_cmd_sync(0xFCE7, buf, buf[2] + 3);
}

uint8_t hci_dtm_vendor_k_power_setting(uint8_t sub_index, uint32_t value, uint8_t *p_tx_gaink,
									   uint32_t *p_tx_flatnessk, uint8_t *p_tx_path_lose)
{
	/* OpCode: 0xFD14, Data Len: Cmd(5+3), Event(12) */
	uint8_t buf_raw[RESERVE_LEN + 12] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[3] = sub_index;
	if (sub_index == 0x00) {         /* get TX power */
		buf[2] = (uint8_t)(1);
	} else if (sub_index == 0x01) {  /* set TX Gain K */
		buf[2] = (uint8_t)(2);
		buf[4] = (uint8_t)(value & 0xFF);
	} else if (sub_index == 0x02) {  /* set TX Flatness K */
		buf[2] = (uint8_t)(5);
		buf[4] = (uint8_t)(value >> 0);
		buf[5] = (uint8_t)(value >> 8);
		buf[6] = (uint8_t)(value >> 16);
		buf[7] = (uint8_t)(value >> 24);
	}

	if (HCI_SUCCESS != hci_sa_send_cmd_sync(0xFD14, buf, buf[2] + 3)) {
		return HCI_FAIL;
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

	return HCI_SUCCESS;
}

uint8_t hci_dtm_vendor_read_thermal_meter_data(uint8_t *p_thermal_value)
{
	/* OpCode: 0xFC40, Data Len: Cmd(3), Event(7) */
	uint8_t buf_raw[RESERVE_LEN + 7] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(0);

	if (HCI_SUCCESS != hci_sa_send_cmd_sync(0xFC40, buf, buf[2] + 3)) {
		return HCI_FAIL;
	}

	if (p_thermal_value != NULL) {
		*p_thermal_value = buf[6];
	}

	return HCI_SUCCESS;
}

uint8_t hci_dtm_vendor_enable_tx_power_tracking(uint8_t is_get_or_set, uint8_t is_set_enable,
												uint8_t *p_is_get_enable)
{
	/* OpCode: 0xFCE8, Data Len: Cmd(2+3), Event(7) */
	uint8_t buf_raw[RESERVE_LEN + 7] = {0};
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[3] = is_get_or_set;
	if (is_get_or_set == 0x00) {         /* set TX power tracking */
		buf[2] = (uint8_t)(2);
		buf[4] = is_set_enable;
	} else if (is_get_or_set == 0x01) {  /* get TX power tracking */
		buf[2] = (uint8_t)(1);
	}

	if (HCI_SUCCESS != hci_sa_send_cmd_sync(0xFCE8, buf, buf[2] + 3)) {
		return HCI_FAIL;
	}

	if (is_get_or_set == 0x01 && p_is_get_enable != NULL) {
		*p_is_get_enable = buf[6];
	}

	return HCI_SUCCESS;
}