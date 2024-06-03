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
#include <rtk_stack_config.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_stack_internal.h>
#include <gap.h>
#include <bte.h>
#include <gap_msg.h>
#include <gap_le.h>
#include <app_msg.h>
#include <gap_config.h>
#include <gap_vendor.h>
#include <trace_app.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_iso.h>
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#include <rtk_bt_le_audio.h>
#include <ble_audio.h>
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <mesh_common.h>
#include <rtk_stack_mesh_internal.h>
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
#endif
#if (defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB) || (defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT)
#include <sysm.h>
#endif
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
#include <ble_mgr.h>
#endif
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#include <remote.h>
#include <btm.h>
#endif
static void *api_task_sem = NULL;
static void *api_task_hdl = NULL;
static void *api_task_io_msg_q = NULL;
static void *api_task_evt_msg_q = NULL;
static bool api_task_running = false;
static uint32_t api_task_msg_num = 0;
static struct list_head g_cmd_pending_list;

static uint16_t bt_stack_act_handler(rtk_bt_cmd_t *p_cmd);

static void bt_stack_api_taskentry(void *ctx)
{
	(void)ctx;
	uint8_t event;
	T_IO_MSG io_msg;

	osif_sem_give(api_task_sem);

#if defined(configENABLE_TRUSTZONE) && configENABLE_TRUSTZONE
	osif_create_secure_context(configMINIMAL_SECURE_STACK_SIZE + 256);
#endif

	while (true) {
		if (true == osif_msg_recv(api_task_evt_msg_q, &event, BT_TIMEOUT_FOREVER)) {
			if (EVENT_GROUP_IO == EVENT_GROUP(event)) {
				if (EVENT_IO_TO_APP == event) {
					if (true == osif_msg_recv(api_task_io_msg_q, &io_msg, BT_TIMEOUT_NONE)) {
						switch (io_msg.type) {
						case IO_MSG_TYPE_API_SYS_CALL:
							/* Check if need to exit task*/
							if (io_msg.subtype == RTK_BT_API_TASK_EXIT) {
								goto out;
							}
							bt_stack_act_handler((rtk_bt_cmd_t *)io_msg.u.buf);
							break;

						case IO_MSG_TYPE_BT_STATUS:
							bt_stack_le_gap_handle_io_msg(io_msg.subtype, (void *)&io_msg.u.param);
							break;
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
						case IO_MSG_TYPE_LE_MGR:
							bt_stack_le_iso_handle_io_msg(&io_msg);
							break;
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
						case IO_MSG_TYPE_LE_AUDIO:
							ble_audio_handle_msg(&io_msg);
							break;
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
						case IO_MSG_TYPE_LE_MESH:
							ble_mesh_handle_io_msg(&io_msg);
							break;
#endif
						default:
							break;
						}
					}
				}
			} else if (EVENT_GROUP_FRAMEWORK == EVENT_GROUP(event)) {
				rtk_stack_framework_event_handler(event);
			}
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
			else if (EVENT_MESH == event) {
				mesh_inner_msg_handle(event);
			}
#endif
			else {
				gap_handle_msg(event);
			}
		}
	}

out:
	API_PRINT("[BT api task] bt api task exit\r\n");
	osif_sem_give(api_task_sem);
	osif_task_delete(NULL);
}

static bool bt_stack_framework_init(void)
{
#if (defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB) || (defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT)
	bool b_sys;
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	bool b_remote = false;
	bool b_bt;
#endif
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	BLE_MGR_PARAMS param = {0};
#endif

	/* System Manager */
	b_sys = sys_mgr_init(api_task_evt_msg_q);
	if (!b_sys) {
		goto fail;
	}

#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	/* Initialize remote control manager*/
	b_remote = remote_mgr_init(REMOTE_SESSION_ROLE_SINGLE);
	if (!b_remote) {
		goto fail;
	}

	/* Bluetooth Manager */
	b_bt = bt_mgr_init();
	if (!b_bt) {
		goto fail;
	}
#endif

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
#if (defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT) && \
    (defined(F_BT_LE_5_0_AE_ADV_SUPPORT) && F_BT_LE_5_0_AE_ADV_SUPPORT) && \
    (defined(RTK_BLE_MGR_LIB_EADV) && RTK_BLE_MGR_LIB_EADV)
	param.ble_ext_adv.enable = true;
	param.ble_ext_adv.adv_num = GAP_MAX_EXT_ADV_SETS;
#endif
	ble_mgr_init(&param);
#endif

	return true;

fail:
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	if (b_bt) {
		bt_mgr_deinit();
	}
	if (b_remote) {
		remote_mgr_deinit();
	}
#endif
	if (b_sys) {
		sys_mgr_deinit();
	}

	return false;
#else
	return true;
#endif
}

