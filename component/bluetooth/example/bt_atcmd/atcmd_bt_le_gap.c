/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_api_config.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_common.h>

static uint8_t def_adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
};

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static uint8_t def_ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'P', 'E', 'R', 'I', 'P', 'H', 'E', 'R', 'A', 'L',
	// Manufacturer Specific Data
	0xd8,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0xd5, 0x00,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
	0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
	0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
	0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc,
	0xd, 0xd, 0xd, 0xd, 0xd,
};

static uint8_t def_ext_scan_data[] = {
	//GAP_ADTYPE_APPEARANCE
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE,
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	// Manufacturer Specific Data
	0xf6,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0x5d, 0x00,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
	0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
	0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
	0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc,
	0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd, 0xd,
	0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe,
	0xf, 0xf, 0xf,
};
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
static uint8_t def_pa_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x13,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'B', 'L', 'E', '_', 'B', 'T', '_', 'P', 'e', 'r', 'i', 'o', 'd', 'i', 'c', 'A', 'D', 'V',
	// Manufacturer Specific Data
	0xe4,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0xe1, 0x00,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50,
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0, 0xd0,
	0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
	0xf0,
};

static rtk_bt_le_pa_param_t def_pa_param = {
	0,
	64,
	80,
	RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER,
	(uint8_t *)def_pa_data,
	sizeof(def_pa_data),
#if defined(RTK_BLE_5_4_PA_RSP_SUPPORT) && RTK_BLE_5_4_PA_RSP_SUPPORT
	2,
	20,
	5,
	10,
	2
#endif
};
#endif

static uint8_t def_scan_rsp_data[] = {
	/* ad_len */
	0x3,
	/* ad_type */
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE,
	/* ad_data */
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_GENERIC_PHONE),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_GENERIC_PHONE),
};

static rtk_bt_le_adv_param_t def_adv_param = {
	.interval_min = 0x30,
	.interval_max = 0x60,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static rtk_bt_le_ext_adv_param_t def_ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = 320,
	.primary_adv_interval_max = 320,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},//;{0x8A, 0xAA, 0xAA, 0x4C, 0xE0, 0x00},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};
#endif

static rtk_bt_le_scan_param_t def_scan_param = {
	.type          = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval      = 0x60,
	.window        = 0x30,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 1,
};

#if defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT
static rtk_bt_le_ext_scan_param_t def_ext_scan_param = {
	.own_addr_type      = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.phys               = {true, true},
	.type               = {RTK_BT_LE_SCAN_TYPE_ACTIVE, RTK_BT_LE_SCAN_TYPE_ACTIVE},
	.interval           = {108, 108},
	.window             = {54, 54},

	.duration           = 0,
	.period             = 0,

	.filter_policy      = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt      = RTK_BT_LE_SCAN_DUPLICATE_ENABLE,
};
#endif

static rtk_bt_le_create_conn_param_t def_conn_param = {
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.scan_interval = 0x60,
	.scan_window = 0x30,
	.filter_policy = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.conn_interval_max = 0x60,
	.conn_interval_min = 0x60,
	.conn_latency      = 0,
	.supv_timeout      = 0x100,
	.scan_timeout      = 1000,
};

#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) || (defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT)
static rtk_bt_le_ext_create_conn_param_t def_ext_conn_param = {
	.filter_policy = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
		.addr_val = {0},
	},
	.init_phys = {true, true, true},
	.scan_interval = {0x60, 0x60, 0x60},
	.scan_window = {0x30, 0x30, 0x30},
	.conn_interval_min = {0x60, 0x60, 0x60},
	.conn_interval_max = {0x60, 0x60, 0x60},
	.conn_latency = {0, 0, 0},
	.supv_timeout = {0x100, 0x100, 0x100},
	.scan_timeout = 1000,
};
#endif

