/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <osif.h>
#include <log_service.h>

#include <rtk_bt_def.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <atcmd_bt_impl.h>
#include <bt_utils.h>
#include <rtk_bas.h>
#include <rtk_hrs.h>
#include <rtk_simple_ble_service.h>
#include <rtk_cte_service.h>

static int atcmd_bt_gatts_notify(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};

	if (argc != 5 && argc != 6) {
		AT_PRINTK("[ATBC] GATTS notify failed, wrong args num!");
		return -1;
	}
	ntf_param.conn_handle = str_to_int(argv[0]);
	ntf_param.app_id = str_to_int(argv[1]);
	ntf_param.index = str_to_int(argv[2]);
	ntf_param.len = str_to_int(argv[3]);

	if (ntf_param.len != strlen(argv[4]) / 2) {
		AT_PRINTK("[ATBC] GATTS notify failed, notify data len is wrong!");
		return -1;
	}
	ntf_param.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, ntf_param.len);
	hexdata_str_to_array(argv[4], (uint8_t *)ntf_param.data, ntf_param.len);

	if (argc == 6) {
		ntf_param.cid = atoi(argv[5]);
	}

	ret = rtk_bt_gatts_notify(&ntf_param);
	if (RTK_BT_OK != ret) {
		osif_mem_free((void *)ntf_param.data);
		AT_PRINTK("[ATBC] GATTS notify failed! err: 0x%x", ret);
		return -1;
	}

	osif_mem_free((void *)ntf_param.data);
	AT_PRINTK("[ATBC] GATTS notify sending ...");
	return 0;
}

static int atcmd_bt_gatts_indicate(int argc, char **argv)
{
	uint16_t ret = 0;
	rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};

	if (argc != 5 && argc != 6) {
		AT_PRINTK("[ATBC] GATTS notify failed, wrong args num!");
		return -1;
	}
	ntf_param.conn_handle = str_to_int(argv[0]);
	ntf_param.app_id = str_to_int(argv[1]);
	ntf_param.index = str_to_int(argv[2]);
	ntf_param.len = str_to_int(argv[3]);

	if (ntf_param.len != strlen(argv[4]) / 2) {
		AT_PRINTK("[ATBC] GATTS indicate failed: indicate data len is wrong!");
		return -1;
	}
	ntf_param.data = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, ntf_param.len);
	hexdata_str_to_array(argv[4], (uint8_t *)ntf_param.data, ntf_param.len);

	if (argc == 6) {
		ntf_param.cid = atoi(argv[5]);
	}

	ret = rtk_bt_gatts_indicate(&ntf_param);
	if (RTK_BT_OK != ret) {
		osif_mem_free((void *)ntf_param.data);
		AT_PRINTK("[ATBC] GATTS indicate failed! err: 0x%x", ret);
		return -1;
	}

	osif_mem_free((void *)ntf_param.data);
	AT_PRINTK("[ATBC] GATTS indicate sending ...");
	return 0;
}

#if ((defined(CONFIG_BT_PERIPHERAL) && CONFIG_BT_PERIPHERAL) || \
    (defined(CONFIG_BT_SCATTERNET) && CONFIG_BT_SCATTERNET))
/* add for EMC test */
uint8_t gatts_loop_send_task_running = 0;
static void gatts_loop_send_task_entry(void *ctx)
{
	uint32_t param = (uint32_t)ctx;
	uint16_t conn_handle = param & 0xFFFF;
	uint16_t interval = (param >> 16) & 0xFFFF;

	while (true) {
		simple_ble_srv_cccd_indicate(conn_handle);
		simple_ble_srv_cccd_notify(conn_handle);
		battery_cccd_notify(conn_handle);
		heart_rate_measurement_cccd_notify(conn_handle);
		if (!gatts_loop_send_task_running) {
			break;
		}

		osif_delay(interval);
	}
	gatts_loop_send_task_running = 2;
	osif_task_delete(NULL);
}

