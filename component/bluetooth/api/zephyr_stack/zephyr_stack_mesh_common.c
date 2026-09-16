/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <rtk_bt_gap.h>
#include <bt_vendor_config.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <access.h>
#include <app_keys.h>
#include <beacon.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <mesh/adv.h>
#include <bt_app_config.h>
#include <rtk_bt_def.h>
#include <zephyr_stack_internal.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_config_client_model.h>
#include <rtk_bt_mesh_sensor_model.h>
#include <rtk_bt_mesh_remote_prov_model.h>
#include <zephyr_stack_mesh_internal.h>

#define BT_MESH_MODEL_ID_DATATRANS 0x0004

extern bool rtk_ble_mesh_scan_enable_flag;

/* -------------------------------------------------MESH init relate function---------------------------------------------------------- */

// Provisioning OOB releate verible and function
#if (defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT) || PROV_SUPPORT_STATIC_OOB
static const uint8_t static_oob_value[16] = STATIC_OOB_VALUE_FOR_AUTHENTICATION;
#endif

static int output_number(bt_mesh_output_action_t action, uint32_t number);
static int notify_input(bt_mesh_input_action_t act, uint8_t size);
static void input_complete_notify(void);

static const uint16_t global_net_idx;
static const uint16_t global_app_idx;
static uint16_t unicast_self_addr = 0;
static bool prov_procedure_success = false;
static enum prov_bearer_link_status last_link_status = PROV_BEARER_LINK_STATUS_SUCCESS;
void bt_mesh_prov_last_link_status(enum prov_bearer_link_status reason)
{
	last_link_status = reason;
}

extern uint16_t bt_stack_gap_vendor_cmd_req(void *param);
static void prov_link_opened(bt_mesh_prov_bearer_t bearer)
{
	uint8_t current_pri = 1;  // 1 means high
	rtk_bt_gap_vendor_cmd_param_t mesh_pri = {VENDOR_CMD_SET_MESH_INFO_OPCODE, 1, &current_pri};
	bt_stack_gap_vendor_cmd_req(&mesh_pri);
	prov_procedure_success = false;

	if ((bearer == BT_MESH_PROV_ADV && !bt_mesh_is_provisioned()) ||
		bearer == BT_MESH_PROV_REMOTE) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_prov_generic_cb_type_t *link_state = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE,
									sizeof(rtk_bt_mesh_prov_generic_cb_type_t));
		if (p_evt) {
			link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)p_evt->data;
			*link_state = RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
}

static void prov_link_closed(bt_mesh_prov_bearer_t bearer)
{
	uint8_t current_pri = 0;  // 0 means low
	rtk_bt_gap_vendor_cmd_param_t mesh_pri = {VENDOR_CMD_SET_MESH_INFO_OPCODE, 1, &current_pri};
	bt_stack_gap_vendor_cmd_req(&mesh_pri);

	if (!prov_procedure_success) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_evt_prov_fail_t *prov_fail = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PROV_FAIL,
									sizeof(rtk_bt_mesh_stack_evt_prov_fail_t));
		if (p_evt) {
			prov_fail = (rtk_bt_mesh_stack_evt_prov_fail_t *)p_evt->data;
			if (last_link_status == PROV_BEARER_LINK_STATUS_TIMEOUT) {
				prov_fail->fail_reason = RTK_BT_MESH_PROV_CB_FAIL_TYPE_PROCEDURE_TIMEOUT;
			} else {
				prov_fail->fail_reason = RTK_BT_MESH_PROV_CB_FAIL_TYPE_PROCEDURE_ABNORMAL;
			}
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}

	if (bearer == BT_MESH_PROV_ADV || bearer == BT_MESH_PROV_REMOTE) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_prov_generic_cb_type_t *link_state = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE,
									sizeof(rtk_bt_mesh_prov_generic_cb_type_t));
		if (p_evt) {
			link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)p_evt->data;
			*link_state = RTK_BT_MESH_PB_GENERIC_CB_LINK_CLOSED;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
}

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
static uint8_t default_pro_uuid[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
static bool pb_adv_link_opening = false;
static bool pb_gatt_link_opening = false;
static uint8_t adv_target_uuid[16] = {0};
static uint8_t gatt_target_uuid[16] = {0};
rtk_bt_mesh_provisioning_method_t pro_provisioning_method = RTK_BT_MESH_PROVISIONING_METHOD_NONE;
static bool dev_info_switch_flag = false;

static void unprovisioned_beacon(uint8_t uuid[16], bt_mesh_prov_oob_info_t oob_info, uint32_t *uri_hash)
{
	(void)oob_info;
	(void)uri_hash;
	if (pb_adv_link_opening && !memcmp(uuid, adv_target_uuid, 16)) {
		// Report PB-ADV link open to app
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_prov_generic_cb_type_t *link_state = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE, sizeof(rtk_bt_mesh_prov_generic_cb_type_t));
		link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)p_evt->data;
		*link_state = RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED;
		rtk_bt_evt_indicate(p_evt, NULL);
		pb_adv_link_opening = false;
		pro_provisioning_method = RTK_BT_MESH_PROVISIONING_PB_ADV_METHOD;
	}

	if (dev_info_switch_flag) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_evt_dev_info_udb_t *device_info_udb;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_DEVICE_INFO_UDB_DISPLAY, sizeof(rtk_bt_mesh_stack_evt_dev_info_udb_t));
		device_info_udb = (rtk_bt_mesh_stack_evt_dev_info_udb_t *)p_evt->data;
		memcpy(device_info_udb->dev_uuid, uuid, 16);
		device_info_udb->dev_info.bt_addr_type = 0xff;  // Used to distinguish zephyr mesh stack with rtk mesh stack
		rtk_bt_evt_indicate(p_evt, NULL);
	}
}

static void unprovisioned_beacon_gatt(uint8_t uuid[16], bt_mesh_prov_oob_info_t oob_info)
{
	(void)oob_info;
	if (pb_gatt_link_opening && !memcmp(uuid, gatt_target_uuid, 16)) {
		// Report PB-GATT conn to app
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_le_conn_ind_t *p_conn_ind = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_GAP, RTK_BT_LE_GAP_EVT_CONNECT_IND, sizeof(rtk_bt_le_conn_ind_t));
		p_conn_ind = (rtk_bt_le_conn_ind_t *)p_evt->data;
		p_conn_ind->err = RTK_BT_OK;
		rtk_bt_evt_indicate(p_evt, NULL);
		pb_gatt_link_opening = false;
		pro_provisioning_method = RTK_BT_MESH_PROVISIONING_PB_GATT_METHOD;
	}

	if (dev_info_switch_flag) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *device_info_prov;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROV_DISPLAY, sizeof(rtk_bt_mesh_stack_evt_dev_info_provision_adv_t));
		device_info_prov = (rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *)p_evt->data;
		memcpy(device_info_prov->dev_uuid, uuid, 16);
		device_info_prov->dev_info.bt_addr_type = 0xff;  // Used to distinguish zephyr mesh stack with rtk mesh stack
		rtk_bt_evt_indicate(p_evt, NULL);
	}
}

#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
extern struct bt_mesh_prov_link bt_mesh_prov_link;
extern rtk_bt_mesh_remote_prov_client_link_open_dkri_t rpr_dkri_info;
#endif
static void node_added(uint16_t idx, uint8_t uuid[16], uint16_t addr, uint8_t num_elem)
{
	(void)idx;
	(void)uuid;
	(void)num_elem;
	struct bt_mesh_cdb_node *node = NULL;
	node = bt_mesh_cdb_node_get(addr);
	if (!node) {
		BT_LOGE("[%s] Get cdb node fail\r\n", __func__);
		return ;
	}

	atomic_set_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED);
	prov_procedure_success = true;

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(node);
	}

	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_stack_evt_prov_complete_t *prov_complete;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PROV_COMPLETE, sizeof(rtk_bt_mesh_stack_evt_prov_complete_t));
	prov_complete = (rtk_bt_mesh_stack_evt_prov_complete_t *)p_evt->data;
	prov_complete->unicast_addr = addr;
