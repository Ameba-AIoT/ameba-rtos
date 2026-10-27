/*
 * Copyright (c) 2021 Xiaomi Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_MESH_GATT_CLI_H_
#define ZEPHYR_MESH_GATT_CLI_H_

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

struct bt_mesh_gatt_cli {
	struct bt_uuid_16 srv_uuid;
	struct bt_uuid_16 data_in_uuid;
	struct bt_uuid_16 data_out_uuid;
	struct bt_uuid_16 data_out_cccd_uuid;

	void (*connected)(struct bt_conn *conn, void *user_data);
	void (*link_open)(struct bt_conn *conn);
	void (*disconnected)(struct bt_conn *conn);
#if ZEPHYR_RTK_PATCH
	/* Optional, called when enabling notify failed (manual mode only) */
	void (*link_open_failed)(struct bt_conn *conn);
#endif
};

int bt_mesh_gatt_cli_connect(const bt_addr_le_t *addr,
			     const struct bt_mesh_gatt_cli *gatt,
			     void *user_data);

int bt_mesh_gatt_send(struct bt_conn *conn,
		      const void *data, uint16_t len,
		      bt_gatt_complete_func_t end, void *user_data);

#if ZEPHYR_RTK_PATCH
/* Manual mode: connect by address, then discover the service and enable notify by explicit steps */
enum bt_mesh_gatt_cli_dis_result {
	BT_MESH_GATT_CLI_DIS_DONE,
	BT_MESH_GATT_CLI_DIS_FAIL,
	BT_MESH_GATT_CLI_DIS_NOT_FOUND,
};

typedef void (*bt_mesh_gatt_cli_dis_cb)(struct bt_conn *conn, enum bt_mesh_gatt_cli_dis_result res);

int bt_mesh_gatt_cli_connect_manual(const bt_addr_le_t *addr,
				    const struct bt_conn_le_create_param *create_param,
				    const struct bt_le_conn_param *conn_param);
int bt_mesh_gatt_cli_discover(struct bt_conn *conn, const struct bt_mesh_gatt_cli *gatt,
			      void *user_data, bt_mesh_gatt_cli_dis_cb result);
int bt_mesh_gatt_cli_reuse(const bt_addr_le_t *addr, const struct bt_mesh_gatt_cli *gatt, void *user_data);
int bt_mesh_gatt_cli_set_notify(struct bt_conn *conn, bool enable);
#endif

void bt_mesh_gatt_client_init(void);

void bt_mesh_gatt_client_deinit(void);

#endif /* ZEPHYR_MESH_GATT_CLI_H_ */
