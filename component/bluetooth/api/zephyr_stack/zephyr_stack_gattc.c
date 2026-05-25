/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <zephyr_stack_api.h>
#include <zephyr_stack_gatt.h>
#include <zephyr_stack_internal.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <stack/host/conn_internal.h>

static void *subscribe_mutex;
static struct list_head subscribe_list;

void gattc_add_subscribe(zephyr_bt_subscribe_t *sub)
{
	osif_mutex_take(subscribe_mutex, BT_TIMEOUT_FOREVER);
	list_add_tail(&sub->list, &subscribe_list);
	osif_mutex_give(subscribe_mutex);
}

void gattc_delete_subscribe(zephyr_bt_subscribe_t *sub)
{
	osif_mutex_take(subscribe_mutex, BT_TIMEOUT_FOREVER);
	list_del(&sub->list);
	osif_mutex_give(subscribe_mutex);
	osif_mem_free(sub);
}

void bt_zephyr_mtu_exchange_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_exchange_params *params)
{
	(void)params;
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_exchange_ind = NULL;
	rtk_bt_evt_t *p_evt_t = NULL;

	p_evt_t = rtk_bt_event_create(RTK_BT_LE_GP_GATTC,
								  RTK_BT_GATTC_EVT_MTU_EXCHANGE,
								  sizeof(rtk_bt_gatt_mtu_exchange_ind_t));
	if (!p_evt_t) {
		return;
	}

	p_gatt_mtu_exchange_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)p_evt_t->data;
	if (err) {
		p_gatt_mtu_exchange_ind->result = RTK_BT_FAIL;
	} else {
		p_gatt_mtu_exchange_ind->result = RTK_BT_OK;
	}
	p_gatt_mtu_exchange_ind->conn_handle = conn->handle;
	p_gatt_mtu_exchange_ind->mtu_size = bt_gatt_get_mtu(conn);

	rtk_bt_evt_indicate(p_evt_t, NULL);
}


struct bt_gatt_exchange_params mtu_param = {
	.func = bt_zephyr_mtu_exchange_cb
};

static uint16_t bt_stack_gattc_exchange_mtu(void *param)
{
	uint16_t conn_handle = *(uint16_t *)param;
	struct bt_conn *conn = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);

	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_gatt_exchange_mtu(conn, &mtu_param);

	bt_conn_unref(conn);

	if (err) {
		return RTK_BT_ERR_LOWER_STACK_API;
	} else {
		return 0;
	}

}

static void _get_uuid_from_zephyr(uint8_t *type, uint8_t *val, const struct bt_uuid *uuid)
{
	*type = uuid->type;
	if (uuid->type == BT_UUID_TYPE_16) {
		memcpy(val, &(((struct bt_uuid_16 *)uuid)->val), sizeof(uint16_t));
	} else {
		memcpy(val, ((struct bt_uuid_128 *)uuid)->val, BT_UUID_SIZE_128);
	}
}