static int atcmd_ble_gap_get_version(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_version_info_t version = {0};

	ret = rtk_bt_le_gap_get_version(&version);
	if (ret) {
		BT_LOGE("GAP get version failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get version, hci_revision: 0x%x, lmp_subversion: 0x%x, "
			"btgap_revision: 0x%x, btgap_buildnum: 0x%x\r\n",
			version.hci_revision, version.lmp_subversion,
			version.btgap_revision, version.btgap_buildnum);
	BT_AT_PRINT("+BLEGAP:version,0x%x,0x%x,0x%x,0x%x\r\n",
				version.hci_revision, version.lmp_subversion,
				version.btgap_revision, version.btgap_buildnum);
	return 0;
}

static int atcmd_ble_gap_get_bd_addr(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_addr_t addr;
	char addr_str[30] = {0};

	ret = rtk_bt_le_gap_get_bd_addr(&addr);
	if (ret) {
		BT_LOGE("GAP get bd_addr failed! err: 0x%x\r\n", ret);
		return -1;
	}

	rtk_bt_le_addr_to_str(&addr, addr_str, sizeof(addr_str));
	BT_LOGA("GAP get bd_addr: %s\r\n", addr_str);
	BT_AT_PRINT("+BLEGAP:addr,%s\r\n", addr_str);
	return 0;
}

static int atcmd_ble_gap_set_dev_name(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;

	if (strlen(argv[0]) == 0 || strlen(argv[0]) > 39) {
		BT_LOGE("GAP set device name failed! too long!\r\n");
		return -1;
	}

	ret = rtk_bt_le_gap_set_device_name((const uint8_t *)argv[0]);
	if (ret) {
		BT_LOGE("GAP set device name failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set device name success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_appearance(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t appearance = 0;

	appearance = str_to_int(argv[0]);

	ret = rtk_bt_le_gap_set_appearance(appearance);
	if (ret) {
		BT_LOGE("GAP set device appearance failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set device appearance success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_preferred_conn_param(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_preferred_conn_param_t preferred_conn_param = {0};

	preferred_conn_param.conn_interval_min = str_to_int(argv[0]);
	preferred_conn_param.conn_interval_max = str_to_int(argv[1]);
	preferred_conn_param.slave_latency = str_to_int(argv[2]);
	preferred_conn_param.supv_timeout =  str_to_int(argv[3]);

	BT_LOGA("conn_interval_min=0x%x,conn_interval_max=0x%x,slave_latency=0x%x,supervision_timeout=0x%x\r\n",
			preferred_conn_param.conn_interval_min,
			preferred_conn_param.conn_interval_max,
			preferred_conn_param.slave_latency,
			preferred_conn_param.supv_timeout);

	ret = rtk_bt_le_gap_set_preferred_conn_param(&preferred_conn_param);
	if (ret) {
		BT_LOGE("GAP set preferred conn param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set preferred conn param success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_rand_addr(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_rand_addr_type_t type = RTK_BT_LE_RAND_ADDR_STATIC;
	uint8_t addr[RTK_BD_ADDR_LEN] = {0};
	bool auto_generate;
	char addr_str[20];

	if (argc == 1) {
		if (hexdata_str_to_bd_addr(argv[0], addr, RTK_BD_ADDR_LEN) == false) {
			return -1;
		}
		auto_generate = false;
	} else if (argc == 2) {
		type = str_to_int(argv[1]);
		auto_generate = true;
	}

	ret = rtk_bt_le_gap_set_rand_addr(auto_generate, type, addr);
	if (ret) {
		BT_LOGE("GAP set random address failed! err: 0x%x\r\n", ret);
		return -1;
	}

	rtk_bt_addr_val_to_str(addr, addr_str, sizeof(addr_str));
	BT_LOGA("GAP set random address success, auto_generate: %d, random_addr: %s\r\n", auto_generate, addr_str);

	return 0;
}


static int atcmd_ble_gap_set_channel_map(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t chan_map[RTK_BT_LE_CHAN_MAP_LEN] = {0};

	if (false == hexnum_str_to_array(argv[0], chan_map, 5) ||
		(ret = rtk_bt_le_gap_set_channels(chan_map)) != RTK_BT_OK) {
		BT_LOGE("GAP set host channel failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set channel map success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_adv_data(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t adv_data[RTK_BT_LE_MAX_ADV_DATA_LEN] = {0};
	uint16_t adv_len  = 0;

	if (argc == 0) {
		ret = rtk_bt_le_gap_set_adv_data(def_adv_data, sizeof(def_adv_data));
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP set default adv data failed! err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP set default adv data success\r\n");
		return 0;
	}

	adv_len = strlen(argv[0]) / 2;
	if (adv_len > 31) {
		BT_LOGE("GAP set adv data failed! too long!\r\n");
		return -1;
	}

	if (false == hexdata_str_to_array(argv[0], adv_data, adv_len) ||
		(ret = rtk_bt_le_gap_set_adv_data(adv_data, adv_len)) != RTK_BT_OK) {
		BT_LOGE("GAP set adv data failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set adv data success\r\n");
	return 0;
}

static int atcmd_ble_gap_op_adv(int argc, char **argv)
{
	rtk_bt_le_adv_param_t adv_param;
	uint16_t ret = 0;
	int en;

	en = str_to_int(argv[0]);
	if (0 == en) {
		ret = rtk_bt_le_gap_stop_adv();
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP stop adv failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP stopping adv ...\r\n");
			return 0;
		}
	} else if (1 == en) {
		memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
		if (argc >= 4) {
			adv_param.type = (rtk_bt_le_adv_type_t)str_to_int(argv[1]);
			adv_param.own_addr_type = (rtk_bt_le_addr_type_t)str_to_int(argv[2]);
			adv_param.filter_policy = (rtk_bt_le_adv_filter_t)str_to_int(argv[3]);
		}
		if (argc >= 6) {
			adv_param.peer_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[4]);
			if (false == hexdata_str_to_bd_addr(argv[5], (uint8_t *)adv_param.peer_addr.addr_val, RTK_BD_ADDR_LEN)) {
				return -1;
			}
		}
		if (argc >= 9) {
			adv_param.interval_min = str_to_int(argv[6]);
			adv_param.interval_max = str_to_int(argv[7]);
			adv_param.channel_map = (rtk_bt_le_adv_ch_map_t)str_to_int(argv[8]);
		}
		ret = rtk_bt_le_gap_start_adv(&adv_param);
		if (ret != RTK_BT_OK) {
			BT_LOGE("GAP start adv failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			/* here, we use [starting] and with [...] means it is on processing(async operation),
			and the result still need to be get from the callback */
			BT_LOGA("GAP starting adv ...\r\n");
		}
	}

	return 0;
}

static int atcmd_ble_gap_get_adv_param(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_adv_param_t adv_param = {0};
	char addr_str[13] = {0};
	uint8_t *addr_val = NULL;

	ret = rtk_bt_le_gap_get_adv_param(&adv_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP get adv param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	addr_val = adv_param.peer_addr.addr_val;
	snprintf(addr_str, sizeof(addr_str), "%02x%02x%02x%02x%02x%02x",
			 addr_val[5], addr_val[4], addr_val[3],
			 addr_val[2], addr_val[1], addr_val[0]);
	BT_LOGA("GAP get adv param success, param: %d,%d,%d,%d,%s,%d,%d,%d\r\n",
			adv_param.type, adv_param.own_addr_type, adv_param.filter_policy,
			adv_param.peer_addr.type, addr_str, adv_param.interval_min,
			adv_param.interval_max, adv_param.channel_map);
	BT_AT_PRINT("+BLEGAP:get_adv_param,%d,%d,%d,%d,%s,%d,%d,%d\r\n",
				adv_param.type, adv_param.own_addr_type, adv_param.filter_policy,
				adv_param.peer_addr.type, addr_str, adv_param.interval_min,
				adv_param.interval_max, adv_param.channel_map);
	return 0;
}

static int atcmd_ble_gap_set_scan_resp(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint32_t scan_resp_len = 0;
	uint8_t scan_resp_data[32] = {0};

	if (argc == 0) {
		ret = rtk_bt_le_gap_set_scan_rsp_data(def_scan_rsp_data, sizeof(def_scan_rsp_data));
		if (ret) {
			BT_LOGE("GAP set default scan resp data failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP set def scan resp data success\r\n");
			return 0;
		}
	}

	scan_resp_len = strlen(argv[0]) / 2;
	if (scan_resp_len > 31) {
		BT_LOGE("GAP set scan resp data failed! too long!\r\n");
		return -1;
	}

	if (false == hexdata_str_to_array(argv[0], scan_resp_data, scan_resp_len) ||
		(ret = rtk_bt_le_gap_set_scan_rsp_data(scan_resp_data, scan_resp_len)) != RTK_BT_OK) {
		BT_LOGE("GAP set scan resp data failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set scan resp data success\r\n");
	return 0;
}

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static int atcmd_ble_gap_set_ext_adv_data(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t adv_handle = 0;
	uint8_t *pdata = (uint8_t *)def_ext_adv_data;
	uint8_t len = sizeof(def_ext_adv_data);

	if (argc > 0) {
		adv_handle = str_to_int(argv[0]);
	}

	if (argc == 2) {
		len = str_to_int(argv[1]);
		if (len > sizeof(def_ext_adv_data)) {
			BT_LOGE("GAP set ext adv data failed! Length too long!\r\n");
			return -1;
		}
	}

	ret = rtk_bt_le_gap_set_ext_adv_data(adv_handle, pdata, len);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP set ext adv(%d) data failed! err: 0x%x\r\n", adv_handle, ret);
		return -1;
	}

	BT_LOGA("GAP set ext adv(%d) data success\r\n", adv_handle);
	return 0;
}

static int atcmd_ble_gap_op_ext_adv(int argc, char **argv)
{
	rtk_bt_le_ext_adv_param_t adv_param;
	uint16_t ret = 0;
	uint8_t operation;
	uint8_t adv_handle = 0;
	uint16_t duration = 0;
	uint8_t num_vents = 0;

	operation = str_to_int(argv[0]);
	if (0 == operation) { //stop ext adv
		if (argc > 1) {
			adv_handle = str_to_int(argv[1]);
		}
		ret = rtk_bt_le_gap_stop_ext_adv(adv_handle);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP stop ext adv(%d) failed! err: 0x%x\r\n", adv_handle, ret);
			return -1;
		} else {
			BT_LOGA("GAP stopping ext adv(%d) ...\r\n", adv_handle);
		}
	} else if (1 == operation) { //start ext adv
		if (argc > 1) {
			adv_handle = str_to_int(argv[1]);
		}
		if (argc > 2) {
			duration = str_to_int(argv[2]);
		}
		if (argc > 3) {
			num_vents = str_to_int(argv[3]);
		}
		ret = rtk_bt_le_gap_start_ext_adv(adv_handle, duration, num_vents);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP start ext adv(%d) failed! err: 0x%x\r\n", adv_handle, ret);
			return -1;
		} else {
			BT_LOGA("GAP starting ext adv(%d) ...\r\n", adv_handle);
		}
	} else if (2 == operation) { //create ext adv handle
		memcpy(&adv_param, &def_ext_adv_param, sizeof(rtk_bt_le_ext_adv_param_t));

		if (argc > 1) {
			adv_param.adv_sid = str_to_int(argv[1]);
		}
		if (argc > 2) {
			adv_param.adv_event_prop = (rtk_bt_le_adv_event_prop_t)str_to_int(argv[2]);
		}
		if (argc > 3) {
			uint8_t phy = str_to_int(argv[3]);
			adv_param.primary_adv_phy = (rtk_bt_le_phy_prim_adv_t)((phy >> 4) & 0xF);
			adv_param.secondary_adv_phy = (rtk_bt_le_phy_type_t)(phy & 0xF);
		}
		if (argc > 4) {
			adv_param.filter_policy = (rtk_bt_le_adv_filter_t)str_to_int(argv[4]);
		}

		if (argc > 5) {
			adv_param.tx_power = (int8_t)str_to_int(argv[5]);
			BT_LOGA("adv_param.tx_power = %d\r\n", adv_param.tx_power);
		}
		if (argc > 6) {
			adv_param.primary_adv_interval_min = str_to_int(argv[6]);
			adv_param.primary_adv_interval_max = adv_param.primary_adv_interval_min + 10;
		}
		if (argc > 7) {
			adv_param.own_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[7]);
			if (adv_param.own_addr.type == RTK_BT_LE_ADDR_TYPE_RANDOM) {
				if (false == hexdata_str_to_bd_addr(argv[8], (uint8_t *)adv_param.own_addr.addr_val, RTK_BD_ADDR_LEN)) {
					return -1;
				}
			}
		}
		if (argc > 9) {
			adv_param.peer_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[9]);
			if (false == hexdata_str_to_bd_addr(argv[10], (uint8_t *)adv_param.peer_addr.addr_val, RTK_BD_ADDR_LEN)) {
				return -1;
			}
		}

		ret = rtk_bt_le_gap_create_ext_adv(&adv_param, &adv_handle);

		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP create ext adv failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP create ext adv success! adv handle: %d\r\n", adv_handle);
		}
	} else if (3 == operation) { //delete ext adv handle
		if (argc > 1) {
			adv_handle = str_to_int(argv[1]);
		}
		ret = rtk_bt_le_gap_remove_ext_adv(adv_handle);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP remove ext adv(%d) failed! err: 0x%x\r\n", adv_handle, ret);
			return -1;
		} else {
			BT_LOGA("GAP remove ext adv(%d) success!\r\n", adv_handle);
		}
	} else {
		BT_LOGE("GAP ext adv operation failed: wrong operation value: %d!\r\n", operation);
		return -1;
	}

	return 0;
}

static int atcmd_ble_gap_set_ext_scan_resp(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t adv_handle = 0;
	uint8_t *pdata = (uint8_t *)def_ext_scan_data;
	uint8_t len = sizeof(def_ext_scan_data);

	if (argc > 0) {
		adv_handle = str_to_int(argv[0]);
	}

	if (argc == 2) {
		len = str_to_int(argv[1]);
		if (len > sizeof(def_ext_scan_data)) {
			BT_LOGE("GAP set ext scan resp data failed! Length too long!\r\n");
			return -1;
		}
	}

	ret = rtk_bt_le_gap_set_ext_scan_rsp_data(adv_handle, pdata, len);
	if (ret) {
		BT_LOGE("GAP set ext scan resp data(%d) failed! err: 0x%x\r\n", adv_handle, ret);
		return -1;
	}

	BT_LOGA("GAP set ext scan resp data(%d) success\r\n", adv_handle);
	return 0;
}
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
static int atcmd_ble_gap_pa_adv(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t operation, adv_handle = 0;
	rtk_bt_le_pa_param_t param;

	memcpy(&param, &def_pa_param, sizeof(rtk_bt_le_pa_param_t));
	operation = str_to_int(argv[0]);

	if (operation == 1) { //enable pa
		if (argc > 1) {
			param.adv_handle = str_to_int(argv[1]);
		}

		if (argc > 2) {
			param.pa_interval_min = str_to_int(argv[2]);
			param.pa_interval_max = param.pa_interval_min + 10;
		}

		if (argc > 3) {
			param.pa_prop = str_to_int(argv[3]) ? RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER : 0;
		}

		if (argc > 4) {
			param.pa_len = str_to_int(argv[4]);
		}

		ret = rtk_bt_le_gap_start_pa(&param);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP start periodic adv(%d) failed! err: 0x%x\r\n", param.adv_handle, ret);
			return -1;
		}
		BT_LOGA("GAP starting periodic adv(%d) ...\r\n", param.adv_handle);

		return 0;
	} else if (operation == 0) { //stop pa
		if (argc > 1) {
			adv_handle = str_to_int(argv[1]);
		}

		ret = rtk_bt_le_gap_stop_pa(adv_handle);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP stop periodic adv(%d) failed! err: 0x%x\r\n", adv_handle, ret);
			return -1;
		}
		BT_LOGA("GAP stopping periodic adv(%d) ...\r\n", adv_handle);

		return 0;
	} else {
		BT_LOGE("GAP periodic adv operation failed: wrong operation value: %d!\r\n", operation);
		return -1;
	}
}

static int atcmd_ble_gap_pa_adv_update(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t adv_handle = 0;
	bool update_DID_only = true;
	uint8_t *data = NULL;
	uint16_t len = 0;

	adv_handle = str_to_int(argv[0]);

	if (argc != 1 && argc != 3) {
		BT_LOGE("GAP set scan paramters failed! wrong args num!\r\n");
		return -1;
	}

	if (argc == 3) {
		update_DID_only = false;
		data = (uint8_t *)argv[1];
		len = str_to_int(argv[2]);
	}

	ret = rtk_bt_le_gap_update_pa(adv_handle, update_DID_only, data, len);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP Update periodic adv(%d) failed! err: 0x%x\r\n", adv_handle, ret);
		return -1;
	}

	BT_LOGA("GAP update periodic adv(%d) success\r\n", adv_handle);
	return 0;
}
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
static int atcmd_ble_gap_pa_sync_modify_list(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t op;
	rtk_bt_le_addr_t addr;
	uint8_t adv_sid;

	op = str_to_int(argv[0]);

	if (op != RTK_BT_LE_PA_SYNC_ADV_LIST_OP_CLEAR) {
		if (argc != 4) {
			BT_LOGE("Wrong args num!\r\n");
			return -1;
		}

		adv_sid = str_to_int(argv[1]);
		addr.type = str_to_int(argv[2]);
		if (false == hexdata_str_to_bd_addr(argv[3], addr.addr_val, RTK_BD_ADDR_LEN)) {
			return -1;
		}
	}

	ret = rtk_bt_le_gap_pa_sync_modify_adv_list(op, addr, adv_sid);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP modify PA sync adv list failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("GAP modify PA sync adv list Success!\r\n");

	return 0;
}

static int atcmd_ble_gap_pa_sync_op(int argc, char **argv)
{
	uint16_t ret = 0;
	uint8_t op;
	uint8_t sync_id;

	op = str_to_int(argv[0]);
	if (op == 1) { //create sync
		rtk_bt_le_pa_sync_create_t create = {
			.options = RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_REPORT_INITIALLY_DISABLED,
			.sync_cte_type = 0,
			.adv_sid = 0,
			.adv_addr = {0, {0, 0, 0, 0, 0, 0}},
			.skip = 0,
			.sync_timeout = 0x1000,
			.p_sync_id = &sync_id,
		};

		if (argc > 1) {
			create.options = str_to_int(argv[1]);
		}

		if (argc > 2) {
			create.sync_cte_type = str_to_int(argv[2]);
		}

		if (!(create.options & RTK_BT_LE_PA_SYNC_CREATE_OPTIONS_USE_PERIODIC_ADV_LIST)) {
			if (argc > 3) {
				create.adv_sid = str_to_int(argv[3]);
			}

			if (argc > 5) {
				create.adv_addr.type = str_to_int(argv[4]);
				if (false == hexdata_str_to_bd_addr(argv[5], create.adv_addr.addr_val, RTK_BD_ADDR_LEN)) {
					return -1;
				}
			}
		}

		if (argc > 6) {
			create.skip = str_to_int(argv[6]);
		}

		if (argc > 7) {
			create.sync_timeout = str_to_int(argv[7]);
		}

		ret = rtk_bt_le_gap_pa_sync_create(&create);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP PA sync create fail! err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP PA sync create success, sync_id = %d!\r\n", sync_id);
	} else if (op == 0) {
		if (argc != 2) {
			BT_LOGE("Wrong args num!\r\n");
			return -1;
		}
		sync_id = str_to_int(argv[1]);
		ret = rtk_bt_le_gap_pa_sync_terminate(sync_id);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP PA sync terminate fail! err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP PA sync terminate success, sync_id = %d!\r\n", sync_id);
	}

	return 0;
}
#endif

#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
static int atcmd_ble_gap_past_send(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle;
	uint8_t service_data, use_sync_id, idx;

	conn_handle = str_to_int(argv[0]);
	service_data = str_to_int(argv[1]);
	use_sync_id = str_to_int(argv[2]);
	idx = str_to_int(argv[3]);

	ret = rtk_bt_le_gap_past_send(conn_handle, service_data, use_sync_id ? true : false, idx);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP PAST transfer fail! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("GAP PAST transfer success!\r\n");

	return 0;
}
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
static int atcmd_ble_gap_past_recv(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_past_recv_param_t param = {
		.conn_handle = 0,
		.mode = RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER,
		.sync_cte_type = 0,
		.skip = 0,
		.sync_timeout = 0x1000,
	};

	if (argc > 0) {
		param.conn_handle = str_to_int(argv[0]);
	}

	if (argc > 1) {
		param.mode = str_to_int(argv[1]);
	}

	if (argc > 2) {
		param.sync_cte_type = str_to_int(argv[2]);
	}

	if (argc > 3) {
		param.skip = str_to_int(argv[3]);
	}

	if (argc > 4) {
		param.sync_timeout = str_to_int(argv[4]);
	}

	ret = rtk_bt_le_gap_past_recipient_set(&param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP PAST recipient set params fail! conn_handle: %d, err: 0x%x\r\n", param.conn_handle, ret);
		return -1;
	}
	BT_LOGA("GAP PAST recipient set params success! conn_handle: %d\r\n", param.conn_handle);

	return 0;
}

static int atcmd_ble_gap_default_past_recv(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_past_recv_param_t param = {
		.conn_handle = 0,
		.mode = RTK_BT_LE_PAST_RECV_MODE_PA_ADV_REPORT_ENABLED_WITH_DUPLICATE_FILTER,
		.sync_cte_type = 0,
		.skip = 0,
		.sync_timeout = 0x1000,
	};

	if (argc > 0) {
		param.mode = str_to_int(argv[0]);
	}

	if (argc > 1) {
		param.sync_cte_type = str_to_int(argv[1]);
	}

	if (argc > 2) {
		param.skip = str_to_int(argv[2]);
	}

	if (argc > 3) {
		param.sync_timeout = str_to_int(argv[3]);
	}

	ret = rtk_bt_le_gap_default_past_recipient_set(&param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP PAST recipient set default params fail! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("GAP PAST recipient set default params success!\r\n");

	return 0;
}
#endif

static int atcmd_ble_gap_set_scan_param(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_scan_param_t scan_param;

	if (argc != 0 && argc != 4 && argc != 6) {
		BT_LOGE("GAP set scan paramters failed! wrong args num!\r\n");
		return -1;
	}

	memcpy(&scan_param, &def_scan_param, sizeof(rtk_bt_le_scan_param_t));
	if (argc == 0) {
		; /* do nothing, just use default def_scan_param */
	}
	if (argc >= 4) {
		scan_param.type = (rtk_bt_le_scan_type_t)str_to_int(argv[0]);
		scan_param.own_addr_type = (rtk_bt_le_addr_type_t)str_to_int(argv[1]);
		scan_param.filter_policy = (rtk_bt_le_scan_filter_t)str_to_int(argv[2]);
		scan_param.duplicate_opt = (uint8_t)str_to_int(argv[3]);
	}
	if (argc >= 6) {
		scan_param.interval = str_to_int(argv[4]);
		scan_param.window = str_to_int(argv[5]);
	}
	ret = rtk_bt_le_gap_set_scan_param(&scan_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP set scan param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set scan param success\r\n");
	return 0;
}

static int atcmd_ble_gap_get_scan_param(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_scan_param_t scan_param = {0};

	ret = rtk_bt_le_gap_get_scan_param(&scan_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP get scan param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get scan param success, param: %d,%d,%d,%d,%d,%d\r\n",
			scan_param.type, scan_param.own_addr_type, scan_param.filter_policy,
			scan_param.duplicate_opt, scan_param.interval, scan_param.window);
	BT_AT_PRINT("+BLEGAP:get_scan_param,%d,%d,%d,%d,%d,%d\r\n",
				scan_param.type, scan_param.own_addr_type, scan_param.filter_policy,
				scan_param.duplicate_opt, scan_param.interval, scan_param.window);
	return 0;
}

static int atcmd_ble_gap_op_scan(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;

	if (1 == str_to_int(argv[0])) {
		ret = rtk_bt_le_gap_start_scan();
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP start scan failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP starting scan ...\r\n");
		}
	} else if (0 == str_to_int(argv[0])) {
		ret = rtk_bt_le_gap_stop_scan();
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP stop scan failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP stopping scan ...\r\n");
		}
	} else {
		BT_LOGE("GAP scan op failed! wrong args!\r\n");
		return -1;
	}

	return 0;
}

#if defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT
static int atcmd_ble_gap_ext_scan_set_param(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_ext_scan_param_t scan_param;
	uint8_t scan_phys;

	if (argc != 0 && argc != 3 && argc != 6 && argc != 9 && argc != 12) {
		BT_LOGE("GAP set scan paramters failed! wrong args num!\r\n");
		return -1;
	}

	if (argc == 0) {
		ret = rtk_bt_le_gap_set_ext_scan_param(&def_ext_scan_param);
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP set default ext scan paramters failed! err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP set default ext scan paramters success\r\n");
		return 0;
	}

	memcpy(&scan_param, &def_ext_scan_param, sizeof(rtk_bt_le_ext_scan_param_t));

	scan_param.own_addr_type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
	scan_param.filter_policy = (rtk_bt_le_scan_filter_t)str_to_int(argv[1]);
	scan_param.duplicate_opt = (rtk_bt_le_scan_duplicate_t)str_to_int(argv[2]);

	if (argc > 3) {
		scan_param.duration = (uint16_t)str_to_int(argv[3]);
		scan_param.period = str_to_int(argv[4]);
		scan_phys = str_to_int(argv[5]);
		scan_param.phys[0] = (scan_phys & 1 << 0) ? true : false;
		scan_param.phys[1] = (scan_phys & 1 << 2) ? true : false;
	}

	if (argc > 6 && scan_param.phys[0]) {
		scan_param.type[0] = (rtk_bt_le_scan_type_t)str_to_int(argv[6]);
		scan_param.interval[0] = str_to_int(argv[7]);
		scan_param.window[0] = str_to_int(argv[8]);
	}

	if (argc > 9 && scan_param.phys[1]) {
		scan_param.type[1] = (rtk_bt_le_scan_type_t)str_to_int(argv[9]);
		scan_param.interval[1] = str_to_int(argv[10]);
		scan_param.window[1] = str_to_int(argv[11]);
	}
	ret = rtk_bt_le_gap_set_ext_scan_param(&scan_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP set scan param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set scan param success\r\n");
	return 0;
}

static int atcmd_ble_gap_op_ext_scan(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;

	if (1 == str_to_int(argv[0])) {
		ret = rtk_bt_le_gap_start_ext_scan();
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP start ext scan failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP starting ext scan ...\r\n");
		}
	} else if (0 == str_to_int(argv[0])) {
		ret = rtk_bt_le_gap_stop_ext_scan();
		if (RTK_BT_OK != ret) {
			BT_LOGE("GAP stop ext scan failed! err: 0x%x\r\n", ret);
			return -1;
		} else {
			BT_LOGA("GAP stopping ext scan ...\r\n");
		}
	} else {
		BT_LOGE("GAP ext scan op failed! wrong args!\r\n");
		return -1;
	}

	return 0;
}
#endif

static int atcmd_ble_gap_connect(int argc, char **argv)
{
	char addr_str[30] = {0};
	uint16_t ret = 0;
	rtk_bt_le_create_conn_param_t conn_param;

	memcpy(&conn_param, &def_conn_param, sizeof(rtk_bt_le_create_conn_param_t));
	if (argc >= 2) {
		conn_param.peer_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
		if (false == hexdata_str_to_bd_addr(argv[1], conn_param.peer_addr.addr_val, RTK_BD_ADDR_LEN)) {
			return -1;
		}
	}
	if (argc >= 4) {
		conn_param.own_addr_type = (rtk_bt_le_addr_type_t)str_to_int(argv[2]);
		conn_param.filter_policy = (rtk_bt_le_conn_filter_t)str_to_int(argv[3]);
	}
	if (argc >= 10) {
		conn_param.scan_interval = str_to_int(argv[4]);
		conn_param.scan_window = str_to_int(argv[5]);
		conn_param.conn_interval_min = str_to_int(argv[6]);
		conn_param.conn_interval_max = str_to_int(argv[7]);
		conn_param.conn_latency = str_to_int(argv[8]);
		conn_param.supv_timeout = str_to_int(argv[9]);
	}
	ret = rtk_bt_le_gap_connect(&conn_param);
	if (ret != RTK_BT_OK) {
		BT_LOGE("GAP connect ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	rtk_bt_le_addr_to_str(&conn_param.peer_addr, addr_str, sizeof(addr_str));
	BT_LOGA("GAP connecting to device %s ...\r\n", addr_str);
	return 0;
}

static int atcmd_ble_gap_connect_cancel(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_addr_t addr;

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);

	if (false == hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN) ||
		(ret = rtk_bt_le_gap_connect_cancel(&addr)) != RTK_BT_OK) {
		BT_LOGE("GAP create connection cancel failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP create connection cancel success\r\n");
	return 0;
}

#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) || (defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT)
static int atcmd_ble_gap_ext_connect(int argc, char **argv)
{
	char addr_str[30] = {0};
	rtk_bt_le_ext_create_conn_param_t ext_conn_param;
	int i = 0;
	uint8_t phys_bit_mask = 0;
	uint16_t ret = 0;

	memcpy(&ext_conn_param, &def_ext_conn_param,
		   sizeof(rtk_bt_le_ext_create_conn_param_t));

	if (argc >= 3) {
		ext_conn_param.peer_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
		if (false == hexdata_str_to_bd_addr(argv[1], ext_conn_param.peer_addr.addr_val, RTK_BD_ADDR_LEN)) {
			return -1;
		}
		phys_bit_mask = (uint8_t)str_to_int(argv[2]);
		ext_conn_param.init_phys[0] = (phys_bit_mask & 1 << 0) ? true : false;
		ext_conn_param.init_phys[1] = (phys_bit_mask & 1 << 1) ? true : false;
		ext_conn_param.init_phys[2] = (phys_bit_mask & 1 << 2) ? true : false;
		;
	}
	if (argc >= 5) {
		ext_conn_param.own_addr_type = (rtk_bt_le_addr_type_t)str_to_int(argv[3]);
		ext_conn_param.filter_policy = (rtk_bt_le_conn_filter_t)str_to_int(argv[4]);
	}
	if (argc >= 11) {
		for (i = 0; i < 3; i++) {
			if (0 == ext_conn_param.init_phys[i]) {
				continue;
			}
			ext_conn_param.scan_interval[i] = str_to_int(argv[5]);
			ext_conn_param.scan_window[i] = str_to_int(argv[6]);
			ext_conn_param.conn_interval_min[i] = str_to_int(argv[7]);
			ext_conn_param.conn_interval_max[i] = str_to_int(argv[8]);
			ext_conn_param.conn_latency[i] = str_to_int(argv[9]);
			ext_conn_param.supv_timeout[i] = str_to_int(argv[10]);
		}
	}
	/* atcmd has limitation with argc num, so we have no choice but let
	    all phys use a same set of params */
#if 0
	uint8_t init_phy_num = 0;
	int j = 0;
	else if (9 == argc || 15 == argc || 21 == argc) {
		init_phy_num = ((ext_conn_param.init_phys[0] == true) ? 1 : 0) +
					   ((ext_conn_param.init_phys[1] == true) ? 1 : 0) +
					   ((ext_conn_param.init_phys[2] == true) ? 1 : 0);
		if (argc != (init_phy_num) * 6 + 3) {
			BT_LOGE("GAP ext connection failed! wrong args num: %d, %d init phys "
					"need %d args!\r\n", argc, init_phy_num, (init_phy_num) * 6 + 3);
			return -1;
		}

		j = 3;
		for (i = 0; i < 3; i++) {
			if (0 == ext_conn_param.init_phys[i]) {
				continue;
			}
			ext_conn_param.scan_interval[i] = str_to_int(argv[j++]);
			ext_conn_param.scan_window[i] = str_to_int(argv[j++]);
			ext_conn_param.conn_interval_min[i] = str_to_int(argv[j++]);
			ext_conn_param.conn_interval_max[i] = str_to_int(argv[j++]);
			ext_conn_param.conn_latency[i] = str_to_int(argv[j++]);
			ext_conn_param.sup_timeout[i] = str_to_int(argv[j++]);
		}
	}
#endif

	if ((ret = rtk_bt_le_gap_ext_connect(&ext_conn_param)) != RTK_BT_OK) {
		BT_LOGE("GAP ext connect failed! err: 0x%x\r\n", ret);
		return -1;
	}
	rtk_bt_le_addr_to_str(&ext_conn_param.peer_addr, addr_str, sizeof(addr_str));
	BT_LOGA("GAP extended connecting to device %s ...\r\n", addr_str);
	return 0;
}
#endif

static int atcmd_ble_gap_get_conn_rssi(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle = 0;
	int8_t rssi_val = 0;

	conn_handle = str_to_int(argv[0]);
	if ((ret = rtk_bt_le_gap_read_rssi(conn_handle, &rssi_val)) != RTK_BT_OK) {
		BT_LOGE("GAP read rssi failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP read rssi, conn_hanlde: %d, value: %d\r\n", conn_handle, rssi_val);
	BT_AT_PRINT("+BLEGAP:conn_rssi,%d,%d\r\n", conn_handle, rssi_val);
	return 0;
}

static int atcmd_ble_gap_get_dev_state(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_gap_dev_state_t dev_state = {0};

	ret = rtk_bt_le_gap_get_dev_state(&dev_state);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP get dev state failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP dev state, init_state: %d, adv_state: %d, scan_state: %d, conn_state: %d\r\n",
			dev_state.gap_init_state, dev_state.gap_adv_state,
			dev_state.gap_scan_state, dev_state.gap_conn_state);
	BT_AT_PRINT("+BLEGAP:dev_state,%d,%d,%d,%d\r\n",
				dev_state.gap_init_state, dev_state.gap_adv_state,
				dev_state.gap_scan_state, dev_state.gap_conn_state);
	return 0;
}

static int atcmd_ble_gap_get_active_conn(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	uint16_t i = 0;
	rtk_bt_le_get_active_conn_t active_conn = {0};

	ret = rtk_bt_le_gap_get_active_conn(&active_conn);
	if (ret) {
		BT_LOGE("GAP get active conn num failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get active conn, num: %d, conn_handle:\r\n", active_conn.conn_num);
	BT_AT_PRINT("+BLEGAP:get_active_conn");
	for (i = 0; i < active_conn.conn_num; i++) {
		BT_LOGA(" %d,", active_conn.conn_handle[i]);
		BT_AT_PRINT(",%d", active_conn.conn_handle[i]);
	}
	BT_LOGA("\r\n");
	BT_AT_PRINT("\r\n");
	return 0;
}

static int atcmd_ble_gap_get_conn_handle_by_addr(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle = 0xFF;
	rtk_bt_le_addr_t addr = {0};

	addr.type = str_to_int(argv[0]);

	if (false == hexdata_str_to_bd_addr(argv[1], addr.addr_val, RTK_BD_ADDR_LEN) ||
		(ret = rtk_bt_le_gap_get_conn_handle_by_addr(&addr, &conn_handle)) != RTK_BT_OK) {
		BT_LOGE("GAP get conn handle by addr failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get conn handle by addr: %d\r\n", conn_handle);
	BT_AT_PRINT("+BLEGAP:get_conn_handle,%d\r\n", conn_handle);
	return 0;
}

static int atcmd_ble_gap_get_conn_info(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle;
	char bd_addr_str[30] = {0};
	rtk_bt_le_conn_info_t conn_info;

	conn_handle = str_to_int(argv[0]);
	if ((ret = rtk_bt_le_gap_get_conn_info(conn_handle, &conn_info)) != RTK_BT_OK) {
		BT_LOGE("GAP get connection info failed! err: 0x%x\r\n", ret);
		return -1;
	}

	rtk_bt_le_addr_to_str(&conn_info.remote, bd_addr_str, sizeof(bd_addr_str));
	BT_LOGA("GAP get connection info:\r\n"
			"conn_handle: %d, local role: %s, peer bd_addr: (%s),\r\n"
			"interval: 0x%x, latency: 0x%x, supv_timeout: 0x%x, tx_phy: 0x%x, rx_phy: 0x%x\r\n",
			conn_handle, (conn_info.role == RTK_BT_LE_ROLE_MASTER) ?
			"master" : ((conn_info.role == RTK_BT_LE_ROLE_SLAVE) ? "slave" : "unknown"),
			bd_addr_str, conn_info.interval, conn_info.latency,
			conn_info.supv_timeout, conn_info.tx_phy, conn_info.rx_phy);
	BT_AT_PRINT("+BLEGAP:conn_info,%d,%s,%s,0x%x,0x%x,0x%x,0x%x,0x%x\r\n",
				conn_handle, (conn_info.role == RTK_BT_LE_ROLE_MASTER) ?
				"master" : ((conn_info.role == RTK_BT_LE_ROLE_SLAVE) ? "slave" : "unknown"),
				bd_addr_str, conn_info.interval, conn_info.latency,
				conn_info.supv_timeout, conn_info.tx_phy, conn_info.rx_phy);
	return 0;
}

static int atcmd_ble_gap_get_mtu_size(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle;
	uint16_t mtu_size = 0;

	conn_handle = str_to_int(argv[0]);
	if ((ret = rtk_bt_le_gap_get_mtu_size(conn_handle, &mtu_size)) != RTK_BT_OK) {
		BT_LOGE("GAP get mtu size failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get mtu size, conn_handle: %d, mtu_size: %d\r\n", conn_handle, mtu_size);
	BT_AT_PRINT("+BLEGAP:mtu_size,%d,%d\r\n", conn_handle, mtu_size);
	return 0;
}

static int atcmd_ble_gap_set_max_mtu_size(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t mtu_size = 0;

	mtu_size = str_to_int(argv[0]);
	ret = rtk_bt_le_gap_set_max_mtu_size(mtu_size);
	if (ret) {
		BT_LOGE("GAP set max MTU size failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set max MTU size success\r\n");
	return 0;
}

static int atcmd_ble_gap_update_conn(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_update_conn_param_t up_conn_param;

	up_conn_param.conn_handle = str_to_int(argv[0]);
	up_conn_param.conn_interval_min = str_to_int(argv[1]);
	up_conn_param.conn_interval_max = str_to_int(argv[2]);
	up_conn_param.conn_latency = str_to_int(argv[3]);
	up_conn_param.supv_timeout = str_to_int(argv[4]);

	ret = rtk_bt_le_gap_update_conn_param(&up_conn_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP update connect parameters ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP updating connection parameters ...\r\n");
	return 0;
}

#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
static int atcmd_ble_gap_set_conn_data_len(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_set_datalen_param_t data_len_param;

	data_len_param.conn_handle = str_to_int(argv[0]);
	data_len_param.max_tx_octets = str_to_int(argv[1]);
	data_len_param.max_tx_time = str_to_int(argv[2]);
	ret = rtk_bt_le_gap_set_data_len(&data_len_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP set connection data len ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP setting connection data len ...\r\n");
	return 0;
}
#endif

#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
static int atcmd_ble_gap_set_conn_phy(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_set_phy_param_t conn_phy_param = {0};

	conn_phy_param.conn_handle = str_to_int(argv[0]);
	conn_phy_param.all_phys = str_to_int(argv[1]);
	conn_phy_param.tx_phys = str_to_int(argv[2]);
	conn_phy_param.rx_phys = str_to_int(argv[3]);
	conn_phy_param.phy_options = str_to_int(argv[4]);

	ret = rtk_bt_le_gap_set_phy(&conn_phy_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("GAP set connection PHY ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP setting connection PHY ...\r\n");
	return 0;
}
#endif

static int atcmd_ble_gap_disconnect(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle;

	conn_handle = str_to_int(argv[0]);
	if ((ret = rtk_bt_le_gap_disconnect(conn_handle)) != 0) {
		BT_LOGE("GAP disconnect ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP disconnecting ...\r\n");
	return 0;
}

static int atcmd_ble_gap_disconnect_with_reason(int argc, char **argv)
{
	(void)argc;
	uint8_t reason;
	uint16_t conn_handle, ret = 0;

	conn_handle = str_to_int(argv[0]);
	reason = str_to_int(argv[1]);
	if ((ret = rtk_bt_le_gap_disconnect_with_reason(conn_handle, reason)) != 0) {
		BT_LOGE("GAP disconnect with reason ops failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP disconnecting with reason ...\r\n");
	return 0;
}

static int atcmd_ble_gap_add_whitelist(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_modify_wl_param_t wl_op_param;

	wl_op_param.op = RTK_BT_LE_WHITELIST_ADD;
	wl_op_param.addr.type = str_to_int(argv[0]);

	if (false == hexdata_str_to_bd_addr(argv[1], (uint8_t *)wl_op_param.addr.addr_val, RTK_BD_ADDR_LEN) ||
		(ret = rtk_bt_le_gap_modify_whitelist(&wl_op_param)) != RTK_BT_OK) {
		BT_LOGE("GAP whitelist add failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP whitelist add success\r\n");
	return 0;
}

static int atcmd_ble_gap_remove_whitelist(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_modify_wl_param_t wl_op_param;

	wl_op_param.op = RTK_BT_LE_WHITELIST_REMOVE;
	wl_op_param.addr.type = str_to_int(argv[0]);

	if (false == hexdata_str_to_bd_addr(argv[1], (uint8_t *)wl_op_param.addr.addr_val, RTK_BD_ADDR_LEN) ||
		(ret = rtk_bt_le_gap_modify_whitelist(&wl_op_param)) != RTK_BT_OK) {
		BT_LOGE("GAP whitelist remove failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP whitelist remove success\r\n");
	return 0;
}

static int atcmd_ble_gap_clear_whitelist(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_le_modify_wl_param_t wl_op_param;
	wl_op_param.op = RTK_BT_LE_WHITELIST_CLEAR;

	ret = rtk_bt_le_gap_modify_whitelist(&wl_op_param);
	if (ret) {
		BT_LOGE("GAP whitelist clear failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get whitelist clear success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_pairing_mode(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_pairing_mode_t pairing_mode = (rtk_bt_le_pairing_mode_t)(str_to_int(argv[0]));

	ret = rtk_bt_le_sm_set_pairing_mode(pairing_mode);
	if (ret) {
		BT_LOGE("GAP set pairing mode failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set pairing mode success\r\n");
	return 0;
}

static int atcmd_ble_gap_set_security_param(int argc, char **argv)
{
	uint16_t ret = 0;
	if (argc != 0 && argc != 7 && argc != 9) {
		BT_LOGE("GAP set security paramters failed! wrong args num!\r\n");
		return -1;
	}

	if (argc == 0) {
		rtk_bt_le_security_param_t def_sec_param = {
			.io_cap = RTK_IO_CAP_KEYBOARD_DISPALY,
			.oob_data_flag = 0,
			.bond_flag = 1,
			.mitm_flag = 0,
			.sec_pair_flag = 0,
			.sec_pair_only_flag = 0,
			.use_fixed_key = 0,
			.fixed_key = 000000,
			.auto_sec_req = 0,
		};
		ret = rtk_bt_le_sm_set_security_param(&def_sec_param);
		if (ret) {
			BT_LOGE("GAP set default security paramters failed! err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP set def security paramters success\r\n");
		return 0;
	}

	rtk_bt_le_security_param_t sec_param = {0};
	if (argc > 6) {
		sec_param.io_cap = (rtk_bt_le_io_cap_t)str_to_int(argv[0]);
		sec_param.oob_data_flag = (uint8_t)str_to_int(argv[1]);
		sec_param.bond_flag = (uint8_t)str_to_int(argv[2]);
		sec_param.mitm_flag = (uint8_t)str_to_int(argv[3]);
		sec_param.sec_pair_flag = (uint8_t)str_to_int(argv[4]);
		sec_param.use_fixed_key = (uint8_t)str_to_int(argv[5]);
		sec_param.fixed_key = (uint32_t)str_to_int(argv[6]);
	}
	if (argc > 8) {
		sec_param.sec_pair_only_flag = (uint8_t)str_to_int(argv[7]);
		sec_param.auto_sec_req = (uint8_t)str_to_int(argv[8]);
	}


	ret = rtk_bt_le_sm_set_security_param(&sec_param);
	if (ret) {
		BT_LOGE("GAP set security paramters failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP set security paramters success\r\n");
	return 0;
}

static int atcmd_ble_gap_get_security_param(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint8_t ret = 0;
	rtk_bt_le_security_param_t sec_param = {0};

	ret = rtk_bt_le_sm_get_security_param(&sec_param);
	if (ret) {
		BT_LOGE("GAP get security paramters failed, err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get security paramters success, param: %d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
			sec_param.io_cap, sec_param.oob_data_flag, sec_param.bond_flag, sec_param.mitm_flag,
			sec_param.sec_pair_flag, sec_param.use_fixed_key, sec_param.fixed_key,
			sec_param.sec_pair_only_flag, sec_param.auto_sec_req);
	BT_AT_PRINT("+BLEGAP:get_sec_param,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
				sec_param.io_cap, sec_param.oob_data_flag, sec_param.bond_flag, sec_param.mitm_flag,
				sec_param.sec_pair_flag, sec_param.use_fixed_key, sec_param.fixed_key,
				sec_param.sec_pair_only_flag, sec_param.auto_sec_req);
	return 0;
}

static int atcmd_ble_gap_security(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle;

	conn_handle = str_to_int(argv[0]);
	if ((ret = rtk_bt_le_sm_start_security(conn_handle)) != RTK_BT_OK) {
		BT_LOGE("GAP start security flow failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP starting security flow ...\r\n");
	return 0;
}

static int atcmd_ble_gap_confirm_pair(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_pair_cfm_t pair_cfm_param;

	pair_cfm_param.conn_handle = str_to_int(argv[0]);
	if ((pair_cfm_param.confirm = str_to_int(argv[1])) > 1 ||
		(ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param)) != RTK_BT_OK) {
		BT_LOGE("GAP pair confirm failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP pair confirm OK!\r\n");
	return 0;
}

static int atcmd_ble_gap_input_auth_key(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_auth_key_input_t auth_key_param;

	auth_key_param.conn_handle = str_to_int(argv[0]);
	if ((auth_key_param.passkey = (uint32_t)str_to_int(argv[1])) > 999999 ||
		(ret = rtk_bt_le_sm_passkey_entry(&auth_key_param)) != RTK_BT_OK) {
		BT_LOGE("GAP input auth key failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP input auth key OK!\r\n");
	return 0;
}

static int atcmd_ble_gap_confirm_auth_key(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_auth_key_confirm_t auth_keycfm_param;

	auth_keycfm_param.conn_handle = str_to_int(argv[0]);
	if ((auth_keycfm_param.confirm = str_to_int(argv[1])) > 1 ||
		(ret = rtk_bt_le_sm_passkey_confirm(&auth_keycfm_param)) != RTK_BT_OK) {
		BT_LOGE("GAP auth key confirm failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP auth key confirm OK!\r\n");
	return 0;
}

#if defined(RTK_BLE_SMP_OOB_SUPPORT) && RTK_BLE_SMP_OOB_SUPPORT
static int atcmd_ble_gap_input_auth_oob(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_set_oob_key_t auth_oob_param = {0};

	auth_oob_param.conn_handle = str_to_int(argv[0]);
	if (false == hexnum_str_to_array(argv[1], (uint8_t *)auth_oob_param.oob_key, RTK_BT_LE_SM_OOB_KEY_LEN) ||
		(ret = rtk_bt_le_sm_set_oob_tk(&auth_oob_param)) != RTK_BT_OK) {
		BT_LOGE("GAP input auth OOB TK failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP input auth OOB TK OK!\r\n");
	return 0;
}
#endif

static int atcmd_ble_gap_get_bond_num(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	uint8_t bond_num;

	ret = rtk_bt_le_sm_get_bond_num(&bond_num);
	if (ret) {
		BT_LOGE("GAP get bond number failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get bond number: %d\r\n", bond_num);
	BT_AT_PRINT("+BLEGAP:bond_num,%d\r\n", bond_num);
	return 0;
}

static int atcmd_ble_gap_get_bond_info(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	char addr_str[30] = {0};
	char ident_addr_str[30] = {0};
	uint8_t bond_size;

	ret = rtk_bt_le_sm_get_bond_num(&bond_size);
	if (ret) {
		BT_LOGE("GAP get bond number failed! err: 0x%x\r\n", ret);
		return -1;
	}

	if (!bond_size) {
		BT_LOGE("GAP no bond device!\r\n");
		return 0;
	}

	rtk_bt_le_bond_info_t *bond_info =
		(rtk_bt_le_bond_info_t *)osif_mem_alloc(RAM_TYPE_DATA_ON,
												bond_size * sizeof(rtk_bt_le_bond_info_t));
	memset(bond_info, 0, bond_size * sizeof(rtk_bt_le_bond_info_t));
	ret = rtk_bt_le_sm_get_bond_info(bond_info, &bond_size);
	if (ret) {
		BT_LOGE("GAP get bond info failed! err:0x%x\r\n", ret);
		goto out;
	}

	BT_LOGA("GAP get bond info:\r\n");
	BT_AT_PRINT("+BLEGAP:bond_info");
	for (int i = 0; i < bond_size; i++) {
		rtk_bt_le_addr_to_str(&bond_info[i].remote_addr, addr_str, sizeof(addr_str));
		rtk_bt_le_addr_to_str(&bond_info[i].ident_addr, ident_addr_str, sizeof(ident_addr_str));
		BT_LOGA("[%d] %s, resolved: %s\r\n", i, addr_str, ident_addr_str);
		BT_AT_PRINT(",%d,%s,%s", i, addr_str, ident_addr_str);
	}
	BT_AT_PRINT("\r\n");
out:
	if (bond_info) {
		osif_mem_free(bond_info);
	}

	return ret;
}

static int atcmd_ble_gap_delete_bond(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_addr_t addr;

	addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);

	if (false == hexdata_str_to_bd_addr(argv[1], (uint8_t *)addr.addr_val, RTK_BD_ADDR_LEN) ||
		(ret = rtk_bt_le_sm_delete_bond_device(&addr)) != RTK_BT_OK) {
		BT_LOGE("GAP remove bond device failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP removing bond device ...\r\n");
	return 0;
}

static int atcmd_ble_gap_clear_bond(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;

	ret = rtk_bt_le_sm_clear_bond_list();
	if (ret) {
		BT_LOGE("GAP clear bond info failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP clearing bond info ...\r\n");
	return 0;
}

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
static int atcmd_ble_gap_set_privacy_mode(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_ident_addr_type_t ident_type;
	rtk_bt_le_privacy_mode_t privacy_mode;
	uint8_t peer_ident[RTK_BD_ADDR_LEN] = {0};

	ident_type = (rtk_bt_le_ident_addr_type_t)str_to_int(argv[0]);
	if (false == hexdata_str_to_bd_addr(argv[1], peer_ident, RTK_BD_ADDR_LEN)) {
		return -1;
	}
	privacy_mode = (rtk_bt_le_privacy_mode_t)str_to_int(argv[2]);

	ret = rtk_bt_le_gap_set_privacy_mode(ident_type, peer_ident, privacy_mode);
	if (ret) {
		BT_LOGE("GAP set privacy mode failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("GAP set privacy mode success\r\n");
	return 0;
}

static int atcmd_ble_gap_read_local_rpa(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_ident_addr_type_t ident_type = RTK_BT_LE_IDENT_ADDR_PUBLIC;
	uint8_t peer_ident[RTK_BD_ADDR_LEN] = {0};
	uint8_t local_rpa[RTK_BD_ADDR_LEN] = {0};
	char addr_str[20];

	if (argc >= 1) {
		ident_type = (rtk_bt_le_ident_addr_type_t)str_to_int(argv[0]);
	}
	if (argc >= 2) {
		if (false == hexdata_str_to_bd_addr(argv[1], peer_ident, RTK_BD_ADDR_LEN)) {
			return -1;
		}
	}

	ret = rtk_bt_le_gap_read_local_resolv_addr(ident_type, peer_ident, local_rpa);
	if (ret) {
		BT_LOGE("GAP read local rpa failed! err: 0x%x\r\n", ret);
		return -1;
	}
	rtk_bt_addr_val_to_str(local_rpa, addr_str, sizeof(addr_str));
	BT_LOGA("GAP read local rpa success, %s\r\n", addr_str);
	BT_AT_PRINT("+BLEGAP:read_local_rpa,%s\r\n", addr_str);
	return 0;
}

static int atcmd_ble_gap_read_peer_rpa(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_le_ident_addr_type_t ident_type;
	uint8_t peer_ident[RTK_BD_ADDR_LEN] = {0};
	uint8_t peer_rpa[RTK_BD_ADDR_LEN] = {0};
	char addr_str[20];

	ident_type = (rtk_bt_le_ident_addr_type_t)str_to_int(argv[0]);
	if (false == hexdata_str_to_bd_addr(argv[1], peer_ident, RTK_BD_ADDR_LEN)) {
		return -1;
	}
	ret = rtk_bt_le_gap_read_peer_resolv_addr(ident_type, peer_ident, peer_rpa);
	if (ret) {
		BT_LOGE("GAP read peer rpa failed! err: 0x%x\r\n", ret);
		return -1;
	}
	rtk_bt_addr_val_to_str(peer_rpa, addr_str, sizeof(addr_str));
	BT_LOGA("GAP read peer rpa success, %s\r\n", addr_str);
	BT_AT_PRINT("+BLEGAP:read_peer_rpa,%s\r\n", addr_str);
	return 0;
}
#endif

static int atcmd_ble_gap_tx_pending_num(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle = 0, tx_pending_num = 0;

	conn_handle = (uint16_t)str_to_int(argv[0]);
	ret = rtk_bt_le_gap_get_tx_pending_num(conn_handle, &tx_pending_num);
	if (ret) {
		BT_LOGE("GAP get tx pending number failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("GAP get tx pending number, conn_handle: %d tx_pending_num: %d\r\n", conn_handle, tx_pending_num);
	BT_AT_PRINT("+BLEGAP:tx_pending_num,%d,%d\r\n", conn_handle, tx_pending_num);

	return 0;
}

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
static int atcmd_ble_gap_txpower_read(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle, ret = 0;
	uint8_t type, phy;
	int8_t cur_txpower = 0, max_txpower = 0;

	type = str_to_int(argv[0]);
	conn_handle = str_to_int(argv[1]);
	phy = str_to_int(argv[2]);

	if (type == 0) { // read local
		ret = rtk_bt_le_gap_read_local_tx_power(conn_handle, (rtk_bt_le_txpower_phy_t)phy, &cur_txpower, &max_txpower);
	} else { // read remote
		ret = rtk_bt_le_gap_read_remote_tx_power(conn_handle, (rtk_bt_le_txpower_phy_t)phy);
	}

	if (RTK_BT_OK != ret) {
		BT_LOGE("Read tx power failed! err: 0x%x\r\n", ret);
		return -1;
	}
	if (type == 0) {
		BT_LOGA("Read local tx power Success! curr: %d, max: %d\r\n", cur_txpower, max_txpower);
	} else {
		BT_LOGA("Read remote tx power Success!\r\n");
	}
	BT_AT_PRINT("+BLEGAP:txpower_read,%d,%d,%d\r\n", conn_handle, cur_txpower, max_txpower);

	return 0;
}

static int atcmd_ble_gap_txpower_report_set(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle, ret = 0;
	bool local_enable, remote_enable;

	conn_handle = str_to_int(argv[0]);
	local_enable = str_to_int(argv[1]);
	remote_enable = str_to_int(argv[2]);

	ret = rtk_bt_le_gap_tx_power_report_set(conn_handle, local_enable, remote_enable);
	if (RTK_BT_OK != ret) {
		BT_LOGE("Tx power reporting set failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("Tx power reporting set Success!\r\n");
	return 0;
}
#endif

static int atcmd_ble_gap_scan_info_filter(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_le_scan_info_filter_param_t info_filter = {0};

	if (str_to_int(argv[0])) {
		if (argc != 3) {
			BT_LOGE("GAP scan info filter set failed, wrong args num\r\n");
			return -1;
		}
		info_filter.enable = true;
		info_filter.offset = str_to_int(argv[1]);
		info_filter.len = strlen(argv[2]);
		info_filter.p_filter = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, info_filter.len);
		memcpy(info_filter.p_filter, argv[2], info_filter.len);
	} else {
		info_filter.enable = false;
	}

	ret = rtk_bt_le_gap_scan_info_filter(&info_filter);
	if (ret) {
		BT_LOGE("GAP scan info filter set failed, err: 0x%x\r\n", ret);
		osif_mem_free(info_filter.p_filter);
		return -1;
	}

	BT_LOGA("GAP scan info filter set success\r\n");
	osif_mem_free(info_filter.p_filter);
	return 0;
}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
static int atcmd_ble_gap_cte_conn_rx_set(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle, ret = 0;
	uint8_t enable;

	rtk_bt_le_gap_conn_cte_rx_param_t rx_params = {0, RTK_BLE_GAP_CTE_LEN_MAX, RTK_BT_LE_GAP_CTE_TYPE_AOA, RTK_BT_LE_GAP_SLOT_DURATIONS_2US, 0, NULL};

	enable = str_to_int(argv[0]);
	conn_handle = str_to_int(argv[1]);
	if (enable) {
		if (argc > 2) {
			rx_params.cte_req_interval  = str_to_int(argv[2]);
		}
		if (argc > 3) {
			rx_params.req_cte_len       = str_to_int(argv[3]);
		}
		if (argc > 4) {
			rx_params.req_cte_type      = str_to_int(argv[4]);
		}
		if (argc > 5) {
			rx_params.slot_durations    = str_to_int(argv[5]);
		}
		if (argc > 6) {
			rx_params.num_ant_ids       = str_to_int(argv[6]);
		}
		if (argc > 7) {
			rx_params.ant_ids = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, rx_params.num_ant_ids * sizeof(uint8_t));
			for (int i = 0; (i < argc - 7) && (i < rx_params.num_ant_ids); ++i) {
				rx_params.ant_ids[i] = str_to_int(argv[i + 7]);
			}
		}

		ret = rtk_bt_le_gap_conn_cte_rx_start(conn_handle, &rx_params);
		if (ret) {
			BT_LOGE("GAP connection cte rx start fail, err: 0x%x\r\n", ret);
			osif_mem_free(rx_params.ant_ids);
			return -1;
		}

		BT_LOGA("GAP connection cte rx start success\r\n");
		osif_mem_free(rx_params.ant_ids);
	} else {
		BT_LOGA("GAP connection cte rx stop");
		ret = rtk_bt_le_gap_conn_cte_rx_stop(conn_handle);
		if (ret) {
			BT_LOGE("GAP connection cte rx stop fail, err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP connection cte rx stop success\r\n");
	}

	return 0;
}

static int atcmd_ble_gap_cte_connless_rx_set(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t sync_id;
	uint8_t enable;

	rtk_bt_le_gap_connless_cte_rx_param_t params = {RTK_BT_LE_GAP_SLOT_DURATIONS_2US, 0, 0, NULL};

	enable = str_to_int(argv[0]);
	sync_id = str_to_int(argv[1]);
	if (enable) {
		if (argc > 2) {
			params.slot_durations   = str_to_int(argv[2]);
		}
		if (argc > 3) {
			params.max_sampled_ctes = str_to_int(argv[3]);
		}
		if (argc > 4) {
			params.num_ant_ids      = str_to_int(argv[4]);
		}
		if (argc > 5) {
			params.ant_ids = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, params.num_ant_ids * sizeof(uint8_t));
			for (int i = 0; (i < argc - 5) && (i < params.num_ant_ids); ++i) {
				params.ant_ids[i] = str_to_int(argv[i + 5]);
			}
		}

		ret = rtk_bt_le_gap_connless_cte_rx_start(sync_id, &params);
		if (ret) {
			BT_LOGE("GAP connectionless cte rx start fail, err: 0x%x\r\n", ret);
			osif_mem_free(params.ant_ids);
			return -1;
		}

		BT_LOGA("GAP connectionless cte rx start success\r\n");
		osif_mem_free(params.ant_ids);

	} else {
		ret = rtk_bt_le_gap_connless_cte_rx_stop(sync_id);
		if (ret) {
			BT_LOGE("GAP connectionless cte rx stop fail, err: 0x%x\r\n", ret);
			return -1;
		}
		BT_LOGA("GAP connectionless cte rx stop success\r\n");
	}
	return 0;
}

static int atcmd_ble_gap_get_antenna_info(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret;
	rtk_bt_le_gap_antenna_info_t antenna_info;

	ret = rtk_bt_le_gap_get_antenna_info(&antenna_info);
	if (ret) {
		BT_LOGE("GAP get antenna information supported fail, err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP get antenna information supported success, supported_switching_sampling_rates=%u, num_antenna=%u, max_switching_pattern_length=%u, max_cte_length=%u\r\n",
			antenna_info.supported_switching_sampling_rates,
			antenna_info.num_antennae,
			antenna_info.max_switching_pattern_length,
			antenna_info.max_cte_length);
	BT_AT_PRINT("+BLEGAP:get_antenna,%u,%u,%u,%u\r\n",
				antenna_info.supported_switching_sampling_rates,
				antenna_info.num_antennae,
				antenna_info.max_switching_pattern_length,
				antenna_info.max_cte_length);

	return 0;
}
#endif

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
static int atcmd_ble_gap_coc_register_psm(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t is_reg = str_to_int(argv[0]);
	uint16_t psm = str_to_int(argv[1]);

	ret = rtk_bt_le_gap_coc_register_psm(is_reg, psm);
	if (ret) {
		BT_LOGE("GAP coc reg or unreg psm failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc reg or unreg psm success\r\n");
	return 0;
}

static int atcmd_ble_gap_coc_set_psm_security(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t active = str_to_int(argv[0]);
	uint16_t le_psm = str_to_int(argv[1]);
	uint8_t sec_mode = str_to_int(argv[2]);
	uint8_t key_size = str_to_int(argv[3]);

	ret = rtk_bt_le_gap_coc_set_psm_security(le_psm, active, sec_mode, key_size);
	if (ret) {
		BT_LOGE("GAP coc se psm security failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc set psm security success\r\n");
	return 0;
}

static int atcmd_ble_gap_coc_set_param(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t param_type = str_to_int(argv[0]);
	uint16_t value = str_to_int(argv[1]);

	ret = rtk_bt_le_gap_coc_set_param(param_type, value);
	if (ret) {
		BT_LOGE("GAP coc set param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc set param success\r\n");
	return 0;
}

static int atcmd_ble_gap_coc_get_param(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint8_t param_type = str_to_int(argv[0]);
	uint16_t cid = str_to_int(argv[1]);
	uint16_t value = 0;

	ret = rtk_bt_le_gap_coc_get_chan_param(param_type, cid, &value);
	if (ret) {
		BT_LOGE("GAP coc get chan param failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc get chan param success, value: %d\r\n", value);
	BT_AT_PRINT("+BLEGAP:coc_get_param,%d\r\n", value);
	return 0;
}

static int atcmd_ble_gap_coc_connect(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t conn_handle = str_to_int(argv[0]);
	uint16_t le_psm = str_to_int(argv[1]);

	ret = rtk_bt_le_gap_coc_connect(conn_handle, le_psm);
	if (ret) {
		BT_LOGE("GAP create coc conn failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc conncting ...\r\n");
	return 0;
}

static int atcmd_ble_gap_coc_disconnect(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t cid = str_to_int(argv[0]);

	ret = rtk_bt_le_gap_coc_disconnect(cid);
	if (ret) {
		BT_LOGE("GAP coc disconn failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc disconnecting ...\r\n");
	return 0;
}

static int atcmd_ble_gap_coc_send_data(int argc, char *argv[])
{
	(void)argc;
	uint16_t ret = 0;
	uint16_t cid = str_to_int(argv[0]);
	uint16_t len = str_to_int(argv[1]);
	uint8_t *data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);

	if (false == hexdata_str_to_array(argv[2], data, len)) {
		return -1;
	}
	ret = rtk_bt_le_gap_coc_send_data(cid, len, data);
	osif_mem_free(data);
	if (ret) {
		BT_LOGE("GAP coc send data failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("GAP coc data sending ...\r\n");
	return 0;
}
#endif /* RTK_BLE_COC_SUPPORT */

static const cmd_table_t le_gap_cmd_table[] = {
	{"version",      atcmd_ble_gap_get_version,        1, 1},
	{"addr",         atcmd_ble_gap_get_bd_addr,        1, 1},
	{"channel",      atcmd_ble_gap_set_channel_map,    2, 2},
	{"dev_state",    atcmd_ble_gap_get_dev_state,      1, 1},
	{"name",         atcmd_ble_gap_set_dev_name,       2, 2},
	{"appear",       atcmd_ble_gap_set_appearance,     2, 2},
	{"set_mtu",      atcmd_ble_gap_set_max_mtu_size,   2, 2},
	{"pre_conn_param", atcmd_ble_gap_set_preferred_conn_param,     5, 5},
	{"rand_addr",    atcmd_ble_gap_set_rand_addr,      2, 3},
	{"adv_data",     atcmd_ble_gap_set_adv_data,       1, 2},
	{"scan_rsp",     atcmd_ble_gap_set_scan_resp,      1, 2},
	{"adv",          atcmd_ble_gap_op_adv,             2, 10},
	{"get_adv_param", atcmd_ble_gap_get_adv_param,     1, 1},
	{"scan_param",   atcmd_ble_gap_set_scan_param,     1, 7},
	{"get_scan_param", atcmd_ble_gap_get_scan_param,   1, 1},
	{"scan_info_filter", atcmd_ble_gap_scan_info_filter,       2, 4},
	{"scan",         atcmd_ble_gap_op_scan,            2, 2},
	{"conn",         atcmd_ble_gap_connect,            3, 11},
	{"conn_cancel",  atcmd_ble_gap_connect_cancel,     3, 3},
	{"get_active_conn", atcmd_ble_gap_get_active_conn, 1, 1},
	{"get_conn_handle", atcmd_ble_gap_get_conn_handle_by_addr, 3, 3},
	{"mtu_size",     atcmd_ble_gap_get_mtu_size,       2, 2},
	{"tx_pending_num", atcmd_ble_gap_tx_pending_num,   2, 2},
	{"conn_rssi",    atcmd_ble_gap_get_conn_rssi,      2, 2},
	{"conn_info",    atcmd_ble_gap_get_conn_info,      2, 2},
	{"conn_update",  atcmd_ble_gap_update_conn,        6, 6},
#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
	{"conn_phy",     atcmd_ble_gap_set_conn_phy,       6, 6},
#endif
#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
	{"conn_datalen", atcmd_ble_gap_set_conn_data_len,  4, 4},
#endif
	{"disconn",      atcmd_ble_gap_disconnect,         2, 2},
	{"disconn_reason", atcmd_ble_gap_disconnect_with_reason,   3, 3},
	{"wl_add",       atcmd_ble_gap_add_whitelist,      3, 3},
	{"wl_remove",    atcmd_ble_gap_remove_whitelist,   3, 3},
	{"wl_clear",     atcmd_ble_gap_clear_whitelist,    1, 1},
	{"pair_mode",    atcmd_ble_gap_set_pairing_mode,   2, 2},
	{"sec_param",    atcmd_ble_gap_set_security_param, 1, 10},
	{"get_sec_param",    atcmd_ble_gap_get_security_param, 1, 1},
	{"sec",          atcmd_ble_gap_security,           2, 2},
	{"pair_cfm",     atcmd_ble_gap_confirm_pair,       3, 3},
	{"auth_key",     atcmd_ble_gap_input_auth_key,     3, 3},
	{"auth_keycfm",  atcmd_ble_gap_confirm_auth_key,   3, 3},
#if defined(RTK_BLE_SMP_OOB_SUPPORT) && RTK_BLE_SMP_OOB_SUPPORT
	{"auth_oob",     atcmd_ble_gap_input_auth_oob,     3, 3},
#endif
	{"bond_num",     atcmd_ble_gap_get_bond_num,       1, 1},
	{"bond_info",    atcmd_ble_gap_get_bond_info,      1, 1},
	{"bond_del",     atcmd_ble_gap_delete_bond,        3, 3},
	{"bond_clear",   atcmd_ble_gap_clear_bond,         1, 1},
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	{"set_privacy_mode", atcmd_ble_gap_set_privacy_mode, 4, 4},
	{"read_local_rpa", atcmd_ble_gap_read_local_rpa,   1, 3},
	{"read_peer_rpa",  atcmd_ble_gap_read_peer_rpa,    3, 3},
#endif
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
	{"eadv_data",    atcmd_ble_gap_set_ext_adv_data,   1, 3},
	{"escan_rsp",    atcmd_ble_gap_set_ext_scan_resp,  1, 3},
	{"eadv",         atcmd_ble_gap_op_ext_adv,         2, 13},
#endif
#if defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT
	{"escan_param",  atcmd_ble_gap_ext_scan_set_param, 1, 13},
	{"escan",        atcmd_ble_gap_op_ext_scan,        2, 2},
#endif
#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) || (defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT)
	{"econn",        atcmd_ble_gap_ext_connect,        4, 12},
#endif
#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
	{"pa_adv",       atcmd_ble_gap_pa_adv,             2, 6},
	{"pa_update",    atcmd_ble_gap_pa_adv_update,      2, 4},
#endif
#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	{"pa_sync_list", atcmd_ble_gap_pa_sync_modify_list, 2, 5},
	{"pa_sync",      atcmd_ble_gap_pa_sync_op,         2, 9},
#endif
#if defined(RTK_BLE_5_1_PAST_SENDER_SUPPORT) && RTK_BLE_5_1_PAST_SENDER_SUPPORT
	{"past_send",    atcmd_ble_gap_past_send,          5, 5},
#endif
#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
	{"past_recv",    atcmd_ble_gap_past_recv,          1, 6},
	{"def_past_recv",    atcmd_ble_gap_default_past_recv,          1, 5},
#endif
#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
	{"txpower_read", atcmd_ble_gap_txpower_read,       4, 4},
	{"txpower_report", atcmd_ble_gap_txpower_report_set, 4, 4},
#endif
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	{"cte_conn_rx",  atcmd_ble_gap_cte_conn_rx_set,    3, 20},
	{"cte_connless_rx", atcmd_ble_gap_cte_connless_rx_set, 3, 18},
	{"get_antenna",  atcmd_ble_gap_get_antenna_info,   1, 1},
#endif
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	{"coc_reg_psm",  atcmd_ble_gap_coc_register_psm, 3, 3},
	{"coc_set_sec",  atcmd_ble_gap_coc_set_psm_security, 5, 5},
	{"coc_set_param",  atcmd_ble_gap_coc_set_param, 3, 3},
	{"coc_get_param",  atcmd_ble_gap_coc_get_param, 3, 3},
	{"coc_conn",     atcmd_ble_gap_coc_connect, 3, 3},
	{"coc_disconn",  atcmd_ble_gap_coc_disconnect, 2, 2},
	{"coc_send",     atcmd_ble_gap_coc_send_data, 4, 4},
#endif
	{NULL,},
};

int atcmd_bt_le_gap(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, le_gap_cmd_table, "[AT+BLEGAP]");
}