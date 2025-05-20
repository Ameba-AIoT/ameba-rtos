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
#include <rtk_bt_gatts.h>
#include <bt_utils.h>
#include <rtk_service_config.h>
#include <bt_api_config.h>

#include <throughput_include.h>

typedef struct {
	uint16_t conn_handle;
	uint8_t cccd_ntf_en_flag;
	uint8_t cccd_ind_en_flag;
	uint32_t timer_count;
	bool phy_update_flag;
	bool disable_tx_flag;
	void *tx_task_handle;
	void *tx_task_sem;
	void *timer_handle_reload;
	void *timer_handle_disconn;
	throughput_config_param_t rcv_cfg_param;
	throughput_result_t rcv_result;
	rtk_bt_gatts_ntf_and_ind_param_t ntf_ind_param;
} throughput_server_links_t;

#define RTK_BT_UUID_THROUGHPUT                  BT_UUID_DECLARE_16(THROUGHPUT_UUID_SRV)
#define RTK_BT_UUID_THROUGHPUT_WRITE_REQ_PARAM  BT_UUID_DECLARE_16(THROUGHPUT_UUID_WRITE_REQ_PARAM)
#define RTK_BT_UUID_THROUGHPUT_WRITE_REQ        BT_UUID_DECLARE_16(THROUGHPUT_UUID_WRITE_REQ)
#define RTK_BT_UUID_THROUGHPUT_WRITE_CMD        BT_UUID_DECLARE_16(THROUGHPUT_UUID_WRITE_CMD)
#define RTK_BT_UUID_THROUGHPUT_NOTIFY           BT_UUID_DECLARE_16(THROUGHPUT_UUID_NOTIFY)
#define RTK_BT_UUID_THROUGHPUT_INDICATE         BT_UUID_DECLARE_16(THROUGHPUT_UUID_INDICATE)
#define RTK_BT_UUID_THROUGHPUT_WRITE_CMD_TEST   BT_UUID_DECLARE_16(THROUGHPUT_UUID_WRITE_CMD_TEST)

#define THROUGHPUT_WRITE_REQ_PARAM_INDEX        2
#define THROUGHPUT_WRITE_REQ_INDEX              4
#define THROUGHPUT_WRITE_CMD_INDEX              6
#define THROUGHPUT_NOTIFY_INDEX                 8
#define THROUGHPUT_NOTIFY_CCCD_INDEX            9
#define THROUGHPUT_INDICATE_INDEX               11
#define THROUGHPUT_INDICATE_CCCD_INDEX          12
#define THROUGHPUT_WRITE_CMD_TEST_INDEX         14

static rtk_bt_gatt_attr_t throughput_srv_attrs[] = {
	/* 0 */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_THROUGHPUT),
	/* 1, 2 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_WRITE_REQ_PARAM,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),
	/* 3, 4 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_WRITE_REQ,
							   RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_WRITE),
	/* 5, 6 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_WRITE_CMD,
							   RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE),
	/* 7, 8 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_NOTIFY,
							   RTK_BT_GATT_CHRC_NOTIFY,
							   RTK_BT_GATT_PERM_NONE),
	/* 9 */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* 10, 11 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_INDICATE,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	/* 12 */
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
	/* 13, 14 */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_THROUGHPUT_WRITE_CMD_TEST,
							   RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
							   RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service throughput_srv_service = RTK_BT_GATT_SERVICE(throughput_srv_attrs, THROUGHPUT_SRV_ID);
static throughput_server_links_t *throughput_srv_links[RTK_BLE_GAP_MAX_LINKS] = {0};

static throughput_server_links_t *ble_throughput_server_choose_link(uint16_t conn_handle)
{
	uint8_t conn_id;

	for (conn_id = 0; conn_id < RTK_BLE_GAP_MAX_LINKS; conn_id++) {
		if (throughput_srv_links[conn_id] != NULL
			&& conn_handle == throughput_srv_links[conn_id]->conn_handle) {
			return throughput_srv_links[conn_id];
		}
	}
	return NULL;
}

