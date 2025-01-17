/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <string.h>
#include <stdio.h>
#include <osif.h>

#include <rtk_bt_att_defs.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_gattc.h>
#include <rtk_bt_gatts.h>
#include <bt_utils.h>
#include <rtk_client_config.h>
#include <rtk_gcs_client.h>
#include <bt_api_config.h>

#include <throughput_include.h>

typedef enum {
	THROUGHPUT_CHAR_WRITE_REQ_PARAM = 0,
	THROUGHPUT_CHAR_WRITE_REQ = 1,
	THROUGHPUT_CHAR_WRITE_CMD = 2,
	THROUGHPUT_CHAR_NOTIFY = 3,
	THROUGHPUT_CHAR_INDICATE = 4,
	THROUGHPUT_CHAR_WRITE_CMD_TEST = 5,
	THROUGHPUT_CHAR_NUM,
} throughput_charac_index_t;

typedef struct {
	uint16_t char_val_handle;
	uint16_t data_len;
	uint16_t cccd_handle;
	uint8_t properties;
} throughput_charac_db_t;

typedef struct {
	client_disc_state_t disc_state;
	uint16_t start_handle;
	uint16_t end_handle;
	throughput_charac_db_t char_db[THROUGHPUT_CHAR_NUM];
} throughput_client_db_t;

typedef struct {
	uint16_t conn_handle;
	uint32_t timer_count;
	bool disable_tx_flag;
	void *tx_task_handle;
	void *tx_task_sem;
	void *timer_handle_reload;
	throughput_client_db_t client_database;
	throughput_config_param_t tp_cfg_param;
	throughput_result_t rcv_result;
	rtk_bt_gattc_write_param_t tp_write_param;
} throughput_client_links_t;

static throughput_client_links_t *throughput_client_links[RTK_BLE_GAP_MAX_LINKS] = {0};
throughput_config_param_t config_param = {0};

void ble_throughput_client_update_phy(uint16_t conn_handle);

static throughput_client_links_t *ble_throughput_client_choose_link(uint16_t conn_handle)
{
	uint8_t conn_id;

	for (conn_id = 0; conn_id < RTK_BLE_GAP_MAX_LINKS; conn_id++) {
		if (throughput_client_links[conn_id] != NULL
			&& conn_handle == throughput_client_links[conn_id]->conn_handle) {
			return throughput_client_links[conn_id];
		}
	}
	return NULL;
}

static uint16_t ble_throughput_client_link_free(uint8_t conn_id)
{
	throughput_client_links_t *p_conn_link;

	if (!throughput_client_links[conn_id]) {
		return RTK_BT_OK;
	}
	p_conn_link = throughput_client_links[conn_id];

	if (p_conn_link->tx_task_sem) {
		p_conn_link->disable_tx_flag = true;  // make tx task to kill itself
		if (false == osif_sem_take(p_conn_link->tx_task_sem, BT_TIMEOUT_FOREVER)) {
			return RTK_BT_ERR_OS_OPERATION;
		}
	}

	if (p_conn_link->tp_write_param.data) {
		osif_mem_free((void *)p_conn_link->tp_write_param.data);
	}

	if (p_conn_link->tx_task_sem) {
		osif_sem_delete(p_conn_link->tx_task_sem);
		p_conn_link->tx_task_sem = NULL;
	}

	if (p_conn_link->timer_handle_reload) {
		osif_timer_delete(&p_conn_link->timer_handle_reload);
		p_conn_link->timer_handle_reload = NULL;
	}

	osif_mem_free(throughput_client_links[conn_id]);
	throughput_client_links[conn_id] = NULL;

	return RTK_BT_OK;
}

static uint16_t ble_throughput_client_send_config_param(uint16_t conn_handle, void *data, uint16_t length)
{
	throughput_config_param_t *p_send_param = (throughput_config_param_t *)data;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (DISC_DONE != p_conn_db->disc_state) {
		return RTK_BT_ERR_STATE_INVALID;
	}

	write_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ_PARAM].char_val_handle;
	write_param.length = length;
	write_param.data = p_send_param;

	return rtk_bt_gattc_write(&write_param);
}