#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
	prov_complete->dkri_flag = 0;
	if (atomic_test_bit(bt_mesh_prov_link.flags, REPROVISION)) {
		prov_complete->dkri_flag = 1;
		prov_complete->dkri = rpr_dkri_info.dkri_procedure;
	}
#endif
	rtk_bt_evt_indicate(p_evt, NULL);

}

static const struct bt_mesh_prov pro_prov = {
	.uuid = default_pro_uuid,
	.unprovisioned_beacon = unprovisioned_beacon,
	.unprovisioned_beacon_gatt = unprovisioned_beacon_gatt,
	.node_added = node_added,
	.output_number = output_number,
	.input = notify_input,
	.input_complete = input_complete_notify,
	.link_open = prov_link_opened,
	.link_close = prov_link_closed,
};
#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
static uint8_t default_dev_uuid[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

static void prov_complete(uint16_t net_idx, uint16_t addr)
{
	(void)net_idx;
	prov_procedure_success = true;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_stack_evt_prov_complete_t *prov_complete;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PROV_COMPLETE, sizeof(rtk_bt_mesh_stack_evt_prov_complete_t));
	prov_complete = (rtk_bt_mesh_stack_evt_prov_complete_t *)p_evt->data;
	prov_complete->unicast_addr = addr;
	rtk_bt_evt_indicate(p_evt, NULL);
}

#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
extern enum bt_mesh_rpr_node_refresh bt_mesh_node_refresh_get(void);
#endif
static void prov_reprovisioned(uint16_t addr)
{
	prov_procedure_success = true;

#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
	if (bt_mesh_node_refresh_get() == BT_MESH_RPR_NODE_REFRESH_ADDR) {
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_evt_prov_complete_t *prov_reprov = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_PROV_COMPLETE,
									sizeof(rtk_bt_mesh_stack_evt_prov_complete_t));
		if (p_evt) {
			prov_reprov = (rtk_bt_mesh_stack_evt_prov_complete_t *)p_evt->data;
			prov_reprov->unicast_addr = addr;
			prov_reprov->dkri_flag = 1;
			prov_reprov->dkri = RTK_BT_MESH_RMT_PROV_DKRI_NODE_ADDR_REFRESH;
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
#else
	(void)addr;
#endif
}

static void prov_reset(void)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_UNPROVISIONED_DEVICE, 0);
	rtk_bt_evt_indicate(p_evt, NULL);
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT
#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
						| BT_MESH_PROV_REMOTE
#endif
					   );
}

static const struct bt_mesh_prov dev_prov = {
	.uuid = default_dev_uuid,
	.complete = prov_complete,
	.reprovisioned = prov_reprovisioned,
	.reset = prov_reset,
	.output_number = output_number,
	.input = notify_input,
	.input_complete = input_complete_notify,
#if PROV_SUPPORT_STATIC_OOB
	.static_val = static_oob_value,
	.static_val_len = sizeof(static_oob_value),
#endif
	.output_actions = PROV_SUPPORT_OUTPUT_OOB_ACTION,
	.output_size = PROV_SUPPORT_OUTPUT_OOB_SIZE,
	.input_actions = PROV_SUPPORT_INPUT_OOB_ACTION,
	.input_size = PROV_SUPPORT_INPUT_OOB_SIZE,
	.link_open = prov_link_opened,
	.link_close = prov_link_closed,
};

#endif  // RTK_BLE_MESH_DEVICE_SUPPORT

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
	switch (action) {
	case BT_MESH_BLINK: {
		BT_LOGA("[%s] OOB blink Number: %d\r\n", __func__, (int)number);
	}
	break;
	case BT_MESH_BEEP: {
		BT_LOGA("[%s] OOB beep Number: %d\r\n", __func__, (int)number);
	}
	break;
	case BT_MESH_VIBRATE: {
		BT_LOGA("[%s] OOB vibrate Number: %d\r\n", __func__, (int)number);
	}
	break;
	case BT_MESH_DISPLAY_NUMBER: {
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_set_auth_value_for_oob_data *oob_data;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_INPUT_OOB, sizeof(rtk_bt_mesh_stack_set_auth_value_for_oob_data));
		oob_data = (rtk_bt_mesh_stack_set_auth_value_for_oob_data *)p_evt->data;
		oob_data->status = true;
		oob_data->random = number;
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_stack_set_auth_value_for_oob_data *oob_data;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_OUTPUT_OOB,
									sizeof(rtk_bt_mesh_stack_set_auth_value_for_oob_data));
		oob_data = (rtk_bt_mesh_stack_set_auth_value_for_oob_data *)p_evt->data;
		oob_data->status = true;
		oob_data->random = number;
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
	}
	break;
	default: {
		BT_LOGE("[%s] Unsupport action:0x%x\r\n", __func__, action);
		return -EINVAL;
	}
	break;
	}

	return 0;
}

static int notify_input(bt_mesh_input_action_t act, uint8_t size)
{
	switch (act) {
	case BT_MESH_ENTER_NUMBER: {
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_OUTPUT_OOB_VALUE, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_INPUT_OOB_VALUE, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
#endif
	}
	break;
	case BT_MESH_ENTER_STRING: {
		BT_LOGA("[%s] Enter a string (max %u chars) with: Input-str <str>\r\n", __func__, size);
	}
	break;
	case BT_MESH_TWIST: {
		BT_LOGA("[%s] \"Twist\" a number (max %u digits) with: Input-num <num>\r\n", __func__, size);
	}
	break;
	case BT_MESH_PUSH: {
		BT_LOGA("[%s] \"Push\" a number (max %u digits) with: Input-num <num>\r\n", __func__, size);
	}
	break;
	default: {
		BT_LOGE("[%s] Unknown input action:%d, size:%d\r\n", __func__, act, size);
		return -EINVAL;
	}
	break;
	}

	return 0;
}

static void input_complete_notify(void)
{
	// BT_LOGA("[%s] Remote device input complete\r\n", __func__);
}

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT

#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT

#endif

static bool is_all_zeros_in_buf(uint8_t *p, uint8_t len)
{
	uint8_t i;
	for (i = 0; i < len; i++) {
		if (p[i]) {
			break;
		}
	}
	if (i == len) {
		return true;
	} else {
		return false;
	}
}

static bool bt_stack_mesh_set_uuid(rtk_bt_mesh_app_conf_t *pmesh_conf)
{
	const struct bt_mesh_prov *prov = NULL;

	prov = bt_mesh_prov_get();

	if (prov) {
		if (is_all_zeros_in_buf(pmesh_conf->bt_mesh_uuid, sizeof(pmesh_conf->bt_mesh_uuid) / sizeof(pmesh_conf->bt_mesh_uuid[0]))) {
			size_t count = 1;
			bt_addr_le_t bt_addr = {};
			bt_id_get(&bt_addr, &count);
			memcpy((void *)prov->uuid, bt_addr.a.val, BT_ADDR_SIZE);
		} else {
			memcpy((void *)prov->uuid, pmesh_conf->bt_mesh_uuid, 16);
		}
	} else {
		BT_LOGE("[%s] Get bt_mesh_prov data fail\r\n", __func__);
		return false;
	}
	return true;
}

uint16_t bt_stack_mesh_init(void *data)
{
	int err;
	rtk_bt_mesh_app_conf_t *pmesh_app_conf = &((rtk_bt_app_conf_t *)data)->bt_mesh_app_conf;
	switch (pmesh_app_conf->bt_mesh_role) {
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	case RTK_BT_MESH_ROLE_PROVISIONER:
		err = bt_mesh_init(&pro_prov, bt_mesh_get_composition_data());
		if (err) {
			BT_LOGE("[%s] Mesh init fail:%d\r\n", __func__, err);
			return err;
		}
		break;
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	case RTK_BT_MESH_ROLE_DEVICE:
		err = bt_mesh_init(&dev_prov, bt_mesh_get_composition_data());
		if (err) {
			BT_LOGE("[%s] Mesh init fail:%d\r\n", __func__, err);
			return err;
		}
		break;
#endif
	default:
		BT_LOGE("[%s] Unknown bt_mesh_role:%d\r\n", __func__, pmesh_app_conf->bt_mesh_role);
		break;
	}

	return 0;
}

