/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_directed_forwarding_model.h>

static int atcmd_ble_mesh_df_path_discovery(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	uint16_t ret = 0;
	rtk_bt_mesh_df_act_df_path_discovery_t pd;
	pd.master_key_index = str_to_int(argv[0]);
	pd.target_addr = str_to_int(argv[1]);
	pd.dependent_addr = str_to_int(argv[2]);
	pd.dependent_elem_num = str_to_int(argv[3]);
	ret = rtk_bt_mesh_df_path_discovery(&pd);
	if (ret) {
		BT_LOGE("[%s] Path discovery failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_path_solicitation(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	uint16_t ret = 0;
	rtk_bt_mesh_df_act_df_path_solicitation_t ps;
	ps.master_key_index = str_to_int(argv[0]);
	ps.addr_num = str_to_int(argv[1]);
	if (ps.addr_num < DIRECTED_FORWARDING_MAX_ADDR_LIST_SIZE) {
		for (int i = 0; i < ps.addr_num; i++) {
			ps.addr[i] = str_to_int(argv[i + 2]);
		}
		ret = rtk_bt_mesh_df_path_solicitation(&ps);
		if (ret) {
			BT_LOGE("[%s] Path solicitation failed! reason: 0x%x", __func__, ret);
			return -1;
		}
	} else {
		BT_LOGE("[%s] Input data is too large.\r\n", __func__);
		return -3;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_path_dependents_update(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	uint16_t ret = 0;
	rtk_bt_mesh_df_act_df_path_dependents_update_t pdu;
	pdu.type = str_to_int(argv[0]);
	pdu.endpoint_addr = str_to_int(argv[1]);
	pdu.dependent_addr = str_to_int(argv[2]);
	pdu.dependent_elem_num = str_to_int(argv[3]);
	ret = rtk_bt_mesh_df_path_dependents_update(&pdu);
	if (ret) {
		BT_LOGE("[%s] Path dependents_update failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_model_df_use(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	uint16_t ret = 0;
	rtk_bt_mesh_df_publish_policy_setting_t policy;
	policy.element_index = str_to_int(argv[0]);
	policy.model_id = str_to_int(argv[1]);
	policy.policy = str_to_int(argv[2]);
	ret = rtk_bt_mesh_df_publish_policy_set(&policy);
	if (ret) {
		BT_LOGE("[%s] Mesh model use df transfer setting failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_control_state_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
	uint16_t ret = 0;
	rtk_bt_mesh_df_control_state_setting_t control_state;
	control_state.master_key_index = str_to_int(argv[0]);
	control_state.ctl.directed_forwarding = str_to_int(argv[1]);
	control_state.ctl.directed_relay = str_to_int(argv[2]);
	control_state.ctl.directed_proxy = str_to_int(argv[3]);
	control_state.ctl.directed_proxy_use_directed_default = str_to_int(argv[4]);
	control_state.ctl.directed_friend = str_to_int(argv[5]);
	ret = rtk_bt_mesh_df_control_state_set(&control_state);
	if (ret) {
		BT_LOGE("[%s] Mesh df control state setting failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_get_t control_get;
	control_get.dst = str_to_int(argv[0]);
	control_get.net_key_index = str_to_int(argv[1]);
	control_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_directed_control_get(&control_get);
	if (ret) {
		BT_LOGE("[%s] Directed control get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_set_t control_set;
	control_set.dst = str_to_int(argv[0]);
	control_set.net_key_index = str_to_int(argv[1]);
	control_set.net_key_index_df = str_to_int(argv[2]);
	control_set.directed_forwarding = str_to_int(argv[3]);
	control_set.directed_relay = str_to_int(argv[4]);
	control_set.directed_proxy = str_to_int(argv[5]);
	control_set.directed_proxy_use_directed_default = str_to_int(argv[6]);
	control_set.directed_friend = str_to_int(argv[7]);
	ret = rtk_bt_mesh_directed_control_set(&control_set);
	if (ret) {
		BT_LOGE("[%s] Directed control set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_path_metric_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_metric_get_t metric_get;
	metric_get.dst = str_to_int(argv[0]);
	metric_get.net_key_index = str_to_int(argv[1]);
	metric_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_path_metric_get(&metric_get);
	if (ret) {
		BT_LOGE("[%s] Path metric get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_path_metric_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_metric_set_t metric_set;
	metric_set.dst = str_to_int(argv[0]);
	metric_set.net_key_index = str_to_int(argv[1]);
	metric_set.net_key_index_df = str_to_int(argv[2]);
	metric_set.metric_type = str_to_int(argv[3]);
	metric_set.lifetime = str_to_int(argv[4]);
	ret = rtk_bt_mesh_path_metric_set(&metric_set);
	if (ret) {
		BT_LOGE("[%s] Path metric set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_discovery_table_capabilities_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t capabilities_get;
	capabilities_get.dst = str_to_int(argv[0]);
	capabilities_get.net_key_index = str_to_int(argv[1]);
	capabilities_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_discovery_table_capabilities_get(&capabilities_get);
	if (ret) {
		BT_LOGE("[%s] Discovery table capabilities get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_discovery_table_capabilities_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t capabilities_set;
	capabilities_set.dst = str_to_int(argv[0]);
	capabilities_set.net_key_index = str_to_int(argv[1]);
	capabilities_set.net_key_index_df = str_to_int(argv[2]);
	capabilities_set.max_concurrent_init = str_to_int(argv[3]);
	ret = rtk_bt_mesh_discovery_table_capabilities_set(&capabilities_set);
	if (ret) {
		BT_LOGE("[%s] Discovery table capabilities set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_add(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_add_t table_add;
	table_add.dst = str_to_int(argv[0]);
	table_add.net_key_index = str_to_int(argv[1]);
	table_add.net_key_index_df = str_to_int(argv[2]);
	table_add.unicast_dst_flag = str_to_int(argv[3]);
	table_add.backward_path_valid = str_to_int(argv[4]);
	table_add.origin_addr = str_to_int(argv[5]);
	table_add.origin_element_num = str_to_int(argv[6]);
	table_add.target_addr = str_to_int(argv[7]);
	table_add.target_element_num = str_to_int(argv[8]);
	table_add.bearer_toward_origin = str_to_int(argv[9]);
	table_add.bearer_toward_target = str_to_int(argv[10]);
	ret = rtk_bt_mesh_forwarding_table_add(&table_add);
	if (ret) {
		BT_LOGE("[%s] Forwarding table add failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_delete(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_delete_t table_delete;
	table_delete.dst = str_to_int(argv[0]);
	table_delete.net_key_index = str_to_int(argv[1]);
	table_delete.net_key_index_df = str_to_int(argv[2]);
	table_delete.path_origin = str_to_int(argv[3]);
	table_delete.path_target = str_to_int(argv[4]);
	ret = rtk_bt_mesh_forwarding_table_delete(&table_delete);
	if (ret) {
		BT_LOGE("[%s] Forwarding table delete failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_dependents_add(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t table_dependent_add;
	table_dependent_add.dst = str_to_int(argv[0]);
	table_dependent_add.net_key_index = str_to_int(argv[1]);
	table_dependent_add.net_key_index_df = str_to_int(argv[2]);
	table_dependent_add.path_origin = str_to_int(argv[3]);
	table_dependent_add.path_target = str_to_int(argv[4]);
	table_dependent_add.do_list_size = str_to_int(argv[5]);
	table_dependent_add.dt_list_size = str_to_int(argv[6]);
	int i = 0;
	int j = 0;
	for (; i < table_dependent_add.do_list_size; i++) {
		table_dependent_add.do_range[i].addr = str_to_int(argv[7 + 2 * i]);
		table_dependent_add.do_range[i].element_num = str_to_int(argv[8 + 2 * i]);
	}
	for (; j < table_dependent_add.dt_list_size; j++) {
		table_dependent_add.dt_range[j].addr = str_to_int(argv[7 + 2 * i + 2 * j]);
		table_dependent_add.dt_range[j].element_num = str_to_int(argv[8 + 2 * i + 2 * j]);
	}
	ret = rtk_bt_mesh_forwarding_table_dependents_add(&table_dependent_add);
	if (ret) {
		BT_LOGE("[%s] Forwarding table dependents add failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_dependents_delete(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t table_dependent_delete;
	table_dependent_delete.dst = str_to_int(argv[0]);
	table_dependent_delete.net_key_index = str_to_int(argv[1]);
	table_dependent_delete.net_key_index_df = str_to_int(argv[2]);
	table_dependent_delete.path_origin = str_to_int(argv[3]);
	table_dependent_delete.path_target = str_to_int(argv[4]);
	table_dependent_delete.do_list_size = str_to_int(argv[5]);
	table_dependent_delete.dt_list_size = str_to_int(argv[6]);
	int i = 0;
	int j = 0;
	for (; i < table_dependent_delete.do_list_size; i++) {
		table_dependent_delete.do_range[i] = str_to_int(argv[7 + i]);
	}
	for (; j < table_dependent_delete.dt_list_size; j++) {
		table_dependent_delete.dt_range[j] = str_to_int(argv[7 + i + j]);
	}
	ret = rtk_bt_mesh_forwarding_table_dependents_delete(&table_dependent_delete);
	if (ret) {
		BT_LOGE("[%s] Forwarding table dependents delete failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_dependents_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t dependent_get;
	dependent_get.dst = str_to_int(argv[0]);
	dependent_get.net_key_index = str_to_int(argv[1]);
	dependent_get.net_key_index_df = str_to_int(argv[2]);
	dependent_get.list_mask = str_to_int(argv[3]);
	dependent_get.fixed_path_flag = str_to_int(argv[4]);
	dependent_get.start_index = str_to_int(argv[5]);
	dependent_get.path_origin = str_to_int(argv[6]);
	dependent_get.path_target = str_to_int(argv[7]);
	dependent_get.update_id_exist = 0;
	if (argc > 8) {
		dependent_get.update_id_exist = 1;
		dependent_get.update_id = str_to_int(argv[8]);
	}
	ret = rtk_bt_mesh_forwarding_table_dependents_get(&dependent_get);
	if (ret) {
		BT_LOGE("[%s] Forwarding table dependents get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_entries_count_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t entries_count_get;
	entries_count_get.dst = str_to_int(argv[0]);
	entries_count_get.net_key_index = str_to_int(argv[1]);
	entries_count_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_forwarding_table_entries_count_get(&entries_count_get);
	if (ret) {
		BT_LOGE("[%s] Forwarding table entries count get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_forwarding_table_entries_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t entries_get = {0};
	entries_get.dst = str_to_int(argv[0]);
	entries_get.net_key_index = str_to_int(argv[1]);
	entries_get.net_key_index_df = str_to_int(argv[2]);
	entries_get.filter_mask = str_to_int(argv[3]);
	entries_get.start_index = str_to_int(argv[4]);
	int i = 5;
	if (entries_get.filter_mask & 0x04) {
		entries_get.path_origin = str_to_int(argv[i]);
		i++;
	}
	if (entries_get.filter_mask & 0x08) {
		entries_get.path_target = str_to_int(argv[i]);
		i++;
	}
	entries_get.update_id_flag = 0;
	if (argc > i) {
		entries_get.update_id_flag = 1;
		entries_get.update_id = str_to_int(argv[i]);
	}
	ret = rtk_bt_mesh_forwarding_table_entries_get(&entries_get);
	if (ret) {
		BT_LOGE("[%s] Forwarding table entries get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_wanted_lanes_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_wanted_lanes_get_t lanes_get;
	lanes_get.dst = str_to_int(argv[0]);
	lanes_get.net_key_index = str_to_int(argv[1]);
	lanes_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_wanted_lanes_get(&lanes_get);
	if (ret) {
		BT_LOGE("[%s] Wanted lanes get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_wanted_lanes_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_wanted_lanes_set_t lanes_set;
	lanes_set.dst = str_to_int(argv[0]);
	lanes_set.net_key_index = str_to_int(argv[1]);
	lanes_set.net_key_index_df = str_to_int(argv[2]);
	lanes_set.wanted_lanes = str_to_int(argv[3]);
	ret = rtk_bt_mesh_wanted_lanes_set(&lanes_set);
	if (ret) {
		BT_LOGE("[%s] Wanted lanes set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_two_way_path_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_two_way_path_get_t two_way_get;
	two_way_get.dst = str_to_int(argv[0]);
	two_way_get.net_key_index = str_to_int(argv[1]);
	two_way_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_two_way_path_get(&two_way_get);
	if (ret) {
		BT_LOGE("[%s] Two way path get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_two_way_path_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_two_way_path_set_t two_way_set;
	two_way_set.dst = str_to_int(argv[0]);
	two_way_set.net_key_index = str_to_int(argv[1]);
	two_way_set.net_key_index_df = str_to_int(argv[2]);
	two_way_set.two_way_path = str_to_int(argv[3]);
	ret = rtk_bt_mesh_two_way_path_set(&two_way_set);
	if (ret) {
		BT_LOGE("[%s] Two way path set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_path_echo_interval_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_echo_interval_get_t echo_get;
	echo_get.dst = str_to_int(argv[0]);
	echo_get.net_key_index = str_to_int(argv[1]);
	echo_get.net_key_index_df = str_to_int(argv[2]);
	ret = rtk_bt_mesh_path_echo_interval_get(&echo_get);
	if (ret) {
		BT_LOGE("[%s] Path echo interval get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_path_echo_interval_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_echo_interval_set_t echo_set;
	echo_set.dst = str_to_int(argv[0]);
	echo_set.net_key_index = str_to_int(argv[1]);
	echo_set.net_key_index_df = str_to_int(argv[2]);
	echo_set.unicast_echo_interval = str_to_int(argv[3]);
	echo_set.multicast_echo_interval = str_to_int(argv[4]);
	ret = rtk_bt_mesh_path_echo_interval_set(&echo_set);
	if (ret) {
		BT_LOGE("[%s] Path echo interval set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_network_transmit_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_network_transmit_get_t net_trans;
	net_trans.dst = str_to_int(argv[0]);
	net_trans.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_directed_network_transmit_get(&net_trans);
	if (ret) {
		BT_LOGE("[%s] Directed network transmit get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_network_transmit_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_network_transmit_set_t net_trans;
	net_trans.dst = str_to_int(argv[0]);
	net_trans.net_key_index = str_to_int(argv[1]);
	net_trans.count = str_to_int(argv[2]);
	net_trans.steps = str_to_int(argv[3]);
	ret = rtk_bt_mesh_directed_network_transmit_set(&net_trans);
	if (ret) {
		BT_LOGE("[%s] Directed network transmit set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_relay_retransmit_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t relay_trans;
	relay_trans.dst = str_to_int(argv[0]);
	relay_trans.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_directed_relay_retransmit_get(&relay_trans);
	if (ret) {
		BT_LOGE("[%s] Directed relay retransmit get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_relay_retransmit_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t relay_trans;
	relay_trans.dst = str_to_int(argv[0]);
	relay_trans.net_key_index = str_to_int(argv[1]);
	relay_trans.count = str_to_int(argv[2]);
	relay_trans.steps = str_to_int(argv[3]);
	ret = rtk_bt_mesh_directed_relay_retransmit_set(&relay_trans);
	if (ret) {
		BT_LOGE("[%s] Directed relay retransmit set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_rssi_threshold_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_rssi_threshold_get_t rssi_get;
	rssi_get.dst = str_to_int(argv[0]);
	rssi_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_rssi_threshold_get(&rssi_get);
	if (ret) {
		BT_LOGE("[%s] Rssi threshold get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_rssi_threshold_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_rssi_threshold_set_t rssi_set;
	rssi_set.dst = str_to_int(argv[0]);
	rssi_set.net_key_index = str_to_int(argv[1]);
	rssi_set.rssi_margin = str_to_int(argv[2]);
	ret = rtk_bt_mesh_rssi_threshold_set(&rssi_set);
	if (ret) {
		BT_LOGE("[%s] Rssi threshold set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_paths_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_paths_get_t path_get;
	path_get.dst = str_to_int(argv[0]);
	path_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_directed_paths_get(&path_get);
	if (ret) {
		BT_LOGE("[%s] Directed paths get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_publish_policy_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_publish_policy_get_t policy_get;
	policy_get.dst = str_to_int(argv[0]);
	policy_get.net_key_index = str_to_int(argv[1]);
	policy_get.elem_addr = str_to_int(argv[2]);
	policy_get.model_id = str_to_int(argv[3]);
	ret = rtk_bt_mesh_directed_publish_policy_get(&policy_get);
	if (ret) {
		BT_LOGE("[%s] Directed publish policy get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_publish_policy_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_publish_policy_set_t policy_set;
	policy_set.dst = str_to_int(argv[0]);
	policy_set.net_key_index = str_to_int(argv[1]);
	policy_set.policy = str_to_int(argv[2]);
	policy_set.elem_addr = str_to_int(argv[3]);
	policy_set.model_id = str_to_int(argv[4]);
	ret = rtk_bt_mesh_directed_publish_policy_set(&policy_set);
	if (ret) {
		BT_LOGE("[%s] Directed publish policy set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_discovery_timing_control_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t timing_get;
	timing_get.dst = str_to_int(argv[0]);
	timing_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_discovery_timing_control_get(&timing_get);
	if (ret) {
		BT_LOGE("[%s] Path discovery timing control get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_discovery_timing_control_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t timing_set;
	timing_set.dst = str_to_int(argv[0]);
	timing_set.net_key_index = str_to_int(argv[1]);
	timing_set.monitor_interval = str_to_int(argv[2]);
	timing_set.dis_retry_interval = str_to_int(argv[3]);
	timing_set.dis_interval = str_to_int(argv[4]);
	timing_set.lan_guard = str_to_int(argv[5]);
	ret = rtk_bt_mesh_discovery_timing_control_set(&timing_set);
	if (ret) {
		BT_LOGE("[%s] Path discovery timing control set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_network_transmit_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t ctl_net_trans;
	ctl_net_trans.dst = str_to_int(argv[0]);
	ctl_net_trans.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_directed_control_network_transmit_get(&ctl_net_trans);
	if (ret) {
		BT_LOGE("[%s] Directed control network transmit get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_network_transmit_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t ctl_net_trans;
	ctl_net_trans.dst = str_to_int(argv[0]);
	ctl_net_trans.net_key_index = str_to_int(argv[1]);
	ctl_net_trans.count = str_to_int(argv[2]);
	ctl_net_trans.steps = str_to_int(argv[3]);
	ret = rtk_bt_mesh_directed_control_network_transmit_set(&ctl_net_trans);
	if (ret) {
		BT_LOGE("[%s] Directed control network transmit set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_relay_retransmit_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t ctl_relay_trans;
	ctl_relay_trans.dst = str_to_int(argv[0]);
	ctl_relay_trans.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_directed_control_relay_retransmit_get(&ctl_relay_trans);
	if (ret) {
		BT_LOGE("[%s] Directed control relay retransmit get failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static int atcmd_ble_mesh_df_client_directed_control_relay_retransmit_set(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t ctl_relay_trans;
	ctl_relay_trans.dst = str_to_int(argv[0]);
	ctl_relay_trans.net_key_index = str_to_int(argv[1]);
	ctl_relay_trans.count = str_to_int(argv[2]);
	ctl_relay_trans.steps = str_to_int(argv[3]);
	ret = rtk_bt_mesh_directed_control_relay_retransmit_set(&ctl_relay_trans);
	if (ret) {
		BT_LOGE("[%s] Directed control relay retransmit set failed! reason: 0x%x", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support directed forwarding or role is not provisioner.");
	return -1;
#endif
}

static const cmd_table_t mesh_df_client_cmd_table[] = {
	{"dfpdis",    atcmd_ble_mesh_df_path_discovery,    5, 5},
	{"dfpsol",    atcmd_ble_mesh_df_path_solicitation,    3, 3 + 3},
	{"dfpdupt",    atcmd_ble_mesh_df_path_dependents_update,    5, 5},
	{"mdu",    atcmd_ble_mesh_df_model_df_use,    4, 4},
	{"dcss",    atcmd_ble_mesh_df_control_state_set,    7, 7},
	{"dcg",    atcmd_ble_mesh_df_client_directed_control_get,    4, 4},
	{"dcs",    atcmd_ble_mesh_df_client_directed_control_set,    9, 9},
	{"pmg",    atcmd_ble_mesh_df_client_path_metric_get,    4, 4},
	{"pms",    atcmd_ble_mesh_df_client_path_metric_set,    6, 6},
	{"dtcg",    atcmd_ble_mesh_df_client_discovery_table_capabilities_get,    4, 4},
	{"dtcs",    atcmd_ble_mesh_df_client_discovery_table_capabilities_set,    5, 5},
	{"fta",    atcmd_ble_mesh_df_client_forwarding_table_add,    12, 12},
	{"ftd",    atcmd_ble_mesh_df_client_forwarding_table_delete,    6, 6},
	{"ftda",    atcmd_ble_mesh_df_client_forwarding_table_dependents_add,    8, 8 + 4 * 3},
	{"ftdd",    atcmd_ble_mesh_df_client_forwarding_table_dependents_delete,    8, 8 + 2 * 3},
	{"ftdg",    atcmd_ble_mesh_df_client_forwarding_table_dependents_get,    9, 10},
	{"ftecg",    atcmd_ble_mesh_df_client_forwarding_table_entries_count_get,    4, 4},
	{"fteg",    atcmd_ble_mesh_df_client_forwarding_table_entries_get,    6, 9},
	{"wlg",    atcmd_ble_mesh_df_client_wanted_lanes_get,    4, 4},
	{"wls",    atcmd_ble_mesh_df_client_wanted_lanes_set,    5, 5},
	{"twpg",    atcmd_ble_mesh_df_client_two_way_path_get,    4, 4},
	{"twps",    atcmd_ble_mesh_df_client_two_way_path_set,    5, 5},
	{"peig",    atcmd_ble_mesh_df_client_path_echo_interval_get,    4, 4},
	{"peis",    atcmd_ble_mesh_df_client_path_echo_interval_set,    6, 6},
	{"dntg",    atcmd_ble_mesh_df_client_directed_network_transmit_get,    3, 3},
	{"dnts",    atcmd_ble_mesh_df_client_directed_network_transmit_set,    5, 5},
	{"drrg",    atcmd_ble_mesh_df_client_directed_relay_retransmit_get,    3, 3},
	{"drrs",    atcmd_ble_mesh_df_client_directed_relay_retransmit_set,    5, 5},
	{"rssitg",    atcmd_ble_mesh_df_client_rssi_threshold_get,    3, 3},
	{"rssits",    atcmd_ble_mesh_df_client_rssi_threshold_set,    4, 4},
	{"dpg",    atcmd_ble_mesh_df_client_directed_paths_get,    3, 3},
	{"dppg",    atcmd_ble_mesh_df_client_directed_publish_policy_get,    5, 5},
	{"dpps",    atcmd_ble_mesh_df_client_directed_publish_policy_set,    6, 6},
	{"pdtcg",    atcmd_ble_mesh_df_client_discovery_timing_control_get,    3, 3},
	{"pdtcs",    atcmd_ble_mesh_df_client_discovery_timing_control_set,    7, 7},
	{"dcntg",    atcmd_ble_mesh_df_client_directed_control_network_transmit_get,    3, 3},
	{"dcnts",    atcmd_ble_mesh_df_client_directed_control_network_transmit_set,    5, 5},
	{"dcrrg",    atcmd_ble_mesh_df_client_directed_control_relay_retransmit_get,    3, 3},
	{"dcrrs",    atcmd_ble_mesh_df_client_directed_control_relay_retransmit_set,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_df(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, mesh_df_client_cmd_table, "AT+BLEMESHDF");
	return 0;
}



#endif // end of RTK_BLE_MESH_SUPPORT