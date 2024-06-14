/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <osif.h>
#include <math.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <rtk_cte_service.h>
#include <bt_utils.h>

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

#define CTE_SRV_UUID                            (0x184A)
#define CTE_ENABLE_CHAR_UUID                    (0x2BAD)
#define CTE_ADV_MIN_LEN_CHAR_UUID               (0x2BAE)
#define CTE_ADV_MIN_TX_CNT_CHAR_UUID            (0x2BAF)
#define CTE_ADV_DURATION_CHAR_UUID              (0x2BB0)
#define CTE_ADV_INTERVAL_CHAR_UUID              (0x2BB1)
#define CTE_ADV_PHY_CHAR_UUID                   (0x2BB2)

#define RTK_BT_UUID_CTE_SRV                         BT_UUID_DECLARE_16(CTE_SRV_UUID)
#define RTK_BT_UUID_CTE_ENABLE_CHAR                 BT_UUID_DECLARE_16(CTE_ENABLE_CHAR_UUID)
#define RTK_BT_UUID_CTE_ADV_MIN_LEN_CHAR            BT_UUID_DECLARE_16(CTE_ADV_MIN_LEN_CHAR_UUID)
#define RTK_BT_UUID_CTE_ADV_MIN_TX_CNT_CHAR         BT_UUID_DECLARE_16(CTE_ADV_MIN_TX_CNT_CHAR_UUID)
#define RTK_BT_UUID_CTE_ADV_DURATION_CHAR           BT_UUID_DECLARE_16(CTE_ADV_DURATION_CHAR_UUID)
#define RTK_BT_UUID_CTE_ADV_INTERVAL_CAHR           BT_UUID_DECLARE_16(CTE_ADV_INTERVAL_CHAR_UUID)
#define RTK_BT_UUID_CTE_ADV_PHY_CHAR                BT_UUID_DECLARE_16(CTE_ADV_PHY_CHAR_UUID)

#define CTE_ENABLE_CHAR_VAL_INDEX                   (2)
#define CTE_ADV_MIN_LEN_CHAR_VAL_INDEX              (4)
#define CTE_ADV_MIN_TX_CNT_CHAR_VAL_INDEX           (6)
#define CTE_ADV_TX_DURATION_CHAR_VAL_INDEX          (8)
#define CTE_ADV_INTERVAL_CHAR_VAL_INDEX             (10)
#define CTE_ADV_PHY_CHAR_VAL_INDEX                  (12)

#define CTE_CHAR_ENABLE_BIT_CONNECTION_AOA          (0x0)
#define CTE_CHAR_ENABLE_BIT_CONNECTIONLESS_AOD      (0x1)

#define CTE_CHAR_CTE_ENABLE_MIN                     (0x00)
#define CTE_CHAR_CTE_ENBALE_MAX                     (0x03)
#define CTE_CHAR_CTE_ENABLE_VALUE_IN_RANGE(value)           \
    ((value) <= CTE_CHAR_CTE_ENBALE_MAX)

#define CTE_CHAR_ADV_CTE_MIN_LEN_MIN                (2)
#define CTE_CHAR_ADV_CTE_MIN_LEN_MAX                (20)
#define CTE_CHAR_ADV_CTE_MIN_LEN_VALUE_IN_RANGE(value)      \
    (((value) >= CTE_CHAR_ADV_CTE_MIN_LEN_MIN) && ((value) <= CTE_CHAR_ADV_CTE_MIN_LEN_MAX))

#define CTE_CHAR_ADV_CTE_MIN_TX_CNT_MIN             (1)
#define CTE_CHAR_ADV_CTE_MIN_TX_CNT_MAX             (15)
#define CTE_CHAR_ADV_CTE_MIN_TX_CNT_VALUE_IN_RANGE(value)   \
    (((value) >= CTE_CHAR_ADV_CTE_MIN_TX_CNT_MIN) && ((value) <= CTE_CHAR_ADV_CTE_MIN_TX_CNT_MAX))