static int atcmd_bt_gatts_loop_send(int argc, char **argv)
{
	(void)argc;
	void *handle;
	uint8_t enable;
	uint16_t conn_handle;
	uint16_t interval;
	uint32_t param;

	enable = str_to_int(argv[0]);

	if (enable) {
		conn_handle = str_to_int(argv[1]);
		interval = str_to_int(argv[2]);

		param = (interval << 16) | conn_handle;
		gatts_loop_send_task_running = 1;

		if (false == osif_task_create(&handle, "gatts_loop_send_task",
									  gatts_loop_send_task_entry, (void *)param,
									  1024, 2))  {
			gatts_loop_send_task_running = 0;
			AT_PRINTK("[ATBC] GATTS loop send task create failed");
			return -1;
		}
	} else {
		if (gatts_loop_send_task_running == 1) {
			gatts_loop_send_task_running = 0;
			while (!gatts_loop_send_task_running) {
				osif_delay(100);
			}
		}
		gatts_loop_send_task_running = 0;
	}

	AT_PRINTK("[ATBC] loop send %s.", enable ? "start" : "stop");
	return 0;
}

#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
static int atcmd_bt_gatts_cte_set_param(int argc, char **argv)
{
	int ret = 0;
	CTE_SRV_PARAM_TYPE_e param_type;

	if (argc < 2) {
		AT_PRINTK("[ATBC] GATTS CTE set param failed: wrong argc num!");
		return -1;
	}

	param_type = str_to_int(argv[0]);

	switch (param_type) {
	case CTE_SRV_PARAM_TYPE_ENABLE_ADV_CTE:
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_LEN:
	case CTE_SRV_PARAM_TYPE_ADV_CTE_MIN_TX_CNT:
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TX_DURATION:
	case CTE_SRV_PARAM_TYPE_ADV_CTE_PHY:
	case CTE_SRV_PARAM_TYPE_ADV_CTE_TYPE:
	case CTE_SRV_PARAM_TYPE_CONN_CTE_TYPE: {
		uint8_t u8Value = str_to_int(argv[1]);
		cte_srv_set_params(param_type, sizeof(u8Value), &u8Value);
		break;
	}
	case CTE_SRV_PARAM_TYPE_ENABLE_CONN_CTE: {
		cte_conn_enable_t param;
		if (argc < 3) {
			AT_PRINTK("[ATBC] GATTS CTE set param failed: wrong argc num!");
			ret = -1;
			break;
		}

		param.enable = str_to_int(argv[1]);
		param.conn_handle = str_to_int(argv[2]);
		cte_srv_set_params(param_type, sizeof(param), &param);
		break;
	}
	case CTE_SRV_PARAM_TYPE_ADV_CTE_INTERVAL: {
		uint16_t u16Value = str_to_int(argv[1]);
		cte_srv_set_params(param_type, sizeof(u16Value), &u16Value);
		break;
	}
	case CTE_SRV_PARAM_TYPE_CONN_ANTENNA:
	case CTE_SRV_PARAM_TYPE_ADV_ANTENNA: {
		cte_antenna_t ant;
		if (argc < 2 || argc > (RTK_BLE_CTE_NUM_ANT_ID_MAX + 2)) {
			AT_PRINTK("[ATBC] GATTS CTE set param failed: wrong argc num!");
			ret = -1;
			break;
		}
		memset(&ant, 0, sizeof(ant));
		ant.num_ant_ids = str_to_int(argv[1]);
		if (argc > 2) {
			for (int i = 0; (i < argc - 2) && (i < RTK_BLE_CTE_NUM_ANT_ID_MAX); ++i) {
				ant.ant_ids[i] = str_to_int(argv[i + 2]);
			}
		}
		cte_srv_set_params(param_type, sizeof(ant), &ant);
		break;
	}
	default: {
		AT_PRINTK("[ATBC] GATTS CTE set param type %u, not supported", param_type);
		ret = -1;
		break;
	}
	}

	return ret;
}
#endif /* RTK_BLE_5_1_CTE_SUPPORT */

#endif

static const cmd_table_t gatts_cmd_table[] = {
	{"notify",      atcmd_bt_gatts_notify,      6, 7},
	{"indicate",    atcmd_bt_gatts_indicate,    6, 7},
#if ((defined(CONFIG_BT_PERIPHERAL) && CONFIG_BT_PERIPHERAL) || \
    (defined(CONFIG_BT_SCATTERNET) && CONFIG_BT_SCATTERNET))
	{"loop_send",   atcmd_bt_gatts_loop_send,   2, 4},
#if defined(RTK_BLE_5_1_CTE_SUPPORT) && RTK_BLE_5_1_CTE_SUPPORT
	{"cte_set_param",   atcmd_bt_gatts_cte_set_param,   3, 15},
#endif
#endif
	{NULL,},
};

int atcmd_bt_gatts(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, gatts_cmd_table, "[ATBC][gatts]");
	return 0;
}