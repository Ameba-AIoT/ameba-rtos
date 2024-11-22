#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include <bt_debug.h>
#include <bt_utils.h>
#include <atcmd_service.h>
#include <bt_mp_api.h>
#include "wifi_intf_drv_to_bt.h"
#include "bt_fast_mp_common.h"

bt_fast_mp_parse_efuse_t bt_efuse = {0};

static void *bt_timer_hdl = NULL;
static void *bt_stop_sema = NULL;

extern void wifi_btcoex_set_pta(enum pta_type type, u8 role, u8 process);
extern bool bt_fast_mp_golden_map_in_range(uint16_t efuse_offset);
extern void bt_fast_mp_efuse_init(void);
extern void bt_fast_mp_golden_map_func(uint16_t efuse_offset, uint8_t *p_efuse);

static int bt_fast_mp_parse_param(char *buf, char **argv)
{
	int argc = 0;
	char *temp = NULL;

	if (buf == NULL) {
		goto exit;
	}
	temp = buf;

	for (; argc < MAX_ARGC; argc++) {
		argv[argc] = strsep(&temp, " ");
		if (argv[argc] == NULL) {
			goto exit;
		}
	}

exit:
	return argc;
}

static void bt_fast_mp_timer_timeout_callback(void *timer_handle)
{
	(void)timer_handle;

	osif_sem_give(bt_stop_sema);
}

