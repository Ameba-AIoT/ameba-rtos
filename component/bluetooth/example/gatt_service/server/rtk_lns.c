/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <bt_api_config.h>
#include <rtk_service_config.h>
#include <rtk_lns.h>
#include <bt_utils.h>

/* LNS UUIDs (spec) */
#define LN_SRV_UUID                         0x1819
#define LN_FEATURE_CHAR_UUID                0x2A6A
#define LOCATION_AND_SPEED_CHAR_UUID        0x2A67
#define POSITION_QUALITY_CHAR_UUID          0x2A69
#define LN_CONTROL_POINT_CHAR_UUID          0x2A6B
#define NAVIGATION_CHAR_UUID                0x2A68

#define RTK_BT_UUID_LN_SRV                  BT_UUID_DECLARE_16(LN_SRV_UUID)
#define RTK_BT_UUID_LN_FEATURE_CHAR         BT_UUID_DECLARE_16(LN_FEATURE_CHAR_UUID)
#define RTK_BT_UUID_LOCATION_AND_SPEED_CHAR BT_UUID_DECLARE_16(LOCATION_AND_SPEED_CHAR_UUID)
#define RTK_BT_UUID_POSITION_QUALITY_CHAR   BT_UUID_DECLARE_16(POSITION_QUALITY_CHAR_UUID)
#define RTK_BT_UUID_LN_CONTROL_POINT_CHAR   BT_UUID_DECLARE_16(LN_CONTROL_POINT_CHAR_UUID)
#define RTK_BT_UUID_NAVIGATION_CHAR         BT_UUID_DECLARE_16(NAVIGATION_CHAR_UUID)

/* Attribute indexes (each CHARACTERISTIC expands to declaration + value,
 * so value index = declaration index + 1) */
#define LN_FEATURE_CHAR_VAL_INDEX           (2)   /* read */
#define LOCATION_AND_SPEED_CHAR_VAL_INDEX   (4)   /* notify */
#define LOCATION_AND_SPEED_CHAR_CCCD_INDEX  (5)   /* cccd notify */
#define POSITION_QUALITY_CHAR_VAL_INDEX     (7)   /* read */
#define LN_CONTROL_POINT_CHAR_VAL_INDEX     (9)   /* write, indicate */
#define LN_CONTROL_POINT_CHAR_CCCD_INDEX    (10)  /* cccd indicate */
#define NAVIGATION_CHAR_VAL_INDEX           (12)  /* notify */
#define NAVIGATION_CHAR_CCCD_INDEX          (13)  /* cccd notify */

/* LN Control Point Op Codes (spec / Assigned Numbers) */
#define LN_CP_OPCODE_SET_CUMULATIVE_VALUE           0x01
#define LN_CP_OPCODE_MASK_LOC_SPEED_CONTENT         0x02
#define LN_CP_OPCODE_NAVIGATION_CONTROL             0x03
#define LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES       0x04
#define LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE          0x05
#define LN_CP_OPCODE_SELECT_ROUTE                   0x06
#define LN_CP_OPCODE_SET_FIX_RATE                   0x07
#define LN_CP_OPCODE_SET_ELEVATION                  0x08
#define LN_CP_OPCODE_RESPONSE_CODE                  0x20

/* LN Control Point Response Value */
#define LN_CP_RESP_SUCCESS                          0x01
#define LN_CP_RESP_OP_CODE_NOT_SUPPORTED            0x02
#define LN_CP_RESP_INVALID_OPERAND                  0x03
#define LN_CP_RESP_OPERATION_FAILED                 0x04

/* Navigation Control parameter values */
#define LN_NAV_CTRL_STOP                            0x00
#define LN_NAV_CTRL_START                           0x01
#define LN_NAV_CTRL_PAUSE                           0x02
#define LN_NAV_CTRL_CONTINUE                        0x03
#define LN_NAV_CTRL_SKIP_WAYPOINT                   0x04
#define LN_NAV_CTRL_SELECT_NEAREST_WAYPOINT         0x05

/* Common Profile and Service error codes (Bluetooth Core Spec / rtk_bt_def.h);
 * PTS SPE/BI expects these exact values, not the 0x80/0x81 application range. */
#define LN_ATT_ERR_PROC_ALREADY_IN_PROGRESS         0xFE
#define LN_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED       0xFD

/* LN Control Point indicate payload max: op_code(1) + req_op_code(1) + resp_value(1) + params */
#define LN_CP_RESP_MAX_LEN                          32

/* Enable feature bits b0-b20 by default to demonstrate all fields / procedures */
static uint32_t ln_feature_val = 0x001FFFFF;

/* Default carries all optional fields. lns_location_speed_notify() splits the value across
 * the ATT MTU per spec 3.2 (fields that do not fit are sent in subsequent notifications), so
 * a single configuration exercises CN/BV-01-C ~ BV-08-C even at the default 23-octet MTU. */