static void bt_stack_framework_deinit(void)
{
#if (defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB) || (defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT)
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	/* bt mgr deinit */
	bt_mgr_deinit();
	/* remote mgr deinit */
	remote_mgr_deinit();
#endif

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	ble_mgr_deinit();
#endif

	sys_mgr_deinit();
#endif
}

static uint16_t bt_stack_init(void *app_config)
{
	bool b_trace_init_ret = false;
	bool b_bte_init_ret = false;
	bool b_framework_init = false;
	uint16_t b_bt_ble_gap_init_ret = 0;
	rtk_bt_app_conf_t *papp_conf = (rtk_bt_app_conf_t *)app_config;
	rtk_bt_app_conf_t default_conf = {0};

	//set default conf
	if (papp_conf != NULL) {
		default_conf.mtu_size = papp_conf->mtu_size;
		default_conf.master_init_mtu_req = papp_conf->master_init_mtu_req;
		default_conf.slave_init_mtu_req = papp_conf->slave_init_mtu_req;
		default_conf.prefer_all_phy = papp_conf->prefer_all_phy;
		default_conf.prefer_tx_phy = papp_conf->prefer_tx_phy;
		default_conf.prefer_rx_phy = papp_conf->prefer_rx_phy;
		default_conf.max_tx_octets = papp_conf->max_tx_octets;
		default_conf.max_tx_time = papp_conf->max_tx_time;
#if defined(RTK_BLE_SET_TX_QUEUE_NUM) && RTK_BLE_SET_TX_QUEUE_NUM
		if (papp_conf->max_stack_tx_pending_num != 0) {
			default_conf.max_stack_tx_pending_num = papp_conf->max_stack_tx_pending_num;
		} else {
			default_conf.max_stack_tx_pending_num = RTK_BT_GATT_DEFAULT_CREDITS;
		}
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		memcpy(default_conf.irk, papp_conf->irk, RTK_BT_LE_GAP_IRK_LEN);
#endif
	} else {
		default_conf.mtu_size = 180;
		default_conf.master_init_mtu_req = true;
		default_conf.slave_init_mtu_req = false;
		default_conf.prefer_all_phy = 0;
		default_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		default_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		default_conf.max_tx_octets = 0x40;
		default_conf.max_tx_time = 0x200;
#if defined(RTK_BLE_SET_TX_QUEUE_NUM) && RTK_BLE_SET_TX_QUEUE_NUM
		default_conf.max_stack_tx_pending_num = RTK_BT_GATT_DEFAULT_CREDITS;
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		memset(default_conf.irk, 0, RTK_BT_LE_GAP_IRK_LEN);
#endif
	}

	//Trace uart init
	b_trace_init_ret = bt_trace_init();
	if (false == b_trace_init_ret) {
		goto failed;
	}

	/* gap_config_xxx() shall be before bte_init() */
	gap_config_max_le_link_num(RTK_BLE_GAP_MAX_LINKS);
	gap_config_max_le_paired_device(RTK_BLE_GAP_MAX_LINKS);
	gap_config_max_mtu_size(default_conf.mtu_size);
	gap_config_deinit_flow(2);
#if (defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT)
	gap_config_ccc_bits_count(GAP_MAX_CCC_BITS_CNT, GAP_MAX_CCC_BITS_CNT);
#else
	if (papp_conf && true == papp_conf->cccd_not_save) {
		gap_config_ccc_bits_count(16, 0);
	}
#endif
#if defined(RTK_BLE_SET_TX_QUEUE_NUM) && RTK_BLE_SET_TX_QUEUE_NUM
	if (false == gap_config_credits_num(default_conf.max_stack_tx_pending_num)) {
		BT_LOGE("%s: gap_config_credits_num fail(%d)\r\n", __func__, default_conf.max_stack_tx_pending_num);
	}
#endif

#if defined(F_BT_5_2_EATT_SUPPORT) && F_BT_5_2_EATT_SUPPORT
#if defined(RTK_BT_5_2_EATT_SUPPORT) && RTK_BT_5_2_EATT_SUPPORT
	gap_config_eatt(true);
	/* If BREDR uses EATT at the same, enlarge the paramter to include gatt channel of BREDR */
	gap_config_gatt_chann_num(RTK_BLE_GAP_MAX_LINKS + GAP_LE_MAX_ECFC_CHANN_NUM);
#else
	gap_config_eatt(false); /*clear GATTS_SERVER_SUPPORTED_FEATURES_EATT_BIT of GATT_UUID_CHAR_SERVER_SUPPORTED_FEATURES*/
#endif
#endif

#if (defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT) && (defined(F_BT_5_2_L2C_ECFC_SUPPORT) && F_BT_5_2_L2C_ECFC_SUPPORT)
	gap_config_le_l2c_chann_num(GAP_LE_MAX_ECFC_CHANN_NUM);
	gap_config_le_sec_entry_num(GAP_MAX_ECFC_PROTOCAL_NUM);
#endif

	//BT Stack init
	b_bte_init_ret = bte_init();
	if (false == b_bte_init_ret) {
		goto failed;
	}

	b_framework_init = bt_stack_framework_init();
	if (false == b_framework_init) {
		goto failed;
	}

	//GAP common initialization
	if (bt_stack_gap_init()) {
		goto failed;
	}

	//LE GAP initialization
	b_bt_ble_gap_init_ret = bt_stack_le_gap_init(&default_conf);
	if (b_bt_ble_gap_init_ret) {
		goto failed;
	}

	//BR/EDR GAP initialization
	if (RTK_BT_FAIL == bt_stack_br_gap_init(api_task_evt_msg_q)) {
		goto failed;
	}

	return 0;

failed:
	if (0 == b_bt_ble_gap_init_ret) {
		bt_stack_le_gap_deinit();
	}

	if (b_framework_init) {
		bt_stack_framework_deinit();
	}

	if (b_bte_init_ret) {
		bte_deinit();
		bte_deinit_free();
	}

	if (b_trace_init_ret) {
		bt_trace_deinit();
	}

	return RTK_BT_FAIL;
}


