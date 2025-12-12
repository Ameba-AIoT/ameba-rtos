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
#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#endif


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

#define _U16_TO_LE(_a)                                  \
        {(uint8_t)((_a) & 0xFF), (uint8_t)(((_a) >> 8) & 0xFF)}


typedef struct {
	uint8_t service_uuid_type;
	uint8_t service_uuid[TRANSFER_MODULE_UUID128_LEN]; // 16-bit UUID stores in first two elements of the array in little endian; 128-bit UUID occupies the entire array in little endian
	uint8_t read_uuid_type;
	uint8_t charac_read_uuid[TRANSFER_MODULE_UUID128_LEN]; // 16-bit UUID stores in first two elements of the array in little endian; 128-bit UUID occupies the entire array in little endian
	uint8_t write_uuid_type;
	uint8_t charac_write_uuid[TRANSFER_MODULE_UUID128_LEN]; // 16-bit UUID stores in first two elements of the array in little endian; 128-bit UUID occupies the entire array in little endian
	uint8_t notify_uuid_type;
	uint8_t charac_notify_uuid[TRANSFER_MODULE_UUID128_LEN]; // 16-bit UUID stores in first two elements of the array in little endian; 128-bit UUID occupies the entire array in little endian
	uint8_t indicate_uuid_type;
	uint8_t charac_indicate_uuid[TRANSFER_MODULE_UUID128_LEN]; // 16-bit UUID stores in first two elements of the array in little endian; 128-bit UUID occupies the entire array in little endian
} transfer_module_uuid_t;

typedef struct {
	uint8_t len;
	uint8_t buf[TRANSFER_MODULE_READ_VAL_MAX_LEN];
} transfer_module_read_val_t;

struct transfer_module_char_indexs {
	uint8_t num;
	uint8_t *indexs;
};

struct transfer_module_read_char {
	uint8_t char_index;
	uint8_t len;
	uint8_t buf[TRANSFER_MODULE_READ_VAL_MAX_LEN];
};

struct transfer_module_read_chars {
	uint8_t num;
	struct transfer_module_read_char *read_char;
};

struct transfer_module_cccd_enable {
	uint8_t char_index;
	uint8_t enable_map[RTK_BLE_GAP_MAX_LINKS];
};

struct transfer_module_cccd_enable_maps {
	uint8_t num;
	struct transfer_module_cccd_enable *cccd;
};

struct transfer_module_srv_info {
	struct rtk_bt_gatt_service              *srv;
	struct transfer_module_read_chars       read_value_index;
	struct transfer_module_char_indexs      write_value_index;
	struct transfer_module_cccd_enable_maps notify_en_maps; /* include notify cccd_index, value_index = cccd_index - 1 */
	struct transfer_module_cccd_enable_maps indicate_en_maps; /* include notify cccd_index, value_index = cccd_index - 1 */
};

static transfer_module_uuid_t transfer_module_uuids = {
	.service_uuid_type    =  BT_UUID_TYPE_16,
	.service_uuid         =  _U16_TO_LE(TRANSFER_MODULE_UUID_SRV),
	.read_uuid_type       =  BT_UUID_TYPE_16,
	.charac_read_uuid     =  _U16_TO_LE(TRANSFER_MODULE_UUID_CHAR_VAL_READ),
	.write_uuid_type      =  BT_UUID_TYPE_16,
	.charac_write_uuid    =  _U16_TO_LE(TRANSFER_MODULE_UUID_CHAR_VAL_WRITE),
	.notify_uuid_type     =  BT_UUID_TYPE_16,
	.charac_notify_uuid   =  _U16_TO_LE(TRANSFER_MODULE_UUID_CHAR_VAL_NOTIFY),
	.indicate_uuid_type   =  BT_UUID_TYPE_16,
	.charac_indicate_uuid =  _U16_TO_LE(TRANSFER_MODULE_UUID_CHAR_VAL_INDICATE),
};

static struct rtk_bt_gatt_service *default_srv = NULL;

static transfer_module_read_val_t transfer_module_read_val = {
	.len = TRANSFER_MODULE_READ_VAL_DEFAULT_LEN,
	.buf = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xa},
};

static uint8_t transfer_module_cccd_ntf_en_map[10] = {0};
static uint8_t transfer_module_cccd_ind_en_map[10] = {0};

static struct transfer_module_srv_info *user_defined_srvs[TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM] = {0};

static uint16_t transfer_module_primary_service_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid);
static uint16_t transfer_module_char_value_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid, uint8_t perm);
static uint16_t transfer_module_char_decl_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid, uint16_t props);
static uint16_t transfer_module_cccd_init(rtk_bt_gatt_attr_t *attr, uint8_t perm);

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

	if (!p_gatts_reg_ind) {
		return;
	}

	if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
		BT_LOGA("[APP] transfer module service(0x%x) register succeed!\r\n", p_gatts_reg_ind->app_id);
	} else
		BT_LOGE("[APP] transfer module service(0x%x) register failed, err: 0x%x\r\n",
				p_gatts_reg_ind->app_id, p_gatts_reg_ind->reg_status);
}

static void transfer_module_server_read_hdl(void *data)
{
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint16_t offset;
	uint16_t ret;
	int srv_idx;
	uint8_t found = FALSE;

	if (!p_read_ind) {
		return;
	}

	offset = p_read_ind->offset;
	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	// default service
	if (p_read_ind->app_id == TRANSFER_MODULE_SRV_ID) {
		if (TRANSFER_MODULE_READ_VAL_INDEX == p_read_ind->index) {
			read_resp.data = &transfer_module_read_val.buf[offset];
			read_resp.len = transfer_module_read_val.len - p_read_ind->offset;
			found = TRUE;
		}
	} else { // user defined services
		srv_idx = p_read_ind->app_id - TRANSFER_MODULE_SRV_ID_BASE;
		if (srv_idx >= 0 && srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM) {
			if (user_defined_srvs[srv_idx]) {
				struct transfer_module_read_char *read_char = user_defined_srvs[srv_idx]->read_value_index.read_char;
				if (read_char) {
					for (int i = 0; i < user_defined_srvs[srv_idx]->read_value_index.num; ++i) {
						if (read_char[i].char_index == p_read_ind->index) {
							read_resp.data = &read_char[i].buf[offset];
							read_resp.len = read_char[i].len - p_read_ind->offset;
							found = TRUE;
							break;
						}
					}
				}
			}
		}
	}

	if (!found) {
		BT_LOGE("[APP] Transfer module read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] Transfer module respond for client read success, offset: %d\r\n", offset);
	} else {
		BT_LOGE("[APP] Transfer module respond for client read failed, err: 0x%x\r\n", ret);
	}

	if ((!read_resp.err_code) && (RTK_BT_OK == ret)) {
		BT_AT_PRINT_INDICATE("+BLEGATTS:read,%u,%u,%u\r\n",
							 read_resp.app_id, read_resp.conn_handle, read_resp.index);
	}
}