#define CTE_CHAR_ADV_CTE_TX_DURATION_MIN            (0)
#define CTE_CHAR_ADV_CTE_TX_DURATION_MAX            (255)
#define CTE_CHAR_ADV_CTE_TX_DURATION_VALUE_IN_RANGE(value)  \
    (((value) >= CTE_CHAR_ADV_CTE_TX_DURATION_MIN) && ((value) <= CTE_CHAR_ADV_CTE_TX_DURATION_MAX))

#define CTE_CHAR_ADV_CTE_INTERVAL_MIN               (0x0006)
#define CTE_CHAR_ADV_CTE_INTERVAL_MAX               (0xFFFF)
#define CTE_CHAR_ADV_CTE_INTERVAL_VALUE_IN_RANGE(value)     \
    ((value) >= CTE_CHAR_ADV_CTE_INTERVAL_MIN)

#define CTE_CHAR_ADV_CTE_PHY_MIN                    (0x0)
#define CTE_CHAR_ADV_CTE_PHY_MAX                    (0x3)
#define CTE_CHAR_ADV_CTE_PHY_VALUE_IN_RANGE(value)          \
    ((value) <= CTE_CHAR_ADV_CTE_PHY_MAX)

#define RTK_BLE_PA_INTERVAL_OFFSET                  (10)

#define POINTER_TO_UINT8(value)     (*((uint8_t *)(value)))
#define POINTER_TO_UINT16(value)    (*((uint16_t *)(value)))

typedef struct {
	bool enable;
	uint8_t adv_handle;
} cte_connless_enable_info_t;

static rtk_bt_gatt_attr_t cte_attrs[] = {
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_CTE_SRV),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ENABLE_CHAR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ADV_MIN_LEN_CHAR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ADV_MIN_TX_CNT_CHAR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ADV_DURATION_CHAR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ADV_INTERVAL_CAHR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),

	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CTE_ADV_PHY_CHAR,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service cte_srv = RTK_BT_GATT_SERVICE(cte_attrs, CTE_SRV_ID);

