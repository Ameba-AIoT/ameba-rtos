/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <access.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <bt_app_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_sensor_model.h>
#include <msg.h>

/******SENSOR PART******/
#define OP_SENSOR_DESCRIPTOR_GET       BT_MESH_MODEL_OP_2(0x82, 0x30)
#define OP_SENSOR_DESCRIPTOR_STATUS    BT_MESH_MODEL_OP_1(0x51)
#define OP_SENSOR_GET                  BT_MESH_MODEL_OP_2(0x82, 0x31)
#define OP_SENSOR_STATUS               BT_MESH_MODEL_OP_1(0x52)
#define OP_SENSOR_COLUMN_GET           BT_MESH_MODEL_OP_2(0x82, 0x32)
#define OP_SENSOR_COLUMN_STATUS        BT_MESH_MODEL_OP_1(0x53)
#define OP_SENSOR_SERIES_GET           BT_MESH_MODEL_OP_2(0x82, 0x33)
#define OP_SENSOR_SERIES_STATUS        BT_MESH_MODEL_OP_1(0x54)

/******SENSOR SETUP PART******/
#define OP_SENSOR_CADENCE_GET          BT_MESH_MODEL_OP_2(0x82, 0x34)
#define OP_SENSOR_CADENCE_SET          BT_MESH_MODEL_OP_1(0x55)
#define OP_SENSOR_CADENCE_SET_UNACK    BT_MESH_MODEL_OP_1(0x56)
#define OP_SENSOR_CADENCE_STATUS       BT_MESH_MODEL_OP_1(0x57)
#define OP_SENSOR_SETTINGS_GET         BT_MESH_MODEL_OP_2(0x82, 0x35)
#define OP_SENSOR_SETTINGS_STATUS      BT_MESH_MODEL_OP_1(0x58)
#define OP_SENSOR_SETTING_GET          BT_MESH_MODEL_OP_2(0x82, 0x36)
#define OP_SENSOR_SETTING_SET          BT_MESH_MODEL_OP_1(0x59)
#define OP_SENSOR_SETTING_SET_UNACK    BT_MESH_MODEL_OP_1(0x5a)
#define OP_SENSOR_SETTING_STATUS       BT_MESH_MODEL_OP_1(0x5b)

/******COMMON API FOR SENSOR DATABASE******/
typedef struct {
	uint16_t format: 1;
	uint16_t length: 4;
	uint16_t property_id: 11;
} _PACKED4_ sensor_formata_t;

typedef struct {
	uint8_t format: 1;
	uint8_t length: 7;
	uint16_t property_id;
} _PACKED4_ sensor_formatb_t;

typedef struct {
	rtk_bt_mesh_sensor_db_t *sensors;
	uint16_t num_sensors;
} sensor_info_t;

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
/***********Setup database for test***********/
#if ((defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL))
rtk_bt_mesh_sensor_descriptor_t descriptor1 = {1, 10, 10, 1, 1, 1};
rtk_bt_mesh_sensor_descriptor_t descriptor2 = {2, 5, 5, 1, 1, 1};
uint8_t sensor_raw_data_len = 5;//the array length in mesh_device.c is 5
uint8_t setting_raw1[] = {1, 2, 3, 4, 5};
uint8_t setting_raw2[] = {5, 4, 3, 2, 1};
rtk_bt_mesh_sensor_setting_t settings[] = {
	{1, 3, 5, setting_raw1},
	{2, 3, 5, setting_raw2}
};

uint8_t setting_raw3[] = {1, 1, 1, 1, 1};
uint8_t setting_raw4[] = {2, 2, 2, 2, 2};
rtk_bt_mesh_sensor_setting_t settings2[] = {
	{1, 3, 5, setting_raw3},
	{2, 3, 5, setting_raw4}
};
uint8_t delta_down_db[] = {3, 2, 1, 2, 3};
uint8_t delta_up_db[] =  {1, 2, 3, 2, 1};
uint8_t cadence_low_db[] = {4, 3, 2, 3, 4};
uint8_t cadence_high_db[] = {2, 3, 4, 3, 2};
uint8_t delta_down_db2[] = {3, 3, 3, 3, 3};
uint8_t delta_up_db2[] =  {4, 4, 4, 4, 4};
uint8_t cadence_low_db2[] = {3, 3, 3, 3, 3};
uint8_t cadence_high_db2[] = {4, 4, 4, 4, 4};
rtk_bt_mesh_sensor_cadence_t cadence_db[] = {{5, 1, 0, delta_down_db, delta_up_db, 1, cadence_low_db, cadence_high_db}};
rtk_bt_mesh_sensor_cadence_t cadence_db2[] = {{5, 1, 0, delta_down_db2, delta_up_db2, 1, cadence_low_db2, cadence_high_db2}};
int32_t pub_count_db = 0;

rtk_bt_mesh_sensor_db_t sensor_db[] = {
	{{1, 10, 10, 1, 1, 1}, 5, 2, settings, cadence_db, 0},
	{{2, 5, 5, 1, 1, 1}, 5, 2, settings2, cadence_db2, 0}
};
sensor_info_t sensors_list = {.sensors = sensor_db, .num_sensors = 2};
#endif

/**********************SENSOR SERVER FUNCTION PART****************************/
#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
// sensor_info_t sensors_list = {.sensors = NULL, .num_sensors = 0}; //for set db api test
static const struct bt_mesh_model *sensor_server_model = NULL;

static int sensor_srv_init(const struct bt_mesh_model *model)
{
	sensor_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_sensor_srv_cb = {
	.init = sensor_srv_init,
};

void sensor_server_set_db(const struct bt_mesh_model *model, rtk_bt_mesh_sensor_db_t *sensors, uint16_t num_sensors)
{
	/*****
	 * When sensor server model init
	 * customer should set a pointer for database in user data, like:
	 * BT_MESH_MODEL(BT_MESH_MODEL_ID_SENSOR_SRV, sensor_srv_op, NULL, &sensors_list),
	 * ****/
	sensor_info_t *pinfo = model->rt->user_data;
	pinfo->sensors = sensors;
	pinfo->num_sensors = num_sensors;
}

static int sensor_descriptor_status(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									uint16_t property_id)
{
	sensor_info_t *pinfo = model->rt->user_data;
	uint16_t msg_len;
	if (property_id == 0) {
		/* get all descriptors */
		msg_len = sizeof(rtk_bt_mesh_sensor_descriptor_t) * pinfo->num_sensors;
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_DESCRIPTOR_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_DESCRIPTOR_STATUS);

		for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
			net_buf_simple_add_mem(&pmsg, &pinfo->sensors[i].descriptor, sizeof(rtk_bt_mesh_sensor_descriptor_t));
		}
		return bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		/* get specified descriptor */
		rtk_bt_mesh_sensor_descriptor_t *pdescriptor = NULL;
		for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
			if (pinfo->sensors[i].descriptor.property_id == property_id) {
				pdescriptor = &(pinfo->sensors[i].descriptor);
				break;
			}
		}

		if (NULL == pdescriptor) {
			msg_len = 2; //PROPERTY_ID_LEN
		} else {
			msg_len = sizeof(rtk_bt_mesh_sensor_descriptor_t);
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_DESCRIPTOR_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_DESCRIPTOR_STATUS);

		if (NULL == pdescriptor) {
			net_buf_simple_add_mem(&pmsg, &property_id, 2);
		} else {
			net_buf_simple_add_mem(&pmsg, pdescriptor, sizeof(rtk_bt_mesh_sensor_descriptor_t));
		}

		/**Event test**/
		rtk_bt_mesh_sensor_server_get_descriptor_num_t *p_get_data_num; //property id == 0 means get all
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_NUM_GET,
									sizeof(rtk_bt_mesh_sensor_server_get_descriptor_num_t));
		p_get_data_num = (rtk_bt_mesh_sensor_server_get_descriptor_num_t *)p_evt->data;
		p_get_data_num->property_id = property_id;
		uint16_t d_num = 0;
		p_get_data_num->descriptor_num = &d_num;
		rtk_bt_evt_indicate(p_evt, NULL);
		/******Get descriptor number******/
		rtk_bt_mesh_sensor_server_get_descriptor_t des_get = {property_id, d_num, NULL};
		if (d_num > SENSOR_GET_DATA_MAX_LEN) {
			BT_LOGE("[%s] The num of sensor descirptors is %d, extend max SENSOR_GET_DATA_MAX_LEN:%d\r\n", __func__, d_num, SENSOR_GET_DATA_MAX_LEN);
			d_num = SENSOR_GET_DATA_MAX_LEN;
		}
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_GET,
									sizeof(rtk_bt_mesh_sensor_server_get_descriptor_t) + d_num * sizeof(rtk_bt_mesh_sensor_descriptor_t));
		rtk_bt_mesh_sensor_descriptor_t sensor_descriptors[SENSOR_GET_DATA_MAX_LEN];
		rtk_bt_mesh_sensor_server_get_descriptor_t *p_get_data_d;
		p_get_data_d = (rtk_bt_mesh_sensor_server_get_descriptor_t *)p_evt->data;
		p_get_data_d->property_id = property_id;
		p_get_data_d->descriptor_num = d_num;
		p_get_data_d->descriptor = sensor_descriptors;
		rtk_bt_evt_indicate(p_evt, NULL);
		des_get.descriptor = sensor_descriptors;
		for (int i = 0; i < d_num; i++) {
			BT_LOGD("property_id: %d, positive_tolerance: %d, negative_tolerance: %d, sampling_function: %d, measurement_period: %d, update_interval: %d \r\n",
					des_get.descriptor[i].property_id, des_get.descriptor[i].positive_tolerance, des_get.descriptor[i].negative_tolerance,
					des_get.descriptor[i].sampling_function, des_get.descriptor[i].measurement_period, des_get.descriptor[i].update_interval);
		}

		/********/
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}

}