static uint8_t discover_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
	zephyr_bt_disc_req_t *req = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gattc_discover_ind_t *p_ind = NULL;
	uint16_t profile_id;
	rtk_bt_gattc_discover_type_t type;

	if (!params) {
		return BT_GATT_ITER_STOP;
	}

	req = CONTAINER_OF(params, zephyr_bt_disc_req_t, params);

	profile_id = req->profile_id;
	type = req->type;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTC, RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND, sizeof(rtk_bt_gattc_discover_ind_t));
	if (!p_evt) {
		osif_mem_free(req);
		return BT_GATT_ITER_STOP;
	}

	p_ind = (rtk_bt_gattc_discover_ind_t *)p_evt->data;

	p_ind->conn_handle = conn->handle;
	p_ind->profile_id = profile_id;
	p_ind->type = type;

	if (!attr) {
		osif_mem_free(req);
		p_ind->status = RTK_BT_STATUS_DONE;
		goto indicate;
	}

	p_ind->status = RTK_BT_STATUS_CONTINUE;

	switch (params->type) {
	case BT_GATT_DISCOVER_PRIMARY: {
		struct bt_gatt_service_val *value = (struct bt_gatt_service_val *)attr->user_data;
		if (req->type == RTK_BT_GATT_DISCOVER_PRIMARY_ALL) {
			p_ind->disc_primary_all_per.start_handle = attr->handle;
			p_ind->disc_primary_all_per.end_handle = value->end_handle;
			_get_uuid_from_zephyr(&p_ind->disc_primary_all_per.uuid_type, p_ind->disc_primary_all_per.uuid, (struct bt_uuid *)value->uuid);
		} else {
			p_ind->disc_primary_by_uuid_per.start_handle = attr->handle;
			p_ind->disc_primary_by_uuid_per.end_handle = value->end_handle;
		}
		break;
	}
	case BT_GATT_DISCOVER_INCLUDE: {
		struct bt_gatt_include *value = (struct bt_gatt_include *)attr->user_data;
		p_ind->disc_inc_per.handle = attr->handle;
		p_ind->disc_inc_per.start_handle = value->start_handle;
		p_ind->disc_inc_per.end_handle = value->end_handle;
		_get_uuid_from_zephyr(&p_ind->disc_inc_per.uuid_type, p_ind->disc_inc_per.uuid, value->uuid);
		break;
	}
	case BT_GATT_DISCOVER_CHARACTERISTIC: {
		struct bt_gatt_chrc *value = (struct bt_gatt_chrc *)attr->user_data;
		if (req->type == RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL) {
			p_ind->disc_char_all_per.handle = attr->handle;
			p_ind->disc_char_all_per.value_handle = value->value_handle;
			p_ind->disc_char_all_per.properties = value->properties;
			_get_uuid_from_zephyr(&p_ind->disc_char_all_per.uuid_type, p_ind->disc_char_all_per.uuid, value->uuid);
		} else {
			p_ind->disc_char_by_uuid_per.handle = attr->handle;
			p_ind->disc_char_by_uuid_per.value_handle = value->value_handle;
			p_ind->disc_char_by_uuid_per.properties = value->properties;
			_get_uuid_from_zephyr(&p_ind->disc_char_by_uuid_per.uuid_type, p_ind->disc_char_by_uuid_per.uuid, params->uuid);
		}
		break;
	}
	case BT_GATT_DISCOVER_DESCRIPTOR:
		p_ind->disc_descriptor_per.handle = attr->handle;
		_get_uuid_from_zephyr(&p_ind->disc_descriptor_per.uuid_type, p_ind->disc_descriptor_per.uuid, attr->uuid);
		break;
	default:
		p_ind->status = RTK_BT_STATUS_FAIL;
		break;
	}

indicate:
	rtk_bt_evt_indicate(p_evt, NULL);

	return BT_GATT_ITER_CONTINUE;
}