static uint16_t ble_throughput_client_srv_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint16_t tp_srv_uuid = THROUGHPUT_UUID_SRV;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_discover_param_t disc_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	disc_param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	memcpy(disc_param.disc_primary_by_uuid.uuid, &tp_srv_uuid, BT_UUID_SIZE_16);

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	p_conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static uint16_t ble_throughput_client_charac_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_discover_param_t disc_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
	disc_param.disc_char_all.start_handle = p_conn_db->start_handle;
	disc_param.disc_char_all.end_handle = p_conn_db->end_handle;

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	p_conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static uint16_t ble_throughput_client_desc_discover(uint16_t conn_handle)
{
	uint16_t ret = 0;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_discover_param_t disc_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	disc_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
	disc_param.disc_char_by_uuid.start_handle = p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].char_val_handle;
	disc_param.disc_char_by_uuid.end_handle = p_conn_db->end_handle;

	ret = rtk_bt_gattc_discover(&disc_param);
	if (RTK_BT_OK != ret) {
		return ret;
	}

	p_conn_db->disc_state = DISC_START;
	return RTK_BT_OK;
}

static void ble_throughput_client_discover_res_hdl(void *data)
{
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	uint16_t conn_handle = disc_res->conn_handle;
	uint16_t uuid = 0;
	uint16_t ret = 0;
	rtk_bt_status_t disc_status = disc_res->status;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	throughput_config_param_t *p_cfg_param;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)
		|| DISC_START != p_conn_db->disc_state) {
		return;
	}
	p_cfg_param = &p_conn_link->tp_cfg_param;

	if (RTK_BT_STATUS_CONTINUE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID: {
			p_conn_db->start_handle = disc_res->disc_primary_all_per.start_handle;
			p_conn_db->end_handle = disc_res->disc_primary_all_per.end_handle;
			break;
		}
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			uint8_t properties = disc_res->disc_char_all_per.properties;
			uint16_t value_handle = disc_res->disc_char_all_per.value_handle;
			if (BT_UUID_TYPE_16 == disc_res->disc_char_all_per.uuid_type) {
				memcpy(&uuid, disc_res->disc_char_all_per.uuid, sizeof(uint16_t));
			}
			switch (uuid) {
			case THROUGHPUT_UUID_WRITE_REQ_PARAM: {
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ_PARAM].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ_PARAM].properties = properties;
				break;
			}
			case THROUGHPUT_UUID_WRITE_REQ: {
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ].properties = properties;
				break;
			}
			case THROUGHPUT_UUID_WRITE_CMD: {
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD].properties = properties;
				break;
			}
			case THROUGHPUT_UUID_NOTIFY: {
				p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].properties = properties;
				break;
			}
			case THROUGHPUT_UUID_INDICATE: {
				p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].properties = properties;
				break;
			}
			case THROUGHPUT_UUID_WRITE_CMD_TEST: {
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD_TEST].char_val_handle = value_handle;
				p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD_TEST].properties = properties;
				break;
			}
			default:
				break;
			}
			break;
		}
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL: {
			uint16_t desc_handle = disc_res->disc_descriptor_per.handle;
			if (BT_UUID_TYPE_16 == disc_res->disc_descriptor_per.uuid_type) {
				memcpy(&uuid, disc_res->disc_descriptor_per.uuid, sizeof(uint16_t));
			}
			if (BT_UUID_GATT_CCC_VAL == uuid) {
				if (desc_handle > p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].char_val_handle &&
					desc_handle < p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].char_val_handle) {
					p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].cccd_handle = desc_handle;
				} else if (desc_handle > p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].char_val_handle &&
						   desc_handle < p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD_TEST].char_val_handle) {
					p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].cccd_handle = desc_handle;
				}
			}
			break;
		}
		default:
			break;
		}
	} else if (RTK_BT_STATUS_DONE == disc_status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID: {
			if (0 == p_conn_db->start_handle && 0 == p_conn_db->end_handle) {
				p_conn_db->disc_state = DISC_FAILED;
				BT_LOGE("[APP] Throughput client discover service failed! conn_handle: %d\r\n", conn_handle);
			} else {
				ble_throughput_client_charac_discover(conn_handle);
			}
			break;
		}
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL: {
			ble_throughput_client_desc_discover(conn_handle);
			break;
		}
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL: {
			p_conn_db->disc_state = DISC_DONE;
			BT_LOGA("[APP] Throughput client discover all success, conn_handle: %d\r\n", conn_handle);
			ret = ble_throughput_client_send_config_param(conn_handle, p_cfg_param, sizeof(throughput_config_param_t));
			if (RTK_BT_OK != ret) {
				BT_LOGE("[APP] Throughput client send config param failed, err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
			}
			break;
		}
		default:
			break;
		}
	}