static bool cte_conn_enable_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static cte_connless_enable_info_t cte_connless_enable_info = {
	.enable = false,
	.adv_handle = 0,
};

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
static rtk_bt_le_ext_adv_param_t ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_NON_SCAN_NON_CONN_UNDIRECTED,
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

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
static uint8_t pa_data[] = {
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
static rtk_bt_le_pa_param_t pa_param = {0, 64, 80, RTK_BT_LE_PA_ADV_PROP_INCLUDE_TX_POWER, (uint8_t *)pa_data, sizeof(pa_data)};
#endif

static cte_antenna_t cte_conn_antenna = {
	.num_ant_ids = 0,
	.ant_ids = {0},
};

/* cte connless duration, unit: second, part by 51 (in order 0, 64, 128, 192, 256) */
#define CTE_CONNLESS_DURATION_GAP       (64.0)
#define CTE_CONNLESS_DURATION_GAP_NUM   (5)
static uint32_t cte_connless_duration_arr[CTE_CONNLESS_DURATION_GAP_NUM] = {1, 1, 445, 198730, 1000000};
static uint32_t cte_connless_duration = 0;

static cte_antenna_t cte_connless_antenna = {
	.num_ant_ids = 0,
	.ant_ids = {0},
};

static rtk_bt_le_gap_connless_cte_tx_param_t cte_connless_param = {
	.duration = 0,
	.cte_len = 0x14,
	.cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_2US,
	.cte_count = 1,
	.num_ant_ids = 0,
	.ant_ids = cte_connless_antenna.ant_ids,
};

static rtk_bt_le_gap_conn_cte_tx_param_t cte_conn_param = {
	.cte_types = RTK_BT_LE_GAP_CTE_TYPE_AOA,
	.num_ant_ids = 0,
	.ant_ids = cte_conn_antenna.ant_ids,
};

static void *cte_connless_timer_hdl = NULL;

static uint16_t cte_adv_enable(uint8_t enable);

static void cte_connless_stop_timer(void)
{
	if (cte_connless_timer_hdl) {
		osif_timer_delete(&cte_connless_timer_hdl);
		cte_connless_timer_hdl = NULL;
	}
}

static void cte_connless_timeout_handler(void *arg)
{
	(void)arg;
	cte_adv_enable(0);
}

static void cte_connless_start_timer(void)
{
	if (cte_connless_timer_hdl) {
		cte_connless_stop_timer();
	}

	osif_timer_create(&cte_connless_timer_hdl, "bt_cte_connless_start_timer", NULL, cte_connless_duration * 1000, false,
					  cte_connless_timeout_handler);
	if (cte_connless_timer_hdl == NULL) {
		BT_LOGE("[CTE Service] cte_connless_start_timer create failed!\r\n");

	} else {
		BT_LOGA("[CTE Service] cte_connless_timer start succeed!\r\n");
		osif_timer_start(&cte_connless_timer_hdl);
	}

	return;
}

static void cte_srv_reset_connless_params(void)
{
	memset(&cte_connless_antenna, 0, sizeof(cte_connless_antenna));

	cte_connless_param.duration = 0;
	cte_connless_param.cte_len = 0x14;
	cte_connless_param.cte_type = RTK_BT_LE_GAP_CTE_TYPE_AOD_2US;
	cte_connless_param.cte_count = 1;
	cte_connless_param.num_ant_ids = 0;
	cte_connless_param.ant_ids = cte_connless_antenna.ant_ids;

#if ((defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) && (defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT))
	pa_param.pa_interval_max = 80;
	pa_param.pa_interval_min = 64;

	ext_adv_param.secondary_adv_phy = RTK_BT_LE_PHYS_2M;
#endif

	cte_connless_duration = 0;
}

static void cte_srv_reset_params(void)
{
	memset(&cte_conn_antenna, 0, sizeof(cte_conn_antenna));

	cte_conn_param.cte_types = RTK_BT_LE_GAP_CTE_TYPE_AOA;
	cte_conn_param.num_ant_ids = 0;
	cte_conn_param.ant_ids = cte_conn_antenna.ant_ids;

	cte_srv_reset_connless_params();
}

uint16_t cte_srv_add(void)
{
	cte_srv.type = GATT_SERVICE_OVER_BLE;
	cte_srv.server_info = 0;
	cte_srv.user_data = NULL;
	cte_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&cte_srv);
}

static bool cte_char_writable_check(uint16_t index)
{
	if (CTE_ENABLE_CHAR_VAL_INDEX           == index ||
		CTE_ADV_MIN_LEN_CHAR_VAL_INDEX      == index ||
		CTE_ADV_MIN_TX_CNT_CHAR_VAL_INDEX   == index ||
		CTE_ADV_TX_DURATION_CHAR_VAL_INDEX  == index ||
		CTE_ADV_INTERVAL_CHAR_VAL_INDEX     == index ||
		CTE_ADV_PHY_CHAR_VAL_INDEX          == index) {
		return true;
	}

	return false;
}

