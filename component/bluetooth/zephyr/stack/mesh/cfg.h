/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

void bt_mesh_cfg_default_set(void);
void bt_mesh_cfg_pending_store(void);
#if ZEPHYR_RTK_PATCH
void bt_mesh_cfg_sar_tx_override(void);
#endif

bool bt_mesh_fixed_group_match(uint16_t addr);
