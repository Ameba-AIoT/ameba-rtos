/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_device.h>
#include <bt_utils.h>

#include <throughput_include.h>

#define RTK_BT_DEV_NAME "RTK_BT_THROUGHPUT"

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
static uint8_t tp_ext_adv_handle = 0;
static uint8_t def_tp_ext_adv_data[] = {
	// Flags
	0x02,
	RTK_BT_LE_GAP_ADTYPE_FLAGS,
	RTK_BT_LE_GAP_ADTYPE_FLAGS_LIMITED | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
	// Local name
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'T', 'H', 'R', 'O', 'U', 'G', 'H', 'P', 'U', 'T',
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
	0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc, 0xc,
};

static rtk_bt_le_ext_adv_param_t def_tp_ext_adv_param = {
	.adv_event_prop           = RTK_BT_LE_EXT_ADV_EXTENDED_ADV_CONN_UNDIRECTED,
	.primary_adv_interval_min = 320,
	.primary_adv_interval_max = 360,
	.primary_adv_channel_map  = RTK_BT_LE_ADV_CHNL_ALL,
	.own_addr                 = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.peer_addr                = {RTK_BT_LE_ADDR_TYPE_PUBLIC, {0}},
	.filter_policy            = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	.tx_power                 = 0x7F,
	.primary_adv_phy          = RTK_BT_LE_PHYS_PRIM_ADV_1M,
	.secondary_adv_max_skip   = 0,
	.secondary_adv_phy        = RTK_BT_LE_PHYS_CODED,
	.adv_sid                  = 0,
};

#else
static uint8_t def_tp_adv_data[] = {
	0x02, //AD len
	RTK_BT_LE_GAP_ADTYPE_FLAGS, //AD types
	RTK_BT_LE_GAP_ADTYPE_FLAGS_GENERAL | RTK_BT_LE_GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, //AD data
	0x12,
	RTK_BT_LE_GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'R', 'T', 'K', '_', 'B', 'T', '_', 'T', 'H', 'R', 'O', 'U', 'G', 'H', 'P', 'U', 'T',
};

static uint8_t def_tp_scan_rsp_data[] = {
	0x3,
	RTK_BT_LE_GAP_ADTYPE_APPEARANCE, //GAP_ADTYPE_APPEARANCE
	LO_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
	HI_WORD(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN),
};

static rtk_bt_le_adv_param_t def_tp_adv_param = {
	.interval_min  = 200, //units of 0.625ms
	.interval_max  = 250,
	.type          = RTK_BT_LE_ADV_TYPE_IND,
	.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
	.peer_addr     = {
		.type      = (rtk_bt_le_addr_type_t)0,
		.addr_val  = {0},
	},
	.channel_map   = RTK_BT_LE_ADV_CHNL_ALL,
	.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
#endif


static void ble_throughput_set_data_len(uint16_t conn_handle)
{
#if defined(RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT) && RTK_BLE_4_2_DATA_LEN_EXT_SUPPORT
	uint16_t ret = 0;
	rtk_bt_le_set_datalen_param_t data_len_param = {0};

	data_len_param.conn_handle = conn_handle;
	data_len_param.max_tx_octets = 0xFB;
	data_len_param.max_tx_time = 0x848;

	ret = rtk_bt_le_gap_set_data_len(&data_len_param);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput set data length ops failed! err: 0x%x\r\n", ret);
		ret = rtk_bt_le_gap_disconnect(conn_handle);
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] Throughput disconnect ops failed! err: 0x%x\r\n", ret);
		}
	}
#else
	(void)conn_handle;
	BT_LOGE("[APP] Platform not support set data len.\r\n");
#endif
}