static uint16_t bt_stack_gattc_discover(void *p_param)
{
	rtk_bt_gattc_discover_param_t *param = (rtk_bt_gattc_discover_param_t *)p_param;
	zephyr_bt_disc_req_t *req = NULL;
	struct bt_gatt_discover_params *disc = NULL;
	struct bt_conn *conn = NULL;
	int err = 0;

	req = (zephyr_bt_disc_req_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_bt_disc_req_t));
	if (!req) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(req, 0, sizeof(zephyr_bt_disc_req_t));

	req->profile_id = param->profile_id;
	req->type = param->type;
	disc = &req->params;

	disc->func = discover_cb;

	switch (param->type) {
	case RTK_BT_GATT_DISCOVER_PRIMARY_ALL:
		disc->type = BT_GATT_DISCOVER_PRIMARY;
		disc->start_handle = 0x0001;
		disc->end_handle = 0xFFFF;
		break;
	case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
		disc->type = BT_GATT_DISCOVER_PRIMARY;
		disc->start_handle = 0x0001;
		disc->end_handle = 0xFFFF;
		bt_uuid_create((struct bt_uuid *)(&req->uuid), param->disc_primary_by_uuid.uuid,
					   (param->disc_primary_by_uuid.uuid_type == BT_UUID_TYPE_16) ? BT_UUID_SIZE_16 : BT_UUID_SIZE_128);
		disc->uuid = (struct bt_uuid *)(&req->uuid);
		break;
	case RTK_BT_GATT_DISCOVER_INCLUDE:
		disc->type = BT_GATT_DISCOVER_INCLUDE;
		disc->start_handle = param->disc_inc.start_handle;
		disc->end_handle = param->disc_inc.end_handle;
		break;
	case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
		disc->type = BT_GATT_DISCOVER_CHARACTERISTIC;
		disc->start_handle = param->disc_char_all.start_handle;
		disc->end_handle = param->disc_char_all.end_handle;
		break;
	case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_BY_UUID:
		disc->type = BT_GATT_DISCOVER_CHARACTERISTIC;
		disc->start_handle = param->disc_char_by_uuid.start_handle;
		disc->end_handle = param->disc_char_by_uuid.end_handle;
		bt_uuid_create((struct bt_uuid *)(&req->uuid), param->disc_char_by_uuid.uuid,
					   (param->disc_char_by_uuid.uuid_type == BT_UUID_TYPE_16) ? BT_UUID_SIZE_16 : BT_UUID_SIZE_128);
		disc->uuid = (struct bt_uuid *)(&req->uuid);
		break;
	case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
		disc->type = BT_GATT_DISCOVER_DESCRIPTOR;
		disc->start_handle = param->disc_descriptor.start_handle;
		disc->end_handle = param->disc_descriptor.end_handle;
		break;
	default:
		break;
	}

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		osif_mem_free(req);
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_gatt_discover(conn, disc);
	bt_conn_unref(conn);

	if (err) {
		osif_mem_free(req);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint8_t read_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_read_params *params, const void *data, uint16_t length)
{
	zephyr_bt_read_req_t *req = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gattc_read_ind_t *p_ind = NULL;
	uint16_t profile_id;
	uint8_t *read_data = NULL;

	if (!params) {
		return BT_GATT_ITER_STOP;
	}

	req = CONTAINER_OF(params, zephyr_bt_read_req_t, params);

	profile_id = req->profile_id;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTC, RTK_BT_GATTC_EVT_READ_RESULT_IND, sizeof(rtk_bt_gattc_read_ind_t) + length);
	if (!p_evt) {
		osif_mem_free(req);
		return BT_GATT_ITER_STOP;
	}

	p_ind = (rtk_bt_gattc_read_ind_t *)p_evt->data;
	read_data = (uint8_t *)p_evt->data + sizeof(rtk_bt_gattc_read_ind_t);
	memcpy(read_data, data, length);

	p_ind->profile_id = profile_id;
	p_ind->conn_handle = conn->handle;
	if (params->handle_count == 0) {
		p_ind->type = RTK_BT_GATT_CHAR_READ_BY_UUID;
	} else if (params->handle_count == 1) {
		p_ind->type = RTK_BT_GATT_CHAR_READ_BY_HANDLE;
	} else {
		if (params->multiple.variable) {
			p_ind->type = RTK_BT_GATT_CHAR_READ_MULTIPLE_VARIABLE;
		} else {
			p_ind->type = RTK_BT_GATT_CHAR_READ_MULTIPLE;
		}
	}

	p_ind->err_code = RTK_BT_ATT_ERR(err);
	p_ind->status = err ? RTK_BT_STATUS_FAIL : (data ? RTK_BT_STATUS_CONTINUE : RTK_BT_STATUS_DONE);

	if (!data || (err != 0)) {
		osif_mem_free(req);
		goto indicate;
	}

	switch (p_ind->type) {
	case RTK_BT_GATT_CHAR_READ_BY_HANDLE:
		p_ind->by_handle.handle = params->single.handle;
		p_ind->by_handle.len = length;
		p_ind->by_handle.value = read_data;
		break;
	case RTK_BT_GATT_CHAR_READ_BY_UUID:
		p_ind->by_uuid_per.handle = params->by_uuid.start_handle;
		p_ind->by_uuid_per.len = length;
		p_ind->by_uuid_per.value = read_data;
		break;
	case RTK_BT_GATT_CHAR_READ_MULTIPLE:
		p_ind->multiple_per.len = length;
		p_ind->multiple_per.value = read_data;
		break;
	case RTK_BT_GATT_CHAR_READ_MULTIPLE_VARIABLE:
		p_ind->multiple_variable_per.len = length;
		p_ind->multiple_variable_per.value = read_data;
		break;
	default:
		break;
	}