#if defined(THROUGHPUT_CLIENT_SHOW_DETAIL) && THROUGHPUT_CLIENT_SHOW_DETAIL
	general_client_discover_res_hdl(data);
#endif
}

static uint16_t ble_throughput_client_enable_notify(uint16_t conn_handle, bool enable)
{
	uint16_t ret = 0;
	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (DISC_DONE != p_conn_db->disc_state) {
		return RTK_BT_ERR_STATE_INVALID;
	}

	cccd_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.bnotify = 1;
	cccd_param.char_val_handle = p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].char_val_handle;
	cccd_param.cccd_handle = p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].cccd_handle;

	if (enable) {
		ret = rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		ret = rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}

	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput client enable notification failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
		return ret;
	}
	return RTK_BT_OK;
}

static uint16_t ble_throughput_client_enable_indicate(uint16_t conn_handle, bool enable)
{
	uint16_t ret = 0;
	rtk_bt_gattc_update_cccd_param_t cccd_param = {0};
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (DISC_DONE != p_conn_db->disc_state) {
		return RTK_BT_ERR_STATE_INVALID;
	}

	cccd_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	cccd_param.conn_handle = conn_handle;
	cccd_param.bindicate = 1;
	cccd_param.char_val_handle = p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].char_val_handle;
	cccd_param.cccd_handle = p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].cccd_handle;

	if (enable) {
		ret = rtk_bt_gattc_enable_notify_or_indicate(&cccd_param);
	} else {
		ret = rtk_bt_gattc_disable_notify_or_indicate(&cccd_param);
	}

	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput client enable indication failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
		return ret;
	}
	return RTK_BT_OK;
}

static void ble_throughput_client_terminate_test(uint16_t conn_handle)
{
	throughput_tx_end_t tx_end;
	throughput_client_links_t *p_conn_link;
	throughput_config_param_t *p_cfg_param;
	throughput_result_t *p_client_result;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))) {
		return;
	}
	p_cfg_param = &p_conn_link->tp_cfg_param;
	p_client_result = &p_conn_link->rcv_result;
	tx_end = ble_throughput_get_tx_end(p_cfg_param->mode);

	p_client_result->end_time = osif_sys_time_get();
	p_client_result->elapsed_time = ble_throughput_get_elapsed_time(p_client_result->start_time,
																	p_client_result->end_time);
	p_client_result->rx_all_Bps = p_client_result->rx_all_pkts * p_cfg_param->length_s
								  * 1000 / p_client_result->elapsed_time;

	if (tx_end & THROUGHPUT_TX_END_SERVER) {
		BT_LOGA("\r\nCONN_HANDLE: %d, START TIME: %d ms, END TIME: %d ms, ELAPSED TIME: %d ms\r\n",
				conn_handle, p_client_result->start_time, p_client_result->end_time, p_client_result->elapsed_time);
		BT_LOGA("[0-%d] AVERAGE CLIENT THROUGHPUT (RX) = %d Bps\r\n",
				p_conn_link->timer_count, p_client_result->rx_all_Bps);
		BT_LOGA("ALL CLIENT RCV PKTS = %d, ERROR CLIENT RCV PKTS = %d\r\n \r\n",
				p_client_result->rx_all_pkts, p_client_result->rx_error_pkts);
	}

	if (p_conn_link->timer_handle_reload) {
		osif_timer_delete(&p_conn_link->timer_handle_reload);
		p_conn_link->timer_handle_reload = NULL;
	}
}