static rtk_bt_evt_cb_ret_t ble_throughput_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;
	uint16_t ret;
	rtk_bt_le_conn_info_t conn_info;

	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
		rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
		if (!adv_start_ind->err) {
			BT_LOGA("[APP] ADV started: adv_type %d\r\n", adv_start_ind->adv_type);
		} else {
			BT_LOGE("[APP] ADV start failed, err 0x%x\r\n", adv_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,start,%d,%d\r\n", (adv_start_ind->err == 0) ? 0 : -1, adv_start_ind->adv_type);
		break;
	}

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] ADV stopped: reason 0x%x\r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] ADV stop failed, err 0x%x\r\n", adv_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:adv,stop,%d,0x%x\r\n", (adv_stop_ind->err == 0) ? 0 : -1, adv_stop_ind->stop_reason);
		break;
	}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
		rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
		if (!ext_adv_ind->err) {
			if (ext_adv_ind->is_start) {
				BT_LOGA("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
			} else {
				BT_LOGA("[APP] Ext ADV(%d) stopped: reason 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
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

	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			BT_LOGA("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
					conn_ind->conn_handle, role, le_addr);
			ble_throughput_set_data_len(conn_ind->conn_handle);

			if (RTK_BT_LE_ROLE_MASTER == conn_ind->role &&
				RTK_BT_OK == ble_throughput_client_link_connected(conn_ind->conn_handle)) {
				BT_LOGA("[APP] Throughput client link connect success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			} else if (RTK_BT_LE_ROLE_SLAVE == conn_ind->role &&
					   RTK_BT_OK == ble_throughput_server_link_connected(conn_ind->conn_handle)) {
				BT_LOGA("[APP] Throughput server link connect success, conn_handle: %d\r\n",
						conn_ind->conn_handle);
			}
		} else {
			BT_LOGE("[APP] Connection establish failed, err: 0x%x, remote device: %s\r\n",
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

		if (RTK_BT_LE_ROLE_SLAVE == disconn_ind->role) {
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
			if (rtk_bt_le_gap_adv_is_idle()) {
				BT_LOGA("[APP] Reconnect Ext ADV starting, adv event prop:%d,  own addr type: %d, filter policy: %d\r\n",
						def_tp_ext_adv_param.adv_event_prop, def_tp_ext_adv_param.own_addr.type, def_tp_ext_adv_param.filter_policy);
				BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(tp_ext_adv_handle, 0, 0));
			}
#else
			rtk_bt_le_gap_dev_state_t dev_state;
			if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK &&
				dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
				BT_LOGA("[APP] Reconnect ADV starting, adv type:%d,  own addr type: %d, filter policy: %d\r\n",
						def_tp_adv_param.type, def_tp_adv_param.own_addr_type, def_tp_adv_param.filter_policy);
				BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&def_tp_adv_param));
			}
#endif
			if (RTK_BT_OK == ble_throughput_server_link_disconnected(disconn_ind->conn_handle)) {
				BT_LOGA("[APP] Throughput server link disconnect success, conn_handle: %d\r\n",
						disconn_ind->conn_handle);
			}
		} else if (RTK_BT_LE_ROLE_MASTER == disconn_ind->role &&
				   RTK_BT_OK == ble_throughput_client_link_disconnected(disconn_ind->conn_handle)) {
			BT_LOGA("[APP] Throughput client link disconnect success, conn_handle: %d\r\n",
					disconn_ind->conn_handle);
		}
		break;
	}

	case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
		rtk_bt_le_conn_update_ind_t *conn_update_ind = (rtk_bt_le_conn_update_ind_t *)param;
		if (RTK_BT_OK != (ret = rtk_bt_le_gap_get_conn_info(conn_update_ind->conn_handle, &conn_info))) {
			BT_LOGE("[APP] Get connection info in RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND cb failed! err: 0x%x\r\n", ret);
			return RTK_BT_EVT_CB_OK;
		}
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
		rtk_bt_le_data_len_change_ind_t *data_len_change = (rtk_bt_le_data_len_change_ind_t *)param;
		BT_LOGA("[APP] Data len is updated, conn_handle: %d, "     \
				"max_tx_octets: 0x%x, max_tx_time: 0x%x, "         \
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
		rtk_bt_le_phy_update_ind_t *phy_update_ind = (rtk_bt_le_phy_update_ind_t *)param;
		if (RTK_BT_OK != (ret = rtk_bt_le_gap_get_conn_info(phy_update_ind->conn_handle, &conn_info))) {
			BT_LOGE("[APP] Get connection info in RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND cb failed! err: 0x%x\r\n", ret);
			return RTK_BT_EVT_CB_OK;
		}
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
			if (RTK_BT_LE_ROLE_MASTER == conn_info.role) {
				ble_throughput_client_select_tx_mode(phy_update_ind->conn_handle);
			} else if (RTK_BT_LE_ROLE_SLAVE == conn_info.role) {
				ble_throughput_server_update_phy(phy_update_ind->conn_handle);
			}
		}
		break;
	}

	default:
		BT_LOGE("[APP] Unknown gap cb evt type: %d\r\n", evt_code);
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

int ble_throughput_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};
	char name[30] = {0};

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
		bt_app_conf.mtu_size = 0xF7;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.slave_init_mtu_req = false;
		bt_app_conf.prefer_all_phy = RTK_BT_LE_PHYS_PREFER_ALL;
		bt_app_conf.prefer_tx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED;
		bt_app_conf.prefer_rx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;
		bt_app_conf.user_def_service = false;
		bt_app_conf.cccd_not_check = false;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP,
													ble_throughput_gap_app_callback));
		memcpy(name, (const char *)RTK_BT_DEV_NAME, strlen((const char *)RTK_BT_DEV_NAME));
		BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)name));
		BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_UNKNOWN));

		/* gatts related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS,
													ble_throughput_server_gatts_app_callback));
		BT_APP_PROCESS(ble_throughput_server_add());

		/* gattc related */
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTC,
													ble_throughput_client_gattc_app_callback));
		BT_APP_PROCESS(ble_throughput_client_add());

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
		BT_APP_PROCESS(rtk_bt_le_gap_create_ext_adv(&def_tp_ext_adv_param, &tp_ext_adv_handle));
		BT_APP_PROCESS(rtk_bt_le_gap_set_ext_adv_data(tp_ext_adv_handle, def_tp_ext_adv_data, sizeof(def_tp_ext_adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_start_ext_adv(tp_ext_adv_handle, 0, 0));
#else
		BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(def_tp_adv_data, sizeof(def_tp_adv_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_set_scan_rsp_data(def_tp_scan_rsp_data, sizeof(def_tp_scan_rsp_data)));
		BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&def_tp_adv_param));
#endif
	} else if (0 == enable) {
		BT_APP_PROCESS(ble_throughput_server_deinit());
		BT_APP_PROCESS(ble_throughput_client_delete());

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}