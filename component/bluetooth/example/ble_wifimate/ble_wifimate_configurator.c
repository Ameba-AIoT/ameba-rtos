/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
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
#include <ble_wifimate_service.h>
#include <bt_utils.h>
#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#endif

static uint8_t client_attach = false;
static struct ble_wifimate_encrypt_decrypt_t s_encrypt = {0};

static rtk_bt_evt_cb_ret_t ble_wifimate_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
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
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(evt_code)) {
			bt_at_sync_set_result(scan_start_ind->err);
			bt_at_sync_sem_give();
		}
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		const char target_name[13] = {'R', 'T', 'K', '_', 'W', 'i', 'F', 'i', 'M', 'a', 't', 'e', '\0'};
		uint8_t *scan_name = NULL;
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		/* local name end with 0 */
		scan_res_ind->adv_report.data[9 + 19] = 0;
		scan_name = &(scan_res_ind->adv_report.data[9]);
		if (strstr((const char *)scan_name, target_name) != NULL) {
			BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d, adv_data: %s \r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len, &(scan_res_ind->adv_report.data[9]));
			BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%d,%d\r\n",
						le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
						scan_res_ind->adv_report.len);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (scan_stop_ind->stop_reason == RTK_BT_LE_SCAN_STOP_BY_HOST) {
			if (bt_at_sync_event_match_check(evt_code)) {
				bt_at_sync_set_result(scan_stop_ind->err);
				bt_at_sync_sem_give();
			}
		}
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
					conn_ind->conn_handle, role, le_addr);

			/* update encrypt key */
			if (s_encrypt.algorithm_type != BLE_WIFIMATE_ENCRYPT_DECRYPT_ALGO_NONE) {
				s_encrypt.key[BLE_WIFIMATE_ENCRYPT_DECRYPT_KEY_LEN - 3] = conn_ind->peer_addr.addr_val[2];
				s_encrypt.key[BLE_WIFIMATE_ENCRYPT_DECRYPT_KEY_LEN - 2] = conn_ind->peer_addr.addr_val[1];
				s_encrypt.key[BLE_WIFIMATE_ENCRYPT_DECRYPT_KEY_LEN - 1] = conn_ind->peer_addr.addr_val[0];
			}
			if (RTK_BT_OK == ble_wifimate_client_encrypt_set(&s_encrypt)) {
				BT_LOGA("[APP] BLE wifimate client set encrypt success.\r\n");
			}