uint16_t bt_stack_mesh_enable(void *data)
{
	rtk_bt_mesh_app_conf_t *pmesh_app_conf = &((rtk_bt_app_conf_t *)data)->bt_mesh_app_conf;
	bt_stack_mesh_set_uuid(pmesh_app_conf);
	switch (pmesh_app_conf->bt_mesh_role) {
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	case RTK_BT_MESH_ROLE_PROVISIONER:
		break;
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	case RTK_BT_MESH_ROLE_DEVICE:
		/* This will be a no-op if settings_load() loaded provisioning info */
		bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT
#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
							| BT_MESH_PROV_REMOTE
#endif
						   );
		break;
#endif
	default:
		BT_LOGE("[%s] Unknown bt_mesh_role:%d\r\n", __func__, pmesh_app_conf->bt_mesh_role);
		break;
	}
	return 0;
}

extern void bt_mesh_deinit(void);
void bt_stack_mesh_deinit(void)
{
	unicast_self_addr = 0;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	pb_adv_link_opening = false;
	pb_gatt_link_opening = false;
	memset(adv_target_uuid, 0, sizeof(adv_target_uuid));
	memset(gatt_target_uuid, 0, sizeof(gatt_target_uuid));
	pro_provisioning_method = RTK_BT_MESH_PROVISIONING_METHOD_NONE;
	dev_info_switch_flag = false;
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	// Disable relate timer, if not do it, the timer will fail to enter callback when repeat init
	bt_mesh_beacon_disable();
#endif
	rtk_ble_mesh_scan_enable_flag = false;
	// Free the mesh stack relate resource
	bt_mesh_deinit();
	return ;
}

/* -------------------------------------------------MESH common API relate function---------------------------------------------------------- */

struct model_err_struct {
	struct bt_mesh_cdb_node *self;
	uint8_t status;
};

static void pre_process_prov_param(rtk_bt_mesh_stack_act_set_prov_param_t *p_data)
{
	uint8_t net_key[16] = {0x7d, 0xd7, 0x36, 0x4c, 0xd8, 0x42, 0xad, 0x18, 0xc1, 0x7c, 0x2b, 0x82, 0x0c, 0x84, 0xc3, 0xd6};
	uint8_t app_key[16] = {0x63, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76, 0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};
	uint8_t dev_key[16] = {0x12, 0x88, 0x98, 0xa4, 0x7e, 0x34, 0xaa, 0xef, 0x12, 0x9f, 0x3e, 0xd8, 0xaa, 0x4f, 0x9e, 0x34};

	size_t count = 1;
	bt_addr_le_t bt_addr = {};
	bt_id_get(&bt_addr, &count);

	if (!p_data->unicast_addr) {
		p_data->unicast_addr = 0x200 | bt_addr.a.val[0];
	}

	if (is_all_zeros_in_buf(p_data->net_key, 16)) {
		memcpy(p_data->net_key, net_key, 16);
		memcpy(&p_data->net_key[10], bt_addr.a.val, BT_ADDR_SIZE);
	}

	if (is_all_zeros_in_buf(p_data->app_key, 16)) {
		memcpy(p_data->app_key, app_key, 16);
		memcpy(&p_data->app_key[10], bt_addr.a.val, BT_ADDR_SIZE);
	}

	if (is_all_zeros_in_buf(p_data->dev_key, 16)) {
		memcpy(p_data->dev_key, dev_key, 16);
		memcpy(&p_data->dev_key[10], bt_addr.a.val, BT_ADDR_SIZE);
	}
}

static void own_models_bind_app_key(const struct bt_mesh_model *model, const struct bt_mesh_elem *elem, bool vnd, bool primary, void *user_data)
{
	(void) elem;
	(void) vnd;
	(void) primary;
	struct model_err_struct *bind = (struct model_err_struct *)user_data;
	int i;
	uint16_t md_id = model->id;

	// There is already a model add app key fail, so do not continue
	if (bind->status) {
		return ;
	}

	BT_LOGD("[%s] model %p key_idx 0x%03x.\r\n", __func__, model, global_app_idx);

	// The follow models do not need bind app key, they use device key:Config client and config server,Remote provision client and Remote provision server,
	// Private beacon client and Private beacon server
	if (BT_MESH_MODEL_ID_CFG_SRV == md_id || BT_MESH_MODEL_ID_CFG_CLI == md_id || BT_MESH_MODEL_ID_REMOTE_PROV_SRV == md_id || \
		BT_MESH_MODEL_ID_REMOTE_PROV_CLI == md_id || BT_MESH_MODEL_ID_PRIV_BEACON_SRV == md_id || BT_MESH_MODEL_ID_PRIV_BEACON_CLI == md_id) {
		BT_LOGD("[%s] The special model id:0x%x, do not need bind app key.\r\n", __func__, md_id);
		return ;
	}

	if (!bt_mesh_app_key_exists(global_app_idx)) {
		// The app key do not exist
		bind->status = 0x01;
		return ;
	}

	for (i = 0; i < model->keys_cnt; i++) {
		BT_LOGD("[%s] model %p id 0x%04x i %d key 0x%03x", __func__, model, model->id, i, model->keys[i]);
		/* Treat existing binding as success */
		if (model->keys[i] == global_app_idx) {
			return ;
		}
	}

	for (i = 0; i < model->keys_cnt; i++) {
		if (model->keys[i] == BT_MESH_KEY_UNUSED) {
			model->keys[i] = global_app_idx;

			if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
				bt_mesh_model_bind_store(model);
			}

			return ;
		}
	}

	bind->status = 0x02;
	BT_LOGE("[%s] Bind app key fail for model id:0x%x, there is no sufficient resource. app key index:%d.\r\n", __func__, md_id, global_app_idx);

	return ;
}

