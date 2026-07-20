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
#include <rtk_cscs.h>
#include <bt_utils.h>

/* CSCS UUIDs (spec) */
#define CSC_SRV_UUID                            0x1816
#define CSC_MEASUREMENT_CHAR_UUID               0x2A5B
#define CSC_FEATURE_CHAR_UUID                   0x2A5C
#define CSC_SENSOR_LOCATION_CHAR_UUID           0x2A5D
#define CSC_SC_CONTROL_POINT_CHAR_UUID          0x2A55

#define RTK_BT_UUID_CSC_SRV                     BT_UUID_DECLARE_16(CSC_SRV_UUID)
#define RTK_BT_UUID_CSC_MEASUREMENT_CHAR        BT_UUID_DECLARE_16(CSC_MEASUREMENT_CHAR_UUID)
#define RTK_BT_UUID_CSC_FEATURE_CHAR            BT_UUID_DECLARE_16(CSC_FEATURE_CHAR_UUID)
#define RTK_BT_UUID_CSC_SENSOR_LOCATION_CHAR    BT_UUID_DECLARE_16(CSC_SENSOR_LOCATION_CHAR_UUID)
#define RTK_BT_UUID_CSC_SC_CONTROL_POINT_CHAR   BT_UUID_DECLARE_16(CSC_SC_CONTROL_POINT_CHAR_UUID)

/* Attribute indexes (verified against RTK_BT_GATT_CHARACTERISTIC macro expansion:
 * each CHARACTERISTIC expands to declaration + value, so value index = decl + 1) */
#define CSC_MEASUREMENT_CHAR_VAL_INDEX          (2)   /* notify */
#define CSC_MEASUREMENT_CHAR_CCCD_INDEX         (3)   /* cccd notify */
#define CSC_FEATURE_CHAR_VAL_INDEX              (5)   /* read */
#define CSC_SENSOR_LOCATION_CHAR_VAL_INDEX      (7)   /* read */
#define CSC_SC_CONTROL_POINT_CHAR_VAL_INDEX     (9)   /* write, indicate */
#define CSC_SC_CONTROL_POINT_CHAR_CCCD_INDEX    (10)  /* cccd indicate */

/* SC Control Point Op Codes
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define CSC_SCCP_OPCODE_SET_CUMULATIVE_VALUE        0x01
#define CSC_SCCP_OPCODE_UPDATE_SENSOR_LOCATION      0x03
#define CSC_SCCP_OPCODE_REQUEST_SUPPORTED_LOCATIONS 0x04
#define CSC_SCCP_OPCODE_RESPONSE_CODE               0x10

/* SC Control Point Response Value
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define CSC_SCCP_RESP_SUCCESS                       0x01
#define CSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED         0x02
#define CSC_SCCP_RESP_INVALID_PARAMETER             0x03
#define CSC_SCCP_RESP_OPERATION_FAILED              0x04

/* Application error codes
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define CSC_ATT_ERR_PROC_ALREADY_IN_PROGRESS        0x80
#define CSC_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED      0x81

/* Sensor Location enumeration
 * [2] Bluetooth Assigned Numbers / GATT Specification Supplement */
#define CSC_SENSOR_LOC_OTHER                        0x00
#define CSC_SENSOR_LOC_TOP_OF_SHOE                  0x01
#define CSC_SENSOR_LOC_IN_SHOE                      0x02
#define CSC_SENSOR_LOC_HIP                          0x03
#define CSC_SENSOR_LOC_CHEST                        0x0E

/* SCCP indicate payload max: op_code(1) + req_op_code(1) + resp_value(1) + params */
#define CSC_SCCP_RESP_MAX_LEN                       16

static cscs_meas_val_t csc_measurement_val = {
	.flags = {
		.wheel_revolution_data_present = 1,
		.crank_revolution_data_present = 1,
	},
	.cumulative_wheel_revolutions = 0,
	.last_wheel_event_time = 0,
	.cumulative_crank_revolutions = 0,
	.last_crank_event_time = 0,
};

/* Enable all 3 feature bits by default (0x0007) */
static uint16_t csc_feature_val = CSCS_FEAT_WHEEL_REV_SUPPORTED | CSCS_FEAT_CRANK_REV_SUPPORTED |
								   CSCS_FEAT_MULTI_SENSOR_LOC_SUPPORTED;

static uint8_t csc_sensor_location_val = CSC_SENSOR_LOC_IN_SHOE;

/* Supported sensor location list (subset of enumeration) */
static const uint8_t csc_supported_sensor_locations[] = {
	CSC_SENSOR_LOC_OTHER,
	CSC_SENSOR_LOC_TOP_OF_SHOE,
	CSC_SENSOR_LOC_IN_SHOE,
	CSC_SENSOR_LOC_HIP,
	CSC_SENSOR_LOC_CHEST,
};