static uint16_t bt_stack_deinit(void)
{
	T_GAP_DEV_STATE state;
	uint16_t ret = 0;

	le_get_gap_param(GAP_PARAM_DEV_STATE, &state);
	if (state.gap_init_state == GAP_INIT_STATE_STACK_READY) {
		bt_stack_le_gap_deinit();
		bte_deinit();
	}
	ret = bt_stack_br_gap_deinit();

	return ret;
}

static uint32_t bt_stack_app_profile_conf = 0;

bool bt_stack_profile_check(rtk_bt_profile_t profile)
{
	if (bt_stack_app_profile_conf & profile) {
		return true;
	}
	return false;
}

static uint16_t bt_stack_profile_init(void *app_conf)
{
	uint16_t ret = 0;
	rtk_bt_app_conf_t *papp_conf = (rtk_bt_app_conf_t *)app_conf;
	uint32_t  app_profile_support = 0;

	if (papp_conf == NULL) {
		app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
	} else {
		app_profile_support = papp_conf->app_profile_support;
	}

	if (app_profile_support & RTK_BT_PROFILE_GATTS) {
		API_PRINT("GATTS Profile init  \r\n");
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
		ret = bt_stack_mesh_init(papp_conf);
		if (ret) {
			return ret;
		}
	}
#endif
	if (app_profile_support & RTK_BT_PROFILE_AVRCP) {
		ret = bt_stack_avrcp_init(papp_conf->a2dp_role);
		if (ret) {
			return ret;
		}
	}
	if (app_profile_support & RTK_BT_PROFILE_A2DP) {
		ret = bt_stack_a2dp_init(papp_conf->a2dp_role);
		if (ret) {
			return ret;
		}
	}
	if (app_profile_support & RTK_BT_PROFILE_SPP) {
		ret = bt_stack_spp_init(papp_conf->spp_role);
		if (ret) {
			return ret;
		}
	}
	if (app_profile_support & RTK_BT_PROFILE_HID) {
		ret = bt_stack_hid_init(papp_conf->hid_role);
		if (ret) {
			return ret;
		}
	}
	if (app_profile_support & RTK_BT_PROFILE_SDP) {
		ret = bt_stack_sdp_init();
		if (ret) {
			return ret;
		}
	}
	if (app_profile_support & RTK_BT_PROFILE_HFP) {
		ret = bt_stack_hfp_init(papp_conf->hfp_role);
		if (ret) {
			return ret;
		}
	}
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	if (app_profile_support & RTK_BT_PROFILE_LEAUDIO) {
		ret = bt_stack_le_audio_init(papp_conf, api_task_io_msg_q, api_task_evt_msg_q);
		if (ret) {
			return ret;
		}
	}
#endif
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
	if (profile_conf & RTK_BT_PROFILE_AVRCP) {
		bt_stack_avrcp_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_A2DP) {
		bt_stack_a2dp_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_SPP) {
		bt_stack_spp_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_HID) {
		bt_stack_hid_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_HFP) {
		bt_stack_hfp_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_SDP) {
		bt_stack_sdp_deinit();
	}
	if (profile_conf & RTK_BT_PROFILE_LEAUDIO) {
		bt_stack_le_audio_deinit();
	}

	bt_stack_app_profile_conf = 0;

	return 0;
}

