/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>

#include <throughput_include.h>

extern throughput_config_param_t config_param;
static throughput_config_param_t def_tp_param = {
	.mode                     = THROUGHPUT_MODE_NOTIFY_WRITE_CMD,
	.phy                      = THROUGHPUT_PHY_2M,
	.conn_interval            = 0x6,
	.conn_peripheral_latency  = 0,
	.conn_supervision_timeout = THROUGHPUT_SUPV_TIMEOUT,
	.length_s                 = THROUGHPUT_DATA_LEN_MAX,
	.length_c                 = THROUGHPUT_DATA_LEN_MAX,
	.period                   = THROUGHPUT_PRINT_PERIOD,
	.duration                 = THROUGHPUT_TIME_MAX,
};

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static rtk_bt_le_ext_create_conn_param_t def_tp_ext_conn_param = {
	.filter_policy     = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.own_addr_type     = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr         = {
		.type          = RTK_BT_LE_ADDR_TYPE_PUBLIC,
		.addr_val      = {0},
	},
	.init_phys         = {true, true, true},
	.scan_interval     = {0x60, 0x60, 0x60},
	.scan_window       = {0x30, 0x30, 0x30},
	.conn_interval_min = {0x28, 0x28, 0x28},
	.conn_interval_max = {0x28, 0x28, 0x28},
	.conn_latency      = {0, 0, 0},
	.supv_timeout      = {0x640, 0x640, 0x640},
	.scan_timeout      = 1000,
};
#else
static rtk_bt_le_create_conn_param_t def_tp_conn_param = {
	.peer_addr         = {
		.type          = (rtk_bt_le_addr_type_t)0,
		.addr_val      = {0},
	},
	.scan_interval     = 0x60,
	.scan_window       = 0x30,
	.filter_policy     = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.conn_interval_max = 0x28,
	.conn_interval_min = 0x28,
	.conn_latency      = 0,
	.supv_timeout      = 1000,
	.scan_timeout      = 1000,
};
#endif

