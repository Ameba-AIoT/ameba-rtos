/**
 * @file      bt_mp_api.h
 * @author
 * @brief     Bluetooth Common function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_MP_API_H__
#define __BT_MP_API_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @typedef   rtk_bt_mp_phy_rx_t
 * @brief     Bluetooth LE PHY type for receiver.
 */
typedef enum {
	RTK_BT_MP_RX_PHYS_1M       = 0x01,    /**< LE PHY 1M used. */
	RTK_BT_MP_RX_PHYS_2M       = 0x02,    /**< LE PHY 2M used. */
	RTK_BT_MP_RX_PHYS_CODED    = 0x03,    /**< LE Coded PHY used. */
} rtk_bt_mp_phy_rx_t;

/**
 * @typedef   rtk_bt_mp_phy_tx_t
 * @brief     Bluetooth LE PHY type for transmitter.
 */
typedef enum {
	RTK_BT_MP_TX_PHYS_1M          = 0x01,    /**< LE PHY 1M used. */
	RTK_BT_MP_TX_PHYS_2M          = 0x02,    /**< LE PHY 2M used. */
	RTK_BT_MP_TX_PHYS_CODED_S8    = 0x03,    /**< LE Coded PHY with S=8 data coding used. */
	RTK_BT_MP_TX_PHYS_CODED_S2    = 0x04,    /**< LE Coded PHY with S=2 data coding used. */
} rtk_bt_mp_phy_tx_t;

/**
 * @typedef   rtk_bt_mp_mod_idx_t
 * @brief     Transmitter will have a modulation index.
 */
typedef enum {
	RTK_BT_MP_MODULATION_INDEX_STANDARD     = 0x00,    /**< Standard modulation index. */
	RTK_BT_MP_MODULATION_INDEX_STABLE       = 0x01,    /**< Stable modulation index. */
} rtk_bt_mp_mod_idx_t;

/**
 * @typedef   rtk_bt_mp_cte_type_t
 * @brief     Constant Tone Extension type.
 */
typedef enum {
	RTK_BT_MP_CTE_TYPE_AOA                = 0x00,    /**< AoA Constant Tone Extension. */
	RTK_BT_MP_CTE_TYPE_AOD_1US_SLOT       = 0x01,    /**< AoD Constant Tone Extension with 1 us slots. */
	RTK_BT_MP_CTE_TYPE_AOD_2US_SLOT       = 0x02,    /**< AoD Constant Tone Extension with 2 us slots. */
} rtk_bt_mp_cte_type_t;

/**
 * @typedef   rtk_bt_mp_slot_durations_t
 * @brief     Slot durations type.
 */
typedef enum {
	RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_1US    = 0x01,    /**< Switching and sampling slots are 1 us each. */
	RTK_BT_MP_SLOT_DURATIONS_SWITCH_SAMPLE_2US    = 0x02,    /**< Switching and sampling slots are 2 us each. */
} rtk_bt_mp_slot_durations_t;

/**
 * @typedef   rtk_bt_mp_packet_payload_t
 * @brief     Transmitted packet payload type.
 */
typedef enum {
	RTK_BT_MP_PACKET_PAYLOAD_PRBS9    = 0x00,    /**< PRBS9 sequence '11111111100000111101' (in transmission order). */
	RTK_BT_MP_PACKET_PAYLOAD_1100     = 0x01,    /**< Repeated '11110000' (in transmission order) sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_10       = 0x02,    /**< Repeated '10101010' (in transmission order) sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_PRBS15   = 0x03,    /**< PRBS15 sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_ALL1     = 0x04,    /**< Repeated '11111111' (in transmission order) sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_ALL0     = 0x05,    /**< Repeated '00000000' (in transmission order) sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_0011     = 0x06,    /**< Repeated '00001111' (in transmission order) sequence. */
	RTK_BT_MP_PACKET_PAYLOAD_01       = 0x07,    /**< Repeated '01010101' (in transmission order) sequence. */
} rtk_bt_mp_packet_payload_t;

