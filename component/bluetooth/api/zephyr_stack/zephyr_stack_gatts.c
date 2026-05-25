/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>
#include <bt_app_config.h>

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

#define BT_TIMEOUT_WAIT_RSP_SLOT          500
#define BT_TIMEOUT_WAIT_RSP_CNT           20

typedef struct {
	uint16_t app_id;
	uint16_t conn_handle;
	uint16_t index;
	uint8_t *data;
	uint16_t len;
	uint8_t flags;
} zephyr_gatts_received_req_t;

#define RECV_REQ_FLAG_DONE          BIT(0)
#define RECV_REQ_FLAG_SUCCEESS      BIT(1)


static struct list_head svc_list;

/* request processing */
static zephyr_gatts_received_req_t received_req[CONFIG_BT_MAX_CONN];
static void *wait_rsp_sem[CONFIG_BT_MAX_CONN] = {NULL};
static void service_node_free(zephyr_svc_node *node);
static zephyr_svc_node *service_node_alloc(struct rtk_bt_gatt_service *svc);

void bt_zephyr_gatts_mtu_udpated(struct bt_conn *conn, uint16_t tx, uint16_t rx);


static struct bt_gatt_cb bt_zephyr_gatts_cb = {
	.att_mtu_updated = bt_zephyr_gatts_mtu_udpated
};

void bt_zephyr_gatts_mtu_udpated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_exchange_ind = NULL;
	rtk_bt_evt_t *p_evt_t = NULL;

	if (conn->role != BT_HCI_ROLE_PERIPHERAL) {
		return;
	}

	p_evt_t = rtk_bt_event_create(RTK_BT_LE_GP_GATTS,
								  RTK_BT_GATTS_EVT_MTU_EXCHANGE,
								  sizeof(rtk_bt_gatt_mtu_exchange_ind_t));
	if (!p_evt_t) {
		return;
	}

	p_gatt_mtu_exchange_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)p_evt_t->data;
	p_gatt_mtu_exchange_ind->result = RTK_BT_OK;
	p_gatt_mtu_exchange_ind->conn_handle = conn->handle;
	p_gatt_mtu_exchange_ind->mtu_size = (tx > rx) ? rx : tx;

	rtk_bt_evt_indicate(p_evt_t, NULL);

}

static zephyr_svc_node *bt_stack_gatts_find_register_srv(uint16_t app_id)
{
	zephyr_svc_node *node = NULL;

	list_for_each_entry(node, &svc_list, list, zephyr_svc_node) {
		if (node->app_id == app_id) {
			return node;
		}
	}

	return NULL;
}

static uint16_t bt_stack_gatts_evt_get_attr_index(const struct bt_gatt_attr *attr, uint16_t *app_id)
{
	uint16_t i = 0;
	zephyr_svc_node *node = NULL;

	list_for_each_entry(node, &svc_list, list, zephyr_svc_node) {
		if (node->registered) {
			for (i = 0; i < node->svc.attr_count; i++) {
				if ((node->svc.attrs + i) == attr) {
					*app_id = node->app_id;
					return i;
				}
			}
		}
	}

	return 0;
}

static ssize_t bt_stack_gatts_read_cb_internal(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	uint16_t value_len = *((uint16_t *)attr->user_data);
	void *value = (void *)((uint8_t *)attr->user_data + 2);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, value_len);
}

static void clear_received_req(zephyr_gatts_received_req_t *req)
{
	if (req->data) {
		osif_mem_free(req->data);
	}

	memset(req, 0, sizeof(zephyr_gatts_received_req_t));
}

