/*
*******************************************************************************
* Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>

#include <basic_types.h>
#include <rtk_bt_gatts.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <rtk_service_config.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_device.h>
#include <bt_utils.h>
#include <transfer_module_common.h>


/*****************************Transfer Module Service and Characteristic UUID******************/
#define TRANSFER_MODULE_UUID_SRV                 0xC00C
#define TRANSFER_MODULE_UUID_CHAR_VAL_READ       0xD001
#define TRANSFER_MODULE_UUID_CHAR_VAL_WRITE      0xD002
#define TRANSFER_MODULE_UUID_CHAR_VAL_NOTIFY     0xD003
#define TRANSFER_MODULE_UUID_CHAR_VAL_INDICATE   0xD004

#define RTK_BT_UUID_TRANSFER_MODULE              BT_UUID_DECLARE_16(TRANSFER_MODULE_UUID_SRV)
#define RTK_BT_UUID_TRANSFER_MODULE_VAL_READ     BT_UUID_DECLARE_16(TRANSFER_MODULE_UUID_CHAR_VAL_READ)
#define RTK_BT_UUID_TRANSFER_MODULE_VAL_WRITE    BT_UUID_DECLARE_16(TRANSFER_MODULE_UUID_CHAR_VAL_WRITE)
#define RTK_BT_UUID_TRANSFER_MODULE_VAL_NOTIFY   BT_UUID_DECLARE_16(TRANSFER_MODULE_UUID_CHAR_VAL_NOTIFY)
#define RTK_BT_UUID_TRANSFER_MODULE_VAL_INDICATE BT_UUID_DECLARE_16(TRANSFER_MODULE_UUID_CHAR_VAL_INDICATE)

#define TRANSFER_MODULE_READ_VAL_DEFAULT_LEN     10

#define TRANSFER_MODULE_SERVICE_INDEX            0
#define TRANSFER_MODULE_READ_CHRC_INDEX          1
#define TRANSFER_MODULE_READ_VAL_INDEX           2
#define TRANSFER_MODULE_WRITE_CHRC_INDEX         3
#define TRANSFER_MODULE_WRITE_VAL_INDEX          4
#define TRANSFER_MODULE_NOTIFY_CHRC_INDEX        5
#define TRANSFER_MODULE_NOTIFY_VAL_INDEX         6
#define TRANSFER_MODULE_NOTIFY_CCCD_INDEX        7
#define TRANSFER_MODULE_INDICATE_CHRC_INDEX      8
#define TRANSFER_MODULE_INDICATE_VAL_INDEX       9
#define TRANSFER_MODULE_INDICATE_CCCD_INDEX      10

typedef struct {
	uint16_t service_uuid;
	uint16_t charac_read_uuid;
	uint16_t charac_write_uuid;
	uint16_t charac_notify_uuid;
	uint16_t charac_indicate_uuid;
} transfer_module_uuid_t;

typedef struct {
	uint8_t len;
	uint8_t buf[TRANSFER_MODULE_READ_VAL_MAX_LEN];
} transfer_module_read_val_t;

static transfer_module_uuid_t transfer_module_uuids = {
	.service_uuid         =  TRANSFER_MODULE_UUID_SRV,
	.charac_read_uuid     =  TRANSFER_MODULE_UUID_CHAR_VAL_READ,
	.charac_write_uuid    =  TRANSFER_MODULE_UUID_CHAR_VAL_WRITE,
	.charac_notify_uuid   =  TRANSFER_MODULE_UUID_CHAR_VAL_NOTIFY,
	.charac_indicate_uuid =  TRANSFER_MODULE_UUID_CHAR_VAL_INDICATE,
};

static rtk_bt_gatt_attr_t transfer_module_attrs[] = {
	/* Primary Service: simple BLE */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_TRANSFER_MODULE),

	/* Read Characteristic */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TRANSFER_MODULE_VAL_READ,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Write Characteristic */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TRANSFER_MODULE_VAL_WRITE,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE),

	/* Notify Characteristic */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TRANSFER_MODULE_VAL_NOTIFY,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Indicate Characteristic */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_TRANSFER_MODULE_VAL_INDICATE,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service transfer_module_srv = RTK_BT_GATT_SERVICE(transfer_module_attrs, TRANSFER_MODULE_SRV_ID);

