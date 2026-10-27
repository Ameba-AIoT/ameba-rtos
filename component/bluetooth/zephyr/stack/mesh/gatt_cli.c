/*
 * Copyright (c) 2021 Xiaomi Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/net/buf.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/mesh.h>

#include "common/bt_str.h"

#include "mesh.h"
#include "net.h"
#include "rpl.h"
#include "transport.h"
#include "prov.h"
#include "beacon.h"
#include "foundation.h"
#include "access.h"
#include "proxy.h"
#include "proxy_msg.h"
#include "proxy_cli.h"
#include "gatt_cli.h"
#include "pb_gatt_cli.h"

#define LOG_LEVEL CONFIG_BT_MESH_PROXY_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bt_mesh_gatt_client);

static struct bt_mesh_gatt_server {
	struct bt_conn *conn;
	uint16_t svc_start_handle;
	uint16_t data_in_handle;
	const struct bt_mesh_gatt_cli *gatt;
#if ZEPHYR_RTK_PATCH
	bool manual;
	bool notify_on;
	uint16_t ccc_handle;
	bt_mesh_gatt_cli_dis_cb dis_result;
#endif

	union {
		void *user_data;
		struct bt_gatt_discover_params discover;
		struct bt_gatt_subscribe_params subscribe;
	};
} servers[CONFIG_BT_MAX_CONN];

static struct bt_mesh_gatt_server *get_server(struct bt_conn *conn)
{
	return &servers[bt_conn_index(conn)];
}

static uint8_t notify_func(struct bt_conn *conn,
			   struct bt_gatt_subscribe_params *params,
			   const void *data, uint16_t length)
{
	const uint8_t *val = data;

	if (!data) {
		LOG_WRN("[UNSUBSCRIBED]");
		params->value_handle = 0U;
		return BT_GATT_ITER_STOP;
	}

	if (length < 1) {
		LOG_WRN("Too small Proxy PDU");
		return BT_GATT_ITER_STOP;
	}

	(void)bt_mesh_proxy_msg_recv(conn, val, length);

	return BT_GATT_ITER_CONTINUE;
}

static void notify_enabled(struct bt_conn *conn, uint8_t err,
			   struct bt_gatt_subscribe_params *params)
{
	ARG_UNUSED(params);
	struct bt_mesh_gatt_server *server = get_server(conn);

	if (err != 0) {
		LOG_WRN("Enable notify failed(err:%d)", err);
#if ZEPHYR_RTK_PATCH
		if (server->manual && server->gatt && server->gatt->link_open_failed) {
			server->gatt->link_open_failed(conn);
		}
#endif
		return;
	}

	LOG_DBG("[SUBSCRIBED]");

#if ZEPHYR_RTK_PATCH
	server->notify_on = true;
#endif
	server->gatt->link_open(conn);
}

#if ZEPHYR_RTK_PATCH
static void dis_report(struct bt_conn *conn, struct bt_mesh_gatt_server *server,
		       enum bt_mesh_gatt_cli_dis_result res)
{
	bt_mesh_gatt_cli_dis_cb result = server->dis_result;

	server->dis_result = NULL;
	if (result) {
		result(conn, res);
	}
}
#endif

static uint8_t discover_func(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	int err;
	struct bt_mesh_gatt_server *server = get_server(conn);

	if (!attr) {
		LOG_DBG("GATT Services Discover complete");
#if ZEPHYR_RTK_PATCH
		if (server->manual) {
			/* attr is NULL before the CCCD is found: the service or a characteristic is missing */
			dis_report(conn, server, BT_MESH_GATT_CLI_DIS_NOT_FOUND);
		}