static lns_loc_speed_val_t ln_loc_speed_val = {
	.flags = {
		.inst_speed_present = 1,
		.total_distance_present = 1,
		.location_present = 1,
		.elevation_present = 1,
		.heading_present = 1,
		.rolling_time_present = 1,
		.utc_time_present = 1,
		.position_status = LNS_POSITION_STATUS_POSITION_OK,
		.speed_distance_format = 0,
		.elevation_source = LNS_ELEVATION_SOURCE_POSITIONING_SYSTEM,
		.heading_source = 0,
	},
	.inst_speed = 500,          /* 5.00 m/s */
	.total_distance = 1000,     /* 100.0 m */
	.latitude = 515074000,      /* 51.5074 deg */
	.longitude = -1278000,      /* -0.1278 deg */
	.elevation = 3500,          /* 35.00 m */
	.heading = 9000,            /* 90.00 deg */
	.rolling_time = 10,
	.utc_time = {0xEA, 0x07, 0x01, 0x01, 0x00, 0x00, 0x00},  /* 2026-01-01 00:00:00 */
};

/* Default carries all optional fields (exercises CR/BV-03-C ~ BV-09-C). For CR/BV-02-C
 * [Position Quality - no optional fields present] the value must be exactly 2 octets
 * (Flags only, all Present/RFU bits = 0); set flags = 0 here or via
 * lns_srv_set_params(LNS_PARAM_POS_QUALITY_FLAG, ...). */
static lns_pos_quality_val_t ln_pos_quality_val = {
	.flags = {
		.beacons_in_solution_present = 1,
		.beacons_in_view_present = 1,
		.time_to_first_fix_present = 1,
		.ehpe_present = 1,
		.evpe_present = 1,
		.hdop_present = 1,
		.vdop_present = 1,
		.position_status = LNS_POSITION_STATUS_POSITION_OK,
	},
	.beacons_in_solution = 8,
	.beacons_in_view = 12,
	.time_to_first_fix = 50,    /* 5.0 s */
	.ehpe = 300,                /* 3.00 m */
	.evpe = 500,                /* 5.00 m */
	.hdop = 5,                  /* 1.0 */
	.vdop = 8,                  /* 1.6 */
};

static lns_navigation_val_t ln_navigation_val = {
	.flags = {
		.remaining_distance_present = 1,
		.remaining_vertical_distance_present = 1,
		.eta_present = 1,
		.position_status = LNS_POSITION_STATUS_POSITION_OK,
		.heading_source = 0,
		.navigation_indicator_type = 0,
		.waypoint_reached = 0,
		.destination_reached = 0,
	},
	.bearing = 4500,                    /* 45.00 deg */
	.heading = 9000,                    /* 90.00 deg */
	.remaining_distance = 5000,         /* 500.0 m */
	.remaining_vertical_distance = 1000,/* 10.00 m */
	.eta = {0xEA, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00},  /* 2026-01-01 01:00:00 */
};

/* Fix Rate (seconds, 0: fastest) and current selected route are single instances */
static uint8_t ln_fix_rate = 1;
static uint16_t ln_selected_route = 0;

/* Route demonstration data */
static const char *const ln_route_names[] = {
	"Route Home",
	"Route Work",
	"Route Gym",
};
#define LN_ROUTE_NUM    (sizeof(ln_route_names) / sizeof(ln_route_names[0]))

static uint8_t ln_loc_speed_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t ln_navigation_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t ln_cp_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t ln_cp_in_progress_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t ln_navigation_running_map[RTK_BLE_GAP_MAX_LINKS] = {0};
/* Location and Speed content mask per link (bit=1 turns off the field), not cached across connections */
static uint16_t ln_content_mask_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/* Periodic notify machinery. rtk_bt_gatts_notify is a synchronous blocking call and must
 * never run inside the software timer daemon; the timer only gives a semaphore, the
 * dedicated task performs the actual notifications. */
#define LN_NTF_TASK_STACK_SIZE      (256 * 4)
#define LN_NTF_TASK_PRIORITY        2
#define LN_NTF_PERIOD_MS            1000

static void *ln_ntf_timer_hdl = NULL;
static void *ln_ntf_sem = NULL;
static void *ln_ntf_task_hdl = NULL;
static uint8_t ln_ntf_task_running = 0;

