#include <stdio.h>
#include <string.h>
#include <basic_types.h>
#include <bt_debug.h>
#include "bt_fast_mp_common.h"
#include "hci_if_dtm.h"

#define FAST_MP_EFUSE_OFFSET    0x1B0
#define FAST_MP_EFUSE_LEN_HEX   3
#define EFUSE(x)                ((x) & 0x000F)

#define HCI_VENDOR_MP           0xFCEB
#define MODULE_PHY              0x05

#define DTM_ARRAY_TO_UINT16(_array)           \
        (((uint16_t)(*(_array + 0)) <<  0) | \
         ((uint16_t)(*(_array + 1)) <<  8))

#define DTM_ARRAY_TO_UINT32(_array)           \
        (((uint32_t)(*(_array + 0)) <<  0) | \
         ((uint32_t)(*(_array + 1)) <<  8) | \
         ((uint32_t)(*(_array + 2)) << 16) | \
         ((uint32_t)(*(_array + 3)) << 24))

#define BT_MP_ARG_LOGE(str)  BT_LOGE("%s (line: %d) arg NOT IN RANGE! (arg: %s)\r\n", __func__, __LINE__, str)

#define VALUE_GET             0x00
#define VALUE_SET             0x01
#define TX_POWER_TYPE         0x01

extern bt_fast_mp_parse_efuse_t bt_efuse;

bool bt_fast_mp_golden_map_in_range(uint16_t efuse_offset)
{
	uint16_t diff = efuse_offset - FAST_MP_EFUSE_OFFSET;

	return ((diff / 16 < FAST_MP_EFUSE_LEN_HEX) && (diff % 16 == 0)) ? true : false;
}

void bt_fast_mp_efuse_init(void)
{
	memset(&bt_efuse, 0, sizeof(bt_fast_mp_parse_efuse_t));
	bt_efuse.efuse_init_state = 1;
	bt_efuse.efuse_flag_cmpl = (BIT0 | BIT1 | BIT2);
	bt_efuse.bit_map_offset = 0x1BE;
	bt_efuse.bit_map_gaink_valid = BIT1;
	bt_efuse.bit_map_flatnessk_valid = BIT2;
	bt_efuse.tx_gain_k_offset = 0x1BF;
	bt_efuse.tx_flatness_k_offset = 0x1C0;
	bt_efuse.tx_power_le_1m_offset = 0X1C7;
	bt_efuse.tx_power_le_2m_offset = 0x1C8;
}

void bt_fast_mp_golden_map_func(uint16_t efuse_offset, uint8_t *p_efuse)
{
	switch (efuse_offset) {
	case FAST_MP_EFUSE_OFFSET: {
		bt_efuse.bit_map = p_efuse[EFUSE(bt_efuse.bit_map_offset)];
		bt_efuse.tx_gain_k = p_efuse[EFUSE(bt_efuse.tx_gain_k_offset)];
		bt_efuse.efuse_flag |= BIT0;
		BT_LOGD("Parse efuse: offset = 0x%x, Bit map = 0x%x, TX Gain K = 0x%x\r\n",
				efuse_offset, bt_efuse.bit_map, bt_efuse.tx_gain_k);
		if (!(bt_efuse.bit_map & bt_efuse.bit_map_gaink_valid)) {
			BT_LOGA("Parse efuse: TX Gain K valid bit is 0! which means TX Gain K value " \
					"will not be applied.\r\n");
		}
		if (!(bt_efuse.bit_map & bt_efuse.bit_map_flatnessk_valid)) {
			BT_LOGA("Parse efuse: TX Flatness K valid bit is 0! which means TX Flatness K value " \
					"will not be applied.\r\n");
		}
		break;
	}

	case FAST_MP_EFUSE_OFFSET + 0x10: {
		bt_efuse.tx_flatness_k =
			((uint32_t)(p_efuse[EFUSE(bt_efuse.tx_flatness_k_offset + 0)] >> 2) << 0)  |
			((uint32_t)(p_efuse[EFUSE(bt_efuse.tx_flatness_k_offset + 1)] >> 2) << 8)  |
			((uint32_t)(p_efuse[EFUSE(bt_efuse.tx_flatness_k_offset + 2)] >> 2) << 16) |
			((uint32_t)(p_efuse[EFUSE(bt_efuse.tx_flatness_k_offset + 3)] >> 2) << 24);
		bt_efuse.tx_power_le_1m = p_efuse[EFUSE(bt_efuse.tx_power_le_1m_offset)];
		bt_efuse.tx_power_le_2m = p_efuse[EFUSE(bt_efuse.tx_power_le_2m_offset)];
		bt_efuse.efuse_flag |= BIT1;
		BT_LOGD("Parse efuse: offset = 0x%x, TX Flatness K = 0x%x, TX power index LE 1M = 0x%x, " \
				"TX power index LE 2M = 0x%x\r\n",
				efuse_offset, bt_efuse.tx_flatness_k,
				bt_efuse.tx_power_le_1m, bt_efuse.tx_power_le_2m);
		if (bt_efuse.tx_power_le_1m == 0xFF) {
			BT_LOGA("Parse efuse: TX power index for LE 1M is 0xFF! which means TX power index value for LE 1M " \
					"will not be applied.\r\n");
		}
		if (bt_efuse.tx_power_le_2m == 0xFF) {
			BT_LOGA("Parse efuse: TX power index for LE 2M is 0xFF! which means TX power index value for LE 2M " \
					"will not be applied.\r\n");
		}
		break;
	}

	case FAST_MP_EFUSE_OFFSET + 0x20: {
		bt_efuse.efuse_flag |= BIT2;
		BT_LOGD("Parse efuse: offset = 0x%x\r\n", efuse_offset);
		break;
	}

	default:
		BT_LOGE("Error: Wrong input efuse offset! (0x%x)\r\n", efuse_offset);
		break;
	}
}