/**
 * @defgroup  bt_mp_api BT MP APIs
 * @brief     BT MP test function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     BT power on for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_on(void);

/**
 * @brief     BT power off for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_off(void);

/**
 * @brief     BT power on for Fast MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_dtm_power_on(void);

/**
 * @brief     BT power off for Fast MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_dtm_power_off(void);

/**
 * @brief     Start receiver test for Fast MP test.
 * @param[in] rx_chann: Channel to receive packets. Range: 0x00 - 0x27.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_rx_test_v1(uint8_t rx_chann);

/**
 * @brief     Start receiver test for Fast MP test.
 * @param[in] rx_chann: Channel to receive packets. Range: 0x00 - 0x27.
 * @param[in] phy:      PHY to receive packets: @ref rtk_bt_mp_phy_rx_t.
 * @param[in] mod_idx:  Modulation index to receive packets: @ref rtk_bt_mp_mod_idx_t.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_rx_test_v2(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx);

/**
 * @brief     Start receiver test for Fast MP test.
 * @param[in] rx_chann: Channel to receive packets. Range: 0x00 - 0x27.
 * @param[in] phy:      PHY to receive packets: @ref rtk_bt_mp_phy_rx_t.
 * @param[in] mod_idx:  Modulation index to receive packets: @ref rtk_bt_mp_mod_idx_t.
 * @param[in] exp_cte_len:  Expected length of the Constant Tone Extensions in received test reference packets.
                            Range: 0x00 (No Constant Tone Extension expected (default)) or 0x02 to 0x14.
                            Units: 8 us.
 * @param[in] exp_cte_type: Expected type of the Constant Tone Extensions in received test reference
                            packets: @ref rtk_bt_mp_cte_type_t.
 * @param[in] slot_durations: Slot durations to receive packets and shall be ignored when exp_cte_type
                              is not set to RTK_BT_MP_CTE_TYPE_AOA: @ref rtk_bt_mp_slot_durations_t.
 * @param[in] sw_pattern_len: The number of Antenna IDs in the pattern and shall be ignored when
                              exp_cte_type is not set to RTK_BT_MP_CTE_TYPE_AOA.
                              Range: 0x02 to max_switching_pattern_length supported by controller,
                              max_switching_pattern_length shall be less than or equal to 0x4B.
 * @param[in] p_antenna_ids:  Antenna ID in the pattern and shall be ignored when exp_cte_type
                              is not set to RTK_BT_MP_CTE_TYPE_AOA.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_rx_test_v3(uint8_t rx_chann, uint8_t phy, uint8_t mod_idx,
								 uint8_t exp_cte_len, uint8_t exp_cte_type, uint8_t slot_durations,
								 uint8_t sw_pattern_len, uint8_t *p_antenna_ids);

/**
 * @brief     Start transmitter test for Fast MP test.
 * @param[in] tx_chann: Channel to transmit packets. Range: 0x00 - 0x27.
 * @param[in] data_len: Length in bytes of payload data in each packet.
 * @param[in] pkt_pl:   Contents of the payload of the test reference packets: @ref rtk_bt_mp_packet_payload_t.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_tx_test_v1(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl);

/**
 * @brief     Start transmitter test for Fast MP test.
 * @param[in] tx_chann: Channel to transmit packets. Range: 0x00 - 0x27.
 * @param[in] data_len: Length in bytes of payload data in each packet.
 * @param[in] pkt_pl:   Contents of the payload of the test reference packets: @ref rtk_bt_mp_packet_payload_t.
 * @param[in] phy:      PHY to receive packets: @ref rtk_bt_mp_phy_tx_t.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_tx_test_v2(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy);

/**
 * @brief     Start transmitter test for Fast MP test.
 * @param[in] tx_chann: Channel to transmit packets. Range: 0x00 - 0x27.
 * @param[in] data_len: Length in bytes of payload data in each packet.
 * @param[in] pkt_pl:   Contents of the payload of the test reference packets: @ref rtk_bt_mp_packet_payload_t.
 * @param[in] phy:      PHY to receive packets: @ref rtk_bt_mp_phy_tx_t.
 * @param[in] cte_len:  Length of the Constant Tone Extensions in transmitted test reference packets.
                        Range: 0x00 (No Constant Tone Extension expected (default)) or 0x02 to 0x14.
                        Units: 8 us.
 * @param[in] cte_type: Type of the Constant Tone Extensions in transmitted test reference packets:
                        @ref rtk_bt_mp_cte_type_t.
 * @param[in] sw_pattern_len: The number of Antenna IDs in the pattern and shall be ignored when
                              cte_type is set to RTK_BT_MP_CTE_TYPE_AOA.
                              Range: 0x02 to max_switching_pattern_length supported by controller,
                              max_switching_pattern_length shall be less than or equal to 0x4B.
 * @param[in] p_antenna_ids:  Antenna ID in the pattern and shall be ignored when cte_type
                              is set to RTK_BT_MP_CTE_TYPE_AOA.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_tx_test_v3(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
								 uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids);

/**
 * @brief     Start transmitter test for Fast MP test.
 * @param[in] tx_chann: Channel to transmit packets. Range: 0x00 - 0x27.
 * @param[in] data_len: Length in bytes of payload data in each packet.
 * @param[in] pkt_pl:   Contents of the payload of the test reference packets: @ref rtk_bt_mp_packet_payload_t.
 * @param[in] phy:      PHY to receive packets: @ref rtk_bt_mp_phy_tx_t.
 * @param[in] cte_len:  Length of the Constant Tone Extensions in transmitted test reference packets.
                        Range: 0x00 (No Constant Tone Extension expected (default)) or 0x02 to 0x14.
                        Units: 8 us.
 * @param[in] cte_type: Type of the Constant Tone Extensions in transmitted test reference packets:
                        @ref rtk_bt_mp_cte_type_t.
 * @param[in] sw_pattern_len: The number of Antenna IDs in the pattern and shall be ignored when
                              cte_type is set to RTK_BT_MP_CTE_TYPE_AOA.
                              Range: 0x02 to max_switching_pattern_length supported by controller,
                              max_switching_pattern_length shall be less than or equal to 0x4B.
 * @param[in] p_antenna_ids:  Antenna ID in the pattern and shall be ignored when cte_type
                              is set to RTK_BT_MP_CTE_TYPE_AOA.
 * @param[in] tx_power_level: Set transmitter to the specified or the nearest transmit power level.
                              Range: 0xXX Set transmitter to the specified or the nearest transmit power level
                                          from -127 to +20.
                                          Units: dBm.
                                     0x7E Set transmitter to minimum transmit power level.
                                     0x7F Set transmitter to maximum transmit power level.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_tx_test_v4(uint8_t tx_chann, uint8_t data_len, uint8_t pkt_pl, uint8_t phy,
								 uint8_t cte_len, uint8_t cte_type, uint8_t sw_pattern_len, uint8_t *p_antenna_ids,
								 uint8_t tx_power_level);

/**
 * @brief     Make test end and check packet count for Fast MP test.
 * @param[out] p_num_pkts: Number of packets received.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_test_end(uint16_t *p_num_pkts);

/**
 * @brief     Get receiver report for Fast MP test.
 * @param[out] p_rssi: RSSI value.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_get_rx_report(int8_t *p_rssi);

/**
 * @brief     Set transmitter packet count for Fast MP test.
 * @param[in] tx_pkt_cnt: Set transmitter packet count.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_set_tx_count(uint8_t tx_pkt_cnt);

/**
 * @brief     Set TX power index for Fast MP test.
 * @param[in] ble_1m: BLE 1M Power level.
 * @param[in] ble_2m: BLE 2M Power level.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_set_tx_power_index(uint8_t ble_1m, uint8_t ble_2m);

/**
 * @brief     Set TX Gain K for Fast MP test.
 * @param[in] tx_gain_k: TX Gain K value, whish is 2’ complement.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_set_tx_gaink(uint8_t tx_gain_k);

/**
 * @brief     Set TX Flatness K for Fast MP test.
 * @param[in] tx_flastness_k: TX Flatness K value, whish is 2’ complement.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_set_tx_flatnessk(uint32_t tx_flastness_k);

/**
 * @brief     Read thermal meter value for Fast MP test.
 * @param[out] p_thermal_value: Thermal meter value.
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_read_thermal(uint8_t *p_thermal_value);

/**
 * @brief     Disable TX power tracking for Fast MP test.
 * @param     None
 * @return
 *      - 0 : Failed
 *      - 1 : Succeed
 */
uint8_t rtk_bt_mp_dtm_set_disable_tx_power_tracking(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_MP_API_H__ */