static uint16_t zephyr_stack_set_prov_param(rtk_bt_mesh_stack_act_set_prov_param_t *p_data)
{
	uint16_t ret = RTK_BT_MESH_STACK_API_FAIL;
	int err;
	struct bt_mesh_cdb_node *self = NULL;

	// Set the default val if the data is all 0s
	pre_process_prov_param(p_data);

	err = bt_mesh_cdb_create(p_data->net_key);
	if (err == -EALREADY) {
		BT_LOGD("[%s] CDB already exist.\r\n", __func__);
		struct bt_mesh_cdb_subnet *net_key = NULL;
		struct bt_mesh_cdb_app_key *app_key = NULL;
		net_key = bt_mesh_cdb_subnet_get(global_net_idx);
		app_key = bt_mesh_cdb_app_key_get(global_app_idx);
		if (net_key && app_key) {
			if (memcmp(p_data->net_key, net_key->keys[0].net_key.key, 16)) {
				BT_LOGE("[%s] The new net key is not equal to old net key(stored in flash last time), you should input the reset AT command and push the reset butten\r\n",
						__func__);
				return err;
			}
			if (memcmp(p_data->app_key, app_key->keys[0].app_key.key, 16)) {
				BT_LOGE("[%s] The new app key is not equal to old app key(stored in flash last time), you should input the reset AT command and push the reset butten\r\n",
						__func__);
				return err;
			}
		} else {
			BT_LOGE("[%s] Get net key or app key fail, you should input the reset AT command and push the reset butten\r\n", __func__);
			return err;
		}
	} else if (err) {
		BT_LOGE("[%s] Failed to create CDB (err %d)\n", __func__, err);
		return err;
	} else {
		BT_LOGD("[%s] Created CDB.\r\n", __func__);
		struct bt_mesh_cdb_app_key *key;
		int err;

		key = bt_mesh_cdb_app_key_alloc(global_net_idx, global_app_idx);
		if (key == NULL) {
			BT_LOGE("[%s] Failed to allocate app-key 0x%04x\n", __func__, global_app_idx);
			return ret;
		}

		err = bt_mesh_cdb_app_key_import(key, 0, p_data->app_key);
		if (err) {
			BT_LOGE("[%s] Failed to import appkey into cdb. Err:%d\r\n", __func__, err);
			return err;
		}

		if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
			bt_mesh_cdb_app_key_store(key);
		}
	}
	err = bt_mesh_provision(p_data->net_key, BT_MESH_NET_PRIMARY, 0, 0, p_data->unicast_addr, p_data->dev_key);
	unicast_self_addr = p_data->unicast_addr;
	if (err == -EALREADY) {
		BT_LOGD("[%s] Using stored settings.\r\n", __func__);
	} else if (err) {
		BT_LOGE("[%s] Provisioning failed (err %d)\n", __func__, err);
		return err;
	}

	self = bt_mesh_cdb_node_get(p_data->unicast_addr);
	if (!self) {
		BT_LOGE("[%s] The new unicast address of provisioner is not equal to old address(stored in flash last time), you should input the reset AT command and push the reset button\r\n",
				__func__);
		return ret;
	}
	err = bt_mesh_app_key_add(global_app_idx, global_net_idx, p_data->app_key);
	if (err) {
		BT_LOGE("[%s] Failed to add app-key (err %d)\n", __func__, err);
		return err;
	}

	// Bind app key for models
	struct model_err_struct bind_err = {0};
	bind_err.self = self;
	bt_mesh_model_foreach(own_models_bind_app_key, &bind_err);
	if (bind_err.status) {
		BT_LOGE("[%s] Models bind app key fail, status:%d.\r\n", __func__, bind_err.status);
		return err;
	}

	atomic_set_bit(self->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(self);
	}

	return RTK_BT_MESH_STACK_API_SUCCESS;
}

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
static uint16_t zephyr_stack_pb_adv_con(rtk_bt_mesh_stack_act_pb_adv_con_t *pbadvcon)
{
	memcpy(adv_target_uuid, pbadvcon->uuid, 16);
	pb_adv_link_opening = true;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_pb_adv_discon(void)
{
	pb_adv_link_opening = false;
	memset(adv_target_uuid, 0, 16);
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_pb_gatt_con(rtk_bt_mesh_stack_act_pb_gatt_con_t *pgatt_con)
{
	int ret = RTK_BT_MESH_STACK_API_SUCCESS;
	if (pgatt_con->param.is_prov_link) {
		memcpy(gatt_target_uuid, pgatt_con->param.u.uuid, 16);
		pb_gatt_link_opening = true;
	} else {
		ret = bt_mesh_proxy_connect(pgatt_con->param.u.net_idx);
	}
	if (-EALREADY == ret) {
		// BT_LOGA("[%s] Proxy link is already connect\r\n", __func__);
		ret = RTK_BT_MESH_STACK_API_SUCCESS;
	}
	return (uint16_t)ret;
}

static uint16_t zephyr_stack_prov_service_discovery(rtk_bt_mesh_stack_act_prov_dis_t *prov_dis)
{
	(void)prov_dis;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_prov_service_set_notify(rtk_bt_mesh_stack_act_prov_set_notify_t *prov_notify)
{
	(void)prov_notify;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_proxy_service_discovery(rtk_bt_mesh_stack_act_proxy_dis_t *proxy_dis)
{
	(void)proxy_dis;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_proxy_service_set_notify(rtk_bt_mesh_stack_act_proxy_set_notify_t *proxy_notify)
{
	(void)proxy_notify;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_pb_gatt_discon(rtk_bt_mesh_stack_act_pb_gatt_discon_t *pgatt_discon)
{
	int ret = RTK_BT_MESH_STACK_API_SUCCESS;
	if (pgatt_discon->param.is_prov_link) {
		pb_gatt_link_opening = false;
		memset(gatt_target_uuid, 0, 16);
	} else {
		ret = bt_mesh_proxy_disconnect(pgatt_discon->param.net_idx);
	}
	if (-EALREADY == ret) {
		// BT_LOGA("[%s] Proxy link is already disconnect\r\n", __func__);
		ret = RTK_BT_MESH_STACK_API_SUCCESS;
	}
	return (uint16_t)ret;
}

#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
extern rtk_bt_mesh_remote_prov_client_link_open_t rpr_link_open_info;
extern struct bt_mesh_rpr_cli rpr_cli;
#endif

static uint16_t zephyr_stack_start_provisioning(rtk_bt_mesh_stack_act_start_prov_t *start_prov)
{
	uint16_t ret = RTK_BT_MESH_STACK_API_FAIL;
	switch (pro_provisioning_method) {
	case RTK_BT_MESH_PROVISIONING_PB_ADV_METHOD:
		ret = (uint16_t)bt_mesh_provision_adv(adv_target_uuid, global_net_idx, 0, start_prov->attn_dur);
		break;
	case RTK_BT_MESH_PROVISIONING_PB_GATT_METHOD:
		ret = (uint16_t)bt_mesh_provision_gatt(gatt_target_uuid, global_net_idx, 0, 0);
		break;
	case RTK_BT_MESH_PROVISIONING_METHOD_NONE:
		BT_LOGE("[%s] Not received UDB matched\r\n", __func__);
		break;
#if defined(CONFIG_BT_MESH_RPR_CLI) && CONFIG_BT_MESH_RPR_CLI
	case RTK_BT_MESH_PROVISIONING_REMOTE_PROV_METHOD: {
		const struct bt_mesh_rpr_node srv = {
			.addr = rpr_link_open_info.dst,
			.net_idx = rpr_link_open_info.net_key_index,
			.ttl = BT_MESH_TTL_DEFAULT,
		};
		ret = (uint16_t)bt_mesh_provision_remote(&rpr_cli, &srv, rpr_link_open_info.uuid,
												 rpr_link_open_info.net_key_index, 0, rpr_link_open_info.link_open_timeout);
		break;
	}
#endif
	default:
		break;
	}
	return ret;
}

static uint16_t zephyr_stack_method_choose_for_prov(rtk_bt_mesh_stack_prov_start_t *method)
{
	uint16_t ret = RTK_BT_MESH_STACK_API_FAIL;
	int err;

	if (RTK_BT_MESH_PROV_START_FIPS_P256_ELLIPTIC_CURVE != method->algorithm) {
		BT_LOGE("[%s] Can not support algorithm:%d\r\n", __func__, method->algorithm);
		return ret;
	}

	if (RTK_BT_MESH_PROV_START_NO_OOB_PUBLIC_KEY != method->public_key) {
		BT_LOGE("[%s] Can not support public method:%d\r\n", __func__, method->public_key);
		return ret;
	}

	switch (method->auth_method) {
	case RTK_BT_MESH_PROV_AUTH_METHOD_NO_OOB: {
		err = bt_mesh_auth_method_set_none();
		if (err) {
			BT_LOGE("[%s] Set auth method to none fail:%d\r\n", __func__, err);
			ret = (uint16_t)err;
			return ret;
		}
		return (uint16_t)err;
	}
	break;
	case RTK_BT_MESH_PROV_AUTH_METHOD_STATIC_OOB: {
		err = bt_mesh_auth_method_set_static(static_oob_value, sizeof(static_oob_value));
		if (err) {
			BT_LOGE("[%s] Set static oob value fail:%d\r\n", __func__, err);
			ret = (uint16_t)err;
			return ret;
		}
		return (uint16_t)err;
	}
	break;
	case RTK_BT_MESH_PROV_AUTH_METHOD_OUTPUT_OOB: {
		bt_mesh_output_action_t output_method = BT_MESH_NO_OUTPUT;
		switch (method->auth_action.output_oob_action) {
		case RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_BLINK: {
			output_method = BT_MESH_BLINK;
		}
		break;
		case RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_BEEP: {
			output_method = BT_MESH_BEEP;
		}
		break;
		case RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_VIBRATE: {
			output_method = BT_MESH_VIBRATE;
		}
		break;
		case RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_OUTPUT_NUMERIC: {
			output_method = BT_MESH_DISPLAY_NUMBER;
		}
		break;
		case RTK_BT_MESH_PROV_START_OUTPUT_OOB_ACTION_OUTPUT_ALPHANUMERIC: {
			output_method = BT_MESH_DISPLAY_STRING;
		}
		break;
		default: {
			BT_LOGE("[%s] Unknown output oob action:%d\r\n", __func__, method->auth_action.output_oob_action);
			return ret;
			break;
		}
		}
		err = bt_mesh_auth_method_set_output(output_method, method->auth_size.output_oob_size);
		if (err) {
			BT_LOGE("[%s] Set output method fail:%d\r\n", __func__, err);
			ret = (uint16_t)err;
			return ret;
		}
		return (uint16_t)err;
	}
	break;
	case RTK_BT_MESH_PROV_AUTH_METHOD_INPUT_OOB: {
		bt_mesh_input_action_t input_method = BT_MESH_NO_INPUT;
		switch (method->auth_action.input_oob_action) {
		case RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_PUSH: {
			input_method = BT_MESH_PUSH;
		}
		break;
		case RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_TWIST: {
			input_method = BT_MESH_TWIST;
		}
		break;
		case RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_INPUT_NUMERIC: {
			input_method = BT_MESH_ENTER_NUMBER;
		}
		break;
		case RTK_BT_MESH_PROV_START_INPUT_OOB_ACTION_INPUT_ALPHANUMERIC: {
			input_method = BT_MESH_ENTER_STRING;
		}
		break;
		default: {
			BT_LOGE("[%s] Unknown auth method:%d\r\n", __func__, method->auth_method);
			return ret;
		}
		break;
		}
		err = bt_mesh_auth_method_set_input(input_method, method->auth_size.oob_size);
		if (err) {
			BT_LOGE("[%s] Set input method fail:%d\r\n", __func__, err);
			ret = (uint16_t)err;
			return ret;
		}
		return (uint16_t)err;
	}
	break;
	default: {
		BT_LOGE("[%s] Unknown auth method:%d\r\n", __func__, method->auth_method);
	}
	break;
	}
	return ret;
}

static void zephyr_stack_set_device_info_display(rkt_bt_mesh_stack_act_device_info_set_t *p_data)
{
	if (p_data->dev_info_dis) {
		dev_info_switch_flag = true;
	} else {
		dev_info_switch_flag = false;
	}
}

#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
static void lpn_established(uint16_t net_idx, uint16_t friend_addr,
							uint8_t queue_size, uint8_t recv_win)
{
	(void)net_idx;
	(void)queue_size;
	(void)recv_win;
	rtk_bt_mesh_stack_evt_lpn_t *lpn_cb;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_LPN_CB, sizeof(rtk_bt_mesh_stack_evt_lpn_t));
	lpn_cb = (rtk_bt_mesh_stack_evt_lpn_t *)p_evt->data;
	lpn_cb->fn_addr = friend_addr;
	lpn_cb->cb_type = RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_SUCCESS;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void lpn_terminated(uint16_t net_idx, uint16_t friend_addr)
{
	(void)net_idx;
	rtk_bt_mesh_stack_evt_lpn_t *lpn_cb;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_LPN_CB, sizeof(rtk_bt_mesh_stack_evt_lpn_t));
	lpn_cb = (rtk_bt_mesh_stack_evt_lpn_t *)p_evt->data;
	lpn_cb->fn_addr = friend_addr;
	lpn_cb->cb_type = RTK_BT_MESH_LPN_CB_TYPE_FRIENDSHIP_LOST;
	rtk_bt_evt_indicate(p_evt, NULL);
	bt_mesh_scan_enable();
}

static void lpn_polled(uint16_t net_idx, uint16_t friend_addr, bool retry)
{
	(void)net_idx;
	(void)friend_addr;
	(void)retry;
	// For debug test
	// BT_LOGA("[%s] LPN: Polling 0x%04x (%s)\r\n", __func__, friend_addr, retry ? "retry" : "initial");
}

BT_MESH_LPN_CB_DEFINE(lpn_cb) = {
	.established = lpn_established,
	.terminated = lpn_terminated,
	.polled = lpn_polled,
};

static uint16_t zephyr_stack_lpn_init(rtk_bt_mesh_stack_act_lpn_init_t *lpn)
{
	(void)lpn;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_lpn_req(rtk_bt_mesh_stack_act_lpn_req_t *lpn)
{
	(void)lpn;
	return bt_mesh_lpn_set(true);
}

static void zephyr_stack_lpn_sub(rtk_bt_mesh_stack_act_lpn_sub_t *lpn_sub)
{
	if (lpn_sub->add_rm) {
		bt_mesh_lpn_group_add(lpn_sub->addr);
	} else {
		bt_mesh_lpn_group_del(&lpn_sub->addr, 1);
	}
}

static int zephyr_stack_lpn_clear(rtk_bt_mesh_stack_act_lpn_clear_t *clear_friendship)
{
	(void)clear_friendship;
	int ret = bt_mesh_lpn_set(false);
	bt_mesh_scan_enable();
	return ret;
}

static uint16_t zephyr_stack_lpn_deinit(void)
{
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

#endif  // RTK_BLE_MESH_LPN_SUPPORT

static uint16_t zephyr_stack_set_service_adv(rtk_bt_mesh_stack_act_set_service_adv_t *param)
{
	if (param->enable) {
		bt_mesh_adv_gatt_enable();
	} else {
		bt_mesh_adv_gatt_disable();
	}

	return RTK_BT_MESH_STACK_API_SUCCESS;
}
#endif  // RTK_BLE_MESH_DEVICE_SUPPORT

uint8_t rtk_bt_mesh_stack_set_scan_switch(bool scan_switch)
{
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_stack_evt_scan_switch_t *p_scan_switch;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_SCAN_SWITCH,
								sizeof(rtk_bt_mesh_stack_evt_scan_switch_t));
	p_scan_switch = (rtk_bt_mesh_stack_evt_scan_switch_t *)p_evt->data;
	p_scan_switch->scan_enable = scan_switch;
	rtk_bt_evt_indicate(p_evt, NULL);
	rtk_ble_mesh_scan_enable_flag = scan_switch;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

/*---------------------------------- USER CMD LIST INFO RELATE FUNCTION -------------------------------------------------*/
#define USER_CMD_LIST_INFO_MAX_LEN    1024
extern struct bt_mesh_net bt_mesh;

static uint32_t get_seq_num_from_mesh_stack(void)
{
	return bt_mesh.seq;
}

static uint32_t get_iv_index_from_mesh_stack(void)
{
	return bt_mesh.iv_index;
}

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
static bool get_device_key_from_mesh_stack(uint8_t *p, uint8_t len)
{
	if (16 != len) {
		return false;
	}
	memcpy(p, bt_mesh.dev_key.key, len);
#if defined(CONFIG_BT_MESH_RPR_SRV) && CONFIG_BT_MESH_RPR_SRV
	if (atomic_test_bit(bt_mesh.flags, BT_MESH_DEVKEY_CAND)) {
		memcpy(p, bt_mesh.dev_key_cand.key, len);
	}
#endif
	return true;
}
#endif

/*
 * @fn      add_list_info_one_block
 * @brief   Save one block data for list info.
 *
 * @param   p_start : start point of data to save one block data
 * @param   info_type : the type of block data type
 * @param   p_val : source data point
 * @param   val_len : lenth of source data
 * @return  The end of data saved
 */
static uint8_t *add_list_info_one_block(uint8_t *p_start, rtk_bt_mesh_stack_user_list_type_t info_type, const uint8_t *p_val, uint8_t val_len)
{
	// Save list info data use type(1byte) + len(1byte) + value(n bytes) + type + len + value + ......
	uint8_t *p = p_start;
	static uint32_t sum_len = 0;
	if (!p_start) {
		BT_LOGE("[%s] p_start == NULL\r\n", __func__);
		return NULL;
	}
	if (RTK_BT_MESH_STACK_USER_LIST_MESH_STATE == info_type) {
		sum_len = 0;
	}
	sum_len += 2 + val_len;
	if (sum_len > USER_CMD_LIST_INFO_MAX_LEN) {
		BT_LOGE("[%s] The sum of len(%d) extend max len(%d) of buffer\r\n", __func__, (int)sum_len, USER_CMD_LIST_INFO_MAX_LEN);
		return NULL;
	}

	*p++ = info_type;

	*p++ = val_len;

	memcpy(p, p_val, val_len);
	p += val_len;

	return p;
}

/*
 * @fn      copy_data_to_memory_byte_by_byte
 * @brief   Save a value to memory using little endian.
 *
 * @param   val : the data to be saved
 * @param   p : the point of memory save data
 * @param   data_len : the length of val
 * @return  operation result
 */
static bool copy_data_to_memory_byte_by_byte(uint32_t val, uint8_t *p, uint8_t data_len)
{
	if (!p || data_len > 4) {
		BT_LOGE("[%s] copy data to memory fail\r\n", __func__);
		return false;
	}
	for (uint8_t i = 0; i < data_len; i++) {
		p[i] = (val & (0xff << i * 8)) >> i * 8;
	}
	return true;
}

static void get_bt_address_list_info(uint8_t **p_data)
{
	size_t count = 1;
	bt_addr_le_t bt_addr = {};
	bt_id_get(&bt_addr, &count);
	*p_data = add_list_info_one_block(*p_data, RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR, bt_addr.a.val, 6);
}

#define DEVICE_KEN_LIST_INFO_LEN    20
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
static uint8_t device_key_index = 0;
static uint8_t pro_get_nodes_list_info(struct bt_mesh_cdb_node *node, void *data)
{
	uint8_t device_key_info[DEVICE_KEN_LIST_INFO_LEN] = {0};
	uint8_t *p, **pp_data;
	pp_data = data;
	p = device_key_info;
	if (node->addr != unicast_self_addr) {
		*p++ = device_key_index++;
		copy_data_to_memory_byte_by_byte(node->addr, p, 2);
		p += 2;
		*p++ = node->num_elem;
		memcpy(p, node->dev_key.key, 16);
		*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_DEV_KEY, device_key_info, DEVICE_KEN_LIST_INFO_LEN);
	}
	return BT_MESH_CDB_ITER_CONTINUE;
}
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
static void dev_get_nodes_list_info(uint8_t **pp_data)
{
	uint8_t device_key_info[DEVICE_KEN_LIST_INFO_LEN] = {0};
	uint8_t *p;
	uint16_t addr = 0;
	p = device_key_info;
	*p++ = 0;
	addr = bt_mesh_primary_addr();
	if (addr) {
		copy_data_to_memory_byte_by_byte(addr, p, 2);
		p += 2;
		*p++ = bt_mesh_elem_count();
		get_device_key_from_mesh_stack(p, 16);
		*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_DEV_KEY, device_key_info, DEVICE_KEN_LIST_INFO_LEN);
	}
}
#endif

#define APP_KEY_LIST_INFO_LEN    23
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
static void zephyr_get_and_copy_app_key(uint16_t app_index, uint8_t *p, uint32_t len)
{
	struct bt_mesh_msg_ctx ctx = {0};
	struct bt_mesh_subnet sub = {0}, *p_sub = &sub;
	const struct bt_mesh_key *p_app_key;
	uint8_t app_id;
	int ret;

	if (16 != len) {
		BT_LOGE("[%s] The input len is not 16, fail.\r\n", __func__);
		memset(p, 0, 16);
		return ;
	}

	ctx.app_idx = app_index;
	ret = bt_mesh_keys_resolve(&ctx, &p_sub, &p_app_key, &app_id);
	if (ret) {
		BT_LOGE("[%s] Get app key fail, ret:%d.\r\n", __func__, ret);
		memset(p, 0, len);
		return ;
	}

	memcpy(p, p_app_key->key, len);
}
#endif

static void get_app_key_list_info(uint8_t **pp_data)
{
	uint8_t app_key_info[APP_KEY_LIST_INFO_LEN] = {0};
	uint8_t *p;

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint8_t app_idxs = 0;
	for (unsigned int i = 0; i < ARRAY_SIZE(bt_mesh_cdb.app_keys); i++) {
		struct bt_mesh_cdb_app_key *app_key = &bt_mesh_cdb.app_keys[i];
		if (app_key->net_idx != 0xffff) {
			p = app_key_info;
			*p++ = app_idxs;
			*(uint16_t *)p = app_key->app_idx;
			p += 2;
			*p++ = 1;
			*p++ = 0;
			*(uint16_t *)p = app_key->net_idx;
			p += 2;
			memcpy(p, app_key->keys[0].app_key.key, 16);
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_APP_KEY, app_key_info, APP_KEY_LIST_INFO_LEN);
			app_idxs++;
		}
	}
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	uint16_t app_idxs[CONFIG_BT_MESH_APP_KEY_COUNT] = {0};
	uint16_t net_idxs[CONFIG_BT_MESH_SUBNET_COUNT] = {0};
	size_t count = 0;
	count = bt_mesh_subnets_get(net_idxs, CONFIG_BT_MESH_SUBNET_COUNT, 0);
	size_t app_count = 0;
	uint8_t index = 0;
	for (size_t i = 0; i < count; i++) {
		app_count = bt_mesh_app_keys_get(net_idxs[i], app_idxs, CONFIG_BT_MESH_APP_KEY_COUNT, 0);
		for (size_t j = 0; j < app_count; j++) {
			p = app_key_info;
			*p++ = index;

			*(uint16_t *)p = app_idxs[j];
			p += 2;
			*p++ = 1;
			*p++ = 0;
			*(uint16_t *)p = net_idxs[i];
			p += 2;
			zephyr_get_and_copy_app_key(app_idxs[j], p, 16);
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_APP_KEY, app_key_info, APP_KEY_LIST_INFO_LEN);
			index++;
		}
	}
#endif
}

#define NET_KEY_LIST_INFO_LEN    23
static void get_net_key_list_info(uint8_t **pp_data)
{
	uint8_t net_key_info[NET_KEY_LIST_INFO_LEN] = {0};
	uint8_t *p;

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint8_t net_idxs = 0;
	for (unsigned int i = 0; i < ARRAY_SIZE(bt_mesh_cdb.subnets); i++) {
		struct bt_mesh_cdb_subnet *net_key = &bt_mesh_cdb.subnets[i];
		if (net_key->net_idx != 0xffff) {
			p = net_key_info;
			*p++ = 1; //net key type
			*p++ = net_idxs;

			*(uint16_t *)p = net_key->net_idx;
			p += 2;

			*p++ = 1;
			*p++ = 0;
			*p++ = net_key->kr_phase;
			memcpy(p, net_key->keys[0].net_key.key, 16);
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_NET_KEY, net_key_info, NET_KEY_LIST_INFO_LEN);
			net_idxs++;
		}
	}
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	struct bt_mesh_subnet *subnet = NULL;
	uint16_t net_idxs[CONFIG_BT_MESH_SUBNET_COUNT] = {0};
	size_t count = 0;
	count = bt_mesh_subnets_get(net_idxs, CONFIG_BT_MESH_SUBNET_COUNT, 0);
	uint8_t net_index = 0;
	for (size_t i = 0; i < count; i++) {
		subnet = bt_mesh_subnet_get(net_idxs[i]);
		if (subnet) {
			p = net_key_info;
			*p++ = 1; //net key type
			*p++ = net_index;

			*(uint16_t *)p = subnet->net_idx;
			p += 2;

			*p++ = 1;
			*p++ = 0;
			*p++ = subnet->kr_phase;
			memcpy(p, subnet->keys[0].net.key, 16);
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_NET_KEY, net_key_info, NET_KEY_LIST_INFO_LEN);
			net_index++;
		}
	}
#endif
}

#define NORMAL_VALS_LIST_INFO_LEN    16
static void get_normal_vals_list_info(uint8_t **pp_data)
{
	uint8_t normal_vals_info[NORMAL_VALS_LIST_INFO_LEN] = {0};
	uint8_t *p;
	const struct bt_mesh_comp *comp;
	p = normal_vals_info;

	*p++ = 0;  // iv update falg

	*(uint32_t *)p = get_iv_index_from_mesh_stack();  // Add iv index
	p += sizeof(uint32_t);

	*(uint32_t *)p = get_seq_num_from_mesh_stack();  // Add sequence number
	p += sizeof(uint32_t);

	*(uint16_t *)p = bt_mesh_primary_addr();  // Add primary addr
	p += sizeof(uint16_t);

	*(uint32_t *)p = bt_mesh_elem_count();  // Add element number
	p += sizeof(uint32_t);

	uint8_t models_num = 0;
	comp = bt_mesh_comp_get();
	for (int i = 0; i < bt_mesh_elem_count(); i++) {
		models_num += (comp->elem + i)->model_count + (comp->elem + i)->vnd_model_count;
	}
	*p++ = models_num;  // Add models number

	*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS, normal_vals_info, NORMAL_VALS_LIST_INFO_LEN);
}