static void ble_throughput_client_reload_timer(void *timer_handle)
{
	uint16_t conn_handle;
	uint32_t timer_id;
	throughput_tx_end_t tx_end;
	throughput_client_links_t *p_conn_link;
	throughput_result_t *p_client_result;
	throughput_config_param_t *p_cfg_param;

	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	conn_handle = (uint16_t)timer_id;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))) {
		return;
	}
	p_client_result = &p_conn_link->rcv_result;
	p_cfg_param = &p_conn_link->tp_cfg_param;
	tx_end = ble_throughput_get_tx_end(p_cfg_param->mode);
	p_conn_link->timer_count++;

	if ((tx_end & THROUGHPUT_TX_END_SERVER) && (p_conn_link->timer_count % p_cfg_param->period == 0)) {
		p_client_result->rx_period_Bps = p_client_result->rx_period_pkts * p_cfg_param->length_s
										 / p_cfg_param->period;
		BT_LOGA("[%d-%d] CLIENT THROUGHPUT (RX) = %d Bps (conn_handle %d)\r\n",
				p_conn_link->timer_count - p_cfg_param->period, p_conn_link->timer_count,
				p_client_result->rx_period_Bps, conn_handle);
		p_client_result->rx_period_pkts = 0;
		p_client_result->rx_period_Bps = 0;
	}

	if (p_conn_link->timer_count >= p_cfg_param->duration) {
		p_conn_link->disable_tx_flag = true;
		ble_throughput_client_terminate_test(conn_handle);
	}
}

static uint16_t ble_throughput_client_update_conn_param(uint16_t conn_handle)
{
	uint16_t ret = 0;
	throughput_client_links_t *p_conn_link;
	throughput_config_param_t *p_cfg_param;
	rtk_bt_le_update_conn_param_t up_conn_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	p_cfg_param = &p_conn_link->tp_cfg_param;

	up_conn_param.conn_handle = conn_handle;
	up_conn_param.conn_interval_min = p_cfg_param->conn_interval;
	up_conn_param.conn_interval_max = p_cfg_param->conn_interval;
	up_conn_param.conn_latency = p_cfg_param->conn_peripheral_latency;
	up_conn_param.supv_timeout = p_cfg_param->conn_supervision_timeout;

	if (RTK_BT_OK != (ret = rtk_bt_le_gap_update_conn_param(&up_conn_param))) {
		return ret;
	}

	return RTK_BT_OK;
}

extern int TRNG_get_random_bytes(void *dst, uint32_t size);
static void ble_throughput_client_tx_taskentry(void *param)
{
	uint16_t conn_handle = *((uint16_t *)param);
	uint16_t ret = 0;
	uint16_t len = 0;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	throughput_test_mode_t mode;
	rtk_bt_gattc_write_param_t *p_write_param;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)
		|| DISC_DONE != p_conn_db->disc_state) {
		goto exit;
	}

	osif_sem_give(p_conn_link->tx_task_sem);

	len = p_conn_link->tp_cfg_param.length_c;
	mode = p_conn_link->tp_cfg_param.mode;
	p_write_param = &p_conn_link->tp_write_param;

	p_write_param->conn_handle = conn_handle;
	p_write_param->profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	p_write_param->length = len;
	p_write_param->data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	TRNG_get_random_bytes((void *)p_write_param->data, (uint32_t)len);
	ble_throughput_inc_sequence_generate((void *)p_write_param->data, THROUGHPUT_CMP_DATA_BYTES);

	if (mode & THROUGHPUT_MODE_WRITE_CMD) {
		p_write_param->type = RTK_BT_GATT_CHAR_WRITE_NO_RSP;
		p_write_param->handle = p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD].char_val_handle;
		while (!p_conn_link->disable_tx_flag) {
			ret = rtk_bt_gattc_write(p_write_param);
			if (RTK_BT_OK != ret && RTK_BT_ERR_QUEUE_FULL != ret && RTK_BT_ERR_NO_CREDITS != ret) {
				BT_LOGE("[APP] Throughput client write cmd failed! err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
				break;
			}
		}
	} else if (mode & THROUGHPUT_MODE_WRITE_REQ) {
		p_write_param->type = RTK_BT_GATT_CHAR_WRITE_REQ;
		p_write_param->handle = p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ].char_val_handle;
		if (!p_conn_link->disable_tx_flag) {
			ret = rtk_bt_gattc_write(p_write_param);
			if (RTK_BT_OK != ret && RTK_BT_ERR_QUEUE_FULL != ret) {
				BT_LOGE("[APP] Throughput client write req failed! err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
			}
		}
	}

