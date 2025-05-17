/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>

#include <rtk_service_config.h>
#include <rtk_client_config.h>
#include <ble_wifimate_service.h>
#include <bt_utils.h>
#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#endif

static uint8_t adv_data[] = {
	/* Flags */
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	/* Service */
	0x03,
	RTK_BT_LE_GAP_ADTYPE_16BIT_COMPLETE,
	LO_WORD(BLE_WIFIMATE_UUID_SRV),
	HI_WORD(BLE_WIFIMATE_UUID_SRV),
	/* Local name */
	0x14,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'W', 'i', 'F', 'i', 'M', 'a', 't', 'e', '_', 'x', 'x', 'y', 'y', 'z', 'z',
};

static uint8_t scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

static rtk_bt_le_adv_param_t def_adv_param = {
	.interval_min = 200, //units of 0.625ms
	.interval_max = 250,
	.type = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t s_ble_wifmate_enable = false;

static void *s_ble_wifimate_timer_hdl = NULL;

static rtk_bt_evt_cb_ret_t ble_wifimate_device_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP] ADV started: adv_type %d  \r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP] ADV start failed, err 0x%x \r\n", adv_start_ind->err);
		}
		if (s_ble_wifimate_timer_hdl) {
			BT_LOGA("[APP] ble wifimate timer start succeed!\r\n");
			osif_timer_start(&s_ble_wifimate_timer_hdl);
		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(evt_code)) {
			bt_at_sync_set_result(adv_start_ind->err);
			bt_at_sync_sem_give();
		}
#endif
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (adv_stop_ind->stop_reason == RTK_BT_LE_ADV_STOP_BY_HOST) {
			if (bt_at_sync_event_match_check(evt_code)) {
				bt_at_sync_set_result(adv_stop_ind->err);
				bt_at_sync_sem_give();
			}
		}
#endif
		break;
	}

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
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%d,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
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
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}

		if (RTK_BT_LE_ROLE_SLAVE == conn_ind->role) {
			ble_wifimate_server_connect(conn_ind->conn_handle);
		}

		if (s_ble_wifimate_timer_hdl) {
			osif_timer_stop(&s_ble_wifimate_timer_hdl);
			BT_LOGA("[APP] ble wifimate stop timer\r\n");
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

		if (RTK_BT_LE_ROLE_SLAVE == disconn_ind->role) {
			/* gap action */
			rtk_bt_le_gap_dev_state_t dev_state;
			rtk_bt_le_adv_param_t adv_param = {0};
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
				BT_LOGA("[APP] Reconnect ADV starting, adv type:%d,  own_addr_type: %d, filter_policy: %d\r\n"
						, adv_param.type,  adv_param.own_addr_type, adv_param.filter_policy);
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
			}
			/* gatts action */
			ble_wifimate_server_disconnect(disconn_ind->conn_handle);
		}
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

	default:
		BT_LOGE("[APP] Unkown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

static void ble_wifimate_timeout_handler(void *arg)
{
	(void)arg;
	rtk_bt_le_gap_stop_adv();
	BT_LOGA("[APP] ble wifimate timeout, stop adv...\r\n");
}

int ble_wifimate_device_main(uint8_t enable, uint16_t timeout)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	bool adv_filter_whitelist = false;
	char addr_str[30] = {0};
	uint8_t device_name[] = "RTK_WiFiMate_xxyyzz";
	rtk_bt_le_adv_param_t adv_param = {0};

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	bt_at_sync_enable(enable);
#endif

	if (1 == enable) {

		if (s_ble_wifmate_enable) {
			BT_LOGA("[APP] ble wifimate has already started, exit\r\n");
			return RTK_BT_ERR_BUSY;
		}
		s_ble_wifmate_enable = true;

		osif_timer_create(&s_ble_wifimate_timer_hdl, "ble_wifimate_start_timer", NULL, timeout * 1000, false,
						  ble_wifimate_timeout_handler);
		if (s_ble_wifimate_timer_hdl == NULL) {
			BT_LOGE("[APP] ble wifimate create timer failed!\r\n");
			return RTK_BT_ERR_OS_OPERATION;
		}

		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
		bt_app_conf.mtu_size = 512;
		bt_app_conf.master_init_mtu_req = false;
		bt_app_conf.slave_init_mtu_req = true;
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

		sprintf((char *)device_name, "RTK_WiFiMate_%02X%02X%02X", bd_addr.addr_val[2], bd_addr.addr_val[1], bd_addr.addr_val[0]);
		memcpy(adv_data + 9, device_name, strlen((char const *)device_name));

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP,
													ble_wifimate_device_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_le_gap_set_device_name(device_name));
		BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN));


		memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));

		/* gatts related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS,
													ble_wifimate_gatts_app_callback));
		BT_APP_PROCESS(ble_wifimate_server_add());

		BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(adv_data, sizeof(adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(scan_rsp_data, sizeof(scan_rsp_data)));
		if (adv_filter_whitelist) {
			adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
		}
		BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));

	} else if (0 == enable) {

		if (!s_ble_wifmate_enable) {
			BT_LOGA("[APP] ble wifimate has already stopped, exit\r\n");
			return RTK_BT_ERR_BUSY;
		}
		s_ble_wifmate_enable = false;
		/* delete timer */
		if (s_ble_wifimate_timer_hdl) {
			osif_timer_delete(&s_ble_wifimate_timer_hdl);
			s_ble_wifimate_timer_hdl = NULL;
		}

		ble_wifimate_server_deinit();

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}