static uint16_t transfer_module_write_index_check(uint16_t app_id, uint16_t index)
{
	uint16_t check = FALSE;
	int srv_idx = app_id - TRANSFER_MODULE_SRV_ID_BASE;

	// default service
	if (app_id == TRANSFER_MODULE_SRV_ID) {
		if (TRANSFER_MODULE_WRITE_VAL_INDEX == index) {
			check = TRUE;
		}
		return check;
	}

	// user defined services
	if (srv_idx >= 0 && srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM) {
		if (user_defined_srvs[srv_idx]) {
			for (int i = 0; i < user_defined_srvs[srv_idx]->write_value_index.num; ++i) {
				uint8_t *indexs = user_defined_srvs[srv_idx]->write_value_index.indexs;
				if (indexs) {
					if (indexs[i] == index) {
						check = TRUE;
						break;
					}
				}
			}
		}
	}

	return check;
}

static void transfer_module_server_write_hdl(void *data)
{
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint16_t ret = RTK_BT_OK;

	if (!p_write_ind) {
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

	if (transfer_module_write_index_check(p_write_ind->app_id, p_write_ind->index)) {
		BT_LOGA("[APP] Transfer module server write event, app_idd:%d, char_index: %d, len: %d, type: %d, data:\r\n",
				p_write_ind->app_id, p_write_ind->index, p_write_ind->len, p_write_ind->type);
		BT_DUMPA("", p_write_ind->value, p_write_ind->len);
		BT_AT_PRINT_INDICATE("+BLEGATTS:write,%u,%u,%u,%u,%u",
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
}

static void transfer_module_default_srv_cccd_hdl(uint16_t conn_handle, uint16_t app_id, uint16_t char_index, uint16_t value)
{
	uint8_t conn_id;
#if !defined(CONFIG_ATCMD_HOST_CONTROL) || !CONFIG_ATCMD_HOST_CONTROL
	(void)app_id;
#endif
	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	switch (char_index) {
	case TRANSFER_MODULE_NOTIFY_CCCD_INDEX:
		if (value & RTK_BT_GATT_CCC_NOTIFY) {
			transfer_module_cccd_ntf_en_map[conn_id] = 1;
			BT_LOGA("[APP] Transfer module notify cccd, notify bit enable\r\n");
		} else {
			transfer_module_cccd_ntf_en_map[conn_id] = 0;
			BT_LOGA("[APP] Transfer module notify cccd, notify bit disable\r\n");
		}
		BT_AT_PRINT_INDICATE("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
							 transfer_module_cccd_ntf_en_map[conn_id], app_id,
							 conn_handle, char_index);
		break;
	case TRANSFER_MODULE_INDICATE_CCCD_INDEX:
		if (value & RTK_BT_GATT_CCC_INDICATE) {
			transfer_module_cccd_ind_en_map[conn_id] = 1;
			BT_LOGA("[APP] Transfer module indicate cccd, indicate bit enable\r\n");
		} else {
			transfer_module_cccd_ind_en_map[conn_id] = 0;
			BT_LOGA("[APP] Transfer module indicate cccd, indicate bit disable\r\n");
		}
		BT_AT_PRINT_INDICATE("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
							 transfer_module_cccd_ind_en_map[conn_id], app_id,
							 conn_handle, char_index);
		break;
	default:
		break;
	}
}

static void transfer_module_user_defined_srv_cccd_hdl(uint16_t conn_handle, uint16_t app_id, uint16_t char_index, uint16_t value)
{
	uint8_t conn_id;
	int srv_idx = app_id - TRANSFER_MODULE_SRV_ID_BASE;

	if ((srv_idx < 0) || (srv_idx >= TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM)) {
		BT_LOGE("%s: invalid app_id\r\n", __func__);
		return;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		BT_LOGE("%s: get conn_id fail\r\n", __func__);
		return;
	}

	if (!user_defined_srvs[srv_idx]) {
		BT_LOGE("user defined srv info invalid\r\n");
		return;
	}

	if (!user_defined_srvs[srv_idx]->srv) {
		BT_LOGE("user defined srv invalid\r\n");
		return;
	}

	if (user_defined_srvs[srv_idx]->srv->app_id != app_id) {
		BT_LOGE("user defined srv and app_id unmatch\r\n");
		return;
	}

	if (user_defined_srvs[srv_idx]->notify_en_maps.cccd) {
		struct transfer_module_cccd_enable *cccd = user_defined_srvs[srv_idx]->notify_en_maps.cccd;
		for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->notify_en_maps.num; ++cccd_idx) {
			if (cccd[cccd_idx].char_index == char_index) {
				if (value & RTK_BT_GATT_CCC_NOTIFY) {
					cccd[cccd_idx].enable_map[conn_id] = 1;
					BT_LOGA("[APP] Transfer module notify cccd, notify bit enable, app_id=%d, char_index=%d conn_handle=%d\r\n",
							app_id, char_index, conn_handle);
				} else {
					cccd[cccd_idx].enable_map[conn_id] = 0;
					BT_LOGA("[APP] Transfer module notify cccd, notify bit disable, app_id=%d, char_index=%d conn_handle=%d\r\n",
							app_id, char_index, conn_handle);
				}
				BT_AT_PRINT_INDICATE("+BLEGATTS:cccd,notify,%d,%u,%u,%u\r\n",
									 cccd[cccd_idx].enable_map[conn_id], app_id,
									 conn_handle, char_index);
				break;
			}
		}
	}

	if (user_defined_srvs[srv_idx]->indicate_en_maps.cccd) {
		struct transfer_module_cccd_enable *cccd = user_defined_srvs[srv_idx]->indicate_en_maps.cccd;
		for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->indicate_en_maps.num; ++cccd_idx) {
			if (cccd[cccd_idx].char_index == char_index) {
				if (value & RTK_BT_GATT_CCC_INDICATE) {
					cccd[cccd_idx].enable_map[conn_id] = 1;
					BT_LOGA("[APP] Transfer module indicate cccd, indicate bit enable, app_id=%d, char_index=%d conn_handle=%d\r\n",
							app_id, char_index, conn_handle);
				} else {
					cccd[cccd_idx].enable_map[conn_id] = 0;
					BT_LOGA("[APP] Transfer module indicate cccd, indicate bit disable, app_id=%d, char_index=%d conn_handle=%d\r\n",
							app_id, char_index, conn_handle);
				}
				BT_AT_PRINT_INDICATE("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
									 cccd[cccd_idx].enable_map[conn_id], app_id,
									 conn_handle, char_index);
				break;
			}
		}
	}

}

static void transfer_module_server_cccd_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;

	if (!p_cccd_ind) {
		BT_LOGE("%s: parameter invalid\r\n", __func__);
		return;
	}

	if (p_cccd_ind->app_id == TRANSFER_MODULE_SRV_ID) {
		transfer_module_default_srv_cccd_hdl(p_cccd_ind->conn_handle, p_cccd_ind->app_id, p_cccd_ind->index, p_cccd_ind->value);
	} else {
		transfer_module_user_defined_srv_cccd_hdl(p_cccd_ind->conn_handle, p_cccd_ind->app_id, p_cccd_ind->index, p_cccd_ind->value);
	}
}

