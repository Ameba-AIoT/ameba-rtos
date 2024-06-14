#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_mesh_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>

uint16_t rtk_bt_mesh_stack_node_reset(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_NODE_RESET, NULL, 0);

	return ret;
}

uint16_t rtk_bt_mesh_stack_set_device_info_display(rkt_bt_mesh_stack_act_device_info_set_t *p_data)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_DEV_INFO_SWITCH, p_data, sizeof(rkt_bt_mesh_stack_act_device_info_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_user_list_info(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_USER_LIST_INFO, NULL, 0);

	return ret;
}

uint16_t rtk_bt_mesh_stack_send_one_shot_adv(rtk_bt_mesh_stack_act_send_adv_t *adv_param)
{
	uint16_t ret;

	if (adv_param->adv_data_len > RTK_BT_LE_MAX_ADV_DATA_LEN) {
		BT_LOGE("[%s] adv_len(%d) > RTK_BT_LE_MAX_ADV_DATA_LEN(%d), fail\r\n", __func__, adv_param->adv_data_len, RTK_BT_LE_MAX_ADV_DATA_LEN);
		return RTK_BT_FAIL;
	}
	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_SEND_ONE_SHOT_ADV, adv_param, sizeof(rtk_bt_mesh_stack_act_send_adv_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_set_random_value_for_authentication(rtk_bt_mesh_stack_act_set_random_value_t *value)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_SET_RANDOM_VALUE, value, sizeof(rtk_bt_mesh_stack_act_set_random_value_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_get_heartbeat_subscribe_result(rtk_bt_mesh_hb_sub_t *hb_sub)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_GET_HB_SUB_RESULT, hb_sub, sizeof(rtk_bt_mesh_hb_sub_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_set_model_subscribe(rtk_bt_mesh_set_model_subscribe_t *model_sub)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_SET_MODEL_SUB, model_sub, sizeof(rtk_bt_mesh_set_model_subscribe_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_retrans_param_set(rtk_bt_mesh_stack_set_retrans_param_t *param)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_SET_RETRANS_PARAM, param, sizeof(rtk_bt_mesh_stack_set_retrans_param_t));

	return ret;
}

#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
uint16_t rtk_bt_mesh_stack_fn_init(rtk_bt_mesh_stack_act_fn_init_t *friend_init)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_FN_INIT, friend_init, sizeof(rtk_bt_mesh_stack_act_fn_init_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_fn_deinit(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_FN_DEINIT, NULL, 0);

	return ret;
}
#endif // end of RTK_BLE_MESH_FN_SUPPORT

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
uint16_t rtk_bt_mesh_stack_provisioner_setting_init(rtk_bt_mesh_stack_act_provisioner_init_setting_t *init_setting)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PROVISIONER_INIT_SETTING, init_setting,
						  sizeof(rtk_bt_mesh_stack_act_provisioner_init_setting_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_pb_adv_con(rtk_bt_mesh_stack_act_pb_adv_con_t *pbadvcon)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PB_ADV_CON, pbadvcon, sizeof(rtk_bt_mesh_stack_act_pb_adv_con_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_pb_adv_discon(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PB_ADV_DISCON, NULL, 0);

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_pb_gatt_con(rtk_bt_mesh_stack_act_pb_gatt_con_t *pgatt_con)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PB_GATT_CON, pgatt_con, sizeof(rtk_bt_mesh_stack_act_pb_gatt_con_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_pb_gatt_discon(rtk_bt_mesh_stack_act_pb_gatt_discon_t *pgatt_discon)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PB_GATT_DISCON, pgatt_discon, sizeof(rtk_bt_mesh_stack_act_pb_gatt_discon_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_start_provisioning(rtk_bt_mesh_stack_act_start_prov_t *start_prov)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_START_PROV, start_prov, sizeof(rtk_bt_mesh_stack_act_start_prov_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_method_choose_for_provisioning(rtk_bt_mesh_stack_prov_start_t *method)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_METHOD_CHOOSE_FOR_PROV, method, sizeof(rtk_bt_mesh_stack_prov_start_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_service_discovery(rtk_bt_mesh_stack_act_prov_dis_t *prov_dis)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PROV_SERVICE_DISCOVERY, prov_dis, sizeof(rtk_bt_mesh_stack_act_prov_dis_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_prov_service_set_notify(rtk_bt_mesh_stack_act_prov_set_notify_t *prov_notify)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PROV_SERVICE_SET_NOTIFY, prov_notify, sizeof(rtk_bt_mesh_stack_act_prov_set_notify_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_proxy_service_discovery(rtk_bt_mesh_stack_act_proxy_dis_t *proxy_dis)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_DISCOVERY, proxy_dis, sizeof(rtk_bt_mesh_stack_act_proxy_dis_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_proxy_service_set_notify(rtk_bt_mesh_stack_act_proxy_set_notify_t *proxy_notify)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_SET_NOTIFY, proxy_notify, sizeof(rtk_bt_mesh_stack_act_proxy_set_notify_t));

	return ret;
}

#endif // end of RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
uint16_t rtk_bt_mesh_stack_lpn_init(rtk_bt_mesh_stack_act_lpn_init_t *lpn_init)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_LPN_INIT, lpn_init, sizeof(rtk_bt_mesh_stack_act_lpn_init_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_lpn_req(rtk_bt_mesh_stack_act_lpn_req_t *lpn_req)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_LPN_REQ, lpn_req, sizeof(rtk_bt_mesh_stack_act_lpn_req_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_lpn_sub(rtk_bt_mesh_stack_act_lpn_sub_t *lpn_sub)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_LPN_SUB, lpn_sub, sizeof(rtk_bt_mesh_stack_act_lpn_sub_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_lpn_clear(rtk_bt_mesh_stack_act_lpn_clear_t *lpn_clear)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_LPN_CLEAR, lpn_clear, sizeof(rtk_bt_mesh_stack_act_lpn_clear_t));

	return ret;
}

uint16_t rtk_bt_mesh_stack_lpn_deinit(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_ACT_LPN_DEINIT, NULL, 0);

	return ret;
}

#endif // end of RTK_BLE_MESH_LPN_SUPPORT

#endif // end of RTK_BLE_MESH_SUPPORT