static rtk_bt_gatt_attr_t lns_attrs[] = {
	/* idx 0 */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_LN_SRV),

	/* idx 1, 2 : LN Feature, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_LN_FEATURE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 3, 4 : Location and Speed, notify only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_LOCATION_AND_SPEED_CHAR,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	/* idx 5 : Location and Speed CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* idx 6, 7 : Position Quality, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_POSITION_QUALITY_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 8, 9 : LN Control Point, write & indicate */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_LN_CONTROL_POINT_CHAR,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* idx 10 : LN Control Point CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* idx 11, 12 : Navigation, notify only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_NAVIGATION_CHAR,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	/* idx 13 : Navigation CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service lns_srv = RTK_BT_GATT_SERVICE(lns_attrs, LOCATION_NAVIGATION_SRV_ID);

uint16_t lns_srv_set_params(lns_param_type_t type, uint8_t len, void *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		return RTK_BT_FAIL;
	}

	switch (type) {
	case LNS_PARAM_FEATURE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_feature_val, value, len);
		}
		break;

	case LNS_PARAM_LOC_SPEED_FLAG:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.flags, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_INST_SPEED:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.inst_speed, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_TOTAL_DISTANCE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.total_distance, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_LATITUDE:
		if (len != sizeof(int32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.latitude, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_LONGITUDE:
		if (len != sizeof(int32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.longitude, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_ELEVATION:
		if (len != sizeof(int32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.elevation, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_HEADING:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.heading, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_ROLLING_TIME:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_loc_speed_val.rolling_time, value, len);
		}
		break;
	case LNS_PARAM_LOC_SPEED_UTC_TIME:
		if (len != sizeof(ln_loc_speed_val.utc_time)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(ln_loc_speed_val.utc_time, value, len);
		}
		break;

	case LNS_PARAM_POS_QUALITY_FLAG:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.flags, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_BEACONS_IN_SOLUTION:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.beacons_in_solution, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_BEACONS_IN_VIEW:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.beacons_in_view, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_TIME_TO_FIRST_FIX:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.time_to_first_fix, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_EHPE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.ehpe, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_EVPE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.evpe, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_HDOP:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.hdop, value, len);
		}
		break;
	case LNS_PARAM_POS_QUALITY_VDOP:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_pos_quality_val.vdop, value, len);
		}
		break;

	case LNS_PARAM_NAVIGATION_FLAG:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_navigation_val.flags, value, len);
		}
		break;
	case LNS_PARAM_NAVIGATION_BEARING:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_navigation_val.bearing, value, len);
		}
		break;
	case LNS_PARAM_NAVIGATION_HEADING:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_navigation_val.heading, value, len);
		}
		break;
	case LNS_PARAM_NAVIGATION_REMAINING_DISTANCE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_navigation_val.remaining_distance, value, len);
		}
		break;
	case LNS_PARAM_NAVIGATION_REMAINING_VERTICAL_DISTANCE:
		if (len != sizeof(int32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&ln_navigation_val.remaining_vertical_distance, value, len);
		}
		break;
	case LNS_PARAM_NAVIGATION_ETA:
		if (len != sizeof(ln_navigation_val.eta)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(ln_navigation_val.eta, value, len);
		}
		break;

	default:
		ret = RTK_BT_FAIL;
		break;
	}

	return ret;
}

/* Location and Speed optional fields in spec order, for MTU-aware notification packing.
 * meta_mask carries the non-present Flags bits (Position Status / format / source) that
 * belong to a field so they travel in the same chunk; size is the serialized field size. */
#define LNS_LOC_SPEED_FIELD_NUM     7

static const struct {
	uint16_t present_bit;
	uint16_t meta_mask;
	uint8_t size;
} lns_loc_speed_fields[LNS_LOC_SPEED_FIELD_NUM] = {
	{ (1 << 0), (1 << 9),    2 },   /* Instantaneous Speed  (+ Speed and Distance Format) */
	{ (1 << 1), (1 << 9),    3 },   /* Total Distance       (+ Speed and Distance Format) */
	{ (1 << 2), (0x3 << 7),  8 },   /* Location lat+lon     (+ Position Status) */
	{ (1 << 3), (0x3 << 10), 3 },   /* Elevation            (+ Elevation Source) */
	{ (1 << 4), (1 << 12),   2 },   /* Heading              (+ Heading Source) */
	{ (1 << 5), 0,           1 },   /* Rolling Time */
	{ (1 << 6), 0,           7 },   /* UTC Time */
};