static int sensor_status_internal(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  uint16_t property_id)
{
	int ret;
	uint16_t msg_len = 0;
	sensor_info_t *pinfo = model->rt->user_data;
	rtk_bt_mesh_sensor_server_get_t p_get_data = {0, NULL};
	if (0 == property_id) {
		/* get all sensor data */
		for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
			if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
				(pinfo->sensors[i].descriptor.property_id < 2048)) {
				msg_len += 2;
			} else {
				msg_len += 3;
			}
			msg_len += pinfo->sensors[i].sensor_raw_data_len;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_STATUS);

		for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
			if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
				(pinfo->sensors[i].descriptor.property_id < 2048)) {
				sensor_formata_t pformat;
				pformat.format = 0;
				pformat.length = pinfo->sensors[i].sensor_raw_data_len - 1;
				pformat.property_id = pinfo->sensors[i].descriptor.property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
			} else {
				sensor_formatb_t pformat;
				pformat.format = 1;
				pformat.length = pinfo->sensors[i].sensor_raw_data_len - 1;
				pformat.property_id = pinfo->sensors[i].descriptor.property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
			}

			p_get_data.property_id = pinfo->sensors[i].descriptor.property_id;
			p_get_data.raw_data = NULL;
			uint8_t sensor_get_value[SENSOR_GET_DATA_MAX_LEN * 2];
			rtk_bt_mesh_sensor_server_get_t *sensor_value;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_GET,
										sizeof(rtk_bt_mesh_sensor_server_get_t) + SENSOR_GET_DATA_MAX_LEN * 2 + 2);
			sensor_value = (rtk_bt_mesh_sensor_server_get_t *)p_evt->data;
			sensor_value->property_id = pinfo->sensors[i].descriptor.property_id;
			sensor_value->raw_data = sensor_get_value;
			rtk_bt_evt_indicate(p_evt, NULL);
			if (LE_TO_U16(sensor_get_value)) {
				p_get_data.raw_data = &sensor_get_value[2];
			} else {
				p_get_data.raw_data = NULL;
			}
			//reserve uint16_t to store data len

			if (NULL != p_get_data.raw_data) {
				for (uint16_t j = 0; j < pinfo->sensors[i].sensor_raw_data_len; ++j) {
					net_buf_simple_add_u8(&pmsg, ((uint8_t *)p_get_data.raw_data)[j]);
				}
			}
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		/* get specified sensor data */
		rtk_bt_mesh_sensor_db_t *psensor = NULL;
		for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
			if (pinfo->sensors[i].descriptor.property_id == property_id) {
				psensor = &pinfo->sensors[i];
			}
		}

		if (NULL == psensor) {
			if (property_id < 2048) {
				msg_len += 2;
			} else {
				msg_len += 3;
			}
		} else {
			if ((psensor->sensor_raw_data_len <= 16) &&
				(psensor->descriptor.property_id < 2048)) {
				msg_len += 2;
			} else {
				msg_len += 3;
			}
			msg_len += psensor->sensor_raw_data_len;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_STATUS);
		if (NULL == psensor) {
			if (property_id < 2048) {
				sensor_formata_t pformat;
				pformat.format = 0;
				pformat.length = 0x0f;
				pformat.property_id = property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
			} else {
				sensor_formatb_t pformat;
				pformat.format = 1;
				pformat.length = 0x7f;
				pformat.property_id = property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formatb_t));
			}
		} else {
			if ((psensor->sensor_raw_data_len <= 16) &&
				(psensor->descriptor.property_id < 2048)) {
				sensor_formata_t pformat;
				pformat.format = 0;
				pformat.length = psensor->sensor_raw_data_len - 1;
				pformat.property_id = psensor->descriptor.property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
			} else {
				sensor_formatb_t pformat;
				pformat.format = 1;
				pformat.length = psensor->sensor_raw_data_len - 1;
				pformat.property_id = psensor->descriptor.property_id;
				net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formatb_t));
			}

			p_get_data.property_id = psensor->descriptor.property_id;
			p_get_data.raw_data = NULL;

			uint8_t sensor_get_value[SENSOR_GET_DATA_MAX_LEN * 2];
			rtk_bt_mesh_sensor_server_get_t *sensor_value;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_GET,
										sizeof(rtk_bt_mesh_sensor_server_get_t) + SENSOR_GET_DATA_MAX_LEN * 2 + 2);
			sensor_value = (rtk_bt_mesh_sensor_server_get_t *)p_evt->data;
			sensor_value->property_id = p_get_data.property_id;
			sensor_value->raw_data = sensor_get_value;
			rtk_bt_evt_indicate(p_evt, NULL);
			if (LE_TO_U16(sensor_get_value)) {
				p_get_data.raw_data = &sensor_get_value[2];
			} else {
				p_get_data.raw_data = NULL;
			}
			//reserve uint16_t to store data len

			if (NULL != p_get_data.raw_data) {
				for (uint16_t j = 0; j < psensor->sensor_raw_data_len; ++j) {
					net_buf_simple_add_u8(&pmsg, ((uint8_t *)p_get_data.raw_data)[j]);
				}
			}
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	}

	return ret;
}

static int sensor_column_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t property_id, uint8_t raw_value_x_len,
								const void *raw_value_x,
								uint16_t column_len, const void *column)
{
	int ret;
	uint16_t msg_len;
	if ((0 == column_len) || (NULL == column)) {
		msg_len = 2 + raw_value_x_len;
	} else {
		msg_len = 2 + column_len;
	}

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_COLUMN_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_SENSOR_COLUMN_STATUS);

	net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
	if ((column_len > 0) && (NULL != column)) {
		net_buf_simple_add_mem(&pmsg, column, column_len);
	} else {
		net_buf_simple_add_mem(&pmsg, raw_value_x, raw_value_x_len);
	}

	ret = bt_mesh_msg_send(model, ctx, &pmsg);

	return ret;
}

static int sensor_series_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t property_id,
								uint8_t series_len, void *series)
{
	int ret;
	uint16_t msg_len = 2 + series_len;
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SERIES_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SERIES_STATUS);

	net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
	net_buf_simple_add_mem(&pmsg, series, series_len);

	ret = bt_mesh_msg_send(model, ctx, &pmsg);

	return ret;
}

static int sensor_srv_desc_get(const struct bt_mesh_model *model,
							   struct bt_mesh_msg_ctx *ctx,
							   struct net_buf_simple *buf)
{
	uint16_t property_id;
	if (buf->len) {
		property_id = net_buf_simple_pull_le16(buf);
		if (property_id == 0) {
			return -EINVAL;
		}
	} else {
		property_id = 0;
	}
	int16_t ret = 0;
	ret = sensor_descriptor_status(model, ctx, property_id);
	return ret;
}

static int sensor_srv_get(const struct bt_mesh_model *model,
						  struct bt_mesh_msg_ctx *ctx,
						  struct net_buf_simple *buf)
{
	uint16_t property_id;
	int ret = -1;
	if (buf->len == 2) {
		/* get specified sensor data */
		property_id = net_buf_simple_pull_le16(buf);
		if (property_id != 0) {
			ret = sensor_status_internal(model, ctx, property_id);
		}
	} else if (buf->len == 0) {
		/* get all sensor data */
		property_id = 0;
		ret = sensor_status_internal(model, ctx, property_id);
	}

	return ret;
}

static int sensor_srv_colu_get(const struct bt_mesh_model *model,
							   struct bt_mesh_msg_ctx *ctx,
							   struct net_buf_simple *buf)
{
	uint16_t property_id = net_buf_simple_pull_le16(buf);
	if (property_id != 0) {
		rtk_bt_mesh_sensor_server_get_column_t p_get_data;
		memset(&p_get_data, 0, sizeof(rtk_bt_mesh_sensor_server_get_column_t));
		p_get_data.property_id = property_id;
		p_get_data.raw_value_x_len = buf->len;
		p_get_data.raw_value_x = buf->data;

		rtk_bt_mesh_sensor_server_direct_get_column_t *column_get;
		uint8_t raw_value_x_len = p_get_data.raw_value_x_len;
		if (p_get_data.raw_value_x_len > SENSOR_COLUMN_DATA_MAX_LEN) {
			BT_LOGE("[%s] The len of raw value x is %d, extend max SENSOR_COLUMN_DATA_MAX_LEN:%d\r\n", __func__, p_get_data.raw_value_x_len, SENSOR_COLUMN_DATA_MAX_LEN);
			raw_value_x_len = SENSOR_COLUMN_DATA_MAX_LEN;
		}
		uint8_t sensor_column_get_value[SENSOR_GET_COLUMN_MAX_LEN + 2];
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_COLUMN_GET,
									sizeof(rtk_bt_mesh_sensor_server_direct_get_column_t) + p_get_data.raw_value_x_len + 4 + SENSOR_GET_COLUMN_MAX_LEN * 2);
		column_get = (rtk_bt_mesh_sensor_server_direct_get_column_t *)p_evt->data;
		column_get->property_id = p_get_data.property_id;
		column_get->raw_value_x_len = raw_value_x_len;
		memcpy(column_get->raw_value_x, p_get_data.raw_value_x, raw_value_x_len);
		column_get->value = sensor_column_get_value;
		rtk_bt_evt_indicate(p_evt, NULL);
		p_get_data.column_len = LE_TO_U16(sensor_column_get_value);
		p_get_data.column = (uint8_t *)sensor_column_get_value + 2;

