/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <bt_utils.h>

static struct bt_uuid_128 service_uuid = BT_UUID_INIT_128(
											 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
											 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xfd
										 );
static struct bt_uuid_128 read_charac_uuid = BT_UUID_INIT_128(
												 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe,
												 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
											 );
static struct bt_uuid_128 write_charac_uuid = BT_UUID_INIT_128(
												  0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01,
												  0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11
											  );

#define LONG_UUID_READ_INDEX            2
#define LONG_UUID_WRITE_INDEX           4

#define LONG_UUID_READ_CHARAC_LEN       20
static uint8_t long_uuid_srv_read_val[LONG_UUID_READ_CHARAC_LEN] = "Long UUID Service";

static rtk_bt_gatt_attr_t long_uuid_srv_attrs[] = {
	/* Primary Service:  long uuid service */
	RTK_BT_GATT_PRIMARY_SERVICE(&service_uuid),

	/* Characteristic: demo for read */
	RTK_BT_GATT_CHARACTERISTIC(&read_charac_uuid,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Characteristic: demo for write */
	RTK_BT_GATT_CHARACTERISTIC(&write_charac_uuid,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service long_uuid_srv =
	RTK_BT_GATT_SERVICE(long_uuid_srv_attrs, LONG_UUID_SRV_ID);

void long_uuid_service_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
			BT_LOGA("[APP] Long uuid service register succeed!\r\n");
		} else
			BT_LOGE("[APP] Long uuid service register failed, err: 0x%x\r\n",
					p_gatts_reg_ind->reg_status);
		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};
		read_resp.app_id = p_read_ind->app_id;
		read_resp.conn_handle = p_read_ind->conn_handle;
		read_resp.cid = p_read_ind->cid;
		read_resp.index = p_read_ind->index;
		if (LONG_UUID_READ_INDEX == p_read_ind->index) {
			read_resp.data = long_uuid_srv_read_val;
			read_resp.len = sizeof(long_uuid_srv_read_val);
		} else {
			BT_LOGE("[APP] Long uuid service read event unknown index: %d\r\n",
					p_read_ind->index);
			read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret)
			BT_LOGA("[APP] Long uuid service response for client read succeed, index: %d\r\n",
					p_read_ind->index);
		else {
			BT_LOGE("[APP] Long uuid service response for client read failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
					read_resp.conn_handle, read_resp.index,
					read_resp.err_code);
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
			BT_LOGE("[APP] Long uuid service write value is empty!\r\n");
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
			goto send_write_rsp;
		}

		if (LONG_UUID_WRITE_INDEX == p_write_ind->index) {
			BT_LOGA("[APP] Long uuid service write event, len: %d, type: %d, data:\r\n",
					p_write_ind->len, p_write_ind->type);
			BT_DUMPA("", p_write_ind->value, p_write_ind->len);
			BT_AT_PRINT("+BLEGATTS:write,%u,%u,%u,%u,%u\r\n",
						p_write_ind->app_id, p_write_ind->conn_handle, p_write_ind->index,
						p_write_ind->len, p_write_ind->type);
		} else {
			BT_LOGE("[APP] Long uuid service write event unknown index: %d\r\n",
					p_write_ind->index);
			write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

send_write_rsp:
		ret = rtk_bt_gatts_write_resp(&write_resp);
		if (RTK_BT_OK == ret)
			BT_LOGA("[APP] Long uuid service response for client write succeed, index: %d\r\n",
					p_write_ind->index);
		else {
			BT_LOGE("[APP] Long uuid service response for client write failed, err: 0x%x\r\n", ret);
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

uint16_t long_uuid_srv_add(void)
{
	long_uuid_srv.type = GATT_SERVICE_OVER_BLE;
	long_uuid_srv.server_info = 0;
	long_uuid_srv.user_data = NULL;
	long_uuid_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&long_uuid_srv);
}