#define MODEL_PUB_LIST_INFO_LEN    5
static void get_model_pub_list_info(struct bt_mesh_model_pub *publish, uint8_t **pp_data)
{
	uint8_t model_pub_info[MODEL_PUB_LIST_INFO_LEN] = {0};
	uint8_t *p = model_pub_info;

	if (publish && publish->addr) {
		copy_data_to_memory_byte_by_byte(publish->addr, p, 2);  // Add publish address
		p += 2;
		*p++ = publish->ttl;  // Add publish TTL
		*(uint16_t *)p = publish->key;  // Add publish app key index

		*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO, model_pub_info, MODEL_PUB_LIST_INFO_LEN);
	}
}

#define MODELS_LIST_INFO_LEN    6
static void get_models_list_info(const struct bt_mesh_model *const model, uint8_t **pp_data, bool vnd)
{
	uint8_t model_info[MODELS_LIST_INFO_LEN] = {0};
	uint8_t *p;
	p = model_info;
	*p++ = model->rt->mod_idx;  // Add model index
	*p++ = model->rt->mod_idx;  // Add model index
	if (vnd) {
		copy_data_to_memory_byte_by_byte(*(uint32_t *)&model->id, p, 4);  // Add Vendor model ID
	} else {
		uint32_t model_id = 0xffff + (model->id << 16);
		copy_data_to_memory_byte_by_byte(model_id, p, 4);  // Add SIG model ID
	}
	*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO, model_info, MODELS_LIST_INFO_LEN);

	for (uint8_t i = 0; i < model->keys_cnt; i++) {
		if (model->keys[i] != 0xffff && bt_mesh_app_key_exists(model->keys[i])) {
			// Add app key list info
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY, (uint8_t *) &model->keys[i], 2);
		}
	}

	// Add model pub list info
	get_model_pub_list_info(model->pub, pp_data);

	for (int i = 0; i < model->groups_cnt; i++) {
		// Add model sub list info
		if (*(model->groups + i)) {
			*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO, (uint8_t *)(model->groups + i), 2);
		}
	}
}

