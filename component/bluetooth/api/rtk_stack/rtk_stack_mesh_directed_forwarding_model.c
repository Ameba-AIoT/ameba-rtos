#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_directed_forwarding_model.h>
#include <rtk_bt_mesh_def.h>
#include <directed_forwarding_model.h>
#include <mesh_node.h>

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING) && BT_MESH_ENABLE_DIRECTED_FORWARDING
uint16_t rtk_stack_df_paramerter_setting(rtk_bt_mesh_directed_control_param_setting_t *param)
{
	if (param->bit_check & DF_NETWORK_TRANSMIT_SETTING) {
		mesh_node.df_net_trans_count = param->network_transmit.directed_network_transmit_count;
		mesh_node.df_net_trans_steps = param->network_transmit.directed_network_transmit_interval_steps;
	}

	if (param->bit_check & DF_RELAY_TRANSMIT_SETTING) {
		mesh_node.df_relay_retrans_count = param->relay_retransmit.directed_relay_retransmit_count;
		mesh_node.df_relay_retrans_steps = param->relay_retransmit.directed_relay_retransmit_interval_steps;
	}

	if (param->bit_check & DF_CTL_NETWORK_TRANSMIT_SETTING) {
		mesh_node.df_net_ctl_trans_count = param->control_network_transmit.directed_control_network_transmit_count;
		mesh_node.df_net_ctl_trans_steps = param->control_network_transmit.directed_control_network_transmit_interval_steps;
	}

	if (param->bit_check & DF_CTL_RELAY_TRANSMIT_SETTING) {
		mesh_node.df_relay_ctl_retrans_count = param->control_relay_retransmit.directed_control_relay_retransmit_count;
		mesh_node.df_relay_ctl_retrans_steps = param->control_relay_retransmit.directed_control_relay_retransmit_interval_steps;
	}

	if (param->bit_check & DF_TIMING_CTL_SETTING) {
		mesh_node.path_monitoring_interval = param->timing_ctl.path_monitoring_interval;
		mesh_node.path_discovery_retry_interval = param->timing_ctl.path_discovery_retry_interval;
		mesh_node.path_discovery_interval = param->timing_ctl.path_discovery_interval;
		mesh_node.lane_discovery_guard_interval = param->timing_ctl.lane_discovery_guard_interval;
	}

	if (param->bit_check & DF_RSSI_SETTING) {
		mesh_node.df_default_rssi_threshold = param->threshold.default_rssi_threshold;
		if (param->threshold.rssi_margin <= 0x32) {
			mesh_node.df_rssi_margin = param->threshold.rssi_margin;
		} else {
			BT_LOGE("DF Rssi Setting is out of the range, rssi_margin is 0 - 50 \r\n");
		}
	}

	if (param->bit_check & DF_DIRECTED_PATHS_SETTING) {
		mesh_node.directed_node_paths = param->directed_paths.directed_node_paths;
		mesh_node.directed_relay_paths = param->directed_paths.directed_relay_paths;
		mesh_node.directed_proxy_paths = param->directed_paths.directed_proxy_paths;
		mesh_node.directed_friend_paths = param->directed_paths.directed_friend_paths;
	}

	if (param->bit_check & DF_DEPENDENT_ADDR_SIZE_SETTING) {
		mesh_node.dependent_addr_size = param->dependent_addr_size;
	}

	if (param->bit_check & DF_FIXED_PATH_SIZE_SETTING) {
		mesh_node.df_fixed_path_size = param->df_fixed_path_size;
	}

	if (param->bit_check & DF_NON_FIXED_PATH_SIZE_SETTING) {
		mesh_node.df_non_fixed_path_size = param->df_non_fixed_path_size;
	}

	return 0;
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_directed_forwarding_common_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_DF_PATH_DISCOVERY: {
		rtk_bt_mesh_df_act_df_path_discovery_t *path_discovery = (rtk_bt_mesh_df_act_df_path_discovery_t *)p_cmd->param;
		if (MESH_NOT_UNASSIGNED_ADDR(path_discovery->dependent_addr)) {
			if (path_discovery->dependent_elem_num > 0) {

			} else {
				BT_LOGE("wrong dependent elem num: %d\r\n", path_discovery->dependent_elem_num);
				ret = RTK_BT_ERR_PARAM_INVALID;
			}
		}
		ret = df_path_discovery(path_discovery->master_key_index, path_discovery->target_addr, path_discovery->dependent_addr, path_discovery->dependent_elem_num);
		break;
	}
	case RTK_BT_MESH_DF_PATH_SOLICITATION: {
		rtk_bt_mesh_df_act_df_path_solicitation_t *path_solicitation = (rtk_bt_mesh_df_act_df_path_solicitation_t *)p_cmd->param;
		uint16_t *addr_list = plt_malloc(path_solicitation->addr_num * sizeof(uint16_t), RAM_TYPE_DATA_ON);
		for (uint16_t i = 0; i < path_solicitation->addr_num; i++) {
			addr_list[i] = path_solicitation->addr[i];
		}
		df_path_solicitation(path_solicitation->master_key_index, addr_list, path_solicitation->addr_num);
		plt_free(addr_list, RAM_TYPE_DATA_ON);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_PATH_DEPENDENTS_UPDATE: {
		rtk_bt_mesh_df_act_df_path_dependents_update_t *path_update = (rtk_bt_mesh_df_act_df_path_dependents_update_t *)p_cmd->param;
		df_path_dependents_update(path_update->type, path_update->endpoint_addr, path_update->dependent_addr, path_update->dependent_elem_num);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_SET: {
		rtk_bt_mesh_df_control_state_setting_t *ctl_state = (rtk_bt_mesh_df_control_state_setting_t *)p_cmd->param;
		directed_control_t ctl = {ctl_state->ctl.directed_forwarding, ctl_state->ctl.directed_relay, ctl_state->ctl.directed_proxy,
								  ctl_state->ctl.directed_proxy_use_directed_default, ctl_state->ctl.directed_friend
								 };
		uint16_t df_index = df_control_set(ctl_state->master_key_index, ctl);
		mesh_flash_store(MESH_FLASH_PARAMS_DF_DIRECTED_CTL, &df_index);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_DIRECTED_CONTROL_STATE_GET: {
		rtk_bt_mesh_df_control_state_setting_t *ctl_state = (rtk_bt_mesh_df_control_state_setting_t *)p_cmd->param;
		directed_control_t ctl = df_control_get(ctl_state->master_key_index);
		memcpy(&ctl_state->ctl, &ctl, sizeof(directed_control_t));
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	/*****-------------------------------------------*****/
	case RTK_BT_MESH_DF_PATH_METRIC_STATE_GET: {
		rtk_bt_mesh_df_path_metric_state_setting_t *pm_state = (rtk_bt_mesh_df_path_metric_state_setting_t *)p_cmd->param;
		path_metric_t pm = df_path_metric_get(pm_state->master_key_index);
		memcpy(&pm_state->metric, &pm, sizeof(path_metric_t));
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_PATH_METRIC_STATE_SET: {
		rtk_bt_mesh_df_path_metric_state_setting_t *pm_state = (rtk_bt_mesh_df_path_metric_state_setting_t *)p_cmd->param;
		path_metric_t pm = {pm_state->metric.metric_type, pm_state->metric.lifetime, pm_state->metric.prohibited};
		ret = df_path_metric_set(pm_state->master_key_index, pm);
		break;
	}
	case RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_GET: {
		rtk_bt_mesh_df_discovery_table_cap_state_setting_t *cap_state = (rtk_bt_mesh_df_discovery_table_cap_state_setting_t *)p_cmd->param;
		discovery_table_capabilities_t cap = df_discovery_table_cap_get(cap_state->master_key_index);
		memcpy(&cap_state->cap, &cap, sizeof(discovery_table_capabilities_t));
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_DISCOVERY_TABLE_CAP_SET: {
		rtk_bt_mesh_df_discovery_table_cap_state_setting_t *cap_state = (rtk_bt_mesh_df_discovery_table_cap_state_setting_t *)p_cmd->param;
		discovery_table_capabilities_t cap = {cap_state->cap.max_concurrent_init, cap_state->cap.max_discovery_table_entries_count};
		ret = df_discovery_table_cap_set(cap_state->master_key_index, cap);
		break;
	}
	case RTK_BT_MESH_DF_WANTED_LANES_GET: {
		rtk_bt_mesh_df_wanted_lanes_state_setting_t *wl_state = (rtk_bt_mesh_df_wanted_lanes_state_setting_t *)p_cmd->param;
		wl_state->wanted_lanes = df_wanted_lanes_get(wl_state->master_key_index);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_WANTED_LANES_SET: {
		rtk_bt_mesh_df_wanted_lanes_state_setting_t *wl_state = (rtk_bt_mesh_df_wanted_lanes_state_setting_t *)p_cmd->param;
		ret = df_wanted_lanes_set(wl_state->master_key_index, wl_state->wanted_lanes);
		break;
	}
	case RTK_BT_MESH_DF_TWO_WAY_PATH_GET: {
		rtk_bt_mesh_df_two_way_path_state_setting_t *twp_state = (rtk_bt_mesh_df_two_way_path_state_setting_t *)p_cmd->param;
		twp_state->two_way_path = df_two_way_path_get(twp_state->master_key_index);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_TWO_WAY_PATH_SET: {
		rtk_bt_mesh_df_two_way_path_state_setting_t *twp_state = (rtk_bt_mesh_df_two_way_path_state_setting_t *)p_cmd->param;
		ret = df_two_way_path_set(twp_state->master_key_index, twp_state->two_way_path);
		break;
	}
	case RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_GET: {
		rtk_bt_mesh_df_echo_interval_state_setting_t *pei_state = (rtk_bt_mesh_df_echo_interval_state_setting_t *)p_cmd->param;
		pei_state->unicast_echo_interval = df_unicast_echo_interval_get(pei_state->master_key_index);
		pei_state->multicast_echo_interval = df_multicast_echo_interval_get(pei_state->master_key_index);
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_PATH_ECHO_INTERVAL_SET: {
		rtk_bt_mesh_df_echo_interval_state_setting_t *pei_state = (rtk_bt_mesh_df_echo_interval_state_setting_t *)p_cmd->param;
		ret = df_path_echo_interval_set(pei_state->master_key_index, pei_state->unicast_echo_interval, pei_state->multicast_echo_interval);
		break;
	}
	case RTK_BT_MESH_DF_PUBLISH_POLICY_SET: {
		rtk_bt_mesh_df_publish_policy_setting_t *policy_state = (rtk_bt_mesh_df_publish_policy_setting_t *)p_cmd->param;
		mesh_model_info_p model_info = mesh_model_info_get_by_model_id(policy_state->element_index, policy_state->model_id);
		if (model_info != NULL) {
			mesh_model_p model = mesh_model_get(model_info->element_index, model_info->model_index);
			mesh_model_pub_params_t pub_param = mesh_model_pub_params_get(model);
			pub_param.pub_policy = policy_state->policy;
			mesh_model_pub_params_set(model, pub_param);
			mesh_flash_store(MESH_FLASH_PARAMS_MODEL_PUB_PARAMS, model);
		}
		ret = RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS;
		break;
	}
	case RTK_BT_MESH_DF_PARAMETER_SET: {
		rtk_bt_mesh_directed_control_param_setting_t *param = (rtk_bt_mesh_directed_control_param_setting_t *)p_cmd->param;
		ret = rtk_stack_df_paramerter_setting(param);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif

#if defined(BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL) && BT_MESH_ENABLE_DIRECTED_FORWARDING_CLIENT_MODEL

uint8_t rtk_bt_mesh_directed_forwarding_client_model_indicate_event(uint32_t event_code, void *pdata, uint32_t len)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DIRECTED_FORWARDING_CLIENT_MODEL, event_code, len);
	if (len) {
		if (event_code == RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_STAT) {
			memcpy(p_evt->data, pdata, sizeof(rtk_bt_mesh_forwarding_table_entries_status_for_call_t));
			rtk_bt_mesh_forwarding_table_entries_status_for_call_t *data = (rtk_bt_mesh_forwarding_table_entries_status_for_call_t *) pdata;
			memcpy((uint8_t *)(p_evt->data) + sizeof(rtk_bt_mesh_forwarding_table_entries_status_for_call_t), data->data,
				   len - sizeof(rtk_bt_mesh_forwarding_table_entries_status_for_call_t));
		} else if (event_code == RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET_STAT) {
			memcpy(p_evt->data, pdata, sizeof(rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t));
			rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t *data = (rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t *) pdata;
			memcpy((uint8_t *)(p_evt->data) + sizeof(rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t), data->data,
				   len - sizeof(rtk_bt_mesh_forwarding_table_dependents_get_status_for_call_t));
		} else {
			memcpy(p_evt->data, pdata, len);
		}
	}
	return rtk_bt_evt_indicate(p_evt, NULL);
}

uint16_t bt_mesh_directed_forwarding_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_GET: {
		rtk_bt_mesh_df_client_act_directed_control_get_t *control_get = (rtk_bt_mesh_df_client_act_directed_control_get_t *)p_cmd->param;
		ret = directed_control_get(control_get->dst, control_get->net_key_index, control_get->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_SET: {
		rtk_bt_mesh_df_client_act_directed_control_set_t *control_set = (rtk_bt_mesh_df_client_act_directed_control_set_t *)p_cmd->param;
		directed_control_t df_state = {control_set->directed_forwarding, control_set->directed_relay, control_set->directed_proxy,
									   control_set->directed_proxy_use_directed_default, control_set->directed_friend
									  };
		ret = directed_control_set(control_set->dst, control_set->net_key_index, control_set->net_key_index_df, df_state);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_METRIC_GET: {
		rtk_bt_mesh_df_client_act_path_metric_get_t *metric_get = (rtk_bt_mesh_df_client_act_path_metric_get_t *)p_cmd->param;
		ret = path_metric_get(metric_get->dst, metric_get->net_key_index, metric_get->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_METRIC_SET: {
		rtk_bt_mesh_df_client_act_path_metric_set_t *metric_set = (rtk_bt_mesh_df_client_act_path_metric_set_t *)p_cmd->param;
		path_metric_t metric = {metric_set->metric_type, metric_set->lifetime, 0};
		ret = path_metric_set(metric_set->dst, metric_set->net_key_index, metric_set->net_key_index_df, metric);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_GET: {
		rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *cap_get = (rtk_bt_mesh_df_client_act_discovery_table_capabilities_get_t *)p_cmd->param;
		ret = discovery_table_capabilities_get(cap_get->dst, cap_get->net_key_index, cap_get->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DISCOVERY_TABLE_CAPABILITIES_SET: {
		rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *cap_set = (rtk_bt_mesh_df_client_act_discovery_table_capabilities_set_t *)p_cmd->param;
		ret = discovery_table_capabilities_set(cap_set->dst, cap_set->net_key_index, cap_set->net_key_index_df, cap_set->max_concurrent_init);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ADD: {
		rtk_bt_mesh_df_client_act_forwarding_table_add_t *fta_add = (rtk_bt_mesh_df_client_act_forwarding_table_add_t *)p_cmd->param;
		forwarding_table_param_t param;
		param.unicast_dst = fta_add->unicast_dst_flag;
		param.backward_path_valid = fta_add->backward_path_valid;
		param.prohibited = 0;
		param.path_origin = unicast_addr_range_transform(true, fta_add->origin_addr,
														 fta_add->origin_element_num);
		if (param.unicast_dst) {
			param.path_target = unicast_addr_range_transform(true, fta_add->target_addr,
															 fta_add->target_element_num);
		} else {
			param.multicast_dst = fta_add->target_addr;
		}
		param.bearer_toward_path_origin = fta_add->bearer_toward_origin;
		param.bearer_toward_path_target = fta_add->bearer_toward_target;
		ret = forwarding_table_add(fta_add->dst, fta_add->net_key_index, fta_add->net_key_index_df, &param);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DELETE: {
		rtk_bt_mesh_df_client_act_forwarding_table_delete_t *fta_del = (rtk_bt_mesh_df_client_act_forwarding_table_delete_t *)p_cmd->param;
		ret = forwarding_table_delete(fta_del->dst, fta_del->net_key_index, fta_del->net_key_index_df, fta_del->path_origin, fta_del->path_target);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_ADD: {
		rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *ftda_add = (rtk_bt_mesh_df_client_act_forwarding_table_dependents_add_t *)p_cmd->param;
		forwarding_table_dependents_add_param_t param;
		param.path_origin = ftda_add->path_origin;
		param.dst = ftda_add->path_target;
		param.dependent_origin_list_size = ftda_add->do_list_size;
		param.dependent_target_list_size = ftda_add->dt_list_size;
		param.addr_range_len = (ftda_add->do_list_size + ftda_add->dt_list_size) * sizeof(
								   mesh_addr_range_t);
		param.paddr_range = plt_zalloc(param.addr_range_len, RAM_TYPE_DATA_ON);

		mesh_addr_range_t dependent_addr_range, *paddr_range;
		uint8_t *pdata = param.paddr_range;

		for (uint8_t i = 0; i < param.dependent_origin_list_size; ++i) {
			dependent_addr_range = unicast_addr_range_transform(true, ftda_add->do_range[i].addr,
																ftda_add->do_range[i].element_num);
			paddr_range = (mesh_addr_range_t *)pdata;
			if (dependent_addr_range.len_present_access) {
				*paddr_range = dependent_addr_range;
				pdata += 3;
			} else {
				paddr_range->addr = dependent_addr_range.addr;
				pdata += 2;
			}
		}

		for (uint8_t i = 0; i < param.dependent_target_list_size; ++i) {
			dependent_addr_range = unicast_addr_range_transform(true, ftda_add->dt_range[i].addr,
																ftda_add->dt_range[i].element_num);
			paddr_range = (mesh_addr_range_t *)pdata;
			if (dependent_addr_range.len_present_access) {
				*paddr_range = dependent_addr_range;
				pdata += 3;
			} else {
				paddr_range->addr = dependent_addr_range.addr;
				pdata += 2;
			}
		}

		param.addr_range_len = (pdata - param.paddr_range);
		ret = forwarding_table_dependents_add(ftda_add->dst, ftda_add->net_key_index, ftda_add->net_key_index_df, &param);
		plt_free(param.paddr_range, RAM_TYPE_DATA_ON);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_DELETE: {
		rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *ftda_del = (rtk_bt_mesh_df_client_act_forwarding_table_dependents_delete_t *)p_cmd->param;
		forwarding_table_dependents_del_param_t param;
		param.path_origin = ftda_del->path_origin;
		param.dst = ftda_del->path_target;
		param.dependent_origin_list_size = ftda_del->do_list_size;
		param.dependent_target_list_size = ftda_del->dt_list_size;
		param.addr_range_len = (ftda_del->do_list_size + ftda_del->dt_list_size) * 16;
		param.paddrs = plt_malloc(param.addr_range_len, RAM_TYPE_DATA_ON);

		uint8_t *pdata = param.paddrs;
		for (uint8_t i = 0; i < param.dependent_origin_list_size; ++i) {
			LE_WORD2EXTRN(pdata, ftda_del->do_range[i] & MESH_UNICAST_ADDR_MASK);
			pdata += 2;
		}

		for (uint8_t i = 0; i < param.dependent_target_list_size; ++i) {
			LE_WORD2EXTRN(pdata, ftda_del->dt_range[i] & MESH_UNICAST_ADDR_MASK);
			pdata += 2;
		}

		param.addr_range_len = (pdata - param.paddrs);
		ret = forwarding_table_dependents_delete(ftda_del->dst, ftda_del->net_key_index, ftda_del->net_key_index_df, &param);
		plt_free(param.paddrs, RAM_TYPE_DATA_ON);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_DEPENDENTS_GET: {
		rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *ftdg = (rtk_bt_mesh_df_client_act_forwarding_table_dependents_get_t *)p_cmd->param;
		ret = forwarding_table_dependents_get(ftdg->dst, ftdg->net_key_index, ftdg->net_key_index_df,
											  ftdg->list_mask, ftdg->fixed_path_flag,
											  ftdg->start_index, ftdg->path_origin, ftdg->path_target, ftdg->update_id_exist, ftdg->update_id);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_COUNT_GET: {
		rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *ftecg = (rtk_bt_mesh_df_client_act_forwarding_table_entries_count_get_t *)p_cmd->param;
		ret = forwarding_table_entries_count_get(ftecg->dst, ftecg->net_key_index, ftecg->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_FORWARDING_TABLE_ENTRIES_GET: {
		rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *fteg = (rtk_bt_mesh_df_client_act_forwarding_table_entries_get_t *)p_cmd->param;
		uint8_t filter_mask = fteg->filter_mask;
		uint8_t data[6];
		uint16_t data_len = 0;
		if (filter_mask & FORWARDING_TABLE_FILTER_MASK_PATH_ORIGIN_MATCH) {
			LE_WORD2EXTRN(&data[data_len], fteg->path_origin);
			data_len += 2;
		}
		if (filter_mask & FORWARDING_TABLE_FILTER_MASK_DST_MATCH) {
			LE_WORD2EXTRN(&data[data_len], fteg->path_target);
			data_len += 2;
		}
		if (fteg->update_id_flag) {
			LE_WORD2EXTRN(&data[data_len], fteg->update_id);
			data_len += 2;
		}
		ret = forwarding_table_entries_get(fteg->dst, fteg->net_key_index, fteg->net_key_index_df, fteg->filter_mask,
										   fteg->start_index, data, data_len);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_WANTED_LANES_GET: {
		rtk_bt_mesh_df_client_act_wanted_lanes_get_t *wlg = (rtk_bt_mesh_df_client_act_wanted_lanes_get_t *)p_cmd->param;
		ret = wanted_lanes_get(wlg->dst, wlg->net_key_index, wlg->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_WANTED_LANES_SET: {
		rtk_bt_mesh_df_client_act_wanted_lanes_set_t *wls = (rtk_bt_mesh_df_client_act_wanted_lanes_set_t *)p_cmd->param;
		ret = wanted_lanes_set(wls->dst, wls->net_key_index, wls->net_key_index_df, wls->wanted_lanes);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_GET: {
		rtk_bt_mesh_df_client_act_two_way_path_get_t *twg = (rtk_bt_mesh_df_client_act_two_way_path_get_t *)p_cmd->param;
		ret = two_way_path_get(twg->dst, twg->net_key_index, twg->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_TWO_WAY_PATH_SET: {
		rtk_bt_mesh_df_client_act_two_way_path_set_t *tws = (rtk_bt_mesh_df_client_act_two_way_path_set_t *)p_cmd->param;
		ret = two_way_path_set(tws->dst, tws->net_key_index, tws->net_key_index_df, tws->two_way_path);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_GET: {
		rtk_bt_mesh_df_client_act_path_echo_interval_get_t *peig = (rtk_bt_mesh_df_client_act_path_echo_interval_get_t *)p_cmd->param;
		ret = path_echo_interval_get(peig->dst, peig->net_key_index, peig->net_key_index_df);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_ECHO_INTERVAL_SET: {
		rtk_bt_mesh_df_client_act_path_echo_interval_set_t *peis = (rtk_bt_mesh_df_client_act_path_echo_interval_set_t *)p_cmd->param;
		ret = path_echo_interval_set(peis->dst, peis->net_key_index, peis->net_key_index_df,
									 peis->unicast_echo_interval, peis->multicast_echo_interval);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_GET: {
		rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *dntg = (rtk_bt_mesh_df_client_act_directed_network_transmit_get_t *)p_cmd->param;
		ret = directed_network_transmit_get(dntg->dst, dntg->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_NETWORK_TRANSMIT_SET: {
		rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *dnts = (rtk_bt_mesh_df_client_act_directed_network_transmit_set_t *)p_cmd->param;
		ret = directed_network_transmit_set(dnts->dst, dnts->net_key_index, dnts->count, dnts->steps);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_GET: {
		rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *drtg = (rtk_bt_mesh_df_client_act_directed_relay_retransmit_get_t *)p_cmd->param;
		ret = directed_relay_retransmit_get(drtg->dst, drtg->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_RELAY_RETRANSMIT_SET: {
		rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *drts = (rtk_bt_mesh_df_client_act_directed_relay_retransmit_set_t *)p_cmd->param;
		ret = directed_relay_retransmit_set(drts->dst, drts->net_key_index, drts->count, drts->steps);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_GET: {
		rtk_bt_mesh_df_client_act_rssi_threshold_get_t *rssig = (rtk_bt_mesh_df_client_act_rssi_threshold_get_t *)p_cmd->param;
		ret = rssi_threshold_get(rssig->dst, rssig->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_RSSI_THRESHOLD_SET: {
		rtk_bt_mesh_df_client_act_rssi_threshold_set_t *rssis = (rtk_bt_mesh_df_client_act_rssi_threshold_set_t *)p_cmd->param;
		ret = rssi_threshold_set(rssis->dst, rssis->net_key_index, rssis->rssi_margin);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_PATHS_GET: {
		rtk_bt_mesh_df_client_act_directed_paths_get_t *dpg = (rtk_bt_mesh_df_client_act_directed_paths_get_t *)p_cmd->param;
		ret = directed_paths_get(dpg->dst, dpg->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_GET: {
		rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *policy_get = (rtk_bt_mesh_df_client_act_directed_publish_policy_get_t *)p_cmd->param;
		ret = directed_publish_policy_get(policy_get->dst, policy_get->net_key_index, policy_get->elem_addr, policy_get->model_id);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_PUBLISH_POLICY_SET: {
		rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *policy_set = (rtk_bt_mesh_df_client_act_directed_publish_policy_set_t *)p_cmd->param;
		directed_publish_policy_t policy;
		memcpy(&policy, &policy_set->policy, sizeof(directed_publish_policy_t));
		ret = directed_publish_policy_set(policy_set->dst, policy_set->net_key_index, policy, policy_set->elem_addr, policy_set->model_id);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_GET: {
		rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *timing_ctl_get = (rtk_bt_mesh_df_client_act_path_discovery_timing_control_get_t *)p_cmd->param;
		ret = path_discovery_timing_control_get(timing_ctl_get->dst, timing_ctl_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_PATH_DISCOVERY_TIMING_CONTROL_SET: {
		rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *timing_ctl_set = (rtk_bt_mesh_df_client_act_path_discovery_timing_control_set_t *)p_cmd->param;
		path_discovery_timing_control_t timing_ctl;
		timing_ctl.path_monitoring_interval = timing_ctl_set->monitor_interval;
		timing_ctl.path_discovery_retry_interval = timing_ctl_set->dis_retry_interval;
		timing_ctl.path_discovery_interval = timing_ctl_set->dis_interval;
		timing_ctl.lane_discovery_guard_interval = timing_ctl_set->lan_guard;
		timing_ctl.prohibited = 0;
		ret = path_discovery_timing_control_set(timing_ctl_set->dst, timing_ctl_set->net_key_index, timing_ctl);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_GET: {
		rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *ctl_net_get = (rtk_bt_mesh_df_client_act_directed_control_network_transmit_get_t *)
																						 p_cmd->param;
		ret = directed_control_network_transmit_get(ctl_net_get->dst, ctl_net_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_NETWORK_TRANSMIT_SET: {
		rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_net_set = (rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *)
																						 p_cmd->param;
		directed_control_network_transmit_t net_transmit;
		net_transmit.directed_control_network_transmit_count = ctl_net_set->count;
		net_transmit.directed_control_network_transmit_interval_steps = ctl_net_set->steps;
		ret = directed_control_network_transmit_set(ctl_net_set->dst, ctl_net_set->net_key_index, net_transmit);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_GET: {
		rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *ctl_relay_get = (rtk_bt_mesh_df_client_act_directed_control_relay_retransmit_get_t *)
																						   p_cmd->param;
		ret = directed_control_relay_retransmit_get(ctl_relay_get->dst, ctl_relay_get->net_key_index);
		break;
	}
	case RTK_BT_MESH_DF_MODEL_DIRECTED_CONTROL_RELAY_RETRANSMIT_SET: {
		rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *ctl_relay_set = (rtk_bt_mesh_df_client_act_directed_control_network_transmit_set_t *)
																						   p_cmd->param;
		directed_control_relay_retransmit_t relay_retransmit;
		relay_retransmit.directed_control_relay_retransmit_count = ctl_relay_set->count;
		relay_retransmit.directed_control_relay_retransmit_interval_steps = ctl_relay_set->steps;
		ret = directed_control_relay_retransmit_set(ctl_relay_set->dst, ctl_relay_set->net_key_index, relay_retransmit);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

bool directed_forwarding_client_init(void)
{
	/* register directed forwarding client model */
	return directed_forwarding_client_reg(0);
}

#endif

#endif
