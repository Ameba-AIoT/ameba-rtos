/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <atcmd_service.h>
#include <atcmd_bt_impl.h>
#include <ble_wifimate_service.h>
#include <rtk_bt_def.h>
#include <bt_utils.h>
#if defined(BT_AT_SYNC) && BT_AT_SYNC
#include <atcmd_bt_cmd_sync.h>
#endif


static int atcmd_bt_wifimate_configurator_wifi_scan(int argc, char **argv)
{
	uint16_t conn_handle;
	int ret = 0;

	if (argc < 1) {
		BT_LOGE("[AT+BTDEMO] bLE wifimate configurator wifi scan param invalid\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	ret = bt_at_sync_init(BT_AT_SYNC_CMD_TYPE_BLE_GATTS_INDICATE, BT_AT_SYNC_OP_TYPE_NONE, conn_handle);
	if (ret != BT_AT_OK) {
		return ret;
	}
#endif

	conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = ble_wifimate_client_wifi_scan(conn_handle);
	if (ret != RTK_BT_OK) {
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		bt_at_sync_deinit();
#endif
		return bt_at_rtk_err_to_at_err(ret);
	}

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	ret = bt_at_sync_sem_take();
	if (ret == BT_AT_OK) {
		ret = bt_at_sync_get_result();
	}
	bt_at_sync_deinit();
#endif
	return ret;
}

static int atcmd_bt_wifimate_configurator_wifi_connect(int argc, char **argv)
{
	uint16_t conn_handle;
	int ret = 0;
	struct wifi_conn_config_t conn_info;

	if (argc < 3) {
		BT_LOGE("[AT+BTDEMO] BLE wifimate configurator wifi connect param invalid\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

	memset(&conn_info, 0, sizeof(conn_info));
	conn_handle = (uint16_t)str_to_int(argv[0]);
	conn_info.ssid_len = strlen(argv[1]);
	memcpy(conn_info.ssid, argv[1], conn_info.ssid_len);
	conn_info.security = (uint8_t)str_to_int(argv[2]);

	if (argc > 3) {
		conn_info.password_len = strlen(argv[3]);
		conn_info.password = (unsigned char *)(argv[3]);

		BT_LOGA("[AT+BTDEMO] BLE wifimate configurator conn_handle=%d, ssid_len=%d, ssid=%s, security=%d, password_len=%d, password=%s\r\n",
				conn_handle, conn_info.ssid_len, conn_info.ssid,
				conn_info.security, conn_info.password_len, conn_info.password);
	} else {
		BT_LOGA("[AT+BTDEMO] BLE wifimate configurator conn_handle=%d, ssid_len=%d, ssid=%s, security=%d, password_len=0, password=\r\n",
				conn_handle, conn_info.ssid_len, conn_info.ssid, conn_info.security);
	}
#if defined(BT_AT_SYNC) && BT_AT_SYNC
	ret = bt_at_sync_init(BT_AT_SYNC_CMD_TYPE_BLE_GATTS_INDICATE, BT_AT_SYNC_OP_TYPE_NONE, conn_handle);
	if (ret != BT_AT_OK) {
		return ret;
	}
#endif

	ret = ble_wifimate_client_wifi_connect(conn_handle, &conn_info);
	if (ret != RTK_BT_OK) {
#if defined(BT_AT_SYNC) && BT_AT_SYNC
		bt_at_sync_deinit();
#endif
		return bt_at_rtk_err_to_at_err(ret);
	}

#if defined(BT_AT_SYNC) && BT_AT_SYNC
	ret = bt_at_sync_sem_take();
	if (ret == BT_AT_OK) {
		ret = bt_at_sync_get_result();
	}
	bt_at_sync_deinit();
#endif
	return ret;
}

uint16_t ble_wifimate_configurator_encrypt_set(uint8_t type, uint8_t key[BLE_WIFIMATE_KEY_LEN]);
static int atcmd_bt_wifimate_configurator_encrypt_set(int argc, char **argv)
{
	uint8_t algo_type = 0;
	uint8_t key[BLE_WIFIMATE_KEY_LEN] = {0};
	int ret = 0;

	if (!argv) {
		return BT_AT_ERR_PARAM_INVALID;
	}

	algo_type = (uint8_t)str_to_int(argv[0]);
	if (algo_type !=  BLE_WIFIMATE_ENCRYPT_DECRYPT_ALGO_NONE) {
		if (argc == 2 && argv[1]) {
			uint8_t str_len = strlen(argv[1]);

			if (str_len != 2 * BLE_WIFIMATE_KEY_LEN) {
				BT_LOGA("[AT+BTDEMO] BLE wifimate configurator: Invalid key string\r\n");
				return BT_AT_ERR_PARAM_INVALID;
			}

			if (false == hexdata_str_to_array(argv[1], key, BLE_WIFIMATE_KEY_LEN)) {
				BT_LOGA("[AT+BTDEMO] BLE wifimate configurator encrypt key invalid\r\n");
				return BT_AT_ERR_PARAM_INVALID;
			}
		} else {
			BT_LOGA("[AT+BTDEMO] BLE wifimate configurator encrypt parameter invalid\r\n");
			return BT_AT_ERR_PARAM_INVALID;
		}
	}
	BT_LOGA("[AT+BTDEMO] BLE wifimate configurator encrypt set algo_type=%d \
				key=%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\r\n",
			algo_type, key[0], key[1], key[2], key[3], key[4], key[5], key[6],
			key[7], key[8], key[9], key[10], key[11], key[12]);

	ret = ble_wifimate_configurator_encrypt_set(algo_type, key);

	return bt_at_rtk_err_to_at_err(ret);
}

static const cmd_table_t ble_wifimate_configrator_cmd_table[] = {
	{"wifi_scan",     atcmd_bt_wifimate_configurator_wifi_scan,       2, 2},
	{"wifi_connect",  atcmd_bt_wifimate_configurator_wifi_connect,    4, 5},
	{"encrypt_set",   atcmd_bt_wifimate_configurator_encrypt_set,     2, 3},
	{NULL,},
};

int atcmd_bt_wifimate_configurator_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, ble_wifimate_configrator_cmd_table, "[AT+BTDEMO=ble_wifimate_configrator]");
}