static bool cte_char_value_range_check(uint16_t index, uint16_t len, uint8_t *value)
{
	bool ret = false;

	if (!value) {
		BT_LOGE("%s: value\r\n", __func__);
		return ret;
	}

	switch (index) {
	case CTE_ENABLE_CHAR_VAL_INDEX:
		if (len == sizeof(uint8_t) && CTE_CHAR_CTE_ENABLE_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_ADV_MIN_LEN_CHAR_VAL_INDEX:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_MIN_LEN_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_ADV_MIN_TX_CNT_CHAR_VAL_INDEX:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_MIN_TX_CNT_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_ADV_INTERVAL_CHAR_VAL_INDEX:
		if (len == sizeof(uint16_t) && CTE_CHAR_ADV_CTE_INTERVAL_VALUE_IN_RANGE(POINTER_TO_UINT16(value))) {
			ret = true;
		}
		break;
	case CTE_ADV_TX_DURATION_CHAR_VAL_INDEX:
		if (len == sizeof(uint8_t)) {
			ret = true;
		}
		break;
	case CTE_ADV_PHY_CHAR_VAL_INDEX:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_PHY_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	default:
		BT_LOGE("%s: unsupport char index %u\r\n", __func__, index);
		break;
	}

	return ret;
}

static bool cte_connection_handle_enable_check(uint16_t conn_handle)
{
	uint8_t conn_id;
	bool enable = false;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return false;
	}

	if (conn_id < RTK_BLE_GAP_MAX_LINKS) {
		enable = cte_conn_enable_map[conn_id];
	}

	return enable;
}

static uint16_t cte_adv_enable(uint8_t enable)
{
	uint16_t ret = RTK_BT_OK;

	if (enable && cte_connless_enable_info.enable) {
		BT_LOGE("[CTE Service] CTE connectionless tx has started already\r\n");
		return ret;
	}

	if (enable) {
#if ((defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) && (defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT))
		ret = rtk_bt_le_gap_connless_cte_tx_start(&cte_connless_param,
												  &ext_adv_param,
												  &pa_param,
												  &cte_connless_enable_info.adv_handle);
		BT_LOGA("[CTE Service] CTE connectionless tx start, adv_handle %u, ret %u\r\n", cte_connless_enable_info.adv_handle, ret);

		if (!ret) {
			cte_connless_enable_info.enable = true;
			if (cte_connless_duration > 0) {
				cte_connless_start_timer();
			}
		}
#endif

	} else {
		if (cte_connless_enable_info.enable) {
#if ((defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) && (defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT))
			ret = rtk_bt_le_gap_connless_cte_tx_stop(cte_connless_enable_info.adv_handle);
			memset(&cte_connless_enable_info, 0, sizeof(cte_connless_enable_info));

			BT_LOGA("[CTE Service] CTE connectionless tx stop, adv_handle %u, ret %u\r\n", cte_connless_enable_info.adv_handle, ret);
#endif
			if (cte_connless_timer_hdl) {
				cte_connless_stop_timer();
			}
			/* reset adv cte duration */
			cte_connless_param.duration = 0;
			cte_connless_duration = 0;
		}
	}

	return ret;
}

uint16_t cte_conn_enable(uint8_t enable, uint16_t conn_handle)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t conn_id;

	ret = rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id);
	if (ret != RTK_BT_OK) {
		return ret;
	}

	if (enable) {
		if (!cte_conn_enable_map[conn_id]) {
			ret = rtk_bt_le_gap_conn_cte_tx_start(conn_handle, &cte_conn_param);
			BT_LOGA("[CTE Service] CTE connection tx start ret %u\r\n", ret);

			if (!ret) {
				cte_conn_enable_map[conn_id] = true;
			}
		}
	} else {
		if (cte_connection_handle_enable_check(conn_handle)) {
			ret = rtk_bt_le_gap_conn_cte_tx_stop(conn_handle);
			cte_conn_enable_map[conn_id] = false;

			BT_LOGA("[CTE Service] CTE connection tx stop ret %u\r\n", ret);
		}
	}

	return ret;
}

static uint16_t cte_connless_tx_update(void)
{
	uint16_t ret = RTK_BT_OK;

	if (cte_connless_enable_info.enable) {
		ret = cte_adv_enable(false);
		if (!ret) {
			ret = cte_adv_enable(true);
		}
	}

	return ret;
}

static uint16_t cte_srv_write_char_cte_adv_min_len(uint8_t value)
{
	uint16_t ret = RTK_BT_OK;

	if (cte_connless_param.cte_len != value) {
		cte_connless_param.cte_len = value;
		ret = cte_connless_tx_update();
	}

	return ret;
}