static uint16_t ble_throughput_server_link_free(uint8_t conn_id)
{
	throughput_server_links_t *p_conn_link;

	if (!throughput_srv_links[conn_id]) {
		return RTK_BT_OK;
	}
	p_conn_link = throughput_srv_links[conn_id];

	if (p_conn_link->tx_task_sem) {
		p_conn_link->disable_tx_flag = true;  // make tx task to kill itself
		if (false == osif_sem_take(p_conn_link->tx_task_sem, BT_TIMEOUT_FOREVER)) {
			return RTK_BT_ERR_OS_OPERATION;
		}
	}

	if (p_conn_link->ntf_ind_param.data) {
		osif_mem_free((void *)p_conn_link->ntf_ind_param.data);
	}

	if (p_conn_link->tx_task_sem) {
		osif_sem_delete(p_conn_link->tx_task_sem);
		p_conn_link->tx_task_sem = NULL;
	}

	if (p_conn_link->timer_handle_reload) {
		osif_timer_delete(&p_conn_link->timer_handle_reload);
		p_conn_link->timer_handle_reload = NULL;
	}
	if (p_conn_link->timer_handle_disconn) {
		osif_timer_delete(&p_conn_link->timer_handle_disconn);
		p_conn_link->timer_handle_disconn = NULL;
	}

	osif_mem_free(throughput_srv_links[conn_id]);
	throughput_srv_links[conn_id] = NULL;

	return RTK_BT_OK;
}

static void ble_throughput_server_disconn_func(void *timer_handle)
{
	uint16_t conn_handle;
	uint32_t timer_id;
	uint16_t ret = 0;

	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	conn_handle = (uint16_t)timer_id;

	ret = rtk_bt_le_gap_disconnect(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput server disconnect ops failed! err: 0x%x, conn_handle: %d\r\n",
				ret, conn_handle);
	}
}

static void ble_throughput_server_terminate_test(uint16_t conn_handle)
{
	throughput_tx_end_t tx_end;
	throughput_server_links_t *p_conn_link;
	throughput_config_param_t *p_cfg_param;
	throughput_result_t *p_server_result;

	if (!(p_conn_link = ble_throughput_server_choose_link(conn_handle))) {
		return;
	}
	p_cfg_param = &p_conn_link->rcv_cfg_param;
	p_server_result = &p_conn_link->rcv_result;
	tx_end = ble_throughput_get_tx_end(p_cfg_param->mode);

	p_server_result->end_time = osif_sys_time_get();
	p_server_result->elapsed_time = ble_throughput_get_elapsed_time(p_server_result->start_time,
																	p_server_result->end_time);
	p_server_result->rx_all_Bps = p_server_result->rx_all_pkts * p_cfg_param->length_c
								  * 1000 / p_server_result->elapsed_time;

	if (tx_end & THROUGHPUT_TX_END_CLIENT) {
		BT_LOGA("\r\nCONN_HANDLE: %d, START TIME: %d ms, END TIME: %d ms, ELAPSED TIME: %d ms\r\n",
				conn_handle, p_server_result->start_time,
				p_server_result->end_time, p_server_result->elapsed_time);
		BT_LOGA("[0-%d] AVERAGE SERVER THROUGHPUT (RX) = %d Bps\r\n",
				p_conn_link->timer_count, p_server_result->rx_all_Bps);
		BT_LOGA("ALL SERVER RCV PKTS = %d, ERROR SERVER RCV PKTS = %d\r\n \r\n",
				p_server_result->rx_all_pkts, p_server_result->rx_error_pkts);
	}

	if (p_conn_link->timer_handle_reload) {
		osif_timer_delete(&p_conn_link->timer_handle_reload);
		p_conn_link->timer_handle_reload = NULL;
	}

	if (false == osif_timer_create(&p_conn_link->timer_handle_disconn, "tp_server_disconn",
								   (uint32_t)conn_handle, 1000, false, ble_throughput_server_disconn_func)) {
		BT_LOGE("[APP] Throughput server create disconn timer failed ! conn_handle: %d\r\n", conn_handle);
		if (p_conn_link->timer_handle_disconn) {
			osif_timer_delete(&p_conn_link->timer_handle_disconn);
			p_conn_link->timer_handle_disconn = NULL;
		}
		return;
	}
	osif_timer_start(&p_conn_link->timer_handle_disconn);
}