/* Serialize one Location and Speed optional field (little endian); returns bytes written. */
static uint8_t lns_loc_speed_pack_field(uint8_t idx, uint8_t *dst)
{
	lns_loc_speed_val_t *src = &ln_loc_speed_val;
	uint8_t len = 0;

	switch (idx) {
	case 0:     /* Instantaneous Speed, uint16 */
		dst[len++] = (uint8_t)(src->inst_speed & 0xFF);
		dst[len++] = (uint8_t)((src->inst_speed >> 8) & 0xFF);
		break;
	case 1:     /* Total Distance, uint24 */
		dst[len++] = (uint8_t)(src->total_distance & 0xFF);
		dst[len++] = (uint8_t)((src->total_distance >> 8) & 0xFF);
		dst[len++] = (uint8_t)((src->total_distance >> 16) & 0xFF);
		break;
	case 2: {   /* Location: Latitude sint32 + Longitude sint32 */
		uint32_t lat = (uint32_t)src->latitude;
		uint32_t lon = (uint32_t)src->longitude;
		dst[len++] = (uint8_t)(lat & 0xFF);
		dst[len++] = (uint8_t)((lat >> 8) & 0xFF);
		dst[len++] = (uint8_t)((lat >> 16) & 0xFF);
		dst[len++] = (uint8_t)((lat >> 24) & 0xFF);
		dst[len++] = (uint8_t)(lon & 0xFF);
		dst[len++] = (uint8_t)((lon >> 8) & 0xFF);
		dst[len++] = (uint8_t)((lon >> 16) & 0xFF);
		dst[len++] = (uint8_t)((lon >> 24) & 0xFF);
		break;
	}
	case 3: {   /* Elevation, sint24 */
		uint32_t elev = (uint32_t)src->elevation;
		dst[len++] = (uint8_t)(elev & 0xFF);
		dst[len++] = (uint8_t)((elev >> 8) & 0xFF);
		dst[len++] = (uint8_t)((elev >> 16) & 0xFF);
		break;
	}
	case 4:     /* Heading, uint16 */
		dst[len++] = (uint8_t)(src->heading & 0xFF);
		dst[len++] = (uint8_t)((src->heading >> 8) & 0xFF);
		break;
	case 5:     /* Rolling Time, uint8 */
		dst[len++] = src->rolling_time;
		break;
	case 6:     /* UTC Time, Date Time 7 octets */
		memcpy(&dst[len], src->utc_time, sizeof(src->utc_time));
		len += (uint8_t)sizeof(src->utc_time);
		break;
	default:
		break;
	}

	return len;
}

/* Regroup Position Quality to little-endian byte stream according to flags */
static uint8_t lns_pos_quality_value_regroup(uint8_t *dst)
{
	uint8_t len = 0;
	lns_pos_quality_val_t *src = &ln_pos_quality_val;

	memcpy(&dst[len], &src->flags, sizeof(uint16_t));
	len += sizeof(uint16_t);

	if (src->flags.beacons_in_solution_present) {
		dst[len++] = src->beacons_in_solution;
	}
	if (src->flags.beacons_in_view_present) {
		dst[len++] = src->beacons_in_view;
	}
	if (src->flags.time_to_first_fix_present) {
		dst[len++] = (uint8_t)(src->time_to_first_fix & 0xFF);
		dst[len++] = (uint8_t)((src->time_to_first_fix >> 8) & 0xFF);
	}
	if (src->flags.ehpe_present) {
		dst[len++] = (uint8_t)(src->ehpe & 0xFF);
		dst[len++] = (uint8_t)((src->ehpe >> 8) & 0xFF);
		dst[len++] = (uint8_t)((src->ehpe >> 16) & 0xFF);
		dst[len++] = (uint8_t)((src->ehpe >> 24) & 0xFF);
	}
	if (src->flags.evpe_present) {
		dst[len++] = (uint8_t)(src->evpe & 0xFF);
		dst[len++] = (uint8_t)((src->evpe >> 8) & 0xFF);
		dst[len++] = (uint8_t)((src->evpe >> 16) & 0xFF);
		dst[len++] = (uint8_t)((src->evpe >> 24) & 0xFF);
	}
	if (src->flags.hdop_present) {
		dst[len++] = src->hdop;
	}
	if (src->flags.vdop_present) {
		dst[len++] = src->vdop;
	}

	return len;
}

/* Regroup Navigation to little-endian byte stream. Bearing and Heading are Mandatory. */
static uint8_t lns_navigation_value_regroup(uint8_t *dst)
{
	uint8_t len = 0;
	lns_navigation_val_t *src = &ln_navigation_val;

	memcpy(&dst[len], &src->flags, sizeof(uint16_t));
	len += sizeof(uint16_t);

	dst[len++] = (uint8_t)(src->bearing & 0xFF);
	dst[len++] = (uint8_t)((src->bearing >> 8) & 0xFF);
	dst[len++] = (uint8_t)(src->heading & 0xFF);
	dst[len++] = (uint8_t)((src->heading >> 8) & 0xFF);

	if (src->flags.remaining_distance_present) {    /* uint24 */
		dst[len++] = (uint8_t)(src->remaining_distance & 0xFF);
		dst[len++] = (uint8_t)((src->remaining_distance >> 8) & 0xFF);
		dst[len++] = (uint8_t)((src->remaining_distance >> 16) & 0xFF);
	}
	if (src->flags.remaining_vertical_distance_present) {   /* sint24 */
		uint32_t rvd = (uint32_t)src->remaining_vertical_distance;
		dst[len++] = (uint8_t)(rvd & 0xFF);
		dst[len++] = (uint8_t)((rvd >> 8) & 0xFF);
		dst[len++] = (uint8_t)((rvd >> 16) & 0xFF);
	}
	if (src->flags.eta_present) {   /* Date Time, 7 octets */
		memcpy(&dst[len], src->eta, sizeof(src->eta));
		len += sizeof(src->eta);
	}

	return len;
}

