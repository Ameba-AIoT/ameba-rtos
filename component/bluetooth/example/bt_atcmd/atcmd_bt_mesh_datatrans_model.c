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

static int atcmd_ble_mesh_datatrnas_write(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	uint16_t ret = 0;
	uint16_t len;
	rtk_bt_mesh_datatrans_client_write_api_t data_write;
	data_write.dst = str_to_int(argv[0]);
	len = strlen(argv[1]);
	if (len / 2 > DATA_TRANS_DATA_MAX_LEN) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, DATA_TRANS_DATA_MAX_LEN);
		return -2;
	}
	data_write.data_len = len / 2;
	if (hexdata_str_to_array(argv[1], data_write.data, len)) {
		data_write.app_key_index = str_to_int(argv[2]);
		data_write.ack = str_to_int(argv[3]);
		ret = rtk_bt_mesh_datatrans_model_write(&data_write);
		if (ret) {
			BT_LOGE("[%s] Datatrans model write failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support datatrans model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_datatrans_read(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_datatrans_client_read_api_t data_read;
	data_read.dst = str_to_int(argv[0]);
	data_read.read_len = str_to_int(argv[1]);
	data_read.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_datatrans_model_read(&data_read);
	if (ret) {
		BT_LOGE("[%s] Datatrans model read failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support datatrans model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_datatrans_model_cmd_table[] = {
	{"write",    atcmd_ble_mesh_datatrnas_write,    5, 5},
	{"read",    atcmd_ble_mesh_datatrans_read,    4, 4},
	{NULL,},
};

int atcmd_bt_mesh_datatrans_model(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_datatrans_model_cmd_table, "[AT+BLEMESHDATA]");
}
#endif // end of RTK_BLE_MESH_SUPPORT
