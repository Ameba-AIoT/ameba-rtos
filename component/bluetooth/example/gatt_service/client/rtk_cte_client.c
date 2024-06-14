/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gattc.h>
#include <rtk_client_config.h>
#include <rtk_cte_client.h>
#include <rtk_gcs_client.h>
#include <bt_utils.h>

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT

#define CTE_SRV_UUID                        (0x184A)
#define CTE_ENABLE_CHAR_UUID                (0x2BAD)
#define CTE_ADV_MIN_LEN_CHAR_UUID           (0x2BAE)
#define CTE_ADV_MIN_TX_CNT_CHAR_UUID        (0x2BAF)
#define CTE_ADV_TX_DURATION_CHAR_UUID       (0x2BB0)
#define CTE_ADV_INTERVAL_CHAR_UUID          (0x2BB1)
#define CTE_ADV_PHY_CHAR_UUID               (0x2BB2)

typedef struct {
	uint8_t  properties;
	uint16_t char_val_handle;
} cte_charac_db_t;

typedef struct {
	client_disc_state_t disc_state;
	uint16_t            start_handle;
	uint16_t            end_handle;
	cte_charac_db_t     char_db[CTE_CHAR_NUM];
} cte_client_db_t;

static cte_client_db_t *cte_database[RTK_BLE_GAP_MAX_LINKS] = {0};

static uint16_t cte_char_uuid_arr[CTE_CHAR_NUM] = {
	CTE_ENABLE_CHAR_UUID,
	CTE_ADV_MIN_LEN_CHAR_UUID,
	CTE_ADV_MIN_TX_CNT_CHAR_UUID,
	CTE_ADV_TX_DURATION_CHAR_UUID,
	CTE_ADV_INTERVAL_CHAR_UUID,
	CTE_ADV_PHY_CHAR_UUID,
};

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
static rtk_bt_gattc_uuid_t  ctes_uuid = {
	.is_uuid16 = true,
	.p.uuid16  = CTE_SRV_UUID
};
#endif

uint16_t cte_client_add(void)
{
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	return rtk_bt_gattc_register_profile(CTE_CLIENT_PROFILE_ID, ctes_uuid);
#else
	return rtk_bt_gattc_register_profile(CTE_CLIENT_PROFILE_ID);
#endif
}

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB

static uint16_t cte_client_char_find(uint16_t conn_handle)
{
	uint8_t i;
	uint16_t char_handle;
	rtk_bt_gattc_find_param_t find_param = {0};
	rtk_bt_gattc_uuid_t char_uuid = {
		.is_uuid16 = true,
	};
	uint8_t conn_id;
	cte_client_db_t *conn_cte_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	find_param.conn_handle = conn_handle;
	find_param.type = RTK_BT_GATT_FIND_CHARACTERISTIC_HANDLE;
	find_param.find_char.srv_uuid = ctes_uuid;
	find_param.find_char.p_handle = &char_handle;

	for (i = 0; i < CTE_CHAR_NUM; ++i) {
		char_uuid.p.uuid16 = cte_char_uuid_arr[i];
		find_param.find_char.char_uuid = char_uuid;
		if (rtk_bt_gattc_find(&find_param) == RTK_BT_OK) {
			conn_cte_db->char_db[i].char_val_handle = char_handle;
			BT_LOGA("[APP] CTE characteristic uuid %u handle is 0x%04x.\r\n", char_uuid.p.uuid16, char_handle);
			BT_AT_PRINT("+BLEGATTC:disc,%d,%d,%04x,%04x,0x%04x\r\n",
						find_param.type, find_param.conn_handle,
						find_param.find_char.srv_uuid.p.uuid16,
						find_param.find_char.char_uuid.p.uuid16,
						char_handle);
		} else {
			BT_LOGE("[APP] Find CTE characteristic uuid %u fail.\r\n");
		}
	}

	return RTK_BT_OK;
}

#else

static uint16_t cte_client_charac_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	rtk_bt_gattc_discover_param_t disc_param = {0};
	uint8_t conn_id;
	cte_client_db_t *conn_cte_db = NULL;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = CTE_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
	disc_param.disc_char_all.start_handle = conn_cte_db->start_handle;
	disc_param.disc_char_all.end_handle   = conn_cte_db->end_handle;

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	conn_cte_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

#endif

uint16_t cte_client_attach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;


	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	cte_database[conn_id] = (cte_client_db_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(cte_client_db_t));
	memset(cte_database[conn_id], 0, sizeof(cte_client_db_t));

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	cte_database[conn_id]->disc_state = DISC_DONE;
#endif

	return RTK_BT_OK;
}

static uint16_t cte_client_database_free(uint8_t conn_id)
{
	if (cte_database[conn_id]) {
		osif_mem_free(cte_database[conn_id]);
		cte_database[conn_id] = NULL;
	}

	return RTK_BT_OK;
}


uint16_t cte_client_detach_conn(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	ret = cte_client_database_free(conn_id);
	if (ret) {
		return ret;
	}

	return RTK_BT_OK;
}

uint16_t cte_client_delete(void)
{
	int i = 0;
	uint16_t ret = 0;

	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		ret = cte_client_database_free(i);
		if (ret) {
			return ret;
		}
	}

	return RTK_BT_OK;
}

