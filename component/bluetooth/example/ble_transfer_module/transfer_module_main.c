/*
 *******************************************************************************
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>

#include <rtk_service_config.h>
#include <rtk_client_config.h>
#include <bt_utils.h>
#include <transfer_module_common.h>

#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#include <bt_types.h>
#endif

#define RTK_BT_DEFAULT_DEV_NAME         "RTK_BT_TRANSFER_MODULE"

static char s_dev_name[TRANSFER_MODULE_DEV_NAME_MAX_LEN] = RTK_BT_DEFAULT_DEV_NAME;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint8_t ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x17,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'T', 'R', 'A', 'N', 'S', 'F', 'E', 'R', '_', 'M', 'O', 'D', 'U', 'L', 'E',
	// Manufacturer Specific Data
	0xc3,
	RTK_BT_LE_GAP_ADTYPE_MANUFACTURER_SPECIFIC,
	0xc0, 0x00,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2,
	0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
	0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4,
	0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9,
	0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
};

static rtk_bt_le_ext_adv_param_t ext_adv_param = {
	.adv_event_prop = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = 320,
	.primary_adv_interval_max = 320,
	.primary_adv_channel_map = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},//;{0x8A, 0xAA, 0xAA, 0x4C, 0xE0, 0x00},
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power = 0x7F,
	.primary_adv_phy = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip = 0,
	.secondary_adv_phy = RTK_BT_LE_PHYS_2M,
	.adv_sid = 0,
};
#else
static uint8_t adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x17,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'T', 'R', 'A', 'N', 'S', 'F', 'E', 'R', '_', 'M', 'O', 'D', 'U', 'L', 'E',
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
#endif

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
#define PRIVACY_USE_DIR_ADV_WHEN_BONDED    0
static bool privacy_enable = false;
static bool privacy_whitelist = true;
static uint8_t privacy_irk[RTK_BT_LE_GAP_IRK_LEN] = "abcdef0123456789";
#endif

uint16_t ble_transfer_module_get_dev_name(uint8_t buf_len, char *buf)
{
	if (!buf) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	if (buf_len < TRANSFER_MODULE_DEV_NAME_MAX_LEN) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(buf, 0, buf_len);
	memcpy(buf, s_dev_name, strlen(s_dev_name));

	return RTK_BT_OK;
}

uint16_t ble_transfer_module_set_dev_name(char *name)
{
	uint16_t ret = RTK_BT_OK;

	if (!name) {
		return RTK_BT_ERR_POINTER_INVALID;
	}

	if (strlen(name) >= TRANSFER_MODULE_DEV_NAME_MAX_LEN) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	memset(s_dev_name, 0, sizeof(s_dev_name));
	memcpy(s_dev_name, name, strlen(name));

	if (rtk_bt_is_enable()) {
		ret = rtk_bt_le_gap_set_device_name((uint8_t *)s_dev_name);
	} else {
		ret = RTK_BT_ERR_NOT_READY;
	}

	return ret;
}

static rtk_bt_evt_cb_ret_t ble_transfer_module_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP] ADV started: adv_type %d  \r\n", adv_start_ind->adv_type);
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
			// if (privacy_enable) {
			//  uint8_t local_rpa[6] = {0};
			//  rtk_bt_le_gap_read_local_resolv_addr(RTK_BT_LE_IDENT_ADDR_PUBLIC, NULL, local_rpa);
			//  rtk_bt_addr_val_to_str(local_rpa, le_addr, sizeof(le_addr));
			//  BT_LOGA("[APP] ADV use local RPA address: %s \r\n", le_addr);
			// }
#endif
		} else {
			BT_LOGE("[APP] ADV start failed, err 0x%x \r\n", adv_start_ind->err);
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

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				BT_LOGA("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				BT_LOGA("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
			}
		} else {
			if (ext_adv_ind->is_start) {
				BT_LOGE("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			} else {
				BT_LOGE("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
			}
		}
		BT_AT_PRINT("+BLEGAP:eadv,%s,%d,%d\r\n",
					ext_adv_ind->is_start ? "start" : "stop",
					(ext_adv_ind->err == 0) ? 0 : -1,
					ext_adv_ind->adv_handle);
		break;
	}
#endif

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
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		BT_AT_PRINT("+BLEGAP:scan,%s,%d,%d,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
#else
		BT_LOGA("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %d, len: %d \r\n",
				le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
				scan_res_ind->adv_report.len);
#endif
		break;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_SCAN_RES_IND: {
		rtk_bt_le_ext_scan_res_ind_t *scan_res_ind = (rtk_bt_le_ext_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->addr), le_addr, sizeof(le_addr));
		BT_LOGA("[APP] Ext Scan info, [Device]: %s, AD evt type: 0x%x, RSSI: %d, PHY: 0x%x, TxPower: %d, Len: %d\r\n",
				le_addr, scan_res_ind->evt_type, scan_res_ind->rssi,
				(scan_res_ind->primary_phy << 4) | scan_res_ind->secondary_phy,
				scan_res_ind->tx_power, scan_res_ind->len);
		BT_AT_PRINT("+BLEGAP:escan,%s,0x%x,%d,0x%x,%d,%d\r\n",
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
			}
#endif /* RTK_BLE_5_0_USE_EXTENDED_ADV */
			/* gatts action */
			if (RTK_BT_OK == ble_transfer_module_server_disconnect(disconn_ind->conn_handle)) {
				BT_LOGA("[APP] Transfer module server disconnect success, conn_handle: %d\r\n",
						disconn_ind->conn_handle);
			}

		}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (disconn_ind->reason != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)) {
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
		BT_AT_PRINT_INDICATE("+BLEGAP:authcmpl,%d,%d\r\n",
							 auth_cplt_ind->conn_handle,
							 (auth_cplt_ind->err == 0) ? 0 : 1);
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
		BT_LOGE("[APP] Unknown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

int ble_transfer_module_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	bool adv_filter_whitelist = false;
	char addr_str[30] = {0};
	int ret = 0;

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	uint8_t adv_handle;
#else
	rtk_bt_le_adv_param_t adv_param = {0};
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
	uint8_t bond_size = 0;
#endif

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	bt_at_sync_enable(enable);
#endif

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
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
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP,
													ble_transfer_module_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)s_dev_name));
		BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN));

		BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));

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
													ble_transfer_module_gatts_app_callback));
		BT_APP_PROCESS(ble_transfer_module_server_add());

		/* gattc related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC,
													ble_transfer_module_gattc_app_callback));
		BT_APP_PROCESS(ble_transfer_module_client_add());

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
		//BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
#endif

	} else if (0 == enable) {

		BT_APP_PROCESS(ble_transfer_module_server_deinit());

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	} else {
		ret = -1;
	}

	return ret;
}