static int atcmd_ble_throughput_conn(int argc, char **argv)
{
	rtk_bt_le_addr_t addr = {(rtk_bt_le_addr_type_t)0, {0}};
	throughput_test_mode_t mode;
	throughput_tx_end_t tx_end;
	throughput_phy_t phy;
	throughput_config_param_t *p_cfg_param;
	uint16_t ret = 0;
	char addr_str[30] = {0};

	if (argc < 3) {
		BT_LOGE("TP client connect op failed! wrong args num!\r\n");
		return -1;
	}

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	if (false == hexdata_str_to_bd_addr(argv[1], addr.addr_val, RTK_BD_ADDR_LEN)) {
		return -1;
	}
	rtk_bt_le_addr_to_str(&addr, addr_str, sizeof(addr_str));

	p_cfg_param = &config_param;
	memcpy(p_cfg_param, &def_tp_param, sizeof(throughput_config_param_t));

	if (strcmp(argv[2], "NTF") == 0 || strcmp(argv[2], "ntf") == 0) {
		mode = THROUGHPUT_MODE_NOTIFY;
	} else if (strcmp(argv[2], "WC") == 0 || strcmp(argv[2], "wc") == 0) {
		mode = THROUGHPUT_MODE_WRITE_CMD;
	} else if (strcmp(argv[2], "NTF_WC") == 0 || strcmp(argv[2], "ntf_wc") == 0) {
		mode = THROUGHPUT_MODE_NOTIFY_WRITE_CMD;
	} else if (strcmp(argv[2], "IND") == 0 || strcmp(argv[2], "ind") == 0) {
		mode = THROUGHPUT_MODE_INDICATE;
	} else if (strcmp(argv[2], "WR") == 0 || strcmp(argv[2], "wr") == 0) {
		mode = THROUGHPUT_MODE_WRITE_REQ;
	} else if (strcmp(argv[2], "IND_WR") == 0 || strcmp(argv[2], "ind_wr") == 0) {
		mode = THROUGHPUT_MODE_INDICATE_WRITE_REQ;
	} else {
		BT_LOGE("TP client %s mode is wrong!\r\n", argv[2]);
		return -1;
	}
	p_cfg_param->mode = mode;

	tx_end = ble_throughput_get_tx_end(mode);
	if (tx_end == THROUGHPUT_TX_END_SERVER) {
		p_cfg_param->length_c = 0;
	} else if (tx_end == THROUGHPUT_TX_END_CLIENT) {
		p_cfg_param->length_s = 0;
	}

	if (argc >= 6) {
		p_cfg_param->conn_interval = (uint16_t)str_to_int(argv[3]);
		phy = (throughput_phy_t)str_to_int(argv[4]);
		if (phy < THROUGHPUT_PHY_1M || phy > THROUGHPUT_PHY_CODED_S8) {
			BT_LOGE("TP client %d phy is wrong!\r\n", phy);
			return -1;
		}
		p_cfg_param->phy = phy;
		p_cfg_param->conn_peripheral_latency = (uint16_t)str_to_int(argv[5]);
	}

	if (tx_end == THROUGHPUT_TX_END_SERVER_CLIENT) {
		if (argc >= 9) {
			p_cfg_param->length_s = (uint16_t)str_to_int(argv[6]);
			p_cfg_param->length_c = (uint16_t)str_to_int(argv[7]);
			if (strcmp(argv[8], "MAX") == 0 || strcmp(argv[8], "max") == 0) {
				p_cfg_param->duration = THROUGHPUT_TIME_MAX;
			} else {
				p_cfg_param->duration = (uint32_t)str_to_int(argv[8]);
			}
		}
		if (argc >= 10) {
			p_cfg_param->period = (uint16_t)str_to_int(argv[9]);
		}
	} else {
		if (argc >= 8) {
			if (tx_end == THROUGHPUT_TX_END_SERVER) {
				p_cfg_param->length_s = (uint16_t)str_to_int(argv[6]);
			} else if (tx_end == THROUGHPUT_TX_END_CLIENT) {
				p_cfg_param->length_c = (uint16_t)str_to_int(argv[6]);
			}
			if (strcmp(argv[7], "MAX") == 0 || strcmp(argv[7], "max") == 0) {
				p_cfg_param->duration = THROUGHPUT_TIME_MAX;
			} else {
				p_cfg_param->duration = (uint32_t)str_to_int(argv[7]);
			}
		}
		if (argc >= 9) {
			p_cfg_param->period = (uint16_t)str_to_int(argv[8]);
		}
	}

	BT_LOGA("Remote BD_ADDR %s : mode = 0x%02x, conn_interval = 0x%x, phy = %d, "      \
			"latency = 0x%x, supervision_timeout = 0x%x, length_s = 0x%x, length_c = 0x%x, " \
			"duration = 0x%x(s), period = 0x%x(s)\r\n",
			addr_str,
			p_cfg_param->mode,
			p_cfg_param->conn_interval,
			p_cfg_param->phy,
			p_cfg_param->conn_peripheral_latency,
			p_cfg_param->conn_supervision_timeout,
			p_cfg_param->length_s,
			p_cfg_param->length_c,
			p_cfg_param->duration,
			p_cfg_param->period);

	if (p_cfg_param->duration == 0 || p_cfg_param->period == 0 || p_cfg_param->duration < p_cfg_param->period) {
		BT_LOGE("TP client duration or period arg is wrong!\r\n");
		return -1;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	rtk_bt_le_ext_create_conn_param_t ext_conn_param;
	memcpy(&ext_conn_param, &def_tp_ext_conn_param, sizeof(rtk_bt_le_ext_create_conn_param_t));
	ext_conn_param.peer_addr = addr;
	ret = rtk_bt_le_gap_ext_connect(&ext_conn_param);
#else
	rtk_bt_le_create_conn_param_t conn_param;
	memcpy(&conn_param, &def_tp_conn_param, sizeof(rtk_bt_le_create_conn_param_t));
	conn_param.peer_addr = addr;
	ret = rtk_bt_le_gap_connect(&conn_param);
#endif
	if (RTK_BT_OK != ret) {
		BT_LOGE("TP client connect op failed! err: 0x%x, remote BD_ADDR: %s\r\n", ret, addr_str);
		return -1;
	}

	return 0;
}

static int atcmd_ble_throughput_test_start(int argc, char **argv)
{
	throughput_test_cmd_t test_cmd;
	uint8_t i = 0;
	uint16_t conn_handle = 0;
	uint16_t ret = 0;
	uint16_t len = 1;
	uint8_t *p_data = NULL;

	test_cmd = THROUGHPUT_TEST_START;
	for (i = 0; i < argc; i++) {
		conn_handle = (uint16_t)str_to_int(argv[i]);
		p_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
		memcpy(p_data, (uint8_t *)(&test_cmd), len);
		ret = ble_throughput_client_send_test_cmd(conn_handle, p_data, len);
		if (RTK_BT_OK != ret) {
			BT_LOGE("TP client send test start cmd op failed! err: 0x%x, conn_handle: %d\r\n",
					ret, conn_handle);
			osif_mem_free(p_data);
			return -1;
		}
		osif_mem_free(p_data);
	}

	return 0;
}

static int atcmd_ble_throughput_test_stop(int argc, char **argv)
{
	throughput_test_cmd_t test_cmd;
	uint8_t i = 0;
	uint16_t conn_handle = 0;
	uint16_t ret = 0;
	uint16_t len = 1;
	uint8_t *p_data = NULL;

	test_cmd = THROUGHPUT_TEST_STOP;
	for (i = 0; i < argc; i++) {
		conn_handle = (uint16_t)str_to_int(argv[i]);
		p_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
		memcpy(p_data, (uint8_t *)(&test_cmd), len);
		ret = ble_throughput_client_send_test_cmd(conn_handle, p_data, len);
		if (RTK_BT_OK != ret) {
			BT_LOGE("TP client send test stop cmd op failed! err: 0x%x, conn_handle: %d\r\n",
					ret, conn_handle);
			osif_mem_free(p_data);
			return -1;
		}
		osif_mem_free(p_data);
	}

	return 0;
}

static const cmd_table_t throughput_test_cmd_table[] = {
	{"conn",       atcmd_ble_throughput_conn,       4, 11},
	{"test_start", atcmd_ble_throughput_test_start, 2, 12},
	{"test_stop",  atcmd_ble_throughput_test_stop,  2, 12},
	{NULL,},
};

int atcmd_bt_throughput_test(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, throughput_test_cmd_table, "[AT+BTDEMO=throughput]");
	return 0;
}