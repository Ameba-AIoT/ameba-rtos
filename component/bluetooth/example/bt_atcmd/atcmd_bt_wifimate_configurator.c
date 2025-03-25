/*
 *******************************************************************************
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <atcmd_service.h>
#include <atcmd_bt_impl.h>
#include <ble_wifimate_service.h>
#include <bt_utils.h>


static int atcmd_bt_wifimate_configurator_wifi_scan(int argc, char **argv)
{
	uint16_t conn_handle;

	if (argc < 1) {
		BT_LOGE("[AT+BTDEMO] bLE wifimate configurator wifi scan param invalid\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

	conn_handle = (uint16_t)str_to_int(argv[0]);

	return ble_wifimate_client_wifi_scan(conn_handle);
}

static int atcmd_bt_wifimate_configurator_wifi_connect(int argc, char **argv)
{
	uint16_t conn_handle;
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

	return ble_wifimate_client_wifi_connect(conn_handle, &conn_info);
}

static const cmd_table_t ble_wifimate_configrator_cmd_table[] = {
	{"wifi_scan",     atcmd_bt_wifimate_configurator_wifi_scan,       2, 2},
	{"wifi_connect",  atcmd_bt_wifimate_configurator_wifi_connect,    4, 5},
	{NULL,},
};

int atcmd_bt_wifimate_configurator_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, ble_wifimate_configrator_cmd_table, "[AT+BTDEMO=ble_wifimate_configrator]");
}