/*
 * Copyright (c) 2021 Xiaomi Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


int bt_mesh_pb_gatt_cli_setup(const uint8_t uuid[16]);

void bt_mesh_pb_gatt_cli_adv_recv(const struct bt_le_scan_recv_info *info,
				  struct net_buf_simple *buf);

#if ZEPHYR_RTK_PATCH
#include "gatt_cli.h"

int bt_mesh_pb_gatt_cli_discover(struct bt_conn *conn, bt_mesh_gatt_cli_dis_cb result);
int bt_mesh_pb_gatt_cli_set_notify(struct bt_conn *conn, bool enable);
#endif
