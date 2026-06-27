/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <rtk_tps.h>
#include <bt_utils.h>

#define TX_POWER_SRV_UUID                   0x1804
#define TX_POWER_LEVEL_CHAR_UUID            0x2A07

#define RTK_BT_UUID_TX_POWER_SRV            BT_UUID_DECLARE_16(TX_POWER_SRV_UUID)
#define RTK_BT_UUID_TX_POWER_LEVEL_CHAR     BT_UUID_DECLARE_16(TX_POWER_LEVEL_CHAR_UUID)

#define TX_POWER_LEVEL_CHAR_VAL_INDEX       2   /* read */

static int8_t tx_power_level = 0;

static rtk_bt_gatt_attr_t tx_power_attrs[] = {
	/* Primary Service: tx_power */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_TX_POWER_SRV),

	/* Characteristic: tx power level */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TX_POWER_LEVEL_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),
};

static struct rtk_bt_gatt_service tx_power_srv = RTK_BT_GATT_SERVICE(tx_power_attrs, TX_POWER_SRV_ID);


void tx_power_srv_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] TPS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] TPS register service failed, err: 0x%x\r\n", reg_srv_res->reg_status);
		}

		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};
		read_resp.app_id = p_read_ind->app_id;
		read_resp.conn_handle = p_read_ind->conn_handle;
		read_resp.cid = p_read_ind->cid;
		read_resp.index = p_read_ind->index;
		if (TX_POWER_LEVEL_CHAR_VAL_INDEX == p_read_ind->index) {
			read_resp.data = &tx_power_level;
			read_resp.len = sizeof(tx_power_level);
		} else {
			BT_LOGE("[APP] TPS read event unknown index: %d\r\n", p_read_ind->index);
			read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] TPS response for client read succeed!\r\n");
		} else {
			BT_LOGE("[APP] TPS response for client read failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
					read_resp.conn_handle, read_resp.index,
					read_resp.err_code);
		break;
	}

	default:
		break;
	}
}

void tx_power_srv_set_level(int8_t level)
{
	tx_power_level = level;
}

uint16_t tx_power_srv_add(void)
{
	tx_power_srv.type = GATT_SERVICE_OVER_BLE;
	tx_power_srv.server_info = 0;
	tx_power_srv.user_data = NULL;
	tx_power_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&tx_power_srv);
}
