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
#include <rtk_hts.h>
#include <bt_utils.h>

#define HTS_TEMPERATURE_TYPE_BODY_GENERAL   2

/* Measurement Interval Valid Range descriptor value: {Min, Max}, little endian, endpoints inclusive */
static uint16_t valid_range[2] = {1, 65535};

/* Temperature Measurement / Intermediate Temperature share the same value structure.
 * Default: Celsius, no Time Stamp / Temperature Type field emitted (flags = 0). The
 * Temperature Type field is mutually exclusive with the Temperature Type characteristic
 * (spec 3.2); a Time Stamp must not carry month/day = 0 (spec 3.1.1.3), so the stored
 * timestamp defaults to a valid date and can be updated (validated) via
 * health_thermometer_srv_set_params(HTS_TEMPERATURE_MEASUREMENT_PARAM_TIMESTAMP, 7, ...). */
static temperature_measurement_flag_t hts_flags = {
	.temperature_unit_bit = 0,      /* Celsius */
	.time_stamp_bit = 0,
	.temperature_type_bit = 0,
};
static uint32_t temperature_value = 0;              /* IEEE-11073 32-bit FLOAT raw bits */
static uint8_t timestamp[7] = {0xEA, 0x07, 0x01, 0x01, 0x00, 0x00, 0x00}; /* 2026-01-01 00:00:00, month/day >= 1 */
static uint8_t temperature_type = HTS_TEMPERATURE_TYPE_BODY_GENERAL;
static uint16_t measurement_interval = 0;           /* seconds, 0 means no periodic measurement */

static uint8_t temp_msmnt_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t intermediate_temp_cccd_ntf_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t msmnt_interval_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/* Periodic Temperature Measurement indicate machinery (HTS/SEN/SP/BV-01-C).
 * rtk_bt_gatts_indicate is a synchronous blocking call and must never run inside
 * the software timer daemon; the timer only gives a semaphore, the dedicated task sends. */
#define HTS_MEAS_TASK_STACK_SIZE   (256 * 4)
#define HTS_MEAS_TASK_PRIORITY     2

static void *hts_meas_timer_hdl = NULL;
static void *hts_meas_sem = NULL;
static void *hts_meas_task_hdl = NULL;
static uint8_t hts_meas_task_running = 0;

static bool hts_any_temp_meas_ind_enabled(void)
{
	uint8_t i;
	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		if (temp_msmnt_cccd_ind_en_map[i]) {
			return true;
		}
	}
	return false;
}

static void hts_meas_timer_callback(void *arg)
{
	(void)arg;
	if (hts_meas_sem) {
		osif_sem_give(hts_meas_sem);
	}
}

static void hts_meas_task_entry(void *param)
{
	(void)param;
	rtk_bt_le_get_active_conn_t active_conn;
	uint8_t conn_id;
	uint16_t i;

	while (hts_meas_task_running) {
		if (!osif_sem_take(hts_meas_sem, BT_TIMEOUT_FOREVER)) {
			continue;
		}
		if (!hts_meas_task_running) {
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
			if (temp_msmnt_cccd_ind_en_map[conn_id]) {
				temperature_measurement_indicate(active_conn.conn_handle[i]);
			}
		}
	}
	osif_task_delete(NULL);
}

/* Start/restart/stop the periodic timer based on measurement_interval and CCCD state */
static void hts_meas_timer_update(void)
{
	if (!hts_meas_timer_hdl) {
		return;
	}
	if (measurement_interval > 0 && hts_any_temp_meas_ind_enabled()) {
		osif_timer_restart(&hts_meas_timer_hdl, (uint32_t)measurement_interval * 1000);
	} else {
		osif_timer_stop(&hts_meas_timer_hdl);
	}
}