		sensor_column_status(model, ctx, p_get_data.property_id, p_get_data.raw_value_x_len, p_get_data.raw_value_x,
							 p_get_data.column_len, p_get_data.column);
	}

	return 0;
}

static int sensor_srv_series_get(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	uint16_t property_id = net_buf_simple_pull_le16(buf);
	if (property_id != 0) {
		rtk_bt_mesh_sensor_server_get_series_t p_get_data;
		memset(&p_get_data, 0, sizeof(rtk_bt_mesh_sensor_server_get_series_t));
		p_get_data.property_id = property_id;
		if (buf->len > 0) {
			p_get_data.raw_value_x_len = buf->len / 2;
			p_get_data.raw_value_x1 = buf->data;
			p_get_data.raw_value_x2 = buf->data + p_get_data.raw_value_x_len;
		}

		rtk_bt_mesh_sensor_server_direct_get_series_t *series_get;
		uint8_t raw_value_x_len = p_get_data.raw_value_x_len;
		if (p_get_data.raw_value_x_len > SENSOR_GET_SERIES_MAX_LEN) {
			BT_LOGE("[%s] The len of raw value x is %d, extend max SENSOR_GET_SERIES_MAX_LEN:%d\r\n", __func__, p_get_data.raw_value_x_len, SENSOR_GET_SERIES_MAX_LEN);
			raw_value_x_len = SENSOR_GET_SERIES_MAX_LEN;
		}
		uint8_t sensor_series_get_value[SENSOR_GET_SERIES_MAX_LEN + 2];
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_SERIES_GET,
									sizeof(rtk_bt_mesh_sensor_server_direct_get_series_t) + 2 * SENSOR_GET_SERIES_MAX_LEN + 2);
		series_get = (rtk_bt_mesh_sensor_server_direct_get_series_t *)p_evt->data;
		series_get->property_id = p_get_data.property_id;
		series_get->raw_value_x_len = raw_value_x_len;
		memcpy(series_get->raw_value_x1, p_get_data.raw_value_x1, raw_value_x_len);
		memcpy(series_get->raw_value_x2, p_get_data.raw_value_x2, raw_value_x_len);
		series_get->value = sensor_series_get_value;
		rtk_bt_evt_indicate(p_evt, NULL);
		p_get_data.series_len = LE_TO_U16(sensor_series_get_value);
		p_get_data.series = (uint8_t *)sensor_series_get_value + 2;
		sensor_series_status(model, ctx, p_get_data.property_id, p_get_data.series_len, p_get_data.series);
	}

	return 0;
}

const struct bt_mesh_model_op sensor_srv_op[] = {
	{ OP_SENSOR_DESCRIPTOR_GET,    BT_MESH_LEN_MIN(0),   sensor_srv_desc_get },
	{ OP_SENSOR_GET,               BT_MESH_LEN_MIN(0),   sensor_srv_get },
	{ OP_SENSOR_COLUMN_GET,        BT_MESH_LEN_MIN(2),   sensor_srv_colu_get },
	{ OP_SENSOR_SERIES_GET,        BT_MESH_LEN_MIN(2),   sensor_srv_series_get },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_sensor_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_sensor_server_db_set_t *db_get = (rtk_bt_mesh_sensor_server_db_set_t *)p_cmd->param;
		sensor_server_set_db(sensor_server_model, db_get->pdata_db, db_get->num_sensors);
		ret = 0;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

/********************ADD PUBLISH API MAYBE USED BY SENSOR SERVER************************/
// BT_MESH_MODEL_PUB_DEFINE(sensor_server_pub, NULL, BT_MESH_TX_SDU_MAX);
//BT_MESH_MODEL(BT_MESH_MODEL_ID_SENSOR_SRV, &sensor_srv_op, &sensor_server_pub, (void *) &sensors_list),
/***
 * If publish is needed, should set global parameter to use in publish timeout
 * to input parameter for call api to publish
 * or customer can implement this feature by using related parameters in model->pub
 * or can call publish api when receive related msgs
 * or if want to periodic publish, can use model->pub->timer to init timer
 * publish api can use in timer handler
 * **/
int sensor_status_publish(const struct bt_mesh_model *model)
{
	int err;
	uint16_t msg_len = 0;
	sensor_info_t *pinfo = model->rt->user_data;
	for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
		if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
			(pinfo->sensors[i].descriptor.property_id < 2048)) {
			msg_len += 2;
		} else {
			msg_len += 3;
		}
		msg_len += pinfo->sensors[i].sensor_raw_data_len;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_SENSOR_STATUS);
	rtk_bt_mesh_sensor_server_get_t p_get_data = {0, NULL};
	for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
		if ((pinfo->sensors[i].sensor_raw_data_len <= 16) &&
			(pinfo->sensors[i].descriptor.property_id < 2048)) {

			sensor_formata_t pformat;
			pformat.format = 0;
			pformat.length = pinfo->sensors[i].sensor_raw_data_len - 1;
			pformat.property_id = pinfo->sensors[i].descriptor.property_id;
			net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
		} else {

			sensor_formatb_t pformat;
			pformat.format = 1;
			pformat.length = pinfo->sensors[i].sensor_raw_data_len - 1;
			pformat.property_id = pinfo->sensors[i].descriptor.property_id;
			net_buf_simple_add_mem(&pmsg, &pformat, sizeof(sensor_formata_t));
		}

		p_get_data.property_id = pinfo->sensors[i].descriptor.property_id;
		p_get_data.raw_data = NULL;

		uint8_t sensor_get_value[SENSOR_GET_DATA_MAX_LEN * 2];
		rtk_bt_mesh_sensor_server_get_t *sensor_value;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_sensor_server_get_t) + SENSOR_GET_DATA_MAX_LEN * 2 + 2);
		sensor_value = (rtk_bt_mesh_sensor_server_get_t *)p_evt->data;
		sensor_value->property_id = pinfo->sensors[i].descriptor.property_id;
		sensor_value->raw_data = sensor_get_value;
		rtk_bt_evt_indicate(p_evt, NULL);
		if (LE_TO_U16(sensor_get_value)) {
			p_get_data.raw_data = &sensor_get_value[2];
		} else {
			p_get_data.raw_data = NULL;
		}
		//reserve uint16_t to store data len

		if (NULL != p_get_data.raw_data) {
			for (uint16_t j = 0; j < pinfo->sensors[i].sensor_raw_data_len; ++j) {
				net_buf_simple_add_u8(&pmsg, ((uint8_t *)p_get_data.raw_data)[j]);
			}
		}
	}
	/*****If want to use below to publish, when need retransmit 3 times, should set 2******/
	err = bt_mesh_msg_send(model, NULL, &pmsg);
	if (err) {
		BT_LOGE("bt_mesh_model_publish err %d\n", err);
	}
	/*****If want to publish periodicly like spec, use below part
	 * when use below part, should check publication related info is ok first
	 * */
	// net_buf_simple_reset(model->pub->msg);
	// net_buf_simple_add_mem(model->pub->msg, pmsg.data, pmsg.len);
	return err;
}

/***If needed, just follow sensor_status_publish to choose which property id to pulish***/
int sensor_srv_update_publish(const struct bt_mesh_model *model)
{
	(void)model;
	return 0;
}

/*******************ADD PUBLISH API FOR SENSOR SERVER END*************************/
#endif // end of BT_MESH_ENABLE_SENSOR_SERVER_MODEL

/********************SENSOR SETUP SERVER PART**********************/
#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
#define TRIGGER_DELTA_UNITLESS_LEN            2
#define SETTING_PROPERTY_ID_LEN               2

#define SENSOR_PROPERTY_ID_INVALID                        0
#define SENSOR_SETTING_PROPERTY_ID_INVALID                0
#define SENSOR_MEASUREMENT_PERIOD_NOT_APPLICABLE          0
#define IS_SENSOR_PROPERTY_ID_VALID(id)                   ((id) != SENSOR_PROPERTY_ID_INVALID)
#define IS_SENSOR_SETTING_PROPERTY_ID_VALID(id)           ((id) != SENSOR_SETTING_PROPERTY_ID_INVALID)
#define IS_FAST_CADENCE_DIVISIOR_VALID(divisor)           ((divisor) <= 15)
#define IS_STATUS_MIN_INTERVAL_VALID(interval)            ((interval) <= 26)

typedef struct {
	uint16_t property_id;
	uint8_t fast_cadence_period_divisor: 7;
	uint8_t status_trigger_type: 1; /**< @ref sensor_trigger_type_t */
	uint8_t cadence[0];
} _PACKED4_ sensor_cadence_set_t;

enum {
	SENSOR_SETTING_ACCESS_READ_ONLY = 0x01,
	SENSOR_SETTING_ACCESS_READ_WRITE = 0x03,
} _SHORT_ENUM_;
typedef uint8_t sensor_setting_access_t;

