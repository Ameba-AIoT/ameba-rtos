
#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_le_gap.h>

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
#include <rtk_bt_mesh_sensor_model.h>
#include <rtk_bt_mesh_health_model.h>
#include <bt_utils.h>

#define BT_APP_PROCESS(func)                                \
    do {                                                    \
        uint16_t __func_ret = func;                         \
        if (RTK_BT_OK != __func_ret) {                      \
            printf("[APP] %s failed!!! line: %d, err: 0x%x\r\n", __func__, __LINE__, __func_ret);   \
            return -1;                                      \
        }                                                   \
    } while (0)

static void mesh_data_uart_dump(uint8_t *pbuffer, uint32_t len)
{
	printf("0x");
	for (uint32_t i = 0; i < len; i++) {
		printf("%02X", pbuffer[i]);
	}
	printf("\r\n");
}

static rtk_bt_evt_cb_ret_t ble_mesh_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	char le_addr[30] = {0};
	char *role;
	switch (evt_code) {
	case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
		rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
		if (!scan_start_ind->err) {
			printf("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
		} else {
			printf("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,start,%d,%d\r\n", (scan_start_ind->err == 0) ? 0 : -1, scan_start_ind->scan_type);
		break;
	}
	case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
		rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
		if (!scan_stop_ind->err) {
			printf("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
		} else {
			printf("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
		}
		BT_AT_PRINT("+BLEGAP:scan,stop,%d,0x%x\r\n", (scan_stop_ind->err == 0) ? 0 : -1, scan_stop_ind->stop_reason);
		break;
	}
	case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
		rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
		rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
		printf("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %i, len: %d \r\n",
			   le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
			   scan_res_ind->adv_report.len);
		BT_AT_PRINT("+BLEGAP:scan,info,%s,%d,%i,%d\r\n",
					le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi,
					scan_res_ind->adv_report.len);
		break;
	}
	case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
		rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
		if (!conn_ind->err) {
			role = conn_ind->role ? "slave" : "master";
			printf("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
				   conn_ind->conn_handle, role, le_addr);
		} else {
			printf("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
				   conn_ind->err, le_addr);
		}
		BT_AT_PRINT("+BLEGAP:conn,%d,%d,%s\r\n", (conn_ind->err == 0) ? 0 : -1, (int)conn_ind->conn_handle, le_addr);
		break;
	}
	case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
		rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
		rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
		role = disconn_ind->role ? "slave" : "master";
		printf("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
			   disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		BT_AT_PRINT("+BLEGAP:disconn,0x%x,%d,%s,%s\r\n",
					disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);
		break;
	}
	case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
		rtk_bt_le_phy_update_ind_t *phy_update_ind = (rtk_bt_le_phy_update_ind_t *)param;
		if (phy_update_ind->err) {
			printf("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
				   phy_update_ind->conn_handle, phy_update_ind->err);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,-1\r\n", phy_update_ind->conn_handle);
		} else {
			printf("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
				   phy_update_ind->conn_handle, phy_update_ind->tx_phy, phy_update_ind->rx_phy);
			BT_AT_PRINT("+BLEGAP:conn_phy,%d,0,%d,%d\r\n",
						phy_update_ind->conn_handle,
						phy_update_ind->tx_phy,
						phy_update_ind->rx_phy);
		}
		break;
	}
	case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
		rtk_bt_le_data_len_change_ind_t *data_len_change =
			(rtk_bt_le_data_len_change_ind_t *)param;
		printf("[APP] Data len is updated, conn_handle: %d, "       \
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
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

static unsigned int get_data_from_memory_byte_by_byte(uint8_t *p, uint8_t data_len)
{
	// Get the data on memory using little endian
	if (!p || data_len > 4) {
		printf("[%s] copy data fail\r\n", __func__);
		return 0;
	}
	uint32_t i, val = 0;
	for (i = 0; i < data_len; i++) {
		val += *p++ << (i * 8);
	}
	return val;
}

static rtk_bt_evt_cb_ret_t ble_mesh_stack_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	rtk_bt_evt_cb_ret_t ret = RTK_BT_EVT_CB_OK;
	switch (evt_code) {
	case RTK_BT_MESH_STACK_EVT_SCAN_SWITCH: {
		rtk_bt_mesh_stack_evt_scan_switch_t *scan_switch;
		scan_switch = (rtk_bt_mesh_stack_evt_scan_switch_t *)param;
		if (scan_switch->scan_enable) {
			printf("[APP] Ble mesh scan enable SUCCESS\r\n\r\n");
		} else {
			printf("[APP] Ble mesh scan disable SUCCESS\r\n\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_UDB_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_udb_t *udb_info;
		udb_info = (rtk_bt_mesh_stack_evt_dev_info_udb_t *)param;
		printf("[APP] ");
		if (0xff != udb_info->dev_info.bt_addr_type) {
			// Only for RTK mesh stack, report bt address and type information(zephyr mesh stack can not report)
			printf("bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", udb_info->dev_info.bt_addr[5], udb_info->dev_info.bt_addr[4],
				   udb_info->dev_info.bt_addr[3], udb_info->dev_info.bt_addr[2], udb_info->dev_info.bt_addr[1], udb_info->dev_info.bt_addr[0], udb_info->dev_info.bt_addr_type,
				   udb_info->dev_info.rssi);
		}
		printf("udb=");
		mesh_data_uart_dump(udb_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROV_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *prov_info;
		prov_info = (rtk_bt_mesh_stack_evt_dev_info_provision_adv_t *)param;
		printf("[APP] ");
		if (0xff != prov_info->dev_info.bt_addr_type) {
			// Only for RTK mesh stack, report bt address and type information(zephyr mesh stack can not report)
			printf("bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", prov_info->dev_info.bt_addr[5], prov_info->dev_info.bt_addr[4],
				   prov_info->dev_info.bt_addr[3], prov_info->dev_info.bt_addr[2], prov_info->dev_info.bt_addr[1], prov_info->dev_info.bt_addr[0],
				   prov_info->dev_info.bt_addr_type, prov_info->dev_info.rssi);
		}
		printf("prov=");
		mesh_data_uart_dump(prov_info->dev_uuid, 16);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_DEVICE_INFO_PROXY_DISPLAY: {
		rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *proxy_info;
		proxy_info = (rtk_bt_mesh_stack_evt_dev_info_proxy_adv_t *)param;
		printf("[APP] bt addr=0x%02x%02x%02x%02x%02x%02x type=%d rssi=%d ", proxy_info->dev_info.bt_addr[5], proxy_info->dev_info.bt_addr[4],
			   proxy_info->dev_info.bt_addr[3], proxy_info->dev_info.bt_addr[2], proxy_info->dev_info.bt_addr[1], proxy_info->dev_info.bt_addr[0],
			   proxy_info->dev_info.bt_addr_type, proxy_info->dev_info.rssi);
		printf("proxy=");
		mesh_data_uart_dump((uint8_t *)&proxy_info->proxy, proxy_info->len);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PB_ADV_LINK_STATE: {
		rtk_bt_mesh_prov_generic_cb_type_t *pb_adv_link_state;
		pb_adv_link_state = (rtk_bt_mesh_prov_generic_cb_type_t *)param;
		switch (*pb_adv_link_state) {
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPENED:
			printf("[APP] PB-ADV Link Opened!\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_OPEN_FAILED:
			printf("[APP] PB-ADV Link Open Failed!\r\n");
			break;
		case RTK_BT_MESH_PB_GENERIC_CB_LINK_CLOSED:
			printf("[APP] PB-ADV Link Closed!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_COMPLETE: {
		rtk_bt_mesh_stack_evt_prov_complete_t *prov_complete;
		prov_complete = (rtk_bt_mesh_stack_evt_prov_complete_t *)param;
		printf("[APP] Provisioning complete,unicast address:0x%x\r\n", prov_complete->unicast_addr);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_FAIL: {
		rtk_bt_mesh_stack_evt_prov_fail_t *prov_fail;
		prov_fail = (rtk_bt_mesh_stack_evt_prov_fail_t *)param;
		printf("[APP] Provisioning fail,reason:%d\r\n", prov_fail->fail_reason);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROV_SERVICE_DIS_RESULT: {
		rtk_bt_mesh_stack_evt_prov_dis_t *dis_res;
		dis_res = (rtk_bt_mesh_stack_evt_prov_dis_t *)param;
		switch (dis_res->dis_result) {
		case RTK_BT_MESH_PROV_DISC_DONE:
			printf("[APP] Prov service discovery done!\r\n");
			break;
		case RTK_BT_MESH_PROV_DISC_FAIL:
			printf("[APP] Prov service discovery fail!\r\n");
			break;
		case RTK_BT_MESH_PROV_DISC_NOT_FOUND:
			printf("[APP] Prov service not found!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_PROXY_SERVICE_DIS_RESULT: {
		rtk_bt_mesh_stack_evt_proxy_dis_t *dis_res;
		dis_res = (rtk_bt_mesh_stack_evt_proxy_dis_t *)param;
		switch (dis_res->dis_result) {
		case RTK_BT_MESH_PROXY_DISC_DONE:
			printf("[APP] Proxy service discovery done!\r\n");
			break;
		case RTK_BT_MESH_PROXY_DISC_FAIL:
			printf("[APP] Proxy service discovery fail!\r\n");
			break;
		case RTK_BT_MESH_PROXY_DISC_NOT_FOUND:
			printf("[APP] Proxy service not found!\r\n");
			break;
		default:
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_FN_CB: {
		rtk_bt_mesh_stack_evt_fn_t *fn_cb = (rtk_bt_mesh_stack_evt_fn_t *)param;
		switch (fn_cb->cb_type) {
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISHING:
			printf("[APP] Establishing friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_FAIL_NO_POLL:
			printf("[APP] Not receive poll LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_ESTABLISH_SUCCESS:
			printf("[APP] Established friendship with LPN: 0x%04x success\r\n", fn_cb->lpn_addr);
			break;
		case RTK_BT_MESH_FN_CB_TYPE_FRND_LOST:
			printf("[APP] Lost friendship with LPN: 0x%04x\r\n", fn_cb->lpn_addr);
			break;
		default:
			printf("[%s] Unknown cb type:%d\r\n", __func__, fn_cb->cb_type);
			break;
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_CAPABILITY_NOT_SUPPORT_OF_DEVICE: {
		rtk_bt_mesh_stack_prov_capabilities_t *prov_capa = (rtk_bt_mesh_stack_prov_capabilities_t *)param;
		printf("[APP] The prov capability of remote device is not strong:");
		printf(" Element num:%d, Algorithm:0x%02x, Public Key Type:0x%02x, Static OOB Type:0x%02x, Output OOB Size:%d, Output OOB Action:0x%02x, Input OOB Size:%d, Input OOB Actions:0x%02x\r\n",
			   \
			   prov_capa->element_num, prov_capa->algorithm, prov_capa->public_key, prov_capa->static_oob, \
			   prov_capa->output_oob_size, prov_capa->output_oob_action, prov_capa->input_oob_size, prov_capa->input_oob_action);
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_STATIC_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_static_oob *static_oob;
		static_oob = (rtk_bt_mesh_stack_set_auth_value_for_static_oob *)param;
		if (static_oob->status) {
			printf("[APP] Set auth value for static oob success:\r\n");
			for (int i = 0; i < 16; i++) {
				printf("%02x ", static_oob->data[i]);
			}
			printf("\r\n");
		} else {
			printf("[APP] Set auth value for static oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_SET_AUTH_VALUE_FOR_INPUT_OOB: {
		rtk_bt_mesh_stack_set_auth_value_for_oob_data *output_oob;
		output_oob = (rtk_bt_mesh_stack_set_auth_value_for_oob_data *)param;
		if (output_oob->status) {
			printf("[APP] Set auth value for output oob success, output random value:%d\r\n", (int)output_oob->random);
		} else {
			printf("[APP] Set auth value for output oob fail\r\n");
		}
		break;
	}
	case RTK_BT_MESH_STACK_EVT_NOTIFY_FOR_OUTPUT_OOB_VALUE: {
		printf("[APP] Please input the random value display on remote unprovisioned device\r\n");
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
				printf("Mesh State:\t%d\r\n", *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_UUID:
				printf("Device UUID:\t");
				mesh_data_uart_dump(p_data + offset, data_len);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR:
				printf("BTAddr:\t\t0x%02x%02x%02x%02x%02x%02x\r\n",
					   *(p_data + offset + 5), *(p_data + offset + 4), *(p_data + offset + 3), *(p_data + offset + 2), *(p_data + offset + 1), *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_DEV_KEY:
				printf("DevKey:\t\t%d-0x%04x-%d-", *(p_data + offset), get_data_from_memory_byte_by_byte(p_data + offset + 1, 2), *(p_data + offset + 3));
				mesh_data_uart_dump(p_data + offset + 4, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NET_KEY:
				printf("NetKey:\t\t%d-0x%04x-%d-%d-%d\r\n\t\t", *(p_data + offset), get_data_from_memory_byte_by_byte(p_data + offset + 1, 2), \
					   * (p_data + offset + 3), *(p_data + offset + 4), *(p_data + offset + 5));
				mesh_data_uart_dump(p_data + offset + 6, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_APP_KEY:
				printf("AppKey:\t\t%d-0x%04x-%d-%d-%d\r\n", *(p_data + offset), get_data_from_memory_byte_by_byte(p_data + offset + 1, 2), \
					   * (p_data + offset + 3), *(p_data + offset + 4), get_data_from_memory_byte_by_byte(p_data + offset + 5, 2));
				mesh_data_uart_dump(p_data + offset + 7, 16);
				break;
			case RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS:
				printf("IVindex:\t%d-0x%x\r\n", *(p_data + offset), get_data_from_memory_byte_by_byte(p_data + offset + 1, 4));
				printf("Seq:\t\t0x%06x\r\n", get_data_from_memory_byte_by_byte(p_data + offset + 5, 4));
				printf("NodeAddr:\t0x%04x-%d-%d\r\n", get_data_from_memory_byte_by_byte(p_data + offset + 9, 2),
					   get_data_from_memory_byte_by_byte(p_data + offset + 11, 4), *(p_data + offset + 15));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO:
				printf("Element:\t%d-%d", *(p_data + offset), get_data_from_memory_byte_by_byte(p_data + offset + 1, 4));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO:
				printf("\r\nModel:\t\t%d-%d-0x%08x", *(p_data + offset), *(p_data + offset + 1), get_data_from_memory_byte_by_byte(p_data + offset + 2, 4));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY:
				printf("(app key:%d)", *(p_data + offset));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO:
				printf("(pub to:0x%04x-%d-%d)", get_data_from_memory_byte_by_byte(p_data + offset, 2), *(p_data + offset + 2),
					   get_data_from_memory_byte_by_byte(p_data + offset + 3, 2));
				break;
			case RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO: {
				uint16_t sub_addr_num = data_len / 2;
				uint16_t temp_offset = offset;
				printf("(sub to:");
				for (uint16_t i = 0; i < sub_addr_num; i++) {
					printf("0x%04x", get_data_from_memory_byte_by_byte(p_data + temp_offset, 2));
					temp_offset += 2;
					if (i == sub_addr_num - 1) {
						printf(")");
					} else {
						printf("-");
					}
				}
				break;
			}
			default:
				printf("[%s] Unknown data type %d for RTK_BT_MESH_STACK_EVT_LIST_INFO\r\n", __func__, (int)type);
				return RTK_BT_EVT_CB_OK;
				break;
			}
			offset += data_len;
		}
		printf("\r\n\r\n");
		if (!len) {
			printf("[APP] Print user cmd list fail, len==0\r\n");
		}
		break;
	}
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return ret;
}

static rtk_bt_evt_cb_ret_t ble_mesh_config_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT: {
		rtk_bt_mesh_cfg_app_key_stat_t *add_app_key_state;
		add_app_key_state = (rtk_bt_mesh_cfg_app_key_stat_t *)param;
		if (add_app_key_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Config client model receive Config AppKey Status message with status SUCCESS\r\n");
		} else {
			printf("[APP] Config client model receive Config AppKey Status message with status:0x%x\r\n", add_app_key_state->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT: {
		rtk_bt_mesh_cfg_model_app_stat_t *model_app_bind_state;
		model_app_bind_state = (rtk_bt_mesh_cfg_model_app_stat_t *)param;
		if (model_app_bind_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Config client model receive Config Model App Status message with status SUCCESS\r\n");
		} else {
			printf("[APP] Config client model receive Config Model App Status message with status :0x%x\r\n", model_app_bind_state->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT: {
		rtk_bt_mesh_cfg_model_sub_stat_t *model_sub_add_state;
		model_sub_add_state = (rtk_bt_mesh_cfg_model_sub_stat_t *)param;
		if (model_sub_add_state->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Config client model receive Config Model Subscription Status message with status SUCCESS\r\n");
		} else {
			printf("[APP] Config client model receive Config Model Subscription Status message with status:0x%x\r\n", model_sub_add_state->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT: {
		rtk_bt_mesh_cfg_default_ttl_stat_t *ttl_stat;
		ttl_stat = (rtk_bt_mesh_cfg_default_ttl_stat_t *)param;
		printf("[APP] Config client model receive message:default TTL:%d\r\n", ttl_stat->ttl);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT: {
		rtk_bt_mesh_cfg_relay_stat_t *relay_stat;
		relay_stat = (rtk_bt_mesh_cfg_relay_stat_t *)param;
		printf("[APP] Config client model receive message,Relay State: %d, Count = %d Step = %d\r\n", relay_stat->state, relay_stat->count, relay_stat->steps);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT: {
		rtk_bt_mesh_cfg_net_transmit_stat_t *net_transmit;
		net_transmit = (rtk_bt_mesh_cfg_net_transmit_stat_t *)param;
		printf("[APP] Config client model receive message,Net transmit state: Count = %d Step = %d\r\n", net_transmit->count, net_transmit->steps);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_RESET_STAT: {
		rtk_bt_mesh_config_client_model_node_reset_stat_t *node_reset_stat;
		node_reset_stat = (rtk_bt_mesh_config_client_model_node_reset_stat_t *)param;
		printf("[APP] Config client model receive message:Node 0x%x is reset SUCCESS\r\n", node_reset_stat->src);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT: {
		rtk_bt_mesh_cfg_beacon_stat_t *beacon_stat;
		beacon_stat = (rtk_bt_mesh_cfg_beacon_stat_t *)param;
		printf("[APP] beacon state: %d\r\n", beacon_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT: {
		printf("[APP] Config client model receive message:COMPO DATA STAT SUCCESS\r\n");
		uint8_t *p_data = (uint8_t *)param;
		if (p_data[5] == 1) {
			uint16_t msg_len = MESH_LE_EXTRN2WORD(p_data + 1);
			uint16_t src = MESH_LE_EXTRN2WORD(p_data + 3);
			//  uint32_t length = MESH_LE_EXTRN2DWORD(p_data + 6);// callback to app msg data length
			uint8_t element_index = 0;
			uint8_t *pdata = p_data + 10;
			uint8_t *pend = p_data + 10 + msg_len - 2;//2 is head length
			printf("cdp0 parsed: src=0x%04x cid=0x%04x pid=0x%04x vid=0x%04x rpl=%d features=0x%04x\r\n",
				   src, MESH_LE_EXTRN2WORD(pdata), MESH_LE_EXTRN2WORD(pdata + 2),
				   MESH_LE_EXTRN2WORD(pdata + 4), MESH_LE_EXTRN2WORD(pdata + 6), MESH_LE_EXTRN2WORD(pdata + 8));
			pdata += 10;
			while (pdata < pend) {
				uint8_t sig_model_num = pdata[2];
				uint8_t vendor_model_num = pdata[3];
				uint8_t model_num;
				printf("element %d: loc=%d sig=%d vendor=%d\r\n", element_index, MESH_LE_EXTRN2WORD(pdata),
					   sig_model_num, vendor_model_num);
				pdata += 4;
				if (sig_model_num) {
					printf("\ts:");
					for (model_num = 0; model_num < sig_model_num; model_num++, pdata += 2) {
						printf(" 0x%04xffff", MESH_LE_EXTRN2WORD(pdata));
					}
					printf("\r\n");
				}
				if (vendor_model_num) {
					printf("\tv:");
					for (model_num = 0; model_num < vendor_model_num; model_num++, pdata += 4) {
						printf(" 0x%08x", (unsigned int)MESH_LE_EXTRN2DWORD(pdata));
					}
					printf("\r\n");
				}
				element_index++;
			}
		} else {
			printf("cdp0 of 0x%04x invalid!\r\n", MESH_LE_EXTRN2WORD(p_data + 3));
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT: {
		rtk_bt_mesh_cfg_proxy_stat_t *proxy_stat;
		proxy_stat = (rtk_bt_mesh_cfg_proxy_stat_t *)param;
		printf("[APP] provisoner receive proxy state: %d\r\n", proxy_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT: {
		rtk_bt_mesh_cfg_model_pub_stat_t *model_pub_stat;
		model_pub_stat = (rtk_bt_mesh_cfg_model_pub_stat_t *)param;
		if (model_pub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Pub stat: pub addr = 0x%04x, ttl = %d, period steps:%d, period resolution:%d, retrans count:%d, retrans steps:%d\r\n",
				   model_pub_stat->pub_addr, model_pub_stat->pub_ttl, model_pub_stat->pub_period.steps, model_pub_stat->pub_period.resol,
				   model_pub_stat->pub_retrans_info.count, model_pub_stat->pub_retrans_info.steps);
		} else {
			printf("[APP] Fail, status = %d!\r\n", model_pub_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST:
	case RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_SUB_LIST: {
		printf("[APP] Config client model receive message:MODEL SUB LIST SUCCESS\r\n");
		uint16_t *model_sub_list = (uint16_t *)param;
		if (model_sub_list[0] == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			uint16_t src = model_sub_list[1];
			uint16_t element_index = model_sub_list[6];
			uint32_t *model_id_pointer = (uint32_t *) &model_sub_list[4];
			uint32_t model_id = *model_id_pointer;
			uint16_t addr_count = model_sub_list[7];
			printf("Model sub list: src 0x%04x element idx %d model id 0x%08x\r\n\t", src,
				   element_index, (unsigned int)model_id);
			for (uint8_t loop = 0; loop < addr_count; loop++) {
				printf(" 0x%04x", model_sub_list[loop + 8]);
			}
			printf("\r\n");
		} else {
			printf("[APP] Fail, status = %d!\r\n", model_sub_list[0]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT: {
		rtk_bt_mesh_cfg_net_key_stat_t *net_key_stat;
		net_key_stat = (rtk_bt_mesh_cfg_net_key_stat_t *)param;
		if (net_key_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Success!\r\n");
		} else {
			printf("[APP] Fail, status = %d!\r\n", net_key_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST: {
		printf("[APP] Config client model receive message:NET_KEY_LIST SUCCESS\r\n");
		uint16_t *net_key_list = (uint16_t *)param;;
		if (net_key_list[0] == 1) {
			uint16_t key_count = net_key_list[1];
			printf("NetKey List: num = %d!\r\n\t", key_count);
			for (uint16_t loop = 0; loop < key_count; loop++) {
				printf(" 0x%03x", net_key_list[loop + 4]);
			}
			printf("\r\n");
		} else {
			printf("[APP] Fail, len = %d!\r\n", net_key_list[1]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST: {
		printf("[APP] Config client model receive message:APP_KEY_LIST SUCCESS\r\n");
		uint16_t *app_key_list = (uint16_t *)param;
		if (app_key_list[0] == 1) {
			uint16_t net_key_index = app_key_list[1];
			uint16_t key_count = app_key_list[4];
			printf("AppKey List: NetKeyIndex = 0x%03x num = %d!\r\n\t", net_key_index,
				   key_count);
			for (uint16_t loop = 0; loop < key_count; loop++) {
				printf(" 0x%03x", app_key_list[loop + 5]);
			}
			printf("\r\n");
		} else {
			printf("[APP] Fail, stat = %d len = %d!\r\n", app_key_list[1], app_key_list[4]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT: {
		rtk_bt_mesh_cfg_node_identity_stat_t *node_identity_stat;
		node_identity_stat = (rtk_bt_mesh_cfg_node_identity_stat_t *)param;
		if (node_identity_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Node identity state: %d on NetKeyIndex 0x%03x\r\n", node_identity_stat->identity,
				   node_identity_stat->net_key_index);
		} else {
			printf("[APP] Fail, stat = %d!\r\n", node_identity_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST:
	case RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_APP_LIST: {
		printf("\r\n [APP] Config client model receive message:MODEL_APP_LIST SUCCESS\r\n");
		uint16_t *model_app_list = (uint16_t *)param;
		if (model_app_list[0] == 1) {

			uint16_t element_index = model_app_list[1];
			uint32_t *model_id_pointer = (uint32_t *) & model_app_list[4];
			uint32_t model_id = *model_id_pointer;
			uint16_t key_count = model_app_list[6];
			printf("Model AppKey List: Element Index = %d model id = 0x%08x num = %d!\r\n\t",
				   element_index, (unsigned int) model_id, key_count);
			for (uint16_t loop = 0; loop < key_count; loop++) {
				printf(" 0x%03x", model_app_list[loop + 7]);
			}
			printf("\r\n");
		} else {
			printf("[APP] Fail, stat = %d len = %d!\r\n", model_app_list[1], model_app_list[4]);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_FRND_STAT: {
		rtk_bt_mesh_cfg_frnd_stat_t *frnd_stat;
		frnd_stat = (rtk_bt_mesh_cfg_frnd_stat_t *)param;
		printf("[APP] frnd state: %d\r\n", frnd_stat->state);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TO_STAT: {
		rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t *lpn_poll_to_stat;
		lpn_poll_to_stat = (rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t *)param;
		printf("[APP] fn = 0x%04x lpn = 0x%04x \r\n", lpn_poll_to_stat->lpn_addr,
			   lpn_poll_to_stat->poll_to[0] + (lpn_poll_to_stat->poll_to[1] << 8) + (lpn_poll_to_stat->poll_to[2] << 16));
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT: {
		rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t *key_refresh_phase_stat;
		key_refresh_phase_stat = (rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t *)param;
		if (key_refresh_phase_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Node 0x%04x: NetKeyIndex = 0x%03x state = %d\r\n", key_refresh_phase_stat->src,
				   key_refresh_phase_stat->net_key_index, key_refresh_phase_stat->state);
		} else {
			printf("[APP] Fail, status = %d!\r\n", key_refresh_phase_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT: {
		rtk_bt_mesh_cfg_hb_pub_stat_t *hb_pub_stat;
		hb_pub_stat = (rtk_bt_mesh_cfg_hb_pub_stat_t *)param;
		if (hb_pub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Hb: pub dst = 0x%04x count_log:%d, period_log:%d, ttl = %d NetKeyIndex = 0x%03x\r\n",
				   hb_pub_stat->dst, hb_pub_stat->count_log, hb_pub_stat->period_log, hb_pub_stat->ttl, hb_pub_stat->net_key_index);
		} else {
			printf("[APP] Fail, status = %d!\r\n", hb_pub_stat->stat);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT: {
		rtk_bt_mesh_cfg_hb_sub_stat_t *hb_sub_stat;
		hb_sub_stat = (rtk_bt_mesh_cfg_hb_sub_stat_t *)param;
		if (hb_sub_stat->stat == RTK_BT_MESH_MSG_STAT_SUCCESS) {
			printf("[APP] Hb: sub src = 0x%04x dst = 0x%04x period_log = %d count_log = %d hops range = [%d, %d]\r\n", hb_sub_stat->src,
				   hb_sub_stat->dst, hb_sub_stat->period_log,  hb_sub_stat->count_log, hb_sub_stat->min_hops, hb_sub_stat->max_hops);
		} else {
			printf("[APP] Fail, status = %d!\r\n", hb_sub_stat->stat);
		}
		break;
	}
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_onoff_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS: {
		rtk_bt_mesh_generic_onoff_client_evt_status_t *onoff_status;
		onoff_status = (rtk_bt_mesh_generic_onoff_client_evt_status_t *)param;
		if (onoff_status->optional) {
			printf("[APP] Generic on off client model receive message: src = 0x%x, present = %d, target = %d, remain time = step(%d), \
resolution(%d)\r\n", onoff_status->src, onoff_status->present_on_off, onoff_status->target_on_off,
				   onoff_status->remaining_time.num_steps, onoff_status->remaining_time.step_resolution);
		} else {
			printf("[APP] Generic on off client model receive message: src = 0x%x, present = %d\r\n", onoff_status->src,
				   onoff_status->present_on_off);
		}
		break;
	}
	default:
		printf("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

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
		            printf("[APP] Datatrans write message len is %d,extend max val %d,only write %d bytes\r\n", p_data_write->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(datatrans_sample_data, p_data_write->data, p_data_write->data_len);
		printf("[APP] Remote write %d bytes: ", p_data_write->data_len);
		mesh_data_uart_dump(p_data_write->data, p_data_write->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS: {
		rtk_bt_mesh_datatrans_client_write_event_t *p_write_status = (rtk_bt_mesh_datatrans_client_write_event_t *)param;
		printf("[APP] Datatrans model receive message: Src %d ,written %d bytes, status:%d\r\n", p_write_status->src, p_write_status->written_len,
			   p_write_status->status);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT: {
		rtk_bt_mesh_datatrans_client_read_event_t *p_read_date = (rtk_bt_mesh_datatrans_client_read_event_t *)param;
		printf("[APP] Datatrans model receive message,read %d bytes date from remote src %d :\r\n", p_read_date->data_len, p_read_date->src);
		mesh_data_uart_dump(p_read_date->data, p_read_date->data_len);
		break;
	}

	case RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_READ: {
		rtk_bt_mesh_datatrans_server_read_event_t *server_read = (rtk_bt_mesh_datatrans_server_read_event_t *)param;
		/*
		        // Avoid compile warning: data_len(uint8_t) always <= DATA_TRANS_DATA_MAX_LEN(0xff)
		        if (server_read->data_len > DATA_TRANS_DATA_MAX_LEN) {
		            server_read->data_len = DATA_TRANS_DATA_MAX_LEN;
		            printf("[APP] Datatrans read message len is %d, extend max val %d, only read %d bytes\r\n", server_read->data_len, DATA_TRANS_DATA_MAX_LEN,
		                   DATA_TRANS_DATA_MAX_LEN);
		        }
		*/
		memcpy(server_read->data, datatrans_sample_data, server_read->data_len);
		printf("[APP] Remote read %d bytes data\r\n", server_read->data_len);
		break;
	}

	default:
		printf("[%s] Unknown event code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_remote_prov_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_CAPA_STATUS: {
		rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *scan_capa_status = (rtk_bt_mesh_rmt_prov_client_scan_capa_status_t *)param;
		printf("[APP] Remote prov scan capability status: src 0x%x, max scan items %d, support active scan %d\r\n", scan_capa_status->src, \
			   scan_capa_status->max_scan_items, (int)scan_capa_status->support_active_scan);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_STATUS: {
		rtk_bt_mesh_rmt_prov_client_scan_status_t *scan_status = (rtk_bt_mesh_rmt_prov_client_scan_status_t *)param;
		printf("[APP] Remote prov scan status: src 0x%x, status %d, scan state %d, scanned items limit %d, timeout %d\r\n", scan_status->src, \
			   scan_status->prov_status, scan_status->scan_status, scan_status->scanned_items_limit, scan_status->timeout);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_SCAN_REPORT: {
		rtk_bt_mesh_rmt_prov_client_scan_report_t *scan_report = (rtk_bt_mesh_rmt_prov_client_scan_report_t *)param;
		if (scan_report->uri_hash) {
			printf("[APP] Remote prov scan report: src 0x%x, rssi %d, oob %d, uri hash %d, uuid ", scan_report->src, scan_report->rssi, scan_report->oob,
				   (int)scan_report->uri_hash);
		} else {
			printf("[APP] Remote prov scan report: src 0x%x, rssi %d, oob %d, uuid ", scan_report->src, scan_report->rssi, scan_report->oob);
		}
		mesh_data_uart_dump(scan_report->uuid, 16);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_STATUS: {
		rtk_bt_mesh_rmt_prov_client_link_status_t *link_status = (rtk_bt_mesh_rmt_prov_client_link_status_t *)param;
		printf("[APP] Remote prov link status: src 0x%x, status %d, link state %d \r\n", link_status->src, link_status->prov_status, link_status->link_status);
		break;
	}
	case RTK_BT_MESH_REMOTE_PROV_CLIENT_EVT_LINK_REPORT: {
		rtk_bt_mesh_rmt_prov_client_link_report_t *link_report = (rtk_bt_mesh_rmt_prov_client_link_report_t *)param;
		printf("[APP] Remote prov link report: src 0x%x, status %d, link state %d", link_report->src, link_report->prov_status, link_report->link_status);
		if (RTK_BT_MESH_RMT_PROV_LINK_NOT_CLOSE != link_report->close_reason) {
			printf("link close reason:%d\r\n", link_report->close_reason);
		} else {
			printf("\r\n");
		}
		break;
	}
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_lightness_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_t *lightness_status;
		lightness_status = (rtk_bt_mesh_light_lightness_client_status_t *) param;
		if (lightness_status->optional) {
			printf("[APP] light lightness client model receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
				   lightness_status->src,
				   lightness_status->present_lightness,
				   lightness_status->target_lightness,
				   lightness_status->remaining_time.num_steps,
				   lightness_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light lightness client model receive: src %d, present %d\r\n", lightness_status->src,
				   lightness_status->present_lightness);
		}
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_t *lightness_linear_status;
		lightness_linear_status = (rtk_bt_mesh_light_lightness_client_status_t *) param;
		if (lightness_linear_status->optional) {
			printf("[APP] light lightness client model receive: src %d, present %d, target %d, remain time step(%d) resolution(%d)\r\n",
				   lightness_linear_status->src,
				   lightness_linear_status->present_lightness,
				   lightness_linear_status->target_lightness,
				   lightness_linear_status->remaining_time.num_steps,
				   lightness_linear_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light lightness client model receive: src %d, present %d\r\n", lightness_linear_status->src,
				   lightness_linear_status->present_lightness);
		}
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_last_t *lightness_last_status;
		lightness_last_status = (rtk_bt_mesh_light_lightness_client_status_last_t *) param;
		printf("[APP] light lightness client model receive: src %d, lightness %d\r\n", lightness_last_status->src,
			   lightness_last_status->lightness);
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_default_t *lightness_default_status;
		lightness_default_status = (rtk_bt_mesh_light_lightness_client_status_default_t *) param;
		printf("[APP] light lightness client model receive: src %d, lightness %d\r\n", lightness_default_status->src,
			   lightness_default_status->lightness);
		break;
	}

	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lightness_client_status_range_t *lightness_range_status;
		lightness_range_status = (rtk_bt_mesh_light_lightness_client_status_range_t *) param;
		printf("[APP] light lightness client model receive: src %d, status %d, min %d, max %d\r\n",
			   lightness_range_status->src, lightness_range_status->status, lightness_range_status->range_min, lightness_range_status->range_max);
		break;
	}
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_ctl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_t *ctl_status;
		ctl_status = (rtk_bt_mesh_light_ctl_client_status_t *)param;
		if (ctl_status->optional) {
			printf("[APP] light ctl client model receive: src %d, present lightness %d, present temperature %d, target lightness %d, target temperature %d, remain time step(%d) resolution(%d)\r\n",
				   ctl_status->src,
				   ctl_status->present_lightness,
				   ctl_status->present_temperature,
				   ctl_status->target_lightness, ctl_status->target_temperature,
				   ctl_status->remaining_time.num_steps,
				   ctl_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light ctl client model receive: src %d, present lightness %d, present temperature %d\r\n",
				   ctl_status->src, ctl_status->present_lightness, ctl_status->present_temperature);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_temperature_t *ctl_temperature_status;
		ctl_temperature_status = (rtk_bt_mesh_light_ctl_client_status_temperature_t *) param;
		if (ctl_temperature_status->optional) {
			printf("[APP] light ctl client model receive: src %d, present temperature %d, present delta_uv %d, target temperature %d, target delta_uv %d, remain time step(%d), resolution(%d)\r\n",
				   ctl_temperature_status->src,
				   ctl_temperature_status->present_temperature,
				   ctl_temperature_status->present_delta_uv,
				   ctl_temperature_status->target_temperature, ctl_temperature_status->target_delta_uv,
				   ctl_temperature_status->remaining_time.num_steps,
				   ctl_temperature_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light ctl client model receive: src %d, present temperature %d, present delta_uv %d\r\n",
				   ctl_temperature_status->src, ctl_temperature_status->present_temperature, ctl_temperature_status->present_delta_uv);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS: {
		rtk_bt_mesh_light_ctl_client_status_temperature_range_t *ctl_temperature_range_status;
		ctl_temperature_range_status = (rtk_bt_mesh_light_ctl_client_status_temperature_range_t *) param;
		printf("[APP] light ctl client model receive: src %d, status %d, min %d, max %d\r\n",
			   ctl_temperature_range_status->src, ctl_temperature_range_status->status,
			   ctl_temperature_range_status->range_min, ctl_temperature_range_status->range_max);
	}
	break;
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS : {
		rtk_bt_mesh_light_ctl_client_status_default_t *ctl_default_status;
		ctl_default_status = (rtk_bt_mesh_light_ctl_client_status_default_t *) param;
		printf("[APP] light ctl client model receive: src  %d, lightness %d, temperature %d, delta_uv %d\r\n",
			   ctl_default_status->src, ctl_default_status->lightness, ctl_default_status->temperature, ctl_default_status->delta_uv);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_hsl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_t *hsl_status;
		hsl_status = (rtk_bt_mesh_light_hsl_client_status_t *)param;
		if (hsl_status->optional) {
			printf("[APP] light hsl client model receive: src %d, present lightness %d, present hue %d, present saturation %d, remain time step(%d) resolution(%d)\r\n",
				   hsl_status->src,
				   hsl_status->lightness,
				   hsl_status->hue, hsl_status->saturation,
				   hsl_status->remaining_time.num_steps,
				   hsl_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light hsl client model receive: src %d, present lightness %d, present hue %d, presnet saturation %d\r\n",
				   hsl_status->src, hsl_status->lightness, hsl_status->hue, hsl_status->saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_t *hsl_target_status;
		hsl_target_status = (rtk_bt_mesh_light_hsl_client_status_t *)param;
		if (hsl_target_status->optional) {
			printf("[APP] light hsl client model receive: src %d, target lightness %d, target hue %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
				   hsl_target_status->src,
				   hsl_target_status->lightness,
				   hsl_target_status->hue, hsl_target_status->saturation,
				   hsl_target_status->remaining_time.num_steps,
				   hsl_target_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light hsl client model receive: src %d, target lightness %d, target hue %d, target saturation %d\r\n",
				   hsl_target_status->src, hsl_target_status->lightness, hsl_target_status->hue, hsl_target_status->saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_hue_t *hsl_last_status;
		hsl_last_status = (rtk_bt_mesh_light_hsl_client_status_hue_t *)param;
		if (hsl_last_status->optional) {
			printf("[APP] light hsl client model receive: src %d, present hue %d, target hue %d, remain time step(%d) resolution(%d)\r\n",
				   hsl_last_status->src,
				   hsl_last_status->present_hue,
				   hsl_last_status->target_hue,
				   hsl_last_status->remaining_time.num_steps,
				   hsl_last_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light hsl client model receive: src %d, present hue %d\r\n",
				   hsl_last_status->src, hsl_last_status->present_hue);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_saturation_t *hsl_saturation_status;
		hsl_saturation_status = (rtk_bt_mesh_light_hsl_client_status_saturation_t *)param;
		if (hsl_saturation_status->optional) {
			printf("[APP] light hsl client model receive: src %d, present saturation %d, target saturation %d, remain time step(%d) resolution(%d)\r\n",
				   hsl_saturation_status->src,
				   hsl_saturation_status->present_saturation,
				   hsl_saturation_status->target_saturation,
				   hsl_saturation_status->remaining_time.num_steps,
				   hsl_saturation_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light hsl client model receive: src %d, present saturation %d\r\n",
				   hsl_saturation_status->src, hsl_saturation_status->present_saturation);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_default_t *hsl_default_status;
		hsl_default_status = (rtk_bt_mesh_light_hsl_client_status_default_t *)param;
		printf("[APP] light hsl client receive: src %d, lightness %d, hue %d, saturation %d\r\n",
			   hsl_default_status->src, hsl_default_status->lightness, hsl_default_status->hue, hsl_default_status->saturation);
	}
	break;
	case RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_hsl_client_status_range_t *hsl_range_status;
		hsl_range_status = (rtk_bt_mesh_light_hsl_client_status_range_t *)param;
		printf("[APP] light hsl client receive: src %d, status %d, hue min %d, hue max %d, saturation min %d, saturation max %d\r\n",
			   hsl_range_status->src, hsl_range_status->status, hsl_range_status->hue_range_min, hsl_range_status->hue_range_max,
			   hsl_range_status->saturation_range_min, hsl_range_status->saturation_range_max);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_xyl_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_t *xyl_status;
		xyl_status = (rtk_bt_mesh_light_xyl_client_status_t *)param;
		if (xyl_status->optional) {
			printf("[APP] light xyl client receive: src %d, present lightness %d, present xyl_x %d, present xyl_y %d, remain time step(%d) resolution(%d)\r\n",
				   xyl_status->src,
				   xyl_status->xyl.xyl_lightness,
				   xyl_status->xyl.xyl_x, xyl_status->xyl.xyl_y,
				   xyl_status->remaining_time.num_steps,
				   xyl_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light xyl client receive: src %d, present lightness %d, present xyl_x %d, presnet xyl_y %d\r\n",
				   xyl_status->src, xyl_status->xyl.xyl_lightness, xyl_status->xyl.xyl_x, xyl_status->xyl.xyl_y);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_t *xyl_target_status;
		xyl_target_status = (rtk_bt_mesh_light_xyl_client_status_t *)param;
		if (xyl_target_status->optional) {
			printf("[APP] light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d, remain time step(%d) resolution(%d)\r\n",
				   xyl_target_status->src,
				   xyl_target_status->xyl.xyl_lightness,
				   xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y,
				   xyl_target_status->remaining_time.num_steps,
				   xyl_target_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light xyl client receive: src %d, target lightness %d, target xyl_x %d, target xyl_y %d\r\n",
				   xyl_target_status->src, xyl_target_status->xyl.xyl_lightness, xyl_target_status->xyl.xyl_x, xyl_target_status->xyl.xyl_y);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_default_t *xyl_default_status;
		xyl_default_status = (rtk_bt_mesh_light_xyl_client_status_default_t *)param;
		printf("[APP] light xyl client receive: src %d, lightness %d, xyl_x %d, xyl_y %d\r\n",
			   xyl_default_status->src, xyl_default_status->xyl.xyl_lightness, xyl_default_status->xyl.xyl_x, xyl_default_status->xyl.xyl_y);
	}
	break;
	case RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_xyl_client_status_range_t *xyl_range_status;
		xyl_range_status = (rtk_bt_mesh_light_xyl_client_status_range_t *)param;
		printf("[APP] light xyl client receive: src %d, status %d, xyl_x min %d, xyl_x max %d, xyl_y min %d, xyl_y max %d\r\n",
			   xyl_range_status->src, xyl_range_status->status, xyl_range_status->xyl_x_range_min, xyl_range_status->xyl_x_range_max,
			   xyl_range_status->xyl_y_range_min, xyl_range_status->xyl_y_range_max);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_light_lc_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_mode_status_t *lc_mode_status;
		lc_mode_status = (rtk_bt_mesh_light_lc_client_mode_status_t *)param;
		printf("[APP] light lc client receive: src %d, mode %d\r\n", lc_mode_status->src, lc_mode_status->mode);
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_om_status_t *lc_om_status;
		lc_om_status = (rtk_bt_mesh_light_lc_client_om_status_t *)param;
		printf("[APP] light lc client receive: src %d, occupancy mode %d\r\n", lc_om_status->src, lc_om_status->mode);
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_light_on_off_status_t *lc_lightonoff_status;
		lc_lightonoff_status = (rtk_bt_mesh_light_lc_client_light_on_off_status_t *)param;
		if (lc_lightonoff_status->optional) {
			printf("[APP] light lc client receive: src %d, present on off %d, target on off %d, remain time step(%d) resolution(%d)\r\n",
				   lc_lightonoff_status->src,
				   lc_lightonoff_status->present_on_off,
				   lc_lightonoff_status->target_on_off,
				   lc_lightonoff_status->remaining_time.num_steps,
				   lc_lightonoff_status->remaining_time.step_resolution);
		} else {
			printf("[APP] light lc client receive: src %d, present on off %d\r\n",
				   lc_lightonoff_status->src, lc_lightonoff_status->present_on_off);
		}
	}
	break;
	case RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_light_lc_client_property_status_t *lc_property_status;
		lc_property_status = (rtk_bt_mesh_light_lc_client_property_status_t *)param;
		printf("[APP] light lc client receive: src %d, property id 0x%x, property value %ld\r\n",
			   lc_property_status->src, lc_property_status->property_id, lc_property_status->property_value);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_time_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_time_client_status_t *time_status;
		time_status = (rtk_bt_mesh_time_client_status_t *)param;
		printf("[APP] time client receive: src %d, subsecond %d, uncertainty %d, time_authority %d, tai_utc_delta %d, time_zone_offset %d\r\n",
			   time_status->src, (time_status->tai_time).subsecond, (time_status->tai_time).uncertainty, (time_status->tai_time).time_authority,
			   (time_status->tai_time).tai_utc_delta, (time_status->tai_time).time_zone_offset);
		printf("tai_seconds[4]%d tai_seconds[3]%d tai_seconds[2]%d tai_seconds[1]%d tai_seconds[0]%d \r\n",
			   (time_status->tai_time).tai_seconds[4],
			   (time_status->tai_time).tai_seconds[3],
			   (time_status->tai_time).tai_seconds[2],
			   (time_status->tai_time).tai_seconds[1],
			   (time_status->tai_time).tai_seconds[0]);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS: {
		rtk_bt_mesh_time_client_status_role_t *role_status;
		role_status = (rtk_bt_mesh_time_client_status_role_t *)param;
		printf("[APP] time client receive: role %d\r\n", role_status->role);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS: {
		rtk_bt_mesh_time_client_status_zone_t *zone_status;
		zone_status = (rtk_bt_mesh_time_client_status_zone_t *)param;
		printf("[APP] time client receive: src %d time_zone_offset_current %d time_zone_offset_new %d\r\n",
			   zone_status->src, zone_status->time_zone_offset_current, zone_status->time_zone_offset_new);
		printf("tai_of_zone_change[4]%d tai_of_zone_change[3]%d tai_of_zone_change[2]%d tai_of_zone_change[1]%d tai_of_zone_change[0]%d \r\n",
			   zone_status->tai_of_zone_change[4],
			   zone_status->tai_of_zone_change[3],
			   zone_status->tai_of_zone_change[2],
			   zone_status->tai_of_zone_change[1],
			   zone_status->tai_of_zone_change[0]);
	}
	break;
	case RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS: {
		rtk_bt_mesh_time_client_status_tai_utc_delta_t *delta_status;
		delta_status = (rtk_bt_mesh_time_client_status_tai_utc_delta_t *)param;
		printf("[APP] time client receive: src %d, tai utc delta current %d, tai utc delta new %d\r\n",
			   delta_status->src, delta_status->tai_utc_delta_current, delta_status->tai_utc_delta_new);
		printf("tai_of_delta_change[4]%d tai_of_delta_change[3]%d tai_of_delta_change[2]%d tai_of_delta_change[1]%d tai_of_delta_change[0]%d \r\n",
			   delta_status->tai_of_delta_change[4],
			   delta_status->tai_of_delta_change[3],
			   delta_status->tai_of_delta_change[2],
			   delta_status->tai_of_delta_change[1],
			   delta_status->tai_of_delta_change[0]);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_scheduler_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scheduler_client_status_t *scheduler_status;
		scheduler_status = (rtk_bt_mesh_scheduler_client_status_t *)param;
		printf("[APP] scheduler client receive: src %d, schedulers 0x%4x\r\n",
			   scheduler_status->src, scheduler_status->schedulers);
	}
	break;
	case RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scheduler_client_status_action_t *action_status;
		action_status = (rtk_bt_mesh_scheduler_client_status_action_t *)param;
		printf(
			"[APP] scheduler client receive: " \
			"index = %d, year = %d, month = %d, day = %d, hour = %d, minute = %d, second = %d, " \
			"day_of_week = %d, action = %d, num_steps = %d, step_resolution = %d, scene_number = %d\r\n", \
			(action_status->scheduler).index, (action_status->scheduler).year, (action_status->scheduler).month, \
			(action_status->scheduler).day, (action_status->scheduler).hour, (action_status->scheduler).minute, (action_status->scheduler).second, \
			(action_status->scheduler).day_of_week, (action_status->scheduler).action, (action_status->scheduler).num_steps, \
			(action_status->scheduler).step_resolution, (action_status->scheduler).scene_number);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_scene_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_scene_client_status_t *scene_status;
		scene_status = (rtk_bt_mesh_scene_client_status_t *)param;
		if (scene_status->optional) {
			printf("[APP] scene client receive: src %d, status 0x%d, current scene %d, target scene %d, remaining time %d\r\n",
				   scene_status->src, scene_status->status, scene_status->current_scene,
				   scene_status->target_scene, (scene_status->remaining_time).num_steps);
		} else {
			printf("[APP] scene client receive: src %d, status 0x%d, current scene %d\r\n",
				   scene_status->src, scene_status->status, scene_status->current_scene);
		}

	}
	break;
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS: {
		uint8_t *pmemory = ((uint8_t *)param + 6);
		uint8_t array_len = *((uint8_t *)param + 5);
		uint16_t src = MESH_LE_EXTRN2WORD((uint8_t *)param);
		rtk_bt_mesh_scene_status_code_t status = *(((uint8_t *)param) + 2);
		uint16_t current_scene = MESH_LE_EXTRN2WORD(((uint8_t *)param) + 3);
		printf("[APP] scene client receive: src %d, status 0x%d, current scene %d, scene array len %d, scene array:\r\n",
			   src, status, current_scene, array_len);
		for (int i = 0; i < array_len; i ++) {
			printf("0x%4x ", MESH_LE_EXTRN2WORD(pmemory + 2 * i));
		}
		printf("\r\n");
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_default_transition_time_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_transition_time_client_status_t *transition_status;
		transition_status = (rtk_bt_mesh_generic_transition_time_client_status_t *)param;
		printf("[APP] generic default transition time client receive: src %d, resolution %d, num steps %d \r\n",
			   transition_status->src, transition_status->trans_time.step_resolution, transition_status->trans_time.num_steps);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_level_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_level_client_status_t *level_status;
		level_status = (rtk_bt_mesh_generic_level_client_status_t *)param;
		if (level_status->optional) {
			printf("[APP] generic level client receive: src %d, present level %d, target level %d, remaining time %d \r\n",
				   level_status->src, level_status->present_level, level_status->target_level, level_status->remaining_time.num_steps);
		} else {
			printf("[APP] generic level client receive: src %d, present level %d \r\n",
				   level_status->src, level_status->present_level);
		}
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_on_off_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_power_on_off_client_status_t *gpoo_status;
		gpoo_status = (rtk_bt_mesh_generic_power_on_off_client_status_t *)param;
		printf("[APP] generic power on off client receive: src %d, on power up %d \r\n",
			   gpoo_status->src, gpoo_status->on_power_up);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_power_level_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_t *gpl_status;
		gpl_status = (rtk_bt_mesh_generic_power_level_client_status_t *)param;
		if (gpl_status->optional) {
			printf("[APP] generic power level client receive: src %d, present power %d, target power %d, remaining time %d \r\n",
				   gpl_status->src, gpl_status->present_power, gpl_status->target_power, gpl_status->remaining_time.num_steps);
		} else {
			printf("[APP] generic power level client receive: src %d, present power %d \r\n",
				   gpl_status->src, gpl_status->present_power);
		}
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_simple_t *gpll_status;
		gpll_status = (rtk_bt_mesh_generic_power_level_client_status_simple_t *)param;
		printf("[APP] generic power level client receive: src %d, power %d \r\n",
			   gpll_status->src, gpll_status->power);
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_simple_t *gpld_status;
		gpld_status = (rtk_bt_mesh_generic_power_level_client_status_simple_t *)param;
		printf("[APP] generic power level client receive: src %d, power %d \r\n",
			   gpld_status->src, gpld_status->power);
	}
	break;
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS: {
		rtk_bt_mesh_generic_power_level_client_status_range_t *gplr_status;
		gplr_status = (rtk_bt_mesh_generic_power_level_client_status_range_t *)param;
		printf("[APP] generic power level client receive: src %d, status %d, range min %d, range max %d \r\n",
			   gplr_status->src, gplr_status->stat, gplr_status->range_min, gplr_status->range_max);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_battery_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_generic_battery_client_status_t *gb_status;
		gb_status = (rtk_bt_mesh_generic_battery_client_status_t *)param;
		printf("[APP] generic battery client receive: src = %d, battery level = %d, time to discharge = %ld, \
time to charge = %ld, presence = %d, indicator = %d, charging = %d, serviceability = %d\r\n",
			   gb_status->src,
			   gb_status->battery_level,
			   gb_status->time_to_discharge, gb_status->time_to_charge,
			   gb_status->flags.presence, gb_status->flags.indicator, gb_status->flags.charging,
			   gb_status->flags.serviceability);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_location_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS: {
		rtk_bt_mesh_generic_location_client_status_global_t *global_status;
		global_status = (rtk_bt_mesh_generic_location_client_status_global_t *)param;
		printf("[APP] Generic location client receive: src = %d, latitude = %ld, longitude = %ld, altitude = %d\r\n",
			   global_status->src,
			   global_status->global.global_latitude,
			   global_status->global.global_longitude,
			   global_status->global.global_altitude);
	}
	break;
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS: {
		rtk_bt_mesh_generic_location_client_status_local_t *local_status;
		local_status = (rtk_bt_mesh_generic_location_client_status_local_t *)param;
		printf("[APP] Generic location client receive: src = %d, north = %d, east = %d, altitude = %d, floor = %d, stationary = %d, updata time = %d, precision = %d\r\n",
			   local_status->src, local_status->local.local_north, local_status->local.local_east, local_status->local.local_altitude,
			   local_status->local.floor_num,
			   local_status->local.uncertainty.stationary, local_status->local.uncertainty.update_time,
			   local_status->local.uncertainty.precision);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_generic_property_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_STATUS: {
		uint16_t src = MESH_LE_EXTRN2WORD((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		printf("[APP] Generic property client receive: src = %d num_ids = %d", src, num_ids);
		if (num_ids > 0) {
			printf(", user property ids = ");
			for (uint8_t i = 0; i < num_ids; ++i) {
				printf("%x ", MESH_LE_EXTRN2WORD(pvalue + 2 * i));
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *user_property_status;
		user_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		printf("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d",
			   user_property_status->src, user_property_status->property_id, property_access);
		uint16_t value_len = MESH_LE_EXTRN2WORD((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		if (value_len > 0) {
			printf(", user property values = ");
			for (uint8_t i = 0; i < value_len; ++i) {
				printf("%d ", pvalue[i]);
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_STATUS: {
		uint16_t src = MESH_LE_EXTRN2WORD((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		printf("[APP] Generic property client receive: src = %d num_ids = %d", src, num_ids);
		if (num_ids > 0) {
			printf(", admin property ids = ");
			for (uint8_t i = 0; i < num_ids; ++i) {
				printf("%x ", MESH_LE_EXTRN2WORD(pvalue + 2 * i));
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *admin_property_status;
		admin_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		printf("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d",
			   admin_property_status->src, admin_property_status->property_id, property_access);
		uint16_t value_len = MESH_LE_EXTRN2WORD((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		if (value_len > 0) {
			printf(", admin property values = ");
			for (uint8_t i = 0; i < value_len; ++i) {
				printf("%d ", pvalue[i]);
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_STATUS: {
		uint16_t src = MESH_LE_EXTRN2WORD((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		printf("[APP] Generic property client receive: src = %d num_ids = %d", src, num_ids);
		if (num_ids > 0) {
			printf(", manufacturer property ids = ");
			for (uint8_t i = 0; i < num_ids; ++i) {
				printf("%x ", MESH_LE_EXTRN2WORD(pvalue + 2 * i));
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS: {
		rtk_bt_mesh_generic_property_client_status_t *manu_property_status;
		manu_property_status = (rtk_bt_mesh_generic_property_client_status_t *)param;
		uint8_t property_access = *((uint8_t *)param + 4);
		printf("[APP] Generic property client receive: src = %d,  property id = 0x%x, property access = %d",
			   manu_property_status->src, manu_property_status->property_id, property_access);
		uint16_t value_len = MESH_LE_EXTRN2WORD((uint8_t *)param + 5);
		uint8_t *pvalue = (uint8_t *)param + 7;
		if (value_len > 0) {
			printf(", manufacturer property values = ");
			for (uint8_t i = 0; i < value_len; ++i) {
				printf("%d ", pvalue[i]);
			}
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_STATUS: {
		uint16_t src = MESH_LE_EXTRN2WORD((uint8_t *)param);
		uint8_t num_ids = *((uint8_t *)param + 2);
		uint8_t *pvalue = (uint8_t *)param + 3;
		printf("[APP] Generic property client receive: src = %d num_ids = %d", src, num_ids);
		if (num_ids > 0) {
			printf(", client property ids = ");
			for (uint8_t i = 0; i < num_ids; ++i) {
				printf("%x ", MESH_LE_EXTRN2WORD(pvalue + 2 * i));
			}
		}
		printf("\r\n");
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
static rtk_bt_evt_cb_ret_t ble_mesh_sensor_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_SENSOR_DESCRIPTOR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_descriptor_t *descriptor_status;
		descriptor_status = (rtk_bt_mesh_sensor_client_status_descriptor_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		rtk_bt_mesh_sensor_descriptor_t descriptor;
		if (descriptor_status->property_id != 0 && descriptor_status->num_descriptors == 0) {
			printf("[APP] Sensor client receive: src %d, property id %d \r\n", descriptor_status->src, descriptor_status->property_id);
		} else {
			printf("[APP] Sensor client receive: src %d, \r\n", descriptor_status->src);
			for (int i = 0; i < descriptor_status->num_descriptors; i++) {
				memcpy(&descriptor, pdata + 8 * i, 8);
				printf("property_id: %d, positive_tolerance: %d, negative_tolerance: %d, sampling_function: %d,"\
					   "measurement_period: %d, update_interval: %d \r\n", \
					   descriptor.property_id, descriptor.positive_tolerance, descriptor.negative_tolerance,
					   descriptor.sampling_function, descriptor.measurement_period, descriptor.update_interval);
			}
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_CADENCE_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_cadence_t *cadence_status;
		cadence_status = (rtk_bt_mesh_sensor_client_status_cadence_t *)param;
		printf("[APP] Sensor client receive: src %d, property id %d \r\n", cadence_status->src, cadence_status->property_id);
		if (cadence_status->flag) {
			uint8_t *pdata = (uint8_t *)param + 5;
			uint8_t raw_value_len = *pdata;
			uint8_t fast_cadence_period_divisor = (*(pdata + 1)) & 0x7F;
			uint8_t status_trigger_type = ((*(pdata + 1)) & 0x80) >> 7;
			printf("fast_cadence_period_divisor: %d, status_trigger_type :%d \r\n",
				   fast_cadence_period_divisor, status_trigger_type);
			uint8_t trigger_len;
			if (status_trigger_type == 0) {
				trigger_len = raw_value_len;
			} else {
				trigger_len = 2;
			}
			pdata += 2;
			printf("status_trigger_delta_down: \r\n");
			mesh_data_uart_dump(pdata, trigger_len);
			pdata += trigger_len;
			printf("status_trigger_delta_up: \r\n");
			mesh_data_uart_dump(pdata, trigger_len);
			pdata += trigger_len;
			uint8_t status_min_interval = *pdata;
			printf("status_min_interval: %d \r\n", status_min_interval);
			pdata++;
			printf("fast_cadence_low: \r\n");
			mesh_data_uart_dump(pdata, raw_value_len);
			pdata += raw_value_len;
			printf("fast_cadence_high: \r\n");
			mesh_data_uart_dump(pdata, raw_value_len);
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_SETTINGS_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_settings_t *settings_status;
		settings_status = (rtk_bt_mesh_sensor_client_status_settings_t *)param;
		printf("[APP] Sensor client receive: src %d, property id %d, ids: \r\n", settings_status->src, settings_status->property_id);
		uint8_t *pdata = (uint8_t *)param + 6;
		for (int i = 0; i < settings_status->num_ids; i++) {
			printf("%d ", MESH_LE_EXTRN2WORD(pdata + 2 * i));
		}
		printf("\r\n");
	}
	break;
	case RTK_BT_MESH_SENSOR_SETTING_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_setting_t *setting_status;
		setting_status = (rtk_bt_mesh_sensor_client_status_setting_t *)param;
		printf("[APP] Sensor client receive: src %d, property id %d, setting property id: %d \r\n",
			   setting_status->src, setting_status->property_id, setting_status->setting_property_id);
		if (setting_status->flag) {
			uint8_t *pdata = (uint8_t *)param + 7;
			rtk_bt_mesh_sensor_setting_access_t setting_access = *(pdata + 2);
			pdata += 3;
			uint8_t raw_len = *pdata;
			pdata++;
			printf("setting access:%d, setting raw: ", setting_access);
			mesh_data_uart_dump(pdata, raw_len);
		}
	}
	break;
	case RTK_BT_MESH_SENSOR_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_t *sensor_status;
		sensor_status = (rtk_bt_mesh_sensor_client_status_t *)param;
		uint8_t *pdata = (uint8_t *)param + 4;
		printf("[APP] Sensor client receive: src %d, marshalled_sensor_data_len %d, data: \r\n",
			   sensor_status->src, sensor_status->marshalled_sensor_data_len);
		mesh_data_uart_dump(pdata, sensor_status->marshalled_sensor_data_len);
	}
	break;
	case RTK_BT_MESH_SENSOR_COLUMN_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_column_t *column_status;
		column_status = (rtk_bt_mesh_sensor_client_status_column_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		printf("[APP] Sensor client receive: src %d, property_id %d, column_raw_value_len: %d, column value: \r\n",
			   column_status->src, column_status->property_id, column_status->column_raw_value_len);
		mesh_data_uart_dump(pdata, column_status->column_raw_value_len);
	}
	break;
	case RTK_BT_MESH_SENSOR_SERIES_CLIENT_MODEL_STATUS: {
		rtk_bt_mesh_sensor_client_status_series_t *series_status;
		series_status = (rtk_bt_mesh_sensor_client_status_series_t *)param;
		uint8_t *pdata = (uint8_t *)param + 6;
		printf("[APP] Sensor client receive: src %d, property_id %d, series_raw_value_len: %d, series value: \r\n",
			   series_status->src, series_status->property_id, series_status->series_raw_value_len);
		mesh_data_uart_dump(pdata, series_status->series_raw_value_len);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}
#endif

static rtk_bt_evt_cb_ret_t ble_mesh_health_client_model_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
	(void)len;
	switch (evt_code) {
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS: {
		rtk_bt_mesh_health_client_status_attention_t *attn_status;
		attn_status = (rtk_bt_mesh_health_client_status_attention_t *)param;
		printf("[APP] Health client receive: attention = %d \r\n",
			   attn_status->attention);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_CURT_STATUS: {
		rtk_bt_mesh_health_client_status_t *health_status;
		health_status = (rtk_bt_mesh_health_client_status_t *)param;
		printf("[APP] Health client receive: src = %d, test id = %d, company id = 0x%04x, fault_array:\r\n",
			   health_status->src, health_status->test_id, health_status->company_id);
		/*May not work, if not work, need memcpy pointer address*/
		mesh_data_uart_dump(health_status->fault_array, health_status->fault_array_len);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS: {
		rtk_bt_mesh_health_client_status_t *fault_status;
		fault_status = (rtk_bt_mesh_health_client_status_t *)param;
		printf("[APP] Health client receive: src = %d, test id = %d, company id = 0x%04x, fault_array:\r\n",
			   fault_status->src, fault_status->test_id, fault_status->company_id);
		/*May not work, if not work, need memcpy pointer address*/
		mesh_data_uart_dump(fault_status->fault_array, fault_status->fault_array_len);
	}
	break;
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS: {
		rtk_bt_mesh_health_client_status_period_t *period_status;
		period_status = (rtk_bt_mesh_health_client_status_period_t *)param;
		printf("[APP] Health client receive: fast period divisor = %d \r\n", period_status->fast_period_divisor);
	}
	break;
	default:
		printf("[%s] Unknown evt_code:%d\r\n", __func__, evt_code);
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

static rtk_bt_mesh_stack_act_provisioner_init_setting_t pro_init_param = {
	// Set to 0 to use default val
	.unicast_addr = 0,
	.net_key = {0},
	.app_key = {0},
	/* Or set to the value you want
	.unicast_addr = 0x23,
	.net_key = {0x7d, 0xd7, 0x36, 0x4c, 0xd8, 0x42, 0xad, 0x18, 0xc1, 0x7c, 0x2b, 0x82, 0x0c, 0x84, 0xc3, 0xd6},
	.app_key = {0x63, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76, 0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48}
	*/
};

extern uint16_t rtk_bt_mesh_stack_provisioner_setting_init(rtk_bt_mesh_stack_act_provisioner_init_setting_t *init_setting);
int ble_mesh_provisioner_main(uint8_t enable)
{
	rtk_bt_app_conf_t bt_app_conf = {0};
	rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
	char addr_str[30] = {0};

	if (1 == enable) {
		//set GAP configuration
		bt_app_conf.app_profile_support = RTK_BT_PROFILE_MESH | RTK_BT_PROFILE_GATTS | RTK_BT_PROFILE_GATTC;
		bt_app_conf.bt_mesh_app_conf.bt_mesh_role = RTK_BT_MESH_ROLE_PROVISIONER;
		bt_app_conf.mtu_size = 180;
		bt_app_conf.master_init_mtu_req = true;
		bt_app_conf.prefer_all_phy = 0;
		bt_app_conf.prefer_tx_phy = 1 | 1 << 2;
		bt_app_conf.prefer_rx_phy = 1 | 1 << 2;
		bt_app_conf.max_tx_octets = 0x40;
		bt_app_conf.max_tx_time = 0x200;

		/* Enable BT */
		BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));

		// MUST DO:set provisioner unicast address,add net key and app key
		BT_APP_PROCESS(rtk_bt_mesh_stack_provisioner_setting_init(&pro_init_param));

		BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
		rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
		printf("[APP] BD_ADDR: %s\r\n", addr_str);

		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_mesh_gap_app_callback));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_STACK, ble_mesh_stack_app_callback));
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, ble_mesh_config_client_model_app_callback));
#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL, ble_mesh_generic_onoff_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, ble_mesh_datatrans_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_REMOTE_PROV_CLIENT_MODEL, ble_mesh_remote_prov_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, ble_mesh_light_lightness_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, ble_mesh_light_ctl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, ble_mesh_light_hsl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, ble_mesh_light_xyl_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, ble_mesh_light_lc_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, ble_mesh_time_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL, ble_mesh_scheduler_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, ble_mesh_scene_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL,
													ble_mesh_generic_default_transition_time_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, ble_mesh_generic_level_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL, ble_mesh_generic_power_on_off_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, ble_mesh_generic_power_level_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL, ble_mesh_generic_battery_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, ble_mesh_generic_location_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, ble_mesh_generic_property_client_model_app_callback));
#endif
#if defined(BT_MESH_ENABLE_SENSOR_CLIENT_MODEL) && BT_MESH_ENABLE_SENSOR_CLIENT_MODEL
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_SENSOR_CLIENT_MODEL, ble_mesh_sensor_client_model_app_callback));
#endif
		BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, ble_mesh_health_client_model_app_callback));
	} else if (0 == enable) {

		/* Disable BT */
		BT_APP_PROCESS(rtk_bt_disable());
	}

	return 0;
}
