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
#include <rtk_rscs.h>
#include <bt_utils.h>

/* RSCS UUIDs (spec) */
#define RSC_SRV_UUID                            0x1814
#define RSC_MEASUREMENT_CHAR_UUID               0x2A53
#define RSC_FEATURE_CHAR_UUID                   0x2A54
#define RSC_SENSOR_LOCATION_CHAR_UUID           0x2A5D
#define RSC_SC_CONTROL_POINT_CHAR_UUID          0x2A55

#define RTK_BT_UUID_RSC_SRV                     BT_UUID_DECLARE_16(RSC_SRV_UUID)
#define RTK_BT_UUID_RSC_MEASUREMENT_CHAR        BT_UUID_DECLARE_16(RSC_MEASUREMENT_CHAR_UUID)
#define RTK_BT_UUID_RSC_FEATURE_CHAR            BT_UUID_DECLARE_16(RSC_FEATURE_CHAR_UUID)
#define RTK_BT_UUID_RSC_SENSOR_LOCATION_CHAR    BT_UUID_DECLARE_16(RSC_SENSOR_LOCATION_CHAR_UUID)
#define RTK_BT_UUID_RSC_SC_CONTROL_POINT_CHAR   BT_UUID_DECLARE_16(RSC_SC_CONTROL_POINT_CHAR_UUID)

/* Attribute indexes (verified against RTK_BT_GATT_CHARACTERISTIC macro expansion:
 * each CHARACTERISTIC expands to declaration + value, so value index = decl + 1) */
#define RSC_MEASUREMENT_CHAR_VAL_INDEX          (2)   /* notify */
#define RSC_MEASUREMENT_CHAR_CCCD_INDEX         (3)   /* cccd notify */
#define RSC_FEATURE_CHAR_VAL_INDEX              (5)   /* read */
#define RSC_SENSOR_LOCATION_CHAR_VAL_INDEX      (7)   /* read */
#define RSC_SC_CONTROL_POINT_CHAR_VAL_INDEX     (9)   /* write, indicate */
#define RSC_SC_CONTROL_POINT_CHAR_CCCD_INDEX    (10)  /* cccd indicate */

/* SC Control Point Op Codes
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define RSC_SCCP_OPCODE_SET_CUMULATIVE_VALUE        0x01
#define RSC_SCCP_OPCODE_START_SENSOR_CALIBRATION    0x02
#define RSC_SCCP_OPCODE_UPDATE_SENSOR_LOCATION      0x03
#define RSC_SCCP_OPCODE_REQUEST_SUPPORTED_LOCATIONS 0x04
#define RSC_SCCP_OPCODE_RESPONSE_CODE               0x10

/* SC Control Point Response Value
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define RSC_SCCP_RESP_SUCCESS                       0x01
#define RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED         0x02
#define RSC_SCCP_RESP_INVALID_PARAMETER             0x03
#define RSC_SCCP_RESP_OPERATION_FAILED              0x04

/* Application error codes
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define RSC_ATT_ERR_PROC_ALREADY_IN_PROGRESS        0x80
#define RSC_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED      0x81

/* Sensor Location enumeration
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define RSC_SENSOR_LOC_OTHER                        0x00
#define RSC_SENSOR_LOC_TOP_OF_SHOE                  0x01
#define RSC_SENSOR_LOC_IN_SHOE                      0x02
#define RSC_SENSOR_LOC_HIP                          0x03
#define RSC_SENSOR_LOC_CHEST                        0x0E

/* SCCP indicate payload max: op_code(1) + req_op_code(1) + resp_value(1) + params */
#define RSC_SCCP_RESP_MAX_LEN                       16

static rscs_meas_val_t rsc_measurement_val = {
	.flags = {
		.inst_stride_len_present = 1,
		.total_distance_present = 1,
		.walking_or_running = 1,
	},
	.inst_speed = 256,       /* 1 m/s */
	.inst_cadence = 90,
	.inst_stride_len = 100,  /* 1 m */
	.total_distance = 0,
};

/* Enable all 5 feature bits by default (0x001F) */
static uint16_t rsc_feature_val = RSCS_FEAT_INST_STRIDE_LEN | RSCS_FEAT_TOTAL_DISTANCE |
								   RSCS_FEAT_WALK_OR_RUN | RSCS_FEAT_CALIBRATION |
								   RSCS_FEAT_MULTI_SENSOR_LOC;

static uint8_t rsc_sensor_location_val = RSC_SENSOR_LOC_IN_SHOE;

/* Sensor calibration outcome (spec 3.4.2.2), reported by the sensor layer via
 * rscs_set_calibration_result(). Default Success; change the default to
 * RSC_SCCP_RESP_OPERATION_FAILED to run RSCS/SEN/SPE/BI-03-C (calibration error). */