exit:
	BT_LOGA("[APP] Throughput client tx task exit, conn_handle: %d\r\n", conn_handle);
	osif_sem_give(p_conn_link->tx_task_sem);
	osif_task_delete(NULL);
}

static uint16_t ble_throughput_client_create_tx(uint16_t conn_handle, throughput_client_links_t *p_conn_link)
{
	throughput_tx_end_t tx_end;
	throughput_test_mode_t mode;
	uint16_t *p_conn_handle;

	p_conn_handle = (uint16_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(uint16_t));
	*p_conn_handle = conn_handle;
	mode = p_conn_link->tp_cfg_param.mode;
	tx_end = ble_throughput_get_tx_end(mode);

	if (tx_end & THROUGHPUT_TX_END_CLIENT) {
		if (false == osif_sem_create(&p_conn_link->tx_task_sem, 0, 1)) {
			goto failed;
		}

		if (false == osif_task_create(&p_conn_link->tx_task_handle, "tp_client_tx_task",
									  ble_throughput_client_tx_taskentry, (void *)p_conn_handle,
									  THROUGHPUT_TASK_STACK_SIZE, THROUGHPUT_TASK_PRIORITY)) {
			goto failed;
		}

		if (false == osif_sem_take(p_conn_link->tx_task_sem, BT_TIMEOUT_FOREVER)) {
			goto failed;
		}
	}

	osif_mem_free(p_conn_handle);
	return RTK_BT_OK;

failed:
	if (p_conn_link->tx_task_handle) {
		osif_task_delete(p_conn_link->tx_task_handle);
	}
	if (p_conn_link->tx_task_sem) {
		osif_sem_delete(p_conn_link->tx_task_sem);
	}
	osif_mem_free(p_conn_handle);
	return RTK_BT_FAIL;
}

static void ble_throughput_client_write_res_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
	uint16_t conn_handle = write_res->conn_handle;
	uint16_t attr_handle = write_res->handle;
	uint16_t err = write_res->err_code;
	rtk_bt_gattc_write_type_t write_type = write_res->type;
	rtk_bt_status_t write_status = write_res->status;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_write_param_t *p_write_param;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return;
	}

	if (err != RTK_BT_OK || write_status != RTK_BT_STATUS_DONE) {
		BT_LOGE("[APP] Throughput client write failed! err: 0x%x, conn_handle: %d\r\n",
				err, conn_handle);
		return;
	}

	if (attr_handle == p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ_PARAM].char_val_handle
		&& RTK_BT_GATT_CHAR_WRITE_REQ == write_type) {
		BT_LOGA("[APP] Throughput client send config param success, conn_handle: %d\r\n", conn_handle);
		err = ble_throughput_client_update_conn_param(conn_handle);
		if (RTK_BT_OK != err) {
			BT_LOGE("[APP] Throughput client update connect params failed! err: 0x%x, conn_handle: %d\r\n",
					err, conn_handle);
		}
		ble_throughput_client_update_phy(conn_handle);

	} else if (attr_handle == p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_REQ].char_val_handle
			   && RTK_BT_GATT_CHAR_WRITE_REQ == write_type) {
		p_write_param = &p_conn_link->tp_write_param;
		if (!p_conn_link->disable_tx_flag) {
			if (RTK_BT_OK != (err = rtk_bt_gattc_write(p_write_param))) {
				BT_LOGE("[APP] Throughput client write req failed! err: 0x%x\r\n", err);
			}
		}

	} else if (attr_handle == p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD].char_val_handle
			   && RTK_BT_GATT_CHAR_WRITE_NO_RSP == write_type) {
		//BT_LOGA("[APP] Throughput client write cmd complete succeed! conn_handle = %d\r\n", conn_handle);

	} else if (attr_handle == p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD_TEST].char_val_handle
			   && RTK_BT_GATT_CHAR_WRITE_NO_RSP == write_type) {
		BT_LOGA("[APP] Throughput client send test cmd success, conn_handle: %d\r\n", conn_handle);
		if (p_conn_link->disable_tx_flag) {
			ble_throughput_client_terminate_test(conn_handle);
			return;
		}
		if (RTK_BT_OK != ble_throughput_client_create_tx(conn_handle, p_conn_link)) {
			BT_LOGE("[APP] Throughput client create tx func failed! conn_handle: %d\r\n", conn_handle);
		}

	} else {
		BT_LOGE("[APP] Throughput client write attr_handle wrong! conn_handle: %d\r\n", conn_handle);
	}
}