int rtk_bt_is_enable(void);
static ssize_t bt_stack_gatts_read_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gatts_read_ind_t *p_read_ind = NULL;
	uint16_t app_id;
	uint16_t index = 0;
	zephyr_gatts_received_req_t *req = &received_req[bt_conn_index(conn)];
	void *sem = wait_rsp_sem[bt_conn_index(conn)];
	ssize_t ret = -1;

	clear_received_req(req);

	index = bt_stack_gatts_evt_get_attr_index(attr, &app_id);
	if (index == 0) {
		return -1;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_READ_IND, sizeof(rtk_bt_gatts_read_ind_t));
	if (!p_evt) {
		return -1;
	}

	req->app_id = app_id;
	req->conn_handle = conn->handle;
	req->index = index;

	p_read_ind = (rtk_bt_gatts_read_ind_t *)p_evt->data;
	p_read_ind->app_id = app_id;
	p_read_ind->conn_handle = conn->handle;
	p_read_ind->index = index;
	p_read_ind->offset = offset;

	if (RTK_BT_OK == rtk_bt_evt_indicate(p_evt, NULL)) {
		int cnt = 0;
		while (rtk_bt_is_enable() && !(req->flags & RECV_REQ_FLAG_DONE) && cnt++ < BT_TIMEOUT_WAIT_RSP_CNT) {
			osif_sem_take(sem, BT_TIMEOUT_WAIT_RSP_SLOT);
		}
		if ((req->flags & RECV_REQ_FLAG_DONE) && (req->flags & RECV_REQ_FLAG_SUCCEESS)) {
			ret = bt_gatt_attr_read(conn, attr, buf, len, 0, req->data, req->len);
		}
	}

	clear_received_req(req);

	return ret;
}

/* if req contains data from perivous write request, append buf to perivous data */
static bool _copy_write_buf(zephyr_gatts_received_req_t *req, const void *buf, uint16_t len, uint16_t handle, uint16_t index, uint16_t app_id)
{
	void *data;
	data = osif_mem_alloc(RAM_TYPE_DATA_ON, len + req->len);
	if (!data) {
		return false;
	}

	if (req->data) { /* copy old data */
		memcpy(data, req->data, req->len);
		osif_mem_free(req->data);
	}

	memcpy((uint8_t *)data + req->len, buf, len);
	req->data = data;
	req->len += len;
	req->conn_handle = handle;
	req->index = index;
	req->app_id = app_id;
	return true;
}

static ssize_t bt_stack_gatts_write_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	rtk_bt_gatts_write_ind_t *p_ind = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	uint16_t app_id;
	uint16_t index = 0;
	zephyr_gatts_received_req_t *req = &received_req[bt_conn_index(conn)];
	void *sem = wait_rsp_sem[bt_conn_index(conn)];
	ssize_t ret = -1;

	index = bt_stack_gatts_evt_get_attr_index(attr, &app_id);
	if (index == 0) {
		goto end;
	}

	/* just buffer data */
	if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
		/* data mismatch */
		if (req->data && ((req->conn_handle != conn->handle) || (req->index != index) || (req->app_id != app_id))) {
			clear_received_req(req);
		}

		if (!_copy_write_buf(req, buf, len, conn->handle, index, app_id)) {
			goto end;
		}

		return 0;
	}

	if (flags & BT_GATT_WRITE_FLAG_EXECUTE) {
		if (!req->data || ((req->conn_handle != conn->handle) || (req->index != index))) {/* data mismatch */
			goto end;
		}
	} else if (!_copy_write_buf(req, buf, len, conn->handle, index, app_id)) {
		goto end;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_WRITE_IND, sizeof(rtk_bt_gatts_write_ind_t) + req->len);
	if (!p_evt) {
		goto end;
	}

	p_ind = (rtk_bt_gatts_write_ind_t *)p_evt->data;
	p_ind->type = (flags & BT_GATT_WRITE_FLAG_CMD) ? RTK_BT_GATTS_WRITE_NO_RESP : RTK_BT_GATTS_WRITE_REQ;
	p_ind->app_id = app_id;
	p_ind->conn_handle = conn->handle;
	p_ind->index = index;
	p_ind->offset = offset;
	p_ind->len = req->len;

	if (req->len) {
		/* 2nd level ptr in write event msg need deep copy */
		p_ind->value = (uint8_t *)p_evt->data + sizeof(rtk_bt_gatts_write_ind_t);
		memcpy(p_ind->value, req->data, req->len);
	}

	if (RTK_BT_OK == rtk_bt_evt_indicate(p_evt, NULL)) {
		if (p_ind->type == RTK_BT_GATTS_WRITE_REQ) {
			int cnt = 0;
			while (rtk_bt_is_enable() && !(req->flags & RECV_REQ_FLAG_DONE) && cnt++ < BT_TIMEOUT_WAIT_RSP_CNT) {
				osif_sem_take(sem, BT_TIMEOUT_WAIT_RSP_SLOT);
			}
			if ((req->flags & RECV_REQ_FLAG_DONE) && (req->flags & RECV_REQ_FLAG_SUCCEESS)) {
				ret = len;
			}
		} else {
			ret = len;
		}
	}

