/*
 *******************************************************************************
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <atcmd_service.h>
#include <atcmd_bt_impl.h>
#include <bt_utils.h>
#include <rtk_bt_device.h>
#include <transfer_module_common.h>
#include <rtk_service_config.h>

static struct transfer_module_srv_t *s_add_srv = NULL;

extern uint8_t transfer_module_is_enable(void);

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
		return bt_at_rtk_err_to_at_err(ret);
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
		return bt_at_rtk_err_to_at_err(ret);
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
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (len != strlen(argv[1]) / 2) {
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed: length unmatch with data!\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}
	data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	if (!data) {
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed: cant alloc memory\r\n");
		return BT_AT_ERR_NO_MEMORY;
	}

	hexdata_str_to_array(argv[1], (uint8_t *)data, len);

	ret = ble_transfer_module_char_read_val_set(TRANSFER_MODULE_SRV_ID, TRANSFER_MODULE_READ_VAL_INDEX, len, data);
	if (ret) {
		osif_mem_free((void *)data);
		BT_LOGE("[AT+BTDEMO] Transfer module set read value failed! err: 0x%x\r\n", ret);
		return bt_at_rtk_err_to_at_err(ret);
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
		return bt_at_rtk_err_to_at_err(ret);
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
		return BT_AT_ERR_PARAM_INVALID;
	}

	ret = ble_transfer_module_set_dev_name(argv[0]);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module set device name failed! err: 0x%x\r\n", ret);
		return bt_at_rtk_err_to_at_err(ret);
	}

	return 0;
}

static int atcmd_bt_transfer_module_srv_add(int argc, char **argv)
{
	uint16_t uuid;

	if ((argc != 1) || (argv == NULL)) {
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (transfer_module_is_enable()) {
		BT_LOGE("[APP] Transfer module srv add fail, please add service before bt init\r\n");
		return BT_AT_FAIL;
	}

	uuid = (uint16_t)str_to_int(argv[0]);
	if (!ble_transfer_module_srv_add_check(uuid)) {
		BT_LOGE("[AT+BTDEMO] Transfer module srv add check fail\r\n");
		return BT_AT_FAIL;
	}

	// memory alloc for s_add_srv
	if (!s_add_srv) {
		s_add_srv = (struct transfer_module_srv_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct transfer_module_srv_t));
		if (!s_add_srv) {
			BT_LOGE("[AT+BTDEMO] Transfer module srv add failed: cant alloc memory\r\n");
			return BT_AT_ERR_NO_MEMORY;
		}
	}

	memset(s_add_srv, 0, sizeof(s_add_srv));
	s_add_srv->srv_uuid = uuid;

	return BT_AT_OK;
}

static int atcmd_bt_transfer_module_char_add(int argc, char **argv)
{
	if (argc != 2 || !argv) {
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (transfer_module_is_enable()) {
		BT_LOGE("[APP] Transfer module char add fail, please add characteristic before bt init\r\n");
		return BT_AT_FAIL;
	}

	if (!s_add_srv) {
		BT_LOGE("[AT+BTDEMO] Transfer module char add failed: please add service first\r\n");
		return BT_AT_ERR_NO_SRV_ADD;
	}

	if (s_add_srv->char_num >= TRANSFER_MODULE_USER_DEFINED_CHAR_MAX_NUM) {
		BT_LOGE("[AT+BTDEMO] Transfer module char add exceed maximum number\r\n");
		return BT_AT_ERR_CHAR_ADD_EXCEED_MAX_NUM;
	}

	s_add_srv->chars[s_add_srv->char_num].char_uuid = (uint16_t)str_to_int(argv[0]);
	s_add_srv->chars[s_add_srv->char_num].char_property = (uint16_t)str_to_int(argv[1]);
	++s_add_srv->char_num;

	return 0;
}

static int atcmd_bt_transfer_module_srv_end(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret;

	if (transfer_module_is_enable()) {
		BT_LOGE("[APP] Transfer module srv add fail, please add service before bt init\r\n");
		return BT_AT_FAIL;
	}

	if (!s_add_srv) {
		BT_LOGE("[AT+BTDEMO] Transfer module srv end failed: please add service first\r\n");
		return BT_AT_ERR_NO_SRV_ADD;
	}

	ret = ble_transfer_module_srv_add(s_add_srv);

	osif_mem_free(s_add_srv);
	s_add_srv = NULL;

	return bt_at_rtk_err_to_at_err(ret);
}

static int atcmd_bt_transfer_module_srv_del(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (transfer_module_is_enable()) {
		BT_LOGE("[APP] Transfer module srv delete fail, please delete service before bt init or after bt deinit\r\n");
		return BT_AT_FAIL;
	}

	if (s_add_srv) {
		osif_mem_free(s_add_srv);
		s_add_srv = NULL;
	}

	ble_transfer_module_srv_del();
	return BT_AT_OK;
}

static int atcmd_bt_transfer_module_char_read_val(int argc, char **argv)
{
	uint16_t ret;
	uint16_t app_id;
	uint16_t char_index;
	uint8_t  len;
	uint8_t *data = NULL;

	if ((argc != 4) || !argv) {
		BT_LOGE("[AT+BTDEMO] Transfer module char read value set: invalid parameter\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (transfer_module_is_enable()) {
		BT_LOGE("[AT+BTDEMO] Transfer module char read value set fail, please set read value before bt init\r\n");
		return BT_AT_FAIL;
	}
	app_id = (uint16_t)str_to_int(argv[0]);
	char_index = (uint16_t)str_to_int(argv[1]);
	len = (uint8_t)str_to_int(argv[2]);

	if (len > TRANSFER_MODULE_READ_VAL_MAX_LEN) {
		BT_LOGE("[AT+BTDEMO] Transfer module char read value failed: wrong arg length!\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (len != strlen(argv[3]) / 2) {
		BT_LOGE("[AT+BTDEMO] Transfer module char read value failed: length unmatch with data!\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}
	data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	if (!data) {
		BT_LOGE("[AT+BTDEMO] Transfer module char read value failed: cant alloc memory\r\n");
		return BT_AT_ERR_NO_MEMORY;
	}

	hexdata_str_to_array(argv[3], (uint8_t *)data, len);

	ret = ble_transfer_module_char_read_val_set(app_id, char_index, len, data);
	if (ret) {
		ret = bt_at_rtk_err_to_at_err(ret);
		BT_LOGE("[AT+BTDEMO] Transfer module char read value set fail, ret=%d\r\n", ret);
	}

	osif_mem_free(data);
	return ret;
}

static const cmd_table_t transfer_module_cmd_table[] = {
	{"get_uuid",      atcmd_bt_transfer_module_get_uuid,          2, 2},
	{"set_uuid",      atcmd_bt_transfer_module_set_uuid,          3, 3},
	{"read_val",      atcmd_bt_transfer_module_read_val,          3, 3},
	{"get_name",      atcmd_bt_transfer_module_get_name,          1, 1},
	{"set_name",      atcmd_bt_transfer_module_set_name,          2, 2},
	{"srv_add",       atcmd_bt_transfer_module_srv_add,           2, 2},
	{"char_add",      atcmd_bt_transfer_module_char_add,          3, 3},
	{"srv_end",       atcmd_bt_transfer_module_srv_end,           1, 1},
	{"srv_del",       atcmd_bt_transfer_module_srv_del,           1, 1},
	{"char_read_val", atcmd_bt_transfer_module_char_read_val,     5, 5},
	{NULL,},
};

int atcmd_bt_transfer_module_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, transfer_module_cmd_table, "[AT+BTDEMO=transfer_module]");
}