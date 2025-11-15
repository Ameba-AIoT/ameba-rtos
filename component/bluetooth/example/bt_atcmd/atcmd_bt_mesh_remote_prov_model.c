/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>

#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_remote_prov_model.h>

static int atcmd_ble_mesh_remote_prov_client_scan_start(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret;
	rtk_bt_mesh_remote_prov_client_scan_start_t rmt_scan;
	memset(&rmt_scan, 0, sizeof(rtk_bt_mesh_remote_prov_client_scan_start_t));
	rmt_scan.dst = str_to_int(argv[0]);
	rmt_scan.net_key_index = str_to_int(argv[1]);
	rmt_scan.scanned_items_limit = str_to_int(argv[2]);
	rmt_scan.scan_timeout = str_to_int(argv[3]);
	if (5 == argc) {
		if (!hexdata_str_to_array(argv[4], rmt_scan.uuid, 16)) {
			BT_LOGE("[%s] UUID String to hex fail\r\n", __func__);
			return -2;
		}
	}
	ret = rtk_bt_mesh_remote_prov_scan_start(&rmt_scan);
	if (RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS != ret) {
		BT_LOGE("[%s] Remote prov start scan fail,reason:%d\r\n", __func__, ret);
		return ret;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_scan_capa_get(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret;
	rtk_bt_mesh_remote_prov_client_scan_capa_get_t scan_capa_get;
	scan_capa_get.dst = str_to_int(argv[0]);
	scan_capa_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_remote_prov_scan_capa_get(&scan_capa_get);
	if (ret) {
		BT_LOGE("[%s] Remote prov scan capa get fail, reason:%d\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_link_open(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret;
	rtk_bt_mesh_remote_prov_client_link_open_t link_open;
	link_open.dst = str_to_int(argv[0]);
	link_open.net_key_index = str_to_int(argv[1]);
	if (hexdata_str_to_array(argv[2], link_open.uuid, 16)) {
		link_open.link_open_timeout = str_to_int(argv[3]);
		ret = rtk_bt_mesh_remote_prov_link_open(&link_open);
		if (ret) {
			BT_LOGE("[%s] Remote prov link open fail! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] String to hex fail\r\n", __func__);
		return -2;
	}
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_link_open_dkri(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret;
	rtk_bt_mesh_remote_prov_client_link_open_dkri_t link_open_dkri;
	link_open_dkri.dst = str_to_int(argv[0]);
	link_open_dkri.net_key_index = str_to_int(argv[1]);
	link_open_dkri.dkri_procedure = str_to_int(argv[2]);
	ret = rtk_bt_mesh_remote_prov_link_open_dkri(&link_open_dkri);
	if (ret) {
		BT_LOGE("[%s] Remote prov link open dkri fail, reason:%d\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_dev_key_refresh(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret = 0;
	uint8_t attn_dur = 0;
	switch (argc) {
	case 1:
		attn_dur = str_to_int(argv[0]);
		break;
	default:
		break;
	}
	ret = rtk_bt_mesh_remote_prov_dev_key_refresh(&attn_dur);
	if (ret) {
		BT_LOGE("[%s] Remote prov device key refresh fail, reason:%d\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_node_addr_refresh(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret = 0;
	rtk_bt_mesh_remote_prov_client_node_addr_refresh_t node = {0};
	node.node_addr = str_to_int(argv[0]);
	if (argc > 1) {
		node.attn_dur = str_to_int(argv[1]);
	}
	ret = rtk_bt_mesh_remote_prov_node_addr_refresh(&node);
	if (ret) {
		BT_LOGE("[%s] Remote prov node addr refresh fail, reason:%d\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_client_composition_refresh(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_CLIENT_MODEL
	uint16_t ret = 0;
	uint8_t attn_dur = 0;
	switch (argc) {
	case 1:
		attn_dur = str_to_int(argv[0]);
		break;
	default:
		break;
	}
	ret = rtk_bt_mesh_remote_prov_composition_refresh(&attn_dur);
	if (ret) {
		BT_LOGE("[%s] Remote prov composition refresh fail, reason:%d\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_remote_prov_server_set_prefer_bearer(int argc, char **argv)
{
	(void)argc;
#if defined(BT_MESH_ENABLE_REMOTE_PROVISIONING_SERVER_MODEL) && BT_MESH_ENABLE_REMOTE_PROVISIONING_SERVER_MODEL
	uint16_t ret;
	rtk_bt_mesh_bearer_field_t bearer;
	bearer = str_to_int(argv[0]);
	ret = rtk_bt_mesh_remote_prov_set_prefer_bearer(&bearer);
	if (RTK_BT_MESH_MSG_SEND_CAUSE_SUCCESS != ret) {
		BT_LOGE("[%s] Remote prov set prefer bearer fail,reason:%d\r\n", __func__, ret);
		return ret;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support remote provisioning server model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_remote_prov_client_model_cmd_table[] = {
	{"scan_start",        atcmd_ble_mesh_remote_prov_client_scan_start,         5, 6},
	{"scan_capa_get",     atcmd_ble_mesh_remote_prov_client_scan_capa_get,      3, 3},
	{"link_open",         atcmd_ble_mesh_remote_prov_client_link_open,          5, 5},
	{"link_open_dkri",    atcmd_ble_mesh_remote_prov_client_link_open_dkri,     4, 4},
	{"dev_key_refresh",   atcmd_ble_mesh_remote_prov_client_dev_key_refresh,    1, 2},
	{"addr_refresh",      atcmd_ble_mesh_remote_prov_client_node_addr_refresh,  2, 3},
	{"compo_refresh",     atcmd_ble_mesh_remote_prov_client_composition_refresh, 1, 2},
	{"set_bearer",        atcmd_ble_mesh_remote_prov_server_set_prefer_bearer, 2, 2},
	{NULL,},
};

int atcmd_bt_mesh_remote_prov_client_model(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_remote_prov_client_model_cmd_table, "[AT+BLEMESHRMT]");
}

#endif // end of RTK_BLE_MESH_SUPPORT
