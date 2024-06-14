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
#include <rtk_ias.h>
#include <bt_utils.h>

#define IMMEDIATE_ALERT_SRV_UUID            0x1802
#define ALERT_LEVEL_CHAR_UUID               0x2A06

#define RTK_BT_UUID_IMMEDIATE_ALERT_SRV     BT_UUID_DECLARE_16(IMMEDIATE_ALERT_SRV_UUID)
#define RTK_BT_UUID_ALERT_LEVEL_CHAR        BT_UUID_DECLARE_16(ALERT_LEVEL_CHAR_UUID)

#define ALERT_LEVEL_CHAR_VAL_INDEX          2   /* write */

static rtk_bt_gatt_attr_t immediate_alert_attrs[] = {
	/* Primary Service: immediate_alert */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_IMMEDIATE_ALERT_SRV),

	/* Characteristic: alert level */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_ALERT_LEVEL_CHAR,
							   RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service immediate_alert_srv = RTK_BT_GATT_SERVICE(immediate_alert_attrs, IMMEDIATE_ALERT_SRV_ID);


void immediate_alert_srv_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] IAS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] IAS register service failed, err: 0x%x\r\n", reg_srv_res->reg_status);
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
		BT_LOGE("[APP] IAS read event, while no readable attr, unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] IAS response for client read succeed!\r\n");
		} else {
			BT_LOGE("[APP] IAS response for client read failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
					read_resp.conn_handle, read_resp.index,
					read_resp.err_code);
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		uint8_t *value;
		rtk_bt_gatts_write_resp_param_t write_resp = {0};
		write_resp.app_id = p_write_ind->app_id;
		write_resp.conn_handle = p_write_ind->conn_handle;
		write_resp.cid = p_write_ind->cid;
		write_resp.index = p_write_ind->index;
		write_resp.type = p_write_ind->type;

		if (!p_write_ind->len || !p_write_ind->value) {
			BT_LOGE("[APP] IAS write value is empty!\r\n");
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
			goto send_write_rsp;
		}

		if (ALERT_LEVEL_CHAR_VAL_INDEX == p_write_ind->index) {
			value = (uint8_t *)(p_write_ind->value);
			BT_LOGA("[APP] IAS write by remote, value: %d, type: %d\r\n",
					*value, p_write_ind->type);
			BT_AT_PRINT("+BLEGATTS:write,%u,%u,%u,%u,%u,%u\r\n",
						p_write_ind->app_id, p_write_ind->conn_handle, p_write_ind->index,
						p_write_ind->len, p_write_ind->type, *value);
		} else {
			BT_LOGE("[APP] IAS write event unknown index: %d\r\n", p_write_ind->index);
			write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

send_write_rsp:
		ret = rtk_bt_gatts_write_resp(&write_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] IAS response for client write success!\r\n");
		} else {
			BT_LOGE("[APP] IAS response for client write failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
					write_resp.conn_handle, write_resp.index,
					write_resp.type, write_resp.err_code);
		break;
	}

	default:
		break;
	}
}

uint16_t immediate_alert_srv_add(void)
{
	immediate_alert_srv.type = GATT_SERVICE_OVER_BLE;
	immediate_alert_srv.server_info = 0;
	immediate_alert_srv.user_data = NULL;
	immediate_alert_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&immediate_alert_srv);
}