/* Send Location and Speed notification(s). The value is split into ATT-MTU-sized chunks
 * (spec 3.2); each chunk carries its own Flags describing only the fields it contains, plus
 * the associated metadata bits, with RFU kept 0. */
static void lns_location_speed_notify(uint16_t conn_handle, uint8_t conn_id)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t value_buf[LNS_LOC_SPEED_VALUE_MAX_LEN];
	uint16_t full_flags = 0, eff_flags, present;
	uint16_t mtu = 0, max_val;
	uint8_t i;

	/* Configured flags with the per-link content mask applied (mask affects bits 0-6) */
	memcpy(&full_flags, &ln_loc_speed_val.flags, sizeof(uint16_t));
	eff_flags = full_flags & (uint16_t)(~(ln_content_mask_map[conn_id] & 0x007F));
	present = eff_flags & 0x007F;

	/* Max notification value = ATT_MTU - 3; fall back to the 23-octet default MTU */
	if (rtk_bt_le_gap_get_mtu_size(conn_handle, &mtu) != RTK_BT_OK || mtu < 23) {
		mtu = 23;
	}
	max_val = (uint16_t)(mtu - 3);
	if (max_val > LNS_LOC_SPEED_VALUE_MAX_LEN) {
		max_val = LNS_LOC_SPEED_VALUE_MAX_LEN;
	}

	ntf_param.app_id = LOCATION_NAVIGATION_SRV_ID;
	ntf_param.conn_handle = conn_handle;
	ntf_param.index = LOCATION_AND_SPEED_CHAR_VAL_INDEX;
	ntf_param.data = value_buf;
	ntf_param.seq = 0;

	/* No optional field present: a single 2-octet Flags-only notification */
	if (present == 0) {
		value_buf[0] = (uint8_t)(eff_flags & 0xFF);
		value_buf[1] = (uint8_t)((eff_flags >> 8) & 0xFF);
		ntf_param.len = 2;
		rtk_bt_gatts_notify(&ntf_param);
		return;
	}

	i = 0;
	while (i < LNS_LOC_SPEED_FIELD_NUM) {
		uint16_t chunk_flags = 0;
		uint8_t len = 2;    /* reserve 2 octets for Flags */

		while (i < LNS_LOC_SPEED_FIELD_NUM) {
			if (!(present & lns_loc_speed_fields[i].present_bit)) {
				i++;
				continue;
			}
			if ((uint16_t)(len + lns_loc_speed_fields[i].size) > max_val) {
				break;      /* does not fit in this chunk; leave for the next one */
			}
			len += lns_loc_speed_pack_field(i, &value_buf[len]);
			chunk_flags |= lns_loc_speed_fields[i].present_bit;
			chunk_flags |= (uint16_t)(full_flags & lns_loc_speed_fields[i].meta_mask);
			i++;
		}

		if (len == 2) {
			/* Defensive: a present field cannot fit even alone (impossible at MTU >= 23);
			 * skip it to avoid stalling. */
			i++;
			continue;
		}

		value_buf[0] = (uint8_t)(chunk_flags & 0xFF);
		value_buf[1] = (uint8_t)((chunk_flags >> 8) & 0xFF);
		ntf_param.len = len;
		rtk_bt_gatts_notify(&ntf_param);
	}
}

static void lns_navigation_notify(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t value_buf[LNS_NAVIGATION_VALUE_MAX_LEN] = {0};
	uint8_t actual_len;

	actual_len = lns_navigation_value_regroup(value_buf);
	ntf_param.app_id = LOCATION_NAVIGATION_SRV_ID;
	ntf_param.conn_handle = conn_handle;
	ntf_param.index = NAVIGATION_CHAR_VAL_INDEX;
	ntf_param.data = value_buf;
	ntf_param.len = actual_len;
	ntf_param.seq = 0;
	rtk_bt_gatts_notify(&ntf_param);
}

static void lns_ntf_timer_callback(void *arg)
{
	(void)arg;
	if (ln_ntf_sem) {
		osif_sem_give(ln_ntf_sem);
	}
}

