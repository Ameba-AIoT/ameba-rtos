#include <stdio.h>
#include "nan_event.h"
#include "rtw_nan_cmd_api.h"

extern RTK_BT_APP_NAN_CB_FUNC_T app_nan_cb;
enum ble_role g_role;
bool wlan_nan_act;

void on_nan_notify_event(const char *bdAddr,
						 uint8_t ble_role,
						 uint8_t data_link_id,
						 uint8_t *srvc_name,
						 int8_t nan_trigger)
{

	if (data_link_id != data_link_id_nan) {
		return;
	}

	if (ble_role != role_scanner) {
		INFO_PRINT("[rtw_ble] %s: role(%d) not support\n", __func__, ble_role);
		return;
	}

	if (nan_trigger) {
		if (wlan_nan_act) {
			DEBUG_PRINT("[rtw_ble] %s: skip nan-trigger, wlan_nan_act(%d)\n",
						__func__, wlan_nan_act);
			return;
		}
		INFO_PRINT("[rtw_ble] %s: send trigger evt\n", __func__);
		rtw_nan_evt_send(NAN_EVT_BLE_RECV_TIGGER, NULL, 0);
		wlan_nan_act = true;
	} else {
		if (!wlan_nan_act) {
			DEBUG_PRINT("[rtw_ble] %s: skip nan-terminate, wlan_nan_act(%d)\n",
						__func__, wlan_nan_act);
			return;
		}
		INFO_PRINT("[rtw_ble] %s: send terminate evt\n", __func__);
		rtw_nan_evt_send(NAN_EVT_BLE_RECV_TERMINATE, NULL, 0);
		wlan_nan_act = false;
	}
}

void on_nan_termination()
{
	return;
}

RTW_RET_STATUS rtw_nan_api_ble_init(enum ble_role role)
{
	if (role != role_advertiser && role != role_scanner) {
		INFO_PRINT("[rtw_ble] %s: role(%d) not support\n", __func__, role);
		return RTW_RET_STATUS_INVALID_INPUT;
	}
	wlan_nan_act = false;
	g_role = role;
	NANSetCallback(app_nan_cb);
	if (stack_init()) {
		return RTW_RET_STATUS_ERROR;
	}
	get_dev_addr();

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_ble_deinit()
{
	if (stack_deinit()) {
		return RTW_RET_STATUS_ERROR;
	}
	return RTW_RET_STATUS_SUCCESS;
}
#if 0
RTW_RET_STATUS rtw_nan_api_ble_start_tds_srvc(char *service)
{
	if (g_role > role_browser || !service) {
		INFO_PRINT("[rtw_ble] %s: role(%d) not support tds srvc or service is null\n", __func__, g_role);
		return RTW_RET_STATUS_INVALID_INPUT;
	}
	if (!add_ble_tds(g_role, data_link_id_nan, 3, service)) {
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	return RTW_RET_STATUS_SUCCESS;
}
#endif
RTW_RET_STATUS rtw_nan_api_ble_start_ble_trigger(uint8_t trigger_nan)
{
	if (g_role != role_advertiser && g_role != role_scanner) {
		INFO_PRINT("[rtw_ble] %s: role(%d) not support ble-trigger\n", __func__, g_role);
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	if (trigger_nan) {
		add_ble_trigger(g_role, true, 0);
	} else if (g_role == role_advertiser) {
		INFO_PRINT("[rtw_ble] start ble-trigger adv: nan-terminate.\n");
		add_ble_trigger(g_role, false, 0);
	}
	/* else. do nothing if trigger_nan is false and role is scanner */

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_ble_stop()
{
	switch (g_role) {
	case role_provider:
	case role_seeker:
	case role_browser:
		remove_ble_tds();
		break;
	case role_advertiser:
	case role_scanner:
		remove_ble_trigger();
		break;
	default:
		INFO_PRINT("[rtw_ble] %s: role(%d) not handle\n", __func__, g_role);
		break;
	}

	return RTW_RET_STATUS_SUCCESS;
}
