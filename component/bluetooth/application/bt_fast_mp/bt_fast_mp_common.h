#ifndef _BT_FAST_MP_COMMON_H_
#define _BT_FAST_MP_COMMON_H_

#define BT_TX_TIME          200   // ms
#define BT_RX_TIME          1200
#define BT_TAKE_SEM_TO      2000

#define BT_TX_PKT_LENGTH    37
#define BT_TX_PKT_COUNT     0xFF
#define BT_TX_PKT_PL        RTK_BT_MP_PACKET_PAYLOAD_PRBS9
#define BT_TX_PHY           RTK_BT_MP_TX_PHYS_1M
#define BT_TX_CTE_LEN       0
#define BT_TX_CTE_TYPE      RTK_BT_MP_CTE_TYPE_AOA

#define BT_RX_PHY           RTK_BT_MP_RX_PHYS_1M
#define BT_RX_MODU_IDX      RTK_BT_MP_MODULATION_INDEX_STABLE
#define BT_RX_EXP_CTE_LEN   0
#define BT_RX_EXP_CTE_TYPE  RTK_BT_MP_CTE_TYPE_AOD_1US_SLOT
#define BT_RX_SLOT_DURA     RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_1US

#define BT_TRX_CHANNEL      19
#define BT_TRX_SW_PATT_LEN  2

typedef struct {
	uint8_t  efuse_init_state;
	uint8_t  efuse_flag_cmpl;
	uint8_t  efuse_flag;
	uint16_t bit_map_offset;
	uint8_t  bit_map_gaink_valid;
	uint8_t  bit_map_flatnessk_valid;
	uint8_t  bit_map;
	uint16_t tx_gain_k_offset;
	uint8_t  tx_gain_k;
	uint16_t tx_flatness_k_offset;
	uint32_t tx_flatness_k;
	uint16_t tx_power_le_1m_offset;
	uint8_t  tx_power_le_1m;
	uint16_t tx_power_le_2m_offset;
	uint8_t  tx_power_le_2m;
} bt_fast_mp_parse_efuse_t;

#endif