#define ELEMENT_LIST_INFO_LEN    5
static void get_elements_list_info(const struct bt_mesh_elem *element, uint32_t elem_index, uint8_t **pp_data)
{
	uint8_t element_info[ELEMENT_LIST_INFO_LEN] = {0};
	uint8_t *p;

	p = element_info;
	*p++ = elem_index;  // Add element index
	*(uint32_t *)p = element->model_count + element->vnd_model_count;  // Add models number

	*pp_data = add_list_info_one_block(*pp_data, RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO, element_info, ELEMENT_LIST_INFO_LEN);

	for (int i = 0; i < element->model_count; i++) {
		get_models_list_info(element->models + i, pp_data, 0);
	}
	for (int i = 0; i < element->vnd_model_count; i++) {
		get_models_list_info(element->vnd_models + i, pp_data, 1);
	}
}

static void get_elements_and_models_list_info(uint8_t **pp_data)
{
	const struct bt_mesh_comp *comp;
	comp = bt_mesh_comp_get();
	for (size_t i = 0; i < comp->elem_count; i++) {
		get_elements_list_info(&comp->elem[i], i, pp_data);
	}
}

static uint16_t zephyr_stack_list_info(void)
{
	uint8_t list_copy[USER_CMD_LIST_INFO_MAX_LEN];
	uint8_t value, *p_data;
	uint32_t len;
	const struct bt_mesh_prov *prov;
	p_data = (uint8_t *)list_copy;
	// Indicate event data struct : type(1 byte) + length(1 byte) + value(n bytes) + type +len + val ......

	// Add mesh state(1 byte)
	value = (uint8_t)bt_mesh_is_provisioned();
	p_data = add_list_info_one_block(p_data, RTK_BT_MESH_STACK_USER_LIST_MESH_STATE, &value, 1);

	// Add device UUID(16 bytes)
	prov = bt_mesh_prov_get();
	if (prov) {
		p_data = add_list_info_one_block(p_data, RTK_BT_MESH_STACK_USER_LIST_DEV_UUID, prov->uuid, 16);
	}

	// Add BT address(6 bytes)
	get_bt_address_list_info(&p_data);

	// Add devices key
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	device_key_index = 0;
	bt_mesh_cdb_node_foreach(pro_get_nodes_list_info, &p_data);
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	dev_get_nodes_list_info(&p_data);
#endif

	// Add app key
	get_app_key_list_info(&p_data);

	// Add net key
	get_net_key_list_info(&p_data);

	// Add normal vals
	get_normal_vals_list_info(&p_data);

	// Add elements and models info
	get_elements_and_models_list_info(&p_data);

	// Indicate event to app
	len = p_data - list_copy;
	if (len > USER_CMD_LIST_INFO_MAX_LEN) {
		BT_LOGE("[%s] Len of data(%d) in list info extend max value(%d)\r\n", __func__, (int)len, USER_CMD_LIST_INFO_MAX_LEN);
		len = USER_CMD_LIST_INFO_MAX_LEN;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_LIST_INFO, len);
	memcpy(p_evt->data, list_copy, len);
	rtk_bt_evt_indicate(p_evt, NULL);

	return RTK_BT_MESH_STACK_API_SUCCESS;
}