static uint8_t csc_msmnt_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t csc_sccp_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t csc_sccp_in_progress_map[RTK_BLE_GAP_MAX_LINKS] = {0};

static rtk_bt_gatt_attr_t cscs_attrs[] = {
	/* idx 0 */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_CSC_SRV),

	/* idx 1, 2 : CSC Measurement, notify only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CSC_MEASUREMENT_CHAR,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	/* idx 3 : Measurement CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* idx 4, 5 : CSC Feature, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CSC_FEATURE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 6, 7 : Sensor Location, read only */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CSC_SENSOR_LOCATION_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* idx 8, 9 : SC Control Point, write & indicate */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_CSC_SC_CONTROL_POINT_CHAR,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* idx 10 : SC Control Point CCCD */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service csc_srv = RTK_BT_GATT_SERVICE(cscs_attrs, CYCLING_SPEED_CADENCE_SRV_ID);

uint16_t cscs_srv_set_params(cscs_param_type_t type, uint8_t len, void *value)
{
	uint16_t ret = RTK_BT_OK;

	if (!value) {
		return RTK_BT_FAIL;
	}

	switch (type) {
	case CSCS_PARAM_MEAS_FLAG:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_measurement_val.flags, value, sizeof(uint8_t));
		}
		break;

	case CSCS_PARAM_MEAS_CUMULATIVE_WHEEL_REV:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_measurement_val.cumulative_wheel_revolutions, value, len);
		}
		break;

	case CSCS_PARAM_MEAS_LAST_WHEEL_EVENT_TIME:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_measurement_val.last_wheel_event_time, value, len);
		}
		break;

	case CSCS_PARAM_MEAS_CUMULATIVE_CRANK_REV:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_measurement_val.cumulative_crank_revolutions, value, len);
		}
		break;

	case CSCS_PARAM_MEAS_LAST_CRANK_EVENT_TIME:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_measurement_val.last_crank_event_time, value, len);
		}
		break;

	case CSCS_PARAM_FEATURE:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_feature_val, value, len);
		}
		break;

	case CSCS_PARAM_SENSOR_LOCATION:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&csc_sensor_location_val, value, len);
		}
		break;

	default:
		ret = RTK_BT_FAIL;
		break;
	}

	return ret;
}

/* Regroup CSC Measurement to little-endian byte stream:
 * flags(1) ->
 * [cumulative_wheel_revolutions(4) + last_wheel_event_time(2) if flag.bit0] ->
 * [cumulative_crank_revolutions(2) + last_crank_event_time(2) if flag.bit1] */
static uint8_t cscs_meas_value_regroup(uint8_t *dst)
{
	uint8_t actual_len = 0;
	cscs_meas_val_t *src = &csc_measurement_val;

	memcpy(&dst[actual_len], &src->flags, sizeof(uint8_t));
	actual_len += sizeof(uint8_t);

	if (src->flags.wheel_revolution_data_present) {
		dst[actual_len++] = (uint8_t)(src->cumulative_wheel_revolutions & 0xFF);
		dst[actual_len++] = (uint8_t)((src->cumulative_wheel_revolutions >> 8) & 0xFF);
		dst[actual_len++] = (uint8_t)((src->cumulative_wheel_revolutions >> 16) & 0xFF);
		dst[actual_len++] = (uint8_t)((src->cumulative_wheel_revolutions >> 24) & 0xFF);
		dst[actual_len++] = (uint8_t)(src->last_wheel_event_time & 0xFF);
		dst[actual_len++] = (uint8_t)((src->last_wheel_event_time >> 8) & 0xFF);
	}

	if (src->flags.crank_revolution_data_present) {
		dst[actual_len++] = (uint8_t)(src->cumulative_crank_revolutions & 0xFF);
		dst[actual_len++] = (uint8_t)((src->cumulative_crank_revolutions >> 8) & 0xFF);
		dst[actual_len++] = (uint8_t)(src->last_crank_event_time & 0xFF);
		dst[actual_len++] = (uint8_t)((src->last_crank_event_time >> 8) & 0xFF);
	}

	return actual_len;
}

static bool cscs_sensor_location_supported(uint8_t loc)
{
	uint8_t i;
	for (i = 0; i < sizeof(csc_supported_sensor_locations); i++) {
		if (csc_supported_sensor_locations[i] == loc) {
			return true;
		}
	}
	return false;
}