indicate:
	rtk_bt_evt_indicate(p_evt, NULL);

	return BT_GATT_ITER_CONTINUE;
}

static uint16_t bt_stack_gattc_read(void *p_param)
{
	rtk_bt_gattc_read_param_t *param = (rtk_bt_gattc_read_param_t *)p_param;
	zephyr_bt_read_req_t *req = NULL;
	struct bt_conn *conn = NULL;
	struct bt_gatt_read_params *read = NULL;
	int err = 0;

	req = (zephyr_bt_read_req_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_bt_read_req_t));
	if (!req) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(req, 0, sizeof(zephyr_bt_read_req_t));

	req->profile_id = param->profile_id;
	read = &req->params;
	read->func = read_cb;
	if (RTK_BT_GATT_CHAR_READ_BY_HANDLE == param->type) {
		read->handle_count = 1;
		read->single.handle = param->by_handle.handle;
		read->single.offset = 0;
	} else if (RTK_BT_GATT_CHAR_READ_BY_UUID == param->type) {
		read->handle_count = 0;
		read->by_uuid.start_handle = param->by_uuid.start_handle;
		read->by_uuid.end_handle = param->by_uuid.end_handle;
		read->by_uuid.uuid = (struct bt_uuid *)(&req->uuid);
		bt_uuid_create((struct bt_uuid *)(&req->uuid), param->by_uuid.uuid, (param->by_uuid.uuid_type == BT_UUID_TYPE_16) ? BT_UUID_SIZE_16 : BT_UUID_SIZE_128);
	} else if (RTK_BT_GATT_CHAR_READ_MULTIPLE == param->type) {
		read->handle_count = param->multiple.handle_count;
		read->multiple.handles = param->multiple.handles;
		read->multiple.variable = false;
	} else if (RTK_BT_GATT_CHAR_READ_MULTIPLE_VARIABLE == param->type) {
		read->handle_count = param->multiple_variable.handle_count;
		read->multiple.handles = param->multiple_variable.handles;
		read->multiple.variable = true;
	}

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		osif_mem_free(req);
		return RTK_BT_ERR_NO_CONNECTION;
	}

	err = bt_gatt_read(conn, read);
	bt_conn_unref(conn);

	if (err) {
		osif_mem_free(req);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static void _write_cb(struct bt_conn *conn, uint8_t err, zephyr_bt_write_req_t *req)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gattc_write_ind_t *p_ind = NULL;

	if (!req) {
		return;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTC, RTK_BT_GATTC_EVT_WRITE_RESULT_IND, sizeof(rtk_bt_gattc_write_ind_t));
	if (!p_evt) {
		goto end;
	}

	p_ind = (rtk_bt_gattc_write_ind_t *)p_evt->data;

	p_ind->profile_id = req->profile_id;
	p_ind->conn_handle = conn->handle;
	p_ind->handle = req->handle;
	p_ind->err_code = RTK_BT_ATT_ERR(err);
	p_ind->status = err ? RTK_BT_STATUS_FAIL : RTK_BT_STATUS_DONE;
	p_ind->type = req->type;

	rtk_bt_evt_indicate(p_evt, NULL);

end:
	osif_mem_free(req);
}

void write_cmd_cb(struct bt_conn *conn, void *user_data)
{
	_write_cb(conn, 0, user_data);
}

void write_req_cb(struct bt_conn *conn, uint8_t err, struct bt_gatt_write_params *params)
{
	_write_cb(conn, err, CONTAINER_OF(params, zephyr_bt_write_req_t, params));
}