static uint16_t bt_stack_api_init(void)
{
	bt_stack_pending_cmd_init();

	if (false == osif_sem_create(&api_task_sem, 0, 1)) {
		goto failed;
	}

	if (false == osif_msg_queue_create(&api_task_io_msg_q, API_TASK_IO_MSG_QUEUE_SIZE, sizeof(T_IO_MSG))) {
		goto failed;
	}

	if (false == osif_msg_queue_create(&api_task_evt_msg_q, API_TASK_EVT_MSG_QUEUE, sizeof(uint8_t))) {
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

	api_task_running = true;
	return 0;

failed:
	if (api_task_hdl) {
		osif_task_delete(api_task_hdl);
	}
	if (api_task_sem) {
		osif_sem_delete(api_task_sem);
	}
	if (api_task_io_msg_q) {
		osif_msg_queue_delete(api_task_io_msg_q);
	}
	if (api_task_evt_msg_q) {
		osif_msg_queue_delete(api_task_evt_msg_q);
	}

	return RTK_BT_FAIL;
}

static void bt_stack_api_stop(void)
{
	api_task_running = false;

	/* Waiting bt_stack_msg_send() on other tasks interrupted by deinit task to complete */
	while (api_task_msg_num) {
		osif_delay(5);
	}
}

static uint16_t bt_stack_api_deinit(void)
{
	uint8_t event = EVENT_IO_TO_APP;
	T_IO_MSG io_msg = {
		.type = IO_MSG_TYPE_API_SYS_CALL,
		.subtype = RTK_BT_API_TASK_EXIT,
	};

	/* indicate bt api task to kill itself */
	if (!osif_msg_send(api_task_io_msg_q, &io_msg, 0) || !osif_msg_send(api_task_evt_msg_q, &event, 0)) {
		return RTK_BT_ERR_OS_OPERATION;
	}

	if (false == osif_sem_take(api_task_sem, BT_TIMEOUT_FOREVER)) {
		return RTK_BT_ERR_OS_OPERATION;
	}

	osif_sem_delete(api_task_sem);
	osif_msg_queue_delete(api_task_io_msg_q);
	osif_msg_queue_delete(api_task_evt_msg_q);
	api_task_sem = NULL;
	api_task_hdl = NULL;
	api_task_io_msg_q = NULL;
	api_task_evt_msg_q = NULL;

	bt_stack_pending_cmd_deinit();

	return 0;
}

uint16_t bt_stack_act_handler(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = 0;
	API_PRINT("bt_stack_act_handler: group = %d, act = %d \r\n", p_cmd->group, p_cmd->act);
	switch (p_cmd->group) {
	case RTK_BT_LE_GP_GAP:
		API_PRINT("RTK_BT_LE_GP_GAP group \r\n");
		bt_stack_le_gap_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_GATTS:
		API_PRINT("RTK_BT_LE_GP_GATTS group \r\n");
		bt_stack_gatts_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_GATTC:
		API_PRINT("RTK_BT_LE_GP_GATTC group \r\n");
		bt_stack_gattc_act_handle(p_cmd);
		break;
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	case RTK_BT_LE_GP_MESH_STACK:
		API_PRINT("RTK_BT_LE_GP_MESH_STACK group");
		bt_mesh_stack_act_handle(p_cmd);
		break;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	case RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL group");
		bt_mesh_config_client_model_act_handle(p_cmd);
		break;
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL group");
		bt_mesh_generic_onoff_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL group");
		bt_mesh_remote_prov_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL group");
		bt_mesh_light_lightness_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL group");
		bt_mesh_light_ctl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL group");
		bt_mesh_light_hsl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL group");
		bt_mesh_light_xyl_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL group");
		bt_mesh_light_lc_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL group");
		bt_mesh_time_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL group");
		bt_mesh_scheduler_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL group");
		bt_mesh_scene_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL group");
		bt_mesh_generic_default_transition_time_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL group");
		bt_mesh_generic_level_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL group");
		bt_mesh_generic_power_on_off_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL group");
		bt_mesh_generic_power_level_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL group");
		bt_mesh_generic_battery_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL group");
		bt_mesh_generic_location_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL group");
		bt_mesh_generic_property_client_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL group");
		bt_mesh_sensor_client_model_act_handle(p_cmd);
		break;
#endif
	case RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL group");
		bt_mesh_health_client_model_act_handle(p_cmd);
		break;
#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_user_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_admin_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL group");
		bt_mesh_generic_manu_property_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL group");
		bt_mesh_scene_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL group");
		bt_mesh_scene_setup_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL group");
		bt_mesh_sensor_server_model_act_handle(p_cmd);
		break;
#endif
#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
	case RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL group");
		bt_mesh_sensor_setup_server_model_act_handle(p_cmd);
		break;
#endif
	case RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL group");
		bt_mesh_health_server_model_act_handle(p_cmd);
		break;
#endif // end of RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	case RTK_BT_LE_GP_MESH_DATATRANS_MODEL:
		API_PRINT("RTK_BT_LE_GP_MESH_DATATRANS_MODEL group");
		bt_mesh_datatrans_model_act_handle(p_cmd);
		break;
#endif
#endif  // RTK_BLE_MESH_SUPPORT
	case RTK_BT_BR_GP_GAP:
		API_PRINT("RTK_BT_BR_GP_GAP group \r\n");
		bt_stack_br_gap_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_AVRCP:
		API_PRINT("RTK_BT_BR_GP_AVRCP group \r\n");
		bt_stack_avrcp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_A2DP:
		API_PRINT("RTK_BT_BR_GP_A2DP group \r\n");
		bt_stack_a2dp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_SPP:
		API_PRINT("RTK_BT_BR_GP_SPP group \r\n");
		bt_stack_spp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_HID:
		API_PRINT("RTK_BT_BR_GP_HID group \r\n");
		bt_stack_hid_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_HFP:
		API_PRINT("RTK_BT_BR_GP_HFP group \r\n");
		bt_stack_hfp_act_handle(p_cmd);
		break;
	case RTK_BT_BR_GP_SDP:
		API_PRINT("RTK_BT_BR_GP_SDP group \r\n");
		bt_stack_sdp_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_ISO:
		API_PRINT("RTK_BT_LE_GP_ISO group \r\n");
		bt_stack_le_iso_act_handle(p_cmd);
		break;
	case RTK_BT_LE_GP_AUDIO:
		API_PRINT("RTK_BT_LE_GP_AUDIO group \r\n");
		bt_stack_le_audio_act_handle(p_cmd);
		break;
	case RTK_BT_COMMON_GP_GAP:
		API_PRINT("RTK_BT_COMMON_GP_GAP group \r\n");
		bt_stack_gap_act_handle(p_cmd);
		break;
	default:
		API_PRINT("bt_stack_le_act_handle:unknown group: %d \r\n", p_cmd->group);
		break;
	}

	return ret;
}

static bool bt_stack_startup(void *app_conf)
{
	(void)app_conf;
	if (false == gap_start_bt_stack(api_task_evt_msg_q, api_task_io_msg_q, API_TASK_GAP_MSG_QUEUE_SIZE)) {
		return false;
	}
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (((rtk_bt_app_conf_t *)app_conf)->app_profile_support & RTK_BT_PROFILE_MESH) {
		mesh_start(EVENT_MESH, EVENT_IO_TO_APP, api_task_evt_msg_q, api_task_io_msg_q);
	}
#endif
	return true;
}

#if defined(RTK_BT_STACK_LOG_CONFIG) && RTK_BT_STACK_LOG_CONFIG
static void bt_stack_log_config(void)
{
	log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, TRACE_LEVEL_WARN, 0);
	log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, TRACE_LEVEL_INFO, 0);
	log_module_bitmap_trace_set(0xFFFFFFFFFFFFFFFF, TRACE_LEVEL_TRACE, 0);
}
#endif