uint16_t ble_throughput_client_send_test_cmd(uint16_t conn_handle, uint8_t *data, uint16_t len)
{
	uint16_t ret = 0;
	throughput_test_cmd_t enable = (throughput_test_cmd_t)(*data);
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;
	rtk_bt_gattc_write_param_t write_param = {0};

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))
		|| !(p_conn_db = &p_conn_link->client_database)) {
		return RTK_BT_ERR_NO_ENTRY;
	}

	if (DISC_DONE != p_conn_db->disc_state) {
		return RTK_BT_ERR_STATE_INVALID;
	}

	if (enable == THROUGHPUT_TEST_START) {
		p_conn_link->rcv_result.start_time = osif_sys_time_get();
		if (p_conn_link->timer_handle_reload) {
			return RTK_BT_ERR_ALREADY_DONE;
		}
		if (false == osif_timer_create(&p_conn_link->timer_handle_reload, "tp_client_reload_timer",
									   (uint32_t)conn_handle, THROUGHPUT_TIME_INTERVAL,
									   true, ble_throughput_client_reload_timer)) {
			BT_LOGE("[APP] Throughput client create reload timer failed!, conn_handle: %d\r\n", conn_handle);
			if (p_conn_link->timer_handle_reload) {
				osif_timer_delete(&p_conn_link->timer_handle_reload);
				p_conn_link->timer_handle_reload = NULL;
			}
			return RTK_BT_ERR_OS_OPERATION;
		}
		osif_timer_start(&p_conn_link->timer_handle_reload);
	} else if (enable == THROUGHPUT_TEST_STOP) {
		p_conn_link->disable_tx_flag = true;
	}

	write_param.profile_id = THROUGHPUT_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_NO_RSP;
	write_param.handle = p_conn_db->char_db[THROUGHPUT_CHAR_WRITE_CMD_TEST].char_val_handle;
	write_param.length = len;
	write_param.data = data;

	if (RTK_BT_OK != (ret = rtk_bt_gattc_write(&write_param))) {
		BT_LOGE("[APP] Throughput client send test cmd failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
#if 1
		uint16_t cnt = 0;
		while (RTK_BT_ERR_NO_CREDITS == ret && cnt < 0xFFFF) {
			ret = rtk_bt_gattc_write(&write_param);
			cnt++;
		}
		BT_LOGA("[APP] Throughput client retry send test cmd %d times! ret: %d\r\n", cnt, ret);
#endif
		return ret;
	}

	return RTK_BT_OK;
}

