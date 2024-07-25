/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_datatrans_model.h>
#include <rtk_bt_mesh_performence_test.h>

bool mesh_performence_flag = false;
static int atcmd_ble_mesh_performence_request(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	if (mesh_performence_flag) {
		uint16_t ret = 0;
		rtk_bt_mesh_datatrans_client_read_api_t data_read;
		data_read.dst = str_to_int(argv[0]);
		data_read.read_len = 3;
		data_read.app_key_index = 0;
		ret = rtk_bt_mesh_datatrans_model_read(&data_read);
		if (ret) {
			AT_PRINTK("[%s] Datatrans model read failed! reason: 0x%x", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		AT_PRINTK("Current example cannot use this atcmd!!! \r\n");
		return 0;
	}
#else
	(void)argv;
	AT_PRINTK("[ATBC] Platform not support datatrans model.");
	return -1;
#endif
}

uint16_t trx_num = 0;
uint32_t trx_interval = 0;
bool trx_ack = 0;
rtk_bt_mesh_datatrans_client_write_api_t data_write_trx;
uint8_t op_store = 0;
uint16_t total_msg_num;
static void *datatrans_trx_timer = NULL;
uint16_t fail_count = 0;
void datatrans_trx_handler(void *FunctionContext)
{
	(void) FunctionContext;
	uint16_t ret = 0;
	if (trx_num == total_msg_num) {
		AT_PRINTK("Send Start \r\n");
		fail_count = 0;
	}
	if (trx_num > 0) {
		trx_num--;
		if (trx_num == 0) {
			data_write_trx.data[0] = RTK_BT_MESH_PERFORMENCE_TEST_END;
		} else {
			data_write_trx.data[0] = RTK_BT_MESH_PERFORMENCE_TEST_SENDING;
		}

		ret = rtk_bt_mesh_datatrans_model_write(&data_write_trx);
		if (ret) {
			fail_count++;
			AT_PRINTK("Provisioner Send fail!!!,reason: %d,fail count %d \r\n", ret, fail_count);
		} else {
			AT_PRINTK("Provisioner Send success \r\n");
		}
	} else {
		AT_PRINTK("Provisioner Send complete \r\n");
		if (fail_count > 0) {
			AT_PRINTK("Send fail count %d\r\n", fail_count);
		}
		osif_timer_stop(&datatrans_trx_timer);
	}
}


static int atcmd_ble_mesh_datatrnas_trx(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	if (mesh_performence_flag) {
		uint16_t ret = 0;
		data_write_trx.dst = str_to_int(argv[0]);
		data_write_trx.data_len = str_to_int(argv[1]);
		if (data_write_trx.data_len < 2) {
			AT_PRINTK("Input data length need above 2!!! \r\n");
			return -1;
		}
		bool switch_trx_timer = str_to_int(argv[2]);
		trx_interval = str_to_int(argv[3]);
		trx_num = str_to_int(argv[4]);
		total_msg_num = str_to_int(argv[4]);
		data_write_trx.app_key_index = str_to_int(argv[5]);
		data_write_trx.ack = str_to_int(argv[6]);
		if (switch_trx_timer) {
			if (datatrans_trx_timer == NULL) {
				osif_timer_create(&datatrans_trx_timer, "datatrans_trx_timer", 0, 0xffffffff, 1, datatrans_trx_handler);
				osif_timer_restart(&datatrans_trx_timer, trx_interval);
			} else {
				osif_timer_restart(&datatrans_trx_timer, trx_interval);
			}
		} else {
			if (datatrans_trx_timer != NULL) {
				osif_timer_delete(datatrans_trx_timer);
				datatrans_trx_timer = NULL;
			}
		}
		return ret;
	} else {
		AT_PRINTK("Current example cannot use this atcmd!!! \r\n");
		return 0;
	}
#else
	(void)argv;
	AT_PRINTK("[ATBC] Platform not support datatrans model.");
	return -1;
#endif
}

static int atcmd_ble_mesh_performence_cmd(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	if (mesh_performence_flag) {
		rtk_bt_mesh_datatrans_client_write_api_t data_write;
		data_write.dst = str_to_int(argv[0]);
		op_store = str_to_int(argv[1]);
		data_write.data_len = 1;
		data_write.data[0] = op_store;
		data_write.app_key_index = 0;
		data_write.ack = 0;
		rtk_bt_mesh_datatrans_model_write(&data_write);
	} else {
		AT_PRINTK("Current example cannot use this atcmd!!! \r\n");
	}
#else
	(void)argv;
	AT_PRINTK("[ATBC] Platform not support datatrans model.");
	return -1;
#endif
	return 0;
}


static const cmd_table_t mesh_performence_test_cmd_table[] = {
	{"cmd",    atcmd_ble_mesh_performence_cmd,    3, 3},
	{"start",    atcmd_ble_mesh_datatrnas_trx,    8, 8},
	{"result",    atcmd_ble_mesh_performence_request,    2, 2},
	{NULL,},
};

int atcmd_bt_mesh_performence_test(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, mesh_performence_test_cmd_table, "[ATBE][provisioner_test]");
	return 0;
}
#endif // end of RTK_BLE_MESH_SUPPORT