static void bt_stack_post_config(void)
{
	gap_vendor_le_set_host_feature(16, 1);
#if defined(RTK_BT_STACK_LOG_CONFIG) && RTK_BT_STACK_LOG_CONFIG
	bt_stack_log_config();
#endif
}

uint16_t bt_stack_enable(void *app_conf)
{
	uint16_t ret = 0;

	//step 1 initialize APP task
	ret = bt_stack_api_init();
	if (ret) {
		return ret;
	}
	//step 2 initialize GAP and other common config
	ret = bt_stack_init(app_conf);
	if (ret) {
		return ret;
	}
	//step3 initialize profile
	ret = bt_stack_profile_init(app_conf);
	if (ret) {
		return ret;
	}
	//step 4 stack enable
	if (false == bt_stack_startup(app_conf)) {
		return RTK_BT_FAIL;
	}

	bt_stack_le_gap_wait_ready();
	bt_stack_br_gap_wait_ready();

	bt_stack_post_config();

	return 0;
}

uint16_t bt_stack_disable(void)
{
	uint16_t ret = 0;

	bt_stack_api_stop();

	ret = bt_stack_deinit();
	if (ret) {
		return ret;
	}

	ret = bt_stack_api_deinit();
	if (ret) {
		return ret;
	}

	ret = bt_stack_profile_deinit();
	if (ret) {
		return ret;
	}

	/* leaudio deinit flow: bte_deinit --> ble_audio_deinit --> ble_mgr_deinit -->sys_mgr_deinit */
	/* classic deinit flow: bte_deinit --> a2ap_deinit --> bt_mgr_deinit --> remote_mgr_deinit -->sys_mgr_deinit */
	bt_stack_framework_deinit();

	/* free stack resource after api task terminated */
	bte_deinit_free();
	bt_trace_deinit();

	return 0;
}