end:
	clear_received_req(req);
	return ret;
}

static void bt_stack_gatts_indicate_cb(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t err)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ind = NULL;
	zephyr_indicate_req_t *req = NULL;

	if (!params) {
		return;
	}

	req = CONTAINER_OF(params, zephyr_indicate_req_t, params);
	if (!req) {
		return;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND, sizeof(rtk_bt_gatts_ntf_and_ind_ind_t));
	if (!p_evt) {
		osif_mem_free(req);
		return;
	}

	p_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)p_evt->data;

	p_ind->app_id = req->app_id;
	p_ind->conn_handle = conn->handle;
	p_ind->seq = 0;
	p_ind->index = req->attr_index;
	p_ind->err_code = RTK_BT_ATT_ERR(err);

	rtk_bt_evt_indicate(p_evt, NULL);

	osif_mem_free(req);
}

static void bt_stack_gatts_notify_send_complete_cb(struct bt_conn *conn, void *user_data)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ind = NULL;
	uint16_t app_id, index;
	struct bt_gatt_attr *attr = (struct bt_gatt_attr *)user_data;

	index = bt_stack_gatts_evt_get_attr_index(attr, &app_id);
	if (index == 0) {
		return;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND, sizeof(rtk_bt_gatts_ntf_and_ind_ind_t));
	if (!p_evt) {
		return;
	}

	p_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)p_evt->data;

	p_ind->app_id = app_id;
	p_ind->conn_handle = conn->handle;
	p_ind->index = index;
	p_ind->err_code = 0;

	rtk_bt_evt_indicate(p_evt, NULL);
}

static ssize_t bt_stack_gatts_ccc_write_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, uint16_t value)
{
	uint16_t app_id;
	uint16_t index = 0;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = NULL;

	index = bt_stack_gatts_evt_get_attr_index(attr, &app_id);
	if (!index) {
		return 0;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_CCCD_IND, sizeof(rtk_bt_gatts_cccd_ind_t));
	if (!p_evt) {
		return 0;
	}

	p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)p_evt->data;

	p_cccd_ind->conn_handle = conn->handle;
	p_cccd_ind->app_id = app_id;
	p_cccd_ind->index = index;
	p_cccd_ind->value = value;

	if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
		return 0;
	}

	return sizeof(value);
}

uint16_t bt_stack_gatts_init(rtk_bt_app_conf_t *app_conf)
{
	(void)app_conf;
	int i;

	memset(received_req, 0, sizeof(zephyr_gatts_received_req_t) * CONFIG_BT_MAX_CONN);

	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		if (osif_sem_create(&wait_rsp_sem[i], 0, 1) == false) {
			goto fail;
		}
	}

	INIT_LIST_HEAD(&svc_list);

	if (is_stack_never_enabled()) {
		bt_gatt_cb_register(&bt_zephyr_gatts_cb);
	}

	return 0;

fail:
	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		if (wait_rsp_sem[i]) {
			osif_sem_delete(wait_rsp_sem[i]);
			wait_rsp_sem[i] = NULL;
		}
	}

	return RTK_BT_ERR_NO_MEMORY;
}

