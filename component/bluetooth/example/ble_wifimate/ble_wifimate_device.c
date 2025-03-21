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

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
#define PRIVACY_USE_DIR_ADV_WHEN_BONDED    0
static bool privacy_enable = false;
static bool privacy_whitelist = true;
#endif

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
		BT_AT_PRINT("+BLEGAP:adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,stop,%d,0x%x\r\n", (adv_stop_ind->err == 0) ? 0 : -1, adv_stop_ind->stop_reason);
		break;
	}

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
		} else {
			BT_LOGE("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
					conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);

		if (RTK_BT_LE_ROLE_SLAVE == conn_ind->role) {
			ble_wifimate_server_connect(conn_ind->conn_handle);
		}

		if (s_ble_wifimate_timer_hdl) {
			osif_timer_delete(&s_ble_wifimate_timer_hdl);
			s_ble_wifimate_timer_hdl = NULL;
			BT_LOGA("[APP] ble wifimate stop timer\r\n");
		}
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

		if (RTK_BT_LE_ROLE_SLAVE == disconn_ind->role) {
			/* gap action */
#if !defined(RTK_BLE_5_0_USE_EXTENDED_ADV) || !RTK_BLE_5_0_USE_EXTENDED_ADV
			rtk_bt_le_gap_dev_state_t dev_state;
			rtk_bt_le_adv_param_t adv_param = {0};
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
				if (privacy_enable) {
					uint8_t bond_size = 0;
					adv_param.own_addr_type = 2;
					BT_APP_PROCESS(rtk_bt_le_sm_get_bond_num(&bond_size));
					if (bond_size != 0) {
#if defined(PRIVACY_USE_DIR_ADV_WHEN_BONDED) && PRIVACY_USE_DIR_ADV_WHEN_BONDED
						rtk_bt_le_bond_info_t bond_info = {0};
						uint8_t bond_num = 1;
						rtk_bt_le_sm_get_bond_info(&bond_info, &bond_num);
						adv_param.type = RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW;
						adv_param.peer_addr.type = 0;
						memcpy(adv_param.peer_addr.addr_val, bond_info.ident_addr.addr_val, RTK_BD_ADDR_LEN);
						adv_param.own_addr_type = 2;
#endif /* PRIVACY_USE_DIR_ADV_WHEN_BONDED */
						if (privacy_whitelist) {
							adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
						} else {
							adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
						}
					}
				}
#endif /* RTK_BLE_PRIVACY_SUPPORT */
				BT_LOGA("[APP] Reconnect ADV starting, adv type:%d,  own_addr_type: %d, filter_policy: %d\r\n"
						, adv_param.type,  adv_param.own_addr_type, adv_param.filter_policy);
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
			}
#endif /* RTK_BLE_5_0_USE_EXTENDED_ADV */
			/* gatts action */
			ble_wifimate_server_disconnect(disconn_ind->conn_handle);
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


int ble_wifimate_device_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	bool adv_filter_whitelist = false;
	char addr_str[30] = {0};
	uint8_t device_name[] = "RTK_WiFiMate_xxyyzz";
	rtk_bt_le_adv_param_t adv_param = {0};

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	uint8_t bond_size = 0;
#endif

	if (1 == enable) {

		if (s_ble_wifmate_enable) {
			BT_LOGA("[APP] ble wifimate has already started, exit\r\n");
			return RTK_BT_ERR_BUSY;
		}
		s_ble_wifmate_enable = true;

		osif_timer_create(&s_ble_wifimate_timer_hdl, "ble_wifimate_start_timer", NULL, 30 * 60 * 1000, false,
						  ble_wifimate_timeout_handler);
		if (s_ble_wifimate_timer_hdl == NULL) {
			BT_LOGE("[APP] ble wifimate create timer failed!\r\n");
			return RTK_BT_ERR_OS_OPERATION;
		} else {
			BT_LOGA("[APP] ble wifimate timer start succeed!\r\n");
			osif_timer_start(&s_ble_wifimate_timer_hdl);
		}

		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
		bt_app_conf.mtu_size = 180;
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

#if !defined(RTK_BLE_5_0_USE_EXTENDED_ADV) || !RTK_BLE_5_0_USE_EXTENDED_ADV
		memcpy(&adv_param, &def_adv_param, sizeof(rtk_bt_le_adv_param_t));
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
		if (privacy_enable) {
			BT_APP_PROCESS(rtk_bt_le_gap_privacy_init(privacy_whitelist));
#if !defined(RTK_BLE_5_0_USE_EXTENDED_ADV) || !RTK_BLE_5_0_USE_EXTENDED_ADV
			/* If privacy on, default use RPA adv, even not bonded */
			adv_param.own_addr_type = 2;
#endif
			BT_APP_PROCESS(rtk_bt_le_sm_get_bond_num(&bond_size));
			if (bond_size != 0) {
#if (defined(PRIVACY_USE_DIR_ADV_WHEN_BONDED) && PRIVACY_USE_DIR_ADV_WHEN_BONDED) && (!defined(RTK_BLE_5_0_USE_EXTENDED_ADV) || !RTK_BLE_5_0_USE_EXTENDED_ADV)
				rtk_bt_le_bond_info_t bond_info = {0};
				uint8_t bond_num = 1;
				rtk_bt_le_sm_get_bond_info(&bond_info, &bond_num);
				adv_param.type = RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW;
				adv_param.peer_addr.type = 0;
				memcpy(adv_param.peer_addr.addr_val, bond_info.ident_addr.addr_val, RTK_BD_ADDR_LEN);
				adv_param.own_addr_type = 2;
#endif
				if (privacy_whitelist) {
					adv_filter_whitelist = true;
				}
			}
		}
#endif /* RTK_BLE_PRIVACY_SUPPORT */

		/* gatts related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS,
													ble_wifimate_gatts_app_callback));
		BT_APP_PROCESS(ble_wifimate_server_add());

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
		if (adv_filter_whitelist) {
			ext_adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
		}
		BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&ext_adv_param, &adv_handle));
		BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(adv_handle, ext_adv_data, sizeof(ext_adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(adv_handle, 0, 0));
#else
		BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(adv_data, sizeof(adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(scan_rsp_data, sizeof(scan_rsp_data)));
		if (adv_filter_whitelist) {
			adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
		}
		BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
#endif

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