uint16_t bt_stack_msg_send(uint16_t type, uint16_t subtype, void *msg)
{
	uint16_t ret = RTK_BT_OK;
	uint8_t event = EVENT_IO_TO_APP;
	T_IO_MSG io_msg;
	uint32_t flags = 0;

	flags = osif_lock();
	api_task_msg_num++;
	osif_unlock(flags);

	if (!api_task_running) {
		ret = RTK_BT_ERR_NOT_READY;
		goto end;
	}

	io_msg.type = type;
	io_msg.subtype = subtype;
	io_msg.u.buf = msg;

	ret = RTK_BT_ERR_OS_OPERATION;
	if (osif_msg_send(api_task_io_msg_q, &io_msg, 0)) {
		if (osif_msg_send(api_task_evt_msg_q, &event, 0)) {
			ret = RTK_BT_OK;
		}
	}

end:
	flags = osif_lock();
	api_task_msg_num--;
	osif_unlock(flags);

	return ret;
}

uint16_t bt_stack_api_send(void *pcmd)
{
	return bt_stack_msg_send(IO_MSG_TYPE_API_SYS_CALL, 0, pcmd);
}

rtk_bt_cmd_t *bt_stack_pending_cmd_search(uint32_t msg_type)
{
	rtk_bt_cmd_t *cmd;

	list_for_each_entry(cmd, &g_cmd_pending_list, list, rtk_bt_cmd_t) {
		if (cmd->user_data == msg_type) {
			return cmd;
		}
	}

	return NULL;
}

void bt_stack_pending_cmd_insert(rtk_bt_cmd_t *p_cmd)
{
	API_PRINT("insert cmd: msg_type = 0x%x\r\n", (unsigned int)p_cmd->user_data);
	list_add_tail(&p_cmd->list, &g_cmd_pending_list);
}

void bt_stack_pending_cmd_delete(rtk_bt_cmd_t *p_cmd)
{
	rtk_bt_cmd_t *cmd, *next;
	API_PRINT("delete cmd: msg_type = 0x%x\r\n", (unsigned int)p_cmd->user_data);
	list_for_each_entry_safe(cmd, next, &g_cmd_pending_list, list, rtk_bt_cmd_t) {
		if (p_cmd == cmd) {
			list_del(&p_cmd->list);
		}
	}
}

void bt_stack_pending_cmd_deinit(void)
{
	rtk_bt_cmd_t *cmd, *next;
	API_PRINT("delete cmd pending list\r\n");

	list_for_each_entry_safe(cmd, next, &g_cmd_pending_list, list, rtk_bt_cmd_t) {
		osif_sem_give(cmd->psem);
		list_del(&cmd->list);
	}
}

void bt_stack_pending_cmd_init(void)
{
	INIT_LIST_HEAD(&g_cmd_pending_list);
}