static void cscs_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint8_t feature_buf[2];

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	if (CSC_FEATURE_CHAR_VAL_INDEX == p_read_ind->index) {
		feature_buf[0] = (uint8_t)(csc_feature_val & 0xFF);
		feature_buf[1] = (uint8_t)((csc_feature_val >> 8) & 0xFF);
		read_resp.data = feature_buf;
		read_resp.len = sizeof(feature_buf);
	} else if (CSC_SENSOR_LOCATION_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = &csc_sensor_location_val;
		read_resp.len = sizeof(csc_sensor_location_val);
	} else {
		BT_LOGE("[APP] CSCS read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] CSCS response for client read succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE("[APP] CSCS response for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index,
				read_resp.err_code);
}

/* Build & send SC Control Point Response Code(0x10) indicate */
static void cscs_sccp_response(uint16_t conn_handle, uint8_t request_op_code,
							   uint8_t response_value, uint8_t *resp_param, uint8_t param_len)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t buf[CSC_SCCP_RESP_MAX_LEN] = {0};
	uint8_t len = 0;

	buf[len++] = CSC_SCCP_OPCODE_RESPONSE_CODE;
	buf[len++] = request_op_code;
	buf[len++] = response_value;

	if (resp_param && param_len) {
		if (param_len > (CSC_SCCP_RESP_MAX_LEN - len)) {
			param_len = CSC_SCCP_RESP_MAX_LEN - len;
		}
		memcpy(&buf[len], resp_param, param_len);
		len += param_len;
	}

	ind_param.app_id = CYCLING_SPEED_CADENCE_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = CSC_SC_CONTROL_POINT_CHAR_VAL_INDEX;
	ind_param.data = buf;
	ind_param.len = len;
	rtk_bt_gatts_indicate(&ind_param);

	BT_LOGA("[APP] CSCS SCCP resp, req_op: %d, resp_val: %d\r\n", request_op_code, response_value);
}

/* Execute a SC Control Point op code (synchronous, single completion) */
static void cscs_sccp_proc(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t op_code = value[0];
	uint8_t resp_value = CSC_SCCP_RESP_SUCCESS;

	switch (op_code) {
	case CSC_SCCP_OPCODE_SET_CUMULATIVE_VALUE:
		if (!(csc_feature_val & CSCS_FEAT_WHEEL_REV_SUPPORTED)) {
			resp_value = CSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + sizeof(uint32_t)) {
			resp_value = CSC_SCCP_RESP_INVALID_PARAMETER;
		} else {
			uint32_t cumulative = (uint32_t)value[1] | ((uint32_t)value[2] << 8) |
								  ((uint32_t)value[3] << 16) | ((uint32_t)value[4] << 24);
			csc_measurement_val.cumulative_wheel_revolutions = cumulative;
		}
		cscs_sccp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case CSC_SCCP_OPCODE_UPDATE_SENSOR_LOCATION:
		if (!(csc_feature_val & CSCS_FEAT_MULTI_SENSOR_LOC_SUPPORTED)) {
			resp_value = CSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED;
		} else if (len < 1 + sizeof(uint8_t)) {
			resp_value = CSC_SCCP_RESP_INVALID_PARAMETER;
		} else if (!cscs_sensor_location_supported(value[1])) {
			resp_value = CSC_SCCP_RESP_INVALID_PARAMETER;
		} else {
			csc_sensor_location_val = value[1];
		}
		cscs_sccp_response(conn_handle, op_code, resp_value, NULL, 0);
		break;

	case CSC_SCCP_OPCODE_REQUEST_SUPPORTED_LOCATIONS:
		if (!(csc_feature_val & CSCS_FEAT_MULTI_SENSOR_LOC_SUPPORTED)) {
			cscs_sccp_response(conn_handle, op_code, CSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED, NULL, 0);
		} else {
			cscs_sccp_response(conn_handle, op_code, CSC_SCCP_RESP_SUCCESS,
							   (uint8_t *)csc_supported_sensor_locations,
							   sizeof(csc_supported_sensor_locations));
		}
		break;

	default:
		cscs_sccp_response(conn_handle, op_code, CSC_SCCP_RESP_OP_CODE_NOT_SUPPORTED, NULL, 0);
		break;
	}
}