static void transfer_module_server_notify_complete_hdl(void *data)
{
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;

	if (!p_ntf_ind) {
		return;
	}

	if (RTK_BT_OK == p_ntf_ind->err_code) {
		BT_LOGA("[APP] Transfer module server notify succeed!\r\n");
	} else {
		BT_LOGE("[APP] Transfer module server notify failed, err: 0x%x\r\n", p_ntf_ind->err_code);
	}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
	if (bt_at_sync_event_match_check(RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND)) {
		bt_at_sync_set_result(p_ntf_ind->err_code);
		bt_at_sync_sem_give();
	}
#endif
}

static void transfer_module_server_indicate_complete_hdl(void *data)
{
	rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;

	if (!p_ind_ind) {
		return;
	}

	if (RTK_BT_OK == p_ind_ind->err_code) {
		BT_LOGA("[APP] Transfer module server indicate succeed!\r\n");
	} else {
		BT_LOGE("[APP] Transfer module server indicate failed, err: 0x%x\r\n", p_ind_ind->err_code);
	}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
	if (bt_at_sync_event_match_check(RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND)) {
		bt_at_sync_set_result(p_ind_ind->err_code);
		bt_at_sync_sem_give();
	}
#endif
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

	if (conn_id >= RTK_BLE_GAP_MAX_LINKS) {
		return RTK_BT_FAIL;
	}

	transfer_module_cccd_ntf_en_map[conn_id] = 0;
	transfer_module_cccd_ind_en_map[conn_id] = 0;

	for (int srv_idx = 0; srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++srv_idx) {
		if (user_defined_srvs[srv_idx]) {
			if (user_defined_srvs[srv_idx]->notify_en_maps.cccd) {
				for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->notify_en_maps.num; ++cccd_idx) {
					user_defined_srvs[srv_idx]->notify_en_maps.cccd[cccd_idx].enable_map[conn_id] = 0;
				}
			}

			if (user_defined_srvs[srv_idx]->indicate_en_maps.cccd) {
				for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->indicate_en_maps.num; ++cccd_idx) {
					user_defined_srvs[srv_idx]->indicate_en_maps.cccd[cccd_idx].enable_map[conn_id] = 0;
				}
			}
		}
	}

	return RTK_BT_OK;
}