static void lns_ntf_task_entry(void *param)
{
	(void)param;
	rtk_bt_le_get_active_conn_t active_conn;
	uint8_t conn_id;
	uint16_t i;

	while (ln_ntf_task_running) {
		if (!osif_sem_take(ln_ntf_sem, BT_TIMEOUT_FOREVER)) {
			continue;
		}
		if (!ln_ntf_task_running) {
			break;
		}
		memset(&active_conn, 0, sizeof(active_conn));
		if (rtk_bt_le_gap_get_active_conn(&active_conn) != RTK_BT_OK) {
			continue;
		}
		for (i = 0; i < active_conn.conn_num; i++) {
			if (rtk_bt_le_gap_get_conn_id(active_conn.conn_handle[i], &conn_id) != RTK_BT_OK) {
				continue;
			}
			if (ln_loc_speed_cccd_ntf_en_map[conn_id]) {
				lns_location_speed_notify(active_conn.conn_handle[i], conn_id);
			}
			if (ln_navigation_cccd_ntf_en_map[conn_id] && ln_navigation_running_map[conn_id]) {
				lns_navigation_notify(active_conn.conn_handle[i]);
			}
		}
	}
	osif_task_delete(NULL);
}

static void lns_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint8_t feature_buf[4];
	uint8_t pq_buf[LNS_POS_QUALITY_VALUE_MAX_LEN] = {0};

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	if (LN_FEATURE_CHAR_VAL_INDEX == p_read_ind->index) {
		feature_buf[0] = (uint8_t)(ln_feature_val & 0xFF);
		feature_buf[1] = (uint8_t)((ln_feature_val >> 8) & 0xFF);
		feature_buf[2] = (uint8_t)((ln_feature_val >> 16) & 0xFF);
		feature_buf[3] = (uint8_t)((ln_feature_val >> 24) & 0xFF);
		read_resp.data = feature_buf;
		read_resp.len = sizeof(feature_buf);
	} else if (POSITION_QUALITY_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = pq_buf;
		read_resp.len = lns_pos_quality_value_regroup(pq_buf);
	} else {
		BT_LOGE("[APP] LNS read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] LNS response for client read succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE("[APP] LNS response for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index,
				read_resp.err_code);
}

/* Build & send LN Control Point Response Code(0x20) indicate */
static void lns_cp_response(uint16_t conn_handle, uint8_t request_op_code,
							uint8_t response_value, uint8_t *resp_param, uint8_t param_len)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t buf[LN_CP_RESP_MAX_LEN] = {0};
	uint8_t len = 0;

	buf[len++] = LN_CP_OPCODE_RESPONSE_CODE;
	buf[len++] = request_op_code;
	buf[len++] = response_value;

	if (resp_param && param_len) {
		if (param_len > (LN_CP_RESP_MAX_LEN - len)) {
			param_len = LN_CP_RESP_MAX_LEN - len;
		}
		memcpy(&buf[len], resp_param, param_len);
		len += param_len;
	}

	ind_param.app_id = LOCATION_NAVIGATION_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = LN_CONTROL_POINT_CHAR_VAL_INDEX;
	ind_param.data = buf;
	ind_param.len = len;
	rtk_bt_gatts_indicate(&ind_param);

	BT_LOGA("[APP] LNS CP resp, req_op: %d, resp_val: %d\r\n", request_op_code, response_value);
}