static const struct bt_mesh_model *sensor_setup_server_model = NULL;

static int sensor_setup_srv_init(const struct bt_mesh_model *model)
{
	sensor_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_sensor_setup_srv_cb = {
	.init = sensor_setup_srv_init,
};

void sensor_setup_server_set_db(const struct bt_mesh_model *model, rtk_bt_mesh_sensor_db_t *sensors, uint16_t num_sensors)
{
	/*****
	 * When sensor server model init
	 * customer should set a pointer for database in user data, like:
	 * BT_MESH_MODEL(BT_MESH_MODEL_ID_SENSOR_SRV, sensor_srv_op, NULL, &sensors_list),
	 * ****/
	sensor_info_t *pinfo = model->rt->user_data;
	pinfo->sensors = sensors;
	pinfo->num_sensors = num_sensors;
}

static rtk_bt_mesh_sensor_cadence_t *sensor_get_cadence(const struct bt_mesh_model *model, uint16_t property_id)
{
	sensor_info_t *pinfo = model->rt->user_data;
	for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
		if (pinfo->sensors[i].descriptor.property_id == property_id) {
			return pinfo->sensors[i].cadence;
		}
	}

	return NULL;
}

static int sensor_cadence_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
								 uint16_t property_id, const rtk_bt_mesh_sensor_cadence_t *cadence)
{
	int ret;
	if (NULL == cadence) {
		uint16_t msg_len = 2;
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_CADENCE_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_CADENCE_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		uint16_t msg_len = 0;
		uint8_t trigger_len;
		if (0 == cadence->status_trigger_type) {
			msg_len = 2 + sizeof(rtk_bt_mesh_sensor_cadence_t) - sizeof(
						  uint8_t *) * 4 - sizeof(uint8_t) + cadence->raw_value_len * 4;
			trigger_len = cadence->raw_value_len;
		} else {
			msg_len = 2 + sizeof(rtk_bt_mesh_sensor_cadence_t) - sizeof(
						  uint8_t *) * 4  - sizeof(uint8_t) + TRIGGER_DELTA_UNITLESS_LEN * 2 + cadence->raw_value_len * 2;
			trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_CADENCE_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_CADENCE_STATUS);

		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		uint8_t cad_data;
		cad_data = cadence->fast_cadence_period_divisor;
		cad_data |= (cadence->status_trigger_type << 7);
		net_buf_simple_add_u8(&pmsg, cad_data);
		for (uint8_t i = 0; i < trigger_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->status_trigger_delta_down)[i]);
		}

		for (uint8_t i = 0; i < trigger_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->status_trigger_delta_up)[i]);
		}

		net_buf_simple_add_u8(&pmsg, cadence->status_min_interval);

		for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->fast_cadence_low)[i]);
		}

		for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->fast_cadence_high)[i]);
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	}

	return ret;
}

static bool sensor_check_msg(const struct bt_mesh_model *model, const sensor_cadence_set_t *pmsg, uint16_t msg_len)
{
	if (!IS_SENSOR_PROPERTY_ID_VALID(pmsg->property_id)) {
		return FALSE;
	}

	if (!IS_FAST_CADENCE_DIVISIOR_VALID(pmsg->fast_cadence_period_divisor)) {
		return FALSE;
	}

	rtk_bt_mesh_sensor_cadence_t *cadence = sensor_get_cadence(model, pmsg->property_id);
	if (NULL == cadence) {
		return TRUE;
	}

	const uint8_t *pdata = pmsg->cadence;
	uint8_t trigger_len;
	if (0 == pmsg->status_trigger_type) {
		trigger_len = (msg_len - sizeof(uint8_t)) / 4;
	} else {
		trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
	}
	pdata += (trigger_len * 2);

	if (!IS_STATUS_MIN_INTERVAL_VALID(*pdata)) {
		return FALSE;
	}

	return TRUE;
}

static int sensor_settings_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
								  uint16_t property_id, const rtk_bt_mesh_sensor_setting_t *settings, uint16_t num_settings)
{
	int ret;
	if ((NULL == settings) || (0 == num_settings)) {
		uint16_t msg_len = 2;
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTINGS_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTINGS_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		uint16_t msg_len = 2 + num_settings * SETTING_PROPERTY_ID_LEN;
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTINGS_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTINGS_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		for (uint16_t i = 0; i < num_settings; ++i) {
			net_buf_simple_add_mem(&pmsg, &settings[i].setting_property_id, sizeof(uint16_t));
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	}

	return ret;
}

static rtk_bt_mesh_sensor_setting_t *sensor_get_setting(const struct bt_mesh_model *model,
														uint16_t property_id, uint16_t setting_property_id)
{
	sensor_info_t *pinfo = model->rt->user_data;
	for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
		if (pinfo->sensors[i].descriptor.property_id == property_id) {
			for (uint16_t j = 0; j < pinfo->sensors[i].num_settings; ++j) {
				if (pinfo->sensors[i].settings[j].setting_property_id == setting_property_id) {
					return &pinfo->sensors[i].settings[j];
				}
			}
			break;
		}
	}

	return NULL;
}

static int sensor_setting_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
								 uint16_t property_id, uint16_t setting_property_id,
								 bool set_read_only, const rtk_bt_mesh_sensor_setting_t *setting)
{
	int ret;
	if (NULL == setting) {
		uint16_t msg_len = 4;
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTING_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTING_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting_property_id, sizeof(uint16_t));
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		uint16_t msg_len = 0;
		if (!set_read_only) {
			msg_len = 4 + 1 + setting->setting_raw_len;
		} else {
			msg_len = 4 + 1;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTING_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTING_STATUS);

		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting_property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting->setting_access, sizeof(rtk_bt_mesh_sensor_setting_access_t));
		if (!set_read_only) {
			for (uint16_t i = 0; i < setting->setting_raw_len; ++i) {
				net_buf_simple_add_u8(&pmsg, ((uint8_t *)setting->setting_raw)[i]);
			}
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	}

	return ret;
}


/********************ADD PUBLISH API MAYBE USED BY SENSOR SETUP SERVER************************/
// BT_MESH_MODEL_PUB_DEFINE(sensor_server_setup_pub, NULL, BT_MESH_TX_SDU_MAX);
//BT_MESH_MODEL(BT_MESH_MODEL_ID_SENSOR_SETUP_SRV, &sensor_setup_srv_op, &sensor_server_setup_pub, (void *) &sensors_list),

/***
 * If publish is needed, should set global parameter to use in publish timeout
 * to input parameter for call api to publish
 * or customer can implement this feature by using related parameters in model->pub
 * ****/
uint16_t property_id_publish = 0;
uint16_t setting_property_id_publish = 0;
int sensor_cadence_publish(const struct bt_mesh_model *model)
{
	int err;
	rtk_bt_mesh_sensor_cadence_t *cadence = sensor_get_cadence(model, property_id_publish);
	if (NULL == cadence) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_CADENCE_STATUS, 2);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_CADENCE_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id_publish, sizeof(uint16_t));
		/*****If want to use below to publish, when need retransmit 3 times, should set 2******/
		err = bt_mesh_msg_send(model, NULL, &pmsg);
		/*****If want to publish periodicly like spec, use below part
		 * when use below part, should check publication related info is ok first
		 * */
		// net_buf_simple_reset(model->pub->msg);
		// net_buf_simple_add_mem(model->pub->msg, pmsg.data, pmsg.len);
	} else {
		uint16_t msg_len = 0;
		uint8_t trigger_len;
		if (0 == cadence->status_trigger_type) {
			msg_len = 2 + sizeof(rtk_bt_mesh_sensor_cadence_t) - sizeof(
						  uint8_t *) * 4 - sizeof(uint8_t) + cadence->raw_value_len * 4;
			trigger_len = cadence->raw_value_len;
		} else {
			msg_len = 2 + sizeof(rtk_bt_mesh_sensor_cadence_t) - sizeof(
						  uint8_t *) * 4  - sizeof(uint8_t) + TRIGGER_DELTA_UNITLESS_LEN * 2 + cadence->raw_value_len * 2;
			trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_CADENCE_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_CADENCE_STATUS);

		net_buf_simple_add_mem(&pmsg, &property_id_publish, sizeof(uint16_t));
		uint8_t cad_data;
		cad_data = cadence->fast_cadence_period_divisor;
		cad_data |= (cadence->status_trigger_type << 7);
		net_buf_simple_add_u8(&pmsg, cad_data);
		for (uint8_t i = 0; i < trigger_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->status_trigger_delta_down)[i]);
		}

		for (uint8_t i = 0; i < trigger_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->status_trigger_delta_up)[i]);
		}

		net_buf_simple_add_u8(&pmsg, cadence->status_min_interval);

		for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->fast_cadence_low)[i]);
		}

		for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
			net_buf_simple_add_u8(&pmsg, ((uint8_t *)cadence->fast_cadence_high)[i]);
		}
		/*****If want to use below to publish, when need retransmit 3 times, should set 2******/
		err = bt_mesh_msg_send(model, NULL, &pmsg);
		/*****If want to publish periodicly like spec, use below part*******/
		// net_buf_simple_reset(model->pub->msg);
		// net_buf_simple_add_mem(model->pub->msg, pmsg.data, pmsg.len);
	}
	return err;
}