static void ble_throughput_server_reload_timer(void *timer_handle)
{
	uint16_t conn_handle;
	uint32_t timer_id;
	throughput_tx_end_t tx_end;
	throughput_server_links_t *p_conn_link;
	throughput_result_t *p_server_result;
	throughput_config_param_t *p_cfg_param;

	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	conn_handle = (uint16_t)timer_id;

	if (!(p_conn_link = ble_throughput_server_choose_link(conn_handle))) {
		return;
	}
	p_server_result = &p_conn_link->rcv_result;
	p_cfg_param = &p_conn_link->rcv_cfg_param;
	tx_end = ble_throughput_get_tx_end(p_cfg_param->mode);
	p_conn_link->timer_count++;

	if ((tx_end & THROUGHPUT_TX_END_CLIENT) && (p_conn_link->timer_count % p_cfg_param->period == 0)) {
		p_server_result->rx_period_Bps = p_server_result->rx_period_pkts * p_cfg_param->length_c
										 / p_cfg_param->period;
		BT_LOGA("[%d-%d] SERVER THROUGHPUT (RX) = %d Bps (conn_handle %d)\r\n",
				p_conn_link->timer_count - p_cfg_param->period, p_conn_link->timer_count,
				p_server_result->rx_period_Bps, conn_handle);
		p_server_result->rx_period_pkts = 0;
		p_server_result->rx_period_Bps = 0;
	}

	if (p_conn_link->timer_count >= p_cfg_param->duration) {
		p_conn_link->disable_tx_flag = true;
		ble_throughput_server_terminate_test(conn_handle);
	}
}

