
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>
#include <math.h>

#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_generic_onoff_model.h>
#include <rtk_bt_mesh_datatrans_model.h>
#include <rtk_bt_mesh_light_model.h>
#include <rtk_bt_mesh_time_model.h>
#include <rtk_bt_mesh_scheduler_model.h>
#include <rtk_bt_mesh_scene_model.h>
#include <rtk_bt_mesh_generic_default_transition_time.h>
#include <rtk_bt_mesh_generic_model.h>
#include <rtk_bt_mesh_sensor_model.h>
#include <rtk_bt_mesh_health_model.h>
#include <bt_utils.h>

static void mesh_data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++) {
		if (pbuffer[i]) {
			break;
		}
	}
	if (i == len) {
		return;
	}

	BT_DUMPHEXA("0x", pbuffer, len, false);
}

static rtk_bt_evt_cb_ret_t ble_mesh_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;
	switch (evt_code) {
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
	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		/*  Do not report scan start event for LPN, because it could auto start and stop scan in every FRIEND POLL interval
		        rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
		        if (!scan_start_ind->err) {
		            BT_LOGA("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		        } else {
		            BT_LOGE("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		        }
		*/
		break;
	}

	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		/*  Do not report scan stop event for LPN, because it could auto start and stop scan in every FRIEND POLL interval
		        rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		        if (!scan_stop_ind->err) {
		            BT_LOGA("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		        } else {
		            BT_LOGE("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		        }
		*/
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

	case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
		rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
		if (!adv_stop_ind->err) {
			BT_LOGA("[APP] ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
		} else {
			BT_LOGE("[APP] ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
		}
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

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_evt_cb_ret_t ble_mesh_stack_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_STACK_EVT_SCAN_SWITCH: {
		rtk_bt_mesh_stack_evt_scan_switch_t *scan_switch;
		scan_switch = (rtk_bt_mesh_stack_evt_scan_switch_t *)param;
		if (scan_switch->scan_enable) {
			BT_LOGA("[APP] Ble mesh scan enable SUCCESS\r\n\r\n");
		} else {
			BT_LOGA("[APP] Ble mesh scan disable SUCCESS\r\n\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_UDB_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_udb_t *udb_info;
		udb_info = (rtk_bt_mesh_stack_evt_dev_info_udb_t *)param;
		BT_LOGA("[APP] bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", udb_info->dev_info.bt_addr[5], udb_info->dev_info.bt_addr[4],
				udb_info->dev_info.bt_addr[3], udb_info->dev_info.bt_addr[2], udb_info->dev_info.bt_addr[1], udb_info->dev_info.bt_addr[0], udb_info->dev_info.bt_addr_type,
				udb_info->dev_info.rssi);
		BT_LOGA("udb=");
		mesh_data_uart_dump(udb_info->dev_uuid, 16);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,udb,",
					udb_info->dev_info.bt_addr[5], udb_info->dev_info.bt_addr[4], udb_info->dev_info.bt_addr[3],
					udb_info->dev_info.bt_addr[2], udb_info->dev_info.bt_addr[1], udb_info->dev_info.bt_addr[0],
					udb_info->dev_info.bt_addr_type, udb_info->dev_info.rssi);
		BT_AT_DUMP_HEXN(udb_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROV_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *prov_info;
		prov_info = (rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *)param;
		BT_LOGA("[APP] bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", prov_info->dev_info.bt_addr[5], prov_info->dev_info.bt_addr[4],
				prov_info->dev_info.bt_addr[3], prov_info->dev_info.bt_addr[2], prov_info->dev_info.bt_addr[1], prov_info->dev_info.bt_addr[0],
				prov_info->dev_info.bt_addr_type, prov_info->dev_info.rssi);
		BT_LOGA("prov=");
		mesh_data_uart_dump(prov_info->dev_uuid, 16);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,prov,",
					prov_info->dev_info.bt_addr[5], prov_info->dev_info.bt_addr[4], prov_info->dev_info.bt_addr[3],
					prov_info->dev_info.bt_addr[2], prov_info->dev_info.bt_addr[1], prov_info->dev_info.bt_addr[0],
					prov_info->dev_info.bt_addr_type, prov_info->dev_info.rssi);
		BT_AT_DUMP_HEXN(prov_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROXY_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *proxy_info;
		proxy_info = (rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *)param;
		BT_LOGA("[APP] bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", proxy_info->dev_info.bt_addr[5], proxy_info->dev_info.bt_addr[4],
				proxy_info->dev_info.bt_addr[3], proxy_info->dev_info.bt_addr[2], proxy_info->dev_info.bt_addr[1], proxy_info->dev_info.bt_addr[0],
				proxy_info->dev_info.bt_addr_type, proxy_info->dev_info.rssi);
		BT_LOGA("proxy=");
		mesh_data_uart_dump((uint8_t *)&proxy_info->proxy, proxy_info->len);
		BT_AT_PRINT("+BLEMESHSTACK:dev_info,0x%02x%02x%02x%02x%02x%02x,%d,%d,proxy,",
					proxy_info->dev_info.bt_addr[5], proxy_info->dev_info.bt_addr[4], proxy_info->dev_info.bt_addr[3],
					proxy_info->dev_info.bt_addr[2], proxy_info->dev_info.bt_addr[1], proxy_info->dev_info.bt_addr[0],
					proxy_info->dev_info.bt_addr_type, proxy_info->dev_info.rssi);
		BT_AT_DUMP_HEXN((uint8_t *)&proxy_info->proxy, proxy_info->len);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_HB_PUB_TIMER_STATE: {
		rtk_bt_mesh_stack_hb_data_timer_state_t *hb_timer_state;
		hb_timer_state = (rtk_bt_mesh_stack_hb_data_timer_state_t *)param;
		if (RTK_BT_MESH_STACK_HB_TIMER_STATE_START == hb_timer_state->timer_state) {
			BT_LOGA("[APP] Heartbeat publish timer start, period = %d\r\n", hb_timer_state->period);
		} else {
			BT_LOGA("[APP] Heartbeat publish timer stop\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_HB_SUB_TIMER_STATE: {
		rtk_bt_mesh_stack_hb_data_timer_state_t *hb_timer_state;
		hb_timer_state = (rtk_bt_mesh_stack_hb_data_timer_state_t *)param;
		if (RTK_BT_MESH_STACK_HB_TIMER_STATE_START == hb_timer_state->timer_state) {
			BT_LOGA("[APP] Heartbeat subscription timer start, period = %d\r\n", hb_timer_state->period);
		} else {
			BT_LOGA("[APP] Heartbeat subscription timer stop\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_HB_PUB_COUNT_UPDATE: {
		rtk_bt_mesh_stack_hb_pub_count_update_t *hb_pub_count;
		hb_pub_count = (rtk_bt_mesh_stack_hb_pub_count_update_t *)param;
		BT_LOGA("[APP] Heartbeat publish count update: %d\r\n", hb_pub_count->count);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_HB_SUB_PERIOD_UPDATE: {
		rtk_bt_mesh_stack_hb_sub_period_update_t *hb_sub_period;
		hb_sub_period = (rtk_bt_mesh_stack_hb_sub_period_update_t *)param;
		BT_LOGA("[APP] Heartbeat subscription period update: %d\r\n", hb_sub_period->period);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_HB_SUB_RECEIVE: {
		rtk_bt_mesh_stack_hb_data_sub_receive_t *hb_sub_receive;
		hb_sub_receive = (rtk_bt_mesh_stack_hb_data_sub_receive_t *)param;
		BT_LOGA("[APP] Receive heartbeat: src = 0x%x, init_ttl = %d, actual ttl = %d, features = %d-%d-%d-%d\r\n",
				hb_sub_receive->src, hb_sub_receive->init_ttl, hb_sub_receive->ttl, hb_sub_receive->features.relay,
				hb_sub_receive->features.proxy, hb_sub_receive->features.frnd, hb_sub_receive->features.lpn);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE: {
		rtk_bt_mesh_prov_generic_cb_type_t *pb_adv_link_state;
		pb_adv_link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)param;
		BT_AT_PRINT("+BLEMESHSTACK:pbadv_state,%d\r\n", *pb_adv_link_state);
		switch (*pb_adv_link_state) {
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED:
			BT_LOGA("[APP] PB-ADV link opened\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPEN_FAILED:
			BT_LOGA("[APP] PB-ADV link open Failed\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_CLOSED:
			BT_LOGA("[APP] PB-ADV link closed\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_COMPLETE: {
		rtk_bt_mesh_stack_evt_prov_complete_t *prov_complete;
		prov_complete = (rtk_bt_mesh_stack_evt_prov_complete_t *)param;
		BT_LOGA("[APP] Provisioning complete,unicast address:0x%x\r\n", prov_complete->unicast_addr);
		BT_AT_PRINT("+BLEMESHSTACK:prov,0,0x%x\r\n", prov_complete->unicast_addr);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_FAIL: {
		rtk_bt_mesh_stack_evt_prov_fail_t *prov_fail;
		prov_fail = (rtk_bt_mesh_stack_evt_prov_fail_t *)param;
		BT_LOGE("[APP] Provisioning fail,reason:%d\r\n", prov_fail->fail_reason);
		BT_AT_PRINT("+BLEMESHSTACK:prov,-1,%d\r\n", prov_fail->fail_reason);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_START_PROVED: {
		BT_LOGA("[APP] Being Provisioned\r\n");
		break;
	}
	case RTK_BT_MESH_STACK_EVT_UNPROVISIONED_DEVICE: {
		BT_LOGA("[APP] Unprovisioned Device\r\n");
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROVISIONED_DEVICE: {
		rtk_bt_mesh_stack_evt_provisioned_device_t *provisioned_dev;
		provisioned_dev = (rtk_bt_mesh_stack_evt_provisioned_device_t *)param;
		BT_LOGA("[APP] Provisioned Device,unicast addr:0x%x\r\n", provisioned_dev->unicast_addr);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_STATIC_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_static_oob *static_oob;
		static_oob = (rtk_bt_mesh_stack_set_auth_value_for_static_oob *)param;
		if (static_oob->status) {
			BT_DUMPA("[APP] Set auth value for static oob success:\r\n", static_oob->data, 16);
		} else {
			BT_LOGE("[APP] Set auth value for static oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_OUTPUT_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_oob_data *output_oob;
		output_oob = (rtk_bt_mesh_stack_set_auth_value_for_oob_data *)param;
		if (output_oob->status) {
			BT_LOGA("[APP] Set auth value for output oob success, output random value:%d\r\n", (int)output_oob->random);
		} else {
			BT_LOGE("[APP] Set auth value for output oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_INPUT_OOB_VALUE: {
		BT_LOGA("[APP] Please input the random value display on remote provisioner\r\n");
		break;
	}
	case RTK_BT_MESH_STACK_EVT_FN_CB: {
		rtk_bt_mesh_stack_evt_fn_t *fn_cb = (rtk_bt_mesh_stack_evt_fn_t *)param;
		switch (fn_cb->cb_type) {
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISHING:
			BT_LOGA("[APP] Establishing friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_FAIL_NO_POLL:
			BT_LOGA("[APP] Not receive poll LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_SUCCESS:
			BT_LOGA("[APP] Established friendship with LPN: 0x%04x success\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_FRND_LOST:
			BT_LOGA("[APP] Lost friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		default:
			BT_LOGE("[%s] Unknown cb type:%d\r\n", __func__, fn_cb->cb_type);
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_LPN_CB: {
		rtk_bt_mesh_stack_evt_lpn_t *lpn_cb = (rtk_bt_mesh_stack_evt_lpn_t *)param;
		switch (lpn_cb->cb_type) {
		case RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_SUCCESS:
			BT_LOGA("[APP] Established friendship with FN: 0x%04x success\r\n", lpn_cb->fn_addr);
			BT_AT_PRINT("+BLEMESHSTACK:lpn,%d,0x%04x\r\n", lpn_cb->cb_type, lpn_cb->fn_addr);
			break;
		case RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_FAIL_NO_OFFER:
			BT_LOGA("[APP] No frnd offer\r\n");
			BT_AT_PRINT("+BLEMESHSTACK:lpn,%d\r\n", lpn_cb->cb_type);
			break;
		case RTK_BT_MESH_LPN_CB_TYPE_ESTABLISH_FAIL_NO_UPDATE:
			BT_LOGA("[APP] No frnd update\r\n");
			BT_AT_PRINT("+BLEMESHSTACK:lpn,%d\r\n", lpn_cb->cb_type);
			break;
		case RTK_BT_MESH_LPN_CB_TYPE_FRIENDSHIP_LOST:
			BT_LOGA("[APP] Lost friendship with FN: 0x%04x\r\n", lpn_cb->fn_addr);
			BT_AT_PRINT("+BLEMESHSTACK:lpn,%d,0x%04x\r\n", lpn_cb->cb_type, lpn_cb->fn_addr);
			break;
		default:
			BT_LOGE("[%s] Unknown cb type:%d\r\n", __func__, lpn_cb->cb_type);
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_LIST_INFO: {
		// Indicate event data struct : type(1 byte) + length(1 byte) + value(n bytes) + type +len + val ......
		uint8_t *p_data = (uint8_t *)param;
		uint32_t offset = 0, type, data_len;
		while (offset < len) {
			type = *(p_data + offset);
			offset++;
			data_len = *(p_data + offset);
			offset++;
			// In case of IPC data align , which will cause the event report len larger than actual data len
			if (offset + data_len > len) {
				break;
			}
			switch (type) {
			case RTK_BT_MESH_STACK_USER_LIST_MESH_STATE:
				BT_LOGA("Mesh State:\t%d\r\n", *(p_data + offset));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d\r\n", type, *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_UUID:
				BT_LOGA("Device UUID:\t");
				mesh_data_uart_dump(p_data + offset, data_len);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,", type);
				BT_AT_DUMP_HEXN(p_data + offset, data_len);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR:
				BT_LOGA("BTAddr:\t\t0x%02x%02x%02x%02x%02x%02x\r\n",
						*(p_data + offset + 5), *(p_data + offset + 4), *(p_data + offset + 3), *(p_data + offset + 2), *(p_data + offset + 1), *(p_data + offset));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,0x%02x%02x%02x%02x%02x%02x\r\n",
							type, *(p_data + offset + 5), *(p_data + offset + 4), *(p_data + offset + 3),
							*(p_data + offset + 2), *(p_data + offset + 1), *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_KEY:
				BT_LOGA("DevKey:\t\t%d-0x%04x-%d-", *(p_data + offset), LE_TO_U16(p_data + offset + 1), *(p_data + offset + 3));
				mesh_data_uart_dump(p_data + offset + 4, 16);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%04x-%d-",
							type, *(p_data + offset), LE_TO_U16(p_data + offset + 1), *(p_data + offset + 3));
				BT_AT_DUMP_HEXN(p_data + offset + 4, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NET_KEY:
				BT_LOGA("NetKey:\t\t%d-0x%04x-%d-%d-%d\r\n\t\t", *(p_data + offset), LE_TO_U16(p_data + offset + 1), \
						* (p_data + offset + 3), *(p_data + offset + 4), *(p_data + offset + 5));
				mesh_data_uart_dump(p_data + offset + 6, 16);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%04x-%d-%d-%d-",
							type, *(p_data + offset), LE_TO_U16(p_data + offset + 1),
							*(p_data + offset + 3), *(p_data + offset + 4), *(p_data + offset + 5));
				BT_AT_DUMP_HEXN(p_data + offset + 6, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_APP_KEY:
				BT_LOGA("AppKey:\t\t%d-0x%04x-%d-%d-%d\r\n", *(p_data + offset), LE_TO_U16(p_data + offset + 1), \
						* (p_data + offset + 3), *(p_data + offset + 4), LE_TO_U16(p_data + offset + 5));
				mesh_data_uart_dump(p_data + offset + 7, 16);
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%04x-%d-%d-%d-",
							type, *(p_data + offset), LE_TO_U16(p_data + offset + 1),
							*(p_data + offset + 3), *(p_data + offset + 4), LE_TO_U16(p_data + offset + 5));
				BT_AT_DUMP_HEXN(p_data + offset + 7, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS:
				BT_LOGA("IVindex:\t%d-0x%x\r\n", *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				BT_LOGA("Seq:\t\t0x%06x\r\n", LE_TO_U32(p_data + offset + 5));
				BT_LOGA("NodeAddr:\t0x%04x-%d-%d\r\n", LE_TO_U16(p_data + offset + 9),
						LE_TO_U32(p_data + offset + 11), *(p_data + offset + 15));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-0x%x,0x%06x,0x%04x-%d-%d\r\n",
							type, *(p_data + offset), LE_TO_U32(p_data + offset + 1),
							LE_TO_U32(p_data + offset + 5), LE_TO_U16(p_data + offset + 9),
							LE_TO_U32(p_data + offset + 11), *(p_data + offset + 15));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO:
				BT_LOGA("Element:\t%d-%d", *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				BT_AT_PRINT("+BLEMESHSTACK:list,%d,%d-%d",
							type, *(p_data + offset), LE_TO_U32(p_data + offset + 1));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO:
				BT_LOGA("\r\nModel:\t\t%d-%d-0x%08x", *(p_data + offset), *(p_data + offset + 1), LE_TO_U32(p_data + offset + 2));
				BT_AT_PRINT("\r\n+BLEMESHSTACK:list,%d,%d-%d-0x%08x",
							type, *(p_data + offset), *(p_data + offset + 1), LE_TO_U32(p_data + offset + 2));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY:
				BT_LOGA("(app key:%d)", *(p_data + offset));
				BT_AT_PRINT(",%d,%d", type, *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO:
				BT_LOGA("(pub to:0x%04x-%d-%d)", LE_TO_U16(p_data + offset), *(p_data + offset + 2),
						LE_TO_U16(p_data + offset + 3));
				BT_AT_PRINT(",%d,0x%04x-%d-%d", type, LE_TO_U16(p_data + offset), *(p_data + offset + 2),
							LE_TO_U16(p_data + offset + 3));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO: {
				uint16_t sub_addr_num = data_len / 2;
				uint16_t temp_offset = offset;
				BT_LOGA("(sub to:");
				BT_AT_PRINT(",%d,", type);
				for (uint16_t i = 0; i < sub_addr_num; i++) {
					BT_LOGA("0x%04x", LE_TO_U16(p_data + temp_offset));
					BT_AT_PRINT("0x%04x", LE_TO_U16(p_data + temp_offset));
					temp_offset += 2;
					if (i == sub_addr_num - 1) {
						BT_LOGA(")");
					} else {
						BT_LOGA("-");
						BT_AT_PRINT("-");
					}
				}
				break;
			}
			default:
				BT_LOGE("[%s] Unknown data type %d for RTK_BT_MESH_STACK_EVT_LIST_INFO\r\n", __func__, (int)type);
				return RTK_BT_EVT_CB_OK;
				break;
			}
			offset += data_len;
		}
		BT_LOGA("\r\n\r\n");
		BT_AT_PRINT("\r\n");
		if (!len) {
			BT_LOGE("[APP] Print user cmd list fail, len==0\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_RETRANS_PARAM_SETTING_RESULT: {
		rtk_bt_mesh_stack_set_retrans_param_t *result;
		result = (rtk_bt_mesh_stack_set_retrans_param_t *)param;
		BT_LOGA("Set node net trans count %d, relay retrans count %d, ttl %d, trans retrans count %d, Success!\r\n",
				result->net_retrans_count, result->relay_retrans_count, result->ttl, result->trans_retrans_count);
		BT_AT_PRINT("+BLEMESHSTACK:retran_set,%d,%d,%d,%d\r\n",
					result->net_retrans_count, result->relay_retrans_count,
					result->ttl, result->trans_retrans_count);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic on off server app callback***/
static rtk_bt_mesh_generic_on_off_t on_off = RTK_BT_MESH_GENERIC_OFF;
static rtk_bt_evt_cb_ret_t ble_mesh_generic_onoff_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_SET: {
		rtk_bt_mesh_generic_onoff_server_evt_set_t *onoff_set;
		onoff_set = (rtk_bt_mesh_generic_onoff_server_evt_set_t *)param;
		if (onoff_set->on_off == RTK_BT_MESH_GENERIC_ON) {
			on_off = RTK_BT_MESH_GENERIC_ON;
			BT_LOGA("[APP] Turn light ON");
			if (0 != onoff_set->remaining_time.num_steps) {
				BT_LOGA(", total time(steps:%d, resolution:%d), remain time(steps:%d, resolution:%d)\r\n",
						onoff_set->total_time.num_steps, onoff_set->total_time.step_resolution, onoff_set->remaining_time.num_steps, onoff_set->remaining_time.step_resolution);
			} else {
				BT_LOGA("\r\n");
			}
		} else if (onoff_set->on_off == RTK_BT_MESH_GENERIC_OFF) {
			on_off = RTK_BT_MESH_GENERIC_OFF;
			BT_LOGA("[APP] Turn light OFF");
			if (0 != onoff_set->remaining_time.num_steps) {
				BT_LOGA(", total time(steps:%d, resolution:%d), remain time(steps:%d, resolution:%d)\r\n",
						onoff_set->total_time.num_steps, onoff_set->total_time.step_resolution, onoff_set->remaining_time.num_steps, onoff_set->remaining_time.step_resolution);
			} else {
				BT_LOGA("\r\n");
			}
		}
		if (0 != onoff_set->remaining_time.num_steps) {
			BT_AT_PRINT("+BLEMESHGOO:%d,%d,1,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_SERVER, onoff_set->on_off, onoff_set->total_time.num_steps, onoff_set->total_time.step_resolution,
						onoff_set->remaining_time.num_steps, onoff_set->remaining_time.step_resolution);
		} else {
			BT_AT_PRINT("+BLEMESHGOO:%d,%d,0\r\n", BT_AT_MESH_ROLE_SERVER, onoff_set->on_off);
		}
		break;
	}
	case RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_GET: {
		rtk_bt_mesh_generic_onoff_server_evt_get_t *onoff_get;
		onoff_get = (rtk_bt_mesh_generic_onoff_server_evt_get_t *)param;
		*(onoff_get->on_off) = on_off;
		break;
	}
	default:
		BT_LOGA("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

#endif // end of BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
/***BT and BLE Mesh related api shall not be called in datatrans model app callback***/
static uint8_t datatrans_sample_data[DATA_TRANS_DATA_MAX_LEN] = {0xf, 0xd, 0xb, 0x9, 0x7, 0x5, 0x3, 0x1};
static rtk_bt_evt_cb_ret_t ble_mesh_datatrans_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_WRITE: {
		rtk_bt_mesh_datatrans_server_write_event_t *p_data_write = (rtk_bt_mesh_datatrans_server_write_event_t *)param;
		/*
		        // Avoid compile warning: data_len(uint8_t) always <= DATA_TRANS_DATA_MAX_LEN(0xff)
		        if (p_data_write->data_len > DATA_TRANS_DATA_MAX_LEN) {
		            p_data_write->data_len = DATA_TRANS_DATA_MAX_LEN;
		            BT_LOGA("[APP] Datatrans write message len is %d,extend max val %d,only write %d bytes\r\n", p_data_write->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(datatrans_sample_data, p_data_write->data, p_data_write->data_len);
		BT_LOGA("[APP] Datatrans model receive msg: Remote write %d bytes: ", p_data_write->data_len);
		mesh_data_uart_dump(p_data_write->data, p_data_write->data_len);
		BT_AT_PRINT("+BLEMESHDATA:write,%d,%d,", BT_AT_MESH_ROLE_SERVER, p_data_write->data_len);
		BT_AT_DUMP_HEXN(p_data_write->data, p_data_write->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS: {
		rtk_bt_mesh_datatrans_client_write_event_t *p_write_status = (rtk_bt_mesh_datatrans_client_write_event_t *)param;
		BT_LOGA("[APP] Datatrans model receive msg: Src %d ,written %d bytes, status:%d\r\n", p_write_status->src, p_write_status->written_len, p_write_status->status);
		BT_AT_PRINT("+BLEMESHDATA:write,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_CLIENT, p_write_status->src, p_write_status->written_len, p_write_status->status);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT: {
		rtk_bt_mesh_datatrans_client_read_event_t *p_read_date = (rtk_bt_mesh_datatrans_client_read_event_t *)param;
		BT_LOGA("[APP] Datatrans model receive msg: Read %d bytes date from remote src %d :\r\n", p_read_date->data_len, p_read_date->src);
		mesh_data_uart_dump(p_read_date->data, p_read_date->data_len);
		BT_AT_PRINT("+BLEMESHDATA:read,%d,%d,%d,",
					BT_AT_MESH_ROLE_CLIENT, p_read_date->src, p_read_date->data_len);
		BT_AT_DUMP_HEXN(p_read_date->data, p_read_date->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_READ: {
		rtk_bt_mesh_datatrans_server_read_event_t *server_read = (rtk_bt_mesh_datatrans_server_read_event_t *)param;
		/*
		        // Avoid compile warning: data_len(uint8_t) always <= DATA_TRANS_DATA_MAX_LEN(0xff)
		        if (server_read->data_len > DATA_TRANS_DATA_MAX_LEN) {
		            server_read->data_len = DATA_TRANS_DATA_MAX_LEN;
		            BT_LOGA("[APP] Datatrans read message len is %d, extend max val %d, only read %d bytes\r\n", server_read->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(server_read->data, datatrans_sample_data, server_read->data_len);
		BT_LOGA("[APP] Remote read %d bytes data\r\n", server_read->data_len);
		BT_AT_PRINT("+BLEMESHDATA:read,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, server_read->data_len);
		break;
	}

	default:
		BT_LOGE("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light lightness server app callback***/
static rtk_bt_mesh_light_lightness_server_get_t default_lightness = {0};
static rtk_bt_mesh_light_lightness_server_get_t last_lightness = {0};
static rtk_bt_mesh_light_lightness_server_get_range_t lightness_range = {0};
static rtk_bt_mesh_light_lightness_server_set_t lightness = {0};
static rtk_bt_mesh_light_lightness_server_set_t linear_lightness = {0};
static uint16_t pre_lightness = 0;
static uint16_t pre_linear_lightness = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_lightness_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_lightness_server_direct_get_t *lightness_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)param;
		memcpy(lightness_get->value, &(lightness.lightness), sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_lightness_server_direct_get_t *linear_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)param;
		memcpy(linear_get->value, &(linear_lightness.lightness), sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_lightness_server_direct_get_t *default_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)param;
		memcpy(default_get->value, &default_lightness, sizeof(rtk_bt_mesh_light_lightness_server_get_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_lightness_server_direct_get_t *last_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)param;
		memcpy(last_get->value, &last_lightness, sizeof(rtk_bt_mesh_light_lightness_server_get_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_lightness_server_direct_get_range_t *range_get = (rtk_bt_mesh_light_lightness_server_direct_get_range_t *)param;
		memcpy(range_get->value, &lightness_range, sizeof(rtk_bt_mesh_light_lightness_server_get_range_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_lightness_server_set_t *lightness_set = (rtk_bt_mesh_light_lightness_server_set_t *)param;
		lightness.total_time = lightness_set->total_time;
		lightness.remaining_time = lightness_set->remaining_time;

		if (lightness.total_time.num_steps == 0 ||
			lightness.remaining_time.num_steps == 0) {
			lightness.lightness = lightness_set->lightness;
		} else {
			if (lightness.total_time.num_steps == lightness.remaining_time.num_steps) {
				pre_lightness = lightness.lightness;
			}
			lightness.lightness =
				lightness_set->lightness - (lightness_set->lightness - pre_lightness) * ((double)lightness.remaining_time.num_steps) /
				lightness.total_time.num_steps;
		}
		BT_LOGA("[APP] light lightness server receive: set lightness %d, total_time %d, remaining_time %d \r\n",
				lightness.lightness,
				lightness.total_time.num_steps,
				lightness.remaining_time.num_steps);
		BT_AT_PRINT("+BLEMESHLL:ll,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, lightness.lightness,
					lightness.total_time.num_steps, lightness.remaining_time.num_steps);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_lightness_server_set_t *linear_set = (rtk_bt_mesh_light_lightness_server_set_t *)param;
		uint16_t linear_cal_value = linear_set->lightness;
		linear_lightness.total_time = linear_set->total_time;
		linear_lightness.remaining_time = linear_set->remaining_time;

		if (linear_lightness.total_time.num_steps == 0 ||
			linear_lightness.remaining_time.num_steps == 0) {
			linear_lightness.lightness = linear_cal_value;
		} else {
			if (linear_lightness.total_time.num_steps == linear_lightness.remaining_time.num_steps) {
				pre_linear_lightness = linear_lightness.lightness;
			}
			linear_lightness.lightness =
				linear_cal_value - (linear_cal_value - pre_linear_lightness) * ((double)linear_lightness.remaining_time.num_steps) /
				linear_lightness.total_time.num_steps;
		}
		BT_LOGA("[APP] light lightness server receive: set linear lightness %d, total_time %d, remaining_time %d \r\n",
				linear_lightness.lightness,
				(linear_set->total_time).num_steps,
				(linear_set->remaining_time).num_steps);
		BT_AT_PRINT("+BLEMESHLL:lll,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, linear_lightness.lightness,
					(linear_set->total_time).num_steps, (linear_set->remaining_time).num_steps);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_lightness_server_get_t *last_set = (rtk_bt_mesh_light_lightness_server_get_t *)param;
		BT_LOGA("[APP] light lightness server receive: set last lightness %d \r\n", last_set->lightness);
		BT_AT_PRINT("+BLEMESHLL:llla,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, last_set->lightness);
		last_lightness.lightness = last_set->lightness;
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
static rtk_bt_mesh_light_lightness_server_get_t default_setup_lightness = {0};
static rtk_bt_mesh_light_lightness_server_get_range_t lightness_setup_range = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_light_lightness_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_lightness_server_set_default_t *default_set = (rtk_bt_mesh_light_lightness_server_set_default_t *)param;
		BT_LOGA("[APP] light lightness server receive: set default lightness %d \r\n", default_set->lightness);
		BT_AT_PRINT("+BLEMESHLL:lld,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, default_set->lightness);
		default_setup_lightness.lightness = default_set->lightness;
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_lightness_server_set_range_t *range_set = (rtk_bt_mesh_light_lightness_server_set_range_t *)param;
		BT_LOGA("[APP] light lightness server receive: set lightness_max %d, lightness_min %d \r\n",
				range_set->range_max,
				range_set->range_min);
		BT_AT_PRINT("+BLEMESHLL:llr,%d,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER,
					range_set->range_min, range_set->range_max);
		lightness_setup_range.range_max = range_set->range_max;
		lightness_setup_range.range_min = range_set->range_min;
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light ctl server app callback***/
static rtk_bt_mesh_light_ctl_server_get_default_t ctl_default = {0};
static rtk_bt_mesh_light_ctl_server_get_temperature_range_t ctl_range = {0};
static rtk_bt_mesh_light_ctl_server_set_t ctl_lightness_set = {0};
static uint16_t pre_ctl_lightness = 0;
static uint16_t pre_ctl_temperature = 0;
static int16_t pre_ctl_delta_uv = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_ctl_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_ctl_server_direct_get_t *ctl_get = (rtk_bt_mesh_light_ctl_server_direct_get_t *)param;
		rtk_bt_mesh_light_ctl_server_get_t ctl_lightness = {0};
		ctl_lightness.lightness = ctl_lightness_set.lightness;
		ctl_lightness.temperature = ctl_lightness_set.temperature;
		memcpy(ctl_get->value, &ctl_lightness, sizeof(rtk_bt_mesh_light_ctl_server_get_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_GET: {
		rtk_bt_mesh_light_ctl_server_direct_get_default_t *ctl_default_get = (rtk_bt_mesh_light_ctl_server_direct_get_default_t *)param;
		memcpy(ctl_default_get->value, &ctl_default, sizeof(rtk_bt_mesh_light_ctl_server_get_default_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET: {
		rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *ctl_temp_range_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *)param;
		memcpy(ctl_temp_range_get->value, &ctl_range, sizeof(rtk_bt_mesh_light_ctl_server_get_temperature_range_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_ctl_server_set_t *ctl_set = (rtk_bt_mesh_light_ctl_server_set_t *)param;
		ctl_lightness_set.remaining_time = ctl_set->remaining_time;
		ctl_lightness_set.total_time = ctl_set->total_time;
		if (ctl_lightness_set.total_time.num_steps == 0 ||
			ctl_lightness_set.remaining_time.num_steps == 0) {
			ctl_lightness_set.lightness = ctl_set->lightness;
			ctl_lightness_set.temperature = ctl_set->temperature;
			ctl_lightness_set.delta_uv = ctl_set->delta_uv;
		} else {
			if (ctl_lightness_set.total_time.num_steps == ctl_lightness_set.remaining_time.num_steps) {
				pre_ctl_lightness = ctl_lightness_set.lightness;
				pre_ctl_temperature = ctl_lightness_set.temperature;
				pre_ctl_delta_uv = ctl_lightness_set.delta_uv;
			}
			ctl_lightness_set.lightness =
				ctl_set->lightness - (ctl_set->lightness - pre_ctl_lightness) * ((double)ctl_lightness_set.remaining_time.num_steps) /
				ctl_lightness_set.total_time.num_steps;
			ctl_lightness_set.temperature =
				ctl_set->temperature - (ctl_set->temperature - pre_ctl_temperature) * ((double)ctl_lightness_set.remaining_time.num_steps) /
				ctl_lightness_set.total_time.num_steps;
			ctl_lightness_set.delta_uv =
				ctl_set->delta_uv - (ctl_set->delta_uv - pre_ctl_delta_uv) * ((double)ctl_lightness_set.remaining_time.num_steps) /
				ctl_lightness_set.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting ctl server receive: set delta_uv %d, lightness %d, remaining_time %d, temperature %d, total_time %d \r\n",
				ctl_lightness_set.delta_uv,
				ctl_lightness_set.lightness,
				ctl_lightness_set.remaining_time.num_steps,
				ctl_lightness_set.temperature,
				ctl_lightness_set.total_time.num_steps);
		BT_AT_PRINT("+BLEMESHLCTL:lc,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, ctl_lightness_set.delta_uv, ctl_lightness_set.lightness,
					ctl_lightness_set.remaining_time.num_steps, ctl_lightness_set.temperature, ctl_lightness_set.total_time.num_steps);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
static rtk_bt_mesh_light_ctl_server_get_default_t ctl_setup_default = {0};
static rtk_bt_mesh_light_ctl_server_get_temperature_range_t ctl_setup_range = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_light_ctl_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_SET: {
		rtk_bt_mesh_light_ctl_server_set_temperature_range_t *ctl_temp_range_set = (rtk_bt_mesh_light_ctl_server_set_temperature_range_t *)param;
		BT_LOGA("[APP] lighting ctl setup server receive: set temperature range_max %d, range_min %d \r\n",
				ctl_temp_range_set->range_max,
				ctl_temp_range_set->range_min);
		BT_AT_PRINT("+BLEMESHLCTL:lctr,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, ctl_temp_range_set->range_min, ctl_temp_range_set->range_max);
		ctl_setup_range.range_max = ctl_temp_range_set->range_max;
		ctl_setup_range.range_min = ctl_temp_range_set->range_min;
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_SET: {
		rtk_bt_mesh_light_ctl_server_set_default_t *ctl_default_set = (rtk_bt_mesh_light_ctl_server_set_default_t *)param;
		BT_LOGA("[APP] lighting ctl setup server receive: set default delta_uv %d, lightness %d, temperature %d \r\n",
				ctl_default_set->delta_uv,
				ctl_default_set->lightness,
				ctl_default_set->temperature);
		BT_AT_PRINT("+BLEMESHLCTL:lcd,%d,%d,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, ctl_default_set->delta_uv,
					ctl_default_set->lightness, ctl_default_set->temperature);
		ctl_setup_default.delta_uv = ctl_default_set->delta_uv;
		ctl_setup_default.lightness = ctl_default_set->lightness;
		ctl_setup_default.temperature = ctl_default_set->temperature;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light ctl temperature server app callback***/
static rtk_bt_mesh_light_ctl_server_set_temperature_t ctl_temp_set = {0};
static rtk_bt_mesh_light_ctl_server_get_temperature_range_t temp_range = {0};
static uint16_t pre_temperature = 0;
static int16_t pre_delta_uv = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_ctl_temperature_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_GET: {
		rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *temp_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *)param;
		rtk_bt_mesh_light_ctl_server_get_temperature_t ctl_temperature = {0};
		ctl_temperature.temperature = ctl_temp_set.temperature;
		ctl_temperature.delta_uv = ctl_temp_set.delta_uv;
		memcpy(temp_get->value, &ctl_temperature, sizeof(rtk_bt_mesh_light_ctl_server_get_temperature_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_SET: {
		rtk_bt_mesh_light_ctl_server_set_temperature_t *temp_set = (rtk_bt_mesh_light_ctl_server_set_temperature_t *)param;
		ctl_temp_set.total_time = temp_set->total_time;
		ctl_temp_set.remaining_time = temp_set->remaining_time;
		if (ctl_temp_set.total_time.num_steps == 0 ||
			ctl_temp_set.remaining_time.num_steps == 0) {
			ctl_temp_set.temperature = temp_set->temperature;
			ctl_temp_set.delta_uv = temp_set->delta_uv;
		} else {
			if (ctl_temp_set.total_time.num_steps == ctl_temp_set.remaining_time.num_steps) {
				pre_temperature = ctl_temp_set.temperature;
				pre_delta_uv = ctl_temp_set.delta_uv;
			}
			ctl_temp_set.temperature =
				temp_set->temperature - (temp_set->temperature - pre_temperature) * ((double)ctl_temp_set.remaining_time.num_steps) /
				ctl_temp_set.total_time.num_steps;
			ctl_temp_set.delta_uv =
				temp_set->delta_uv - (temp_set->delta_uv - pre_delta_uv) * ((double)ctl_temp_set.remaining_time.num_steps) /
				ctl_temp_set.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting ctl temperature server receive: set temperature %d, delta_uv %d, total_time %d, remaining_time %d \r\n",
				ctl_temp_set.temperature,
				ctl_temp_set.delta_uv,
				ctl_temp_set.total_time.num_steps,
				ctl_temp_set.remaining_time.num_steps);
		BT_AT_PRINT("+BLEMESHLCTL:lct,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, ctl_temp_set.temperature, ctl_temp_set.delta_uv,
					ctl_temp_set.total_time.num_steps, ctl_temp_set.remaining_time.num_steps);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET_T: {
		rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *temp_range_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *)param;
		memcpy(temp_range_get->value, &temp_range, sizeof(rtk_bt_mesh_light_ctl_server_get_temperature_range_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light hsl server app callback***/
static rtk_bt_mesh_light_hsl_server_set_t hsl_value = {0};
static rtk_bt_mesh_light_hsl_server_get_default_t hsl_default = {0};
static rtk_bt_mesh_light_hsl_server_get_range_t hsl_range = {0};

static uint16_t pre_hsl_hue = 0;
static uint16_t pre_hsl_saturation = 0;
static uint16_t pre_hsl_lightness = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_hsl_server_direct_get_t *hsl_get = (rtk_bt_mesh_light_hsl_server_direct_get_t *)param;
		rtk_bt_mesh_light_hsl_server_get_t hsl_lightness;
		hsl_lightness.hue = hsl_value.hue;
		hsl_lightness.lightness = hsl_value.lightness;
		hsl_lightness.saturation = hsl_value.saturation;
		memcpy(hsl_get->value, &hsl_lightness, sizeof(rtk_bt_mesh_light_hsl_server_get_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_hsl_server_direct_get_default_t *hsl_default_get = (rtk_bt_mesh_light_hsl_server_direct_get_default_t *)param;
		memcpy(hsl_default_get->value, &hsl_default, sizeof(rtk_bt_mesh_light_hsl_server_get_default_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_hsl_server_direct_get_range_t *hsl_range_get = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)param;
		memcpy(hsl_range_get->value, &hsl_range, sizeof(rtk_bt_mesh_light_hsl_server_get_range_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_hsl_server_set_t *hsl_set = (rtk_bt_mesh_light_hsl_server_set_t *)param;
		hsl_value.remaining_time = hsl_set->remaining_time;
		hsl_value.total_time = hsl_set->total_time;
		if (hsl_value.total_time.num_steps == 0 ||
			hsl_value.remaining_time.num_steps == 0) {
			hsl_value.hue = hsl_set->hue;
			hsl_value.lightness = hsl_set->lightness;
			hsl_value.saturation = hsl_set->saturation;
		} else {
			if (hsl_value.total_time.num_steps == hsl_value.remaining_time.num_steps) {
				pre_hsl_hue = hsl_value.hue;
				pre_hsl_lightness = hsl_value.lightness;
				pre_hsl_saturation = hsl_value.saturation;
			}
			hsl_value.hue =
				hsl_set->hue - (hsl_set->hue - pre_hsl_hue) * ((double)hsl_value.remaining_time.num_steps) /
				hsl_value.total_time.num_steps;
			hsl_value.lightness =
				hsl_set->lightness - (hsl_set->lightness - pre_hsl_lightness) * ((double)hsl_value.remaining_time.num_steps) /
				hsl_value.total_time.num_steps;
			hsl_value.saturation =
				hsl_set->saturation - (hsl_set->saturation - pre_hsl_saturation) * ((double)hsl_value.remaining_time.num_steps) /
				hsl_value.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting hsl server receive: set hue %d, saturation %d, lightness %d, remaining_time %d, total_time %d \r\n",
				hsl_value.hue,
				hsl_value.saturation,
				hsl_value.lightness,
				hsl_value.remaining_time.num_steps,
				hsl_value.total_time.num_steps);
		BT_AT_PRINT("+BLEMESHLHSL:lh,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, hsl_value.hue, hsl_value.saturation, hsl_value.lightness,
					hsl_value.remaining_time.num_steps, hsl_value.total_time.num_steps);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light hsl hue server app callback***/
static rtk_bt_mesh_light_hsl_server_set_hue_t hue_store = {0};
static rtk_bt_mesh_light_hsl_server_get_range_t hue_range = {0};
static uint16_t pre_hue = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_hue_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_hsl_server_direct_get_hue_t *hue_get = (rtk_bt_mesh_light_hsl_server_direct_get_hue_t *)param;
		rtk_bt_mesh_light_hsl_server_get_hue_t hue_value;
		hue_value.hue = hue_store.hue;
		memcpy(hue_get->value, &hue_value, sizeof(rtk_bt_mesh_light_hsl_server_get_hue_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_hsl_server_set_hue_t *hue_set = (rtk_bt_mesh_light_hsl_server_set_hue_t *)param;
		hue_store.remaining_time = hue_set->remaining_time;
		hue_store.total_time = hue_set->total_time;
		if (hue_store.total_time.num_steps == 0 ||
			hue_store.remaining_time.num_steps == 0) {
			hue_store.hue = hue_set->hue;
		} else {
			if (hue_store.total_time.num_steps == hue_store.remaining_time.num_steps) {
				pre_hue = hue_store.hue;
			}
			hue_store.hue =
				hue_set->hue - (hue_set->hue - pre_hue) * ((double)hue_store.remaining_time.num_steps) /
				hue_store.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting hsl hue server receive: set hue %d, remaining_time %d, total_time %d \r\n",
				hue_store.hue,
				hue_store.remaining_time.num_steps,
				hue_store.total_time.num_steps);
		BT_AT_PRINT("+BLEMESHLHSL:lhh,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, hue_store.hue,
					hue_store.remaining_time.num_steps, hue_store.total_time.num_steps);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_H: {
		rtk_bt_mesh_light_hsl_server_direct_get_range_t *range_get = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)param;
		memcpy(range_get->value, &hue_range, sizeof(rtk_bt_mesh_light_hsl_server_get_range_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light hsl saturation server app callback***/
static rtk_bt_mesh_light_hsl_server_set_saturation_t saturation_store = {0};
static rtk_bt_mesh_light_hsl_server_get_range_t saturation_range = {0};
static uint16_t pre_saturation = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_saturation_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *saturation_get = (rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *)param;
		rtk_bt_mesh_light_hsl_server_get_saturation_t saturation_value;
		saturation_value.saturation = saturation_store.saturation;
		memcpy(saturation_get->value, &saturation_value, sizeof(rtk_bt_mesh_light_hsl_server_get_saturation_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_hsl_server_set_saturation_t *saturation_set = (rtk_bt_mesh_light_hsl_server_set_saturation_t *)param;
		saturation_store.remaining_time = saturation_set->remaining_time;
		saturation_store.total_time = saturation_set->total_time;
		if (saturation_store.total_time.num_steps == 0 ||
			saturation_store.remaining_time.num_steps == 0) {
			saturation_store.saturation = saturation_set->saturation;
		} else {
			if (saturation_store.total_time.num_steps == saturation_store.remaining_time.num_steps) {
				pre_saturation = saturation_store.saturation;
			}
			saturation_store.saturation =
				saturation_set->saturation - (saturation_set->saturation - pre_saturation) * ((double)saturation_store.remaining_time.num_steps) /
				saturation_store.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting hsl saturation server receive: set saturation %d, remaining_time %d, total_time %d \r\n",
				saturation_store.saturation,
				saturation_store.remaining_time.num_steps,
				saturation_store.total_time.num_steps);
		BT_AT_PRINT("+BLEMESHLHSL:lhs,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, saturation_store.saturation,
					saturation_store.remaining_time.num_steps, saturation_store.total_time.num_steps);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_S: {
		rtk_bt_mesh_light_hsl_server_direct_get_range_t *range_get = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)param;
		memcpy(range_get->value, &saturation_range, sizeof(rtk_bt_mesh_light_hsl_server_get_range_t));
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
static rtk_bt_mesh_light_hsl_server_get_default_t hsl_setup_default = {0};
static rtk_bt_mesh_light_hsl_server_get_range_t hsl_setup_range = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_hsl_server_set_default_t *default_set = (rtk_bt_mesh_light_hsl_server_set_default_t *)param;
		BT_LOGA("[APP] lighting hsl setup server receive: set default lightness %d, hue %d, saturation %d \r\n",
				default_set->lightness,
				default_set->hue,
				default_set->saturation);
		BT_AT_PRINT("+BLEMESHLHSL:lhd,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, default_set->lightness,
					default_set->hue, default_set->saturation);
		hsl_setup_default.lightness = default_set->lightness;
		hsl_setup_default.hue = default_set->hue;
		hsl_setup_default.saturation = default_set->saturation;
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_hsl_server_set_range_t *range_set = (rtk_bt_mesh_light_hsl_server_set_range_t *)param;
		BT_LOGA("[APP] lighting hsl setup server receive: set hue_range_min %d, hue_range_max %d, saturation_range_min %d, saturation_range_max %d \r\n",
				range_set->hue_range_min,
				range_set->hue_range_max,
				range_set->saturation_range_min,
				range_set->saturation_range_max);
		BT_AT_PRINT("+BLEMESHLHSL:lhr,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, range_set->hue_range_min, range_set->hue_range_max,
					range_set->saturation_range_min, range_set->saturation_range_max);
		hsl_setup_range.hue_range_min = range_set->hue_range_min;
		hsl_setup_range.hue_range_max = range_set->hue_range_max;
		hsl_setup_range.saturation_range_min = range_set->saturation_range_min;
		hsl_setup_range.saturation_range_max = range_set->saturation_range_max;
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light xyl server app callback***/
static rtk_bt_mesh_light_xyl_server_set_t xyl_value = {0};
static rtk_bt_mesh_light_xyl_server_get_default_t xyl_default = {0};
static rtk_bt_mesh_light_xyl_server_get_range_t xyl_range = {0};

static uint16_t pre_xyl_x = 0;
static uint16_t pre_xyl_y = 0;
static uint16_t pre_xyl_lightness = 0;

static rtk_bt_evt_cb_ret_t ble_mesh_light_xyl_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_xyl_server_direct_get_t *xyl_get = (rtk_bt_mesh_light_xyl_server_direct_get_t *)param;
		memcpy(xyl_get->value, &(xyl_value.xyl), sizeof(rtk_bt_mesh_light_xyl_server_get_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_xyl_server_direct_get_default_t *default_get = (rtk_bt_mesh_light_xyl_server_direct_get_default_t *)param;
		memcpy(default_get->value, &xyl_default, sizeof(rtk_bt_mesh_light_xyl_server_get_default_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_GET: {
		rtk_bt_mesh_light_xyl_server_direct_get_range_t *range_get = (rtk_bt_mesh_light_xyl_server_direct_get_range_t *)param;
		memcpy(range_get->value, &xyl_range, sizeof(rtk_bt_mesh_light_xyl_server_get_range_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_xyl_server_set_t *xyl_set = (rtk_bt_mesh_light_xyl_server_set_t *)param;
		xyl_value.total_time = xyl_set->total_time;
		xyl_value.remaining_time = xyl_set->remaining_time;
		if (xyl_value.total_time.num_steps == 0 ||
			xyl_value.remaining_time.num_steps == 0) {
			xyl_value.xyl = xyl_set->xyl;
		} else {
			if (xyl_value.total_time.num_steps == xyl_value.remaining_time.num_steps) {
				pre_xyl_lightness = xyl_value.xyl.xyl_lightness;
				pre_xyl_x = xyl_value.xyl.xyl_x;
				pre_xyl_y = xyl_value.xyl.xyl_y;
			}
			xyl_value.xyl.xyl_x =
				(xyl_set->xyl).xyl_x - ((xyl_set->xyl).xyl_x - pre_xyl_x) * ((double)xyl_value.remaining_time.num_steps) /
				xyl_value.total_time.num_steps;
			xyl_value.xyl.xyl_y =
				(xyl_set->xyl).xyl_y - ((xyl_set->xyl).xyl_y - pre_xyl_y) * ((double)xyl_value.remaining_time.num_steps) /
				xyl_value.total_time.num_steps;
			xyl_value.xyl.xyl_lightness =
				(xyl_set->xyl).xyl_lightness - ((xyl_set->xyl).xyl_lightness - pre_xyl_lightness) * ((double)xyl_value.remaining_time.num_steps) /
				xyl_value.total_time.num_steps;
		}
		BT_LOGA("[APP] lighting xyl server receive: set xyl_x %d, xyl_y %d, xyl_lightness %d, total_time %d, remaining_time %d \r\n",
				xyl_value.xyl.xyl_x,
				xyl_value.xyl.xyl_y,
				xyl_value.xyl.xyl_lightness,
				(xyl_set->total_time).num_steps,
				(xyl_set->remaining_time).num_steps);
		BT_AT_PRINT("+BLEMESHLXYL:lx,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, xyl_value.xyl.xyl_x, xyl_value.xyl.xyl_y, xyl_value.xyl.xyl_lightness,
					(xyl_set->total_time).num_steps, (xyl_set->remaining_time).num_steps);
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
static rtk_bt_mesh_light_xyl_server_get_default_t xyl_setup_default = {0};
static rtk_bt_mesh_light_xyl_server_get_range_t xyl_setup_range = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_light_xyl_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_xyl_server_set_default_t *default_set = (rtk_bt_mesh_light_xyl_server_set_default_t *)param;
		xyl_setup_default.xyl_lightness = default_set->xyl_lightness;
		xyl_setup_default.xyl_x = default_set->xyl_x;
		xyl_setup_default.xyl_y = default_set->xyl_y;
		BT_LOGA("[APP] lighting xyl server receive: default xyl_lightness %d, xyl_x %d, xyl_y %d \r\n",
				default_set->xyl_lightness,
				default_set->xyl_x,
				default_set->xyl_y);
		BT_AT_PRINT("+BLEMESHLXYL:lxd,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, default_set->xyl_lightness, default_set->xyl_x, default_set->xyl_y);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_SET: {
		rtk_bt_mesh_light_xyl_server_set_range_t *range_set = (rtk_bt_mesh_light_xyl_server_set_range_t *)param;
		xyl_setup_range.xyl_x_range_max = range_set->xyl_x_range_max;
		xyl_setup_range.xyl_x_range_min = range_set->xyl_x_range_min;
		xyl_setup_range.xyl_y_range_max = range_set->xyl_y_range_max;
		xyl_setup_range.xyl_y_range_min = range_set->xyl_y_range_min;
		BT_LOGA("[APP] lighting xyl setup server receive: set xyl_x_range_max %d, xyl_x_range_min %d, xyl_y_range_max %d, xyl_y_range_min %d \r\n",
				range_set->xyl_x_range_max,
				range_set->xyl_x_range_min,
				range_set->xyl_y_range_max,
				range_set->xyl_y_range_min);
		BT_AT_PRINT("+BLEMESHLXYL:lxr,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, range_set->xyl_x_range_min, range_set->xyl_x_range_max,
					range_set->xyl_y_range_min, range_set->xyl_y_range_max);
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light lc server app callback***/
static rtk_bt_mesh_light_lc_server_set_mode_t lc_mode = {0};
static rtk_bt_mesh_light_lc_server_set_om_t lc_om = {0};
static rtk_bt_mesh_light_lc_server_set_light_on_off_t lc_onoff = {0};

static rtk_bt_evt_cb_ret_t ble_mesh_light_lc_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_GET: {
		rtk_bt_mesh_light_lc_server_direct_get_mode_t *mode_get = (rtk_bt_mesh_light_lc_server_direct_get_mode_t *)param;
		memcpy(mode_get->value, &lc_mode, sizeof(rtk_bt_mesh_light_lc_server_get_mode_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_SET: {
		rtk_bt_mesh_light_lc_server_set_mode_t *mode_set = (rtk_bt_mesh_light_lc_server_set_mode_t *)param;
		BT_LOGA("[APP] lighting lc server receive: set light_lc mode %d \r\n",
				mode_set->mode);
		BT_AT_PRINT("+BLEMESHLLC:llcm,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, mode_set->mode);
		lc_mode.mode = mode_set->mode;
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_GET: {
		rtk_bt_mesh_light_lc_server_direct_get_om_t *om_get = (rtk_bt_mesh_light_lc_server_direct_get_om_t *)param;
		memcpy(om_get->value, &lc_om, sizeof(rtk_bt_mesh_light_lc_server_get_om_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_SET: {
		rtk_bt_mesh_light_lc_server_set_om_t *om_set = (rtk_bt_mesh_light_lc_server_set_om_t *)param;
		BT_LOGA("[APP] lighting lc server receive: set light_lc om %d \r\n",
				om_set->mode);
		BT_AT_PRINT("+BLEMESHLLC:llcom,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, om_set->mode);
		lc_om.mode = om_set->mode;
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_GET: {
		rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t *light_get = (rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t *)param;
		memcpy(light_get->value, &(lc_onoff.light_on_off), sizeof(rtk_bt_mesh_light_lc_server_get_light_on_off_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_SM_TRANSITION_TIME_GET: {
		rtk_bt_mesh_light_lc_server_direct_get_sm_transition_time_t *sm_get = (rtk_bt_mesh_light_lc_server_direct_get_sm_transition_time_t *)param;
		rtk_bt_mesh_light_lc_server_get_sm_transition_time_t sm_value;
		sm_value.light_on_off = lc_onoff.light_on_off;
		sm_value.sm_trans_time = lc_onoff.remaining_time;
		memcpy(sm_get->value, &sm_value, sizeof(rtk_bt_mesh_light_lc_server_get_sm_transition_time_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_SET: {
		rtk_bt_mesh_light_lc_server_set_light_on_off_t *light_set = (rtk_bt_mesh_light_lc_server_set_light_on_off_t *)param;
		lc_onoff.total_time = light_set->total_time;
		lc_onoff.remaining_time = light_set->remaining_time;
		if (lc_onoff.total_time.num_steps == 0 ||
			lc_onoff.remaining_time.num_steps == 0) {
			lc_onoff.light_on_off = light_set->light_on_off;
			BT_LOGA("[APP] lighting lc server receive: set light_on_off %d, total_time %d, remaining_time %d \r\n",
					lc_onoff.light_on_off,
					(light_set->total_time).num_steps,
					(light_set->remaining_time).num_steps);
			BT_AT_PRINT("+BLEMESHLLC:llcloo,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_SERVER, lc_onoff.light_on_off,
						(light_set->total_time).num_steps, (light_set->remaining_time).num_steps);
		} else if (lc_onoff.total_time.num_steps == lc_onoff.remaining_time.num_steps) {
			if (light_set->light_on_off == RTK_BT_MESH_GENERIC_ON  && lc_onoff.light_on_off != light_set->light_on_off) {
				lc_onoff.light_on_off = light_set->light_on_off;
			}
			BT_LOGA("[APP] lighting lc server receive: set light_on_off %d, total_time %d, remaining_time %d \r\n",
					lc_onoff.light_on_off,
					(light_set->total_time).num_steps,
					(light_set->remaining_time).num_steps);
			BT_AT_PRINT("+BLEMESHLLC:llcloo,%d,%d,%d,%d\r\n",
						BT_AT_MESH_ROLE_SERVER, lc_onoff.light_on_off,
						(light_set->total_time).num_steps, (light_set->remaining_time).num_steps);
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL

#if defined(BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in light lc setup server app callback***/
static rtk_bt_mesh_light_lc_server_get_property_t property_store[2] = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_light_lc_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_GET: {
		rtk_bt_mesh_light_lc_server_direct_get_property_t *property_get = (rtk_bt_mesh_light_lc_server_direct_get_property_t *)param;
		BT_LOGA("[APP] Light lc setup server model get property id: 0x%x \r\n", property_get->property_id);
		BT_AT_PRINT("+BLEMESHLLC:llcpg,%d,0x%x\r\n", BT_AT_MESH_ROLE_SERVER, property_get->property_id);
		rtk_bt_mesh_light_lc_server_direct_get_property_para_t property;
		if ((property_get->property_id >= 0x2E) && (property_get->property_id <= 0x30)) {
			property.property_value = property_store[0].property_value;
			property.value_len = property_store[0].value_len;
		} else if ((property_get->property_id >= 0x36) && (property_get->property_id <= 0x3c)) {
			property.property_value = property_store[1].property_value;
			property.value_len = property_store[1].value_len;
		} else {
			property.value_len = 0;
		}
		memcpy(property_get->value, &property, sizeof(rtk_bt_mesh_light_lc_server_direct_get_property_para_t));
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_SET: {
		rtk_bt_mesh_light_lc_server_set_property_t *property_set = (rtk_bt_mesh_light_lc_server_set_property_t *)param;
		BT_LOGA("[APP] Light lc setup server receive: set property id %d, property value %d \r\n",
				property_set->property_id,
				property_set->property_value);
		BT_AT_PRINT("+BLEMESHLLC:llcps,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, property_set->property_id, property_set->property_value);
		if ((property_set->property_id >= 0x2E) && (property_set->property_id <= 0x30)) {
			property_store[0].property_value = property_set->property_value;
			property_store[0].value_len = 2;
		} else if ((property_set->property_id >= 0x36) && (property_set->property_id <= 0x3c)) {
			property_store[1].property_value = property_set->property_value;
			property_store[1].value_len = 3;
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in time server app callback***/
static rtk_bt_mesh_time_server_get_t time_store = {0};
static rtk_bt_mesh_time_server_get_zone_t zone_store = {0};
static rtk_bt_mesh_time_server_get_tai_utc_delta_t delta_store = {0};
static rtk_bt_mesh_time_role_t role_srv_store = {0};

static rtk_bt_evt_cb_ret_t ble_mesh_time_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_TIME_SERVER_MODEL_GET: {
		rtk_bt_mesh_time_server_direct_get_t *time_get = (rtk_bt_mesh_time_server_direct_get_t *)param;
		memcpy(time_get->value, &time_store, sizeof(rtk_bt_mesh_time_server_get_t));
		break;
	}
	case RTK_BT_MESH_TIME_SERVER_MODEL_ZONE_GET: {
		rtk_bt_mesh_time_server_direct_get_zone_t *zone_get = (rtk_bt_mesh_time_server_direct_get_zone_t *)param;
		memcpy(zone_get->value, &zone_store, sizeof(rtk_bt_mesh_time_server_get_zone_t));
		break;
	}
	case RTK_BT_MESH_TIME_SERVER_MODEL_TAI_UTC_DELTA_GET: {
		rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *delta_get = (rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *)param;
		memcpy(delta_get->value, &delta_store, sizeof(rtk_bt_mesh_time_server_get_tai_utc_delta_t));
		break;
	}
	case RTK_BT_MESH_TIME_SERVER_MODEL_STATUS_SET: {
		rtk_bt_mesh_time_server_status_set_t *time_set = (rtk_bt_mesh_time_server_status_set_t *)param;
		memcpy(time_store.tai_seconds, time_set->tai_seconds, 5);
		time_store.subsecond = time_set->subsecond;
		time_store.uncertainty = time_set->uncertainty;
		time_store.tai_utc_delta = time_set->tai_utc_delta;
		time_store.time_zone_offset = time_set->time_zone_offset;
		BT_LOGA("[APP] Time server receive: set subsecond %d uncertainty %d tai_utc_delta %d time_zone_offset %d\r\n",
				time_store.subsecond,
				time_store.uncertainty,
				time_store.tai_utc_delta,
				time_store.time_zone_offset);
		break;
	}
	case RTK_BT_MESH_TIME_SERVER_MODEL_ROLE_GET: {
		rtk_bt_mesh_time_server_get_role_t *time_role_get = (rtk_bt_mesh_time_server_get_role_t *)param;
		memcpy(time_role_get->role, &role_srv_store, sizeof(rtk_bt_mesh_time_role_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_TIME_SERVER_MODEL

#if defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in time setup server app callback***/
static rtk_bt_mesh_time_server_set_t time_setup_store = {0};
static rtk_bt_mesh_time_server_set_zone_t time_zone_store = {0};
static rtk_bt_mesh_time_role_t role_store = {0};
static rtk_bt_mesh_time_server_set_tai_utc_delta_t time_delta_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_time_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_SET: {
		rtk_bt_mesh_time_server_set_t *time_set = (rtk_bt_mesh_time_server_set_t *)param;
		memcpy(&time_setup_store, time_set, sizeof(rtk_bt_mesh_time_server_set_t));
		BT_LOGA("[APP] Time setup server receive: set subsecond %d uncertainty %d tai_utc_delta %d time_zone_offset %d time_authority %d\r\n",
				time_setup_store.subsecond,
				time_setup_store.uncertainty,
				time_setup_store.tai_utc_delta,
				time_setup_store.time_zone_offset,
				time_setup_store.time_authority);
		BT_LOGA("tai_seconds[4]%d tai_seconds[3]%d tai_seconds[2]%d tai_seconds[1]%d tai_seconds[0]%d \r\n",
				time_setup_store.tai_seconds[4],
				time_setup_store.tai_seconds[3],
				time_setup_store.tai_seconds[2],
				time_setup_store.tai_seconds[1],
				time_setup_store.tai_seconds[0]);
		BT_AT_PRINT("+BLEMESHTIME:time,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, time_setup_store.subsecond, time_setup_store.uncertainty,
					time_setup_store.tai_utc_delta, time_setup_store.time_zone_offset, time_setup_store.time_authority,
					time_setup_store.tai_seconds[4], time_setup_store.tai_seconds[3], time_setup_store.tai_seconds[2],
					time_setup_store.tai_seconds[1], time_setup_store.tai_seconds[0]);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_SET: {
		rtk_bt_mesh_time_server_set_zone_t *time_zone_set = (rtk_bt_mesh_time_server_set_zone_t *)param;
		memcpy(&time_zone_store, time_zone_set, sizeof(rtk_bt_mesh_time_server_set_zone_t));
		BT_LOGA("[APP] Time setup server receive: set time_zone_offset %d \r\n", time_zone_store.time_zone_offset_new);
		BT_LOGA("tai_of_zone_change[4]%d tai_of_zone_change[3]%d tai_of_zone_change[2]%d tai_of_zone_change[1]%d tai_of_zone_change[0]%d \r\n",
				time_zone_store.tai_of_zone_change[4],
				time_zone_store.tai_of_zone_change[3],
				time_zone_store.tai_of_zone_change[2],
				time_zone_store.tai_of_zone_change[1],
				time_zone_store.tai_of_zone_change[0]);
		BT_AT_PRINT("+BLEMESHTIME:tz,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, time_zone_store.time_zone_offset_new, time_zone_store.tai_of_zone_change[4],
					time_zone_store.tai_of_zone_change[3], time_zone_store.tai_of_zone_change[2],
					time_zone_store.tai_of_zone_change[1], time_zone_store.tai_of_zone_change[0]);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_GET: {
		rtk_bt_mesh_time_server_get_role_t *time_role_get = (rtk_bt_mesh_time_server_get_role_t *)param;
		memcpy(time_role_get->role, &role_store, sizeof(rtk_bt_mesh_time_role_t));
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_SET: {
		rtk_bt_mesh_time_server_set_role_t *time_role_set = (rtk_bt_mesh_time_server_set_role_t *)param;
		role_store = time_role_set->role;
		BT_LOGA("[APP] Time setup server receive: set role %d \r\n", role_store);
		BT_AT_PRINT("+BLEMESHTIME:tr,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, role_store);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_SET: {
		rtk_bt_mesh_time_server_set_tai_utc_delta_t *delta_set = (rtk_bt_mesh_time_server_set_tai_utc_delta_t *)param;
		memcpy(&time_delta_store, delta_set, sizeof(rtk_bt_mesh_time_server_set_tai_utc_delta_t));
		BT_LOGA("[APP] Time setup server receive: set tai_utc_delta_new %d \r\n", time_delta_store.tai_utc_delta_new);
		BT_LOGA("tai_of_delta_change[4]%d tai_of_delta_change[3]%d tai_of_delta_change[2]%d tai_of_delta_change[1]%d tai_of_delta_change[0]%d \r\n",
				time_delta_store.tai_of_delta_change[4],
				time_delta_store.tai_of_delta_change[3],
				time_delta_store.tai_of_delta_change[2],
				time_delta_store.tai_of_delta_change[1],
				time_delta_store.tai_of_delta_change[0]);
		BT_AT_PRINT("+BLEMESHTIME:ttud,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, time_delta_store.tai_utc_delta_new, time_delta_store.tai_of_delta_change[4],
					time_delta_store.tai_of_delta_change[3], time_delta_store.tai_of_delta_change[2],
					time_delta_store.tai_of_delta_change[1], time_delta_store.tai_of_delta_change[0]);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_GET: {
		rtk_bt_mesh_time_server_direct_get_zone_t *zone_get = (rtk_bt_mesh_time_server_direct_get_zone_t *)param;
		(zone_get->value)->time_zone_offset_current = time_zone_store.time_zone_offset_new;
		(zone_get->value)->time_zone_offset_new = time_zone_store.time_zone_offset_new;
		memcpy((zone_get->value)->tai_of_zone_change, time_zone_store.tai_of_zone_change, 5);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_GET: {
		rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *delta_get = (rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *)param;
		(delta_get->value)->tai_utc_delta_current = time_delta_store.tai_utc_delta_new;
		(delta_get->value)->tai_utc_delta_new = time_delta_store.tai_utc_delta_new;
		memcpy((delta_get->value)->tai_of_delta_change, time_delta_store.tai_of_delta_change, 5);
		break;
	}
	case RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_GET: {
		rtk_bt_mesh_time_server_direct_get_t *time_get = (rtk_bt_mesh_time_server_direct_get_t *)param;
		memcpy(time_get->value, &time_setup_store, sizeof(rtk_bt_mesh_time_server_get_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in scheduler server app callback***/
static uint16_t schedulers_store = 0x3ff;
static rtk_bt_mesh_scheduler_register_t scheduler_store[2] = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_scheduler_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCHEDULER_SERVER_MODEL_GET: {
		rtk_bt_mesh_scheduler_server_get_t *scheduler_get = (rtk_bt_mesh_scheduler_server_get_t *)param;
		memcpy(scheduler_get->schedulers, &schedulers_store, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_GET: {
		rtk_bt_mesh_scheduler_server_get_action_t *action_get = (rtk_bt_mesh_scheduler_server_get_action_t *)param;
		if (action_get->index == 1) {
			memcpy(action_get->scheduler, &scheduler_store[0], sizeof(rtk_bt_mesh_scheduler_register_t));
		} else {
			memcpy(action_get->scheduler, &scheduler_store[1], sizeof(rtk_bt_mesh_scheduler_register_t));
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in scheduler setup server app callback***/
static rtk_bt_mesh_scheduler_register_t scheduler_setup_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_scheduler_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_SET: {
		rtk_bt_mesh_scheduler_server_set_action_t *scheduler_set = (rtk_bt_mesh_scheduler_server_set_action_t *)param;
		memcpy(&scheduler_setup_store, scheduler_set, sizeof(rtk_bt_mesh_scheduler_register_t));
		BT_LOGA("[APP] scheduler client receive: " \
				"index = %d, year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d, " \
				"day_of_week = %d, action = %d, num_steps = %d, step_resolution = %d, scene_number = %d\r\n", \
				scheduler_setup_store.index, scheduler_setup_store.year, scheduler_setup_store.month, \
				scheduler_setup_store.day, scheduler_setup_store.hour, scheduler_setup_store.minute, scheduler_setup_store.second, \
				scheduler_setup_store.day_of_week, scheduler_setup_store.action, scheduler_setup_store.num_steps, \
				scheduler_setup_store.step_resolution, scheduler_setup_store.scene_number);
		BT_AT_PRINT("+BLEMESHSCHEDULER:schea,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, scheduler_setup_store.index, scheduler_setup_store.year,
					scheduler_setup_store.month, scheduler_setup_store.day, scheduler_setup_store.hour,
					scheduler_setup_store.minute, scheduler_setup_store.second, scheduler_setup_store.day_of_week,
					scheduler_setup_store.action, scheduler_setup_store.num_steps, scheduler_setup_store.step_resolution,
					scheduler_setup_store.scene_number);
		break;
	}
	case RTK_BT_MESH_SCHEDULER_SETUP_SERVER_MODEL_GET_ACTION: {
		rtk_bt_mesh_scheduler_server_get_action_t *action_get = (rtk_bt_mesh_scheduler_server_get_action_t *)param;
		memcpy(action_get->scheduler, &scheduler_setup_store, sizeof(rtk_bt_mesh_scheduler_register_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in scene server app callback***/
static uint16_t current_scene = 1;
static rtk_bt_mesh_scene_server_recall_t recall_store = {0};
static rtk_bt_mesh_scene_status_code_t register_status = 0;
static uint16_t scenes_nums = 2;
static uint16_t scenes_index[2] = {1, 2};
static rtk_bt_mesh_scene_status_code_t recall_status = 1;
static rtk_bt_evt_cb_ret_t ble_mesh_scene_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCENE_SERVER_MODEL_GET: {
		rtk_bt_mesh_scene_server_get_t *scene_get = (rtk_bt_mesh_scene_server_get_t *)param;
		memcpy(scene_get->current_scene, &current_scene, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_SCENE_SERVER_MODEL_RECALL: {
		rtk_bt_mesh_scene_server_recall_t *scene_recall = (rtk_bt_mesh_scene_server_recall_t *)param;
		memcpy(&recall_store, scene_recall, sizeof(rtk_bt_mesh_scene_server_recall_t));
		BT_LOGA("[APP] scene server receive: scene number:%d, total time:%d , remain time %d \r\n",
				recall_store.scene_number, recall_store.total_time.num_steps, recall_store.remaining_time.num_steps);
		current_scene = recall_store.scene_number;
		uint8_t *pmemory = (uint8_t *)param + 4;
		mesh_data_uart_dump(pmemory, SCENE_DATA_MAX_LEN);
		BT_LOGA("\r\n");
		BT_AT_PRINT("+BLEMESHSCENE:sr,%d,%d,%d,%d,",
					BT_AT_MESH_ROLE_SERVER, recall_store.scene_number,
					recall_store.total_time.num_steps, recall_store.remaining_time.num_steps);
		BT_AT_DUMP_HEXN(pmemory, SCENE_DATA_MAX_LEN);
		break;
	}
	case RTK_BT_MESH_SCENE_SERVER_MODEL_GET_REGISTER_STATUS: {
		rtk_bt_mesh_scene_server_get_register_status_t *register_get = (rtk_bt_mesh_scene_server_get_register_status_t *)param;
		memcpy(register_get->status, &register_status, sizeof(rtk_bt_mesh_scene_status_code_t));
		break;
	}
	case RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES_NUM: {
		rtk_bt_mesh_scene_server_get_scenes_num_t *num_get = (rtk_bt_mesh_scene_server_get_scenes_num_t *)param;
		memcpy(num_get->num_scenes, &scenes_nums, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES: {
		rtk_bt_mesh_scene_server_get_scenes_t *scenes_get = (rtk_bt_mesh_scene_server_get_scenes_t *)param;
		uint16_t num = scenes_get->num_scenes;
		if (num > SCENE_DATA_MAX_LEN) {
			num = SCENE_DATA_MAX_LEN;
		}
		memcpy(scenes_get->scenes, scenes_index, sizeof(uint16_t) * num);
		break;
	}
	case RTK_BT_MESH_SCENE_SERVER_MODEL_GET_STATUS_RECALL: {
		rtk_bt_mesh_scene_server_get_status_recall_t *status_register_get = (rtk_bt_mesh_scene_server_get_status_recall_t *)param;
		BT_LOGA("[APP] scene server receive: scene number: %d \r\n", status_register_get->scene_number);
		memcpy(status_register_get->status_recall, &recall_status, sizeof(rtk_bt_mesh_scene_status_code_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SCENE_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in scene setup server app callback***/
static uint8_t store_scene_value[] = {0x0a, 0x00, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb};
static rtk_bt_mesh_scene_server_delete_t delete_store = {0};
static uint16_t setup_current_scene = 1;
static uint16_t scenes_setup_nums = 2;
static uint16_t scenes_setup_index[2] = {2, 3};
static rtk_bt_mesh_scene_status_code_t setup_register_status = 1;
static rtk_bt_evt_cb_ret_t ble_mesh_scene_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_STORE: {
		rtk_bt_mesh_scene_server_store_t *store_set = (rtk_bt_mesh_scene_server_store_t *)param;
		uint16_t length = sizeof(store_scene_value) / sizeof(store_scene_value[0]);
		if (length > SCENE_DATA_MAX_LEN) {
			length = SCENE_DATA_MAX_LEN;
		}
		memcpy(store_set->pmemory, &store_scene_value, length);
		BT_LOGA("[APP] scene setup server receive: status:%d, scene number:%d \r\n",
				store_set->status, store_set->scene_number);
		BT_AT_PRINT("+BLEMESHSCENE:ss,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, store_set->status, store_set->scene_number);
		break;
	}
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_DELETE: {
		rtk_bt_mesh_scene_server_delete_t *scene_delete = (rtk_bt_mesh_scene_server_delete_t *)param;
		memcpy(&delete_store, scene_delete, sizeof(rtk_bt_mesh_scene_server_delete_t));
		BT_LOGA("[APP] scene setup server receive: scene number:%d \r\n", delete_store.scene_number);
		BT_AT_PRINT("+BLEMESHSCENE:sd,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, delete_store.scene_number);
		break;
	}
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET: {
		rtk_bt_mesh_scene_server_get_t *scene_setup_get = (rtk_bt_mesh_scene_server_get_t *)param;
		memcpy(scene_setup_get->current_scene, &setup_current_scene, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES_NUM: {
		rtk_bt_mesh_scene_server_get_scenes_num_t *num_get = (rtk_bt_mesh_scene_server_get_scenes_num_t *)param;
		memcpy(num_get->num_scenes, &scenes_setup_nums, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES: {
		rtk_bt_mesh_scene_server_get_scenes_t *scenes_get = (rtk_bt_mesh_scene_server_get_scenes_t *)param;
		memcpy(scenes_get->scenes, scenes_setup_index, sizeof(uint16_t) * (scenes_get->num_scenes));
		break;
	}
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_STATUS_REGISTER: {
		rtk_bt_mesh_scene_setup_server_get_register_status_t *status_register_get = (rtk_bt_mesh_scene_setup_server_get_register_status_t *)param;
		BT_LOGA("[APP] scene setup server receive: scene number: %d \r\n", status_register_get->scene_number);
		memcpy(status_register_get->status, &setup_register_status, sizeof(rtk_bt_mesh_scene_status_code_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic default transition time server app callback***/
static rtk_bt_mesh_generic_transition_time_t trans_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_default_transition_time_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_GET: {
		rtk_bt_mesh_generic_default_transition_time_server_get_t *time_get = (rtk_bt_mesh_generic_default_transition_time_server_get_t *)param;
		memcpy(time_get->trans_time, &trans_store, sizeof(rtk_bt_mesh_generic_transition_time_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_SET: {
		rtk_bt_mesh_generic_default_transition_time_server_set_t *time_set = (rtk_bt_mesh_generic_default_transition_time_server_set_t *)param;
		memcpy(&trans_store, &(time_set->trans_time), sizeof(rtk_bt_mesh_generic_transition_time_t));
		BT_LOGA("[APP] generic default transition time server receive: resolution %d, num steps %d \r\n",
				trans_store.step_resolution, trans_store.num_steps);
		BT_AT_PRINT("+BLEMESHGDTT:gdtt,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, trans_store.step_resolution, trans_store.num_steps);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic level server app callback***/
static rtk_bt_mesh_generic_level_server_set_t level_store = {0};
static int16_t pre_level = 0;
static rtk_bt_mesh_generic_level_server_set_move_t move_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_level_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_GET: {
		rtk_bt_mesh_generic_level_server_get_t *level_get = (rtk_bt_mesh_generic_level_server_get_t *)param;
		memcpy(level_get->level, &level_store.level, sizeof(int16_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET: {
		rtk_bt_mesh_generic_level_server_set_t *level_set = (rtk_bt_mesh_generic_level_server_set_t *)param;
		level_store.total_time = level_set->total_time;
		level_store.remaining_time = level_set->remaining_time;
		if (level_store.total_time.num_steps == 0 ||
			level_store.remaining_time.num_steps == 0) {
			level_store.level = level_set->level;
		} else {
			if (level_store.total_time.num_steps == level_store.remaining_time.num_steps) {
				pre_level = level_store.level;
			}
			level_store.level =
				level_set->level - (level_set->level - pre_level) * ((double)level_store.remaining_time.num_steps) / level_store.total_time.num_steps;
		}
		BT_LOGA("[APP] generic level server receive: level %d, total time %d, reamining time %d \r\n",
				level_store.level, level_store.total_time.num_steps, level_store.remaining_time.num_steps);
		BT_AT_PRINT("+BLEMESHGLE:gl,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, level_store.level, level_store.total_time.num_steps, level_store.remaining_time.num_steps);
		break;
	}
	case RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET_MOVE: {
		rtk_bt_mesh_generic_level_server_set_move_t *move_set = (rtk_bt_mesh_generic_level_server_set_move_t *)param;
		memcpy(&move_store, move_set, sizeof(rtk_bt_mesh_generic_level_server_set_move_t));
		BT_LOGA("[APP] generic level server receive: move delta %d, target level %d, total time %d, remaining time %d \r\n",
				move_store.move_delta, move_store.target_level,
				move_store.total_time.num_steps, move_store.remaining_time.num_steps);
		BT_AT_PRINT("+BLEMESHGLE:glms,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, move_store.move_delta, move_store.target_level,
					move_store.total_time.num_steps, move_store.remaining_time.num_steps);
		pre_level = level_store.level;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic power on off server app callback***/
static rtk_bt_mesh_generic_on_power_up_t on_power_up_store;
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_on_off_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_GET: {
		rtk_bt_mesh_generic_power_on_off_server_get_t *gpoo_get = (rtk_bt_mesh_generic_power_on_off_server_get_t *)param;
		memcpy(gpoo_get->on_power_up, &on_power_up_store, sizeof(rtk_bt_mesh_generic_on_power_up_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
static rtk_bt_mesh_generic_on_power_up_t on_power_up_setup_store;
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_on_off_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_SET: {
		rtk_bt_mesh_generic_power_on_off_server_set_t *gpoo_set = (rtk_bt_mesh_generic_power_on_off_server_set_t *)param;
		on_power_up_setup_store = gpoo_set->on_power_up;
		BT_LOGA("[APP] generic power on off setup server receive: on power up %d \r\n",
				on_power_up_setup_store);
		BT_AT_PRINT("+BLEMESHGPOO:gp,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, on_power_up_setup_store);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic power level server app callback***/
static rtk_bt_mesh_generic_power_level_server_set_t power_level_store = {0};
static rtk_bt_mesh_generic_power_level_server_get_range_t power_level_range_store = {0};
static uint16_t last_power_store = 0;
static uint16_t default_power_store = 0;
static uint16_t pre_power_level = 0;
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_level_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_GET: {
		rtk_bt_mesh_generic_power_level_server_get_t *gpl_get = (rtk_bt_mesh_generic_power_level_server_get_t *)param;
		memcpy(gpl_get->power, &power_level_store.power, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_SET: {
		rtk_bt_mesh_generic_power_level_server_set_t *gpl_set = (rtk_bt_mesh_generic_power_level_server_set_t *)param;
		power_level_store.total_time = gpl_set->total_time;
		power_level_store.remaining_time = gpl_set->remaining_time;
		if (power_level_store.total_time.num_steps == 0 ||
			power_level_store.remaining_time.num_steps == 0) {
			power_level_store.power = gpl_set->power;
		} else {
			if (power_level_store.total_time.num_steps == power_level_store.remaining_time.num_steps) {
				pre_power_level = power_level_store.power;
			}
			power_level_store.power =
				gpl_set->power - (gpl_set->power - pre_power_level) * ((double)power_level_store.remaining_time.num_steps) / power_level_store.total_time.num_steps;
		}
		BT_LOGA("[APP] Generic power level server model receive: set power %d, total time %d, remaining time %d \r\n",
				power_level_store.power, power_level_store.total_time.num_steps, power_level_store.remaining_time.num_steps);
		BT_AT_PRINT("+BLEMESHGPL:gpl,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, power_level_store.power,
					power_level_store.total_time.num_steps, power_level_store.remaining_time.num_steps);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_GET: {
		rtk_bt_mesh_generic_power_level_server_get_t *last_get = (rtk_bt_mesh_generic_power_level_server_get_t *)param;
		memcpy(last_get->power, &last_power_store, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_SET: {
		rtk_bt_mesh_generic_power_level_server_set_last_t *last_set = (rtk_bt_mesh_generic_power_level_server_set_last_t *)param;
		last_power_store = last_set->power;
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_DEFAULT_GET: {
		rtk_bt_mesh_generic_power_level_server_get_t *default_get = (rtk_bt_mesh_generic_power_level_server_get_t *)param;
		memcpy(default_get->power, &default_power_store, sizeof(uint16_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_RANGE_GET: {
		rtk_bt_mesh_generic_power_level_server_direct_get_range_t *range_get = (rtk_bt_mesh_generic_power_level_server_direct_get_range_t *)param;
		memcpy(range_get->value, &power_level_range_store, sizeof(rtk_bt_mesh_generic_power_level_server_get_range_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
static uint16_t default_power_setup_store = 0;
static rtk_bt_mesh_generic_power_level_server_get_range_t power_level_range_setup_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_level_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_DEFAULT_SET: {
		rtk_bt_mesh_generic_power_level_server_set_default_t *gpld_set = (rtk_bt_mesh_generic_power_level_server_set_default_t *)param;
		default_power_setup_store = gpld_set->power;
		BT_LOGA("[APP] generic power level setup server receive: set default %d \r\n",
				default_power_setup_store);
		BT_AT_PRINT("+BLEMESHGPL:gpld,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, default_power_setup_store);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_RANGE_SET: {
		rtk_bt_mesh_generic_power_level_server_set_range_t *gplr_set = (rtk_bt_mesh_generic_power_level_server_set_range_t *)param;
		memcpy(&power_level_range_setup_store, gplr_set, sizeof(rtk_bt_mesh_generic_power_level_server_get_range_t));
		BT_LOGA("[APP] generic power level setup server receive: set range min %d range max %d \r\n",
				power_level_range_setup_store.range_min, power_level_range_setup_store.range_max);
		BT_AT_PRINT("+BLEMESHGPL:gplr,%d,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER,
					power_level_range_setup_store.range_min, power_level_range_setup_store.range_max);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic battery server app callback***/
static rtk_bt_mesh_generic_battery_server_get_t battery_store = {1, 1, 1, {1, 1, 1, 0}};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_battery_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_GET: {
		rtk_bt_mesh_generic_battery_server_direct_get_t *gb_get = (rtk_bt_mesh_generic_battery_server_direct_get_t *)param;
		memcpy(gb_get->value, &battery_store, sizeof(rtk_bt_mesh_generic_battery_server_get_t));
		BT_LOGA("[APP] generic battery server receive: get battery level = %d, time to discharge = %d, \
time to charge = %d, presence = %d, indicator = %d, charging = %d, serviceability = %d\r\n",
				(gb_get->value)->battery_level,
				(gb_get->value)->time_to_discharge, (gb_get->value)->time_to_charge,
				(gb_get->value)->flags.presence, (gb_get->value)->flags.indicator, (gb_get->value)->flags.charging,
				(gb_get->value)->flags.serviceability);
		BT_AT_PRINT("+BLEMESHGB:gbg,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, (gb_get->value)->battery_level, (gb_get->value)->time_to_discharge,
					(gb_get->value)->time_to_charge, (gb_get->value)->flags.presence, (gb_get->value)->flags.indicator,
					(gb_get->value)->flags.charging, (gb_get->value)->flags.serviceability);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic location server app callback***/
static rtk_bt_mesh_generic_location_server_get_global_t global_location_store = {0};
static rtk_bt_mesh_generic_location_server_get_local_t local_location_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_location_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_GLOBAL_GET: {
		rtk_bt_mesh_generic_location_server_direct_get_global_t *global_get = (rtk_bt_mesh_generic_location_server_direct_get_global_t *)param;
		memcpy(global_get->value, &global_location_store, sizeof(rtk_bt_mesh_generic_location_server_get_global_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_LOCAL_GET: {
		rtk_bt_mesh_generic_location_server_direct_get_local_t *local_get = (rtk_bt_mesh_generic_location_server_direct_get_local_t *)param;
		memcpy(local_get->value, &local_location_store, sizeof(rtk_bt_mesh_generic_location_server_get_local_t));
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic location setup server app callback***/
static rtk_bt_mesh_generic_location_server_get_global_t global_location_setup_store = {0};
static rtk_bt_mesh_generic_location_server_get_local_t local_location_setup_store = {0};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_location_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_GET: {
		rtk_bt_mesh_generic_location_server_direct_get_global_t *global_get = (rtk_bt_mesh_generic_location_server_direct_get_global_t *)param;
		memcpy(global_get->value, &global_location_setup_store, sizeof(rtk_bt_mesh_generic_location_server_get_global_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_GET: {
		rtk_bt_mesh_generic_location_server_direct_get_local_t *local_get = (rtk_bt_mesh_generic_location_server_direct_get_local_t *)param;
		memcpy(local_get->value, &local_location_setup_store, sizeof(rtk_bt_mesh_generic_location_server_get_local_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_SET: {
		rtk_bt_mesh_generic_location_server_set_global_t *global_set = (rtk_bt_mesh_generic_location_server_set_global_t *)param;
		memcpy(&global_location_setup_store, global_set, sizeof(rtk_bt_mesh_generic_location_server_get_global_t));
		BT_LOGA("[APP] Generic location setup server receive: set global_latitude %d , global_longitude %d , lobal_altitude %d\r\n",
				global_location_setup_store.global_latitude,
				global_location_setup_store.global_longitude,
				global_location_setup_store.global_altitude);
		BT_AT_PRINT("+BLEMESHGLO:glg,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, global_location_setup_store.global_latitude,
					global_location_setup_store.global_longitude, global_location_setup_store.global_altitude);
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_SET: {
		rtk_bt_mesh_generic_location_server_set_local_t *local_set = (rtk_bt_mesh_generic_location_server_set_local_t *)param;
		memcpy(&local_location_setup_store, local_set, sizeof(rtk_bt_mesh_generic_location_server_get_local_t));
		BT_LOGA("[APP] Generic location setup server receive: set local_north %d , local_east %d , local_altitude %d , floor_num %d , stationary %d , precision %d , update_time %d \r\n",
				local_location_setup_store.local_north,
				local_location_setup_store.local_east,
				local_location_setup_store.local_altitude,
				local_location_setup_store.floor_num,
				local_location_setup_store.uncertainty.stationary,
				local_location_setup_store.uncertainty.precision,
				local_location_setup_store.uncertainty.update_time);
		BT_AT_PRINT("+BLEMESHGLO:gll,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, local_location_setup_store.local_north, local_location_setup_store.local_east,
					local_location_setup_store.local_altitude, local_location_setup_store.floor_num,
					local_location_setup_store.uncertainty.stationary, local_location_setup_store.uncertainty.precision,
					local_location_setup_store.uncertainty.update_time);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic user property server app callback***/
static uint8_t user_property_value[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
static rtk_bt_mesh_generic_property_server_para_t user_para_store = {1, 3, 2};
static uint8_t num_ids_store = 3;
static uint16_t ids_store[3] = {0x3, 0x1, 0x2};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_user_property_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PROPERTY_SET: {
		uint16_t property_id = LE_TO_U16((uint8_t *)param);
		uint16_t value_len = LE_TO_U16((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 4;
		BT_LOGA("[APP] Generic user property server model receive: property id %d\r\n", property_id);
		BT_DUMPA("value: ", pvalue, value_len);
		BT_AT_PRINT("+BLEMESHGP:gup,%d,%d", BT_AT_MESH_ROLE_SERVER, property_id);
		BT_AT_DUMP("", pvalue, value_len);
		break;
	}
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PARAMETER_GET: {
		rtk_bt_mesh_generic_property_server_para_get_t *user_para_get = (rtk_bt_mesh_generic_property_server_para_get_t *)param;
		BT_LOGA("[APP] Generic user property server model get parameter, property id = %d \r\n", user_para_get->property_id);
		memcpy(user_para_get->pvalue, &user_para_store, sizeof(rtk_bt_mesh_generic_property_server_para_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_value_get_t *user_val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)param;
		memcpy(user_val_get->pproperty_value, user_property_value, user_val_get->value_len);
		break;
	}
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_NUM_IDS_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_get_t *num_ids_get = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)param;
		// memcpy(user_val_get->pproperty_value, user_property_value, user_val_get->value_len);
		num_ids_get->pvalue->pdb = 1;
		num_ids_get->pvalue->num_ids = num_ids_store;
		break;
	}
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_IDS_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_value_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)param;
		memcpy(ids_value_get->property_ids, ids_store, 2 * ids_value_get->num_ids);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic admin property server app callback***/
static uint8_t admin_property_value[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
static rtk_bt_mesh_generic_property_server_para_t admin_para_store = {1, 3, 2};
static uint8_t admin_num_ids_store = 3;
static uint16_t admin_ids_stores[3] = {0x3, 0x1, 0x2};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_admin_property_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PROPERTY_SET: {
		uint16_t property_id = LE_TO_U16((uint8_t *)param);
		uint8_t property_access = *((uint8_t *)param + 2);
		uint16_t value_len = LE_TO_U16((uint8_t *)param + 3);
		uint8_t *pvalue = (uint8_t *)param + 5;
		BT_LOGA("[APP] Generic admin property server model receive: property id %d, property access %d\r\n", property_id, property_access);
		BT_DUMPA("value: ", pvalue, value_len);
		BT_AT_PRINT("+BLEMESHGP:gap,%d,%d,%d", BT_AT_MESH_ROLE_SERVER, property_id, property_access);
		BT_AT_DUMP("", pvalue, value_len);
		break;
	}
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PARAMETER_GET: {
		rtk_bt_mesh_generic_property_server_para_get_t *admin_para_get = (rtk_bt_mesh_generic_property_server_para_get_t *)param;
		BT_LOGA("[APP] Generic admin property server model get parameter, property id = %d \r\n", admin_para_get->property_id);
		memcpy(admin_para_get->pvalue, &admin_para_store, sizeof(rtk_bt_mesh_generic_property_server_para_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_value_get_t *admin_val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)param;
		memcpy(admin_val_get->pproperty_value, admin_property_value, admin_val_get->value_len);
		break;
	}
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_NUM_IDS_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_get_t *num_ids_get = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)param;
		// memcpy(user_val_get->pproperty_value, user_property_value, user_val_get->value_len);
		num_ids_get->pvalue->pdb = 1;
		num_ids_get->pvalue->num_ids = admin_num_ids_store;
		break;
	}
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_IDS_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_value_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)param;
		memcpy(ids_value_get->property_ids, admin_ids_stores, 2 * ids_value_get->num_ids);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic manufacturer property server app callback***/
static rtk_bt_mesh_generic_manu_property_server_set_t manu_property_set_store = {0};
static uint8_t manu_property_value[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
static rtk_bt_mesh_generic_property_server_para_t manu_para_store = {1, 3, 2};
static uint8_t manu_num_ids_store = 3;
static uint16_t manu_ids_stores[3] = {0x3, 0x1, 0x2};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_manu_property_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PROPERTY_SET: {
		rtk_bt_mesh_generic_manu_property_server_set_t *manu_property_set = (rtk_bt_mesh_generic_manu_property_server_set_t *)param;
		memcpy(&manu_property_set_store, manu_property_set, sizeof(rtk_bt_mesh_generic_manu_property_server_set_t));
		BT_LOGA("[APP] Generic manufacturer server model receive: property id %d, property_access %d\r\n",
				manu_property_set_store.property_id,
				manu_property_set_store.property_access);
		BT_AT_PRINT("+BLEMESHGP:gmp,%d,%d,%d\r\n",
					BT_AT_MESH_ROLE_SERVER, manu_property_set_store.property_id, manu_property_set_store.property_access);
		break;
	}
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PARAMETER_GET: {
		rtk_bt_mesh_generic_property_server_para_get_t *manu_para_get = (rtk_bt_mesh_generic_property_server_para_get_t *)param;
		BT_LOGA("[APP] Generic manufacturer property server model get parameter, property id = %d \r\n", manu_para_get->property_id);
		memcpy(manu_para_get->pvalue, &manu_para_store, sizeof(rtk_bt_mesh_generic_property_server_para_t));
		break;
	}
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_value_get_t *manu_val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)param;
		memcpy(manu_val_get->pproperty_value, manu_property_value, manu_val_get->value_len);
		break;
	}
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_NUM_IDS_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_get_t *num_ids_get = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)param;
		num_ids_get->pvalue->pdb = 1;
		num_ids_get->pvalue->num_ids = manu_num_ids_store;
		break;
	}
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_IDS_VALUE_GET: {
		rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_value_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)param;
		memcpy(ids_value_get->property_ids, manu_ids_stores, 2 * ids_value_get->num_ids);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in generic client property server app callback***/
static uint8_t client_num_ids = 3;
static uint16_t client_ids_store[3] = {0x3, 0x1, 0x2};
static rtk_bt_evt_cb_ret_t ble_mesh_generic_client_property_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_PROPERTY_GET: {
		rtk_bt_mesh_generic_client_property_server_direct_get_t *client_property_set = (rtk_bt_mesh_generic_client_property_server_direct_get_t *)param;
		memcpy(client_property_set->value, &client_num_ids, 1);
		memcpy(client_property_set->value + 1, client_ids_store, client_num_ids * 2);
		break;
	}

	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in sensor server app callback***/
static uint8_t sensor_raw_data[] = {0x04, 0x00, 0x01, 0x02, 0x03};
static uint16_t sensor_raw_length = 5; //should equal sensor raw data len in sensor db, default is 5
static uint16_t column_length = 5;
static uint16_t series_length = 5;
static bool need_divisior = 1;
static uint16_t des_num_store = 2;
static rtk_bt_mesh_sensor_descriptor_t descriptors_store[] = {
	{1, 2, 1, 1, 1, 2},
	{2, 1, 2, 2, 1, 3}
};
static rtk_bt_evt_cb_ret_t ble_mesh_sensor_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_GET: {
		rtk_bt_mesh_sensor_server_get_t *sensor_get = (rtk_bt_mesh_sensor_server_get_t *)param;
		if (sensor_raw_length > SENSOR_GET_DATA_MAX_LEN) {
			sensor_raw_length = SENSOR_GET_DATA_MAX_LEN;
		}
		memcpy(sensor_get->raw_data, &sensor_raw_length, 2);
		memcpy((uint8_t *)sensor_get->raw_data + 2, sensor_raw_data, sensor_raw_length);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_COLUMN_GET: {
		rtk_bt_mesh_sensor_server_direct_get_column_t *column_get = (rtk_bt_mesh_sensor_server_direct_get_column_t *)param;
		BT_LOGA("[APP] Sensor server receive: property id:%d, raw value x len:%d, raw value x: \r\n",
				column_get->property_id, column_get->raw_value_x_len);
		mesh_data_uart_dump(column_get->raw_value_x, column_get->raw_value_x_len);
		BT_AT_PRINT("+BLEMESHSENSOR:scog,%d,%d,%d,",
					BT_AT_MESH_ROLE_SERVER, column_get->property_id, column_get->raw_value_x_len);
		BT_AT_DUMP_HEXN(column_get->raw_value_x, column_get->raw_value_x_len);
		if (column_length > SENSOR_GET_COLUMN_MAX_LEN) {
			column_length = SENSOR_GET_COLUMN_MAX_LEN;
		}
		memcpy(column_get->value, &column_length, 2);
		memcpy((uint8_t *)column_get->value + 2, sensor_raw_data, column_length);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_SERIES_GET: {
		rtk_bt_mesh_sensor_server_direct_get_series_t *series_get = (rtk_bt_mesh_sensor_server_direct_get_series_t *)param;
		BT_LOGA("[APP] Sensor server receive: property id:%d, raw value x len:%d, raw value x1: \r\n",
				series_get->property_id, series_get->raw_value_x_len);
		BT_AT_PRINT("+BLEMESHSENSOR:sseg,%d,%d,%d,",
					BT_AT_MESH_ROLE_SERVER, series_get->property_id, series_get->raw_value_x_len);
		if (series_get->raw_value_x_len == 0) {
			BT_LOGA("Sensor client need get all series information \r\n");
		}

		mesh_data_uart_dump(series_get->raw_value_x1, series_get->raw_value_x_len);
		BT_AT_DUMP_HEX(series_get->raw_value_x1, series_get->raw_value_x_len);
		BT_LOGA("raw value x2:\r\n");
		BT_AT_PRINT(",");
		mesh_data_uart_dump(series_get->raw_value_x2, series_get->raw_value_x_len);
		BT_AT_DUMP_HEXN(series_get->raw_value_x2, series_get->raw_value_x_len);
		if (series_length > SENSOR_GET_SERIES_MAX_LEN) {
			series_length = SENSOR_GET_SERIES_MAX_LEN;
		}
		memcpy(series_get->value, &series_length, 2);
		memcpy((uint8_t *)series_get->value + 2, sensor_raw_data, series_length);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_COMPARE_CADENCE: {
		rtk_bt_mesh_sensor_server_compare_cadence_t *compare_get = (rtk_bt_mesh_sensor_server_compare_cadence_t *)param;
		BT_LOGA("[APP] Sensor server receive: property id:%d \r\n", compare_get->property_id);
		*(compare_get->need_fast_divisor) = need_divisior;
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_NUM_GET: {
		rtk_bt_mesh_sensor_server_get_descriptor_num_t *des_num_get = (rtk_bt_mesh_sensor_server_get_descriptor_num_t *)param;
		BT_LOGA("[APP] Sensor server receive: property id:%d \r\n", des_num_get->property_id);
		*(des_num_get->descriptor_num) = des_num_store;
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_DESCRIPTOR_GET: {
		rtk_bt_mesh_sensor_server_get_descriptor_t *des_get = (rtk_bt_mesh_sensor_server_get_descriptor_t *)param;
		BT_LOGA("[APP] Sensor server receive: property id:%d \r\n", des_get->property_id);
		memcpy(des_get->descriptor, descriptors_store, sizeof(rtk_bt_mesh_sensor_descriptor_t) * des_get->descriptor_num);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SENSOR_SERVER_MODEL

#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
/***BT and BLE Mesh related api shall not be called in sensor setup server app callback***/
static uint8_t app_setting_store[SENSOR_SETTING_DATA_MAX_LEN + 2] = {0};
static uint8_t delta_down[SENSOR_CADENCE_DATA_MAX_LEN] = {0};
static uint8_t delta_up[SENSOR_CADENCE_DATA_MAX_LEN] = {0};
static uint8_t cadence_low[SENSOR_CADENCE_DATA_MAX_LEN] = {0};
static uint8_t cadence_high[SENSOR_CADENCE_DATA_MAX_LEN] = {0};
static uint8_t app_store_raw_value_len = 0;
static uint8_t divisionr_trigger = 0;
static uint16_t app_store_cadence_len = 0;
static uint8_t app_store_status_min_interval = 0;
static uint16_t app_settings_store[] = {2, 1, 2};
static rtk_bt_evt_cb_ret_t ble_mesh_sensor_setup_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_SET: {
		uint8_t *pdata = (uint8_t *)param;
		uint16_t property_id = LE_TO_U16(pdata);
		BT_LOGA("[APP] Sensor setup server receive: set property id %d \r\n", property_id);
		pdata += 2;
		uint8_t raw_value_len = *pdata;
		app_store_raw_value_len = raw_value_len;
		uint8_t check_raw_len = raw_value_len;
		if (app_store_raw_value_len > SENSOR_CADENCE_DATA_MAX_LEN) {
			check_raw_len = SENSOR_CADENCE_DATA_MAX_LEN;
		}
		pdata++;
		divisionr_trigger = *pdata;
		uint8_t fast_cadence_period_divisor = (*(pdata)) & 0x7F;
		uint8_t status_trigger_type = ((*(pdata)) & 0x80) >> 7;
		BT_LOGA("fast_cadence_period_divisor: %d, status_trigger_type :%d \r\n",
				fast_cadence_period_divisor, status_trigger_type);
		BT_AT_PRINT("+BLEMESHSENSOR:sc,%d,%d,%d,%d,",
					BT_AT_MESH_ROLE_SERVER, property_id, fast_cadence_period_divisor, status_trigger_type);
		pdata++;
		uint8_t trigger_len;
		if (status_trigger_type == 0) {
			trigger_len = raw_value_len;
		} else {
			trigger_len = 2;
		}
		uint8_t check_trigger_len = trigger_len;
		if (trigger_len > SENSOR_CADENCE_DATA_MAX_LEN) {
			check_trigger_len = SENSOR_CADENCE_DATA_MAX_LEN;
		}
		if (status_trigger_type) {
			BT_LOGA("status_trigger_delta_down: %d\r\n", LE_TO_U16(pdata));
			BT_AT_PRINT("%d", LE_TO_U16(pdata));
			memcpy(delta_down, pdata, check_trigger_len);
			pdata += trigger_len;
			BT_LOGA("status_trigger_delta_up: %d\r\n", LE_TO_U16(pdata));
			BT_AT_PRINT("%d", LE_TO_U16(pdata));
			memcpy(delta_up, pdata, check_trigger_len);
			pdata += trigger_len;
		} else {
			BT_LOGA("status_trigger_delta_down: \r\n");
			mesh_data_uart_dump(pdata, trigger_len);
			BT_AT_DUMP_HEX(pdata, trigger_len);
			memcpy(delta_down, pdata, check_trigger_len);
			pdata += trigger_len;
			BT_LOGA("status_trigger_delta_up: \r\n");
			BT_AT_PRINT(",");
			mesh_data_uart_dump(pdata, trigger_len);
			BT_AT_DUMP_HEX(pdata, trigger_len);
			memcpy(delta_up, pdata, check_trigger_len);
			pdata += trigger_len;
		}
		uint8_t status_min_interval = *pdata;
		app_store_status_min_interval = status_min_interval;
		BT_LOGA("status_min_interval: %d \r\n", status_min_interval);
		BT_AT_PRINT(",%d,", status_min_interval);
		pdata++;
		BT_LOGA("fast_cadence_low: \r\n");
		mesh_data_uart_dump(pdata, raw_value_len);
		BT_AT_DUMP_HEX(pdata, raw_value_len);
		memcpy(cadence_low, pdata, check_raw_len);
		pdata += raw_value_len;
		BT_LOGA("fast_cadence_high: \r\n");
		BT_AT_PRINT(",");
		mesh_data_uart_dump(pdata, raw_value_len);
		BT_AT_DUMP_HEXN(pdata, raw_value_len);
		memcpy(cadence_high, pdata, check_raw_len);
		app_store_cadence_len = 2 * check_trigger_len + 2 * check_raw_len + 3;
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_SET: {
		uint8_t *pdata = (uint8_t *)param;
		uint16_t property_id = LE_TO_U16(pdata);
		pdata += 2;
		uint16_t setting_property_id = LE_TO_U16(pdata);
		pdata += 2;
		rtk_bt_mesh_sensor_setting_access_t access = (rtk_bt_mesh_sensor_setting_access_t)(*pdata);
		pdata++;
		BT_LOGA("[APP] Sensor setup server receive: set property id %d, setting property id %d, access %d \r\n",
				property_id, setting_property_id, access);
		uint8_t raw_len = *pdata;
		pdata++;
		BT_LOGA("setting value: ");
		mesh_data_uart_dump(pdata, raw_len);
		BT_AT_PRINT("+BLEMESHSENSOR:ss,%d,%d,%d,%d,",
					BT_AT_MESH_ROLE_SERVER, property_id, setting_property_id, access);
		BT_AT_DUMP_HEXN(pdata, raw_len);
		memcpy(app_setting_store, &access, 1);
		uint8_t store_raw_len = raw_len;
		if (raw_len > SENSOR_SETTING_DATA_MAX_LEN) {
			store_raw_len = SENSOR_SETTING_DATA_MAX_LEN;
		}
		memcpy(&app_setting_store[1], &store_raw_len, 1);
		memcpy(&app_setting_store[2], pdata, store_raw_len);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_CADENCE_GET: {
		rtk_bt_mesh_sensor_server_get_cadence_t *pdata = (rtk_bt_mesh_sensor_server_get_cadence_t *)param;
		uint16_t property_id = pdata->property_id;
		BT_LOGA("[APP] Sensor setup server receive: property id %d \r\n", property_id);
		uint8_t *cadence_data = pdata->cadence;
		uint16_t data_len = app_store_cadence_len;
		if (app_store_cadence_len > 4 * SENSOR_CADENCE_DATA_MAX_LEN) {
			data_len = 0;
		}
		memcpy(cadence_data, &data_len, 2);
		BT_LOGA("app_store_cadence_len %d \r\n", app_store_cadence_len);
		if (data_len != 0) {
			uint8_t check_raw_len = app_store_raw_value_len;
			if (app_store_raw_value_len > SENSOR_CADENCE_DATA_MAX_LEN) {
				check_raw_len = SENSOR_CADENCE_DATA_MAX_LEN;
			}
			memcpy(cadence_data + 2, &check_raw_len, 1);
			memcpy(cadence_data + 3, &divisionr_trigger, 1);// fast_cadence_period_divisor + status_trigger_type
			uint8_t trigger_len;
			if (((divisionr_trigger & 0x80) >> 7) == 0) {
				trigger_len = app_store_raw_value_len;
			} else {
				trigger_len = 2;
			}
			uint8_t check_trigger_len = trigger_len;
			if (trigger_len > SENSOR_CADENCE_DATA_MAX_LEN) {
				check_trigger_len = SENSOR_CADENCE_DATA_MAX_LEN;
			}
			memcpy(cadence_data + 4, delta_down, check_trigger_len);
			memcpy(cadence_data + 4 + check_trigger_len, delta_up, check_trigger_len);
			memcpy(cadence_data + 4 + 2 * check_trigger_len, &app_store_status_min_interval, 1);
			memcpy(cadence_data + 5 + 2 * check_trigger_len, cadence_low, check_raw_len);
			memcpy(cadence_data + 5 + 2 * check_trigger_len + check_raw_len, cadence_high, check_raw_len);
		}

		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTINGS_GET: {
		rtk_bt_mesh_sensor_server_get_settings_t *pdata = (rtk_bt_mesh_sensor_server_get_settings_t *)param;
		uint16_t property_id = pdata->property_id;
		BT_LOGA("[APP] Sensor setup server receive: property id %d \r\n", property_id);
		uint16_t num = app_settings_store[0];
		if (num > SENSOR_SETTINGS_DATA_MAX_LEN) {
			num = SENSOR_SETTINGS_DATA_MAX_LEN;
		}
		memcpy(pdata->settings_data, &num, 2);
		memcpy(pdata->settings_data + 1, &app_settings_store[1], num * 2);
		break;
	}
	case RTK_BT_MESH_SENSOR_SERVER_MODEL_SETTING_GET: {
		rtk_bt_mesh_sensor_server_get_setting_t *pdata = (rtk_bt_mesh_sensor_server_get_setting_t *)param;
		uint16_t property_id = pdata->property_id;
		uint16_t setting_id = pdata->setting_property_id;
		BT_LOGA("[APP] Sensor setup server receive: property id %d, setting property id %d \r\n", property_id, setting_id);
		uint8_t setting_len = app_setting_store[1];
		if (setting_len > SENSOR_SETTING_DATA_MAX_LEN) {
			setting_len = SENSOR_SETTING_DATA_MAX_LEN;
		}
		memcpy(pdata->setting_data, app_setting_store, setting_len + 2);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif // end of BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL

static uint8_t app_health_period = 0;
static uint8_t app_fault_array[] = {0x3, 0x1, 0x1, 0x2}; //1 byte data_length(test id + faults length) + 1 byte test id + n bytes faults
static uint8_t app_clear_array[] = {1, 0x1};
static uint8_t app_attn = 0;
static uint8_t app_current_array[] = {0x5, 0x1, 0x5d, 0x00, 0x3, 0x4};

/***BT and BLE Mesh related api shall not be called in health server app callback***/
static rtk_bt_evt_cb_ret_t ble_mesh_health_server_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_GET: {
		rtk_bt_mesh_health_server_fault_get_t *fault_get = (rtk_bt_mesh_health_server_fault_get_t *)param;
		BT_LOGA("[APP] Health server receive: company id %04x \r\n", fault_get->company_id);
		BT_AT_PRINT("+BLEMESHHEALTH:hfg,%d,%04x\r\n", BT_AT_MESH_ROLE_SERVER, fault_get->company_id);
		uint8_t array_len = app_fault_array[0];
		if (array_len - 1 > HEALTH_FAULT_ARRAY_LEN) {
			array_len = HEALTH_FAULT_ARRAY_LEN + 1;
		}
		memcpy(fault_get->fault_array, &array_len, 1);
		memcpy(fault_get->fault_array + 1, &app_fault_array[1], array_len);
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_CLEAR: {
		rtk_bt_mesh_health_server_fault_clear_t *fault_clear = (rtk_bt_mesh_health_server_fault_clear_t *)param;
		BT_LOGA("[APP] Health server receive: company id %04x \r\n", fault_clear->company_id);
		BT_AT_PRINT("+BLEMESHHEALTH:hfc,%d,%04x\r\n", BT_AT_MESH_ROLE_SERVER, fault_clear->company_id);
		uint8_t array_len = app_clear_array[0];
		if (array_len - 1 > HEALTH_FAULT_ARRAY_LEN) {
			array_len = HEALTH_FAULT_ARRAY_LEN + 1;
		}
		memcpy(fault_clear->fault_array, &array_len, 1);
		memcpy(fault_clear->fault_array + 1, &app_clear_array[1], array_len);
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_TEST: {
		rtk_bt_mesh_health_server_fault_test_t *fault_test = (rtk_bt_mesh_health_server_fault_test_t *)param;
		BT_LOGA("[APP] Health server receive: test id %d company id %04x \r\n", fault_test->test_id, fault_test->company_id);
		BT_AT_PRINT("+BLEMESHHEALTH:hft,%d,%d,%04x\r\n",
					BT_AT_MESH_ROLE_SERVER, fault_test->test_id, fault_test->company_id);
		/*call related customer test cb*/
		uint8_t array_len = app_fault_array[0];
		if (array_len - 1 > HEALTH_FAULT_ARRAY_LEN) {
			array_len = HEALTH_FAULT_ARRAY_LEN + 1;
		}
		memcpy(fault_test->fault_array, &array_len, 1);
		memcpy(fault_test->fault_array + 1, &app_fault_array[1], array_len);
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_GET: {
		rtk_bt_mesh_health_server_period_get_t *period = (rtk_bt_mesh_health_server_period_get_t *)param;
		*(period->fast_period_divisor) = app_health_period;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_SET: {
		rtk_bt_mesh_health_server_period_set_t *period = (rtk_bt_mesh_health_server_period_set_t *)param;
		app_health_period = (period->fast_period_divisor);
		BT_LOGA("[APP] Health server receive: set fast period divisor %d \r\n", app_health_period);
		BT_AT_PRINT("+BLEMESHHEALTH:hp,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, app_health_period);
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_GET: {
		rtk_bt_mesh_health_server_attn_get_t *attn = (rtk_bt_mesh_health_server_attn_get_t *)param;
		*(attn->attn) = app_attn;
		/****Get from attention timer created by customer****/
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_SET: {
		rtk_bt_mesh_health_server_attn_set_t *attn = (rtk_bt_mesh_health_server_attn_set_t *)param;
		app_attn = attn->attn;
		BT_LOGA("[APP] Health server receive: set attention timer %d \r\n", app_attn);
		BT_AT_PRINT("+BLEMESHHEALTH:ha,%d,%d\r\n", BT_AT_MESH_ROLE_SERVER, app_attn);
		/*****If needed, customer create the attention timer here******/
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_CURRENT_GET: {
		//data style: 1 byte(total length) + 1 byte(test id) + 2 byte(company id) + n bytes(faults)
		uint8_t array_len = app_clear_array[0];
		if (array_len + 1 > 4 + HEALTH_FAULT_ARRAY_LEN) {
			array_len = HEALTH_FAULT_ARRAY_LEN + 3;
		}
		memcpy(param, &array_len, 1);
		memcpy((uint8_t *)param + 1, &app_current_array[1], array_len);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

int ble_mesh_device_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_MESH | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
		bt_app_conf.bt_mesh_app_conf.bt_mesh_role = RTK_BT_MESH_ROLE_DEVICE;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		// Mesh stack will report a event when stack is ready, so should regist callback before bt enable
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_STACK, ble_mesh_stack_app_callback));
		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		BT_LOGA("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_mesh_gap_app_callback));

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_SERVER_MODEL, ble_mesh_generic_onoff_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, ble_mesh_datatrans_model_app_callback));
#endif // end of BT_MESH_ENABLE_DATATRANS_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, ble_mesh_light_lightness_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL, ble_mesh_light_lightness_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, ble_mesh_light_ctl_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_CTL_SETUP_SERVER_MODEL, ble_mesh_light_ctl_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, ble_mesh_light_ctl_temperature_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, ble_mesh_light_hsl_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, ble_mesh_light_hsl_hue_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, ble_mesh_light_hsl_saturation_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_SETUP_SERVER_MODEL, ble_mesh_light_hsl_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, ble_mesh_light_xyl_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_XYL_SETUP_SERVER_MODEL, ble_mesh_light_xyl_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, ble_mesh_light_lc_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
#if defined(BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LC_SETUP_SERVER_MODEL, ble_mesh_light_lc_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, ble_mesh_time_server_app_callback));
#endif // end of BT_MESH_ENABLE_TIME_SERVER_MODEL
#if defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, ble_mesh_time_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCHEDULER_SERVER_MODEL, ble_mesh_scheduler_server_app_callback));
#endif // end of BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCHEDULER_SETUP_SERVER_MODEL, ble_mesh_scheduler_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, ble_mesh_scene_server_app_callback));
#endif // end of BT_MESH_ENABLE_SCENE_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, ble_mesh_scene_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL,
													ble_mesh_generic_default_transition_time_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, ble_mesh_generic_level_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL, ble_mesh_generic_power_on_off_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL,
													ble_mesh_generic_power_on_off_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, ble_mesh_generic_power_level_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL, ble_mesh_generic_power_level_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_SERVER_MODEL, ble_mesh_generic_battery_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SERVER_MODEL, ble_mesh_generic_location_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL, ble_mesh_generic_location_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, ble_mesh_generic_user_property_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, ble_mesh_generic_admin_property_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, ble_mesh_generic_manu_property_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
#if defined(BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL, ble_mesh_generic_client_property_server_app_callback));
#endif // end of BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SENSOR_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SENSOR_SERVER_MODEL, ble_mesh_sensor_server_app_callback));
#endif // end of BT_MESH_ENABLE_SENSOR_SERVER_MODEL
#if defined(BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SENSOR_SETUP_SERVER_MODEL, ble_mesh_sensor_setup_server_app_callback));
#endif // end of BT_MESH_ENABLE_SENSOR_SETUP_SERVER_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, ble_mesh_health_server_app_callback));
	} else if (0 == enable) {

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}