static uint8_t rsc_cal_result = RSC_SCCP_RESP_SUCCESS;

/* Supported sensor location list (subset of enumeration) */
static const uint8_t rsc_supported_sensor_locations[] = {
	RSC_SENSOR_LOC_OTHER,
	RSC_SENSOR_LOC_TOP_OF_SHOE,
	RSC_SENSOR_LOC_IN_SHOE,
	RSC_SENSOR_LOC_HIP,
	RSC_SENSOR_LOC_CHEST,
};

static uint8_t rsc_msmnt_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t rsc_sccp_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t rsc_sccp_in_progress_map[RTK_BLE_GAP_MAX_LINKS] = {0};

static rtk_bt_gatt_attr_t rscs_attrs[] = {
	/* idx 0 */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_RSC_SRV),

	/* idx 1, 2 : RSC Measurement, notify only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_RSC_MEASUREMENT_CHAR,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	/* idx 3 : Measurement CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* idx 4, 5 : RSC Feature, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_RSC_FEATURE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 6, 7 : Sensor Location, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_RSC_SENSOR_LOCATION_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 8, 9 : SC Control Point, write & indicate */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_RSC_SC_CONTROL_POINT_CHAR,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* idx 10 : SC Control Point CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service rsc_srv = RTK_BT_GATT_SERVICE(rscs_attrs, RUNNING_SPEED_CADENCE_SRV_ID);

uint16_t rscs_srv_set_params(rscs_param_type_t type, uint8_t len, void *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		return RTK_BT_FAIL;
	}

	switch (type) {
	case RSCS_PARAM_MEAS_FLAG:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_measurement_val.flags, value, sizeof(uint8_t));
		}
		break;

	case RSCS_PARAM_MEAS_INST_SPEED:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_measurement_val.inst_speed, value, len);
		}
		break;

	case RSCS_PARAM_MEAS_INST_CADENCE:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_measurement_val.inst_cadence, value, len);
		}
		break;

	case RSCS_PARAM_MEAS_INST_STRIDE_LEN:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_measurement_val.inst_stride_len, value, len);
		}
		break;

	case RSCS_PARAM_MEAS_TOTAL_DISTANCE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_measurement_val.total_distance, value, len);
		}
		break;

	case RSCS_PARAM_FEATURE:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_feature_val, value, len);
		}
		break;

	case RSCS_PARAM_SENSOR_LOCATION:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&rsc_sensor_location_val, value, len);
		}
		break;

	default:
		ret = RTK_BT_FAIL;
		break;
	}

	return ret;
}

/* Regroup RSC Measurement to little-endian byte stream:
 * flags(1) -> inst_speed(2) -> inst_cadence(1) ->
 * [inst_stride_len(2) if flag.bit0] -> [total_distance(4) if flag.bit1] */
static uint8_t rscs_meas_value_regroup(uint8_t *dst)
{
	uint8_t actual_len = 0;
	rscs_meas_val_t *src = &rsc_measurement_val;

	memcpy(&dst[actual_len], &src->flags, sizeof(uint8_t));
	actual_len += sizeof(uint8_t);

	dst[actual_len++] = (uint8_t)(src->inst_speed & 0xFF);
	dst[actual_len++] = (uint8_t)((src->inst_speed >> 8) & 0xFF);

	dst[actual_len++] = src->inst_cadence;

	if (src->flags.inst_stride_len_present) {
		dst[actual_len++] = (uint8_t)(src->inst_stride_len & 0xFF);
		dst[actual_len++] = (uint8_t)((src->inst_stride_len >> 8) & 0xFF);
	}

	if (src->flags.total_distance_present) {
		dst[actual_len++] = (uint8_t)(src->total_distance & 0xFF);
		dst[actual_len++] = (uint8_t)((src->total_distance >> 8) & 0xFF);
		dst[actual_len++] = (uint8_t)((src->total_distance >> 16) & 0xFF);
		dst[actual_len++] = (uint8_t)((src->total_distance >> 24) & 0xFF);
	}

	return actual_len;
}

static bool rscs_sensor_location_supported(uint8_t loc)
{
	uint8_t i;
	for (i = 0; i < sizeof(rsc_supported_sensor_locations); i++) {
		if (rsc_supported_sensor_locations[i] == loc) {
			return true;
		}
	}
	return false;
}

static void rscs_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint8_t feature_buf[2];

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	if (RSC_FEATURE_CHAR_VAL_INDEX == p_read_ind->index) {
		feature_buf[0] = (uint8_t)(rsc_feature_val & 0xFF);
		feature_buf[1] = (uint8_t)((rsc_feature_val >> 8) & 0xFF);
		read_resp.data = feature_buf;
		read_resp.len = sizeof(feature_buf);
	} else if (RSC_SENSOR_LOCATION_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = &rsc_sensor_location_val;
		read_resp.len = sizeof(rsc_sensor_location_val);
	} else {
		BT_LOGE("[APP] RSCS read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] RSCS response for client read succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE("[APP] RSCS response for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index,
				read_resp.err_code);
}