static void ble_throughput_server_tx_taskentry(void *param)
{
	uint16_t conn_handle = *((uint16_t *)param);
	uint16_t len;
	uint16_t ret = 0;
	throughput_test_mode_t mode = 0;
	throughput_server_links_t *p_conn_link;
	rtk_bt_gatts_ntf_and_ind_param_t *p_ntf_ind_param;

	if (!(p_conn_link = ble_throughput_server_choose_link(conn_handle))) {
		goto exit;
	}

	osif_sem_give(p_conn_link->tx_task_sem);

	len = p_conn_link->rcv_cfg_param.length_s;
	mode = p_conn_link->rcv_cfg_param.mode;
	p_ntf_ind_param = &p_conn_link->ntf_ind_param;

	p_ntf_ind_param->app_id = THROUGHPUT_SRV_ID;
	p_ntf_ind_param->conn_handle = conn_handle;
	p_ntf_ind_param->len = len;
	p_ntf_ind_param->data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	osif_rand((void *)p_ntf_ind_param->data, (uint32_t)len);
	ble_throughput_inc_sequence_generate((void *)p_ntf_ind_param->data, THROUGHPUT_CMP_DATA_BYTES);

	if (mode & THROUGHPUT_MODE_NOTIFY && p_conn_link->cccd_ntf_en_flag) {
		p_ntf_ind_param->index = THROUGHPUT_NOTIFY_INDEX;
		while (!p_conn_link->disable_tx_flag) {
			ret = rtk_bt_gatts_notify(p_ntf_ind_param);
			if (RTK_BT_OK != ret && RTK_BT_ERR_QUEUE_FULL != ret && RTK_BT_ERR_NO_CREDITS != ret) {
				BT_LOGE("[APP] Throughput server notify failed! err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
				break;
			}
		}
	} else if (mode & THROUGHPUT_MODE_INDICATE && p_conn_link->cccd_ind_en_flag) {
		p_ntf_ind_param->index = THROUGHPUT_INDICATE_INDEX;
		if (!p_conn_link->disable_tx_flag) {
			ret = rtk_bt_gatts_indicate(p_ntf_ind_param);
			if (RTK_BT_OK != ret || RTK_BT_ERR_QUEUE_FULL == ret) {
				BT_LOGE("[APP] Throughput server indicate failed! err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
			}
		}
	}

exit:
	BT_LOGA("[APP] Throughput server tx task exit, conn_handle: %d\r\n", conn_handle);
	osif_sem_give(p_conn_link->tx_task_sem);
	osif_task_delete(NULL);
}

static uint16_t ble_throughput_server_create_tx(uint16_t conn_handle, throughput_server_links_t *p_conn_link)
{
	throughput_result_t *p_server_result = &p_conn_link->rcv_result;
	uint16_t *p_conn_handle;
	throughput_tx_end_t tx_end;
	throughput_test_mode_t mode;

	p_conn_handle = (uint16_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(uint16_t));
	*p_conn_handle = conn_handle;
	p_server_result->start_time = osif_sys_time_get();
	mode = p_conn_link->rcv_cfg_param.mode;
	tx_end = ble_throughput_get_tx_end(mode);

	if (false == osif_timer_create(&p_conn_link->timer_handle_reload, "tp_server_reload_timer",
								   (uint32_t)conn_handle, THROUGHPUT_TIME_INTERVAL,
								   true, ble_throughput_server_reload_timer)) {
		BT_LOGE("[APP] Throughput server create reload timer failed ! conn_handle: %d\r\n", conn_handle);
		goto failed;
	}
	osif_timer_start(&p_conn_link->timer_handle_reload);

	if (tx_end & THROUGHPUT_TX_END_SERVER) {
		if (false == osif_sem_create(&p_conn_link->tx_task_sem, 0, 1)) {
			goto failed;
		}

		if (false == osif_task_create(&p_conn_link->tx_task_handle, "tp_server_tx_task",
									  ble_throughput_server_tx_taskentry, (void *)p_conn_handle,
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
	if (p_conn_link->timer_handle_reload) {
		osif_timer_delete(&p_conn_link->timer_handle_reload);
	}
	osif_mem_free(p_conn_handle);
	return RTK_BT_FAIL;
}

static void ble_throughput_server_write_hdl(void *data)
{
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	throughput_server_links_t *p_conn_link;
	throughput_config_param_t *p_cfg_param;
	throughput_result_t *p_server_result;
	throughput_test_mode_t mode;
	uint16_t ret = 0;
	uint16_t conn_handle = p_write_ind->conn_handle;

	if (NULL == (p_conn_link = ble_throughput_server_choose_link(conn_handle))) {
		return;
	}
	p_cfg_param = &p_conn_link->rcv_cfg_param;
	p_server_result = &p_conn_link->rcv_result;
	mode = p_cfg_param->mode;

	switch (p_write_ind->index) {
	case THROUGHPUT_WRITE_REQ_INDEX: {
		uint8_t *p_data = (uint8_t *)p_write_ind->value;
		if (mode & THROUGHPUT_MODE_WRITE_REQ) {
			//BT_LOGA("[APP] Throughput server write request received, conn_handle = %d\r\n", conn_handle);
			ble_throughput_check_calcu(p_server_result, p_cfg_param->length_c, p_write_ind->len,
									   p_data, THROUGHPUT_CMP_DATA_BYTES);
			write_resp.app_id = p_write_ind->app_id;
			write_resp.conn_handle = conn_handle;
			write_resp.cid = p_write_ind->cid,
			write_resp.index = p_write_ind->index;
			write_resp.type = p_write_ind->type;

			ret = rtk_bt_gatts_write_resp(&write_resp);
			if (RTK_BT_OK != ret)
				BT_LOGE("[APP] Throughput server response for client write request failed! err: 0x%x, conn_handle: %d\r\n",
						ret, conn_handle);
		}
		break;
	}

	case THROUGHPUT_WRITE_CMD_INDEX: {
		uint8_t *p_data = (uint8_t *)p_write_ind->value;
		if (mode & THROUGHPUT_MODE_WRITE_CMD) {
			//BT_LOGA("[APP] Throughput server write command received, conn_handle = %d\r\n", conn_handle);
			ble_throughput_check_calcu(p_server_result, p_cfg_param->length_c, p_write_ind->len,
									   p_data, THROUGHPUT_CMP_DATA_BYTES);
		}
		break;
	}

	case THROUGHPUT_WRITE_CMD_TEST_INDEX: {
		throughput_test_cmd_t test_cmd = (throughput_test_cmd_t)(*p_write_ind->value);

		if (p_write_ind->len != 1) {
			BT_LOGE("[APP] Throughput server receive test cmd length error, conn_handle: %d\r\n", conn_handle);
			return;
		}
		BT_LOGA("[APP] Throughput server receive test cmd succeed, test_cmd: %d, conn_handle: %d\r\n",
				test_cmd, conn_handle);

		if (test_cmd == THROUGHPUT_TEST_START) {
			if (RTK_BT_OK != ble_throughput_server_create_tx(conn_handle, p_conn_link)) {
				BT_LOGE("[APP] Throughput server create tx func failed! conn_handle: %d\r\n", conn_handle);
			}
		} else if (test_cmd == THROUGHPUT_TEST_STOP) {
			p_conn_link->disable_tx_flag = true;
			ble_throughput_server_terminate_test(conn_handle);
		}
		break;
	}

	case THROUGHPUT_WRITE_REQ_PARAM_INDEX: {
		memcpy(p_cfg_param, (throughput_config_param_t *)p_write_ind->value, sizeof(throughput_config_param_t));
		BT_LOGA("[APP] Throughput server receive write param succeed: conn_handle = %d, mode = 0x%02x, "   \
				"conn_interval = 0x%x, phy = %d, latency = 0x%x, supervision_timeout = 0x%x, "     \
				"length_s = 0x%x, length_c = 0x%x, duration = 0x%x(s), period = 0x%x(s)\n\r",
				conn_handle,
				p_cfg_param->mode,
				p_cfg_param->conn_interval,
				p_cfg_param->phy,
				p_cfg_param->conn_peripheral_latency,
				p_cfg_param->conn_supervision_timeout,
				p_cfg_param->length_s,
				p_cfg_param->length_c,
				p_cfg_param->duration,
				p_cfg_param->period);

		write_resp.app_id = p_write_ind->app_id;
		write_resp.conn_handle = conn_handle;
		write_resp.cid = p_write_ind->cid,
		write_resp.index = p_write_ind->index;
		write_resp.type = p_write_ind->type;

		ret = rtk_bt_gatts_write_resp(&write_resp);
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] Throughput server response for client write config param failed! err: 0x%x, conn_handle: %d\r\n",
					ret, conn_handle);
		}
		break;
	}

	default:
		BT_LOGE("[APP] Throughput server receive write event unknown index: %d, conn_handle: %d\r\n",
				p_write_ind->index, conn_handle);
		break;
	}
}

void ble_throughput_server_update_phy(uint16_t conn_handle)
{
#if defined(RTK_BLE_5_0_SET_PHYS_SUPPORT) && RTK_BLE_5_0_SET_PHYS_SUPPORT
	uint16_t ret = 0;
	rtk_bt_le_set_phy_param_t conn_phy_param = {0};
	throughput_server_links_t *p_conn_link;

	if (!(p_conn_link = ble_throughput_server_choose_link(conn_handle))
		|| (p_conn_link->phy_update_flag == true)
		|| (p_conn_link->rcv_cfg_param.phy < THROUGHPUT_PHY_CODED_S2)) {
		return;
	}

	p_conn_link->phy_update_flag = true;
	conn_phy_param.conn_handle = conn_handle;
	conn_phy_param.all_phys = THROUGHPUT_PHYS_PREFER_ALL;
	conn_phy_param.tx_phys = THROUGHPUT_PHYS_PREFER_CODED_BIT;
	conn_phy_param.rx_phys = THROUGHPUT_PHYS_PREFER_CODED_BIT;
	if (p_conn_link->rcv_cfg_param.phy == THROUGHPUT_PHY_CODED_S2) {
		conn_phy_param.phy_options = THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S2;
	} else if (p_conn_link->rcv_cfg_param.phy == THROUGHPUT_PHY_CODED_S8) {
		conn_phy_param.phy_options = THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S8;
	}
	if ((ret = rtk_bt_le_gap_set_phy(&conn_phy_param)) != RTK_BT_OK) {
		BT_LOGE("[APP] Throuhgput set coded PHY failed! err:0x%x\r\n", ret);
	}
#else
	(void)conn_handle;
	BT_LOGE("[APP] Platform not support set PHY.\r\n");
#endif
}

rtk_bt_evt_cb_ret_t ble_throughput_server_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
	(void)len;
	uint16_t ret = 0;
	throughput_server_links_t *p_conn_link;
	rtk_bt_gatts_ntf_and_ind_param_t *p_ntf_ind_param;

	switch (event) {
	case RTK_BT_GATTS_EVT_MTU_EXCHANGE: {
		rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
		if (p_gatt_mtu_ind->result == RTK_BT_OK) {
			BT_LOGA("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d\r\n",
					p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
		} else {
			BT_LOGE("[APP] GATTS mtu exchange fail, conn_handle: %d\r\n",
					p_gatt_mtu_ind->conn_handle);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == p_gatts_reg_ind->reg_status) {
			BT_LOGA("[APP] Throughput server register servive succeed!\r\n");
		} else {
			BT_LOGE("[APP] Throughput server register service failed, err: 0x%x\r\n",
					p_gatts_reg_ind->reg_status);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND:
		ble_throughput_server_write_hdl(data);
		break;

	case RTK_BT_GATTS_EVT_CCCD_IND: {
		rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
		if (!(p_conn_link = ble_throughput_server_choose_link(p_cccd_ind->conn_handle))) {
			return RTK_BT_EVT_CB_OK;
		}

		switch (p_cccd_ind->index) {
		case THROUGHPUT_NOTIFY_CCCD_INDEX: {
			if (p_cccd_ind->value & RTK_BT_GATT_CCC_NOTIFY) {
				p_conn_link->cccd_ntf_en_flag = 1;
				BT_LOGA("[APP] Throughput server notify bit enable, conn_handle: %d\r\n",
						p_cccd_ind->conn_handle);
			} else {
				p_conn_link->cccd_ntf_en_flag = 0;
				BT_LOGA("[APP] Throughput server notify bit disable, conn_handle: %d\r\n",
						p_cccd_ind->conn_handle);
			}
			break;
		}
		case THROUGHPUT_INDICATE_CCCD_INDEX: {
			if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
				p_conn_link->cccd_ind_en_flag = 1;
				BT_LOGA("[APP] Throughput server indicate bit enable, conn_handle: %d\r\n",
						p_cccd_ind->conn_handle);
			} else {
				p_conn_link->cccd_ind_en_flag = 0;
				BT_LOGA("[APP] Throughput server indicate bit enable, conn_handle: %d\r\n",
						p_cccd_ind->conn_handle);
			}
			break;
		}
		default:
			break;
		}
		break;
	}

	case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ntf_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK != p_ntf_ind->err_code) {
			BT_LOGE("[APP] Throughput server notify complete failed! err: 0x%x, conn_handle: %d\r\n",
					p_ntf_ind->err_code, p_ntf_ind->conn_handle);
		}
		break;
	}

	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (!(p_conn_link = ble_throughput_server_choose_link(p_ind_ind->conn_handle))) {
			return RTK_BT_EVT_CB_OK;
		}
		p_ntf_ind_param = &p_conn_link->ntf_ind_param;
		if (RTK_BT_OK == p_ind_ind->err_code) {
			if (!p_conn_link->disable_tx_flag) {
				ret = rtk_bt_gatts_indicate(p_ntf_ind_param);
				if (RTK_BT_OK != ret && RTK_BT_ERR_QUEUE_FULL != ret) {
					BT_LOGE("[APP] Throughput server indicate failed! err: 0x%x, conn_handle: %d\r\n",
							ret, p_ntf_ind_param->conn_handle);
				}
			}
		} else {
			BT_LOGE("[APP] Throughput server indicate complete failed! err: 0x%x\r\n, conn_handle: %d\r\n",
					p_ind_ind->err_code, p_ind_ind->conn_handle);
		}
		break;
	}

	default:
		break;
	}

	return RTK_BT_EVT_CB_OK;
}