static rtk_bt_gatt_attr_t hts_attrs[] = {
	/* Primary Service: Health Thermometer */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_HEALTH_THERMOMETER_SRV),

	/* Characteristic: Temperature Measurement (indicate only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TEMPERATURE_MEASUREMENT_CHAR,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: Temperature Type (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TEMPERATURE_TYPE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Characteristic: Intermediate Temperature (notify only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_INTERMEDIATE_TEMPERATURE_CHAR,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Characteristic: Measurement Interval (read, write with authentication, indicate).
	 * HTS spec 3.4 Table 3.1: Write is "Writable with authentication" -> PERM_WRITE_AUTHEN. */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_MEASUREMENT_INTERVAL_CHAR,
							   RTK_BT_GATT_CHRC_READ | RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE_AUTHEN),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Descriptor: Valid Range for Measurement Interval (read handled by stack internally) */
	RTK_BT_GATT_DESCRIPTOR(BT_UUID_DECLARE_16(VALID_RANGE_DESC_UUID),
						   RTK_BT_GATT_PERM_READ,
						   (void *)valid_range, sizeof(valid_range), RTK_BT_GATT_INTERNAL),
};
static struct rtk_bt_gatt_service health_thermometer_srv = RTK_BT_GATT_SERVICE(hts_attrs, HEALTH_THERMOMETER_SRV_ID);

uint16_t health_thermometer_srv_set_params(hts_param_type_t param_type, uint8_t len, void *p_value)
{
	uint16_t ret = RTK_BT_OK;

	if (!p_value) {
		return RTK_BT_FAIL;
	}

	switch (param_type) {
	case HTS_TEMPERATURE_MEASUREMENT_PARAM_FLAG:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&hts_flags, p_value, sizeof(uint8_t));
			hts_flags.rfu = 0;   /* RFU shall be 0 (spec 3.1.1.1) */
			/* Time Stamp field must not carry month/day = 0 (spec 3.1.1.3) */
			if (hts_flags.time_stamp_bit && (timestamp[2] == 0 || timestamp[3] == 0)) {
				hts_flags.time_stamp_bit = 0;
			}
		}
		break;

	case HTS_TEMPERATURE_MEASUREMENT_PARAM_TIMESTAMP:
		/* Date Time: Year(2 LE) Month Day Hours Minutes Seconds; month/day must be >= 1 */
		if (len != sizeof(timestamp) ||
			((uint8_t *)p_value)[2] < 1 || ((uint8_t *)p_value)[2] > 12 ||
			((uint8_t *)p_value)[3] < 1 || ((uint8_t *)p_value)[3] > 31) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(timestamp, p_value, sizeof(timestamp));
		}
		break;

	case HTS_TEMPERATURE_MEASUREMENT_PARAM_TEMPERATURE_VALUE:
		if (len != sizeof(uint32_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&temperature_value, p_value, sizeof(uint32_t));
		}
		break;

	case HTS_TEMPERATURE_TYPE_PARAM_VALUE:
		if (len != sizeof(uint8_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&temperature_type, p_value, sizeof(uint8_t));
		}
		break;

	case HTS_MEASUREMENT_INTERVAL_PARAM_VALUE:
		if (len != sizeof(uint16_t)) {
			ret = RTK_BT_FAIL;
		} else {
			memcpy(&measurement_interval, p_value, sizeof(uint16_t));
		}
		break;

	default:
		ret = RTK_BT_FAIL;
		break;
	}

	return ret;
}

/* Regroup Temperature Measurement / Intermediate Temperature value according to flags */
static uint8_t temperature_measurement_value_regroup(uint8_t *dst)
{
	uint8_t actual_len = 0;

	memcpy(&dst[actual_len], &hts_flags, 1);
	actual_len += 1;

	memcpy(&dst[actual_len], &temperature_value, sizeof(uint32_t));
	actual_len += sizeof(uint32_t);

	if (hts_flags.time_stamp_bit) {
		memcpy(&dst[actual_len], timestamp, sizeof(timestamp));
		actual_len += sizeof(timestamp);
	}

	if (hts_flags.temperature_type_bit) {
		memcpy(&dst[actual_len], &temperature_type, sizeof(uint8_t));
		actual_len += sizeof(uint8_t);
	}

	return actual_len;
}

static void hts_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	if (HTS_TEMPERATURE_TYPE_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = &temperature_type;
		read_resp.len = sizeof(temperature_type);
	} else if (HTS_MEASUREMENT_INTERVAL_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = &measurement_interval;
		read_resp.len = sizeof(measurement_interval);
	} else {
		BT_LOGE("[APP] HTS read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] HTS response for client read succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE("[APP] HTS response for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index,
				read_resp.err_code);
}