/* Build & send SC Control Point Response Code(0x10) indicate */
static void rscs_sccp_response(uint16_t conn_handle, uint8_t request_op_code,
							   uint8_t response_value, uint8_t *resp_param, uint8_t param_len)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t buf[RSC_SCCP_RESP_MAX_LEN] = {0};
	uint8_t len = 0;

	buf[len++] = RSC_SCCP_OPCODE_RESPONSE_CODE;
	buf[len++] = request_op_code;
	buf[len++] = response_value;

	if (resp_param && param_len) {
		if (param_len > (RSC_SCCP_RESP_MAX_LEN - len)) {
			param_len = RSC_SCCP_RESP_MAX_LEN - len;
		}
		memcpy(&buf[len], resp_param, param_len);
		len += param_len;
	}

	ind_param.app_id = RUNNING_SPEED_CADENCE_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = RSC_SC_CONTROL_POINT_CHAR_VAL_INDEX;
	ind_param.data = buf;
	ind_param.len = len;
	rtk_bt_gatts_indicate(&ind_param);

	BT_LOGA("[APP] RSCS SCCP resp, req_op: %d, resp_val: %d\r\n", request_op_code, response_value);
}

/* Execute a SC Control Point op code (synchronous, single completion) */
static void rscs_sccp_proc(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t op_code = value[0];
	uint8_t resp_value = RSC_SCCP_RESP_SUCCESS;

	switch (op_code) {
	case RSC_SCCP_OPCODE_SET_CUMULATIVE_VALUE:
		if (!(rsc_feature_val & RSCS_FEAT_TOTAL_DISTANCE)) {
			resp_value = RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + sizeof(uint32_t)) {
			resp_value = RSC_SCCP_RESP_INVALID_PARAMETER;
		} else {
			uint32_t total = (uint32_t)value[1] | ((uint32_t)value[2] << 8) |
							 ((uint32_t)value[3] << 16) | ((uint32_t)value[4] << 24);
			rsc_measurement_val.total_distance = total;
		}
		rscs_sccp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case RSC_SCCP_OPCODE_START_SENSOR_CALIBRATION:
		if (!(rsc_feature_val & RSCS_FEAT_CALIBRATION)) {
			resp_value = RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED;
		} else {
			/* Outcome comes from the sensor calibration interface
			 * (rscs_set_calibration_result); spec 3.4.2.2. */
			resp_value = rsc_cal_result;
		}
		rscs_sccp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case RSC_SCCP_OPCODE_UPDATE_SENSOR_LOCATION:
		if (!(rsc_feature_val & RSCS_FEAT_MULTI_SENSOR_LOC)) {
			resp_value = RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + sizeof(uint8_t)) {
			resp_value = RSC_SCCP_RESP_INVALID_PARAMETER;
		} else if (!rscs_sensor_location_supported(value[1])) {
			resp_value = RSC_SCCP_RESP_INVALID_PARAMETER;
		} else {
			rsc_sensor_location_val = value[1];
		}
		rscs_sccp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case RSC_SCCP_OPCODE_REQUEST_SUPPORTED_LOCATIONS:
		if (!(rsc_feature_val & RSCS_FEAT_MULTI_SENSOR_LOC)) {
			rscs_sccp_response(conn_handle, op_code, RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED, NULL, 0);
		} else {
			rscs_sccp_response(conn_handle, op_code, RSC_SCCP_RESP_SUCCESS,
							   (uint8_t *)rsc_supported_sensor_locations,
							   sizeof(rsc_supported_sensor_locations));
		}
		break;

	default:
		rscs_sccp_response(conn_handle, op_code, RSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED, NULL, 0);
		break;
	}
}