static uint16_t bt_stack_gattc_write(void *p_param)
{
	rtk_bt_gattc_write_param_t *param = (rtk_bt_gattc_write_param_t *)p_param;
	struct bt_conn *conn = NULL;
	zephyr_bt_write_req_t *req = NULL;
	void *data = NULL;
	int err = 0;

	req = (zephyr_bt_write_req_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_bt_write_req_t) + param->length);
	if (!req) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(req, 0, sizeof(zephyr_bt_write_req_t));

	req->profile_id = param->profile_id;
	req->type = param->type;
	req->handle = param->handle;
	if (param->length != 0) {
		data = (uint8_t *)req + sizeof(zephyr_bt_write_req_t);
		memcpy(data, param->data, param->length);
	}

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		osif_mem_free(req);
		return RTK_BT_ERR_NO_CONNECTION;
	}

	if (param->type == RTK_BT_GATT_CHAR_WRITE_REQ) {
		struct bt_gatt_write_params *write = &req->params;
		write->data = data;
		write->length = param->length;
		write->handle = param->handle;
		write->func = write_req_cb;
		err = bt_gatt_write(conn, write);
	} else {
		bool sign = (param->type == RTK_BT_GATT_CHAR_WRITE_NO_RSP_SIGNED);
		err = bt_gatt_write_without_response_cb(conn, param->handle, data, param->length,
												sign, write_cmd_cb, req);
	}

	bt_conn_unref(conn);
	if (err) {
		osif_mem_free(req);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static zephyr_bt_subscribe_t *find_subscribe(uint16_t conn_handle, uint16_t ccc_handle)
{
	zephyr_bt_subscribe_t *tmp, *found = NULL;

	osif_mutex_take(subscribe_mutex, BT_TIMEOUT_FOREVER);
	list_for_each_entry(tmp, &subscribe_list, list, zephyr_bt_subscribe_t) {
		if ((tmp->conn_handle == conn_handle) &&
			(tmp->params.ccc_handle == ccc_handle)) {
			found = tmp;
			break;
		}
	}
	osif_mutex_give(subscribe_mutex);

	return found;
}

static uint8_t notify_cb(struct bt_conn *conn,
						 struct bt_gatt_subscribe_params *params,
						 const void *data, uint16_t length)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gattc_cccd_value_ind_t *p_ind = NULL;
	zephyr_bt_subscribe_t *sub = NULL;

	/* NULL data to  */
	if (!data) {
		return BT_GATT_ITER_STOP;
	}

	sub = find_subscribe(conn->handle, params->ccc_handle);
	if (!sub) {
		return BT_GATT_ITER_STOP;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTC, sub->notify ? RTK_BT_GATTC_EVT_NOTIFY_IND : RTK_BT_GATTC_EVT_INDICATE_IND,
								sizeof(rtk_bt_gattc_cccd_value_ind_t) + length);
	if (!p_evt) {
		return BT_GATT_ITER_STOP;
	}

	p_ind = (rtk_bt_gattc_cccd_value_ind_t *)p_evt->data;

	p_ind->profile_id = sub->profile_id;
	p_ind->conn_handle = conn->handle;
	p_ind->value_handle = params->value_handle;
	p_ind->len = length;
	p_ind->value = (uint8_t *)p_evt->data + sizeof(rtk_bt_gattc_cccd_value_ind_t);
	memcpy(p_ind->value, data, length);

	rtk_bt_evt_indicate(p_evt, NULL);

	return BT_GATT_ITER_CONTINUE;
}

static void indicate_subcribe_result(bool enable, uint8_t err, uint16_t conn_handle, uint16_t profile_id, uint16_t ccc_handle, bool notify)
{
	rtk_bt_evt_t *p_evt = NULL;
	uint8_t evt_type;
	rtk_bt_gattc_cccd_update_ind_t *p_ind = NULL;

	if (enable) {
		evt_type = RTK_BT_GATTC_EVT_CCCD_ENABLE_IND;
	} else {
		evt_type = RTK_BT_GATTC_EVT_CCCD_DISABLE_IND;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTC, evt_type, sizeof(rtk_bt_gattc_cccd_update_ind_t));
	if (!p_evt) {
		return;
	}

	p_ind = (rtk_bt_gattc_cccd_update_ind_t *)p_evt->data;

	p_ind->profile_id = profile_id;
	p_ind->conn_handle = conn_handle;
	p_ind->status = err ? RTK_BT_STATUS_FAIL : RTK_BT_STATUS_DONE;
	p_ind->err_code = RTK_BT_ATT_ERR(err);
	p_ind->cccd_handle = ccc_handle;
	p_ind->bnotify = notify;
	p_ind->bindicate = !notify;

	rtk_bt_evt_indicate(p_evt, NULL);
}