static uint16_t cte_srv_write_char_cte_adv_min_tx_cnt(uint8_t value)
{
	uint16_t ret = RTK_BT_OK;

	if (cte_connless_param.cte_count != value) {
		cte_connless_param.cte_count = value;
		ret = cte_connless_tx_update();
	}

	return ret;
}

static uint16_t cte_srv_write_char_cte_adv_tx_duration(uint8_t value)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t index;
	uint32_t duration = 0;

	if (value > 0) {
		index = (uint8_t)(floor(value / CTE_CONNLESS_DURATION_GAP));
		if (index < CTE_CONNLESS_DURATION_GAP_NUM) {
			duration = cte_connless_duration_arr[index];
		}
	}

	if (cte_connless_duration != duration) {
		cte_connless_duration = duration;
		ret = cte_connless_tx_update();
	}

	return ret;
}

static uint16_t cte_srv_write_char_cte_adv_interval(uint16_t value)
{
	uint16_t ret = RTK_BT_OK;

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
	if (pa_param.pa_interval_min != value) {
		// Minimum advertising interval must be different from periodic_adv_interval_max
		pa_param.pa_interval_min = value;
		pa_param.pa_interval_max = pa_param.pa_interval_min + RTK_BLE_PA_INTERVAL_OFFSET;

		ret = cte_connless_tx_update();
	}
#else
	(void)value;
#endif
	return ret;
}

static uint16_t cte_srv_write_char_cte_adv_phy(uint8_t value)
{
	uint16_t ret = RTK_BT_OK;

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
	if (ext_adv_param.secondary_adv_phy != value) {
		ext_adv_param.secondary_adv_phy = value;
		ret = cte_connless_tx_update();
	}
#else
	(void)value;
#endif

	return ret;
}

static uint16_t cte_srv_write_char_cte_enable(uint16_t conn_handle, uint8_t value)
{
	uint16_t ret;
	uint8_t conn_id;
	uint8_t connless_aod_enable = (value >> CTE_CHAR_ENABLE_BIT_CONNECTIONLESS_AOD) & 0x1;
	uint8_t conn_aoa_enable = (value >> CTE_CHAR_ENABLE_BIT_CONNECTION_AOA) & 0x1;

	ret = rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id);
	if (ret != RTK_BT_OK) {
		return ret;
	}

	if (conn_aoa_enable) {
		if (cte_conn_enable_map[conn_id] && ((cte_conn_param.cte_types & RTK_BT_LE_GAP_CTE_TYPE_AOA) == 0)) {
			uint8_t value = RTK_BT_LE_GAP_CTE_TYPE_AOA;
			cte_srv_set_params(CTE_SRV_PARAM_TYPE_CONN_CTE_TYPE, sizeof(value), &value);
			cte_conn_enable(false, conn_handle);
		}
	}

	if (connless_aod_enable) {
		if (cte_connless_enable_info.enable && cte_connless_param.cte_type == RTK_BT_LE_GAP_CTE_TYPE_AOA) {
			uint8_t value = RTK_BT_LE_GAP_CTE_TYPE_AOD_2US;
			cte_srv_set_params(CTE_SRV_PARAM_TYPE_ADV_CTE_TYPE, sizeof(value), &value);
			cte_adv_enable(false);
		}
	}

	ret = cte_conn_enable(conn_aoa_enable, conn_handle);
	if (ret == RTK_BT_OK) {
		ret = cte_adv_enable(connless_aod_enable);
	}

	return ret;
}