#endif
		(void)memset(params, 0, sizeof(*params));
		return BT_GATT_ITER_STOP;
	}

	LOG_DBG("[ATTRIBUTE UUID 0x%04x] handle %u", BT_UUID_16(server->discover.uuid)->val,
		attr->handle);

	if (!bt_uuid_cmp(server->discover.uuid, &server->gatt->srv_uuid.uuid)) {
		server->svc_start_handle = attr->handle;

		server->discover.uuid = &server->gatt->data_in_uuid.uuid;
		server->discover.start_handle = attr->handle + 1;
		server->discover.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &server->discover);
		if (err) {
			LOG_DBG("Discover GATT data in char failed (err %d)", err);
#if ZEPHYR_RTK_PATCH
			if (server->manual) {
				dis_report(conn, server, BT_MESH_GATT_CLI_DIS_FAIL);
			}
#endif
		}
	} else if (!bt_uuid_cmp(server->discover.uuid,
				&server->gatt->data_in_uuid.uuid)) {
		server->data_in_handle = attr->handle + 1;

		server->discover.uuid = &server->gatt->data_out_uuid.uuid;
		server->discover.start_handle = server->svc_start_handle + 1;
		server->discover.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &server->discover);
		if (err) {
			LOG_DBG("Discover GATT data out char failed (err %d)", err);
#if ZEPHYR_RTK_PATCH
			if (server->manual) {
				dis_report(conn, server, BT_MESH_GATT_CLI_DIS_FAIL);
			}
#endif
		}
	} else if (!bt_uuid_cmp(server->discover.uuid,
				&server->gatt->data_out_uuid.uuid)) {
		server->discover.uuid = &server->gatt->data_out_cccd_uuid.uuid;
		server->discover.start_handle = attr->handle + 2;
		server->discover.type = BT_GATT_DISCOVER_DESCRIPTOR;

		err = bt_gatt_discover(conn, &server->discover);
		if (err) {
			LOG_DBG("Discover GATT CCCD failed (err %d)", err);
#if ZEPHYR_RTK_PATCH
			if (server->manual) {
				dis_report(conn, server, BT_MESH_GATT_CLI_DIS_FAIL);
			}
#endif
		}
	} else {
#if ZEPHYR_RTK_PATCH
		if (server->manual) {
			server->ccc_handle = attr->handle;
			dis_report(conn, server, BT_MESH_GATT_CLI_DIS_DONE);
			return BT_GATT_ITER_STOP;
		}
#endif
		(void)memset(&server->subscribe, 0, sizeof(server->subscribe));

		server->subscribe.notify = notify_func;
		server->subscribe.subscribe = notify_enabled;
		server->subscribe.value = BT_GATT_CCC_NOTIFY;
		server->subscribe.ccc_handle = attr->handle;
		server->subscribe.value_handle = attr->handle - 1;

		err = bt_gatt_subscribe(conn, &server->subscribe);
		if (err && err != -EALREADY) {
			LOG_DBG("Subscribe failed (err %d)", err);
		}
	}

	return BT_GATT_ITER_STOP;
}

int bt_mesh_gatt_send(struct bt_conn *conn,
		      const void *data, uint16_t len,
		      bt_gatt_complete_func_t end, void *user_data)
{
	struct bt_mesh_gatt_server *server = get_server(conn);

	LOG_DBG("%u bytes: %s", len, bt_hex(data, len));

	return bt_gatt_write_without_response_cb(conn, server->data_in_handle,
						 data, len, false, end, user_data);
}

#if ZEPHYR_RTK_PATCH
/* What is done once the connection of an auto mode server is up, shared by a new and a reused connection */
static void gatt_cli_start_discover(struct bt_conn *conn, struct bt_mesh_gatt_server *server)
{
	int err;

	server->gatt->connected(conn, server->user_data);

	server->discover.uuid = &server->gatt->srv_uuid.uuid;
	server->discover.func = discover_func;
	server->discover.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	server->discover.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	server->discover.type = BT_GATT_DISCOVER_PRIMARY;
	err = bt_gatt_discover(conn, &server->discover);
	if (err) {
		LOG_ERR("Unable discover GATT Services (err %d)", err);
	}
}
#endif

static void gatt_connected(struct bt_conn *conn, uint8_t conn_err)
{
	struct bt_mesh_gatt_server *server = get_server(conn);
	struct bt_conn_info info;
	int err;

	err = bt_conn_get_info(conn, &info);
#if ZEPHYR_RTK_PATCH
	if (err || info.role != BT_CONN_ROLE_CENTRAL ||
	    (!server->gatt && !server->manual)) {
		return;
	}
#else
	if (err || info.role != BT_CONN_ROLE_CENTRAL ||
	    !server->gatt) {
		return;
	}
#endif

	if (conn_err) {
		LOG_ERR("Failed to connect GATT Services(%u)", conn_err);

		bt_conn_unref(server->conn);
		server->conn = NULL;
#if ZEPHYR_RTK_PATCH
		if (server->manual) {
			server->manual = false;
			return;
		}
#endif

		(void)bt_mesh_scan_enable();

		return;
	}

	LOG_DBG("conn %p err 0x%02x", (void *)conn, conn_err);

#if ZEPHYR_RTK_PATCH
	if (server->manual) {
		/* The service is unknown until discovery is requested */
		return;
	}

	(void)bt_mesh_scan_enable();
	gatt_cli_start_discover(conn, server);
#else
	server->gatt->connected(conn, server->user_data);

	(void)bt_mesh_scan_enable();

	server->discover.uuid = &server->gatt->srv_uuid.uuid;
	server->discover.func = discover_func;
	server->discover.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	server->discover.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	server->discover.type = BT_GATT_DISCOVER_PRIMARY;
	err = bt_gatt_discover(conn, &server->discover);
	if (err) {
		LOG_ERR("Unable discover GATT Services (err %d)", err);
	}
#endif
}

