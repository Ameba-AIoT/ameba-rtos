/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <zephyr_stack_config.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <stack/host/conn_internal.h>
#include <stack/host/l2cap_internal.h>
#include <zephyr/settings/settings.h>
#include <settings_nvs.h>
#include <settings.h>
#include <ameba.h> /* to get ftl flash location */


#include <zephyr_msg.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <bt_api_config.h>
#include <zephyr_stack_internal.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <hci_core.h>
#include <zephyr_log.h>

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_iso.h>
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <rtk_bt_le_audio.h>
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <zephyr_stack_mesh_internal.h>
#include <zephyr_config_bt_mesh.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_config_client_model.h>
#include <rtk_bt_mesh_generic_onoff_model.h>
#include <rtk_bt_mesh_datatrans_model.h>
#include <rtk_bt_mesh_remote_prov_model.h>
#include <rtk_bt_mesh_light_model.h>
#include <rtk_bt_mesh_time_model.h>
#include <rtk_bt_mesh_scheduler_model.h>
#include <rtk_bt_mesh_scene_model.h>
#include <rtk_bt_mesh_generic_default_transition_time.h>
#include <rtk_bt_mesh_generic_model.h>
#include <rtk_bt_mesh_health_model.h>
#include <rtk_bt_mesh_sensor_model.h>
#endif

static bool stack_never_enabled = true;
static void *api_task_sem = NULL;
static void *api_task_hdl = NULL;
static void *api_task_msg_q = NULL;

extern int bt_uart_init(void);

static uint32_t bt_stack_app_profile_conf = 0;

bool bt_stack_profile_check(rtk_bt_profile_t profile)
{
	if (bt_stack_app_profile_conf & profile) {
		return true;
	}
	return false;
}

uint16_t bt_stack_act_handler(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	BT_LOGD("bt_stack_act_handler: group = %d, act = %d \r\n", p_cmd->group, p_cmd->act);
	switch (p_cmd->group) {
	case RTK_BT_LE_GP_GAP:
		BT_LOGD("RTK_BT_LE_GP_GAP group \r\n");
		bt_stack_le_gap_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_GATTS:
		BT_LOGD("RTK_BT_LE_GP_GATTS group \r\n");
		bt_stack_gatts_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_GATTC:
		BT_LOGD("RTK_BT_LE_GP_GATTC group \r\n");
		bt_stack_gattc_act_handle(p_cmd);
		break;
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	case RTK_BT_LE_GP_MESH_STACK:
		BT_LOGD("RTK_BT_LE_GP_MESH_STACK group");
		bt_mesh_stack_act_handle(p_cmd);
		break;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	case RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL group");
		bt_mesh_config_client_model_act_handle(p_cmd);
		break;
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL group");
		bt_mesh_generic_onoff_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL group");
		bt_mesh_light_lightness_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL group");
		bt_mesh_light_ctl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL group");
		bt_mesh_light_hsl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL group");
		bt_mesh_light_xyl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL group");
		bt_mesh_light_lc_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL group");
		bt_mesh_sensor_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL group");
		bt_mesh_scene_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL group");
		bt_mesh_time_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL group");
		bt_mesh_scheduler_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL group");
		bt_mesh_generic_default_transition_time_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL group");
		bt_mesh_generic_level_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL group");
		bt_mesh_generic_power_on_off_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL group");
		bt_mesh_generic_power_level_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL group");
		bt_mesh_generic_battery_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL group");
		bt_mesh_generic_location_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL group");
		bt_mesh_generic_property_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
	case RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL group");
		bt_mesh_remote_prov_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(CONFIG_BT_MESH_PRIV_BEACON_CLI) && CONFIG_BT_MESH_PRIV_BEACON_CLI
	case RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_PRIVATE_BEACON_CLIENT_MODEL group");
		bt_mesh_private_beacon_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
	case RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL group");
		bt_mesh_device_firmware_update_standlone_model_act_handle(p_cmd);
		break;
#endif
	case RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL group");
		bt_mesh_health_client_model_act_handle(p_cmd);
		break;
#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_user_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_admin_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_manu_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL group");
		bt_mesh_sensor_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL group");
		bt_mesh_sensor_setup_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL group");
		bt_mesh_scene_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL group");
		bt_mesh_scene_setup_server_model_act_handle(p_cmd);
		break;
#endif
	case RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL group");
		bt_mesh_health_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	case RTK_BT_LE_GP_MESH_DATATRANS_MODEL:
		BT_LOGD("RTK_BT_LE_GP_MESH_DATATRANS_MODEL group");
		bt_mesh_datatrans_model_act_handle(p_cmd);
		break;
#endif
#endif
	case RTK_BT_BR_GP_GAP:
		BT_LOGD("RTK_BT_BR_GP_GAP group \r\n");
		//bt_stack_br_gap_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_AVRCP:
		BT_LOGD("RTK_BT_BR_GP_AVRCP group \r\n");
		//bt_stack_avrcp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_A2DP:
		BT_LOGD("RTK_BT_BR_GP_A2DP group \r\n");
		//bt_stack_a2dp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_SPP:
		BT_LOGD("RTK_BT_BR_GP_SPP group \r\n");
		//bt_stack_spp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_HID:
		BT_LOGD("RTK_BT_BR_GP_HID group \r\n");
		//bt_stack_hid_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_HFP:
		BT_LOGD("RTK_BT_BR_GP_HFP group \r\n");
		//bt_stack_hfp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_SDP:
		BT_LOGD("RTK_BT_BR_GP_SDP group \r\n");
		//bt_stack_sdp_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_ISO:
		BT_LOGD("RTK_BT_LE_GP_ISO group \r\n");
		//bt_stack_le_iso_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_BAP:
	case RTK_BT_LE_GP_CAP:
	case RTK_BT_LE_GP_TMAP:
	case RTK_BT_LE_GP_GMAP:
		BT_LOGD("RTK_BT_LE_GP_AUDIO group \r\n");
		//bt_stack_le_audio_act_handle(p_cmd);
		break;
	case RTK_BT_COMMON_GP_GAP:
		BT_LOGD("RTK_BT_COMMON_GP_GAP group \r\n");
		bt_stack_gap_act_handle(p_cmd);
		break;
	default:
		BT_LOGE("bt_stack_le_act_handle:unknown group: %d \r\n", p_cmd->group);
		break;
	}

	return ret;
}