void ble_tranfer_module_default_srv_free(void)
{
	if (default_srv) {
		if (default_srv->attrs) {
			for (int attr_idx = 0; attr_idx < default_srv->attr_count; ++attr_idx) {
				rtk_bt_gatt_attr_t *attr = &default_srv->attrs[attr_idx];
				if (attr->user_data) {

					if (BT_UUID_16(attr->uuid)->val == BT_UUID_GATT_CHRC_VAL) {
						struct bt_uuid *uuid = (struct bt_uuid *)(((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid);
						if (uuid) {
							osif_mem_free(uuid);
						}
					}
					osif_mem_free(attr->user_data);
					attr->user_data = NULL;
				}

				if (attr->uuid) {
					osif_mem_free((void *)(attr->uuid));
					attr->uuid = NULL;
				}

			}
			osif_mem_free(default_srv->attrs);
			default_srv->attrs = NULL;
		}

		osif_mem_free(default_srv);
		default_srv = NULL;
	}
}

static uint16_t ble_transfer_module_default_srv_init(void)
{
	uint16_t ret = RTK_BT_OK;

	default_srv = (struct rtk_bt_gatt_service *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct rtk_bt_gatt_service));
	if (!default_srv) {
		BT_LOGE("[APP] alloc user defined srv fail\r\n");
		ret = RTK_BT_ERR_NO_MEMORY;
		goto end;
	}
	memset(default_srv, 0, sizeof(struct rtk_bt_gatt_service));

	default_srv->type = GATT_SERVICE_OVER_BLE;
	default_srv->server_info = 0;
	default_srv->user_data = NULL;
	default_srv->register_status = 0;

	default_srv->app_id = TRANSFER_MODULE_SRV_ID;
	default_srv->attr_count = TRANSFER_MODULE_INDEX_NUM;
	default_srv->attrs = (rtk_bt_gatt_attr_t *)osif_mem_alloc(
							 RAM_TYPE_DATA_ON,
							 default_srv->attr_count * sizeof(rtk_bt_gatt_attr_t));
	if (!default_srv->attrs) {
		BT_LOGE("[APP] alloc user defined srv attrs fail\r\n");
		ret = RTK_BT_ERR_NO_MEMORY;
		goto end;
	}

	ret = transfer_module_primary_service_init(&default_srv->attrs[TRANSFER_MODULE_SERVICE_INDEX],
											   transfer_module_uuids.service_uuid_type, transfer_module_uuids.service_uuid);
	if (ret != RTK_BT_OK) {
		goto end;
	}
	/* Read Characteristic */
	ret = transfer_module_char_decl_init(&default_srv->attrs[TRANSFER_MODULE_READ_CHRC_INDEX], transfer_module_uuids.read_uuid_type,
										 transfer_module_uuids.charac_read_uuid, RTK_BT_GATT_CHRC_READ);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_char_value_init(&default_srv->attrs[TRANSFER_MODULE_READ_VAL_INDEX], transfer_module_uuids.read_uuid_type,
										  transfer_module_uuids.charac_read_uuid, RTK_BT_GATT_PERM_READ);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	/* Write Characteristic */
	ret = transfer_module_char_decl_init(&default_srv->attrs[TRANSFER_MODULE_WRITE_CHRC_INDEX], transfer_module_uuids.write_uuid_type,
										 transfer_module_uuids.charac_write_uuid, RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_char_value_init(&default_srv->attrs[TRANSFER_MODULE_WRITE_VAL_INDEX], transfer_module_uuids.write_uuid_type,
										  transfer_module_uuids.charac_write_uuid, RTK_BT_GATT_PERM_WRITE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	/* Notify Characteristic */
	ret = transfer_module_char_decl_init(&default_srv->attrs[TRANSFER_MODULE_NOTIFY_CHRC_INDEX], transfer_module_uuids.notify_uuid_type,
										 transfer_module_uuids.charac_notify_uuid, RTK_BT_GATT_CHRC_NOTIFY);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_char_value_init(&default_srv->attrs[TRANSFER_MODULE_NOTIFY_VAL_INDEX], transfer_module_uuids.notify_uuid_type,
										  transfer_module_uuids.charac_notify_uuid, RTK_BT_GATT_PERM_NONE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_cccd_init(&default_srv->attrs[TRANSFER_MODULE_NOTIFY_CCCD_INDEX], RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	/* Indicate Characteristic */
	ret = transfer_module_char_decl_init(&default_srv->attrs[TRANSFER_MODULE_INDICATE_CHRC_INDEX], transfer_module_uuids.indicate_uuid_type,
										 transfer_module_uuids.charac_indicate_uuid, RTK_BT_GATT_CHRC_INDICATE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_char_value_init(&default_srv->attrs[TRANSFER_MODULE_INDICATE_VAL_INDEX], transfer_module_uuids.indicate_uuid_type,
										  transfer_module_uuids.charac_indicate_uuid, RTK_BT_GATT_PERM_NONE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_cccd_init(&default_srv->attrs[TRANSFER_MODULE_INDICATE_CCCD_INDEX], RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE);
	if (ret != RTK_BT_OK) {
		goto end;
	}

end:
	if (ret != RTK_BT_OK) {
		ble_tranfer_module_default_srv_free();
	}

	return ret;
}

static uint16_t ble_transfer_module_register_default_srv(void)
{
	uint16_t ret = ble_transfer_module_default_srv_init();
	if (ret != RTK_BT_OK) {
		BT_LOGE("[%s] ble_transfer_module_default_srv_init fail\r\n", __func__);
		return ret;
	}

	return rtk_bt_gatts_register_service(default_srv);
}

static uint16_t ble_transfer_module_register_default_user_defined_srvs(void)
{
	uint16_t ret;

	for (int i = 0; i < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++i) {
		if (user_defined_srvs[i] && user_defined_srvs[i]->srv) {
			struct rtk_bt_gatt_service *srv = user_defined_srvs[i]->srv;
			srv->type = GATT_SERVICE_OVER_BLE;
			srv->server_info = 0;
			srv->user_data = NULL;
			srv->register_status = 0;

			ret = rtk_bt_gatts_register_service(srv);
			if (ret != RTK_BT_OK) {
				BT_LOGE("[APP] Transfer module register user defined service fail, srv_idx(%d) ret=%d\r\n", i, ret);
				break;
			}
		}
	}

	return ret;
}

uint16_t ble_transfer_module_server_add(void)
{
	uint8_t user_defined = FALSE;
	uint16_t ret;

	for (int i = 0; i < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++i) {
		if (user_defined_srvs[i]) {
			user_defined = TRUE;
			break;
		}
	}

	if (user_defined == FALSE) {
		ret = ble_transfer_module_register_default_srv();
	} else {
		ret = ble_transfer_module_register_default_user_defined_srvs();
	}

	return ret;
}

uint16_t ble_transfer_module_server_deinit(void)
{
	struct transfer_module_cccd_enable *cccd = NULL;

	memset(transfer_module_cccd_ind_en_map, 0, sizeof(transfer_module_cccd_ind_en_map));
	memset(transfer_module_cccd_ntf_en_map, 0, sizeof(transfer_module_cccd_ntf_en_map));

	for (int srv_idx = 0; srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++srv_idx) {
		if (user_defined_srvs[srv_idx]) {
			if (user_defined_srvs[srv_idx]->notify_en_maps.cccd) {
				cccd = user_defined_srvs[srv_idx]->notify_en_maps.cccd;
				for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->notify_en_maps.num; ++cccd_idx) {
					memset(cccd[cccd_idx].enable_map, 0, sizeof(cccd[cccd_idx].enable_map));
				}

			}
			if (user_defined_srvs[srv_idx]->indicate_en_maps.cccd) {
				cccd = user_defined_srvs[srv_idx]->indicate_en_maps.cccd;
				for (int cccd_idx = 0; cccd_idx < user_defined_srvs[srv_idx]->indicate_en_maps.num; ++cccd_idx) {
					memset(cccd[cccd_idx].enable_map, 0, sizeof(cccd[cccd_idx].enable_map));
				}
			}
		}
	}

	// free default_srv
	ble_tranfer_module_default_srv_free();

	return RTK_BT_OK;
}

uint16_t ble_transfer_module_get_uuid(uint8_t attr, uint8_t *uuid_type, uint8_t *uuid)
{
	uint16_t ret = RTK_BT_OK;
	if (!uuid_type || !uuid) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	switch (attr) {
	case TRANSFER_MODULE_ATTR_SERVICE:
		*uuid_type = transfer_module_uuids.service_uuid_type;
		memcpy(uuid, transfer_module_uuids.service_uuid, sizeof(transfer_module_uuids.service_uuid));
		break;
	case TRANSFER_MODULE_ATTR_READ:
		*uuid_type = transfer_module_uuids.read_uuid_type;
		memcpy(uuid, transfer_module_uuids.charac_read_uuid, sizeof(transfer_module_uuids.charac_read_uuid));
		break;
	case TRANSFER_MODULE_ATTR_WRITE:
		*uuid_type = transfer_module_uuids.write_uuid_type;
		memcpy(uuid, transfer_module_uuids.charac_write_uuid, sizeof(transfer_module_uuids.charac_write_uuid));
		break;
	case TRANSFER_MODULE_ATTR_NOTIFY:
		*uuid_type = transfer_module_uuids.notify_uuid_type;
		memcpy(uuid, transfer_module_uuids.charac_notify_uuid, sizeof(transfer_module_uuids.charac_notify_uuid));
		break;
	case TRANSFER_MODULE_ATTR_INDICATE:
		*uuid_type = transfer_module_uuids.indicate_uuid_type;
		memcpy(uuid, transfer_module_uuids.charac_indicate_uuid, sizeof(transfer_module_uuids.charac_indicate_uuid));
		break;
	default:
		BT_LOGE("[APP] Invalid transfer module attribute attr=%d\r\n", attr);
		ret = RTK_BT_ERR_PARAM_INVALID;
		break;
	}
	return ret;
}

uint16_t ble_transfer_module_set_uuid(uint8_t attr, uint8_t uuid_type, uint8_t *uuid)
{
	uint16_t ret = RTK_BT_OK;

	if (!uuid) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}
	if (rtk_bt_is_enable()) {
		BT_LOGE("[APP] Transfer module set uuid fail, please set uuid before bt init\r\n");
		return RTK_BT_FAIL;
	}

	switch (attr) {
	case TRANSFER_MODULE_ATTR_SERVICE:
		transfer_module_uuids.service_uuid_type = uuid_type;
		memcpy(transfer_module_uuids.service_uuid, uuid, sizeof(transfer_module_uuids.service_uuid));
		break;
	case TRANSFER_MODULE_ATTR_READ:
		transfer_module_uuids.read_uuid_type = uuid_type;
		memcpy(transfer_module_uuids.charac_read_uuid, uuid, sizeof(transfer_module_uuids.charac_read_uuid));
		break;
	case TRANSFER_MODULE_ATTR_WRITE:
		transfer_module_uuids.write_uuid_type = uuid_type;
		memcpy(transfer_module_uuids.charac_write_uuid, uuid, sizeof(transfer_module_uuids.charac_write_uuid));
		break;
	case TRANSFER_MODULE_ATTR_NOTIFY:
		transfer_module_uuids.notify_uuid_type = uuid_type;
		memcpy(transfer_module_uuids.charac_notify_uuid, uuid, sizeof(transfer_module_uuids.charac_notify_uuid));
		break;
	case TRANSFER_MODULE_ATTR_INDICATE:
		transfer_module_uuids.indicate_uuid_type = uuid_type;
		memcpy(transfer_module_uuids.charac_indicate_uuid, uuid, sizeof(transfer_module_uuids.charac_indicate_uuid));
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
static void ble_transfer_module_del_one_srv(uint8_t srv_idx)
{
	if (!user_defined_srvs[srv_idx]) {
		return;
	}

	if (user_defined_srvs[srv_idx]->srv) {
		struct rtk_bt_gatt_service *srv = user_defined_srvs[srv_idx]->srv;
		if (srv->attrs) {
			for (int attr_idx = 0; attr_idx < srv->attr_count; ++attr_idx) {
				rtk_bt_gatt_attr_t *attr = &srv->attrs[attr_idx];
				if (attr->user_data) {

					if (BT_UUID_16(attr->uuid)->val == BT_UUID_GATT_CHRC_VAL) {
						struct bt_uuid *uuid = (struct bt_uuid *)(((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid);
						if (uuid) {
							osif_mem_free(uuid);
						}
					}
					osif_mem_free(attr->user_data);
					attr->user_data = NULL;
				}

				if (attr->uuid) {
					osif_mem_free((void *)(attr->uuid));
					attr->uuid = NULL;
				}

			}
			osif_mem_free(srv->attrs);
			srv->attrs = NULL;
		}

		osif_mem_free(user_defined_srvs[srv_idx]->srv);
		user_defined_srvs[srv_idx]->srv = NULL;
	}

	if (user_defined_srvs[srv_idx]->read_value_index.read_char) {
		osif_mem_free(user_defined_srvs[srv_idx]->read_value_index.read_char);
		user_defined_srvs[srv_idx]->read_value_index.read_char = NULL;
	}

	if (user_defined_srvs[srv_idx]->write_value_index.indexs) {
		osif_mem_free(user_defined_srvs[srv_idx]->write_value_index.indexs);
		user_defined_srvs[srv_idx]->write_value_index.indexs = NULL;
	}

	if (user_defined_srvs[srv_idx]->notify_en_maps.cccd) {
		osif_mem_free(user_defined_srvs[srv_idx]->notify_en_maps.cccd);
		user_defined_srvs[srv_idx]->notify_en_maps.cccd = NULL;
	}

	if (user_defined_srvs[srv_idx]->indicate_en_maps.cccd) {
		osif_mem_free(user_defined_srvs[srv_idx]->indicate_en_maps.cccd);
		user_defined_srvs[srv_idx]->indicate_en_maps.cccd = NULL;
	}

	osif_mem_free(user_defined_srvs[srv_idx]);
	user_defined_srvs[srv_idx] = NULL;
}

void ble_transfer_module_srv_del(void)
{
	for (uint16_t srv_idx = 0; srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++srv_idx) {
		ble_transfer_module_del_one_srv(srv_idx);
	}
}

static uint16_t transfer_module_primary_service_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid)
{
	if (!attr || !uuid || (uuid_type != BT_UUID_TYPE_16 && uuid_type != BT_UUID_TYPE_128)) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	attr->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
	if (!attr->uuid) {
		BT_LOGE("[APP] alloc primary service attr uuid fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}
	if (uuid_type == BT_UUID_TYPE_16) {
		attr->user_data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
	} else if (uuid_type == BT_UUID_TYPE_128) {
		attr->user_data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_128));
	}
	if (!attr->user_data) {
		BT_LOGE("[APP] alloc primary service attr user_data fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy((void *)attr->uuid, BT_UUID_GATT_PRIMARY, sizeof(struct bt_uuid_16));
	attr->perm = RTK_BT_GATT_PERM_READ;
	if (uuid_type == BT_UUID_TYPE_16) {
		uint16_t uuid16 = LE_TO_U16(uuid);
		memcpy(attr->user_data, BT_UUID_DECLARE_16(uuid16), sizeof(struct bt_uuid_16));
	} else if (uuid_type == BT_UUID_TYPE_128) {
		((struct bt_uuid_128 *)(attr->user_data))->uuid.type = BT_UUID_TYPE_128;
		memcpy(((struct bt_uuid_128 *)(attr->user_data))->val, uuid, TRANSFER_MODULE_UUID128_LEN);
	}
	attr->len = 0;
	attr->flag = RTK_BT_GATT_INTERNAL;

	return RTK_BT_OK;
}

static uint16_t transfer_module_char_decl_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid, uint16_t props)
{
	struct bt_uuid *char_uuid = NULL;
	if (!attr || !uuid || (uuid_type != BT_UUID_TYPE_16 && uuid_type != BT_UUID_TYPE_128)) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	attr->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
	if (!attr->uuid) {
		BT_LOGE("[APP] alloc characteristic declaration attr uuid fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	attr->user_data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct rtk_bt_gatt_chrc));
	if (!attr->user_data) {
		BT_LOGE("[APP] alloc characteristic declaration attr user_data fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy((void *)attr->uuid, BT_UUID_GATT_CHRC, sizeof(struct bt_uuid_16));
	attr->perm = RTK_BT_GATT_PERM_READ;
	memcpy(attr->user_data,
	(struct rtk_bt_gatt_chrc[]) {
		RTK_BT_GATT_CHRC_INIT(NULL, 0U, props)
	},
	sizeof(struct rtk_bt_gatt_chrc));

	if (uuid_type == BT_UUID_TYPE_16) {
		uint16_t uuid16 = LE_TO_U16(uuid);
		((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
		char_uuid = (struct bt_uuid *)(((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid);
		if (!char_uuid) {
			BT_LOGE("[APP] alloc characteristic uuid fail\r\n");
			return RTK_BT_ERR_NO_MEMORY;
		}
		memcpy(char_uuid,  BT_UUID_DECLARE_16(uuid16), sizeof(struct bt_uuid_16));
	} else if (uuid_type == BT_UUID_TYPE_128) {
		((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_128));
		char_uuid = (struct bt_uuid *)(((struct rtk_bt_gatt_chrc *)(attr->user_data))->uuid);
		if (!char_uuid) {
			BT_LOGE("[APP] alloc characteristic uuid fail\r\n");
			return RTK_BT_ERR_NO_MEMORY;
		}
		((struct bt_uuid_128 *)char_uuid)->uuid.type = BT_UUID_TYPE_128;
		memcpy(((struct bt_uuid_128 *)char_uuid)->val, uuid, TRANSFER_MODULE_UUID128_LEN);
	}

	attr->len = 0;
	attr->flag = RTK_BT_GATT_INTERNAL;

	return RTK_BT_OK;
}

static uint16_t transfer_module_char_value_init(rtk_bt_gatt_attr_t *attr, uint8_t uuid_type, uint8_t *uuid, uint8_t perm)
{
	if (!attr || !uuid || (uuid_type != BT_UUID_TYPE_16 && uuid_type != BT_UUID_TYPE_128)) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (uuid_type == BT_UUID_TYPE_16) {
		attr->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
	} else if (uuid_type == BT_UUID_TYPE_128) {
		attr->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_128));
	}
	if (!attr->uuid) {
		BT_LOGE("[APP] alloc characteristic value attr uuid fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	if (uuid_type == BT_UUID_TYPE_16) {
		uint16_t uuid16 = LE_TO_U16(uuid);
		memcpy((void *)attr->uuid, BT_UUID_DECLARE_16(uuid16), sizeof(struct bt_uuid_16));
	} else if (uuid_type == BT_UUID_TYPE_128) {
		((struct bt_uuid_128 *)(attr->uuid))->uuid.type = BT_UUID_TYPE_128;
		memcpy(((struct bt_uuid_128 *)(attr->uuid))->val, uuid, TRANSFER_MODULE_UUID128_LEN);
	}
	attr->perm = perm;
	attr->user_data = NULL;
	attr->len = 0;
	attr->flag = RTK_BT_GATT_APP;

	return RTK_BT_OK;
}

static uint16_t transfer_module_cccd_init(rtk_bt_gatt_attr_t *attr, uint8_t perm)
{
	if (!attr) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	attr->uuid = (struct bt_uuid *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct bt_uuid_16));
	if (!attr->uuid) {
		BT_LOGE("[APP] alloc ccc attr uuid fail\r\n");
		return RTK_BT_ERR_NO_MEMORY;
	}

	memcpy((void *)attr->uuid, BT_UUID_GATT_CCC, sizeof(struct bt_uuid_16));
	attr->perm = perm;
	attr->user_data = NULL;
	attr->len = 0;
	attr->flag = RTK_BT_GATT_INTERNAL;

	return RTK_BT_OK;
}

static uint16_t transfer_module_char_index_init(struct transfer_module_srv_info *srv_info,
												int char_num,
												struct transfer_module_char_t *chars)
{
	uint8_t read_char_num = 0, write_char_num = 0;
	uint8_t notify_char_num = 0, indicate_char_num = 0;
	uint8_t read_value_index[TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM] = {0};
	uint8_t write_value_index[TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM] = {0};
	uint8_t notify_cccd_index[TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM] = {0};
	uint8_t indicate_cccd_index[TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM] = {0};

	if (!srv_info || !chars || !(srv_info->srv)) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	BT_AT_PRINT("+BTDEMO:transfer_module,srv_end,0,%d\r\n", srv_info->srv->app_id);

	for (int char_idx = 0, attr_idx = 2; char_idx < char_num; ++char_idx) {
		uint16_t props = chars[char_idx].char_property;
		if (props & RTK_BT_GATT_CHRC_READ) {
			read_value_index[read_char_num++] = attr_idx;
		}
		if (props & (RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP)) {
			write_value_index[write_char_num++] = attr_idx;
		}
		if (props & RTK_BT_GATT_CHRC_INDICATE) {
			indicate_cccd_index[indicate_char_num++] = attr_idx + 1;
		}
		if (props & RTK_BT_GATT_CHRC_NOTIFY) {
			notify_cccd_index[notify_char_num++] = attr_idx + 1;
		}

		BT_AT_PRINT("+BTDEMO:transfer_module,srv_end,1,%d", attr_idx);
		if (props & (RTK_BT_GATT_CHRC_INDICATE | RTK_BT_GATT_CHRC_NOTIFY)) {
			BT_AT_PRINT(",%d", attr_idx + 1);
			attr_idx += 3;
		} else {
			attr_idx += 2;
		}
		BT_AT_PRINT("\r\n");
	}

	if (read_char_num > 0) {
		srv_info->read_value_index.num = read_char_num;
		srv_info->read_value_index.read_char = (struct transfer_module_read_char *)osif_mem_alloc(
												   RAM_TYPE_DATA_ON,
												   sizeof(struct transfer_module_read_char) * srv_info->read_value_index.num);
		if (!srv_info->read_value_index.read_char) {
			return RTK_BT_ERR_NO_MEMORY;
		}

		memset(srv_info->read_value_index.read_char, 0,
			   sizeof(sizeof(struct transfer_module_read_char) * srv_info->read_value_index.num));
		for (int i = 0; i < srv_info->read_value_index.num; ++i) {
			srv_info->read_value_index.read_char[i].char_index = read_value_index[i];
			srv_info->read_value_index.read_char[i].len = 24;
			sprintf((char *)(srv_info->read_value_index.read_char[i].buf), "RTK_TRANSFER_MODULE_0x%02x", srv_info->srv->app_id);
		}
	}

	if (write_char_num > 0) {
		srv_info->write_value_index.num = write_char_num;
		srv_info->write_value_index.indexs = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, srv_info->write_value_index.num * sizeof(uint8_t));
		if (!srv_info->write_value_index.indexs) {
			return RTK_BT_ERR_NO_MEMORY;
		}

		memcpy(srv_info->write_value_index.indexs, write_value_index, srv_info->write_value_index.num * sizeof(uint8_t));
	}

	if (notify_char_num > 0) {
		srv_info->notify_en_maps.num = notify_char_num;
		srv_info->notify_en_maps.cccd = (struct transfer_module_cccd_enable *)osif_mem_alloc(
											RAM_TYPE_DATA_ON, srv_info->notify_en_maps.num * sizeof(struct transfer_module_cccd_enable));
		if (!srv_info->notify_en_maps.cccd) {
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(srv_info->notify_en_maps.cccd, 0, srv_info->notify_en_maps.num * sizeof(struct transfer_module_cccd_enable));
		for (int i = 0; i < notify_char_num; ++i) {
			srv_info->notify_en_maps.cccd[i].char_index = notify_cccd_index[i];
		}
	}

	if (indicate_char_num > 0) {
		srv_info->indicate_en_maps.num = indicate_char_num;
		srv_info->indicate_en_maps.cccd = (struct transfer_module_cccd_enable *)osif_mem_alloc(
											  RAM_TYPE_DATA_ON, srv_info->indicate_en_maps.num * sizeof(struct transfer_module_cccd_enable));
		if (!srv_info->indicate_en_maps.cccd) {
			return RTK_BT_ERR_NO_MEMORY;
		}
		memset(srv_info->indicate_en_maps.cccd, 0, srv_info->indicate_en_maps.num * sizeof(struct transfer_module_cccd_enable));
		for (int i = 0; i < indicate_char_num; ++i) {
			srv_info->indicate_en_maps.cccd[i].char_index = indicate_cccd_index[i];
		}
	}

	return RTK_BT_OK;
}

static uint16_t transfer_module_srv_attrs_init(struct rtk_bt_gatt_service *srv,
											   uint8_t uuid_type,
											   uint8_t *uuid_val,
											   int char_num,
											   struct transfer_module_char_t *chars)
{
	int attr_idx = 0;
	uint16_t ret = RTK_BT_OK;

	if (!srv || (char_num != 0 && !chars) || !uuid_val) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	ret = transfer_module_primary_service_init(&srv->attrs[attr_idx++], uuid_type, uuid_val);
	if (ret != RTK_BT_OK) {
		return ret;
	}
	BT_LOGD("%s: srv->attrs[%d].uuid=%d\r\n", __func__, attr_idx - 1, ((struct bt_uuid_16 *)(srv->attrs[attr_idx - 1].uuid))->val);

	for (int char_idx = 0; char_idx < char_num; ++char_idx) {
		uint8_t perm = 0;
		uint16_t props = chars[char_idx].char_property;
		uint8_t uuid_type = chars[char_idx].uuid_type;
		uint8_t *uuid = chars[char_idx].char_uuid;

		ret = transfer_module_char_decl_init(&srv->attrs[attr_idx++], uuid_type, uuid, props);
		if (ret != RTK_BT_OK) {
			break;
		}

		if (props & RTK_BT_GATT_CHRC_READ) {
			perm |= RTK_BT_GATT_PERM_READ;
		}
		if (props & (RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP)) {
			perm |= RTK_BT_GATT_PERM_WRITE;
		}

		ret = transfer_module_char_value_init(&srv->attrs[attr_idx++], uuid_type, uuid, perm);
		if (ret != RTK_BT_OK) {
			break;
		}

		if (props & (RTK_BT_GATT_CHRC_INDICATE | RTK_BT_GATT_CHRC_NOTIFY)) {
			ret = transfer_module_cccd_init(&srv->attrs[attr_idx++], perm);
			if (ret != RTK_BT_OK) {
				break;
			}
		}
	}

	return ret;
}

uint16_t ble_transfer_module_srv_add(struct transfer_module_srv_t *add_srv)
{
	int srv_idx = 0;
	int cccd_num = 0;
	uint16_t ret = RTK_BT_OK;
	struct rtk_bt_gatt_service *srv = NULL;

	if (!add_srv) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	for (srv_idx = 0; srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++srv_idx) {
		if (!user_defined_srvs[srv_idx]) {
			break;
		}
	}

	if (srv_idx > TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM) {
		BT_LOGE("[APP] user defined srvs reach maxmimum\r\n");
		return RTK_BT_FAIL;
	}

	for (int char_idx = 0; char_idx < add_srv->char_num; ++char_idx) {
		if (add_srv->chars[char_idx].char_property & (RTK_BT_GATT_CHRC_NOTIFY | RTK_BT_GATT_CHRC_INDICATE)) {
			++cccd_num;
		}
	}

	user_defined_srvs[srv_idx] = (struct transfer_module_srv_info *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct transfer_module_srv_info));
	if (!user_defined_srvs[srv_idx]) {
		BT_LOGE("[APP] user defined srv info alloc fail\r\n");
		ret = RTK_BT_ERR_NO_MEMORY;
		goto end;
	}
	memset(user_defined_srvs[srv_idx], 0, sizeof(struct transfer_module_srv_info));

	user_defined_srvs[srv_idx]->srv = (struct rtk_bt_gatt_service *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct rtk_bt_gatt_service));
	if (!user_defined_srvs[srv_idx]->srv) {
		BT_LOGE("[APP] alloc user defined srv fail\r\n");
		ret = RTK_BT_ERR_NO_MEMORY;
		goto end;
	}
	srv = user_defined_srvs[srv_idx]->srv;
	memset(srv, 0, sizeof(struct rtk_bt_gatt_service));
	srv->app_id = TRANSFER_MODULE_SRV_ID_BASE + srv_idx;
	srv->attr_count = 1 + 2 * add_srv->char_num + cccd_num;
	srv->attrs = (rtk_bt_gatt_attr_t *)osif_mem_alloc(
					 RAM_TYPE_DATA_ON,
					 srv->attr_count * sizeof(rtk_bt_gatt_attr_t));
	if (!srv->attrs) {
		BT_LOGE("[APP] alloc user defined srv attrs fail\r\n");
		ret = RTK_BT_ERR_NO_MEMORY;
		goto end;
	}

	ret = transfer_module_char_index_init(user_defined_srvs[srv_idx], add_srv->char_num, add_srv->chars);
	if (ret != RTK_BT_OK) {
		goto end;
	}

	ret = transfer_module_srv_attrs_init(srv, add_srv->uuid_type, add_srv->srv_uuid, add_srv->char_num, add_srv->chars);
	if (ret != RTK_BT_OK) {
		goto end;
	}

end:
	if (ret != RTK_BT_OK) {
		ble_transfer_module_del_one_srv(srv_idx);
	}

	return ret;
}

uint16_t ble_transfer_module_srv_add_check(uint8_t uuid_type, uint8_t *uuid)
{
	int srv_idx = 0;
	uint8_t unuse = 0;
	uint8_t exist = 0;

	if (!uuid || (uuid_type != BT_UUID_TYPE_16 && uuid_type != BT_UUID_TYPE_128)) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return FALSE;
	}

	for (srv_idx = 0; srv_idx < TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM; ++srv_idx) {
		if (!user_defined_srvs[srv_idx]) {
			unuse = TRUE;
		} else if (user_defined_srvs[srv_idx]->srv &&
				   user_defined_srvs[srv_idx]->srv->attrs &&
				   user_defined_srvs[srv_idx]->srv->attrs[0].user_data) {

			if (uuid_type == BT_UUID_TYPE_16) {
				struct bt_uuid_16 *uuid16 = (struct bt_uuid_16 *)(user_defined_srvs[srv_idx]->srv->attrs[0].user_data);
				if (uuid16->uuid.type == BT_UUID_TYPE_16 && uuid16->val == LE_TO_U16(uuid)) {
					exist = TRUE;
				}
			} else if (uuid_type == BT_UUID_TYPE_128) {
				struct bt_uuid_128 *uuid128 = (struct bt_uuid_128 *)(user_defined_srvs[srv_idx]->srv->attrs[0].user_data);
				if (uuid128->uuid.type == BT_UUID_TYPE_128 && !memcmp(uuid128->val, uuid, TRANSFER_MODULE_UUID128_LEN)) {
					exist = TRUE;
				}
			}
		}
	}

	if (unuse && !exist) {
		return TRUE;
	}

	return FALSE;
}

uint16_t user_defined_service_char_read_val_set(uint16_t app_id, uint16_t char_idx, uint8_t len, const void *data)
{
	int srv_idx = app_id - TRANSFER_MODULE_SRV_ID_BASE;
	struct transfer_module_read_char *target_char = NULL;
	struct transfer_module_read_char *read_char = NULL;

	if (!len || !data) {
		BT_LOGE("[APP] char value set: len=%d, data=0x%x\r\n", len, data);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (len > TRANSFER_MODULE_READ_VAL_MAX_LEN) {
		BT_LOGE("[APP] char value set: len=%d\r\n", len);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (srv_idx < 0 ||
		srv_idx >= TRANSFER_MODULE_USER_DEFINED_SRV_MAX_NUM) {
		BT_LOGE("[APP] char read value set fail: invalid input param\r\n");
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (!user_defined_srvs[srv_idx]) {
		BT_LOGE("[APP] char read value set fail: invalid app_id=%d\r\n", app_id);
		return RTK_BT_ERR_PARAM_INVALID;
	}

	read_char = user_defined_srvs[srv_idx]->read_value_index.read_char;
	if (!read_char) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	for (int i = 0; i < user_defined_srvs[srv_idx]->read_value_index.num; ++i) {
		if (read_char[i].char_index == char_idx) {
			target_char = &(read_char[i]);
			break;
		}
	}
	if (target_char) {
		target_char->len = len;
		memset(target_char->buf, 0, sizeof(target_char->buf));
		memcpy(target_char->buf, data, len);
	} else {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	return RTK_BT_OK;
}

uint16_t ble_transfer_module_char_read_val_set(uint16_t app_id, uint16_t char_idx, uint8_t len, const void *data)
{
	// default service
	if (app_id == TRANSFER_MODULE_SRV_ID) {
		if (char_idx == TRANSFER_MODULE_READ_VAL_INDEX) {
			return ble_transfer_module_update_read_val(len, (uint8_t *)data);
		}
		return RTK_BT_ERR_PARAM_INVALID;
	}

	// user defined service
	return user_defined_service_char_read_val_set(app_id, char_idx, len, data);
}