int sensor_setting_publish(const struct bt_mesh_model *model, uint16_t property_id, uint16_t setting_property_id)
{
	int err;
	rtk_bt_mesh_sensor_setting_t *setting = sensor_get_setting(model, property_id_publish, setting_property_id_publish);
	if (NULL == setting) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTING_STATUS, 4);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTING_STATUS);
		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting_property_id, sizeof(uint16_t));
		/*****If want to use below to publish, when need retransmit 3 times, should set 2******/
		err = bt_mesh_msg_send(model, NULL, &pmsg);
		/*****If want to publish periodicly like spec, use below part*******/
		// net_buf_simple_reset(model->pub->msg);
		// net_buf_simple_add_mem(model->pub->msg, pmsg.data, pmsg.len);
	} else {
		uint16_t msg_len = 0;
		if (setting->setting_access == RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_WRITE) {
			msg_len = 4 + 1 + setting->setting_raw_len;
		} else {
			msg_len = 4 + 1;
		}
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SENSOR_SETTING_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_SENSOR_SETTING_STATUS);

		net_buf_simple_add_mem(&pmsg, &property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting_property_id, sizeof(uint16_t));
		net_buf_simple_add_mem(&pmsg, &setting->setting_access, sizeof(rtk_bt_mesh_sensor_setting_access_t));
		if (setting->setting_access == RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_WRITE) {
			for (uint16_t i = 0; i < setting->setting_raw_len; ++i) {
				net_buf_simple_add_u8(&pmsg, ((uint8_t *)setting->setting_raw)[i]);
			}
		}
		/*****If want to use below to publish, when need retransmit 3 times, should set 2******/
		err = bt_mesh_msg_send(model, NULL, &pmsg);
		/*****If want to publish periodicly like spec, use below part
		 * when use below part, should check publication related info is ok first
		 * */
		// net_buf_simple_reset(model->pub->msg);
		// net_buf_simple_add_mem(model->pub->msg, pmsg.data, pmsg.len);
	}
	return err;
}

/***If needed, just follow sensor_cadence_publish
 *  or sensor_setting_publish to choose
 *  which property id and which status to pulish***/
int sensor_setup_srv_update_publish(const struct bt_mesh_model *model)
{
	(void)model;
	return 0;
}

/*******************ADD PUBLISH API FOR SENSOR SETUP SERVER END*************************/

static void mesh_data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	BT_DUMPHEXA("0x", pbuffer, len, false);
}

typedef struct {
	uint16_t property_id;
	/*
	 * just notify new cadence value in database,
	 * do not need to modify it, just use it if needed
	 */
	rtk_bt_mesh_sensor_cadence_t *cadence;
} sensor_server_get_cadence_t;

static int sensor_setup_srv_cadence_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	int ret = 0;
	uint16_t property_id;
	if (buf->len) {
		property_id = net_buf_simple_pull_le16(buf);
		if (property_id != 0) {
			rtk_bt_mesh_sensor_cadence_t *cadence = sensor_get_cadence(model, property_id);
			ret = sensor_cadence_status(model, ctx, property_id, cadence);
		}
	}

	/**
	 * This part is to verify if get the cadence status success from the app layer
	 * when the database is in or above upper app layer
	*/
	sensor_server_get_cadence_t get_data = {property_id, NULL};
	rtk_bt_mesh_sensor_server_get_cadence_t *p_get_data;
	uint8_t cadence_value[SENSOR_CADENCE_DATA_MAX_LEN * 4] = {0};
	uint8_t cb_ret = 0;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_GET,
								sizeof(rtk_bt_mesh_sensor_server_get_cadence_t) + 4 * SENSOR_CADENCE_DATA_MAX_LEN);
	p_get_data = (rtk_bt_mesh_sensor_server_get_cadence_t *)p_evt->data;
	p_get_data->property_id = get_data.property_id;
	p_get_data->cadence = cadence_value;
	rtk_bt_evt_indicate(p_evt, &cb_ret);
	// uint16_t len = LE_TO_U16(&cadence_value[0]);
	/****dump cadence data to p_data*****/
	/*reserve 2 bytes to store msg length for ipc and direct calling*/
	rtk_bt_mesh_sensor_cadence_t cadence_transfer;
	if (LE_TO_U16(&cadence_value[0]) != 0) {
		BT_LOGD("Get cadence len %d \r\n", LE_TO_U16(&cadence_value[0]));
		cadence_transfer.raw_value_len = cadence_value[2];
		cadence_transfer.fast_cadence_period_divisor = cadence_value[3] & 0x7F;
		cadence_transfer.status_trigger_type = (cadence_value[3] & 0x80) >> 7;
		BT_LOGD("fast_cadence_period_divisor: %d, status_trigger_type :%d \r\n",
				cadence_transfer.fast_cadence_period_divisor, cadence_transfer.status_trigger_type);
		cadence_transfer.status_trigger_delta_down = &cadence_value[4];
		uint16_t trigger_len = 0;
		if (cadence_transfer.status_trigger_type) {
			trigger_len = 2;
		} else {
			trigger_len = cadence_transfer.raw_value_len;
		}
		BT_LOGD("status_trigger_delta_down: \r\n");
		mesh_data_uart_dump(cadence_transfer.status_trigger_delta_down, trigger_len);
		cadence_transfer.status_trigger_delta_up = &cadence_value[4 + trigger_len];
		BT_LOGD("status_trigger_delta_up: \r\n");
		mesh_data_uart_dump(cadence_transfer.status_trigger_delta_up, trigger_len);
		cadence_transfer.status_min_interval = cadence_value[4 + 2 * trigger_len];
		BT_LOGD("status_min_interval: %d \r\n", cadence_transfer.status_min_interval);
		cadence_transfer.fast_cadence_low = &cadence_value[5 + 2 * trigger_len];
		BT_LOGD("fast_cadence_low: \r\n");
		mesh_data_uart_dump(cadence_transfer.fast_cadence_low, cadence_transfer.raw_value_len);
		cadence_transfer.fast_cadence_high = &cadence_value[5 + 2 * trigger_len + cadence_value[2]];
		BT_LOGD("fast_cadence_high: \r\n");
		mesh_data_uart_dump(cadence_transfer.fast_cadence_high, cadence_transfer.raw_value_len);
		get_data.cadence = &cadence_transfer;
	} else {
		get_data.cadence = NULL;
	}

	if (get_data.cadence != NULL) {
		BT_LOGD("fast_cadence_period_divisor: %d, status_trigger_type :%d \r\n",
				get_data.cadence->fast_cadence_period_divisor, get_data.cadence->status_trigger_type);
		uint8_t trigger_len;
		if (get_data.cadence->status_trigger_type == 0) {
			trigger_len = get_data.cadence->raw_value_len;
		} else {
			trigger_len = 2;
		}
		BT_LOGD("status_trigger_delta_down: \r\n");
		mesh_data_uart_dump(get_data.cadence->status_trigger_delta_down, trigger_len);
		BT_LOGD("status_trigger_delta_up: \r\n");
		mesh_data_uart_dump(get_data.cadence->status_trigger_delta_up, trigger_len);
		BT_LOGD("status_min_interval: %d \r\n", get_data.cadence->status_min_interval);
		BT_LOGD("fast_cadence_low: \r\n");
		mesh_data_uart_dump(get_data.cadence->fast_cadence_low, get_data.cadence->raw_value_len);
		BT_LOGD("fast_cadence_high: \r\n");
		mesh_data_uart_dump(get_data.cadence->fast_cadence_high, get_data.cadence->raw_value_len);
	}

	return ret;
}

static int sensor_setup_srv_cadence_set_unack(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)ctx;
	sensor_cadence_set_t *pmsg = (sensor_cadence_set_t *)buf->data;
	if (sensor_check_msg(model, pmsg, buf->len)) {
		rtk_bt_mesh_sensor_cadence_t *cadence = sensor_get_cadence(model, pmsg->property_id);
		if (NULL != cadence) {
			uint8_t *pdata = pmsg->cadence;
			uint8_t trigger_len;
			cadence->fast_cadence_period_divisor = pmsg->fast_cadence_period_divisor;
			cadence->status_trigger_type = pmsg->status_trigger_type;

			uint8_t raw_value_len;
			if (0 == pmsg->status_trigger_type) {
				raw_value_len = (buf->len - sizeof(uint8_t) - 3) / 4;
				cadence->raw_value_len = raw_value_len;
				trigger_len = cadence->raw_value_len;
			} else {
				raw_value_len = (buf->len - sizeof(uint8_t) - 2 * TRIGGER_DELTA_UNITLESS_LEN - 3) / 2;
				cadence->raw_value_len = raw_value_len;
				trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
			}

			for (uint8_t i = 0; i < trigger_len; ++i) {
				((uint8_t *)cadence->status_trigger_delta_down)[i] = *pdata ++;
			}

			for (uint8_t i = 0; i < trigger_len; ++i) {
				((uint8_t *)cadence->status_trigger_delta_up)[i] = *pdata ++;
			}

			cadence->status_min_interval = *pdata ++;

			for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
				((uint8_t *)cadence->fast_cadence_low)[i] = *pdata ++;
			}

			for (uint8_t i = 0; i < cadence->raw_value_len; ++i) {
				((uint8_t *)cadence->fast_cadence_high)[i] = *pdata ++;
			}
			rtk_bt_mesh_sensor_server_set_cadence_t p_set_data = {pmsg->property_id, cadence};
			// uint8_t trigger_len;
			raw_value_len = (p_set_data.cadence)->raw_value_len;
			if (0 == (p_set_data.cadence)->status_trigger_type) {
				trigger_len = (p_set_data.cadence)->raw_value_len;
			} else {
				trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
			}
			uint32_t len = (p_set_data.cadence)->raw_value_len * 2 + trigger_len * 2 + 5;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_SET,
										len);
			memcpy(p_evt->data, &p_set_data.property_id, 2);
			memcpy((uint8_t *)(p_evt->data) + 2, p_set_data.cadence, 2);
			memcpy((uint8_t *)(p_evt->data) + 4, (p_set_data.cadence)->status_trigger_delta_down, trigger_len);
			memcpy((uint8_t *)(p_evt->data) + 4 + trigger_len, (p_set_data.cadence)->status_trigger_delta_up, trigger_len);
			memcpy((uint8_t *)(p_evt->data) + 4 + 2 * trigger_len, &(p_set_data.cadence)->status_min_interval, 1);
			memcpy((uint8_t *)(p_evt->data) + 5 + 2 * trigger_len, (p_set_data.cadence)->fast_cadence_low, raw_value_len);
			memcpy((uint8_t *)(p_evt->data) + 5 + 2 * trigger_len + raw_value_len, (p_set_data.cadence)->fast_cadence_high, raw_value_len);
			rtk_bt_evt_indicate(p_evt, NULL);
		}
		return 0;
	}

	return -1;
}