void bt_stack_gatts_deinit(void)
{
	zephyr_svc_node *node;
	int i;

	/* DON'T use list_for_each_entry_safe here.
	 * Included node is also on list, service_node_free() frees included node when included service not registered yet,
	 * so 'next' of list_for_each_entry_safe will be invalid. */
	while (!list_empty(&svc_list)) {
		node = list_first_entry(&svc_list, zephyr_svc_node, list);
		if (node->registered) {
			bt_gatt_service_unregister(&node->svc);
			service_node_free(node);
		}
	}

	for (i = 0; i < CONFIG_BT_MAX_CONN; i++) {
		clear_received_req(&received_req[i]);
		osif_sem_delete(wait_rsp_sem[i]);
		wait_rsp_sem[i] = NULL;
	}
}

void bt_stack_gatts_disconnect_clear(struct bt_conn *conn)
{
	clear_received_req(&received_req[bt_conn_index(conn)]);
}

static uint16_t _duplicate_uuid(const struct bt_uuid *old, void **new)
{
	int len;
	struct bt_uuid *uuid;

	if (old->type == BT_UUID_TYPE_16) {
		len = sizeof(struct bt_uuid_16);
	} else {
		len = sizeof(struct bt_uuid_128);
	}

	uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);

	if (!uuid) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy(uuid, old, len);
	*((struct bt_uuid **)new) = uuid;

	return 0;
}