static transfer_module_read_val_t transfer_module_read_val = {
	.len = TRANSFER_MODULE_READ_VAL_DEFAULT_LEN,
	.buf = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xa},
};

static uint8_t transfer_module_cccd_ntf_en_map[10] = {0};
static uint8_t transfer_module_cccd_ind_en_map[10] = {0};

static void transfer_module_server_mtu_exchange_hdl(void *data)
{
	rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;

	if (!p_gatt_mtu_ind) {
		return;
	}

	if (p_gatt_mtu_ind->result == RTK_BT_OK) {
		BT_LOGA("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
				p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
	} else {
		BT_LOGE("[APP] GATTS mtu exchange fail \r\n");
	}
}

static void transfer_module_server_register_service_hdl(void *data)
{
	rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;

	if (!p_gatts_reg_ind || p_gatts_reg_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
		BT_LOGA("[APP] transfer module service register succeed!\r\n");
	} else
		BT_LOGE("[APP] transfer module service register failed, err: 0x%x\r\n",
				p_gatts_reg_ind->reg_status);
}

static void transfer_module_server_read_hdl(void *data)
{
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint16_t offset;
	uint16_t actual_len;
	uint16_t ret;

	if (!p_read_ind || p_read_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	offset = p_read_ind->offset;
	actual_len = transfer_module_read_val.len - p_read_ind->offset;
	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;
	if (TRANSFER_MODULE_READ_VAL_INDEX == p_read_ind->index) {
		read_resp.data = &transfer_module_read_val.buf[offset];
		read_resp.len = actual_len;
	} else {
		BT_LOGE("[APP] Transfer module read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] Transfer module respond for client read success, offset: %d\r\n", offset);
	} else {
		BT_LOGE("[APP] Transfer module respond for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index,
				read_resp.err_code);
}

static void transfer_module_server_write_hdl(void *data)
{
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint16_t ret = RTK_BT_OK;

	if (!p_write_ind || p_write_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (!p_write_ind->len || !p_write_ind->value) {
		BT_LOGA("[APP] Transfer module server write value is empty!\r\n");
		write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		goto send_write_rsp;
	}

	if (TRANSFER_MODULE_WRITE_VAL_INDEX == p_write_ind->index) {
		BT_LOGA("[APP] Transfer module server write event, len: %d, type: %d, data:\r\n",
				p_write_ind->len, p_write_ind->type);
		BT_DUMPA("", p_write_ind->value, p_write_ind->len);
		BT_AT_PRINT("+BLEGATTS:write,%u,%u,%u,%u,%u",
					p_write_ind->app_id, p_write_ind->conn_handle, p_write_ind->index,
					p_write_ind->type, p_write_ind->len);
		BT_AT_DUMP("", p_write_ind->value, p_write_ind->len);
	} else {
		BT_LOGE("[APP] Transfer module server write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

send_write_rsp:
	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] Transfer module server response for client write success!\r\n");
	} else {
		BT_LOGE("[APP] Transfer module server response for client write failed, err: 0x%x\r\n", ret);
	}
	if ((write_resp.type == RTK_BT_GATTS_WRITE_REQ) ||
		(write_resp.type == RTK_BT_GATTS_WRITE_LONG)) {
		BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
					write_resp.conn_handle, write_resp.index,
					write_resp.type, write_resp.err_code);
	}
}

static void transfer_module_server_cccd_hdl(void *data)
{
	uint8_t conn_id;
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;

	if (!p_cccd_ind || p_cccd_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	switch (p_cccd_ind->index) {
	case TRANSFER_MODULE_NOTIFY_CCCD_INDEX:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) {
			transfer_module_cccd_ntf_en_map[conn_id] = 1;
			BT_LOGA("[APP] Transfer module notify cccd, notify bit enable\r\n");
		} else {
			transfer_module_cccd_ntf_en_map[conn_id] = 0;
			BT_LOGA("[APP] Transfer module notify cccd, notify bit disable\r\n");
		}
		BT_AT_PRINT("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
					transfer_module_cccd_ntf_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;
	case TRANSFER_MODULE_INDICATE_CCCD_INDEX:
		if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
			transfer_module_cccd_ind_en_map[conn_id] = 1;
			BT_LOGA("[APP] Transfer module indicate cccd, indicate bit enable\r\n");
		} else {
			transfer_module_cccd_ind_en_map[conn_id] = 0;
			BT_LOGA("[APP] Transfer module indicate cccd, indicate bit disable\r\n");
		}
		BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
					transfer_module_cccd_ind_en_map[conn_id], p_cccd_ind->app_id,
					p_cccd_ind->conn_handle, p_cccd_ind->index);
		break;
	default:
		break;
	}
}

static void transfer_module_server_notify_complete_hdl(void *data)
{
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;

	if (!p_ntf_ind || p_ntf_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	if (RTK_BT_OK == p_ntf_ind->err_code) {
		BT_LOGA("[APP] Transfer module server notify succeed!\r\n");
	} else {
		BT_LOGE("[APP] Transfer module server notify failed, err: 0x%x\r\n", p_ntf_ind->err_code);
	}
	BT_AT_PRINT("+BLEGATTS:notify,%d,%u,%u,%u\r\n",
				(RTK_BT_OK == p_ntf_ind->err_code) ? 0 : -1, p_ntf_ind->app_id,
				p_ntf_ind->conn_handle, p_ntf_ind->index);
}

static void transfer_module_server_indicate_complete_hdl(void *data)
{
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;

	if (!p_ind_ind || p_ind_ind->app_id != TRANSFER_MODULE_SRV_ID) {
		return;
	}

	if (RTK_BT_OK == p_ind_ind->err_code) {
		BT_LOGA("[APP] Transfer module server indicate succeed!\r\n");
	} else {
		BT_LOGE("[APP] Transfer module server indicate failed, err: 0x%x\r\n", p_ind_ind->err_code);
	}
	BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
				(RTK_BT_OK == p_ind_ind->err_code) ? 0 : -1, p_ind_ind->app_id,
				p_ind_ind->conn_handle, p_ind_ind->index);
}

rtk_bt_evt_cb_ret_t ble_transfer_module_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;

	switch (event) {
	case RTK_BT_GATTS_EVT_MTU_EXCHANGE:
		transfer_module_server_mtu_exchange_hdl(data);
		break;
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		transfer_module_server_register_service_hdl(data);
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND: {
		transfer_module_server_read_hdl(data);
		break;
	}
	case RTK_BT_GATTS_EVT_WRITE_IND: {
		transfer_module_server_write_hdl(data);
		break;
	}
	case RTK_BT_GATTS_EVT_CCCD_IND: {
		transfer_module_server_cccd_hdl(data);
		break;
	}
	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		transfer_module_server_notify_complete_hdl(data);
		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		transfer_module_server_indicate_complete_hdl(data);
		break;
	}
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

uint16_t ble_transfer_module_server_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	transfer_module_cccd_ntf_en_map[conn_id] = 0;
	transfer_module_cccd_ind_en_map[conn_id] = 0;

	return RTK_BT_OK;
}

uint16_t ble_transfer_module_server_add(void)
{
	rtk_bt_gatt_attr_t *attrs = transfer_module_srv.attrs;

	/* service uuid */
	attrs[TRANSFER_MODULE_SERVICE_INDEX].user_data = BT_UUID_DECLARE_16(transfer_module_uuids.service_uuid);

	/* charc read uuid */
	((struct rtk_bt_gatt_chrc *)(attrs[TRANSFER_MODULE_READ_CHRC_INDEX].user_data))->uuid
		= BT_UUID_DECLARE_16(transfer_module_uuids.charac_read_uuid);
	attrs[TRANSFER_MODULE_READ_VAL_INDEX].uuid = BT_UUID_DECLARE_16(transfer_module_uuids.charac_read_uuid);

	/* charc write uuid */
	((struct rtk_bt_gatt_chrc *)(attrs[TRANSFER_MODULE_WRITE_CHRC_INDEX].user_data))->uuid
		= BT_UUID_DECLARE_16(transfer_module_uuids.charac_write_uuid);
	attrs[TRANSFER_MODULE_WRITE_VAL_INDEX].uuid = BT_UUID_DECLARE_16(transfer_module_uuids.charac_write_uuid);

	/* charc notify uuid */
	((struct rtk_bt_gatt_chrc *)(attrs[TRANSFER_MODULE_NOTIFY_CHRC_INDEX].user_data))->uuid
		= BT_UUID_DECLARE_16(transfer_module_uuids.charac_notify_uuid);
	attrs[TRANSFER_MODULE_NOTIFY_VAL_INDEX].uuid = BT_UUID_DECLARE_16(transfer_module_uuids.charac_notify_uuid);

	/* charc indicate uuid */
	((struct rtk_bt_gatt_chrc *)(attrs[TRANSFER_MODULE_INDICATE_CHRC_INDEX].user_data))->uuid
		= BT_UUID_DECLARE_16(transfer_module_uuids.charac_indicate_uuid);
	attrs[TRANSFER_MODULE_INDICATE_VAL_INDEX].uuid = BT_UUID_DECLARE_16(transfer_module_uuids.charac_indicate_uuid);

	transfer_module_srv.type = GATT_SERVICE_OVER_BLE;
	transfer_module_srv.server_info = 0;
	transfer_module_srv.user_data = NULL;
	transfer_module_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&transfer_module_srv);
}