static int sensor_setup_srv_cadence_set(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	uint16_t property_id;
	if (buf->len) {
		property_id = LE_TO_U16(buf->data);
	}
	if (0 == sensor_setup_srv_cadence_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_sensor_cadence_t *cadence = sensor_get_cadence(model, property_id);
		return sensor_cadence_status(model, ctx, property_id, cadence);
	}
	return 0;
}

static int sensor_setup_srv_settings_get(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	uint16_t property_id;
	if (buf->len) {
		property_id = net_buf_simple_pull_le16(buf);
		if (IS_SENSOR_PROPERTY_ID_VALID(property_id)) {
			sensor_info_t *pinfo = model->rt->user_data;
			rtk_bt_mesh_sensor_setting_t *psettings = NULL;
			uint16_t num_settings = 0;
			for (uint16_t i = 0; i < pinfo->num_sensors; ++i) {
				if (pinfo->sensors[i].descriptor.property_id == property_id) {
					psettings = pinfo->sensors[i].settings;
					num_settings = pinfo->sensors[i].num_settings;
					break;
				}
			}
			sensor_settings_status(model, ctx, property_id, psettings, num_settings);
		}
	}

	/**
	 * This part is to verify if get the settings status success from the app layer
	 * when the database is in or above upper app layer
	*/
	rtk_bt_mesh_sensor_server_get_settings_t settings_get = {property_id, NULL};
	uint16_t settings_store[SENSOR_SETTINGS_DATA_MAX_LEN];
	rtk_bt_mesh_sensor_server_get_settings_t *p_get_data;
	uint8_t cb_ret = 0;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTINGS_GET,
								sizeof(rtk_bt_mesh_sensor_server_get_settings_t) + SENSOR_SETTINGS_DATA_MAX_LEN);
	p_get_data = (rtk_bt_mesh_sensor_server_get_settings_t *)p_evt->data;
	p_get_data->property_id = settings_get.property_id;
	p_get_data->settings_data = settings_store;
	rtk_bt_evt_indicate(p_evt, &cb_ret);
	settings_get.settings_data = settings_store;

	uint16_t settings_num = settings_get.settings_data[0];
	BT_DUMP16A("setting id:", &settings_get.settings_data[1], settings_num);

	return 0;
}

typedef struct {
	uint16_t property_id;
	rtk_bt_mesh_sensor_setting_t *setting;
} sensor_server_get_setting_t;

static int sensor_setup_srv_setting_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	int ret = 0;
	uint16_t property_id;
	if (buf->len) {
		property_id = net_buf_simple_pull_le16(buf);
		if (IS_SENSOR_PROPERTY_ID_VALID(property_id)) {
			uint16_t setting_property_id = net_buf_simple_pull_le16(buf);
			rtk_bt_mesh_sensor_setting_t *setting = sensor_get_setting(model, property_id, setting_property_id);
			ret = sensor_setting_status(model, ctx, property_id, setting_property_id, FALSE, setting);
			/**
			 * This part is to verify if get the setting status success from the app layer
			 * when the database is in or above upper app layer
			*/
			rtk_bt_mesh_sensor_setting_t setting_get_value = {0};
			setting_get_value.setting_property_id = setting_property_id;

			rtk_bt_mesh_sensor_server_get_setting_t *p_get_data;
			uint8_t setting_store[SENSOR_SETTING_DATA_MAX_LEN];
			uint8_t cb_ret = 0;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_GET,
										sizeof(rtk_bt_mesh_sensor_server_get_setting_t) + SENSOR_SETTING_DATA_MAX_LEN);
			p_get_data = (rtk_bt_mesh_sensor_server_get_setting_t *)p_evt->data;
			p_get_data->property_id = property_id;
			p_get_data->setting_property_id = setting_property_id;
			p_get_data->setting_data = setting_store;
			rtk_bt_evt_indicate(p_evt, &cb_ret);

			setting_get_value.setting_access = setting_store[0];
			setting_get_value.setting_raw_len = setting_store[1];
			if (setting_get_value.setting_raw_len == 0) {
				setting_get_value.setting_raw = NULL;
			} else {
				setting_get_value.setting_raw = &setting_store[2];
			}

			if (setting_get_value.setting_access != 0) {
				BT_LOGD("access: %d, raw value len %d, raw value:",
						setting_get_value.setting_access,
						setting_get_value.setting_raw_len);
				mesh_data_uart_dump(setting_get_value.setting_raw, setting_get_value.setting_raw_len);
			}
		}
	}

	return ret;
}

static int sensor_setup_srv_setting_set_unack(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)ctx;
	uint16_t property_id = net_buf_simple_pull_le16(buf);
	uint16_t setting_property_id = net_buf_simple_pull_le16(buf);
	if (IS_SENSOR_PROPERTY_ID_VALID(property_id) &&
		IS_SENSOR_SETTING_PROPERTY_ID_VALID(setting_property_id)) {
		rtk_bt_mesh_sensor_setting_t *setting = sensor_get_setting(model, property_id, setting_property_id);
		// bool set_read_only = FALSE;
		if (NULL != setting) {
			if (RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_WRITE == setting->setting_access) {
				uint16_t data_len = buf->len;
				for (uint16_t i = 0; i < data_len; ++i) {
					setting->setting_raw_len = data_len;
					((uint8_t *)setting->setting_raw)[i] = net_buf_simple_pull_u8(buf);
				}
				rtk_bt_mesh_sensor_server_set_setting_t p_set_data = {property_id, setting};
				uint32_t len = (p_set_data.setting)->setting_raw_len + 6;
				rtk_bt_evt_t *p_evt = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_SET,
											len);
				memcpy(p_evt->data, &p_set_data.property_id, 2);
				memcpy((uint8_t *)(p_evt->data) + 2, &(p_set_data.setting->setting_property_id), 2);
				memcpy((uint8_t *)(p_evt->data) + 4, &(p_set_data.setting->setting_access), 1);
				memcpy((uint8_t *)(p_evt->data) + 5, &(p_set_data.setting->setting_raw_len), 1);
				memcpy((uint8_t *)(p_evt->data) + 6, p_set_data.setting->setting_raw, (p_set_data.setting)->setting_raw_len);
				rtk_bt_evt_indicate(p_evt, NULL);
			} else {
				// set_read_only = TRUE;
			}
		}
		return 0;
	}
	return -1;
}

static int sensor_setup_srv_setting_set(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	uint16_t property_id;
	uint16_t setting_property_id;
	if (buf->len > 4) {
		property_id = LE_TO_U16(buf->data);
		setting_property_id = LE_TO_U16(&buf->data[2]);
	}
	if (0 == sensor_setup_srv_setting_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_sensor_setting_t *setting = sensor_get_setting(model, property_id, setting_property_id);
		bool set_read_only = FALSE;
		if (NULL != setting) {
			if (RTK_BT_MESH_SENSOR_SETTING_ACCESS_READ_WRITE == setting->setting_access) {
				set_read_only = FALSE;
			} else {
				set_read_only = TRUE;
			}
		}

		sensor_setting_status(model, ctx, property_id, setting_property_id, set_read_only, setting);
	}

	return 0;
}