static uint16_t cte_write_ind_post_proc(uint16_t conn_handle,
										uint16_t index,
										uint8_t *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		return RTK_BT_FAIL;
	}

	switch (index) {
	case CTE_ENABLE_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_enable(conn_handle, POINTER_TO_UINT8(value));
		break;
	case CTE_ADV_MIN_LEN_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_adv_min_len(POINTER_TO_UINT8(value));
		break;
	case CTE_ADV_MIN_TX_CNT_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_adv_min_tx_cnt(POINTER_TO_UINT8(value));
		break;
	case CTE_ADV_TX_DURATION_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_adv_tx_duration(POINTER_TO_UINT8(value));
		break;
	case CTE_ADV_INTERVAL_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_adv_interval(POINTER_TO_UINT16(value));
		break;
	case CTE_ADV_PHY_CHAR_VAL_INDEX:
		ret = cte_srv_write_char_cte_adv_phy(POINTER_TO_UINT8(value));
		break;
	default:
		break;
	}

	return ret;
}

static void cte_write_hdl(void *data)
{
	uint16_t ret;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (cte_char_writable_check(p_write_ind->index)) {
		if (!p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_HANDLE;
		} else if (!cte_char_value_range_check(p_write_ind->index, p_write_ind->len, p_write_ind->value)) {
			write_resp.err_code = RTK_BT_ATT_ERR_OUT_OF_RANGE;
			BT_LOGE("[APP] CTE write index %u out of range\r\n", p_write_ind->index);
		} else {
			ret = cte_write_ind_post_proc(p_write_ind->conn_handle, p_write_ind->index, p_write_ind->value);
			if (ret != RTK_BT_OK) {
				write_resp.err_code = RTK_BT_ATT_ERR_WRITE_REQUEST_REJECTED;
			}
		}
	} else {
		write_resp.err_code = RTK_BT_ATT_ERR_INVALID_HANDLE;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] CTE response for client write succeed, index:%d\r\n", p_write_ind->index);
	} else {
		BT_LOGE("[APP] CTE response for client write failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);
}

void cte_srv_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] CTE register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] CTE register service failed, err: 0x%x\r\n", reg_srv_res->reg_status);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};
		read_resp.app_id = p_read_ind->app_id;
		read_resp.conn_handle = p_read_ind->conn_handle;
		read_resp.cid = p_read_ind->cid;
		read_resp.index = p_read_ind->index;
		BT_LOGE("[APP] CTE read event, while no readable attr, unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] CTE response for client read succeed!\r\n");
		} else {
			BT_LOGE("[APP] CTE response for client read failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
					read_resp.conn_handle, read_resp.index,
					read_resp.err_code);
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND: {
		cte_write_hdl(data);
		break;
	}

	default:
		BT_LOGE("[APP] CTE NO corresponding event: %d\r\n", event);
		break;
	}
}

void cte_srv_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (conn_id < RTK_BLE_GAP_MAX_LINKS) {
		cte_conn_enable_map[conn_id] = false;
	}
}

void cte_srv_status_deinit(void)
{
	memset(cte_conn_enable_map, 0, sizeof(cte_conn_enable_map));
	memset(&cte_connless_enable_info, 0, sizeof(cte_connless_enable_info));
	cte_srv_reset_params();
	if (cte_connless_timer_hdl) {
		cte_connless_stop_timer();
	}
}