#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB

			/* when connection established, attach the profile(who has been registered)
			    to this connection */
			if (RTK_BT_OK == ble_wifimate_client_attach_conn(conn_ind->conn_handle)) {
				BT_LOGA("[APP] BLE wifimate client attach connection success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			}
			client_attach = true;
#endif
			if (rtk_bt_le_sm_is_device_bonded(&conn_ind->peer_addr)) {
				BT_LOGA("[APP] Bonded device, start link encryption procedure\r\n");
				rtk_bt_le_sm_start_security(conn_ind->conn_handle);
			}
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (!conn_ind->err) {
			if (conn_ind->role == RTK_BT_LE_ROLE_MASTER) {
				BT_AT_PRINT("+BLEGAP:conn,%d,%s\r\n", conn_ind->conn_handle, le_addr);
			} else if (conn_ind->role == RTK_BT_LE_ROLE_SLAVE) {
				BT_AT_PRINT_INDICATE("+BLEGAP:conn,%d,%s\r\n", conn_ind->conn_handle, le_addr);
			}
		}
		if (conn_ind->role == RTK_BT_LE_ROLE_MASTER) {
			if (bt_at_sync_event_match_check(evt_code)) {
				bt_at_sync_set_result(conn_ind->err);
				bt_at_sync_sem_give();
			}
		}
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		BT_LOGA("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
				disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		if (RTK_BT_OK == ble_wifimate_client_detach_conn(disconn_ind->conn_handle)) {
			BT_LOGA("[APP] GATTC Profiles detach connection success, conn_handle: %d\r\n",
					disconn_ind->conn_handle);
		}
		client_attach = false;

#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (disconn_ind->reason != (RTK_BT_ERR_HCI_GROUP | RTK_BT_HCI_ERR_LOCAL_HOST_TERMINATE)) {
			BT_AT_PRINT_INDICATE("+BLEGAP:disconn,%d,%s\r\n",
								 disconn_ind->conn_handle, le_addr);
		}

		bt_at_sync_disconnect_hdl(disconn_ind->conn_handle);
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind =
			(rtk_bt_le_conn_update_ind_t *)param;
		if (conn_update_ind->err) {
			BT_LOGE("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
					conn_update_ind->conn_handle, conn_update_ind->err);
		} else {
			BT_LOGA("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "       \
					"conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
					conn_update_ind->conn_handle,
					conn_update_ind->conn_interval,
					conn_update_ind->conn_latency,
					conn_update_ind->supv_timeout);
		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(evt_code)) {
			bt_at_sync_set_result(conn_update_ind->err);
			bt_at_sync_sem_give();
		}
#endif
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
		break;
	}

	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind =
			(rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			BT_LOGE("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
					phy_update_ind->conn_handle,
					phy_update_ind->err);
		} else {
			BT_LOGA("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
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
		BT_AT_PRINT_INDICATE("+BLEGAP:pair_cfm,%d\r\n", pair_cfm_ind->conn_handle);
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
		BT_AT_PRINT_INDICATE("+BLEGAP:passkey_display,%d,%d\r\n",
							 (int)key_dis_ind->conn_handle,
							 (int)key_dis_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
		rtk_bt_le_auth_key_input_ind_t *key_input_ind =
			(rtk_bt_le_auth_key_input_ind_t *)param;
		BT_LOGA("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
				key_input_ind->conn_handle);
		BT_AT_PRINT_INDICATE("+BLEGAP:passkey_input,%d\r\n", key_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
		rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind =
			(rtk_bt_le_auth_key_cfm_ind_t *)param;
		BT_LOGA("[APP] Auth passkey confirm: %d, conn_handle: %d. "  \
				"Please comfirm if the passkeys are equal!\r\n",
				key_cfm_ind->passkey,
				key_cfm_ind->conn_handle);
		BT_AT_PRINT_INDICATE("+BLEGAP:passkey_cfm,%d,%d\r\n",
							 (int)key_cfm_ind->conn_handle,
							 (int)key_cfm_ind->passkey);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
		rtk_bt_le_auth_oob_input_ind_t *oob_input_ind =
			(rtk_bt_le_auth_oob_input_ind_t *)param;
		BT_LOGA("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
				oob_input_ind->conn_handle);
		BT_AT_PRINT_INDICATE("+BLEGAP:oobkey_input,%d\r\n", oob_input_ind->conn_handle);
		break;
	}

	case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
		rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
			(rtk_bt_le_auth_complete_ind_t *)param;
		BT_AT_PRINT_INDICATE("+BLEGAP:sec,%d,%d\r\n",
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

#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if ((bond_mdf_ind->op == RTK_BT_LE_BOND_DELETE) ||
			(bond_mdf_ind->op == RTK_BT_LE_BOND_CLEAR)) {
			if (bt_at_sync_event_match_check(evt_code)) {
				bt_at_sync_set_result(BT_AT_EVT_RESULT_SUCCESS);
				bt_at_sync_sem_give();
			}
		}
#endif
		break;
	}

	default:
		BT_LOGE("[APP] Unkown gap cb evt type: %d\r\n", evt_code);
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

uint16_t ble_wifimate_configurator_encrypt_set(uint8_t type, uint8_t key[BLE_WIFIMATE_KEY_LEN])
{
	if (!key) {
		BT_LOGE("[APP] BLE WifiMate configurator encrypt key invalid.\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (client_attach) {
		BT_LOGE("[APP] BLE WifiMate configurator encrypt inuse, can't set now.\r\n");
		return RTK_BT_ERR_ALREADY_IN_PROGRESS;
	}

	s_encrypt.algorithm_type = type;
	memcpy(s_encrypt.key, key, BLE_WIFIMATE_KEY_LEN);

	return RTK_BT_OK;
}


int ble_wifimate_configurator_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	bt_at_sync_enable(enable);
#endif

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTC;
		bt_app_conf.mtu_size = 512;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.slave_init_mtu_req = false;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
		bt_app_conf.user_def_service = false;
		bt_app_conf.cccd_not_check = false;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_le_gap_set_scan_param(&scan_param));
		/* Initilize GAP part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_wifimate_gap_app_callback));

		BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));

		/* Initilize GATT (client) part */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC, ble_wifimate_gattc_app_callback));
		BT_APP_PROCESS(ble_wifimate_client_add());

	} else if (0 == enable) {
		client_attach = false;
		memset(&s_encrypt, 0, sizeof(s_encrypt));

		ble_wifimate_client_deinit();

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}