const struct bt_mesh_model_op sensor_setup_srv_op[] = {
	{ OP_SENSOR_CADENCE_GET,       BT_MESH_LEN_EXACT(2), sensor_setup_srv_cadence_get },
	{ OP_SENSOR_CADENCE_SET,       BT_MESH_LEN_MIN(4),   sensor_setup_srv_cadence_set },
	{ OP_SENSOR_CADENCE_SET_UNACK, BT_MESH_LEN_MIN(4),   sensor_setup_srv_cadence_set_unack },
	{ OP_SENSOR_SETTINGS_GET,      BT_MESH_LEN_MIN(2),   sensor_setup_srv_settings_get },
	{ OP_SENSOR_SETTING_GET,       BT_MESH_LEN_MIN(2),   sensor_setup_srv_setting_get },
	{ OP_SENSOR_SETTING_SET,       BT_MESH_LEN_MIN(2),   sensor_setup_srv_setting_set },
	{ OP_SENSOR_SETTING_SET_UNACK, BT_MESH_LEN_MIN(2),   sensor_setup_srv_setting_set_unack },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_sensor_setup_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SENSOR_SETUP_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_sensor_server_db_set_t *db_get = (rtk_bt_mesh_sensor_server_db_set_t *)p_cmd->param;
		sensor_server_set_db(sensor_setup_server_model, db_get->pdata_db, db_get->num_sensors);
		ret = 0;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif // end of BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL

/*******************SENSOR CLIENT PART**********************/
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
static const struct bt_mesh_model *sensor_client_model = NULL;

static int sensor_cli_init(const struct bt_mesh_model *model)
{
	sensor_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_sensor_cli_cb = {
	.init = sensor_cli_init,
};

#define TRIGGER_DELTA_UNITLESS_LEN            2

static int sensor_cli_desc_status(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_descriptor_t status = {ctx->addr, 0, 0, NULL};
	if (buf->len > 1) {
		if (buf->len == 2) {
			status.property_id = net_buf_simple_pull_le16(buf);
		} else {
			status.num_descriptors = buf->len / sizeof(rtk_bt_mesh_sensor_descriptor_t);
			status.descriptors = (rtk_bt_mesh_sensor_descriptor_t *)buf->data;
		}
		rtk_bt_evt_t *p_evt = NULL;
		uint32_t len = 6 + status.num_descriptors * sizeof(rtk_bt_mesh_sensor_descriptor_t);
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_STATUS,
									len);
		memcpy(p_evt->data, &status, 6);
		memcpy((uint8_t *)p_evt->data + 6, status.descriptors, status.num_descriptors * sizeof(rtk_bt_mesh_sensor_descriptor_t));
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	return 0;
}

static int sensor_cli_status(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_t status_data = {ctx->addr, buf->len, buf->data};
	rtk_bt_evt_t *p_evt = NULL;
	uint32_t len = status_data.marshalled_sensor_data_len + 4;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CLIENT_MODEL_STATUS,
								len);
	memcpy(p_evt->data, &status_data, 4);
	memcpy((uint8_t *)p_evt->data + 4, status_data.marshalled_sensor_data, len - 4);
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int sensor_cli_column_status(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_column_t colum_stat = {ctx->addr, 0, 0, NULL};
	colum_stat.property_id = net_buf_simple_pull_le16(buf);
	colum_stat.column_raw_value_len = buf->len;
	colum_stat.column_raw_value = buf->data;
	rtk_bt_evt_t *p_evt = NULL;
	uint32_t len = colum_stat.column_raw_value_len + 6;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_STATUS,
								len);
	memcpy(p_evt->data, &colum_stat, 6);
	memcpy((uint8_t *)p_evt->data + 6, colum_stat.column_raw_value, len - 6);
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int sensor_cli_series_status(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_series_t status_data = {ctx->addr, 0, 0, NULL};
	status_data.property_id = net_buf_simple_pull_le16(buf);
	status_data.series_raw_value_len = buf->len;
	status_data.series_raw_value = buf->data;
	rtk_bt_evt_t *p_evt = NULL;
	uint32_t len = status_data.series_raw_value_len + 6;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_STATUS,
								len);
	memcpy(p_evt->data, &status_data, 6);
	memcpy((uint8_t *)p_evt->data + 6, status_data.series_raw_value, len - 6);
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int sensor_cli_cadence_status(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_cadence_t status_data = {ctx->addr, 0, FALSE, NULL};
	rtk_bt_mesh_sensor_cadence_t cadence;
	if (buf->len > 2) {
		status_data.property_id = net_buf_simple_pull_le16(buf);
		status_data.flag = TRUE;
		uint8_t divisor_trigger = net_buf_simple_pull_u8(buf);
		cadence.fast_cadence_period_divisor = (divisor_trigger & 0x7f);
		cadence.status_trigger_type = (divisor_trigger >> 7);
		uint8_t trigger_len = 0;
		if (0 == cadence.status_trigger_type) {
			trigger_len = (buf->len - 1) / 4;
			cadence.raw_value_len = trigger_len;
		} else {
			trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
			cadence.raw_value_len = (buf->len - 1 - trigger_len * 2) / 2;
		}
		cadence.status_trigger_delta_down = buf->data;
		cadence.status_trigger_delta_up = (uint8_t *)cadence.status_trigger_delta_down + trigger_len;
		cadence.status_min_interval = *((uint8_t *)cadence.status_trigger_delta_up + trigger_len);
		cadence.fast_cadence_low = (uint8_t *)cadence.status_trigger_delta_up + trigger_len + sizeof(
									   uint8_t);
		cadence.fast_cadence_high = (uint8_t *)cadence.fast_cadence_low + cadence.raw_value_len;
		status_data.cadence = &cadence;
	} else {
		status_data.property_id = net_buf_simple_pull_le16(buf);
	}

	if (status_data.cadence != NULL) {
		uint8_t status_trigger_type = status_data.cadence->status_trigger_type;
		uint8_t trigger_len = 0;
		uint8_t raw_value_len = status_data.cadence->raw_value_len;
		if (0 == status_trigger_type) {
			trigger_len = status_data.cadence->raw_value_len;
		} else {
			trigger_len = TRIGGER_DELTA_UNITLESS_LEN;
		}
		uint32_t len = 9 + 2 * status_data.cadence->raw_value_len + 2 * trigger_len;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS,
									len);
		memcpy(p_evt->data, & status_data.src, 2);
		memcpy((uint8_t *)p_evt->data + 2, &status_data.property_id, 2);
		memcpy((bool *)p_evt->data + 4, &status_data.flag, 1);
		memcpy((uint8_t *)p_evt->data + 5, & status_data.cadence->raw_value_len, 2);
		memcpy((uint8_t *)p_evt->data + 7, status_data.cadence->status_trigger_delta_down, trigger_len);
		memcpy((uint8_t *)p_evt->data + 7 + trigger_len, status_data.cadence->status_trigger_delta_up, trigger_len);
		memcpy((uint8_t *)p_evt->data + 7 + 2 * trigger_len, &status_data.cadence->status_min_interval, 1);
		memcpy((uint8_t *)p_evt->data + 8 + 2 * trigger_len, status_data.cadence->fast_cadence_low, raw_value_len);
		memcpy((uint8_t *)p_evt->data + 8 + 2 * trigger_len + raw_value_len, status_data.cadence->fast_cadence_high, raw_value_len);
		rtk_bt_evt_indicate(p_evt, NULL);
	} else {
		uint32_t len = 5;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS,
									len);
		memcpy(p_evt->data, &status_data.src, 2);
		memcpy((uint8_t *)p_evt->data + 2, &status_data.property_id, 2);
		memcpy((bool *)p_evt->data + 4, &status_data.flag, 1);
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	return 0;
}

static int sensor_cli_settings_status(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_settings_t settings_stat = {ctx->addr, 0, 0, NULL};
	settings_stat.property_id = net_buf_simple_pull_le16(buf);
	settings_stat.num_ids = buf->len / 2;
	settings_stat.setting_ids = (uint16_t *)buf->data;
	rtk_bt_evt_t *p_evt = NULL;
	uint32_t len = settings_stat.num_ids * 2 + 6;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_STATUS,
								len);
	memcpy(p_evt->data, &settings_stat, 6);
	memcpy((uint8_t *)p_evt->data + 6, settings_stat.setting_ids, len - 6);
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int sensor_cli_setting_status(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_sensor_client_status_setting_t status_data = {ctx->addr, 0, 0, FALSE, NULL};
	status_data.property_id = net_buf_simple_pull_le16(buf);
	status_data.setting_property_id = net_buf_simple_pull_le16(buf);
	rtk_bt_mesh_sensor_setting_t setting;
	if (buf->len) {
		status_data.flag = TRUE;
		setting.setting_property_id = status_data.setting_property_id;
		setting.setting_access = (rtk_bt_mesh_sensor_setting_access_t) net_buf_simple_pull_u8(buf);
		setting.setting_raw_len = buf->len;
		setting.setting_raw = buf->data;
		status_data.setting = &setting;
	}
	rtk_bt_evt_t *p_evt = NULL;
	// bool flag = 0;
	uint32_t len = 0;
	if (status_data.flag) {
		len = 11 + status_data.setting->setting_raw_len;
	} else {
		len = 7;
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS,
								len);
	memcpy(p_evt->data, &status_data, 7);
	// memcpy((bool *)p_evt->data + 6, &flag, 1);
	if (status_data.flag) {
		memcpy((uint8_t *)p_evt->data + 7, &status_data.setting->setting_property_id, 4);
		memcpy((uint8_t *)p_evt->data + 11, status_data.setting->setting_raw, status_data.setting->setting_raw_len);
	}
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

const struct bt_mesh_model_op sensor_cli_op[] = {
	{ OP_SENSOR_DESCRIPTOR_STATUS,   BT_MESH_LEN_MIN(2),   sensor_cli_desc_status },
	{ OP_SENSOR_STATUS,              BT_MESH_LEN_MIN(0),   sensor_cli_status },
	{ OP_SENSOR_COLUMN_STATUS,       BT_MESH_LEN_MIN(2),   sensor_cli_column_status },
	{ OP_SENSOR_SERIES_STATUS,       BT_MESH_LEN_MIN(2),   sensor_cli_series_status },
	{ OP_SENSOR_CADENCE_STATUS,      BT_MESH_LEN_MIN(4),   sensor_cli_cadence_status },
	{ OP_SENSOR_SETTINGS_STATUS,     BT_MESH_LEN_MIN(2),   sensor_cli_settings_status },
	{ OP_SENSOR_SETTING_STATUS,      BT_MESH_LEN_MIN(5),   sensor_cli_setting_status },
	BT_MESH_MODEL_OP_END,
};

/********SEND MSG API**********/
static int sensor_descriptor_get(const struct bt_mesh_model *model, uint16_t dst,
								 uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	if (0 == property_id) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_DESCRIPTOR_GET, 0);
		bt_mesh_model_msg_init(&buf, OP_SENSOR_DESCRIPTOR_GET);
		return bt_mesh_msg_send(model, &ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_DESCRIPTOR_GET, 2);
		bt_mesh_model_msg_init(&buf, OP_SENSOR_DESCRIPTOR_GET);
		net_buf_simple_add_mem(&buf, &property_id, 2);
		return bt_mesh_msg_send(model, &ctx, &buf);
	}
}