static void hts_write_hdl(void *data)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint16_t interval = 0;

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (!p_write_ind->len || !p_write_ind->value) {
		BT_LOGE("[APP] HTS write value is empty!\r\n");
		write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		goto send_rsp;
	}

	if (HTS_MEASUREMENT_INTERVAL_CHAR_VAL_INDEX == p_write_ind->index) {
		if (p_write_ind->len != sizeof(uint16_t)) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
			goto send_rsp;
		}
		memcpy(&interval, p_write_ind->value, sizeof(uint16_t));
		/* 0 is allowed (disable periodic measurement), otherwise must be within valid range */
		if (interval != 0 && (interval < valid_range[0] || interval > valid_range[1])) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE;
			goto send_rsp;
		}
		measurement_interval = interval;
		hts_meas_timer_update();
		BT_LOGA("[APP] HTS measurement interval write value: %d, type: %d\r\n",
				measurement_interval, p_write_ind->type);
		BT_AT_PRINT("+BLEGATTS:write,%u,%u,%u,%u,%u,%u\r\n",
					p_write_ind->app_id, p_write_ind->conn_handle, p_write_ind->index,
					p_write_ind->len, p_write_ind->type, measurement_interval);
	} else {
		BT_LOGE("[APP] HTS write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

send_rsp:
	if (RTK_BT_GATTS_WRITE_NO_RESP == p_write_ind->type ||
		RTK_BT_GATTS_WRITE_NO_RESP_SIGNED == p_write_ind->type) {
		return;
	}

	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] HTS response for client write succeed!\r\n");
	} else {
		BT_LOGE("[APP] HTS response for client write failed!\r\n");
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);
}

static void hts_cccd_update_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t cccd_ind = p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE;
	uint8_t cccd_ntf = p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	switch (p_cccd_ind->index) {
	case HTS_TEMPERATURE_MEASUREMENT_CHAR_CCCD_INDEX:
		temp_msmnt_cccd_ind_en_map[conn_id] = cccd_ind;
		hts_meas_timer_update();
		BT_LOGA("[APP] HTS temperature measurement cccd, indicate bit: %d\r\n", cccd_ind);
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					temp_msmnt_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	case HTS_INTERMEDIATE_TEMPERATURE_CHAR_CCCD_INDEX:
		intermediate_temp_cccd_ntf_en_map[conn_id] = cccd_ntf;
		BT_LOGA("[APP] HTS intermediate temperature cccd, notify bit: %d\r\n", cccd_ntf);
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					intermediate_temp_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	case HTS_MEASUREMENT_INTERVAL_CHAR_CCCD_INDEX:
		msmnt_interval_cccd_ind_en_map[conn_id] = cccd_ind;
		BT_LOGA("[APP] HTS measurement interval cccd, indicate bit: %d\r\n", cccd_ind);
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					msmnt_interval_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;

	default:
		BT_LOGE("[APP] HTS CCCD event unknown index: %d\r\n", p_cccd_ind->index);
		BT_AT_PRINT("+BLEGATTS:cccd,unknown_index\r\n");
		break;
	}
}

void health_thermometer_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] HTS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] HTS register service failed: %d\r\n", reg_srv_res->reg_status);
		}

		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ind_ind->err_code) {
			BT_LOGA("[APP] HTS indicate succeed!\r\n");
		} else {
			BT_LOGE("[APP] HTS indicate failed \r\n");
		}
		BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ind_ind->err_code) ? 0 : -1, p_ind_ind->app_id,
					p_ind_ind->conn_handle, p_ind_ind->index);
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ntf_ind->err_code) {
			BT_LOGA("[APP] HTS notify succeed!\r\n");
		} else {
			BT_LOGE("[APP] HTS notify failed, local error \r\n");
		}
		BT_AT_PRINT("+BLEGATTS:notify,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ntf_ind->err_code) ? 0 : -1, p_ntf_ind->app_id,
					p_ntf_ind->conn_handle, p_ntf_ind->index);
		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND:
		hts_read_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_WRITE_IND:
		hts_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND:
		hts_cccd_update_hdl(data);
		break;

	default:
		break;
	}
}

uint16_t temperature_measurement_indicate(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t value_buf[HTS_TEMPERATURE_MEASUREMENT_VALUE_MAX_LEN] = {0};
	uint8_t actual_len = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	if (!temp_msmnt_cccd_ind_en_map[conn_id]) {
		BT_LOGE("[APP] HTS temperature measurement indicate not enabled!\r\n");
		return RTK_BT_FAIL;
	}

	actual_len = temperature_measurement_value_regroup(value_buf);
	ind_param.app_id = HEALTH_THERMOMETER_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = HTS_TEMPERATURE_MEASUREMENT_CHAR_VAL_INDEX;
	ind_param.data = value_buf;
	ind_param.len = actual_len;
	ind_param.seq = 0;

	return rtk_bt_gatts_indicate(&ind_param);
}