/* Execute a LN Control Point op code (synchronous, single completion) */
static void lns_cp_proc(uint16_t conn_handle, uint8_t conn_id, uint8_t *value, uint16_t len)
{
	uint8_t op_code = value[0];
	uint8_t resp_value = LN_CP_RESP_SUCCESS;

	switch (op_code) {
	case LN_CP_OPCODE_SET_CUMULATIVE_VALUE:
		if (!(ln_feature_val & LNS_FEAT_TOTAL_DISTANCE)) {
			resp_value = LN_CP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + 3) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			ln_loc_speed_val.total_distance = (uint32_t)value[1] |
											  ((uint32_t)value[2] << 8) |
											  ((uint32_t)value[3] << 16);
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case LN_CP_OPCODE_MASK_LOC_SPEED_CONTENT:
		if (!(ln_feature_val & LNS_FEAT_LOC_SPEED_CONTENT_MASKING)) {
			resp_value = LN_CP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + 2) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			ln_content_mask_map[conn_id] = (uint16_t)value[1] | ((uint16_t)value[2] << 8);
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case LN_CP_OPCODE_NAVIGATION_CONTROL:
		if (len < 1 + 1) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			switch (value[1]) {
			case LN_NAV_CTRL_START:
			case LN_NAV_CTRL_CONTINUE:
			case LN_NAV_CTRL_SELECT_NEAREST_WAYPOINT:
				ln_navigation_running_map[conn_id] = 1;
				break;
			case LN_NAV_CTRL_STOP:
			case LN_NAV_CTRL_PAUSE:
				ln_navigation_running_map[conn_id] = 0;
				break;
			case LN_NAV_CTRL_SKIP_WAYPOINT:
				/* running state unchanged */
				break;
			default:
				resp_value = LN_CP_RESP_INVALID_OPERAND;
				break;
			}
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES: {
		uint8_t resp_param[2];
		uint16_t num = (uint16_t)LN_ROUTE_NUM;
		resp_param[0] = (uint8_t)(num & 0xFF);
		resp_param[1] = (uint8_t)((num >> 8) & 0xFF);
		lns_cp_response(conn_handle, op_code, LN_CP_RESP_SUCCESS, resp_param, sizeof(resp_param));
		break;
	}

	case LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE:
		if (len < 1 + 2) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
			lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		} else {
			uint16_t route_no = (uint16_t)value[1] | ((uint16_t)value[2] << 8);
			if (route_no >= LN_ROUTE_NUM) {
				lns_cp_response(conn_handle, op_code, LN_CP_RESP_INVALID_OPERAND, NULL, 0);
			} else {
				const char *name = ln_route_names[route_no];
				lns_cp_response(conn_handle, op_code, LN_CP_RESP_SUCCESS,
								(uint8_t *)name, (uint8_t)strlen(name));
			}
		}
		break;

	case LN_CP_OPCODE_SELECT_ROUTE:
		if (len < 1 + 2) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			uint16_t route_no = (uint16_t)value[1] | ((uint16_t)value[2] << 8);
			if (route_no >= LN_ROUTE_NUM) {
				resp_value = LN_CP_RESP_INVALID_OPERAND;
			} else {
				ln_selected_route = route_no;
			}
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case LN_CP_OPCODE_SET_FIX_RATE:
		if (!(ln_feature_val & LNS_FEAT_FIX_RATE_SETTING)) {
			resp_value = LN_CP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + 1) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			ln_fix_rate = value[1];
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case LN_CP_OPCODE_SET_ELEVATION:
		if (!(ln_feature_val & LNS_FEAT_ELEVATION_SETTING)) {
			resp_value = LN_CP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + 3) {
			resp_value = LN_CP_RESP_INVALID_OPERAND;
		} else {
			int32_t elev = (int32_t)((uint32_t)value[1] |
									 ((uint32_t)value[2] << 8) |
									 ((uint32_t)value[3] << 16));
			if (elev & 0x00800000) {
				elev |= (int32_t)0xFF000000;    /* sign extend sint24 */
			}
			ln_loc_speed_val.elevation = elev;
		}
		lns_cp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	default:
		lns_cp_response(conn_handle, op_code, LN_CP_RESP_OP_CODE_NOT_SUPPORTED, NULL, 0);
		break;
	}
}

static void lns_write_hdl(void *data)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint8_t conn_id = 0;
	bool do_proc = false;

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (LN_CONTROL_POINT_CHAR_VAL_INDEX == p_write_ind->index) {
		if (!p_write_ind->len || !p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		} else if (rtk_bt_le_gap_get_conn_id(p_write_ind->conn_handle, &conn_id) != RTK_BT_OK) {
			write_resp.err_code = RTK_BT_ATT_ERR_UNLIKELY;
		} else if (0 == ln_cp_cccd_ind_en_map[conn_id]) {
			/* LN Control Point CCCD not configured for indicate */
			write_resp.err_code = LN_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
		} else if (ln_cp_in_progress_map[conn_id]) {
			/* a procedure is already in progress */
			write_resp.err_code = LN_ATT_ERR_PROC_ALREADY_IN_PROGRESS;
		} else {
			ln_cp_in_progress_map[conn_id] = 1;
			do_proc = true;
		}
	} else {
		BT_LOGE("[APP] LNS write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] LNS response for client write succeed, index: %d\r\n", p_write_ind->index);
	} else {
		BT_LOGE("[APP] LNS response for client write failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);

	if (do_proc) {
		/* The procedure stays in progress until its Response Code indication is
		 * confirmed (cleared in RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND), so a
		 * concurrent write is rejected with Procedure Already In Progress. */
		lns_cp_proc(p_write_ind->conn_handle, conn_id, p_write_ind->value, p_write_ind->len);
	}
}

static void lns_cccd_update_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	switch (p_cccd_ind->index) {
	case LOCATION_AND_SPEED_CHAR_CCCD_INDEX:
		ln_loc_speed_cccd_ntf_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) ? 1 : 0;
		BT_LOGA("[APP] LNS location and speed cccd, notify bit: %d\r\n", ln_loc_speed_cccd_ntf_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					ln_loc_speed_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	case LN_CONTROL_POINT_CHAR_CCCD_INDEX:
		ln_cp_cccd_ind_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) ? 1 : 0;
		BT_LOGA("[APP] LNS control point cccd, indicate bit: %d\r\n", ln_cp_cccd_ind_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					ln_cp_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	case NAVIGATION_CHAR_CCCD_INDEX:
		ln_navigation_cccd_ntf_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) ? 1 : 0;
		BT_LOGA("[APP] LNS navigation cccd, notify bit: %d\r\n", ln_navigation_cccd_ntf_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					ln_navigation_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	default:
		BT_LOGE("[APP] LNS CCCD event unknown index: %d\r\n", p_cccd_ind->index);
		BT_AT_PRINT("+BLEGATTS:cccd,unknown_index\r\n");
		break;
	}
}

