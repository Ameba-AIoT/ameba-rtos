/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_device.h>

#include <rtk_client_config.h>
#include <rtk_gcs_client.h>
#include <rtk_gaps_client.h>
#include <rtk_bas_client.h>
#include <rtk_simple_ble_client.h>
#include <rtk_cte_client.h>
#include <bt_utils.h>


#if defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT
#define BT_POWER_TEST_MODE         0
#if defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE
#include "rtk_bt_power_control.h"

#define BT_POWER_TEST_WAKE_TIME    5    //Unit:s

static void *bt_power_test_wake_timer_hdl = NULL;

static void bt_power_test_wake_timeout_handler(void *arg)
{
	(void)arg;
	rtk_bt_release_wakelock();
}

static void bt_power_test_suspend(void)
{
	BT_LOGA("[BT_PS] Enter bt_power_test_suspend\r\n");
}

static void bt_power_test_resume(void)
{
	BT_LOGA("[BT_PS] Enter bt_power_test_resume\r\n");

	if (BT_POWER_TEST_WAKE_TIME != 0) {
		osif_timer_restart(&bt_power_test_wake_timer_hdl, BT_POWER_TEST_WAKE_TIME * 1000);
	} else {
		rtk_bt_release_wakelock();
	}
}

static void bt_power_test_init(void)
{
	if (BT_POWER_TEST_WAKE_TIME != 0) {
		osif_timer_create(&bt_power_test_wake_timer_hdl, "bt_power_test_wake_timer", NULL, BT_POWER_TEST_WAKE_TIME * 1000, false,
						  bt_power_test_wake_timeout_handler);
		if (bt_power_test_wake_timer_hdl == NULL) {
			BT_LOGE("[BT_PS] bt_power_test_wake_timer create failed!\r\n");
			return;
		}
	}

	rtk_bt_power_save_init((rtk_bt_ps_callback)bt_power_test_suspend, (rtk_bt_ps_callback)bt_power_test_resume);
}

static void bt_power_test_deinit(void)
{
	rtk_bt_power_save_deinit();

	if (BT_POWER_TEST_WAKE_TIME != 0) {
		if (bt_power_test_wake_timer_hdl) {
			osif_timer_delete(&bt_power_test_wake_timer_hdl);
			bt_power_test_wake_timer_hdl = NULL;
		}
	}
}
#endif
#endif