static void rscs_write_hdl(void *data)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint8_t conn_id;
	bool do_proc = false;

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (RSC_SC_CONTROL_POINT_CHAR_VAL_INDEX == p_write_ind->index) {
		if (!p_write_ind->len || !p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		} else if (rtk_bt_le_gap_get_conn_id(p_write_ind->conn_handle, &conn_id) != RTK_BT_OK) {
			write_resp.err_code = RTK_BT_ATT_ERR_UNLIKELY;
		} else if (0 == rsc_sccp_cccd_ind_en_map[conn_id]) {
			/* SCCP CCCD not configured for indicate */
			write_resp.err_code = RSC_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
		} else if (rsc_sccp_in_progress_map[conn_id]) {
			/* a procedure is already in progress */
			write_resp.err_code = RSC_ATT_ERR_PROC_ALREADY_IN_PROGRESS;
		} else {
			rsc_sccp_in_progress_map[conn_id] = 1;
			do_proc = true;
		}
	} else {
		BT_LOGE("[APP] RSCS write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] RSCS response for client write succeed, index: %d\r\n", p_write_ind->index);
	} else {
		BT_LOGE("[APP] RSCS response for client write failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);

	if (do_proc) {
		/* The procedure stays in progress until its Response Code indication is
		 * confirmed (cleared in RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND), so a
		 * concurrent write is rejected with Procedure Already In Progress. */
		rscs_sccp_proc(p_write_ind->conn_handle, p_write_ind->value, p_write_ind->len);
	}
}

static void rscs_cccd_update_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (RSC_MEASUREMENT_CHAR_CCCD_INDEX == p_cccd_ind->index) {
		rsc_msmnt_cccd_ntf_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) ? 1 : 0;
		BT_LOGA("[APP] RSCS measurement cccd, notify bit: %d\r\n", rsc_msmnt_cccd_ntf_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					rsc_msmnt_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
	} else if (RSC_SC_CONTROL_POINT_CHAR_CCCD_INDEX == p_cccd_ind->index) {
		rsc_sccp_cccd_ind_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) ? 1 : 0;
		BT_LOGA("[APP] RSCS sccp cccd, indicate bit: %d\r\n", rsc_sccp_cccd_ind_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					rsc_sccp_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
	} else {
		BT_LOGE("[APP] RSCS CCCD event unknown index: %d\r\n", p_cccd_ind->index);
		BT_AT_PRINT("+BLEGATTS:cccd,unknown_index\r\n");
	}
}

void rscs_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] RSCS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] RSCS register service failed: %d\r\n", reg_srv_res->reg_status);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		uint8_t conn_id;
		if (RTK_BT_OK == p_ind_ind->err_code) {
			BT_LOGA("[APP] RSCS indicate succeed!\r\n");
		} else {
			BT_LOGE("[APP] RSCS indicate failed\r\n");
		}
		/* A SC Control Point procedure completes when its Response Code
		 * indication is confirmed (or fails); release the in-progress lock. */
		if (rtk_bt_le_gap_get_conn_id(p_ind_ind->conn_handle, &conn_id) == RTK_BT_OK) {
			rsc_sccp_in_progress_map[conn_id] = 0;
		}
		BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ind_ind->err_code) ? 0 : -1, p_ind_ind->app_id,
					p_ind_ind->conn_handle, p_ind_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ntf_ind->err_code) {
			BT_LOGA("[APP] RSCS notify succeed!\r\n");
		} else {
			BT_LOGE("[APP] RSCS notify failed, local error\r\n");
		}
		BT_AT_PRINT("+BLEGATTS:notify,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ntf_ind->err_code) ? 0 : -1, p_ntf_ind->app_id,
					p_ntf_ind->conn_handle, p_ntf_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND:
		rscs_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		rscs_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		rscs_cccd_update_hdl(data);
		break;

	default:
		break;
	}
}

void rscs_measurement_notify(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t meas_buf[RSCS_MEASUREMENT_VALUE_MAX_LEN] = {0};
	uint8_t actual_len = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (rsc_msmnt_cccd_ntf_en_map[conn_id]) {
		actual_len = rscs_meas_value_regroup(meas_buf);
		ntf_param.app_id = RUNNING_SPEED_CADENCE_SRV_ID;
		ntf_param.conn_handle = conn_handle;
		ntf_param.index = RSC_MEASUREMENT_CHAR_VAL_INDEX;
		ntf_param.data = meas_buf;
		ntf_param.len = actual_len;
		ntf_param.seq = 0;
		rtk_bt_gatts_notify(&ntf_param);
	}
}

/* Sensor calibration interface: the sensor layer reports the calibration
 * outcome as the SC Control Point Response Value, which is returned in the
 * Start Sensor Calibration Response Code (spec 3.4.2.2). */
void rscs_set_calibration_result(uint8_t response_value)
{
	rsc_cal_result = response_value;
}

uint16_t rscs_srv_add(void)
{
	rsc_srv.type = GATT_SERVICE_OVER_BLE;
	rsc_srv.server_info = 0;
	rsc_srv.user_data = NULL;
	rsc_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&rsc_srv);
}

void rscs_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	rsc_msmnt_cccd_ntf_en_map[conn_id] = 0;
	rsc_sccp_cccd_ind_en_map[conn_id] = 0;
	rsc_sccp_in_progress_map[conn_id] = 0;
}

void rscs_status_deinit(void)
{
	memset(rsc_msmnt_cccd_ntf_en_map, 0, sizeof(rsc_msmnt_cccd_ntf_en_map));
	memset(rsc_sccp_cccd_ind_en_map, 0, sizeof(rsc_sccp_cccd_ind_en_map));
	memset(rsc_sccp_in_progress_map, 0, sizeof(rsc_sccp_in_progress_map));
}