static void subscribe_cb(struct bt_conn *conn, uint8_t err,
						 struct bt_gatt_subscribe_params *params)
{
	zephyr_bt_subscribe_t *sub = NULL;
	bool enable = false;

	sub = find_subscribe(conn->handle, params->ccc_handle);
	if (!sub) {
		return;
	}

	if ((!err) && sub->params.value) {
		sub->status = ZEPHYR_SUBSCRIBE_DONE;
	}

	enable = (sub->status == ZEPHYR_SUBSCRIBE_DONE) ? true : false;
	indicate_subcribe_result(enable, err, conn->handle, sub->profile_id, params->ccc_handle, sub->notify);

	/* Free memory when subcrib fail or unsubscribed */
	if (err || (!sub->params.value)) {
		gattc_delete_subscribe(sub);
	}
}

static uint16_t bt_stack_gattc_update_cccd_enable(void *p_param)
{
	rtk_bt_gattc_update_cccd_param_t *cccd = (rtk_bt_gattc_update_cccd_param_t *)p_param;
	struct bt_conn *conn = NULL;
	zephyr_bt_subscribe_t *sub = NULL;
	struct bt_gatt_subscribe_params *params = NULL;
	uint16_t conn_handle, profile_id, ccc_handle;
	int err = 0;

	conn_handle = cccd->conn_handle;
	profile_id = cccd->profile_id;
	ccc_handle = cccd->cccd_handle;

	sub = find_subscribe(conn_handle, ccc_handle);

	/* Already subscribed */
	if (sub) {
		if (sub->status == ZEPHYR_SUBSCRIBE_DONE) {
			indicate_subcribe_result(true, 0, conn_handle, profile_id, ccc_handle, sub->params.value & BT_GATT_CCC_NOTIFY);
			return 0;
		} else {/* if (sub->status == ZEPHYR_SUBSCRIBE_PROGRESSING) */
			return RTK_BT_ERR_ALREADY_IN_PROGRESS;
		}
	}

	sub = (zephyr_bt_subscribe_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_bt_subscribe_t));
	if (!sub) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(sub, 0, sizeof(zephyr_bt_subscribe_t));
	sub->conn_handle = conn_handle;
	sub->profile_id = profile_id;

	conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		osif_mem_free(sub);
		return RTK_BT_ERR_NO_CONNECTION;
	}

	sub->notify = cccd->bnotify;
	sub->status = ZEPHYR_SUBSCRIBE_PROGRESSING;
	params = &sub->params;
	params->notify = notify_cb;
	params->subscribe = subscribe_cb;
	params->value_handle = cccd->char_val_handle;
	params->ccc_handle = ccc_handle;
	if (cccd->bnotify) {
		params->value |= BT_GATT_CCC_NOTIFY;
	} else { //if (cccd->bindicate)
		params->value |= BT_GATT_CCC_INDICATE;
	}

	gattc_add_subscribe(sub);
	err = bt_gatt_subscribe(conn, params);

	bt_conn_unref(conn);
	if (err) {
		gattc_delete_subscribe(sub);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_gattc_update_cccd_disable(void *p_param)
{
	rtk_bt_gattc_update_cccd_param_t *param = (rtk_bt_gattc_update_cccd_param_t *)p_param;
	struct bt_conn *conn = NULL;
	zephyr_bt_subscribe_t *sub = NULL;
	int err = 0;

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	sub = find_subscribe(param->conn_handle, param->cccd_handle);

	/* No subscribe exist */
	if (!sub) {
		indicate_subcribe_result(false, 0, param->conn_handle, param->profile_id, param->cccd_handle, param->bnotify);
	} else {
		if (sub->status == ZEPHYR_SUBSCRIBE_DONE) {
			sub->status = ZEPHYR_SUBSCRIBE_PROGRESSING;
			if (bt_gatt_unsubscribe(conn, &sub->params)) {
				gattc_delete_subscribe(sub);
				err = RTK_BT_ERR_LOWER_STACK_API;
			}
		} else {/* if (sub->status == ZEPHYR_SUBSCRIBE_PROGRESSING) */
			err = RTK_BT_ERR_ALREADY_IN_PROGRESS;
		}
	}

	bt_conn_unref(conn);
	return err;
}

uint16_t bt_stack_gattc_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;

	if (true != bt_stack_profile_check(RTK_BT_PROFILE_GATTC)) {
		BT_LOGE("Error: Gattc profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_GATTC_ACT_REGISTER_PROFILE:
		ret = 0;
		break;
	case RTK_BT_GATTC_ACT_EXCHANGE_MTU:
		ret = bt_stack_gattc_exchange_mtu(p_cmd->param);
		break;
	case RTK_BT_GATTC_ACT_DISCOVER:
		ret = bt_stack_gattc_discover(p_cmd->param);
		break;
	case RTK_BT_GATTC_ACT_READ:
		ret = bt_stack_gattc_read(p_cmd->param);
		break;
	case RTK_BT_GATTC_ACT_WRITE:
		ret = bt_stack_gattc_write(p_cmd->param);
		break;
	case RTK_BT_GATTC_ACT_CCCD_ENABLE:
		ret = bt_stack_gattc_update_cccd_enable(p_cmd->param);
		break;
	case RTK_BT_GATTC_ACT_CCCD_DISABLE:
		ret = bt_stack_gattc_update_cccd_disable(p_cmd->param);
		break;
	default:
		BT_LOGE("bt_stack_le_act_handle:unknown act: %d \r\n", p_cmd->act);
		ret = RTK_BT_ERR_NO_CASE_ELEMENT;
		break;
	}

end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

void bt_zephyr_gattc_mtu_udpated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	(void)rx;
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_exchange_ind = NULL;
	rtk_bt_evt_t *p_evt_t = NULL;

	if (conn->role != BT_HCI_ROLE_CENTRAL) {
		return;
	}

	p_evt_t = rtk_bt_event_create(RTK_BT_LE_GP_GATTC,
								  RTK_BT_GATTC_EVT_MTU_EXCHANGE,
								  sizeof(rtk_bt_gatt_mtu_exchange_ind_t));
	if (!p_evt_t) {
		return;
	}

	p_gatt_mtu_exchange_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)p_evt_t->data;
	p_gatt_mtu_exchange_ind->result = RTK_BT_OK;
	p_gatt_mtu_exchange_ind->conn_handle = conn->handle;
	p_gatt_mtu_exchange_ind->mtu_size = tx;

	rtk_bt_evt_indicate(p_evt_t, NULL);
}