static bool cte_srv_param_value_range_check(CTE_SRV_PARAM_TYPE_e param_type, uint16_t len, void *value)
{
	bool ret = false;

	if (!value) {
		return false;
	}

	switch (param_type) {
	case CTE_SRV_PARAM_TYPE_ENABLE_ADV_CTE:
		if (len == sizeof(uint8_t) && RTK_BLE_GAP_CTE_TX_ENABLE_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ENABLE_CONN_CTE:
		if (len == sizeof(cte_conn_enable_t) && RTK_BLE_GAP_CTE_TX_ENABLE_VALUE_IN_RANGE(((cte_conn_enable_t *)value)->enable)) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_LEN:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_MIN_LEN_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_TX_CNT:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_MIN_TX_CNT_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TX_DURATION:
		if (len == sizeof(uint8_t)) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_INTERVAL:
		if (len == sizeof(uint16_t) && CTE_CHAR_ADV_CTE_INTERVAL_VALUE_IN_RANGE(POINTER_TO_UINT16(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_PHY:
		if (len == sizeof(uint8_t) && CTE_CHAR_ADV_CTE_PHY_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TYPE:
		if (len == sizeof(uint8_t) && RTK_BLE_GAP_CTE_TYPE_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	case CTE_SRV_PARAM_TYPE_CONN_ANTENNA:
	case CTE_SRV_PARAM_TYPE_ADV_ANTENNA: {
		/* AoA TX and AoD RX use single antenna. */
		ret = true;
		break;
	}
	case CTE_SRV_PARAM_TYPE_CONN_CTE_TYPE:
		if (len == sizeof(uint8_t) && RTK_BLE_GAP_CTE_TYPES_VALUE_IN_RANGE(POINTER_TO_UINT8(value))) {
			ret = true;
		}
		break;
	default:
		BT_LOGE("%s: cte srv set param type unsupport\r\n", __func__);
		break;
	}

	return ret;
}

uint16_t cte_srv_set_params(CTE_SRV_PARAM_TYPE_e param_type, uint16_t len, void *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		BT_LOGE("[APP] CTE service set param invalid value\r\n");
		return RTK_BT_FAIL;
	}

	if (!cte_srv_param_value_range_check(param_type, len, value)) {
		BT_LOGE("[APP] CTE service set param value check failed\r\n");
		return RTK_BT_FAIL;
	}

	switch (param_type) {
	case CTE_SRV_PARAM_TYPE_ENABLE_ADV_CTE: {
		ret = cte_adv_enable(POINTER_TO_UINT8(value));
		break;
	}
	case CTE_SRV_PARAM_TYPE_ENABLE_CONN_CTE: {
		cte_conn_enable_t *param = (cte_conn_enable_t *)value;
		ret = cte_conn_enable(param->enable, param->conn_handle);
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_LEN: {
		memcpy(&cte_connless_param.cte_len, value, len);
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_TX_CNT: {
		memcpy(&cte_connless_param.cte_count, value, len);
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TX_DURATION: {
		uint8_t duration = POINTER_TO_UINT8(value);
		uint8_t index;
		if (duration > 0) {
			index = (uint8_t)(duration / CTE_CONNLESS_DURATION_GAP);
			if (index < CTE_CONNLESS_DURATION_GAP_NUM) {
				cte_connless_duration = cte_connless_duration_arr[index];
			}
		} else {
			cte_connless_duration = 0;
		}
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_INTERVAL: {
#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
		memcpy(&pa_param.pa_interval_min, value, len);
		pa_param.pa_interval_max = pa_param.pa_interval_min + RTK_BLE_PA_INTERVAL_OFFSET;
#endif
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_PHY: {
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
		memcpy(&ext_adv_param.secondary_adv_phy, value, len);
#endif
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TYPE: {
		memcpy(&cte_connless_param.cte_type, value, len);
		break;
	}
	case CTE_SRV_PARAM_TYPE_CONN_ANTENNA: {
		memcpy(&cte_conn_antenna, value, len);
		cte_conn_param.num_ant_ids = cte_conn_antenna.num_ant_ids;
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_ANTENNA: {
		memcpy(&cte_connless_antenna, value, len);
		cte_connless_param.num_ant_ids = cte_connless_antenna.num_ant_ids;
		break;
	}
	case CTE_SRV_PARAM_TYPE_CONN_CTE_TYPE: {
		memcpy(&cte_conn_param.cte_types, value, len);
		break;
	}
	default: {
		BT_LOGE("%s: unsupported param type %u\r\n", __func__, param_type);
		break;
	}
	}

	BT_LOGA("[APP] CTE service set param type %u %s\r\n", param_type, (ret == RTK_BT_OK) ? "succeed" : "failed");

	return ret;
}

#endif  /* RTK_BLE_5_1_CTE_SUPPORT */