uint16_t ble_transfer_module_server_deinit(void)
{
	memset(transfer_module_cccd_ind_en_map, 0, sizeof(transfer_module_cccd_ind_en_map));
	memset(transfer_module_cccd_ntf_en_map, 0, sizeof(transfer_module_cccd_ntf_en_map));

	return RTK_BT_OK;
}

uint16_t ble_transfer_module_get_uuid(uint8_t attr, uint16_t *uuid)
{
	uint16_t ret = RTK_BT_OK;
	if (!uuid) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	switch (attr) {
	case TRANSFER_MODULE_ATTR_SERVICE:
		*uuid = transfer_module_uuids.service_uuid;
		break;
	case TRANSFER_MODULE_ATTR_READ:
		*uuid = transfer_module_uuids.charac_read_uuid;
		break;
	case TRANSFER_MODULE_ATTR_WRITE:
		*uuid = transfer_module_uuids.charac_write_uuid;
		break;
	case TRANSFER_MODULE_ATTR_NOTIFY:
		*uuid = transfer_module_uuids.charac_notify_uuid;
		break;
	case TRANSFER_MODULE_ATTR_INDICATE:
		*uuid = transfer_module_uuids.charac_indicate_uuid;
		break;
	default:
		BT_LOGE("[APP] Invalid transfer module attribute attr=%d\r\n", attr);
		ret = RTK_BT_ERR_PARAM_INVALID;
		break;
	}
	return ret;
}