static uint16_t _duplicate_attrs(const struct rtk_bt_gatt_service *svc, struct bt_gatt_attr **p_attrs)
{
	uint16_t i;
	struct bt_gatt_attr *attrs;

	attrs = (struct bt_gatt_attr *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_gatt_attr) * svc->attr_count);

	if (!attrs) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	memset(attrs, 0, sizeof(struct bt_gatt_attr) * svc->attr_count);

	*p_attrs = attrs;

	for (i = 0; i < svc->attr_count; i++) {
		rtk_bt_gatt_attr_t *src = svc->attrs + i;
		struct bt_gatt_attr *dst = attrs + i;

		dst->perm = src->perm;
		/* If permission include writable, make it also prepare writable, compatible to rtk bt stack.
		    And if you want remote device to prepare write to local, config CONFIG_BT_ATT_PREPARE_COUNT
		    to big enough, or prepare write will fail */
		if (src->perm & (BT_GATT_PERM_WRITE | BT_GATT_PERM_WRITE_ENCRYPT | BT_GATT_PERM_WRITE_AUTHEN)) {
			dst->perm |= BT_GATT_PERM_PREPARE_WRITE;
		}

		if (_duplicate_uuid(src->uuid, (void **)&dst->uuid)) {
			return RTK_BT_ERR_NO_MEMORY;
		}

		if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_PRIMARY) ||
			!bt_uuid_cmp(src->uuid, BT_UUID_GATT_SECONDARY)) {
			dst->read = bt_gatt_attr_read_service;
			if (_duplicate_uuid((struct bt_uuid *)src->user_data, &dst->user_data)) {
				return RTK_BT_ERR_NO_MEMORY;
			}

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_INCLUDE)) {
			zephyr_svc_node *node = NULL;
			node = service_node_alloc((struct rtk_bt_gatt_service *)src->user_data);
			if (!node) {
				return RTK_BT_ERR_NO_MEMORY;
			}

			node->include_ref++;
			dst->read = bt_gatt_attr_read_included;
			dst->user_data = node->svc.attrs;

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_CHRC)) {
			struct rtk_bt_gatt_chrc *rtk_chrc = (struct rtk_bt_gatt_chrc *)src->user_data;
			struct bt_gatt_chrc *p_chrc = (struct bt_gatt_chrc *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_gatt_chrc));
			if (!p_chrc) {
				return RTK_BT_ERR_NO_MEMORY;
			}
			p_chrc->uuid = NULL;
			dst->user_data = p_chrc;

			if (_duplicate_uuid(rtk_chrc->uuid, (void **)(&p_chrc->uuid))) {
				return RTK_BT_ERR_NO_MEMORY;
			}

			p_chrc->value_handle = rtk_chrc->value_handle;
			p_chrc->properties = rtk_chrc->properties;
			dst->read = bt_gatt_attr_read_chrc;

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_CCC)) {
			struct _bt_gatt_ccc *p_ccc = (struct _bt_gatt_ccc *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct _bt_gatt_ccc));
			if (!p_ccc) {
				return RTK_BT_ERR_NO_MEMORY;
			}

			memset(p_ccc, 0, sizeof(struct _bt_gatt_ccc));
			p_ccc->cfg_write = bt_stack_gatts_ccc_write_cb;
			dst->read = bt_gatt_attr_read_ccc;
			dst->write = bt_gatt_attr_write_ccc;
			dst->user_data = p_ccc;

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_CEP)) {
			dst->read = bt_gatt_attr_read_cep;
			dst->user_data = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_gatt_cep));
			if (!dst->user_data) {
				return RTK_BT_ERR_NO_MEMORY;
			}
			((struct bt_gatt_cep *)dst->user_data)->properties = ((struct rtk_bt_gatt_cep *)src->user_data)->properties;

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_CUD)) {
			int cud_len = strlen(src->user_data) + 1;
			dst->read = bt_gatt_attr_read_cud;
			dst->user_data = osif_mem_alloc(RAM_TYPE_DATA_ON, cud_len);
			if (!dst->user_data) {
				return RTK_BT_ERR_NO_MEMORY;
			}
			memcpy(dst->user_data, src->user_data, cud_len);

		} else if (!bt_uuid_cmp(src->uuid, BT_UUID_GATT_CPF)) {
			dst->read = bt_gatt_attr_read_cpf;
			dst->user_data = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_gatt_cpf));
			if (!dst->user_data) {
				return RTK_BT_ERR_NO_MEMORY;
			}
			((struct bt_gatt_cpf *)dst->user_data)->format = ((struct rtk_bt_gatt_cpf *)src->user_data)->format;
			((struct bt_gatt_cpf *)dst->user_data)->exponent = ((struct rtk_bt_gatt_cpf *)src->user_data)->exponent;
			((struct bt_gatt_cpf *)dst->user_data)->unit = ((struct rtk_bt_gatt_cpf *)src->user_data)->unit;
			((struct bt_gatt_cpf *)dst->user_data)->name_space = ((struct rtk_bt_gatt_cpf *)src->user_data)->name_space;
			((struct bt_gatt_cpf *)dst->user_data)->description = ((struct rtk_bt_gatt_cpf *)src->user_data)->description;

		} else {
			if (src->flag == RTK_BT_GATT_INTERNAL) { /* stack covers read */
				dst->read = bt_stack_gatts_read_cb_internal;
				dst->user_data = osif_mem_alloc(RAM_TYPE_DATA_ON, src->len + sizeof(uint16_t));
				if (!dst->user_data) {
					return RTK_BT_ERR_NO_MEMORY;
				}
				*((uint16_t *)dst->user_data) = src->len;
				memcpy(dst->user_data, &src->len, sizeof(uint16_t));
				memcpy((uint8_t *)dst->user_data + sizeof(uint16_t), src->user_data, src->len);
			} else { /* APP covers read */
				dst->read = bt_stack_gatts_read_cb;
				dst->write = bt_stack_gatts_write_cb;
			}
		}
	}

	return 0;
}

static void service_node_free(zephyr_svc_node *node)
{
	uint16_t i = 0;

	if (!node) {
		return;
	}

	if (node->svc.attrs) {
		/* free memory in every attribute */
		for (i = 0; i < node->svc.attr_count; i++) {
			struct bt_gatt_attr *attr = node->svc.attrs + i;
			if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_INCLUDE)) { /* free unregistered included service node */
				zephyr_svc_node *include_node = NULL, *tmp = NULL;
				list_for_each_entry(tmp, &svc_list, list, zephyr_svc_node) {
					if (tmp->svc.attrs == (struct bt_gatt_attr *)attr->user_data) {
						include_node = tmp;
						break;
					}
				}
				if (include_node) {
					node->include_ref--;
					if (!node->include_ref && !node->registered) {
						service_node_free(node);
					}
				}
			} else {/* user_data of include service uuid will be freed in service_node_free() */
				if (attr->user_data) { /* free user data */
					if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC)) {
						struct bt_gatt_chrc *p_chrc = (struct bt_gatt_chrc *)attr->user_data;
						if (p_chrc->uuid) {
							osif_mem_free((void *)p_chrc->uuid);
						}
					}
					osif_mem_free(attr->user_data);
				}
			}
			osif_mem_free((void *)attr->uuid);
		}
		osif_mem_free(node->svc.attrs);
	}

	list_del(&node->list);
	osif_mem_free(node);
}

