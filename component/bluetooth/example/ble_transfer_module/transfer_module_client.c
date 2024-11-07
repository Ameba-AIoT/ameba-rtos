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

	return general_client_app_callback(event, data);
}