static void gatt_disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(reason);
	struct bt_conn_info info;
	struct bt_mesh_gatt_server *server = get_server(conn);
	int err;

	err = bt_conn_get_info(conn, &info);
#if ZEPHYR_RTK_PATCH
	if (err || info.role != BT_CONN_ROLE_CENTRAL ||
	    (!server->gatt && !server->manual)) {
		return;
	}

	if (server->gatt) {
		server->gatt->disconnected(conn);
	}
#else
	if (err || info.role != BT_CONN_ROLE_CENTRAL ||
	    !server->gatt) {
		return;
	}

	server->gatt->disconnected(conn);
#endif

	bt_conn_unref(server->conn);

	(void)memset(server, 0, sizeof(struct bt_mesh_gatt_server));
}

int bt_mesh_gatt_cli_connect(const bt_addr_le_t *addr,
			     const struct bt_mesh_gatt_cli *gatt,
			     void *user_data)
{
	int err;
	struct bt_conn *conn;
	struct bt_mesh_gatt_server *server;

	/* Avoid interconnection between proxy client and server */
	conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr);
	if (conn) {
		bt_conn_unref(conn);
		return -EALREADY;
	}

	err = bt_mesh_scan_disable();
	if (err) {
		return err;
	}

	LOG_DBG("Try to connect services");

	err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
				BT_LE_CONN_PARAM_DEFAULT, &conn);
	if (err) {
		LOG_ERR("Connection failed (err:%d)", err);

		(void)bt_mesh_scan_enable();

		return err;
	}

	server = get_server(conn);
	server->conn = conn;
	server->gatt = gatt;
	server->user_data = user_data;

	return 0;
}

#if ZEPHYR_RTK_PATCH
/* The peer is already connected with us as the central, e.g. the device also advertises while connected.
 * Use that connection instead of creating one: auto mode discover and subscribe.
 * Returns -ENOENT if there is no connection with the peer, so the caller can create one.
 */
int bt_mesh_gatt_cli_reuse(const bt_addr_le_t *addr, const struct bt_mesh_gatt_cli *gatt, void *user_data)
{
	struct bt_conn *conn;
	struct bt_conn_info info;
	struct bt_mesh_gatt_server *server;

	conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr);
	if (!conn) {
		return -ENOENT;
	}

	server = get_server(conn);
	if (bt_conn_get_info(conn, &info) || info.role != BT_CONN_ROLE_CENTRAL || info.state != BT_CONN_STATE_CONNECTED ||
	    server->conn || server->gatt || server->manual) {
		/* Peer is the central, not connected yet, or the connection is used by someone else */
		bt_conn_unref(conn);
		return -EALREADY;
	}

	/* The reference taken by the lookup is kept in server->conn, like a new connection does */
	server->conn = conn;
	server->gatt = gatt;
	server->user_data = user_data;

	gatt_cli_start_discover(conn, server);

	return 0;
}
#endif

#if ZEPHYR_RTK_PATCH
int bt_mesh_gatt_cli_connect_manual(const bt_addr_le_t *addr,
				    const struct bt_conn_le_create_param *create_param,
				    const struct bt_le_conn_param *conn_param)
{
	int err;
	struct bt_conn *conn;
	struct bt_mesh_gatt_server *server;

	conn = bt_conn_lookup_addr_le(BT_ID_DEFAULT, addr);
	if (conn) {
		bt_conn_unref(conn);
		return -EALREADY;
	}

	/* The mesh scan is left running, same as a plain GAP connect */
	err = bt_conn_le_create(addr, create_param, conn_param, &conn);
	if (err) {
		return err;
	}

	server = get_server(conn);
	server->conn = conn;
	server->manual = true;

	return 0;
}

