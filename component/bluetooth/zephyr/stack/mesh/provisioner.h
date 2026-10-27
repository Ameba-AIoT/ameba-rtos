/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2020 Lingao Meng
 *
 * SPDX-License-Identifier: Apache-2.0
 */

int bt_mesh_pb_adv_open(const uint8_t uuid[16], uint16_t net_idx, uint16_t addr,
			uint8_t attention_duration);

int bt_mesh_pb_gatt_open(const uint8_t uuid[16], uint16_t net_idx, uint16_t addr,
			 uint8_t attention_duration);

#if ZEPHYR_RTK_PATCH
int bt_mesh_pb_adv_link_open(const uint8_t uuid[16], uint16_t net_idx);
int bt_mesh_prov_invite(uint16_t addr, uint8_t attention_duration);
#if defined(CONFIG_BT_MESH_PB_GATT_CLIENT)
int bt_mesh_pb_gatt_prov_link_open(uint16_t net_idx);
void bt_mesh_pb_gatt_prov_link_abort(void);
#endif
#if defined(CONFIG_BT_MESH_RPR_CLI)
int bt_mesh_pb_remote_link_open(struct bt_mesh_rpr_cli *cli, const struct bt_mesh_rpr_node *srv,
				const uint8_t uuid[16], uint16_t net_idx, uint8_t link_open_timeout);
int bt_mesh_pb_remote_link_open_node(struct bt_mesh_rpr_cli *cli, struct bt_mesh_rpr_node *srv,
				     enum bt_mesh_rpr_node_refresh refresh);
#endif
#endif
