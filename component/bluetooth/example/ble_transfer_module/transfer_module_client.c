/*
*******************************************************************************
* Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>

#include <basic_types.h>
#include <rtk_bt_def.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_att_defs.h>
#include <rtk_gcs_client.h>
#include <rtk_bt_le_gap.h>
#include <rtk_client_config.h>
#include <transfer_module_common.h>
#include <bt_utils.h>

#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#endif

static void print_uuid(uint8_t type, uint8_t *uuid)
{
	if (type == BT_UUID_TYPE_16) {
		BT_LOGA("%04x\r\n", LE_TO_U16(uuid));
		BT_AT_PRINT("%04x\r\n", LE_TO_U16(uuid));
	} else if (type == BT_UUID_TYPE_128) {
		BT_LOGA("%08x-%04x-%04x-%04x-%04x%08x\r\n",
				LE_TO_U32(uuid + 12), LE_TO_U16(uuid + 10),
				LE_TO_U16(uuid + 8), LE_TO_U16(uuid + 6),
				LE_TO_U16(uuid + 4), LE_TO_U32(uuid));
		BT_AT_PRINT("%08x-%04x-%04x-%04x-%04x%08x\r\n",
					LE_TO_U32(uuid + 12), LE_TO_U16(uuid + 10),
					LE_TO_U16(uuid + 8), LE_TO_U16(uuid + 6),
					LE_TO_U16(uuid + 4), LE_TO_U32(uuid));
	}
}

void ble_transfer_module_client_discover_res_hdl(void *data)
{
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	rtk_bt_status_t disc_status = disc_res->status;

	if (RTK_BT_STATUS_CONTINUE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_ALL:
			BT_LOGA("[APP] GATTC discover primary result: start_handle: 0x%04x, end_handle: 0x%04x, UUID: ",
					disc_res->disc_primary_all_per.start_handle, disc_res->disc_primary_all_per.end_handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%u,0x%04x,0x%04x,",
						disc_res->type, disc_res->conn_handle,
						disc_res->disc_primary_all_per.start_handle, disc_res->disc_primary_all_per.end_handle);
			print_uuid(disc_res->disc_primary_all_per.uuid_type, disc_res->disc_primary_all_per.uuid);
			break;

		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			BT_LOGA("[APP] GATTC discover primary by uuid result: start_handle: 0x%04x, end_handle: 0x%04x\r\n",
					disc_res->disc_primary_by_uuid_per.start_handle, disc_res->disc_primary_by_uuid_per.end_handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%u,0x%04x,0x%04x\r\n",
						disc_res->type, disc_res->conn_handle,
						disc_res->disc_primary_by_uuid_per.start_handle, disc_res->disc_primary_by_uuid_per.end_handle);
			break;

		case RTK_BT_GATT_DISCOVER_INCLUDE:
			BT_LOGA("[APP] GATTC discover include result: handle: 0x%04x, start_handle: 0x%04x, end_handle: 0x%04x, UUID: ",
					disc_res->disc_inc_per.handle, disc_res->disc_inc_per.start_handle, disc_res->disc_inc_per.end_handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%u,0x%04x,0x%04x,0x%04x,",
						disc_res->type, disc_res->conn_handle,
						disc_res->disc_inc_per.handle, disc_res->disc_inc_per.start_handle,
						disc_res->disc_inc_per.end_handle);
			print_uuid(disc_res->disc_inc_per.uuid_type, disc_res->disc_inc_per.uuid);
			break;

		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_BY_UUID:
			BT_LOGA("[APP] GATTC discover characteristic of a service result: handle: 0x%04x, "
					"properties: 0x%02x, value_handle: 0x%04x, UUID: ", disc_res->disc_char_all_per.handle,
					disc_res->disc_char_all_per.properties,
					disc_res->disc_char_all_per.value_handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%u,0x%04x,0x%02x,0x%04x,",
						disc_res->type, disc_res->conn_handle,
						disc_res->disc_char_all_per.handle, disc_res->disc_char_all_per.properties,
						disc_res->disc_char_all_per.value_handle);
			print_uuid(disc_res->disc_char_all_per.uuid_type, disc_res->disc_char_all_per.uuid);
			break;

		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			BT_LOGA("[APP] GATTC discover descriptor all result: handle: 0x%04x, UUID: ",
					disc_res->disc_descriptor_per.handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%u,0x%04x,",
						disc_res->type, disc_res->conn_handle,
						disc_res->disc_descriptor_per.handle);
			print_uuid(disc_res->disc_descriptor_per.uuid_type, disc_res->disc_descriptor_per.uuid);
			break;

		default:
			BT_LOGE("[APP] GATTC discover result event, wrong event type (%d)!\r\n", disc_res->type);
			break;
		}
	} else if (RTK_BT_STATUS_DONE == disc_status) {
		BT_LOGA("[APP] GATTC discover completed. profile_id: %d, conn_handle: %d, type: %d\r\n",
				disc_res->profile_id, disc_res->conn_handle, disc_res->type);
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND)) {
			bt_at_sync_set_result(disc_res->err_code);
			bt_at_sync_sem_give();
		}
#endif
		return;
	} else {
		BT_LOGE("[APP] GATTC discover failed! profile_id: %d, conn_handle: %d, type: %d, err: 0x%x\r\n",
				disc_res->profile_id, disc_res->conn_handle, disc_res->type, disc_res->err_code);
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND)) {
			bt_at_sync_set_result(disc_res->err_code);
			bt_at_sync_sem_give();
		}
#endif
		return;
	}
}

void ble_transfer_module_client_read_res_hdl(void *data)
{
	rtk_bt_gattc_read_ind_t *read_res = (rtk_bt_gattc_read_ind_t *)data;
	rtk_bt_status_t read_status = read_res->status;
	uint16_t handle = 1;

	if (RTK_BT_STATUS_FAIL == read_status) {
		BT_LOGE("[APP] GATTC read failed, profile_id: %d, conn_handle: %d, type: %d, err: 0x%x\r\n",
				read_res->profile_id, read_res->conn_handle, read_res->type, read_res->err_code);
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_READ_RESULT_IND)) {
			bt_at_sync_set_result(read_res->err_code);
			bt_at_sync_sem_give();
		}
#endif
		return;
	} else if (RTK_BT_STATUS_CONTINUE == read_status) {
		if (!read_res->by_handle.len || !read_res->by_handle.value) {
			BT_LOGE("[APP] GATT client read value is empty!\r\n");
#if defined(BT_AT_SYNC) && BT_AT_SYNC
			if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_READ_RESULT_IND)) {
				bt_at_sync_set_result(BT_AT_EVT_RESULT_FAIL);
				bt_at_sync_sem_give();
			}
#endif
			return;
		}
		switch (read_res->type) {
		case RTK_BT_GATT_CHAR_READ_BY_HANDLE:
			handle = read_res->by_handle.handle;
			BT_LOGA("[APP] GATT client read result, profile_id: %d, conn_handle: %d, type: %d, status: %d, handle: 0x%04x\r\n",
					read_res->profile_id, read_res->conn_handle, read_res->type, read_status, handle);
			BT_DUMPA("[APP] GATTC read result:\r\n", read_res->by_handle.value, read_res->by_handle.len);
			BT_AT_PRINT("+BLEGATTC:read,%d,%d,0x%04x,%d",
						read_res->conn_handle,
						read_res->type, handle, read_res->by_handle.len);
			BT_AT_DUMP("", read_res->by_handle.value, read_res->by_handle.len);
			break;

		case RTK_BT_GATT_CHAR_READ_BY_UUID:
			handle = read_res->by_uuid_per.handle;
			BT_LOGA("[APP] GATT client read result, profile_id: %d, conn_handle: %d, type: %d, status: %d, handle: 0x%04x\r\n",
					read_res->profile_id, read_res->conn_handle, read_res->type, read_status, handle);
			BT_DUMPA("[APP] GATTC read result:\r\n", read_res->by_uuid_per.value, read_res->by_uuid_per.len);
			BT_AT_PRINT("+BLEGATTC:read,%d,%d,0x%04x,%d",
						read_res->conn_handle,
						read_res->type, handle, read_res->by_handle.len);
			BT_AT_DUMP("", read_res->by_uuid_per.value, read_res->by_uuid_per.len);
			break;

		case RTK_BT_GATT_CHAR_READ_MULTIPLE_VARIABLE:
			BT_LOGA("[APP] GATT client read result, profile_id: %d, conn_handle: %d, type: %d, status: %d\r\n",
					read_res->profile_id, read_res->conn_handle, read_res->type, read_status);
			BT_DUMPA("[APP] GATTC read result:\r\n", read_res->multiple_variable_per.value, read_res->multiple_variable_per.len);
			BT_AT_PRINT("+BLEGATTC:read,%d,%d,%d",
						read_res->conn_handle, read_res->type, read_res->by_handle.len);
			BT_AT_DUMP("", read_res->multiple_variable_per.value, read_res->multiple_variable_per.len);
			break;

		default:
			break;
		}
	} else if (RTK_BT_STATUS_DONE == read_status) {
		BT_LOGA("[APP] GATTC read completed, profile_id: %d, conn_handle: %d, type: %d\r\n",
				read_res->profile_id, read_res->conn_handle, read_res->type);
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_READ_RESULT_IND)) {
			bt_at_sync_set_result(BT_AT_EVT_RESULT_SUCCESS);
			bt_at_sync_sem_give();
		}
#endif
	}

}

void ble_transfer_module_client_write_res_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
	rtk_bt_status_t write_status =  write_res->status;

	if (RTK_BT_STATUS_FAIL == write_status) {
		BT_LOGE("[APP] GATTC write failed, profile_id: %d, conn_handle: %d, type: %d, err: 0x%x\r\n",
				write_res->profile_id, write_res->conn_handle, write_res->type, write_res->err_code);
	} else if (RTK_BT_STATUS_DONE == write_status) {
		BT_LOGA("[APP] GATTC write completed, profile_id: %d, conn_handle: %d, type: %d, handle: 0x%x\r\n",
				write_res->profile_id, write_res->conn_handle, write_res->type, write_res->handle);
	} else {
		return;
	}

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	if (RTK_BT_STATUS_FAIL == write_status || RTK_BT_STATUS_DONE == write_status) {
		if (bt_at_sync_event_match_check(RTK_BT_GATTC_EVT_WRITE_RESULT_IND)) {
			bt_at_sync_set_result(write_res->err_code);
			bt_at_sync_sem_give();
		}
	}
#endif
}

void ble_transfer_module_client_notify_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *ntf_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;

	if (!ntf_ind->len || !ntf_ind->value) {
		BT_LOGE("[APP] GATTC notify received value is empty!\r\n");
		return;
	}
	BT_LOGA("[APP] GATTC notify received, profile_id: %d, conn_handle: %d, handle: 0x%x\r\n",
			ntf_ind->profile_id, ntf_ind->conn_handle, ntf_ind->value_handle);
	BT_DUMPA("[APP] GATTC notify event:\r\n", ntf_ind->value, ntf_ind->len);
	BT_AT_PRINT_INDICATE("+BLEGATTC:notify,%d,0x%x,%d",
						 ntf_ind->conn_handle, ntf_ind->value_handle, ntf_ind->len);
	BT_AT_DUMP("", ntf_ind->value, ntf_ind->len);
}

void ble_transfer_module_client_indicate_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *indicate_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;

	if (!indicate_ind->len || !indicate_ind->value) {
		BT_LOGE("[APP] GATTC indicate received value is empty!\r\n");
		return;
	}
	BT_LOGA("[APP] GATTC indicate received, profile_id: %d, conn_handle: %d, handle: 0x%x\r\n",
			indicate_ind->profile_id, indicate_ind->conn_handle, indicate_ind->value_handle);
	BT_DUMPA("[APP] GATTC indicate event:\r\n", indicate_ind->value, indicate_ind->len);
	BT_AT_PRINT_INDICATE("+BLEGATTC:indicate,%d,0x%x,%d",
						 indicate_ind->conn_handle, indicate_ind->value_handle, indicate_ind->len);
	BT_AT_DUMP("", indicate_ind->value, indicate_ind->len);
}

void ble_transfer_module_client_cccd_enable_hdl(void *data)
{
	rtk_bt_gattc_cccd_update_ind_t *cccd_update =
		(rtk_bt_gattc_cccd_update_ind_t *)data;
	rtk_bt_status_t status = cccd_update->status;
	if (RTK_BT_STATUS_DONE == status) {
		BT_LOGA("[APP] GATTC enable cccd succeed, profile_id: %d, conn_handle: %d, handle: 0x%x\r\n",
				cccd_update->profile_id, cccd_update->conn_handle, cccd_update->cccd_handle);
		BT_AT_PRINT("+BLEGATTC:en_cccd,0,%d,0x%x\r\n",
					cccd_update->conn_handle,
					cccd_update->cccd_handle);
	} else {
		BT_LOGE("[APP] GATTC enable cccd failed, profile_id: %d, conn_handle: %d, handle: 0x%x, err: 0x%x\r\n",
				cccd_update->profile_id, cccd_update->conn_handle,
				cccd_update->cccd_handle, cccd_update->err_code);
		BT_AT_PRINT("+BLEGATTC:en_cccd,-1,%d,0x%x,0x%x\r\n",
					cccd_update->conn_handle, cccd_update->cccd_handle,
					cccd_update->err_code);
	}
}

void ble_transfer_module_client_cccd_disable_hdl(void *data)
{
	rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
	rtk_bt_status_t status = cccd_update->status;
	if (RTK_BT_STATUS_DONE == status) {
		BT_LOGA("[APP] GATTC disable cccd succeed, profile_id: %d, conn_handle: %d, handle: 0x%x\r\n",
				cccd_update->profile_id, cccd_update->conn_handle, cccd_update->cccd_handle);
		BT_AT_PRINT("+BLEGATTC:dis_cccd,0,%d,0x%x\r\n",
					cccd_update->conn_handle,
					cccd_update->cccd_handle);
	} else {
		BT_LOGE("[APP] GATTC disable cccd failed, profile_id: %d, conn_handle: %d, handle: 0x%x, err: 0x%x\r\n",
				cccd_update->profile_id, cccd_update->conn_handle, cccd_update->cccd_handle, cccd_update->err_code);
		BT_AT_PRINT("+BLEGATTC:dis_cccd,-1,%d,0x%x,0x%x\r\n",
					cccd_update->conn_handle, cccd_update->cccd_handle,
					cccd_update->err_code);
	}
}

void ble_transfer_module_client_mtu_exchange_hdl(void *data)
{
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
	if (p_gatt_mtu_ind->result == RTK_BT_OK) {
		BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
				p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
	} else {
		BT_LOGE("[APP] GATTC mtu exchange fail \r\n");
	}

	return;
}

uint16_t ble_transfer_module_client_add(void)
{
	return rtk_bt_gattc_register_profile(TRANSFER_MODULE_CLIENT_PROFILE_ID);
}

rtk_bt_evt_cb_ret_t ble_transfer_module_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;

	if (!data) {
		return RTK_BT_EVT_CB_FAIL;
	}

	switch (event) {
	case RTK_BT_GATTC_EVT_MTU_EXCHANGE:
		ble_transfer_module_client_mtu_exchange_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		ble_transfer_module_client_discover_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_READ_RESULT_IND:
		ble_transfer_module_client_read_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		ble_transfer_module_client_write_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_NOTIFY_IND:
		ble_transfer_module_client_notify_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_INDICATE_IND:
		ble_transfer_module_client_indicate_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
		ble_transfer_module_client_cccd_enable_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND:
		ble_transfer_module_client_cccd_disable_hdl(data);
		break;
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}