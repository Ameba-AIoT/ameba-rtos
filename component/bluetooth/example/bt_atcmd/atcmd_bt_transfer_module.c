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
#include <rtk_bt_att_defs.h>
#include <ctype.h>

static struct transfer_module_srv_t *s_add_srv = NULL;

extern uint8_t transfer_module_is_enable(void);

static int uuid_string_to_little_endian(const char *uuid_str, uint8_t *uuid_type, uint8_t uuid[16])
{
	char hex_str[33] = {0};
	int hi, lo;
	int j = 0;
	int len = 0;
	int i = 0;
	int str_len = 0;

	if (!uuid_str || !uuid_type || !uuid) {
		BT_LOGE("[%s] invalid input param\r\n", __func__);
		return BT_AT_ERR_PARAM_INVALID;
	}
	str_len = strlen(uuid_str);
	// skip the leading '0x' or '0X' in uuid_str
	if (!strncmp(uuid_str, "0x", 2) || !strncmp(uuid_str, "0X", 2)) {
		i = 2;
	}
	// remove '-' in uuid_str
	for (; i < str_len && uuid_str[i] && j < 32; i++) {
		if (uuid_str[i] != '-') {
			if (!isxdigit((unsigned char)uuid_str[i])) {
				// not hex
				BT_LOGE("[%s] uuid string not hex\r\n", __func__);
				return BT_AT_ERR_PARAM_INVALID;
			}
			hex_str[j++] = uuid_str[i];
		}
	}

	if (i < str_len) {
		BT_LOGE("[%s] uuid string length wrong\r\n", __func__);
		return BT_AT_ERR_PARAM_INVALID;
	}

	len = j / 2;
	if (len == TRANSFER_MODULE_UUID16_LEN) {
		*uuid_type = BT_UUID_TYPE_16;
	} else if (len == TRANSFER_MODULE_UUID128_LEN) {
		*uuid_type = BT_UUID_TYPE_128;
	} else {
		BT_LOGE("[%s] uuid string length wrong\r\n", __func__);
		return BT_AT_ERR_PARAM_INVALID;
	}

	for (i = 0; i < len; i++) {
		hi = hex_str[2 * (len - 1 - i)];
		lo = hex_str[2 * (len - 1 - i) + 1];
		if (isupper(hi)) {
			hi = tolower(hi);
		}
		if (isupper(lo)) {
			lo = tolower(lo);
		}

		uint8_t val = ((hi >= 'a') ? hi - 'a' + 10 : hi - '0') * 16
					  + ((lo >= 'a') ? lo - 'a' + 10 : lo - '0');
		uuid[i] = val;
	}
	return BT_AT_OK;
}

static int atcmd_bt_transfer_module_get_uuid(int argc, char **argv)
{
	(void)argc;
	uint8_t attr = 0;
	uint8_t uuid_type = 0;
	uint8_t uuid[TRANSFER_MODULE_UUID128_LEN] = {0};
	uint16_t ret = 0;

	attr = (uint8_t)str_to_int(argv[0]);
	ret = ble_transfer_module_get_uuid(attr, &uuid_type, uuid);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module get uuid failed, err: 0x%x\r\n", ret);
		return bt_at_rtk_err_to_at_err(ret);
	}

	if (uuid_type == BT_UUID_TYPE_16) {
		uint16_t uuid16 = uuid[0] | (uuid[1] << 8);
		BT_LOGA("[AT+BTDEMO] Transfer module get uuid attr=%d uuid=0x%04x\r\n", attr, uuid16);
		BT_AT_PRINT("+BTDEMO:transfer_module,get_uuid,%d,0x%04x\r\n", attr, uuid16);
	} else if (uuid_type == BT_UUID_TYPE_128) {
		// 128-bit UUID format: 8-4-4-4-12
		BT_LOGA("[AT+BTDEMO] Transfer module get uuid attr=%d uuid=%02x%02x%02x%02x-",
				attr, uuid[15], uuid[14], uuid[13], uuid[12]);
		BT_LOGA("%02x%02x-", uuid[11], uuid[10]);
		BT_LOGA("%02x%02x-", uuid[9], uuid[8]);
		BT_LOGA("%02x%02x-", uuid[7], uuid[6]);
		BT_LOGA("%02x%02x%02x%02x%02x%02x\n",
				uuid[5], uuid[4], uuid[3], uuid[2], uuid[1], uuid[0]);

		BT_AT_PRINT("+BTDEMO:transfer_module,get_uuid,%d,%02x%02x%02x%02x-",
					attr, uuid[15], uuid[14], uuid[13], uuid[12]);
		BT_AT_PRINT("%02x%02x-", uuid[11], uuid[10]);
		BT_AT_PRINT("%02x%02x-", uuid[9], uuid[8]);
		BT_AT_PRINT("%02x%02x-", uuid[7], uuid[6]);
		BT_AT_PRINT("%02x%02x%02x%02x%02x%02x\n",
					uuid[5], uuid[4], uuid[3], uuid[2], uuid[1], uuid[0]);
	}
	return BT_AT_OK;
}

static int atcmd_bt_transfer_module_set_uuid(int argc, char **argv)
{
	(void)argc;
	uint8_t attr = 0;
	uint8_t uuid_type;
	uint8_t uuid[TRANSFER_MODULE_UUID128_LEN] = {0};
	int ret = 0;

	attr = (uint8_t)str_to_int(argv[0]);

	ret = uuid_string_to_little_endian(argv[1], &uuid_type, uuid);
	if (ret != BT_AT_OK) {
		return ret;
	}

	ret = ble_transfer_module_set_uuid(attr, uuid_type, uuid);
	if (ret) {
		BT_LOGE("[AT+BTDEMO] Transfer module set uuid failed, err: 0x%x\r\n", ret);
		return bt_at_rtk_err_to_at_err(ret);
	}
	return BT_AT_OK;
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
	uint8_t uuid_type;
	uint8_t uuid[TRANSFER_MODULE_UUID128_LEN] = {0};
	int ret;

	if ((argc != 1) || (argv == NULL)) {
		return BT_AT_ERR_PARAM_INVALID;
	}

	ret = uuid_string_to_little_endian(argv[0], &uuid_type, uuid);
	if (ret != BT_AT_OK) {
		return ret;
	}

	if (transfer_module_is_enable()) {
		BT_LOGE("[APP] Transfer module srv add fail, please add service before bt init\r\n");
		return BT_AT_FAIL;
	}

	if (!ble_transfer_module_srv_add_check(uuid_type, uuid)) {
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

	memset(s_add_srv, 0, sizeof(struct transfer_module_srv_t));
	s_add_srv->uuid_type = uuid_type;
	memcpy(s_add_srv->srv_uuid, uuid, TRANSFER_MODULE_UUID128_LEN);

	return BT_AT_OK;
}

static int atcmd_bt_transfer_module_char_add(int argc, char **argv)
{
	uint8_t uuid_type;
	uint8_t uuid[TRANSFER_MODULE_UUID128_LEN] = {0};
	int ret;

	if (argc != 2 || !argv) {
		return BT_AT_ERR_PARAM_INVALID;
	}

	ret = uuid_string_to_little_endian(argv[0], &uuid_type, uuid);
	if (ret != BT_AT_OK) {
		return ret;
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

	memset(&s_add_srv->chars[s_add_srv->char_num], 0, sizeof(s_add_srv->chars[s_add_srv->char_num]));
	s_add_srv->chars[s_add_srv->char_num].uuid_type = uuid_type;
	memcpy(s_add_srv->chars[s_add_srv->char_num].char_uuid, uuid, TRANSFER_MODULE_UUID128_LEN);
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