#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_device.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#include <rtk_bt_mesh_health_model.h>
#include <rtk_bt_mesh_def.h>

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
uint16_t rtk_bt_mesh_health_fault_get(rtk_bt_mesh_health_fault_get_t *health_fault_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_GET, health_fault_get,
						  sizeof(rtk_bt_mesh_health_fault_get_t));

	BT_LOGA("ret %d \r\n", ret);
	return ret;
}

uint16_t rtk_bt_mesh_health_fault_clear(rtk_bt_mesh_health_fault_clear_t *health_fault_clear)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_CLEAR, health_fault_clear,
						  sizeof(rtk_bt_mesh_health_fault_clear_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_fault_test(rtk_bt_mesh_health_fault_test_t *health_fault_test)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_TEST, health_fault_test,
						  sizeof(rtk_bt_mesh_health_fault_test_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_period_get(rtk_bt_mesh_health_period_get_t *health_period_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_GET, health_period_get,
						  sizeof(rtk_bt_mesh_health_period_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_period_set(rtk_bt_mesh_health_period_set_t *health_period_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_SET, health_period_set,
						  sizeof(rtk_bt_mesh_health_period_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_attn_get(rtk_bt_mesh_health_attn_get_t *health_attn_get)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_GET, health_attn_get,
						  sizeof(rtk_bt_mesh_health_attn_get_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_attn_set(rtk_bt_mesh_health_attn_set_t *health_attn_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_SET, health_attn_set,
						  sizeof(rtk_bt_mesh_health_attn_set_t));

	return ret;
}
#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
uint16_t rtk_bt_mesh_health_server_fault_register(rtk_bt_mesh_health_server_fault_register_act_t *health_fault_register)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_REGISTER, health_fault_register,
						  sizeof(rtk_bt_mesh_health_server_fault_register_act_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_server_fault_clear(rtk_bt_mesh_health_server_fault_clear_act_t *health_fault_clear)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR, health_fault_clear,
						  sizeof(rtk_bt_mesh_health_server_fault_clear_act_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_server_fault_clear_all(void)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR_ALL, NULL,
						  0);

	return ret;
}

uint16_t rtk_bt_mesh_health_server_fault_count(rtk_bt_mesh_health_server_count_fault_act_t *fault_count)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_COUNT, fault_count,
						  sizeof(rtk_bt_mesh_health_server_count_fault_act_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_server_fault_check(rtk_bt_mesh_health_server_check_fault_act_t *health_fault_check)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CHECK_FAULT_IS_SET, health_fault_check,
						  sizeof(rtk_bt_mesh_health_server_check_fault_act_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_server_set_tests(rtk_bt_mesh_health_server_test_set_t *test_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_TEST_SET, test_set,
						  sizeof(rtk_bt_mesh_health_server_test_set_t));

	return ret;
}

uint16_t rtk_bt_mesh_health_server_set_company_id(rtk_bt_mesh_health_server_company_id_set_t *company_id_set)
{
	uint16_t ret;

	ret = rtk_bt_send_cmd(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_COMPANY_ID_SET, company_id_set,
						  sizeof(rtk_bt_mesh_health_server_company_id_set_t));

	return ret;
}

#endif // RTK_BLE_MESH_DEVICE_SUPPORT

#endif // end of RTK_BLE_MESH_SUPPORT