/*-------------------------- END OF USER CMD LIST INFO ------------------------------------------*/

static uint16_t zephyr_stack_node_reset(void)
{
	bt_mesh_reset();
	bt_mesh_cdb_clear();
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_set_random_value_for_authentication(rtk_bt_mesh_stack_act_set_random_value_t *value)
{
	int err;
	err = bt_mesh_input_number(value->random);
	if (err) {
		BT_LOGE("[%s] Input number fail:%d\r\n", __func__, err);
		return (uint16_t)err;
	}
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_retrans_param_set(rtk_bt_mesh_stack_set_retrans_param_t *param)
{
	uint16_t ret = RTK_BT_MESH_STACK_API_SUCCESS;
	uint8_t xmit;
	uint32_t int_ms;
	int err;

	// Set default ttl
	if (bt_mesh_default_ttl_set(param->ttl)) {
		BT_LOGE("[%s] Set default ttl to %d fail\r\n", __func__, param->ttl);
		ret = RTK_BT_MESH_STACK_API_FAIL;
	}

	// Set relay count
	if (param->relay_retrans_count > 7) {
		BT_LOGE("[%s] Relay retrans count %d > 7, set fail\r\n", __func__, param->relay_retrans_count);
		ret = RTK_BT_MESH_STACK_API_FAIL;
	} else {
		xmit = bt_mesh_relay_retransmit_get();
		int_ms = BT_MESH_TRANSMIT_INT(xmit);
		err = bt_mesh_relay_set(BT_MESH_FEATURE_ENABLED, BT_MESH_TRANSMIT(param->relay_retrans_count, int_ms));
		if (err && err != -EALREADY) {
			BT_LOGE("[%s] Set relay retrans count %d fail\r\n", __func__, param->relay_retrans_count);
			ret = RTK_BT_MESH_STACK_API_FAIL;
		}
	}

	// Set network retrans count
	if (param->net_retrans_count > 7) {
		BT_LOGE("[%s] Network retrans count %d > 7, set fail\r\n", __func__, param->net_retrans_count);
		ret = RTK_BT_MESH_STACK_API_FAIL;
	} else {
		xmit = bt_mesh_net_transmit_get();
		int_ms = BT_MESH_TRANSMIT_INT(xmit);
		bt_mesh_net_transmit_set(BT_MESH_TRANSMIT(param->net_retrans_count, int_ms));
	}

	if (param->trans_retrans_count > 15) {
		BT_LOGE("[%s] Transport retrans count %d > 15, set fail\r\n", __func__, param->trans_retrans_count);
		ret = RTK_BT_MESH_STACK_API_FAIL;
	} else {
		bt_mesh.sar_tx.unicast_retrans_count = param->trans_retrans_count;
		bt_mesh.sar_tx.multicast_retrans_count = param->trans_retrans_count;
		bt_mesh.sar_tx.unicast_retrans_without_prog_count = param->trans_retrans_count;
	}
	if (!ret) {
		rtk_bt_mesh_stack_set_retrans_param_t *result;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_RETRANS_PARAM_SETTING_RESULT, sizeof(rtk_bt_mesh_stack_set_retrans_param_t));
		result = (rtk_bt_mesh_stack_set_retrans_param_t *)p_evt->data;
		result->net_retrans_count = BT_MESH_TRANSMIT_COUNT(bt_mesh_net_transmit_get());
		result->relay_retrans_count = BT_MESH_TRANSMIT_COUNT(bt_mesh_relay_retransmit_get());
		result->ttl = bt_mesh_default_ttl_get();
		result->trans_retrans_count = bt_mesh.sar_tx.unicast_retrans_count;
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	return ret;
}

static uint16_t zephyr_stack_get_heartbeat_subscribe_result(rtk_bt_mesh_hb_sub_t *hb_sub)
{
	struct bt_mesh_hb_sub get;
	bt_mesh_hb_sub_get(&get);
	hb_sub->src = get.src;
	hb_sub->dst = get.dst;
	hb_sub->period = get.period;
	hb_sub->count = get.count;
	hb_sub->min_hops = get.min_hops;
	hb_sub->max_hops = get.max_hops;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
static void friend_established(uint16_t net_idx, uint16_t lpn_addr,
							   uint8_t recv_delay, uint32_t polltimeout)
{
	(void)net_idx;
	(void)recv_delay;
	(void)polltimeout;
	rtk_bt_mesh_stack_evt_fn_t *fn_cb;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_FN_CB, sizeof(rtk_bt_mesh_stack_evt_fn_t));
	fn_cb = (rtk_bt_mesh_stack_evt_fn_t *)p_evt->data;
	fn_cb->lpn_addr = lpn_addr;
	fn_cb->cb_type = RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_SUCCESS;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void friend_terminated(uint16_t net_idx, uint16_t lpn_addr)
{
	(void)net_idx;
	rtk_bt_mesh_stack_evt_fn_t *fn_cb;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_FN_CB, sizeof(rtk_bt_mesh_stack_evt_fn_t));
	fn_cb = (rtk_bt_mesh_stack_evt_fn_t *)p_evt->data;
	fn_cb->lpn_addr = lpn_addr;
	fn_cb->cb_type = RTK_BT_MESH_FN_CB_TYPE_FRND_LOST;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void friend_polled(uint16_t net_idx, uint16_t lpn_addr)
{
	(void)net_idx;
	(void)lpn_addr;
	// For debug test
	// BT_LOGA("[%s] Friend: Poll from 0x%04x\r\n", __func__, lpn_addr);
}

BT_MESH_FRIEND_CB_DEFINE(friend) = {
	.established = friend_established,
	.terminated = friend_terminated,
	.polled = friend_polled,
};

static uint16_t zephyr_stack_fn_init(rtk_bt_mesh_stack_act_fn_init_t *friend_init)
{
	(void)friend_init;
	return RTK_BT_MESH_STACK_API_SUCCESS;
}

static uint16_t zephyr_stack_fn_deinit(void)
{
	return RTK_BT_MESH_STACK_API_SUCCESS;
}
#endif  // RTK_BLE_MESH_FN_SUPPORT

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_stack_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_STACK_API_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("[%s] Error: BLE MESH profile is not initiated\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_STACK_ACT_SET_PROV_PARAM:
		ret = zephyr_stack_set_prov_param(p_cmd->param);
		break;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	case RTK_BT_MESH_STACK_ACT_PB_ADV_CON:
		ret = zephyr_stack_pb_adv_con(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PB_ADV_DISCON:
		ret = zephyr_stack_pb_adv_discon();
		break;
	case RTK_BT_MESH_STACK_ACT_PB_GATT_CON:
		ret = zephyr_stack_pb_gatt_con(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PROV_SERVICE_DISCOVERY:
		ret = zephyr_stack_prov_service_discovery(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PROV_SERVICE_SET_NOTIFY:
		ret = zephyr_stack_prov_service_set_notify(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_DISCOVERY:
		ret = zephyr_stack_proxy_service_discovery(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PROXY_SERVICE_SET_NOTIFY:
		ret = zephyr_stack_proxy_service_set_notify(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_PB_GATT_DISCON:
		ret = zephyr_stack_pb_gatt_discon(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_START_PROV:
		ret = zephyr_stack_start_provisioning(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_METHOD_CHOOSE_FOR_PROV:
		ret = zephyr_stack_method_choose_for_prov(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_DEV_INFO_SWITCH:
		zephyr_stack_set_device_info_display(p_cmd->param);
		ret = RTK_BT_MESH_STACK_API_SUCCESS;
		break;
#endif  // RTK_BLE_MESH_PROVISIONER_SUPPORT
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	case RTK_BT_MESH_STACK_ACT_LPN_INIT:
		ret = zephyr_stack_lpn_init(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_LPN_REQ: {
		ret = zephyr_stack_lpn_req(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_STACK_ACT_LPN_SUB: {
		zephyr_stack_lpn_sub(p_cmd->param);
		ret = RTK_BT_MESH_STACK_API_SUCCESS;
		break;
	}
	case RTK_BT_MESH_STACK_ACT_LPN_CLEAR: {
		ret = zephyr_stack_lpn_clear(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_STACK_ACT_LPN_DEINIT:
		ret = zephyr_stack_lpn_deinit();
		break;
#endif  // RTK_BLE_MESH_LPN_SUPPORT
	case RTK_BT_MESH_STACK_ACT_SET_SERVICE_ADV:
		ret = zephyr_stack_set_service_adv(p_cmd->param);
		break;
#endif  // RTK_BLE_MESH_DEVICE_SUPPORT
	case RTK_BT_MESH_STACK_ACT_USER_LIST_INFO:
		ret = zephyr_stack_list_info();
		break;
	case RTK_BT_MESH_STACK_ACT_NODE_RESET:
		ret = zephyr_stack_node_reset();
		break;
	case RTK_BT_MESH_STACK_ACT_SET_RANDOM_VALUE:
		ret = zephyr_stack_set_random_value_for_authentication(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_SET_RETRANS_PARAM:
		ret = zephyr_stack_retrans_param_set(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_GET_HB_SUB_RESULT:
		ret = zephyr_stack_get_heartbeat_subscribe_result(p_cmd->param);
		break;
#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
	case RTK_BT_MESH_STACK_ACT_FN_INIT:
		ret = zephyr_stack_fn_init(p_cmd->param);
		break;
	case RTK_BT_MESH_STACK_ACT_FN_DEINIT:
		ret = zephyr_stack_fn_deinit();
		break;
#endif  // RTK_BLE_MESH_FN_SUPPORT
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif  // RTK_BLE_MESH_SUPPORT