uint16_t ble_transfer_module_set_uuid(uint8_t attr, uint16_t uuid)
{
	uint16_t ret = RTK_BT_OK;

	if (rtk_bt_is_enable()) {
		BT_LOGE("[APP] Transfer module set uuid fail, please set uuid before bt init\r\n");
		return RTK_BT_FAIL;
	}

	switch (attr) {
	case TRANSFER_MODULE_ATTR_SERVICE:
		transfer_module_uuids.service_uuid = uuid;
		break;
	case TRANSFER_MODULE_ATTR_READ:
		transfer_module_uuids.charac_read_uuid = uuid;
		break;
	case TRANSFER_MODULE_ATTR_WRITE:
		transfer_module_uuids.charac_write_uuid = uuid;
		break;
	case TRANSFER_MODULE_ATTR_NOTIFY:
		transfer_module_uuids.charac_notify_uuid = uuid;
		break;
	case TRANSFER_MODULE_ATTR_INDICATE:
		transfer_module_uuids.charac_indicate_uuid = uuid;
		break;
	default:
		BT_LOGE("[APP] Invalid transfer module attribute attr=%d\r\n", attr);
		ret = RTK_BT_ERR_PARAM_INVALID;
		break;
	}
	return ret;
}

uint16_t ble_transfer_module_update_read_val(uint8_t len, uint8_t *data)
{
	if (!len || !data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (len > TRANSFER_MODULE_READ_VAL_MAX_LEN) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	transfer_module_read_val.len = len;
	memset(transfer_module_read_val.buf, 0, sizeof(transfer_module_read_val.buf));
	memcpy(transfer_module_read_val.buf, data, len);

	return RTK_BT_OK;
}