static void bt_stack_api_taskentry(void *ctx)
{
	(void)ctx;
	rtk_bt_cmd_t *pcmd;

	osif_sem_give(api_task_sem);

	osif_create_secure_context(BT_SECURE_STACK_SIZE);

	while (true) {
		if (true == osif_msg_recv(api_task_msg_q, &pcmd, BT_TIMEOUT_FOREVER)) {

			/* Check if need to exit task*/
			if (RTK_BT_API_TASK_EXIT == pcmd->group) {
				break;
			}
			bt_stack_act_handler(pcmd);
		}
	}

	BT_LOGD("[BT api task] bt api task exit\r\n");
	osif_sem_give(api_task_sem);
	osif_task_delete(NULL);
}

static uint16_t bt_stack_api_init(void)
{
	if (false == osif_sem_create(&api_task_sem, 0, 1)) {
		goto failed;
	}

	if (false == osif_msg_queue_create(&api_task_msg_q, API_TASK_MSG_QUEUE_SIZE, sizeof(void *))) {
		goto failed;
	}

	if (false == osif_task_create(&api_task_hdl, "bt_api_task", bt_stack_api_taskentry, NULL,
								  API_TASK_STACK_SIZE, API_TASK_PRIORITY)) {
		goto failed;
	}

	/* Wait until event task is running */
	if (false == osif_sem_take(api_task_sem, BT_TIMEOUT_FOREVER)) {
		goto failed;
	}

	return 0;

failed:
	if (api_task_hdl) {
		osif_task_delete(api_task_hdl);
	}
	if (api_task_sem) {
		osif_sem_delete(api_task_sem);
	}
	if (api_task_msg_q) {
		osif_msg_queue_delete(api_task_msg_q);
	}

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_api_deinit(void)
{
	rtk_bt_cmd_t exit_cmd = {
		.group = RTK_BT_API_TASK_EXIT,
	};
	rtk_bt_cmd_t *pcmd = &exit_cmd;

	/* indicate bt api task to kill itself */
	if (false == osif_msg_send(api_task_msg_q, &pcmd, 0)) {
		return RTK_BT_ERR_OS_OPERATION;
	}

	if (false == osif_sem_take(api_task_sem, BT_TIMEOUT_FOREVER)) {
		return RTK_BT_ERR_OS_OPERATION;
	}

	osif_sem_delete(api_task_sem);
	osif_msg_queue_delete(api_task_msg_q);
	api_task_sem = NULL;
	api_task_hdl = NULL;
	api_task_msg_q = NULL;

	return 0;
}

static uint16_t bt_stack_profile_init(void *app_conf)
{
	uint16_t ret = 0;
	rtk_bt_app_conf_t *papp_conf = (rtk_bt_app_conf_t *)app_conf;
	uint32_t app_profile_support = 0;

	if (papp_conf == NULL) {
		app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
		bt_host.cccd_not_check = false;
		bt_host.att_mtu = 23;
		bt_host.master_init_mtu_req = true;
		bt_host.slave_init_mtu_req = false;
	} else {
		app_profile_support = papp_conf->app_profile_support;
		bt_host.cccd_not_check = papp_conf->cccd_not_check;
		bt_host.att_mtu = MAX(23, papp_conf->mtu_size);
		bt_host.master_init_mtu_req = papp_conf->master_init_mtu_req;
		bt_host.slave_init_mtu_req = papp_conf->slave_init_mtu_req;
	}

	if (app_profile_support & RTK_BT_PROFILE_GATTS) {
		BT_LOGD("GATTS Profile init  \r\n");
		ret = bt_stack_gatts_init(app_conf);
		if (ret) {
			return ret;
		}
	}

	if (app_profile_support & RTK_BT_PROFILE_GATTC) {
		ret = bt_stack_gattc_init(app_conf);
		if (ret) {
			return ret;
		}
	}
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (app_profile_support & RTK_BT_PROFILE_MESH) {
		ret = bt_stack_mesh_enable(papp_conf);
		if (ret) {
			return ret;
		}
	}
#endif
	// if (app_profile_support & RTK_BT_PROFILE_AVRCP) {
	//  ret = bt_stack_avrcp_init();
	//  if (ret) {
	//      return ret;
	//  }
	// }
	// if (app_profile_support & RTK_BT_PROFILE_A2DP) {
	//  ret = bt_stack_a2dp_init();
	//  if (ret) {
	//      return ret;
	//  }
	// }

	bt_stack_app_profile_conf = app_profile_support;

	return 0;
}

static uint16_t bt_stack_profile_deinit(void)
{
	uint32_t profile_conf = bt_stack_app_profile_conf;
	if (profile_conf & RTK_BT_PROFILE_GATTS) {
		bt_stack_gatts_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_GATTC) {
		bt_stack_gattc_deinit();
	}
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (profile_conf & RTK_BT_PROFILE_MESH) {
		bt_stack_mesh_deinit();
	}
#endif

	bt_stack_app_profile_conf = 0;

	return 0;
}

static uint16_t zephyr_bt_enable(bool user_def_service)
{
	int b_resource_init = 0;
	int b_stack_init = 0;

	zephyr_builtin_gatt_svc_set(!user_def_service);

	b_resource_init = zephyr_res_alloc();
	if (b_resource_init) {
		BT_LOGE("b_resource_init failed (err %d)\r\n", b_resource_init);
		goto failed;
	}

	b_stack_init = bt_enable(NULL);
	if (b_stack_init) {
		BT_LOGE("b_stack_init failed (err %d)\r\n", b_stack_init);
		goto failed;
	}

	return 0;

failed:
	if (b_stack_init) {
		bt_disable();
		zephyr_internal_res_free();
	}

	if (b_resource_init) {
		zephyr_res_free();
	}

	return RTK_BT_FAIL;
}

static void conn_cleanup_check(struct bt_conn *conn, void *data)
{
	if (atomic_get(&conn->ref) > 1) {
		*((bool *)data) = false;
	}
}

void wait_bt_conn_cleanup(void)
{
	bool conn_cleanup_done = false;

	while (!conn_cleanup_done) {
		conn_cleanup_done = true;
		bt_conn_foreach(BT_CONN_TYPE_ALL, conn_cleanup_check, &conn_cleanup_done);
		osif_delay(5);
	}
}

static uint16_t zephyr_bt_disable(void)
{
	bt_disable();
	zephyr_internal_res_free();
	zephyr_res_free();

	return 0;
}

static uint16_t bt_stack_init(void *app_config)
{
	int b_uart_init = 0;
	int b_zephyr_init = 0;
	int b_le_gap_init = 0;
	// int b_br_gap_init = 0;
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	int b_mesh_init = 0;
#endif

	zephyr_btlog_init();

	b_uart_init = bt_uart_init();
	if (b_uart_init) {
		BT_LOGE("bt_uart_init failed (err %d)\r\n", b_uart_init);
		goto failed;
	}

	b_zephyr_init = zephyr_bt_enable(app_config && ((rtk_bt_app_conf_t *)app_config)->user_def_service);

	if (b_zephyr_init) {
		BT_LOGE("b_zephyr_init failed (err %d)\r\n", b_zephyr_init);
		goto failed;
	}

	b_le_gap_init = bt_stack_le_gap_init(app_config);
	if (b_le_gap_init) {
		BT_LOGE("b_le_gap_init failed (err %d)\r\n", b_le_gap_init);
		goto failed;
	}

	// b_br_gap_init = bt_stack_br_gap_init();
	// if (b_br_gap_init) {
	//  BT_LOGE("b_br_gap_init failed (err %d)\r\n", b_br_gap_init);
	//  goto failed;
	// }

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (app_config && ((rtk_bt_app_conf_t *)app_config)->app_profile_support & RTK_BT_PROFILE_MESH) {
		b_mesh_init = bt_stack_mesh_init(app_config);
		if (b_mesh_init) {
			BT_LOGE("b_mesh_init failed (err %d)\r\n", b_mesh_init);
			goto failed;
		}
	}
#endif

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		settings_load();
	}

	return 0;

failed:
	if (b_zephyr_init) {
		zephyr_bt_disable();
	}

	zephyr_btlog_deinit();

	return RTK_BT_FAIL;
}