static rtk_bt_evt_cb_ret_t central_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)param;
	(void)len;
	rtk_bt_evt_cb_ret_t ret = RTK_BT_EVT_CB_OK;

	switch (evt_code) {
#if defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT
	case RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND: {
		char addr_str[40] = {0};
		rtk_bt_ecfc_conn_cmpl_ind_t *p_ind = (rtk_bt_ecfc_conn_cmpl_ind_t *)param;
		rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
		if (p_ind->err) {
			BT_LOGE("[APP] ECFC connection established fail(0x%x). %s\r\n", p_ind->err, addr_str);
		} else {
			BT_LOGA("[APP] ECFC connection established OK. %s\r\n", addr_str);
			BT_LOGA("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) local_mtu(%d) local_mps(%d)\r\n",
					p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DATA_IND: {
		rtk_bt_ecfc_data_ind_t *p_ind = (rtk_bt_ecfc_data_ind_t *)param;
		BT_LOGA("[APP] ECFC data received. proto_id(%d) conn_handle(%d) cid(%d) length(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->length);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND: {
		char addr_str[40] = {0};
		rtk_bt_ecfc_conn_req_ind_t *p_ind = (rtk_bt_ecfc_conn_req_ind_t *)param;
		rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
		BT_LOGA("[APP] ECFC connection requset. %s\r\n", addr_str);
		BT_LOGA("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) identity_id(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->identity_id);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_DISCONN_IND: {
		rtk_bt_ecfc_disconn_ind_t *p_ind = (rtk_bt_ecfc_disconn_ind_t *)param;
		BT_LOGA("[APP] ECFC disconnect. proto_id(%d) conn_handle(%d) cid(%d) cause(0x%x)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->cause);
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_REQ_IND: { //BT api shall not be called here
		rtk_bt_ecfc_reconf_req_ind_t *p_ind = (rtk_bt_ecfc_reconf_req_ind_t *)param;
		BT_LOGA("[APP] ECFC reconfigure requset. proto_id(%d) conn_handle(%d) remote_mtu(%d) remote_mps(%d)\r\n",
				p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->remote_mps);
		BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		if (p_ind->remote_mtu < RTK_BT_GAP_ECFC_MIN_MTU || p_ind->remote_mps < RTK_BT_GAP_ECFC_MIN_MPS) {
			ret = RTK_BT_EVT_CB_REJECT;
			BT_LOGE("[APP] Reject!\r\n");
		} else {
			ret = RTK_BT_EVT_CB_OK;
			BT_LOGA("[APP] Accept!\r\n");
		}
		break;
	}
	case RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND: {
		rtk_bt_ecfc_reconf_rsp_ind_t *p_ind = (rtk_bt_ecfc_reconf_rsp_ind_t *)param;
		if (p_ind->cause) {
			BT_LOGE("[APP] ECFC reconfigure fail\r\n");
		} else {
			BT_LOGA("[APP] ECFC reconfigure OK, local_mtu(%d) local mps(%d)\r\n",
					p_ind->local_mtu, p_ind->local_mps);
			BT_DUMP16A("[APP] cid: ", p_ind->cid, p_ind->cid_num);
		}
		break;
	}
#endif
	default:
		BT_LOGE("[APP] Unkown common gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return ret;
}

static rtk_bt_evt_cb_ret_t ble_central_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
		if (!scan_start_ind->err) {
			BT_LOGA("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		} else {
			BT_LOGE("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %i, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%i,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
		break;
	}

#if defined(RTK_BLE_5_0_AE_SCAN_SUPPORT) && RTK_BLE_5_0_AE_SCAN_SUPPORT
	case RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND: {
		rtk_bt_le_ext_scan_res_ind_t *scan_res_ind = (rtk_bt_le_ext_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %i, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%i,0x%x,%d,%d\r\n",
					le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
					(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
					scan_res_ind->tx_power, scan_res_ind->len);
		break;
	}
#endif

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
					conn_ind->conn_handle, role, le_addr);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
			/* when connection established, attach the profile(who has been registered)
			    to this connection */
			if (RTK_BT_OK == general_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == bas_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == gaps_client_attach_conn(conn_ind->conn_handle) &&
				RTK_BT_OK == simple_ble_client_attach_conn(conn_ind->conn_handle)
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
				&& RTK_BT_OK == cte_client_attach_conn(conn_ind->conn_handle)
#endif
			   ) {
				BT_LOGA("[APP] GATTC Profiles attach connection success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			}
#endif
			if (rtk_bt_le_sm_is_device_bonded(&conn_ind->peer_addr)) {
				BT_LOGA("[APP] Bonded device, start link encryption procedure\r\n");
				rtk_bt_le_sm_start_security(conn_ind->conn_handle);
			}
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
				disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		if (RTK_BT_OK == general_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == bas_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == gaps_client_detach_conn(disconn_ind->conn_handle) &&
			RTK_BT_OK == simple_ble_client_detach_conn(disconn_ind->conn_handle)
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
			&& RTK_BT_OK == cte_client_detach_conn(disconn_ind->conn_handle)
#endif
		   ) {
			BT_LOGA("[APP] GATTC Profiles detach connection success, conn_handle: %d\r\n",
					disconn_ind->conn_handle);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind =
			(rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			BT_LOGE("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
					conn_update_ind->conn_handle, conn_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,-1\r\n", conn_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "       \
					"conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
					conn_update_ind->conn_handle,
					conn_update_ind->conn_interval,
					conn_update_ind->conn_latency,
					conn_update_ind->supv_timeout);
			BT_AT_PRINT("+BLEGAP:conn_update,%d,0,0x%x,0x%x,0x%x\r\n",
						conn_update_ind->conn_handle,
						conn_update_ind->conn_interval,
						conn_update_ind->conn_latency,
						conn_update_ind->supv_timeout);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT api shall not be called here
		rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req =
			(rtk_bt_le_remote_conn_update_req_ind_t *)param;
		BT_LOGA("[APP] Remote device request a change in conn param, conn_handle: %d, "      \
				"conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "      \
				"timeout: 0x%x. The host stack accept it.\r\n",
				rmt_update_req->conn_handle,
				rmt_update_req->conn_interval_max,
				rmt_update_req->conn_interval_min,
				rmt_update_req->conn_latency,
				rmt_update_req->supv_timeout);
		return RTK_BT_EVT_CB_ACCEPT;
		break;
	}

	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change =
			(rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] Data len is updated, conn_handle: %d, "       \
				"max_tx_octets: 0x%x, max_tx_time: 0x%x, "        \
				"max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
				data_len_change->conn_handle,
				data_len_change->max_tx_octets,
				data_len_change->max_tx_time,
				data_len_change->max_rx_octets,
				data_len_change->max_rx_time);
		BT_AT_PRINT("+BLEGAP:conn_datalen,%d,0x%x,0x%x,0x%x,0x%x\r\n",
					data_len_change->conn_handle,
					data_len_change->max_tx_octets,
					data_len_change->max_tx_time,
					data_len_change->max_rx_octets,
					data_len_change->max_rx_time);
		break;
	}

	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind =
			(rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			BT_LOGE("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
		} else {
			BT_LOGA("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->tx_phy,
					phy_update_ind->rx_phy);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
		rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind =
			(rtk_bt_le_auth_pair_cfm_ind_t *)param;
		BT_LOGA("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
				pair_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
		rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
		uint16_t ret = 0;
		pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
		pair_cfm_param.confirm = 1;
		ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] Just work pairing auto confirm succcess\r\n");
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
		rtk_bt_le_auth_key_display_ind_t *key_dis_ind =
			(rtk_bt_le_auth_key_display_ind_t *)param;
		BT_LOGA("[APP] Auth passkey display: %d, conn_handle:%d\r\n",
				key_dis_ind->passkey,
				key_dis_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_display,%d,%d\r\n",
					(int)key_dis_ind->conn_handle,
					(int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind =
			(rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
				key_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind =
			(rtk_bt_le_auth_key_cfm_ind_t *)param;
		BT_LOGA("[APP] Auth passkey confirm: %d, conn_handle: %d. "  \
				"Please comfirm if the passkeys are equal!\r\n",
				key_cfm_ind->passkey,
				key_cfm_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:passkey_cfm,%d,%d\r\n",
					(int)key_cfm_ind->conn_handle,
					(int)key_cfm_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind =
			(rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
				oob_input_ind->conn_handle);
		BT_AT_PRINT("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
			(rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT("+BLEGAP:sec,%d,%d\r\n",
					auth_cplt_ind->conn_handle,
					(auth_cplt_ind->err == 0) ? 0 : -1);
		if (auth_cplt_ind->err) {
			BT_LOGE("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
					auth_cplt_ind->err, auth_cplt_ind->conn_handle);
		} else {
			BT_LOGA("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
			BT_DUMPHEXA("[APP] long term key is 0x", auth_cplt_ind->dev_ltk, auth_cplt_ind->dev_ltk_length, true);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
		rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
			(rtk_bt_le_bond_modify_ind_t *)param;
		char ident_addr[30] = {0};
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
		rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
		BT_LOGA("[APP] Bond info modified, op: %d, addr: %s, ident_addr: %s\r\n",
				bond_mdf_ind->op, le_addr, ident_addr);
		BT_AT_PRINT("+BLEGAP:bond_modify,%d,%s,%s\r\n", bond_mdf_ind->op, le_addr, ident_addr);

		break;
	}

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	case RTK_BT_LE_GAP_EVT_RESOLV_LIST_MODIFY_IND: {
		rtk_bt_le_modify_resolv_list_ind_t *p_ind = (rtk_bt_le_modify_resolv_list_ind_t *)param;
		rtk_bt_le_addr_t addr;

		if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD || p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_REMOVE) {
			addr.type = (rtk_bt_le_addr_type_t)p_ind->entry.addr_type;
			memcpy(addr.addr_val, p_ind->entry.addr, RTK_BD_ADDR_LEN);
			rtk_bt_le_addr_to_str(&addr, le_addr, sizeof(le_addr));
			if (p_ind->err) {
				BT_LOGE("[APP] Resolving list %s %s fail, cause:%x.\r\n",
						(p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
						le_addr, p_ind->err);
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,-1\r\n", p_ind->op);
			} else {
				BT_LOGA("[APP] Resolving list %s %s success, %s privacy mode.\r\n",
						(p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
						le_addr, p_ind->entry.device_mode ? "device" : "network");
				BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,0,%s,%s\r\n",
							p_ind->op, le_addr, p_ind->entry.device_mode ? "device" : "network");
			}
		} else if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_CLEAR) {
			if (p_ind->err) {
				BT_LOGE("[APP] Resolving list clear fail, cause:%x.\r\n", p_ind->err);
			} else {
				BT_LOGA("[APP] Resolving list clear success.\r\n");
			}
			BT_AT_PRINT("+BLEGAP:resolv_list_modify,%d,%d\r\n", p_ind->op, (p_ind->err == 0) ? 0 : -1);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_RESOLV_LIST_PENDING_IND: {
		rtk_bt_le_resolv_list_pending_ind_t *p_ind = (rtk_bt_le_resolv_list_pending_ind_t *)param;
		BT_LOGA("[APP] WARN: Resolving list modification is pending because of adv(%d) scan(%d) connect(%d)!!!\r\n",
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_ADV) ? 1 : 0,
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_SCAN) ? 1 : 0,
				(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_CONNECT) ? 1 : 0);
		BT_AT_PRINT("+BLEGAP:resolv_list_pending,%d,%d,%d\r\n",
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_ADV) ? 1 : 0,
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_SCAN) ? 1 : 0,
					(p_ind->reason & RTK_BT_LE_RESOLV_LIST_PEND_BY_CONNECT) ? 1 : 0);
		break;
	}
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
	case RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND: {
		rtk_bt_le_pa_sync_ind_t *pa_sync_ind = (rtk_bt_le_pa_sync_ind_t *)param;
		BT_LOGA("[APP] PA sync state change: sync_id: %d, state = %d, cause: 0x%x\r\n",
				pa_sync_ind->sync_id, pa_sync_ind->state, pa_sync_ind->cause);

		if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
			rtk_bt_le_pa_synced_info_t *p_info = &pa_sync_ind->info;
			rtk_bt_le_addr_to_str(&p_info->addr, le_addr, sizeof(le_addr));
			BT_LOGA("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid: %d, past_received: %d\r\n",
					le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
			BT_AT_PRINT("+BLEGAP:pa_sync,%s,0x%x,%d,%d\r\n",
						le_addr, p_info->sync_handle, p_info->adv_sid, p_info->past_received);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND: {
		rtk_bt_le_pa_adv_report_ind_t *pa_report = (rtk_bt_le_pa_adv_report_ind_t *)param;
		BT_LOGA("[APP] PA sync ADV report: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
				pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
				pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		BT_AT_PRINT("+BLEGAP:pa_report,%d,0x%x,%d,%d,%d,0x%x,%d\r\n",
					pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
					pa_report->cte_type, pa_report->data_status, pa_report->data_len);
		break;
	}
#endif

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
	case RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND: {
		rtk_bt_le_txpower_ind_t *txpower_ind = (rtk_bt_le_txpower_ind_t *)param;
		BT_LOGA("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n",
				txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		BT_AT_PRINT("+BLEGAP:txpower_report,%d,%d,%d\r\n",
					txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
		break;
	}
#endif

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	case RTK_BT_LE_GAP_EVT_CONNLESS_IQ_REPORT_IND: {
		rtk_bt_le_gap_connless_iq_report_ind_t *report_ind = (rtk_bt_le_gap_connless_iq_report_ind_t *)param;
		BT_LOGA("[APP] Connectionless IQ report: sync_id %u, sync_handle 0x%x, channel_index %u, "       \
				"rssi %d, rssi_antenna_id %u, cte_type %d, "     \
				"slot_durations %d, packet_status %d, periodic_event_counter %d, "       \
				"sample_count %d \r\n",
				report_ind->sync_id,
				report_ind->sync_handle,
				report_ind->channel_index,
				report_ind->rssi,
				report_ind->rssi_antenna_id,
				report_ind->cte_type,
				report_ind->slot_durations,
				report_ind->packet_status,
				report_ind->periodic_event_counter,
				report_ind->sample_count);
		BT_AT_PRINT("+BLEGAP:cte_connless_report,%u,0x%x,%u,%d,%u,%d,%d,%d,%d,%d\r\n",
					report_ind->sync_id,
					report_ind->sync_handle,
					report_ind->channel_index,
					report_ind->rssi,
					report_ind->rssi_antenna_id,
					report_ind->cte_type,
					report_ind->slot_durations,
					report_ind->packet_status,
					report_ind->periodic_event_counter,
					report_ind->sample_count);

		BT_DUMPA("[APP] Connectionless IQ sample: ", report_ind->iq_sample, report_ind->sample_count * 2);
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_IQ_REPORT_IND: {
		rtk_bt_le_gap_conn_iq_report_ind_t *report_ind = (rtk_bt_le_gap_conn_iq_report_ind_t *)param;
		BT_LOGA("[APP] Connection IQ report: conn_handle %d, rx_phy %d, data_chan_index %d, "        \
				"rssi %d, rssi_antenna_id %d, cte_type %d, "     \
				"slot_durations %d, packet_status %d, connection_event_counter %d, "     \
				"sample_count %d\r\n",
				report_ind->conn_handle,
				report_ind->rx_phy,
				report_ind->data_chan_index,
				report_ind->rssi,
				report_ind->rssi_antenna_id,
				report_ind->cte_type,
				report_ind->slot_durations,
				report_ind->packet_status,
				report_ind->connection_event_counter,
				report_ind->sample_count);
		BT_AT_PRINT("+BLEGAP:cte_conn_report,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					report_ind->conn_handle,
					report_ind->rx_phy,
					report_ind->data_chan_index,
					report_ind->rssi,
					report_ind->rssi_antenna_id,
					report_ind->cte_type,
					report_ind->slot_durations,
					report_ind->packet_status,
					report_ind->connection_event_counter,
					report_ind->sample_count);

		BT_DUMPA("[APP] Connection IQ sample: ", report_ind->iq_sample, report_ind->sample_count * 2);
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_CTE_REQ_FAIL_IND: {
		rtk_bt_le_gap_cte_req_fail_ind_t *fail_ind = (rtk_bt_le_gap_cte_req_fail_ind_t *)param;
		BT_LOGE("[APP] CTE request fail conn_handle %u, cause %u\r\n", fail_ind->conn_handle, fail_ind->cause);
		BT_AT_PRINT("+BLEGAP:cte_conn_rx,%d,fail\r\n", fail_ind->conn_handle);
		break;
	}
#endif

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	case RTK_BT_LE_GAP_EVT_COC_CONNECT_IND: {
		rtk_bt_le_coc_conn_state_ind_t *coc_conn_ind = (rtk_bt_le_coc_conn_state_ind_t *)param;
		if (!coc_conn_ind->err) {
			BT_LOGA("[APP] LE COC connected, conn_handle: %d, cid: 0x%x\r\n",
					coc_conn_ind->conn_handle, coc_conn_ind->cid);
		} else {
			BT_LOGE("[APP] LE COC connect failed, conn_hande: %d, cid: 0x%x, err: 0x%x\r\n",
					coc_conn_ind->conn_handle, coc_conn_ind->cid, coc_conn_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:coc_conn,%d,0x%x,%d\r\n", coc_conn_ind->conn_handle, coc_conn_ind->cid,
					(coc_conn_ind->err == 0) ? 0 : -1);
		break;
	}

	case RTK_BT_LE_GAP_EVT_COC_DISCONNECT_IND: {
		rtk_bt_le_coc_conn_state_ind_t *coc_disconn_ind = (rtk_bt_le_coc_conn_state_ind_t *)param;
		if (!coc_disconn_ind->err) {
			BT_LOGA("[APP] LE COC disconnected, conn_handle: %d, cid: 0x%x\r\n",
					coc_disconn_ind->conn_handle, coc_disconn_ind->cid);
		} else {
			BT_LOGE("[APP] LE COC disconnect failed, conn_hande: %d, cid: 0x%x, err: 0x%x\r\n",
					coc_disconn_ind->conn_handle, coc_disconn_ind->cid, coc_disconn_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:coc_disconn,%d,0x%x,%d\r\n", coc_disconn_ind->conn_handle,
					coc_disconn_ind->cid, (coc_disconn_ind->err == 0) ? 0 : -1);
		break;
	}

	case RTK_BT_LE_GAP_EVT_COC_SEND_DATA_RESULT_IND: {
		rtk_bt_le_coc_send_data_res_ind_t *res_ind = (rtk_bt_le_coc_send_data_res_ind_t *)param;
		BT_LOGA("[APP] LE COC send data completed, conn_handle: %d, cid: 0x%x, credit: %d, err: 0x%x\r\n",
				res_ind->conn_handle, res_ind->cid, res_ind->credit, res_ind->err);
		BT_AT_PRINT("+BLEGAP:coc_send,%d,0x%x,%d,%d\r\n", res_ind->conn_handle, res_ind->cid,
					res_ind->credit, (res_ind->err == 0) ? 0 : -1);
		break;
	}

	case RTK_BT_LE_GAP_EVT_COC_RECEIVE_DATA_IND: {
		rtk_bt_le_coc_receive_data_ind_t *data_ind = (rtk_bt_le_coc_receive_data_ind_t *)param;
		BT_LOGA("[APP] LE COC receive data, conn_handle: %d, cid: 0x%x, len: %d\r\n",
				data_ind->conn_handle, data_ind->cid, data_ind->len);
		BT_DUMPA("[HEX]: ", data_ind->data, data_ind->len);
		BT_AT_PRINT("+BLEGAP:coc_recv,%d,0x%x\r\n", data_ind->conn_handle, data_ind->cid);
		break;
	}
#endif /* RTK_BLE_COC_SUPPORT */

	default:
		BT_LOGE("[APP] Unkown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gattc_profile_id(uint8_t event, void *data)
{
	uint16_t profile_id = 0xFFFF;

	switch (event) {
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND: {
		rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
		profile_id = disc_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_READ_RESULT_IND: {
		rtk_bt_gattc_read_ind_t *read_res = (rtk_bt_gattc_read_ind_t *)data;
		profile_id = read_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND: {
		rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
		profile_id = write_res->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_NOTIFY_IND:
	case RTK_BT_GATTC_EVT_INDICATE_IND: {
		rtk_bt_gattc_cccd_value_ind_t *cccd_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
		profile_id = cccd_ind->profile_id;
		break;
	}
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND: {
		rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
		profile_id = cccd_update->profile_id;
		break;
	}
	default:
		break;
	}

	return profile_id;
}

static rtk_bt_evt_cb_ret_t ble_central_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t profile_id = 0xFFFF;

	if (RTK_BT_GATTC_EVT_MTU_EXCHANGE == event) {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTC mtu exchange fail \r\n");
		}
		return RTK_BT_EVT_CB_OK;
	}

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	if (RTK_BT_GATTC_EVT_DISCOVER_ALL_STATE_IND == event) {
		rtk_bt_gattc_discover_all_state_ind_t *p_ind = (rtk_bt_gattc_discover_all_state_ind_t *)data;
		BT_LOGA("[APP] GATTC discover all finished: conn_handle: %d, is_success: %d, load_from_storage: %d\r\n",
				p_ind->conn_handle, p_ind->is_success, p_ind->load_from_storage);

		return RTK_BT_EVT_CB_OK;
	}

	if (RTK_BT_GATTC_EVT_GATT_SERVICE_INFO_IND == event) {
		rtk_bt_gattc_gatt_service_info_ind_t *p_ind = (rtk_bt_gattc_gatt_service_info_ind_t *)data;
		BT_LOGA("[APP] GATTC discover GATT service info: conn_handle: %d, is_found: %d, load_from_storage: %d\r\n",
				p_ind->conn_handle, p_ind->is_found, p_ind->load_from_storage);
		if ((p_ind->char_flag & RTK_BT_GATT_SVC_SERVER_SUPPORTED_FEATURES_FLAG) &&
			(p_ind->server_features[0] & RTK_BT_GATTS_SERVER_SUPPORTED_FEATURES_EATT_BIT)) {
			BT_LOGA("[APP] Server supported features: 0x%02x, Remote server supports EATT.\r\n", p_ind->server_features[0]);
		}

		return RTK_BT_EVT_CB_OK;
	}
#endif

	profile_id = app_get_gattc_profile_id(event, data);
	switch (profile_id) {
	case GCS_CLIENT_PROFILE_ID:
		general_client_app_callback(event, data);
		break;
	case GAPS_CLIENT_PROFILE_ID:
		gaps_client_app_callback(event, data);
		break;
	case BAS_CLIENT_PROFILE_ID:
		bas_client_app_callback(event, data);
		break;
	case SIMPLE_BLE_CLIENT_PROFILE_ID:
		simple_ble_client_app_callback(event, data);
		break;
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	case CTE_CLIENT_PROFILE_ID:
		cte_client_app_callback(event, data);
		break;
#endif
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_le_scan_param_t scan_param = {
	.type          = RTK_BT_LE_SCAN_TYPE_ACTIVE,
	.interval      = 0x60,
	.window        = 0x30,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL,
	.duplicate_opt = 0,
};

static rtk_bt_le_security_param_t sec_param = {
	.io_cap = RTK_IO_CAP_NO_IN_NO_OUT,
	.oob_data_flag = 0,
	.bond_flag = 1,
	.mitm_flag = 0,
	.sec_pair_flag = 0,
	.use_fixed_key = 0,
	.fixed_key = 000000,
};

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
static bool privacy_enable = false;
static bool privacy_whitelist = true;
static uint8_t privacy_irk[RTK_BT_LE_GAP_IRK_LEN] = "0123456789abcdef";
#endif

int ble_central_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};

	if (1 == enable || 2 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.slave_init_mtu_req = false;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		memcpy(bt_app_conf.irk, privacy_irk, RTK_BT_LE_GAP_IRK_LEN);
#endif
		bt_app_conf.user_def_service = false;
		bt_app_conf.cccd_not_check = false;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&scan_param));
		/* Initilize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, central_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_central_gap_app_callback));

		BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		if (privacy_enable) {
			BT_APP_PROCESS(rtk_bt_le_gap_privacy_init(privacy_whitelist));
		}
#endif

		/* Initilize GATT (client) part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, ble_central_gattc_app_callback));
		BT_APP_PROCESS(general_client_add());
		BT_APP_PROCESS(bas_client_add());
		BT_APP_PROCESS(gaps_client_add());
		BT_APP_PROCESS(simple_ble_client_add());
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
		BT_APP_PROCESS(cte_client_add());
#endif

#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
		bt_power_test_init();
#endif
	} else if (0 == enable) {
#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
		bt_power_test_deinit();
#endif
		BT_APP_PROCESS(general_client_delete());
		BT_APP_PROCESS(bas_client_delete());
		BT_APP_PROCESS(gaps_client_delete());
		BT_APP_PROCESS(simple_ble_client_delete());
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
		BT_APP_PROCESS(cte_client_delete());
#endif

		/* no need to unreg callback here, it is done in rtk_bt_disable */
		// BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GAP));
		// BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTC));

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}