static int sensor_cadence_get(const struct bt_mesh_model *model, uint16_t dst,
							  uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_CADENCE_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_SENSOR_CADENCE_GET);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_cadence_set(const struct bt_mesh_model *model, uint16_t dst,
							  uint16_t app_key_index, uint16_t property_id, const rtk_bt_mesh_sensor_cadence_t *cadence, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint8_t cad_data;
	cad_data = cadence->fast_cadence_period_divisor;
	cad_data |= (cadence->status_trigger_type << 7);
	uint16_t trigger_len;
	if (cadence->status_trigger_type == 0) {
		trigger_len = cadence->raw_value_len;
	} else {
		trigger_len = 2;
	}
	uint16_t msg_len = 2 + 1 + 2 * trigger_len + 1 + 2 * cadence->raw_value_len;
	uint32_t op = ack ? OP_SENSOR_CADENCE_SET : OP_SENSOR_CADENCE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_CADENCE_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	net_buf_simple_add_u8(&buf, cad_data);
	net_buf_simple_add_mem(&buf, cadence->status_trigger_delta_down, trigger_len);
	net_buf_simple_add_mem(&buf, cadence->status_trigger_delta_up, trigger_len);
	net_buf_simple_add_u8(&buf, cadence->status_min_interval);
	net_buf_simple_add_mem(&buf, cadence->fast_cadence_low, cadence->raw_value_len);
	net_buf_simple_add_mem(&buf, cadence->fast_cadence_high, cadence->raw_value_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_settings_get(const struct bt_mesh_model *model, uint16_t dst,
							   uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_SETTINGS_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_SENSOR_SETTINGS_GET);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_setting_get(const struct bt_mesh_model *model, uint16_t dst,
							  uint16_t app_key_index, uint16_t property_id, uint16_t setting_property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_SETTING_GET, 4);
	bt_mesh_model_msg_init(&buf, OP_SENSOR_SETTING_GET);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	net_buf_simple_add_mem(&buf, &setting_property_id, 2);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_setting_set(const struct bt_mesh_model *model, uint16_t dst,
							  uint16_t app_key_index, uint16_t property_id, uint16_t setting_property_id,
							  uint8_t setting_raw_len, const void *setting_raw, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_SENSOR_SETTING_SET : OP_SENSOR_SETTING_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_SETTING_SET, 4 + setting_raw_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	net_buf_simple_add_mem(&buf, &setting_property_id, 2);
	net_buf_simple_add_mem(&buf, setting_raw, setting_raw_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_get(const struct bt_mesh_model *model, uint16_t dst,
					  uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	if (0 == property_id) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_GET, 0);
		bt_mesh_model_msg_init(&buf, OP_SENSOR_GET);
		return bt_mesh_msg_send(model, &ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_GET, 2);
		bt_mesh_model_msg_init(&buf, OP_SENSOR_GET);
		net_buf_simple_add_mem(&buf, &property_id, 2);
		return bt_mesh_msg_send(model, &ctx, &buf);
	}
}

static int sensor_column_get(const struct bt_mesh_model *model, uint16_t dst,
							 uint16_t app_key_index, uint16_t property_id, uint8_t raw_value_x_len, const void *raw_value_x)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_COLUMN_GET, 2 + raw_value_x_len);
	bt_mesh_model_msg_init(&buf, OP_SENSOR_COLUMN_GET);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	net_buf_simple_add_mem(&buf, raw_value_x, raw_value_x_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int sensor_series_get(const struct bt_mesh_model *model, uint16_t dst,
							 uint16_t app_key_index, uint16_t property_id, uint8_t raw_value_x_len, const void *raw_value_x1,
							 const void *raw_value_x2)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SENSOR_SERIES_GET, 2 + raw_value_x_len * 2);
	bt_mesh_model_msg_init(&buf, OP_SENSOR_SERIES_GET);
	net_buf_simple_add_mem(&buf, &property_id, 2);
	net_buf_simple_add_mem(&buf, raw_value_x1, raw_value_x_len);
	net_buf_simple_add_mem(&buf, raw_value_x2, raw_value_x_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

/************ADD ACT HANDLER HERE**************/
uint16_t bt_mesh_sensor_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	int ret = -1;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_descriptor_get_t *sensor_des_get = (rtk_bt_mesh_sensor_descriptor_get_t *)p_cmd->param;
		ret = sensor_descriptor_get(sensor_client_model, sensor_des_get->dst, sensor_des_get->app_key_index,
									sensor_des_get->property_id);
		break;
	}
	case RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_cadence_get_t *sensor_cad_get = (rtk_bt_mesh_sensor_cadence_get_t *)p_cmd->param;
		ret = sensor_cadence_get(sensor_client_model, sensor_cad_get->dst, sensor_cad_get->app_key_index,
								 sensor_cad_get->property_id);
		break;
	}
	case RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_sensor_cadence_set_t *sensor_cad_set = (rtk_bt_mesh_sensor_cadence_set_t *)p_cmd->param;
		rtk_bt_mesh_sensor_cadence_t cadence;
		cadence.fast_cadence_period_divisor = sensor_cad_set->fast_cadence_period_divisor;
		cadence.status_trigger_type = sensor_cad_set->status_trigger_type;
		cadence.raw_value_len = sensor_cad_set->raw_value_len;
		cadence.status_trigger_delta_down = sensor_cad_set->trigger_down;
		cadence.status_trigger_delta_up = sensor_cad_set->trigger_up;
		cadence.status_min_interval = sensor_cad_set->status_min_interval;
		cadence.fast_cadence_low = sensor_cad_set->cadence_low;
		cadence.fast_cadence_high = sensor_cad_set->cadence_high;
		ret = sensor_cadence_set(sensor_client_model, sensor_cad_set->dst, sensor_cad_set->app_key_index,
								 sensor_cad_set->property_id, &cadence, sensor_cad_set->ack);
		break;
	}
	case RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_settings_get_t *sensor_sets_get = (rtk_bt_mesh_sensor_settings_get_t *)p_cmd->param;
		ret = sensor_settings_get(sensor_client_model, sensor_sets_get->dst, sensor_sets_get->app_key_index,
								  sensor_sets_get->property_id);
		break;
	}
	case RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_setting_get_t *sensor_set_get = (rtk_bt_mesh_sensor_setting_get_t *)p_cmd->param;
		ret = sensor_setting_get(sensor_client_model, sensor_set_get->dst, sensor_set_get->app_key_index,
								 sensor_set_get->property_id, sensor_set_get->setting_property_id);
		break;
	}
	case RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_SET: {
		rtk_bt_mesh_sensor_setting_set_t *sensor_set_set = (rtk_bt_mesh_sensor_setting_set_t *)p_cmd->param;
		ret = sensor_setting_set(sensor_client_model, sensor_set_set->dst, sensor_set_set->app_key_index,
								 sensor_set_set->property_id, sensor_set_set->setting_property_id,
								 sensor_set_set->setting_raw_len, (void *)&sensor_set_set->setting_raw,
								 sensor_set_set->ack);
		break;
	}
	case RTK_BT_MESH_SENSOR_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_get_t *sensor_g = (rtk_bt_mesh_sensor_get_t *)p_cmd->param;
		ret = sensor_get(sensor_client_model, sensor_g->dst, sensor_g->app_key_index, sensor_g->property_id);
		break;
	}
	case RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_column_get_t *sensor_col_get = (rtk_bt_mesh_sensor_column_get_t *)p_cmd->param;
		ret = sensor_column_get(sensor_client_model, sensor_col_get->dst, sensor_col_get->app_key_index,
								sensor_col_get->property_id, sensor_col_get->raw_value_x_len,
								(void *)&sensor_col_get->raw_value_x);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_GET: {
		rtk_bt_mesh_sensor_series_get_t *sensor_ser_get = (rtk_bt_mesh_sensor_series_get_t *)p_cmd->param;
		ret = sensor_series_get(sensor_client_model, sensor_ser_get->dst, sensor_ser_get->app_key_index,
								sensor_ser_get->property_id, sensor_ser_get->raw_value_x_len,
								(void *)&sensor_ser_get->raw_value_x1, (void *)&sensor_ser_get->raw_value_x2);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif  // BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
#endif // end of RTK_BLE_MESH_SUPPORT