uint16_t ble_throughput_client_select_tx_mode(uint16_t conn_handle)
{
	// entry function for different modes
	uint16_t ret = 0;
	throughput_client_links_t *p_conn_link;
	throughput_test_mode_t mode;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	mode = p_conn_link->tp_cfg_param.mode;

	switch (mode) {
	case THROUGHPUT_MODE_NOTIFY:
	case THROUGHPUT_MODE_NOTIFY_WRITE_CMD:
		ret = ble_throughput_client_enable_notify(conn_handle, 1);
		break;
	case THROUGHPUT_MODE_INDICATE:
	case THROUGHPUT_MODE_INDICATE_WRITE_REQ:
		ret = ble_throughput_client_enable_indicate(conn_handle, 1);
		break;
	default:
		break;
	}

	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput client select tx mode failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
		return ret;
	}
	return RTK_BT_OK;
}

void ble_throughput_client_update_phy(uint16_t conn_handle)
{
#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
	uint16_t ret = 0;
	uint8_t prefer_phy = THROUGHPUT_PHYS_PREFER_2M_BIT;
	uint16_t option = THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_NO;
	rtk_bt_le_set_phy_param_t conn_phy_param = {0};
	throughput_client_links_t *p_conn_link;

	if (!(p_conn_link = ble_throughput_client_choose_link(conn_handle))) {
		return;
	}

	if (p_conn_link->tp_cfg_param.phy == THROUGHPUT_PHY_1M) {
		prefer_phy = THROUGHPUT_PHYS_PREFER_1M_BIT;
	} else if (p_conn_link->tp_cfg_param.phy == THROUGHPUT_PHY_2M) {
		prefer_phy = THROUGHPUT_PHYS_PREFER_2M_BIT;
	} else if (p_conn_link->tp_cfg_param.phy == THROUGHPUT_PHY_CODED_S2) {
		prefer_phy = THROUGHPUT_PHYS_PREFER_CODED_BIT;
		option = THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S2;
	} else if (p_conn_link->tp_cfg_param.phy == THROUGHPUT_PHY_CODED_S8) {
		prefer_phy = THROUGHPUT_PHYS_PREFER_CODED_BIT;
		option = THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S8;
	}
	conn_phy_param.conn_handle = conn_handle;
	conn_phy_param.all_phys = THROUGHPUT_PHYS_PREFER_ALL;
	conn_phy_param.tx_phys = prefer_phy;
	conn_phy_param.rx_phys = prefer_phy;
	conn_phy_param.phy_options = option;

	if (RTK_BT_OK != (ret = rtk_bt_le_gap_set_phy(&conn_phy_param))) {
		BT_LOGE("[APP] Throughput client update phy ops failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
		if (RTK_BT_OK != (ret = rtk_bt_le_gap_disconnect(conn_handle))) {
			BT_LOGE("[APP] Throughput client disconnect ops failed! err: 0x%x, conn_handle: %d\r\n",
					ret, conn_handle);
		}
	}
#else
	(void)conn_handle;
	BT_LOGE("[APP] Platform not support set PHY.\r\n");
#endif
}

rtk_bt_evt_cb_ret_t ble_throughput_client_gattc_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	throughput_client_links_t *p_conn_link;
	throughput_client_db_t *p_conn_db;

	switch (event) {
	case RTK_BT_GATTC_EVT_MTU_EXCHANGE: {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTC mtu exchange success, mtu_size: %d, conn_handle: %d \r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTC mtu exchange fail, conn_handle: %d\r\n", p_gatt_mtu_ind->conn_handle);
		}
		break;
	}
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		ble_throughput_client_discover_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		ble_throughput_client_write_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_NOTIFY_IND: {
		rtk_bt_gattc_cccd_value_ind_t *ntf_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
		if (!(p_conn_link = ble_throughput_client_choose_link(ntf_ind->conn_handle))
			|| !(p_conn_db = &p_conn_link->client_database)
			|| p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].char_val_handle != ntf_ind->value_handle) {
			return RTK_BT_EVT_CB_OK;
		}
		//BT_LOGA("[APP] Throughput client notification received, conn_handle = %d\r\n", ntf_ind->conn_handle);
		ble_throughput_check_calcu(&p_conn_link->rcv_result, p_conn_link->tp_cfg_param.length_s,
								   ntf_ind->len, ntf_ind->value, THROUGHPUT_CMP_DATA_BYTES);
		break;
	}
	case RTK_BT_GATTC_EVT_INDICATE_IND: {
		rtk_bt_gattc_cccd_value_ind_t *indic_ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
		if (!(p_conn_link = ble_throughput_client_choose_link(indic_ind->conn_handle))
			|| !(p_conn_db = &p_conn_link->client_database)
			|| p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].char_val_handle != indic_ind->value_handle) {
			return RTK_BT_EVT_CB_OK;
		}
		//BT_LOGA("[APP] Throughput client indication received, conn_handle = %d\r\n", indic_ind->conn_handle);
		ble_throughput_check_calcu(&p_conn_link->rcv_result, p_conn_link->tp_cfg_param.length_s,
								   indic_ind->len, indic_ind->value, THROUGHPUT_CMP_DATA_BYTES);
		break;
	}
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND: {
		rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
		if (!(p_conn_link = ble_throughput_client_choose_link(cccd_update->conn_handle))
			|| !(p_conn_db = &p_conn_link->client_database)
			|| RTK_BT_STATUS_DONE != cccd_update->status) {
			return RTK_BT_EVT_CB_OK;
		}
		if (p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].cccd_handle == cccd_update->cccd_handle) {
			BT_LOGA("[APP] Throughput client enable notify succeed, conn_handle:%d\r\n", cccd_update->conn_handle);
		} else if (p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].cccd_handle == cccd_update->cccd_handle) {
			BT_LOGA("[APP] Throughput client enable indicate succeed, conn_handle:%d\r\n", cccd_update->conn_handle);
		}
		break;
	}
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND: {
		rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
		if (!(p_conn_link = ble_throughput_client_choose_link(cccd_update->conn_handle))
			|| !(p_conn_db = &p_conn_link->client_database)
			|| RTK_BT_STATUS_DONE != cccd_update->status) {
			return RTK_BT_EVT_CB_OK;
		}
		if (p_conn_db->char_db[THROUGHPUT_CHAR_NOTIFY].cccd_handle == cccd_update->cccd_handle) {
			BT_LOGA("[APP] Throughput client disable notify succeed, conn_handle:%d\r\n", cccd_update->conn_handle);
		} else if (p_conn_db->char_db[THROUGHPUT_CHAR_INDICATE].cccd_handle == cccd_update->cccd_handle) {
			BT_LOGA("[APP] Throughput client disable indicate succeed, conn_handle: %d\r\n", cccd_update->conn_handle);
		}
		break;
	}
	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