static void cte_client_discover_res_hdl(void *data)
{
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;

	if (disc_res->is_found) {
		BT_LOGA("[APP] CTE client discover all success\r\n");
		cte_client_attach_conn(disc_res->conn_handle);
		cte_client_char_find(disc_res->conn_handle);
	}
#else

	uint16_t uuid = 0;
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	rtk_bt_status_t disc_status = disc_res->status;
	uint16_t conn_handle = disc_res->conn_handle;
	uint8_t conn_id;
	cte_client_db_t *conn_cte_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db || conn_cte_db->disc_state != DISC_START) {
		return;
	}

	if (RTK_BT_STATUS_CONTINUE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			conn_cte_db->start_handle = disc_res->disc_primary_all_per.start_handle;
			conn_cte_db->end_handle   = disc_res->disc_primary_all_per.end_handle;
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			uint8_t properties = disc_res->disc_char_all_per.properties;
			uint16_t value_handle = disc_res->disc_char_all_per.value_handle;
			cte_charac_db_t *char_db = NULL;

			if (BT_UUID_TYPE_16 == disc_res->disc_char_all_per.uuid_type) {
				memcpy(&uuid, disc_res->disc_char_all_per.uuid, sizeof(uint16_t));
			}
			switch (uuid) {
			case CTE_ENABLE_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ENABLE];
				BT_LOGA("[APP] CTE_ENABLE_CHAR_UUID discover success\r\n");
				break;
			case CTE_ADV_INTERVAL_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ADV_INTERVAL];
				BT_LOGA("[APP] CTE_ADV_INTERVAL_CHAR_UUID discover success\r\n");
				break;
			case CTE_ADV_MIN_LEN_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ADV_MIN_LEN];
				BT_LOGA("[APP] CTE_ADV_MIN_LEN_CHAR_UUID discover success\r\n");
				break;
			case CTE_ADV_MIN_TX_CNT_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ADV_MIN_TX_CNT];
				BT_LOGA("[APP] CTE_ADV_MIN_TX_CNT_CHAR_UUID discover success\r\n");
				break;
			case CTE_ADV_PHY_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ADV_PHY];
				BT_LOGA("[APP] CTE_ADV_PHY_CHAR_UUID discover success\r\n");
				break;
			case CTE_ADV_TX_DURATION_CHAR_UUID:
				char_db = &conn_cte_db->char_db[CTE_CHAR_CTE_ADV_TX_DURATION];
				BT_LOGA("[APP] CTE_ADV_TX_DURATION_CHAR_UUID discover success\r\n");
				break;
			default:
				break;
			}

			if (char_db) {
				char_db->char_val_handle = value_handle;
				char_db->properties      = properties;
			}
			break;
		}
		default:
			break;
		}
	} else if (RTK_BT_STATUS_DONE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			if (0 == conn_cte_db->start_handle && 0 == conn_cte_db->end_handle) {
				conn_cte_db->disc_state = DISC_FAILED;
				BT_LOGE("[APP] CTE client discover service failed\r\n");
			} else {
				cte_client_charac_discover(conn_handle);
			}
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
			conn_cte_db->disc_state = DISC_DONE;
			BT_LOGA("[APP] CTE client discover all success\r\n");
			break;
		default:
			break;
		}
	}

#if defined(CTE_CLIENT_SHOW_DETAIL) && CTE_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl(data);
#endif
#endif /* #if RTK_BLE_MGR_LIB */
}

static void cte_client_write_res_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
	rtk_bt_status_t write_status = write_res->status;
	uint16_t att_handle = write_res->handle;
	uint16_t conn_handle = write_res->conn_handle;
	uint8_t conn_id;
	cte_client_db_t *conn_cte_db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db) {
		return;
	}

	if (RTK_BT_STATUS_DONE == write_status) {
		uint8_t i;
		for (i = 0; i < CTE_CHAR_NUM; ++i) {
			if (att_handle == conn_cte_db->char_db[i].char_val_handle) {
				BT_LOGA("[APP] CTE client write characteristic uuid 0x%04x success\r\n", cte_char_uuid_arr[i]);
				BT_AT_PRINT("+BLEGATTC:write,%u,0x%04x,%u\r\n",
							conn_handle, write_res->handle, write_status);
				break;
			}
		}
	} else if (RTK_BT_STATUS_FAIL == write_status) {
		BT_LOGE("[APP] CTE client write characteristic fail, err_code %u\r\n", write_res->err_code);
	}
}

rtk_bt_evt_cb_ret_t cte_client_app_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		cte_client_discover_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		cte_client_write_res_hdl(data);
		break;
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
uint16_t cte_client_srv_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint16_t cte_srv_uuid = CTE_SRV_UUID;
	cte_client_db_t *conn_cte_db;
	uint8_t conn_id;
	rtk_bt_gattc_discover_param_t disc_param = {0};
	disc_param.profile_id = CTE_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	disc_param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	memcpy(disc_param.disc_primary_by_uuid.uuid, &cte_srv_uuid, BT_UUID_SIZE_16);

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	conn_cte_db->disc_state = DISC_START;
	return RTK_BT_OK;
}
#endif  /* #if !RTK_BLE_MGR_LIB */

uint16_t cte_client_write_charac(uint16_t conn_handle,
								 cte_charac_index_e char_idx,
								 uint16_t len,
								 uint8_t *data)
{
	rtk_bt_gattc_write_param_t write_param = {0};
	cte_client_db_t *conn_cte_db;
	uint8_t conn_id;

	if (char_idx >= CTE_CHAR_NUM || !data) {
		return RTK_BT_ERR_PARAM_INVALID;
	}

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	conn_cte_db = cte_database[conn_id];

	if (!conn_cte_db) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (conn_cte_db->disc_state != DISC_DONE) {
		BT_LOGE("[APP] CTE client need discover service before write charac !!!\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}

	write_param.conn_handle = conn_handle;
	write_param.profile_id = CTE_CLIENT_PROFILE_ID;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = conn_cte_db->char_db[char_idx].char_val_handle;
	write_param.length = len;
	write_param.data = data;

	return rtk_bt_gattc_write(&write_param);

}

#endif  /* RTK_BLE_5_1_CTE_SUPPORT */