static void bt_fast_mp_test_start_dut_func(uint8_t bt_channel)
{
	uint16_t num_pkts = 0;
	int8_t rssi = 0;
	uint8_t antenna_ids[BT_TRX_SW_PATT_LEN] = {0};

	if (false == osif_sem_create(&bt_stop_sema, 0, 1)) {
		BT_LOGE("DUT semaphore create failed!\r\n");
		goto exit;
	}

	if (false == osif_timer_create(&bt_timer_hdl, "dut_timer", 0, BT_TX_TIME,
								   false, bt_fast_mp_timer_timeout_callback)) {
		BT_LOGE("DUT timer create failed!\r\n");
		goto exit;
	}

	// 1. DUT TX
	/// 1.1 disable tx power tracking
	if (0 == rtk_bt_mp_dtm_set_disable_tx_power_tracking()) {
		BT_LOGE("DUT disable TX power tracking failed!\r\n");
		goto exit;
	}

	/// 1.2 set tx power level
	if (bt_efuse.tx_power_le_1m != 0xFF || bt_efuse.tx_power_le_2m != 0xFF) {
		if (0 == rtk_bt_mp_dtm_set_tx_power_index(bt_efuse.tx_power_le_1m, bt_efuse.tx_power_le_2m)) {
			BT_LOGE("DUT set TX power level failed!\r\n");
			goto exit;
		}
	}

	/// 1.3 set tx gain k
	if (bt_efuse.bit_map & bt_efuse.bit_map_gaink_valid) {
		if (0 == rtk_bt_mp_dtm_set_tx_gaink((uint32_t)(bt_efuse.tx_gain_k))) {
			BT_LOGE("DUT set TX Gain K failed!\r\n");
			goto exit;
		}
	}

	/// 1.4 set flatness k
	if (bt_efuse.bit_map & bt_efuse.bit_map_flatnessk_valid) {
		if (0 == rtk_bt_mp_dtm_set_tx_flatnessk(bt_efuse.tx_flatness_k)) {
			BT_LOGE("DUT set TX Flatness K failed!\r\n");
			goto exit;
		}
	}

	/// 1.5 set tx pkt count
	if (0 == rtk_bt_mp_dtm_set_tx_count(BT_TX_PKT_COUNT)) {
		BT_LOGE("DUT set TX PKT Count failed!\r\n");
		goto exit;
	}

	/// 1.6 start tx
	BT_LOGA("\r\nDUT start TX: Channel = %d, PKT length = %d, PKT Payload Type = %d, PHY = %d\r\n",
			bt_channel, BT_TX_PKT_LENGTH, BT_TX_PKT_PL, BT_TX_PHY);
	osif_timer_start(&bt_timer_hdl);
	if (0 == rtk_bt_mp_dtm_tx_test_v3(bt_channel, BT_TX_PKT_LENGTH, BT_TX_PKT_PL, BT_TX_PHY,
									  BT_TX_CTE_LEN, BT_TX_CTE_TYPE, BT_TRX_SW_PATT_LEN, antenna_ids)) {
		BT_LOGE("DUT TX failed!\r\n");
		goto exit;
	}

	if (false == osif_sem_take(bt_stop_sema, BT_TAKE_SEM_TO)) {
		BT_LOGE("DUT TX semaphore take timeout!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_test_end(&num_pkts)) {
		BT_LOGE("DUT TX end failed!\r\n");
		goto exit;
	}
	BT_LOGA("DUT TX end.\r\n\r\n");

	// 2. DUT RX
	// extend DUT RX time to ensure GOLDEN return first
	BT_LOGA("DUT start RX: Channel = %d, PHY = %d, Modulation Index = %d\r\n",
			bt_channel, BT_RX_PHY, BT_RX_MODU_IDX);
	osif_timer_restart(&bt_timer_hdl, BT_RX_TIME + 100);
	if (0 == rtk_bt_mp_dtm_rx_test_v3(bt_channel, BT_RX_PHY, BT_RX_MODU_IDX, BT_RX_EXP_CTE_LEN,
									  BT_RX_EXP_CTE_TYPE, BT_RX_SLOT_DURA, BT_TRX_SW_PATT_LEN, antenna_ids)) {
		BT_LOGE("DUT RX failed!\r\n");
		goto exit;
	}

	if (false == osif_sem_take(bt_stop_sema, BT_TAKE_SEM_TO)) {
		BT_LOGE("DUT RX semaphore take timeout!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_test_end(&num_pkts)) {
		BT_LOGE("DUT RX end failed!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_get_rx_report(&rssi)) {
		BT_LOGE("DUT get RX RSSI failed!\r\n");
		goto exit;
	}
	BT_LOGA("DUT RX end: RX PKT number = %d, RX RSSI = %d\r\n\r\n", num_pkts, rssi);

exit:
	if (bt_stop_sema) {
		osif_sem_delete(bt_stop_sema);
		bt_stop_sema = NULL;
	}
	if (bt_timer_hdl) {
		osif_timer_delete(&bt_timer_hdl);
		bt_timer_hdl = NULL;
	}
}

static void bt_fast_mp_test_start_golden_func(uint8_t bt_channel)
{
	uint16_t num_pkts = 0;
	int8_t rssi = 0;
	uint8_t antenna_ids[BT_TRX_SW_PATT_LEN] = {0};

	if (false == osif_sem_create(&bt_stop_sema, 0, 1)) {
		BT_LOGE("GOLDEN semaphore create failed!\r\n");
		goto exit;
	}

	if (false == osif_timer_create(&bt_timer_hdl, "golden_timer", 0, BT_RX_TIME,
								   false, bt_fast_mp_timer_timeout_callback)) {
		BT_LOGE("GOLDEN timer create failed!\r\n");
		goto exit;
	}

	// 1. GOLDEN RX
	BT_LOGA("\r\nGOLDEN start RX: Channel = %d, PHY = %d, Modulation Index = %d\r\n",
			bt_channel, BT_RX_PHY, BT_RX_MODU_IDX);
	osif_timer_start(&bt_timer_hdl);
	if (0 == rtk_bt_mp_dtm_rx_test_v3(bt_channel, BT_RX_PHY, BT_RX_MODU_IDX, BT_RX_EXP_CTE_LEN,
									  BT_RX_EXP_CTE_TYPE, BT_RX_SLOT_DURA, BT_TRX_SW_PATT_LEN, antenna_ids)) {
		BT_LOGE("GOLDEN RX failed!\r\n");
		goto exit;
	}

	if (false == osif_sem_take(bt_stop_sema, BT_TAKE_SEM_TO)) {
		BT_LOGE("GOLDEN RX semaphore take timeout!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_test_end(&num_pkts)) {
		BT_LOGE("GOLDEN RX end failed!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_get_rx_report(&rssi)) {
		BT_LOGE("GOLDEN get RX RSSI failed!\r\n");
		goto exit;
	}
	BT_LOGA("GOLDEN RX end: RX PKT number = %d, RX RSSI = %d\r\n\r\n", num_pkts, rssi);

	// 2. GOLDEN TX
	if (0 == rtk_bt_mp_dtm_set_tx_count(BT_TX_PKT_COUNT)) {
		BT_LOGE("GOLDEN TX set TX PKT count failed!\r\n");
		goto exit;
	}

	BT_LOGA("GOLDEN start TX: Channel = %d, PKT length = %d, PKT Payload Type = %d, PHY = %d\r\n",
			bt_channel, BT_TX_PKT_LENGTH, BT_TX_PKT_PL, BT_TX_PHY);
	osif_timer_restart(&bt_timer_hdl, BT_TX_TIME);
	if (0 == rtk_bt_mp_dtm_tx_test_v3(bt_channel, BT_TX_PKT_LENGTH, BT_TX_PKT_PL, BT_TX_PHY,
									  BT_TX_CTE_LEN, BT_TX_CTE_TYPE, BT_TRX_SW_PATT_LEN, antenna_ids)) {
		BT_LOGE("GOLDEN TX failed!\r\n");
		goto exit;
	}

	if (false == osif_sem_take(bt_stop_sema, BT_TAKE_SEM_TO)) {
		BT_LOGE("GOLDEN TX semaphore take timeout!\r\n");
		goto exit;
	}

	if (0 == rtk_bt_mp_dtm_test_end(&num_pkts)) {
		BT_LOGE("GOLDEN TX end failed!\r\n");
		goto exit;
	}
	BT_LOGA("GOLDEN TX end.\r\n\r\n");

exit:
	if (bt_stop_sema) {
		osif_sem_delete(bt_stop_sema);
		bt_stop_sema = NULL;
	}
	if (bt_timer_hdl) {
		osif_timer_delete(&bt_timer_hdl);
		bt_timer_hdl = NULL;
	}
}

void bt_fast_mp_cmd_handle_api(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	char *p_buf = NULL;
	char *buf = NULL;
	uint16_t efuse_offset = 0;
	uint8_t efuse_arr[16] = {0};
	uint8_t bt_channel = BT_TRX_CHANNEL;

	buf = (char *)osif_mem_alloc(RAM_TYPE_DATA_ON, strlen(arg) + 1);
	if (!buf) {
		BT_LOGE("Error: Memory alloc failed!\r\n");
		return;
	}
	memset(buf, 0, strlen(arg) + 1);
	memcpy(buf, arg, strlen(arg));
	p_buf = buf;

	argc = bt_fast_mp_parse_param(p_buf, argv);

	if (argc < 2) {
		BT_LOGE("Error: Wrong input args number!\r\n");
		goto exit;
	}

	if (strcmp(argv[0], "fastmp") != 0) {
		BT_LOGE("Error: Wrong input arg! (%s)\r\n", argv[0]);
		goto exit;
	}

	if (strcmp(argv[1], "golden_map") == 0) {
		p_buf = argv[2];
		efuse_offset = (uint16_t)strtoul((const char *)p_buf, (char **)NULL, 16);
		if (!efuse_offset) {
			BT_LOGE("Error: Wrong golden_map offset!\r\n");
			goto exit;
		}
		p_buf = argv[3];
		if (p_buf == NULL || 32 != strlen(p_buf) || !hexdata_str_to_array(p_buf, efuse_arr, 16)) {
			BT_LOGE("Error: Wrong golden_map value!\r\n");
			goto exit;
		}
		if (!bt_fast_mp_golden_map_in_range(efuse_offset)) {
			goto exit;
		}
		if (!bt_efuse.efuse_init_state) {
			bt_fast_mp_efuse_init();
		}
		bt_fast_mp_golden_map_func(efuse_offset, efuse_arr);
	} else if (strcmp(argv[1], "test_start") == 0) {
		if (strcmp(argv[2], "DUT") != 0 && strcmp(argv[2], "GOLDEN") != 0) {
			BT_LOGE("Error: Wrong input role! (%s)\r\n", argv[2]);
			goto exit;
		}
		if (argc > 3) {
			bt_channel = str_to_int(argv[3]);
		}
		if (bt_channel > 39) {
			BT_LOGE("Error: BT channel not in range! (range: 0 ~ 39)\r\n");
			goto exit;
		}
		if (strcmp(argv[2], "DUT") == 0 &&
			(!bt_efuse.efuse_init_state || bt_efuse.efuse_flag != bt_efuse.efuse_flag_cmpl)) {
			BT_LOGE("Error: DUT golden_map cmd not complete yet!\r\n");
			goto exit;
		}
		BT_LOGA("BT power on.\n\r");
		rtk_bt_mp_dtm_power_on();
		wifi_btcoex_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
		BT_LOGA("Switch GNT_BT to BT.\n\r");
		if (strcmp(argv[2], "DUT") == 0) {
			bt_fast_mp_test_start_dut_func(bt_channel);
		}
		if (strcmp(argv[2], "GOLDEN") == 0) {
			bt_fast_mp_test_start_golden_func(bt_channel);
		}
		BT_LOGA("BT power off.\n\r");
		rtk_bt_mp_dtm_power_off();
		wifi_btcoex_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
		BT_LOGA("Switch GNT_BT to WIFI.\n\r");
	} else {
		goto exit;
	}

exit:
	if (buf) {
		osif_mem_free(buf);
	}
}