uint16_t ble_throughput_server_link_connected(uint16_t conn_handle)
{
	uint8_t conn_id;

	if ((rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK)) {
		return RTK_BT_FAIL;
	}

	throughput_srv_links[conn_id] =
		(throughput_server_links_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(throughput_server_links_t));
	memset(throughput_srv_links[conn_id], 0, sizeof(throughput_server_links_t));
	throughput_srv_links[conn_id]->conn_handle = conn_handle;

	return RTK_BT_OK;
}

uint16_t ble_throughput_server_link_disconnected(uint16_t conn_handle)
{
	uint16_t ret = 0;
	uint8_t conn_id;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	ret = ble_throughput_server_link_free(conn_id);
	if (RTK_BT_OK != ret) {
		BT_LOGE("[APP] Throughput server disconnect link failed! err = %d\r\n", ret);
		return ret;
	}

	return RTK_BT_OK;
}

uint16_t ble_throughput_server_add(void)
{
	throughput_srv_service.type = GATT_SERVICE_OVER_BLE;
	throughput_srv_service.server_info = 0;
	throughput_srv_service.user_data = NULL;
	throughput_srv_service.register_status = 0;

	return rtk_bt_gatts_register_service(&throughput_srv_service);
}

uint16_t ble_throughput_server_deinit(void)
{
	uint16_t ret;
	uint8_t conn_id;

	for (conn_id = 0; conn_id < RTK_BLE_GAP_MAX_LINKS; conn_id++) {
		ret = ble_throughput_server_link_free(conn_id);
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] Throughput server deinit failed! err = %d\r\n", ret);
			return ret;
		}
	}

	return RTK_BT_OK;
}