void lns_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] LNS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] LNS register service failed: %d\r\n", reg_srv_res->reg_status);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		uint8_t conn_id;
		if (RTK_BT_OK == p_ind_ind->err_code) {
			BT_LOGA("[APP] LNS indicate succeed!\r\n");
		} else {
			BT_LOGE("[APP] LNS indicate failed\r\n");
		}
		/* A LN Control Point procedure completes when its Response Code
		 * indication is confirmed (or fails); release the in-progress lock. */
		if (rtk_bt_le_gap_get_conn_id(p_ind_ind->conn_handle, &conn_id) == RTK_BT_OK) {
			ln_cp_in_progress_map[conn_id] = 0;
		}
		BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ind_ind->err_code) ? 0 : -1, p_ind_ind->app_id,
					p_ind_ind->conn_handle, p_ind_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ntf_ind->err_code) {
			BT_LOGA("[APP] LNS notify succeed!\r\n");
		} else {
			BT_LOGE("[APP] LNS notify failed, local error\r\n");
		}
		BT_AT_PRINT("+BLEGATTS:notify,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ntf_ind->err_code) ? 0 : -1, p_ntf_ind->app_id,
					p_ntf_ind->conn_handle, p_ntf_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND:
		lns_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		lns_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		lns_cccd_update_hdl(data);
		break;

	default:
		break;
	}
}

uint16_t lns_srv_add(void)
{
	lns_srv.type = GATT_SERVICE_OVER_BLE;
	lns_srv.server_info = 0;
	lns_srv.user_data = NULL;
	lns_srv.register_status = 0;

	/* Create the periodic (1 s) notify resources once and start the timer */
	if (ln_ntf_sem == NULL) {
		osif_sem_create(&ln_ntf_sem, 0, 1);
	}
	if (ln_ntf_timer_hdl == NULL) {
		osif_timer_create(&ln_ntf_timer_hdl, "lns_ntf_timer", 0, LN_NTF_PERIOD_MS, true, lns_ntf_timer_callback);
	}
	if (ln_ntf_task_hdl == NULL) {
		ln_ntf_task_running = 1;
		osif_task_create(&ln_ntf_task_hdl, "lns_ntf_task", lns_ntf_task_entry, NULL,
						 LN_NTF_TASK_STACK_SIZE, LN_NTF_TASK_PRIORITY);
	}
	if (ln_ntf_timer_hdl) {
		osif_timer_start(&ln_ntf_timer_hdl);
	}

	return rtk_bt_gatts_register_service(&lns_srv);
}

void lns_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	ln_loc_speed_cccd_ntf_en_map[conn_id] = 0;
	ln_navigation_cccd_ntf_en_map[conn_id] = 0;
	ln_cp_cccd_ind_en_map[conn_id] = 0;
	ln_cp_in_progress_map[conn_id] = 0;
	ln_navigation_running_map[conn_id] = 0;
	ln_content_mask_map[conn_id] = 0;    /* content mask must not be cached across connections */
}

void lns_status_deinit(void)
{
	memset(ln_loc_speed_cccd_ntf_en_map, 0, sizeof(ln_loc_speed_cccd_ntf_en_map));
	memset(ln_navigation_cccd_ntf_en_map, 0, sizeof(ln_navigation_cccd_ntf_en_map));
	memset(ln_cp_cccd_ind_en_map, 0, sizeof(ln_cp_cccd_ind_en_map));
	memset(ln_cp_in_progress_map, 0, sizeof(ln_cp_in_progress_map));
	memset(ln_navigation_running_map, 0, sizeof(ln_navigation_running_map));
	memset(ln_content_mask_map, 0, sizeof(ln_content_mask_map));

	/* Stop and delete the periodic timer, then signal the task to exit. The task will
	 * break out of its loop (running == 0) and call osif_task_delete(NULL) itself.
	 * The semaphore is intentionally left alive to avoid a use-after-free race with
	 * the task that may still be waking up from osif_sem_take(). */
	if (ln_ntf_timer_hdl) {
		osif_timer_stop(&ln_ntf_timer_hdl);
		osif_timer_delete(&ln_ntf_timer_hdl);
	}
	if (ln_ntf_task_hdl) {
		ln_ntf_task_running = 0;
		if (ln_ntf_sem) {
			osif_sem_give(ln_ntf_sem);
		}
		ln_ntf_task_hdl = NULL;
	}
}
