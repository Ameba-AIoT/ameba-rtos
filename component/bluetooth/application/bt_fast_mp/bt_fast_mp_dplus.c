#include <stdio.h>
#include <string.h>
#include <basic_types.h>
#include <bt_debug.h>
#include "bt_fast_mp_common.h"

#define FAST_MP_EFUSE_OFFSET    0x1B0
#define FAST_MP_EFUSE_LEN_HEX   3
#define EFUSE(x)                ((x) & 0x000F)

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