static bool _hci_dtm_send(uint8_t subcmd, uint8_t *buf, uint16_t params_len)
{
	buf[2] = (uint8_t)(params_len + 2);
	buf[3] = MODULE_PHY;
	buf[4] = subcmd;

	if (false == hci_dtm_send(HCI_VENDOR_MP, buf, params_len + 5)) {
		return false;
	}

	return true;
}

bool rtk_bt_mp_dtm_get_rx_report(int8_t *p_rssi)
{
	if (p_rssi == NULL) {
		BT_MP_ARG_LOGE("p_rssi");
		return false;
	}
	uint8_t subcmd = 0x0C;
	uint8_t params_len = 0;
	uint8_t buf[32] = {0};

	uint32_t *p_rx_pkts = NULL;
	uint32_t *p_rx_bits = NULL;
	uint32_t *p_rx_error_bits = NULL;
	uint16_t *p_ber = NULL;
	uint16_t *p_cfo = NULL;

	if (false == _hci_dtm_send(subcmd, buf, params_len)) {
		return false;
	}

	if (p_rx_pkts != NULL) {
		*p_rx_pkts = DTM_ARRAY_TO_UINT32(&buf[8]);
	}
	if (p_rx_bits != NULL) {
		*p_rx_bits = DTM_ARRAY_TO_UINT32(&buf[12]);
	}
	if (p_rx_error_bits != NULL) {
		*p_rx_error_bits = DTM_ARRAY_TO_UINT32(&buf[16]);
	}
	if (p_ber != NULL) {
		*p_ber = DTM_ARRAY_TO_UINT16(&buf[29]);
	}
	if (p_cfo != NULL) {
		*p_cfo = DTM_ARRAY_TO_UINT16(&buf[22]);
	}
	if (p_rssi != NULL) {
		*p_rssi = (int8_t)buf[24];
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
	uint8_t subcmd = 0x00;
	uint8_t params_len = 13;
	uint8_t buf[32] = {0};

	buf[5] = VALUE_SET;
	buf[6] = TX_POWER_TYPE;
	buf[7] = 0xFF;
	buf[8] = 0xFF;
	buf[9] = 0xFF;
	buf[10] = ble_1m;
	buf[11] = ble_1m;
	buf[12] = ble_1m;
	buf[13] = ble_1m;
	buf[14] = ble_2m;
	buf[15] = ble_2m;
	buf[16] = ble_2m;
	buf[17] = ble_2m;

	return _hci_dtm_send(subcmd, buf, params_len);
}

bool rtk_bt_mp_dtm_set_tx_gaink(uint8_t tx_gain_k)
{
	uint8_t subcmd = 0x04;
	uint8_t params_len = 2;
	uint8_t buf[32] = {0};

	buf[5] = VALUE_SET;
	buf[6] = tx_gain_k;

	return _hci_dtm_send(subcmd, buf, params_len);
}

bool rtk_bt_mp_dtm_set_tx_flatnessk(uint32_t tx_flatness_k)
{
	uint8_t subcmd = 0x05;
	uint8_t params_len = 5;
	uint8_t buf[32] = {0};

	buf[5] = VALUE_SET;
	buf[6] = (uint8_t)(tx_flatness_k >> 0);
	buf[7] = (uint8_t)(tx_flatness_k >> 8);
	buf[8] = (uint8_t)(tx_flatness_k >> 16);
	buf[9] = (uint8_t)(tx_flatness_k >> 24);

	return _hci_dtm_send(subcmd, buf, params_len);
}

bool rtk_bt_mp_dtm_read_thermal(uint8_t *p_thermal_value)
{
	if (p_thermal_value == NULL) {
		BT_MP_ARG_LOGE("p_thermal_value");
		return false;
	}

	uint8_t subcmd = 0x02;
	uint8_t params_len = 1;
	uint8_t buf[32] = {0};

	buf[5] = VALUE_GET;

	if (false == _hci_dtm_send(subcmd, buf, params_len)) {
		return false;
	}

	if (p_thermal_value != NULL) {
		*p_thermal_value = buf[8];
	}

	return true;
}

bool rtk_bt_mp_dtm_set_disable_tx_power_tracking(void)
{
	uint8_t subcmd = 0x03;
	uint8_t params_len = 1;
	uint8_t buf[32] = {0};
	uint8_t is_get_or_set = 0x00;        /* type:VALUE_SET */
	uint8_t is_set_enable = 0x00;        /* disable_tx_power_tracking */
	uint8_t *p_is_get_enable = NULL;

	if (is_get_or_set == 0x00) {         /* set TX power tracking */
		buf[5] = VALUE_SET;
		buf[6] = is_set_enable;
		params_len = 2;
	} else if (is_get_or_set == 0x01) {  /* get TX power tracking */
		buf[5] = VALUE_GET;
		params_len = 1;
	}

	if (false == _hci_dtm_send(subcmd, buf, params_len)) {
		return false;
	}

	if (is_get_or_set == 0x01 && p_is_get_enable != NULL) {
		*p_is_get_enable = buf[8];
	}

	return true;
}