static zephyr_svc_node *service_node_alloc(struct rtk_bt_gatt_service *svc)
{
	zephyr_svc_node *node = NULL;
	node = bt_stack_gatts_find_register_srv(svc->app_id);

	if (node) {
		return node;
	}

	node = (zephyr_svc_node *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_svc_node));
	if (!node) {
		return NULL;
	}

	memset(node, 0, sizeof(zephyr_svc_node));
	list_add_tail(&node->list, &svc_list);

	if (_duplicate_attrs(svc, &node->svc.attrs) == 0) {
		node->svc.attr_count = svc->attr_count;
		node->app_id = svc->app_id;

		return node;
	}

	/* attributes allocation fail, free memory */
	service_node_free(node);
	return NULL;
}

static uint16_t bt_stack_gatts_register_service(void *p_gatts_srv)
{
	struct rtk_bt_gatt_service *svc = (struct rtk_bt_gatt_service *)p_gatts_srv;
	zephyr_svc_node *node = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_gatts_reg_ind_t *p_ind = NULL;

	node = service_node_alloc(svc);

	if (!node) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	if (node->registered) {
		return RTK_BT_ERR_ALREADY_DONE;
	}

	if (bt_gatt_service_register(&node->svc)) {
		service_node_free(node);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	node->registered = true;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GATTS, RTK_BT_GATTS_EVT_REGISTER_SERVICE, sizeof(rtk_bt_gatts_reg_ind_t));
	if (p_evt) {
		p_ind = (rtk_bt_gatts_reg_ind_t *)p_evt->data;
		p_ind->app_id = svc->app_id;
		p_ind->reg_status = RTK_BT_OK;
	}
	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

static uint16_t bt_stack_gatts_notify(void *p_param)
{
	rtk_bt_gatts_ntf_and_ind_param_t *param = (rtk_bt_gatts_ntf_and_ind_param_t *)p_param;
	zephyr_svc_node *node = NULL;
	struct bt_conn *conn = NULL;
	struct bt_gatt_notify_params notify;
	int ret = 0;

	node = bt_stack_gatts_find_register_srv(param->app_id);
	if (!node) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}

	memset(&notify, 0, sizeof(struct bt_gatt_notify_params));

	notify.func = bt_stack_gatts_notify_send_complete_cb;
	notify.attr = &node->svc.attrs[param->index];
	notify.len = param->len;
	notify.data = param->data;
	notify.user_data = (void *)notify.attr;

	ret = bt_gatt_notify_cb(conn, &notify);

	bt_conn_unref(conn);

	if (ret) {
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_gatts_indicate(void *p_param)
{
	rtk_bt_gatts_ntf_and_ind_param_t *param = (rtk_bt_gatts_ntf_and_ind_param_t *)p_param;
	zephyr_svc_node *node = NULL;
	struct bt_conn *conn = NULL;
	zephyr_indicate_req_t *req = NULL;
	struct bt_gatt_indicate_params *ind = NULL;
	uint8_t *data = NULL;
	int ret = 0;

	node = bt_stack_gatts_find_register_srv(param->app_id);
	if (!node) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	req = (zephyr_indicate_req_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(zephyr_indicate_req_t) + param->len);
	if (!req) {
		return RTK_BT_ERR_NO_MEMORY;
	}

	if (param->len) {
		data = (uint8_t *)req + sizeof(zephyr_indicate_req_t);
		memcpy(data, param->data, param->len);
	}

	memset(req, 0, sizeof(zephyr_indicate_req_t));
	req->app_id = param->app_id;
	req->attr_index = param->index;

	ind = &req->params;
	ind->attr = &node->svc.attrs[param->index];
	ind->func = bt_stack_gatts_indicate_cb;
	ind->len = param->len;
	ind->data = data;

	conn = bt_conn_lookup_handle(param->conn_handle, BT_CONN_TYPE_LE);
	if (!conn) {
		osif_mem_free(req);
		return RTK_BT_ERR_NO_CONNECTION;
	}

	ret = bt_gatt_indicate(conn, ind);

	bt_conn_unref(conn);

	if (ret) {
		osif_mem_free(req);
		return RTK_BT_ERR_LOWER_STACK_API;
	}

	return 0;
}

static uint16_t bt_stack_gatts_read_rsp(void *param)
{
	rtk_bt_gatts_read_resp_param_t *rsp = (rtk_bt_gatts_read_resp_param_t *)param;
	zephyr_gatts_received_req_t *req;
	uint8_t conn_id;
	struct bt_conn *conn = bt_conn_lookup_handle(rsp->conn_handle, BT_CONN_TYPE_LE);

	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}
	conn_id = bt_conn_index(conn);
	bt_conn_unref(conn);

	req = &received_req[conn_id];
	if ((rsp->app_id != req->app_id) || (rsp->conn_handle != req->conn_handle) || (rsp->index != req->index)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (rsp->err_code == 0) {
		if (rsp->len != 0) {
			req->data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, rsp->len);
			if (!req->data) {
				osif_sem_give(wait_rsp_sem[conn_id]);
				return RTK_BT_ERR_NO_MEMORY;
			} else {
				req->len = rsp->len;
				memcpy(req->data, rsp->data, rsp->len);
			}
		}
		req->flags |= RECV_REQ_FLAG_SUCCEESS;
	}

	req->flags |= RECV_REQ_FLAG_DONE;
	osif_sem_give(wait_rsp_sem[conn_id]);
	return 0;
}

static uint16_t bt_stack_gatts_write_rsp(void *param)
{
	rtk_bt_gatts_write_resp_param_t *rsp = (rtk_bt_gatts_write_resp_param_t *)param;
	zephyr_gatts_received_req_t *req;
	uint8_t conn_id;
	struct bt_conn *conn = bt_conn_lookup_handle(rsp->conn_handle, BT_CONN_TYPE_LE);

	if (!conn) {
		return RTK_BT_ERR_NO_CONNECTION;
	}
	conn_id = bt_conn_index(conn);
	bt_conn_unref(conn);

	req = &received_req[conn_id];
	if ((rsp->app_id != req->app_id) || (rsp->conn_handle != req->conn_handle) || (rsp->index != req->index)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (rsp->err_code == 0) {
		req->flags |= RECV_REQ_FLAG_SUCCEESS;
	}

	req->flags |= RECV_REQ_FLAG_DONE;
	osif_sem_give(wait_rsp_sem[conn_id]);
	return 0;
}

uint16_t bt_stack_gatts_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_OK;

	if (true != bt_stack_profile_check(RTK_BT_PROFILE_GATTS)) {
		BT_LOGE("Error: Gatts profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_GATTS_ACT_REGISTER_SERVICE:
		ret = bt_stack_gatts_register_service(p_cmd->param);
		break;
	case RTK_BT_GATTS_ACT_READ_RSP:
		ret = bt_stack_gatts_read_rsp(p_cmd->param);
		break;
	case RTK_BT_GATTS_ACT_WRITE_RSP:
		ret = bt_stack_gatts_write_rsp(p_cmd->param);
		break;
	case RTK_BT_GATTS_ACT_NOTIFY:
		ret = bt_stack_gatts_notify(p_cmd->param);
		break;
	case RTK_BT_GATTS_ACT_INDICATE:
		ret = bt_stack_gatts_indicate(p_cmd->param);
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

