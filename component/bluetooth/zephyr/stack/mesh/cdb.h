/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if ZEPHYR_RTK_PATCH
int bt_mesh_cdb_init(void);
void bt_mesh_cdb_deinit(void);
#endif
void bt_mesh_cdb_node_store(const struct bt_mesh_cdb_node *node);
void bt_mesh_cdb_pending_store(void);
