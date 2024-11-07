/*
 *******************************************************************************
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <atcmd_service.h>
#include <atcmd_bt_impl.h>
#include <bt_utils.h>
#include <transfer_module_common.h>


static int atcmd_bt_transfer_module_get_uuid(int argc, char **argv)
{
	(void)argc;
	uint8_t attr = 0;
	uint16_t uuid = 0;
	uint16_t ret = 0;

	attr = (uint8_t)str_to_int(argv[0]);
	ret = ble_transfer_module_get_uuid(attr, &uuid);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module get uuid failed, err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("[AT+BTDEMO] Transfer module get uuid attr=%d uuid=0x%04x\r\n", attr, uuid);
	BT_AT_PRINT("+BTDEMO:transfer_module,get_uuid,%d,0x%04x\r\n", attr, uuid);
	return 0;
}

static int atcmd_bt_transfer_module_set_uuid(int argc, char **argv)
{
	(void)argc;
	uint8_t attr = 0;
	uint16_t uuid = 0;
	uint16_t ret = 0;

	attr = (uint8_t)str_to_int(argv[0]);
	uuid = (uint16_t)str_to_int(argv[1]);
	ret = ble_transfer_module_set_uuid(attr, uuid);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module set uuid failed, err: 0x%x\r\n", ret);
		return -1;
	}
	return 0;
}

static int atcmd_bt_transfer_module_read_val(int argc, char **argv)
{
	(void)argc;
	uint8_t  len = 0;
	uint8_t *data = NULL;
	uint16_t ret = 0;

	len = (uint8_t)str_to_int(argv[0]);

	if (len > TRANSFER_MODULE_READ_VAL_MAX_LEN) {
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed: wrong arg length!\r\n");
		return -1;
	}

	if (len != strlen(argv[1]) / 2) {
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed: length unmatch with data!\r\n");
		return -1;
	}
	data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	if (!data) {
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed: cant alloc memory\r\n");
		return -1;
	}

	hexdata_str_to_array(argv[1], (uint8_t *)data, len);

	ret = ble_transfer_module_update_read_val(len, data);
	if (ret) {
		osif_mem_free((void *)data);
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed! err: 0x%x\r\n", ret);
		return -1;
	}

	osif_mem_free((void *)data);
	return 0;
}

static int atcmd_bt_transfer_module_get_name(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	uint8_t buf_len = TRANSFER_MODULE_DEV_NAME_MAX_LEN;
	char buf[TRANSFER_MODULE_DEV_NAME_MAX_LEN];

	ret = ble_transfer_module_get_dev_name(buf_len, buf);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module get device name failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("[AT+BTDEMO] Transfer module get device name: %s\r\n", buf);
	BT_AT_PRINT("+BTDEMO:transfer_module,get_name,%s\r\n", buf);
	return 0;
}

static int atcmd_bt_transfer_module_set_name(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;

	if (strlen(argv[0]) >= TRANSFER_MODULE_DEV_NAME_MAX_LEN) {
		BT_LOGE("[AT+BTDEMO] Transfer module set device name string too long\r\n");
		return -1;
	}

	ret = ble_transfer_module_set_dev_name(argv[0]);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module set device name failed! err: 0x%x\r\n", ret);
		return -1;
	}

	return 0;
}

static const cmd_table_t transfer_module_cmd_table[] = {
	{"get_uuid",    atcmd_bt_transfer_module_get_uuid,    2, 2},
	{"set_uuid",    atcmd_bt_transfer_module_set_uuid,    3, 3},
	{"read_val",    atcmd_bt_transfer_module_read_val,    3, 3},
	{"get_name",    atcmd_bt_transfer_module_get_name,    1, 1},
	{"set_name",    atcmd_bt_transfer_module_set_name,    2, 2},
	{NULL,},
};

int atcmd_bt_transfer_module_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, transfer_module_cmd_table, "[AT+BTDEMO=transfer_module]");
}