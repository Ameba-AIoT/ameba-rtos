#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_directed_forwarding_model.h>
#include <rtk_bt_mesh_def.h>
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL

uint16_t rtk_bt_mesh_directed_control_get(rtk_bt_mesh_df_client_act_directed_control_get_t *control_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_GET, control_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_control_set(rtk_bt_mesh_df_client_act_directed_control_set_t *control_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_SET, control_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_path_metric_get(rtk_bt_mesh_df_client_act_path_metric_get_t *path_metric_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_METRIC_GET, path_metric_get,
						  sizeof(rtk_bt_mesh_df_client_act_path_metric_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_path_metric_set(rtk_bt_mesh_df_client_act_path_metric_set_t *path_metric_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_METRIC_SET, path_metric_set,
						  sizeof(rtk_bt_mesh_df_client_act_path_metric_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_discovery_table_capabilities_get(rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *dis_tab_cap_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_GET, dis_tab_cap_get,
						  sizeof(rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_discovery_table_capabilities_set(rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *dis_tab_cap_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_SET, dis_tab_cap_set,
						  sizeof(rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_add(rtk_bt_mesh_df_client_act_forwarding_table_add_t *for_tab_add)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ADD, for_tab_add,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_delete(rtk_bt_mesh_df_client_act_forwarding_table_delete_t *for_tab_del)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DELETE, for_tab_del,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_delete_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_dependents_add(rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *for_tab_dep_add)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_ADD, for_tab_dep_add,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_dependents_delete(rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *for_tab_dep_del)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_DELETE, for_tab_dep_del,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_dependents_get(rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *for_tab_dep_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET, for_tab_dep_get,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_entries_count_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *for_tab_entries_count_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_GET, for_tab_entries_count_get,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_forwarding_table_entries_get(rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *for_tab_entries_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_GET, for_tab_entries_get,
						  sizeof(rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_wanted_lanes_get(rtk_bt_mesh_df_client_act_wanted_lanes_get_t *wanted_lanes_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_WANTED_LANES_GET, wanted_lanes_get,
						  sizeof(rtk_bt_mesh_df_client_act_wanted_lanes_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_wanted_lanes_set(rtk_bt_mesh_df_client_act_wanted_lanes_set_t *wanted_lanes_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_WANTED_LANES_SET, wanted_lanes_set,
						  sizeof(rtk_bt_mesh_df_client_act_wanted_lanes_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_two_way_path_get(rtk_bt_mesh_df_client_act_two_way_path_get_t *two_way_path_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_GET, two_way_path_get,
						  sizeof(rtk_bt_mesh_df_client_act_two_way_path_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_two_way_path_set(rtk_bt_mesh_df_client_act_two_way_path_set_t *two_way_path_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_SET, two_way_path_set,
						  sizeof(rtk_bt_mesh_df_client_act_two_way_path_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_path_echo_interval_get(rtk_bt_mesh_df_client_act_path_echo_interval_get_t *path_echo_interval_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_GET, path_echo_interval_get,
						  sizeof(rtk_bt_mesh_df_client_act_path_echo_interval_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_path_echo_interval_set(rtk_bt_mesh_df_client_act_path_echo_interval_set_t *path_echo_interval_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_SET, path_echo_interval_set,
						  sizeof(rtk_bt_mesh_df_client_act_path_echo_interval_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_network_transmit_get(rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *net_trans_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_GET, net_trans_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_network_transmit_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_network_transmit_set(rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *net_trans_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_SET, net_trans_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_network_transmit_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *relay_retrans_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_GET, relay_retrans_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *relay_retrans_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_SET, relay_retrans_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_rssi_threshold_get(rtk_bt_mesh_df_client_act_rssi_threshold_get_t *rssi_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_GET, rssi_get,
						  sizeof(rtk_bt_mesh_df_client_act_rssi_threshold_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_rssi_threshold_set(rtk_bt_mesh_df_client_act_rssi_threshold_set_t *rssi_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_SET, rssi_set,
						  sizeof(rtk_bt_mesh_df_client_act_rssi_threshold_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_paths_get(rtk_bt_mesh_df_client_act_directed_paths_get_t *path_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_GET, path_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_paths_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_publish_policy_get(rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *policy_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_GET, policy_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_publish_policy_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_publish_policy_set(rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *policy_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_SET, policy_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_publish_policy_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_discovery_timing_control_get(rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *timing_ctl_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_GET, timing_ctl_get,
						  sizeof(rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_discovery_timing_control_set(rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *timing_ctl_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_SET, timing_ctl_set,
						  sizeof(rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_control_network_transmit_get(rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *ctl_net_trans_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET, ctl_net_trans_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_control_network_transmit_set(rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_net_trans_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET, ctl_net_trans_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_control_relay_retransmit_get(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *ctl_relay_retrans_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET, ctl_relay_retrans_get,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_directed_control_relay_retransmit_set(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t *ctl_relay_retrans_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET, ctl_relay_retrans_set,
						  sizeof(rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_set_t));

	return ret;
}
#endif
#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING

uint16_t rtk_bt_mesh_df_path_discovery(rtk_bt_mesh_df_act_df_path_discovery_t *path_discovery)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_DISCOVERY, path_discovery,
						  sizeof(rtk_bt_mesh_df_act_df_path_discovery_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_solicitation(rtk_bt_mesh_df_act_df_path_solicitation_t *path_solicitation)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_SOLICITATION, path_solicitation,
						  sizeof(rtk_bt_mesh_df_act_df_path_solicitation_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_dependents_update(rtk_bt_mesh_df_act_df_path_dependents_update_t *path_dep_update)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_DEPENDENTS_UPDATE, path_dep_update,
						  sizeof(rtk_bt_mesh_df_act_df_path_dependents_update_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_control_state_get(rtk_bt_mesh_df_control_state_setting_t *ctl)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_GET, ctl,
						  sizeof(rtk_bt_mesh_df_control_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_control_state_set(rtk_bt_mesh_df_control_state_setting_t *ctl)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_SET, ctl,
						  sizeof(rtk_bt_mesh_df_control_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_metric_state_get(rtk_bt_mesh_df_path_metric_state_setting_t *pm)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_METRIC_STATE_GET, pm,
						  sizeof(rtk_bt_mesh_df_path_metric_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_metric_state_set(rtk_bt_mesh_df_path_metric_state_setting_t *pm)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_METRIC_STATE_SET, pm,
						  sizeof(rtk_bt_mesh_df_path_metric_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_get(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_GET, dtcs,
						  sizeof(rtk_bt_mesh_df_discovery_table_cap_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_discovery_table_capablities_state_set(rtk_bt_mesh_df_discovery_table_cap_state_setting_t *dtcs)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_SET, dtcs,
						  sizeof(rtk_bt_mesh_df_discovery_table_cap_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_wanted_lanes_state_get(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_WANTED_LANES_GET, wls,
						  sizeof(rtk_bt_mesh_df_wanted_lanes_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_wanted_lanes_state_set(rtk_bt_mesh_df_wanted_lanes_state_setting_t *wls)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_WANTED_LANES_SET, wls,
						  sizeof(rtk_bt_mesh_df_wanted_lanes_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_two_way_path_state_get(rtk_bt_mesh_df_two_way_path_state_setting_t *twps)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_TWO_WAY_PATH_GET, twps,
						  sizeof(rtk_bt_mesh_df_two_way_path_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_two_way_path_state_set(rtk_bt_mesh_df_two_way_path_state_setting_t *twps)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_TWO_WAY_PATH_SET, twps,
						  sizeof(rtk_bt_mesh_df_two_way_path_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_echo_interval_state_get(rtk_bt_mesh_df_echo_interval_state_setting_t *peis)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_GET, peis,
						  sizeof(rtk_bt_mesh_df_echo_interval_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_path_echo_interval_state_set(rtk_bt_mesh_df_echo_interval_state_setting_t *peis)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_SET, peis,
						  sizeof(rtk_bt_mesh_df_echo_interval_state_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_publish_policy_set(rtk_bt_mesh_df_publish_policy_setting_t *policy)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PUBLISH_POLICY_SET, policy,
						  sizeof(rtk_bt_mesh_df_publish_policy_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_df_parameter_set(rtk_bt_mesh_directed_control_param_setting_t *param)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_COMMON, RTK_BT_MESH_DF_PARAMETER_SET, param,
						  sizeof(rtk_bt_mesh_directed_control_param_setting_t));

	return ret;
}


#endif
#endif