uint16_t intermediate_temperature_notify(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
	uint8_t value_buf[HTS_TEMPERATURE_MEASUREMENT_VALUE_MAX_LEN] = {0};
	uint8_t actual_len = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	if (!intermediate_temp_cccd_ntf_en_map[conn_id]) {
		BT_LOGE("[APP] HTS intermediate temperature notify not enabled!\r\n");
		return RTK_BT_FAIL;
	}

	actual_len = temperature_measurement_value_regroup(value_buf);
	ntf_param.app_id = HEALTH_THERMOMETER_SRV_ID;
	ntf_param.conn_handle = conn_handle;
	ntf_param.index = HTS_INTERMEDIATE_TEMPERATURE_CHAR_VAL_INDEX;
	ntf_param.data = value_buf;
	ntf_param.len = actual_len;
	ntf_param.seq = 0;

	return rtk_bt_gatts_notify(&ntf_param);
}

uint16_t measurement_interval_indicate(uint16_t conn_handle)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	if (!msmnt_interval_cccd_ind_en_map[conn_id]) {
		BT_LOGE("[APP] HTS measurement interval indicate not enabled!\r\n");
		return RTK_BT_FAIL;
	}

	ind_param.app_id = HEALTH_THERMOMETER_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = HTS_MEASUREMENT_INTERVAL_CHAR_VAL_INDEX;
	ind_param.data = &measurement_interval;
	ind_param.len = sizeof(measurement_interval);
	ind_param.seq = 0;

	return rtk_bt_gatts_indicate(&ind_param);
}

uint16_t health_thermometer_srv_add(void)
{
	health_thermometer_srv.type = GATT_SERVICE_OVER_BLE;
	health_thermometer_srv.server_info = 0;
	health_thermometer_srv.user_data = NULL;
	health_thermometer_srv.register_status = 0;

	/* Create periodic measurement resources once. Timer period is a placeholder here;
	 * the real interval is applied via hts_meas_timer_update() on restart. */
	if (hts_meas_sem == NULL) {
		osif_sem_create(&hts_meas_sem, 0, 1);
	}
	if (hts_meas_timer_hdl == NULL) {
		osif_timer_create(&hts_meas_timer_hdl, "hts_meas_timer", 0, 1000, true, hts_meas_timer_callback);
	}
	if (hts_meas_task_hdl == NULL) {
		hts_meas_task_running = 1;
		osif_task_create(&hts_meas_task_hdl, "hts_meas_task", hts_meas_task_entry, NULL,
						 HTS_MEAS_TASK_STACK_SIZE, HTS_MEAS_TASK_PRIORITY);
	}

	return rtk_bt_gatts_register_service(&health_thermometer_srv);
}

void hts_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	temp_msmnt_cccd_ind_en_map[conn_id] = 0;
	intermediate_temp_cccd_ntf_en_map[conn_id] = 0;
	msmnt_interval_cccd_ind_en_map[conn_id] = 0;

	/* Stop periodic measurement once all links have disabled the indication */
	hts_meas_timer_update();
}

void hts_status_deinit(void)
{
	memset(temp_msmnt_cccd_ind_en_map, 0, sizeof(temp_msmnt_cccd_ind_en_map));
	memset(intermediate_temp_cccd_ntf_en_map, 0, sizeof(intermediate_temp_cccd_ntf_en_map));
	memset(msmnt_interval_cccd_ind_en_map, 0, sizeof(msmnt_interval_cccd_ind_en_map));

	/* Stop and delete the periodic timer, then signal the task to exit. The task will
	 * break out of its loop (running == 0) and call osif_task_delete(NULL) itself.
	 * The semaphore is intentionally left alive to avoid a use-after-free race with
	 * the task that may still be waking up from osif_sem_take(). */
	if (hts_meas_timer_hdl) {
		osif_timer_stop(&hts_meas_timer_hdl);
		osif_timer_delete(&hts_meas_timer_hdl);
	}
	if (hts_meas_task_hdl) {
		hts_meas_task_running = 0;
		if (hts_meas_sem) {
			osif_sem_give(hts_meas_sem);
		}
		hts_meas_task_hdl = NULL;
	}
}