static uint16_t bt_stack_deinit(void)
{
	zephyr_bt_disable();

	zephyr_btlog_deinit();

	return 0;
}

uint16_t bt_stack_enable(void *app_conf)
{
	uint16_t ret = 0;

	//step 1 initialize APP task
	ret = bt_stack_api_init();
	if (ret) {
		return ret;
	}

	//step 2 initialize GAP
	ret = bt_stack_init(app_conf);
	if (ret) {
		return ret;
	}

	//step3 initialize profile
	ret = bt_stack_profile_init(app_conf);
	if (ret) {
		return ret;
	}

	if (is_stack_never_enabled()) {
		stack_never_enabled = false;
	}

	return 0;
}

uint16_t bt_stack_disable(void)
{
	uint16_t ret = 0;

	ret = bt_stack_api_deinit();
	if (ret) {
		return ret;
	}

	ret = bt_stack_deinit();
	if (ret) {
		return ret;
	}

	ret = bt_stack_profile_deinit();
	if (ret) {
		return ret;
	}

	return 0;
}

uint16_t bt_stack_mp_start(void)
{
	return 0;
}

uint16_t bt_stack_api_send(void *pcmd)
{

	if (!pcmd) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	if (false == osif_msg_send(api_task_msg_q, &pcmd, 0)) {
		return RTK_BT_ERR_OS_OPERATION;
	}

	return RTK_BT_OK;
}

bool is_stack_never_enabled(void)
{
	return stack_never_enabled;
}

void zephyr_get_flash_info(uint32_t *offset, uint16_t *sec_cnt)
{
	u32 ftl_start_addr, ftl_end_addr;

	flash_get_layout_info(FTL, &ftl_start_addr, &ftl_end_addr);
	*offset = (uint32_t)(ftl_start_addr - SPI_FLASH_BASE);
	*sec_cnt = (uint16_t)((ftl_end_addr - ftl_start_addr + 1) / PAGE_SIZE_4K);
}

uint16_t zephyr_err_to_rtk(int err)
{
	if (err > 0 && RTK_BT_ERR_HCI_GROUP == (err & ~(0xFF))) {
		return (uint16_t)err;
	} else if (0 == err) {
		return 0;
	} else {
		return RTK_BT_ERR_LOWER_STACK_API;
	}
}