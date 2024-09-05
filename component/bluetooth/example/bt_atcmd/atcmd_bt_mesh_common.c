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
#include <rtk_bt_mesh_common.h>

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
static rtk_bt_le_create_conn_param_t def_conn_param = {
	.peer_addr = {
		.type = (rtk_bt_le_addr_type_t)0,
		.addr_val = {0},
	},
	.scan_interval = 0x60,
	.scan_window = 0x30,
	.filter_policy = RTK_BT_LE_CONN_FILTER_WITHOUT_WHITELIST,
	.conn_interval_max = 0x60,
	.conn_interval_min = 0x60,
	.conn_latency      = 0,
	.supv_timeout      = 0x100,
	.scan_timeout      = 1000,
};
#endif

static int atcmd_ble_mesh_node_reset(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	ret = rtk_bt_mesh_stack_node_reset();
	if (ret) {
		BT_LOGE("[%s] Node reset oneself failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
}

static int atcmd_ble_mesh_dev_info(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rkt_bt_mesh_stack_act_device_info_set_t info_switch;
	info_switch.dev_info_dis = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_set_device_info_display(&info_switch);
	if (ret) {
		BT_LOGE("[%s] Set dev info switch failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
}

static int atcmd_ble_mesh_user_list_info(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	ret = rtk_bt_mesh_stack_user_list_info();
	if (ret) {
		BT_LOGE("[%s] Print device list info failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
}

static int atcmd_ble_mesh_set_random_value(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_set_random_value_t value;
	value.random = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_set_random_value_for_authentication(&value);
	if (ret) {
		BT_LOGE("[%s] Set random value failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
}

static int atcmd_ble_mesh_get_hb_subscribe_result(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t ret = 0;
	rtk_bt_mesh_hb_sub_t hb_sub = {0};
	ret = rtk_bt_mesh_stack_get_heartbeat_subscribe_result(&hb_sub);
	if (ret) {
		BT_LOGE("[%s] Get heartbeat subscribe result failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	} else {
		BT_LOGA("[%s] Src:0x%x, dst:0x%x, period:%d, count:%d, min_hops:%d, max_hops:%d\r\n", __func__,
				hb_sub.src, hb_sub.dst, hb_sub.period, hb_sub.count, hb_sub.min_hops, hb_sub.max_hops);
		return 0;
	}
}

static int atcmd_ble_mesh_set_model_subscribe(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_mesh_set_model_subscribe_t model_sub = {0};
	model_sub.element_index = str_to_int(argv[0]);
	model_sub.model_id = str_to_int(argv[1]);
	model_sub.sub_addr = str_to_int(argv[2]);
	ret = rtk_bt_mesh_stack_set_model_subscribe(&model_sub);
	if (ret) {
		BT_LOGE("[%s] Set model subscribe failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	} else {
		return 0;
	}
}

static int atcmd_ble_mesh_set_retrans_param(int argc, char **argv)
{
	(void)argc;
	uint16_t ret = 0;
	rtk_bt_mesh_stack_set_retrans_param_t param = {0};
	param.ttl = str_to_int(argv[0]);
	param.relay_retrans_count = str_to_int(argv[1]);
	param.net_retrans_count = str_to_int(argv[2]);
	param.trans_retrans_count = str_to_int(argv[3]);
	ret = rtk_bt_mesh_stack_retrans_param_set(&param);
	if (ret) {
		BT_LOGE("[%s] Set retransmit parameter failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	} else {
		return 0;
	}
}

static int atcmd_ble_mesh_fn_init(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_fn_init_t fn_init;
	fn_init.lpn_num = str_to_int(argv[0]);
	fn_init.queue_size = str_to_int(argv[1]);
	fn_init.rx_window = str_to_int(argv[2]);
	ret = rtk_bt_mesh_stack_fn_init(&fn_init);
	if (ret) {
		BT_LOGE("[%s] Friendship FN init failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support friend feature.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_fn_deinit(int argc, char **argv)
{
	(void)argc;
	(void)argv;
#if defined(RTK_BLE_MESH_FN_SUPPORT) && RTK_BLE_MESH_FN_SUPPORT
	uint16_t ret = 0;
	ret = rtk_bt_mesh_stack_fn_deinit();
	if (ret) {
		BT_LOGE("[%s] Friendship FN init failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Platform not support friend feature.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pbadvcon(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_pb_adv_con_t pbadvcon;
	if (hexdata_str_to_array(argv[0], pbadvcon.uuid, 16)) {
		ret = rtk_bt_mesh_stack_prov_pb_adv_con(&pbadvcon);
		if (ret) {
			BT_LOGE("[%s] Pbadvcon failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] String to hex fail\r\n", __func__);
		return -2;
	}
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pbadv_discon(int argc, char **argv)
{
	(void)argc;
	(void)argv;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	ret = rtk_bt_mesh_stack_prov_pb_adv_discon();
	if (ret) {
		BT_LOGE("[%s] Pbadv discon failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pb_gatt_con(int argc, char **argv)
{
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_pb_gatt_con_t gatt_con = {0};
	switch (argc) {
	case 0: {
		// For zephyr mesh stack, establish a BLE GATT link with any provisioned device for T/R mesh message use mesh proxy service
		gatt_con.param.is_prov_link = 0;
		gatt_con.param.u.net_idx = 0;
		break;
	}
	case 1: {
		// For zephyr mesh stack, use UUID to establish PB GATT link and start provisioning
		gatt_con.param.is_prov_link = 1;
		if (!hexdata_str_to_array(argv[0], gatt_con.param.u.uuid, 16)) {
			BT_LOGE("[%s] String to hex for UUID fail!\r\n", __func__);
			return -1;
		}
		break;
	}
	case 2: {
		// For RTK mesh stack, use BT address to establish GATT link
		memcpy(&gatt_con.conn_param, &def_conn_param, sizeof(rtk_bt_le_create_conn_param_t));
		gatt_con.conn_param.peer_addr.type = (rtk_bt_le_addr_type_t)str_to_int(argv[0]);
		if (false == hexdata_str_to_bd_addr(argv[1], gatt_con.conn_param.peer_addr.addr_val, RTK_BD_ADDR_LEN)) {
			return -2;
		}
		break;
	}
	default:
		break;
	}
	ret = rtk_bt_mesh_stack_prov_pb_gatt_con(&gatt_con);
	if (ret) {
		BT_LOGE("[%s] Establish pb gatt failed! reason: 0x%x\r\n", __func__, ret);
		return -2;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pb_gatt_discon(int argc, char **argv)
{
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_pb_gatt_discon_t gatt_discon = {0};
	switch (argc) {
	case 0:
		// For zephyr mesh stack, disconnect with the provisioned mesh device use net idx
		gatt_discon.param.is_prov_link = 0;
		gatt_discon.param.net_idx = 0;
		break;
	case 1:
		// For RTK mesh stack, disconnect a BLE GATT link use conn handle
		gatt_discon.conn_handle = str_to_int(argv[0]);
		break;
	default:
		break;
	}
	if ((ret = rtk_bt_mesh_stack_prov_pb_gatt_discon(&gatt_discon)) != 0) {
		BT_LOGE("Pb gatt disconnect failed! err: 0x%x\r\n", ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_start_provisioning(int argc, char **argv)
{
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_start_prov_t start_prov = {0};
	switch (argc) {
	case 1:
		start_prov.attn_dur = str_to_int(argv[0]);
		break;
	default:
		break;
	}
	ret = rtk_bt_mesh_stack_start_provisioning(&start_prov);
	if (ret) {
		BT_LOGE("[%s] Start provisioning failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_method_choose_for_prov(int argc, char **argv)
{
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_prov_start_t prov_choose = {0};
	switch (argc) {
	case 5:
		prov_choose.auth_action.oob_action = str_to_int(argv[3]);
		prov_choose.auth_size.oob_size = str_to_int(argv[4]);
		__attribute__((fallthrough));
	case 3:
		prov_choose.algorithm = str_to_int(argv[0]);
		prov_choose.public_key = str_to_int(argv[1]);
		prov_choose.auth_method = str_to_int(argv[2]);
		break;
	default:
		break;
	}
	ret = rtk_bt_mesh_stack_method_choose_for_provisioning(&prov_choose);
	if (ret) {
		BT_LOGE("[%s] Choose method for provisioning failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_prov_service_disvery(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_prov_dis_t prov_dis;
	prov_dis.conn_handle = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_prov_service_discovery(&prov_dis);
	if (ret) {
		BT_LOGE("[%s] Prov service discovery failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_prov_service_set_notify(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_prov_set_notify_t prov_notify;
	prov_notify.is_enable = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_prov_service_set_notify(&prov_notify);
	if (ret) {
		BT_LOGE("[%s] Set prov service notify failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_proxy_service_disvery(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_proxy_dis_t proxy_dis;
	proxy_dis.conn_handle = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_proxy_service_discovery(&proxy_dis);
	if (ret) {
		BT_LOGE("[%s] Proxy service discovery failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_proxy_service_set_notify(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_proxy_set_notify_t proxy_notify;
	proxy_notify.is_enable = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_proxy_service_set_notify(&proxy_notify);
	if (ret) {
		BT_LOGE("[%s] Set proxy service notify failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_init(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_lpn_init_t lpn_init;
	lpn_init.fn_num = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_lpn_init(&lpn_init);
	if (ret) {
		BT_LOGE("[%s] Low power node init failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not device role or not suppport low power feature, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_req(int argc, char **argv)
{
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_lpn_req_t lpn_request = {0};
	switch (argc) {
	case 0:
		// For zephyr mesh stack, do not need any param
		break;
	case 6: {
		// For RTK mesh stack
		lpn_request.fn_index = str_to_int(argv[0]);
		lpn_request.net_key_index = str_to_int(argv[1]);
		lpn_request.poll_interval = str_to_int(argv[2]);
		lpn_request.poll_timeout = str_to_int(argv[3]);
		lpn_request.frnd_rx_delay = str_to_int(argv[4]);
		lpn_request.frnd_rx_widen = str_to_int(argv[5]);
		break;
	}
	default:
		break;
	}
	ret = rtk_bt_mesh_stack_lpn_req(&lpn_request);
	if (ret) {
		BT_LOGE("[%s] Low power node request friendship failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argc;
	(void)argv;
	BT_LOGE("Is not device role or not suppport low power feature, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_sub(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_lpn_sub_t lpn_sub;
	lpn_sub.fn_index = str_to_int(argv[0]);
	lpn_sub.addr = str_to_int(argv[1]);
	lpn_sub.add_rm = str_to_int(argv[2]);
	ret = rtk_bt_mesh_stack_lpn_sub(&lpn_sub);
	if (ret) {
		BT_LOGE("[%s] Low power node subscribe add or remove failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not device role or not suppport low power feature, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_clear(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_stack_act_lpn_clear_t lpn_clear;
	lpn_clear.fn_index = str_to_int(argv[0]);
	ret = rtk_bt_mesh_stack_lpn_clear(&lpn_clear);
	if (ret) {
		BT_LOGE("[%s] Low power node clear friendship failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not device role or not suppport low power feature, do not support this function.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_deinit(int argc, char **argv)
{
	(void)argc;
	(void)argv;
#if defined(RTK_BLE_MESH_LPN_SUPPORT) && RTK_BLE_MESH_LPN_SUPPORT
	uint16_t ret = 0;
	ret = rtk_bt_mesh_stack_lpn_deinit();
	if (ret) {
		BT_LOGE("[%s] Low power node deinit failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not device role or not suppport low power feature, do not support this function.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_stack_cmd_table[] = {
	{"reset",       atcmd_ble_mesh_node_reset,                1, 1},
	{"dev_info",    atcmd_ble_mesh_dev_info,                  2, 2},
	{"list",        atcmd_ble_mesh_user_list_info,            1, 1},
	{"random",      atcmd_ble_mesh_set_random_value,          2, 2},
	{"hb_sub",      atcmd_ble_mesh_get_hb_subscribe_result,   1, 1},
	{"model_sub",   atcmd_ble_mesh_set_model_subscribe,       4, 4},
	{"retran_set",  atcmd_ble_mesh_set_retrans_param,         5, 5},
	{"fn_init",     atcmd_ble_mesh_fn_init,                   4, 4},
	{"fn_deinit",   atcmd_ble_mesh_fn_deinit,                 1, 1},
	{"pbadvcon",    atcmd_ble_mesh_pbadvcon,                  2, 2},
	{"pbadvdiscon", atcmd_ble_mesh_pbadv_discon,              1, 1},
	{"gatt_con",    atcmd_ble_mesh_pb_gatt_con,               1, 3},
	{"gatt_discon", atcmd_ble_mesh_pb_gatt_discon,            1, 2},
	{"prov",        atcmd_ble_mesh_start_provisioning,        1, 2},
	{"prov_choose", atcmd_ble_mesh_method_choose_for_prov,    1, 6},
	{"prov_dis",    atcmd_ble_mesh_prov_service_disvery,      2, 2},
	{"prov_notify", atcmd_ble_mesh_prov_service_set_notify,   2, 2},
	{"proxy_dis",   atcmd_ble_mesh_proxy_service_disvery,     2, 2},
	{"proxy_notify", atcmd_ble_mesh_proxy_service_set_notify,  2, 2},
	{"lpn_init",    atcmd_ble_mesh_lpn_init,                  2, 2},
	{"lpn_req",     atcmd_ble_mesh_lpn_req,                   1, 7},
	{"lpn_sub",     atcmd_ble_mesh_lpn_sub,                   4, 4},
	{"lpn_clear",   atcmd_ble_mesh_lpn_clear,                 2, 2},
	{"lpn_deinit",  atcmd_ble_mesh_lpn_deinit,                1, 1},
	{NULL,},
};

int atcmd_bt_mesh_stack(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_stack_cmd_table, "[AT+BLEMESHSTACK]");
}
#endif // end of RTK_BLE_MESH_SUPPORT
