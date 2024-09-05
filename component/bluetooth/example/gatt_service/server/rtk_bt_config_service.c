/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_service_config.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_config_service.h>

static rtk_bt_gatt_attr_t bt_config_attrs[] = {
	/* Primary Service: BT CONFIG */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_BT_CONFIG),

	/* Data TRX Characteristic */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_DATA_TRX_VAL,
							   RTK_BT_GATT_CHRC_READ | RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service bt_config_srv = RTK_BT_GATT_SERVICE(bt_config_attrs, BT_CONFIG_SRV_ID);

extern void rtk_bt_config_handle_read_request(uint8_t **pRead_buf, uint32_t *pRead_buf_len, uint16_t read_offset);
extern void rtk_bt_config_send_cmd(uint8_t *cmd, uint32_t len);
void bt_config_service_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
			BT_LOGA("[APP] bt config service register succeed!\r\n");
		} else {
			BT_LOGE("[APP] bt config service register failed, err: 0x%x\r\n", p_gatts_reg_ind->reg_status);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		uint16_t offset = p_read_ind->offset;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};
		uint8_t *read_buf = NULL;
		uint32_t read_buf_len = 0;
		read_resp.app_id = p_read_ind->app_id;
		read_resp.conn_handle = p_read_ind->conn_handle;
		read_resp.cid = p_read_ind->cid;
		read_resp.index = p_read_ind->index;
		if (BT_CONFIG_READ_WRITE_INDEX == p_read_ind->index) {
			rtk_bt_config_handle_read_request(&read_buf, &read_buf_len, offset);
			read_resp.data = read_buf;
			read_resp.len = read_buf_len;
		} else {
			BT_LOGE("[APP] BT Config read event unknown index: %d\r\n", p_read_ind->index);
			read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret) {
			//BT_LOGA("[APP] BT Config respond for client read success, offset: %d\r\n", offset);
		} else {
			BT_LOGE("[APP] BT Config respond for client read failed, err: 0x%x\r\n", ret);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		rtk_bt_gatts_write_resp_param_t write_resp = {0};
		write_resp.app_id = p_write_ind->app_id;
		write_resp.conn_handle = p_write_ind->conn_handle;
		write_resp.cid = p_write_ind->cid,
		write_resp.index = p_write_ind->index;
		write_resp.type = p_write_ind->type;

		if (!p_write_ind->len || !p_write_ind->value) {
			BT_LOGE("[APP] BT Config write value is empty!\r\n");
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
			goto send_write_rsp;
		}

		if (BT_CONFIG_READ_WRITE_INDEX == p_write_ind->index) {
			rtk_bt_config_send_cmd(p_write_ind->value, p_write_ind->len);
			write_resp.err_code = 0;
		} else {
			BT_LOGE("[APP] BT Config write event unknown index: %d\r\n", p_write_ind->index);
			write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

send_write_rsp:
		ret = rtk_bt_gatts_write_resp(&write_resp);
		if (RTK_BT_OK == ret) {
			//BT_LOGA("[APP] BT Config response for client write success!\r\n");
		} else {
			BT_LOGE("[APP] BT Config response for client write failed, err: 0x%x\r\n", ret);
		}
		break;
	}
	default:
		break;
	}
}

uint16_t bt_config_srv_add(void)
{
	bt_config_srv.type = GATT_SERVICE_OVER_BLE;
	bt_config_srv.server_info = 0;
	bt_config_srv.user_data = NULL;
	bt_config_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&bt_config_srv);
}