uint16_t ble_throughput_client_link_connected(uint16_t conn_handle)
{
	uint16_t ret;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	throughput_client_links[conn_id] =
		(throughput_client_links_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(throughput_client_links_t));
	memset(throughput_client_links[conn_id], 0, sizeof(throughput_client_links_t));
	memcpy(&throughput_client_links[conn_id]->tp_cfg_param, &config_param, sizeof(throughput_config_param_t));
	throughput_client_links[conn_id]->conn_handle = conn_handle;

	ret = ble_throughput_client_srv_discover(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput client discover service failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
		return ret;
	}

	return RTK_BT_OK;
}

uint16_t ble_throughput_client_link_disconnected(uint16_t conn_handle)
{
	uint16_t ret;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	ret = ble_throughput_client_link_free(conn_id);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput client disconnect link failed! err = %d\r\n", ret);
		return ret;
	}

	return RTK_BT_OK;
}

uint16_t ble_throughput_client_add(void)
{
	return rtk_bt_gattc_register_profile(THROUGHPUT_CLIENT_PROFILE_ID);
}

uint16_t ble_throughput_client_delete(void)
{
	uint16_t ret;
	uint8_t conn_id;

	for (conn_id = 0; conn_id < RTK_BLE_GAP_MAX_LINKS; conn_id++) {
		ret = ble_throughput_client_link_free(conn_id);
		if (RTK_BT_OK != ret) {
			return ret;
		}
	}

	return RTK_BT_OK;
}