static void cscs_write_hdl(void *data)
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

	if (CSC_SC_CONTROL_POINT_CHAR_VAL_INDEX == p_write_ind->index) {
		if (!p_write_ind->len || !p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		} else if (rtk_bt_le_gap_get_conn_id(p_write_ind->conn_handle, &conn_id) != RTK_BT_OK) {
			write_resp.err_code = RTK_BT_ATT_ERR_UNLIKELY;
		} else if (0 == csc_sccp_cccd_ind_en_map[conn_id]) {
			/* SCCP CCCD not configured for indicate */
			write_resp.err_code = CSC_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
		} else if (csc_sccp_in_progress_map[conn_id]) {
			/* a procedure is already in progress */
			write_resp.err_code = CSC_ATT_ERR_PROC_ALREADY_IN_PROGRESS;
		} else {
			csc_sccp_in_progress_map[conn_id] = 1;
			do_proc = true;
		}
	} else {
		BT_LOGE("[APP] CSCS write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] CSCS response for client write succeed, index: %d\r\n", p_write_ind->index);
	} else {
		BT_LOGE("[APP] CSCS response for client write failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);

	if (do_proc) {
		/* The procedure stays in progress until its Response Code indication is
		 * confirmed (cleared in RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND), so a
		 * concurrent write is rejected with Procedure Already In Progress. */
		cscs_sccp_proc(p_write_ind->conn_handle, p_write_ind->value, p_write_ind->len);
	}
}

static void cscs_cccd_update_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (CSC_MEASUREMENT_CHAR_CCCD_INDEX == p_cccd_ind->index) {
		csc_msmnt_cccd_ntf_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) ? 1 : 0;
		BT_LOGA("[APP] CSCS measurement cccd, notify bit: %d\r\n", csc_msmnt_cccd_ntf_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					csc_msmnt_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
	} else if (CSC_SC_CONTROL_POINT_CHAR_CCCD_INDEX == p_cccd_ind->index) {
		csc_sccp_cccd_ind_en_map[conn_id] = (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) ? 1 : 0;
		BT_LOGA("[APP] CSCS sccp cccd, indicate bit: %d\r\n", csc_sccp_cccd_ind_en_map[conn_id]);
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					csc_sccp_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
	} else {
		BT_LOGE("[APP] CSCS CCCD event unknown index: %d\r\n", p_cccd_ind->index);
		BT_AT_PRINT("+BLEGATTS:cccd,unknown_index\r\n");
	}
}

void cscs_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] CSCS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] CSCS register service failed: %d\r\n", reg_srv_res->reg_status);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		uint8_t conn_id;
		if (RTK_BT_OK == p_ind_ind->err_code) {
			BT_LOGA("[APP] CSCS indicate succeed!\r\n");
		} else {
			BT_LOGE("[APP] CSCS indicate failed\r\n");
		}
		/* A SC Control Point procedure completes when its Response Code
		 * indication is confirmed (or fails); release the in-progress lock. */
		if (rtk_bt_le_gap_get_conn_id(p_ind_ind->conn_handle, &conn_id) == RTK_BT_OK) {
			csc_sccp_in_progress_map[conn_id] = 0;
		}
		BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ind_ind->err_code) ? 0 : -1, p_ind_ind->app_id,
					p_ind_ind->conn_handle, p_ind_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ntf_ind->err_code) {
			BT_LOGA("[APP] CSCS notify succeed!\r\n");
		} else {
			BT_LOGE("[APP] CSCS notify failed, local error\r\n");
		}
		BT_AT_PRINT("+BLEGATTS:notify,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ntf_ind->err_code) ? 0 : -1, p_ntf_ind->app_id,
					p_ntf_ind->conn_handle, p_ntf_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND:
		cscs_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		cscs_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		cscs_cccd_update_hdl(data);
		break;

	default:
		break;
	}
}

void cscs_measurement_notify(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t meas_buf[CSCS_MEASUREMENT_VALUE_MAX_LEN] = {0};
	uint8_t actual_len = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	if (csc_msmnt_cccd_ntf_en_map[conn_id]) {
		actual_len = cscs_meas_value_regroup(meas_buf);
		ntf_param.app_id = CYCLING_SPEED_CADENCE_SRV_ID;
		ntf_param.conn_handle = conn_handle;
		ntf_param.index = CSC_MEASUREMENT_CHAR_VAL_INDEX;
		ntf_param.data = meas_buf;
		ntf_param.len = actual_len;
		ntf_param.seq = 0;
		rtk_bt_gatts_notify(&ntf_param);
	}
}

uint16_t cscs_srv_add(void)
{
	csc_srv.type = GATT_SERVICE_OVER_BLE;
	csc_srv.server_info = 0;
	csc_srv.user_data = NULL;
	csc_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&csc_srv);
}

void cscs_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	csc_msmnt_cccd_ntf_en_map[conn_id] = 0;
	csc_sccp_cccd_ind_en_map[conn_id] = 0;
	csc_sccp_in_progress_map[conn_id] = 0;
}

void cscs_status_deinit(void)
{
	memset(csc_msmnt_cccd_ntf_en_map, 0, sizeof(csc_msmnt_cccd_ntf_en_map));
	memset(csc_sccp_cccd_ind_en_map, 0, sizeof(csc_sccp_cccd_ind_en_map));
	memset(csc_sccp_in_progress_map, 0, sizeof(csc_sccp_in_progress_map));
}
