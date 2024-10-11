/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <osif.h>

#include "platform_autoconf.h"
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <../bt_hid/gamepad_des.h>
#include <../bt_hid/mouse_des.h>
#include <rtk_bt_hid.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_hid_disconnect(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hid_disconnect(bd_addr)) {
		BT_LOGE("HID disconnect fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HID disconnecting to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hid_get_report_rsp(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_hid_get_report_rsp(bd_addr)) {
		BT_LOGE("HID get report response fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HID get report response to device %s ...\r\n", addr_str);

	return 0;
}

static int atcmd_bt_hid_data_send(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_hid_input_data_t data_t = {0};

	hexdata_str_to_bd_addr(argv[0], data_t.bd_addr, RTK_BD_ADDR_LEN);
	for (uint8_t i = 0; i < (argc - 1); i ++) {
		data_t.data[i] = (uint8_t)str_to_int(argv[i + 1]);
	}
	if (rtk_bt_hid_input_data_send(data_t.bd_addr, data_t.data, data_t.len)) {
		BT_LOGE("HID data send fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(data_t.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HID data send op to device %s ...\r\n", addr_str);

	return 0;
}

#if defined(CONFIG_BT_HID) && CONFIG_BT_HID
uint16_t rtk_bt_hid_mouse_control(rtk_bt_hid_mouse_report_t *pmouse_t);
static int atcmd_bt_hid_mouse(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_hid_mouse_report_t report = {0};

	hexdata_str_to_bd_addr(argv[0], report.bd_addr, RTK_BD_ADDR_LEN);
	report.ReportID = (uint8_t)str_to_int(argv[1]);
	report.mse_data.Button = (uint8_t)str_to_int(argv[2]);
	report.mse_data.LocationX = (int8_t)str_to_int(argv[3]);
	report.mse_data.LocationY = (int8_t)str_to_int(argv[4]);
	report.mse_data.Wheel = (int8_t)str_to_int(argv[5]);

	if (rtk_bt_hid_mouse_control(&report)) {
		BT_LOGE("HID mouse fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(report.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HID mouse op to device %s ...\r\n", addr_str);

	return 0;
}

uint16_t rtk_bt_hid_gamepad_control(rtk_bt_hid_gamepad_report_t *pgamepad_t);
static int atcmd_bt_hid_gamepad(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	rtk_bt_hid_gamepad_report_t report = {0};

	hexdata_str_to_bd_addr(argv[0], report.bd_addr, RTK_BD_ADDR_LEN);
	report.ReportID = (uint8_t)str_to_int(argv[1]);
	report.gpd_data.LeftThumbstickX = (int16_t)str_to_int(argv[2]);
	report.gpd_data.LeftThumbstickY = (int16_t)str_to_int(argv[3]);
	report.gpd_data.RightThumbstickX = (int16_t)str_to_int(argv[4]);
	report.gpd_data.RightThumbstickY = (int16_t)str_to_int(argv[5]);
	report.gpd_data.LeftTrigger = (uint16_t)str_to_int(argv[6]);
	report.gpd_data.RightTrigger = (uint16_t)str_to_int(argv[7]);
	report.gpd_data.DPadButtons = (uint8_t)str_to_int(argv[8]);
	report.gpd_data.DigitalButtons.Value16 = (uint16_t)str_to_int(argv[9]);
	report.gpd_data.ShareButton = (uint8_t)str_to_int(argv[10]);
	if (rtk_bt_hid_gamepad_control(&report)) {
		BT_LOGE("HID gamepad fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(report.bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HID gamepad op to device %s ...\r\n", addr_str);

	return 0;
}
#endif

static const cmd_table_t hid_cmd_table[] = {
	{"disconn",             atcmd_bt_hid_disconnect,             2, 2},
	{"get_report_rsp",      atcmd_bt_hid_get_report_rsp,         2, 2},
	{"data_send",           atcmd_bt_hid_data_send,              2, 22},
#if defined(CONFIG_BT_HID) && CONFIG_BT_HID
	{"mouse",               atcmd_bt_hid_mouse,                  7, 7},
	{"gamepad",             atcmd_bt_hid_gamepad,                12, 12},
#endif
	{NULL,},
};

int atcmd_bt_hid_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, hid_cmd_table, "[AT+BTHID]");
}