int bt_mesh_gatt_cli_discover(struct bt_conn *conn, const struct bt_mesh_gatt_cli *gatt,
			      void *user_data, bt_mesh_gatt_cli_dis_cb result)
{
	struct bt_mesh_gatt_server *server = get_server(conn);
	int err;

	if (!server->manual || server->conn != conn) {
		return -ENOTCONN;
	}

	if (server->gatt) {
		return -EBUSY;
	}

	server->gatt = gatt;
	server->dis_result = result;
	server->ccc_handle = 0;
	server->gatt->connected(conn, user_data);

	server->discover.uuid = &server->gatt->srv_uuid.uuid;
	server->discover.func = discover_func;
	server->discover.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	server->discover.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	server->discover.type = BT_GATT_DISCOVER_PRIMARY;
	err = bt_gatt_discover(conn, &server->discover);
	if (err) {
		server->dis_result = NULL;
	}

	return err;
}

int bt_mesh_gatt_cli_set_notify(struct bt_conn *conn, bool enable)
{
	struct bt_mesh_gatt_server *server = get_server(conn);

	if (!server->manual || !server->gatt || server->conn != conn) {
		return -ENOTCONN;
	}

	if (!server->ccc_handle) {
		return -EINVAL;
	}

	if (!enable) {
		int err;

		if (!server->notify_on) {
			return -EALREADY;
		}

		err = bt_gatt_unsubscribe(conn, &server->subscribe);
		if (!err) {
			server->notify_on = false;
		}

		return err;
	}

	if (server->notify_on) {
		return -EALREADY;
	}

	(void)memset(&server->subscribe, 0, sizeof(server->subscribe));
	server->subscribe.notify = notify_func;
	server->subscribe.subscribe = notify_enabled;
	server->subscribe.value = BT_GATT_CCC_NOTIFY;
	server->subscribe.ccc_handle = server->ccc_handle;
	server->subscribe.value_handle = server->ccc_handle - 1;

	return bt_gatt_subscribe(conn, &server->subscribe);
}
#endif

static void gatt_advertising_recv(const struct bt_le_scan_recv_info *info,
				   struct net_buf_simple *buf)
{
	uint16_t uuid;

	if (buf->len < 3) {
		return;
	}

	uuid = net_buf_simple_pull_le16(buf);
	switch (uuid) {
#if defined(CONFIG_BT_MESH_PROXY_CLIENT)
	case BT_UUID_MESH_PROXY_VAL:
#if ZEPHYR_RTK_PATCH
		if (bt_mesh_prov && bt_mesh_prov->proxy_adv) {
			bt_mesh_prov->proxy_adv(info->addr, info->rssi, buf->data, buf->len);
		}

		if (!bt_mesh_proxy_has_avail_conn()) {
			break;
		}
#endif
		bt_mesh_proxy_cli_adv_recv(info, buf);
		break;
#endif
#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
	case BT_UUID_MESH_PROV_VAL:
#if ZEPHYR_RTK_PATCH
		if (!bt_mesh_proxy_has_avail_conn()) {
			break;
		}
#endif
		bt_mesh_pb_gatt_cli_adv_recv(info, buf);
		break;
#endif

	default:
		break;
	}
}

static void scan_recv(const struct bt_le_scan_recv_info *info,
		      struct net_buf_simple *buf)
{
	if (info->adv_type != BT_GAP_ADV_TYPE_ADV_IND) {
		return;
	}

#if !ZEPHYR_RTK_PATCH
	if (!bt_mesh_proxy_has_avail_conn()) {
		return;
	}
#endif

	while (buf->len > 1) {
		struct net_buf_simple_state state;
		uint8_t len, type;

		len = net_buf_simple_pull_u8(buf);
		/* Check for early termination */
		if (len == 0U) {
			return;
		}

		if (len > buf->len) {
			LOG_WRN("AD malformed");
			return;
		}

		net_buf_simple_save(buf, &state);

		type = net_buf_simple_pull_u8(buf);

		buf->len = len - 1;

		switch (type) {
		case BT_DATA_SVC_DATA16:
			gatt_advertising_recv(info, buf);
			break;
		default:
			break;
		}

		net_buf_simple_restore(buf, &state);
		net_buf_simple_pull(buf, len);
	}
}

static struct bt_le_scan_cb scan_cb = {
	.recv = scan_recv,
};

void bt_mesh_gatt_client_init(void)
{
	bt_le_scan_cb_register(&scan_cb);
}

void bt_mesh_gatt_client_deinit(void)
{
	bt_le_scan_cb_unregister(&scan_cb);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = gatt_connected,
	.disconnected = gatt_disconnected,
};