static struct bt_gatt_cb bt_zephyr_gatt_cb = {
	.att_mtu_updated = bt_zephyr_gattc_mtu_udpated
};

uint16_t bt_stack_gattc_init(rtk_bt_app_conf_t *app_conf)
{
	(void)app_conf;

	if (osif_mutex_create(&subscribe_mutex) == false) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	INIT_LIST_HEAD(&subscribe_list);

	if (is_stack_never_enabled()) {
		bt_gatt_cb_register(&bt_zephyr_gatt_cb);
	}

	return 0;
}

void bt_stack_gattc_deinit(void)
{
	zephyr_bt_subscribe_t *sub, *next;

	osif_mutex_take(subscribe_mutex, BT_TIMEOUT_FOREVER);
	list_for_each_entry_safe(sub, next, &subscribe_list, list, zephyr_bt_subscribe_t) {
		list_del(&sub->list);
		osif_mem_free(sub);
	}
	osif_mutex_give(subscribe_mutex);

	osif_mutex_delete(subscribe_mutex);
	subscribe_mutex = NULL;
}

void bt_stack_gattc_disconnect_clear(uint16_t conn_handle)
{
	zephyr_bt_subscribe_t *sub, *next;

	osif_mutex_take(subscribe_mutex, BT_TIMEOUT_FOREVER);
	list_for_each_entry_safe(sub, next, &subscribe_list, list, zephyr_bt_subscribe_t) {
		if (sub->conn_handle == conn_handle) {
			list_del(&sub->list);
			osif_mem_free(sub);
		}
	}
	osif_mutex_give(subscribe_mutex);
}