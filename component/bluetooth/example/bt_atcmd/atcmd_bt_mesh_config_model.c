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
#include <rtk_bt_mesh_config_client_model.h>

static int atcmd_ble_mesh_app_key_add(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_add_app_key_t app_key_add;
	app_key_add.unicast_addr = str_to_int(argv[0]);
	app_key_add.net_key_index = str_to_int(argv[1]);
	app_key_add.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_add_app_key(&app_key_add);
	if (ret) {
		BT_LOGE("[%s] Add app key failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_app_bind(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_app_bind_t model_app_bind;
	model_app_bind.dst = str_to_int(argv[0]);
	model_app_bind.element_addr = model_app_bind.dst + str_to_int(argv[1]);
	model_app_bind.model_id = str_to_int(argv[2]);
	model_app_bind.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_config_client_model_app_bind(&model_app_bind);
	if (ret) {
		BT_LOGE("[%s] Model bind app key failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_sub_add(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_sub_add_t model_sub_add;
	model_sub_add.dst = str_to_int(argv[0]);
	model_sub_add.element_addr = model_sub_add.dst + str_to_int(argv[1]);
	model_sub_add.model_id = str_to_int(argv[2]);
	model_sub_add.va_flag = false;
	model_sub_add.group_addr[0] = str_to_int(argv[3]) & 0xff;
	model_sub_add.group_addr[1] = (str_to_int(argv[3]) & 0xff00) >> 8;
	ret = rtk_bt_mesh_config_client_model_sub_add(&model_sub_add);
	if (ret) {
		BT_LOGE("[%s] Model sub add failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_ttl_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_default_ttl_get_t ttl_get;
	ttl_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_default_ttl_get(&ttl_get);
	if (ret) {
		BT_LOGE("[%s] Get default ttl failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_ttl_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_default_ttl_set_t ttl_set;
	ttl_set.dst = str_to_int(argv[0]);
	ttl_set.ttl = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_default_ttl_set(&ttl_set);
	if (ret) {
		BT_LOGE("[%s] Set default ttl failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_relay_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_relay_get_t relay_get;
	relay_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_relay_get(&relay_get);
	if (ret) {
		BT_LOGE("[%s] Get relay state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_relay_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_relay_set_t relay_set;
	relay_set.dst = str_to_int(argv[0]);
	relay_set.state = str_to_int(argv[1]);
	relay_set.count = str_to_int(argv[2]);
	relay_set.steps = str_to_int(argv[3]);
	ret = rtk_bt_mesh_config_client_model_relay_set(&relay_set);
	if (ret) {
		BT_LOGE("[%s] Set relay state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_trans_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_net_transmit_get_t net_get;
	net_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_net_transmit_get(&net_get);
	if (ret) {
		BT_LOGE("[%s] Get net trans state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_trans_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_net_transmit_set_t net_set;
	net_set.dst = str_to_int(argv[0]);
	net_set.count = str_to_int(argv[1]);
	net_set.steps = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_net_transmit_set(&net_set);
	if (ret) {
		BT_LOGE("[%s] Set net trans state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_node_reset(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_node_reset_t node_reset;
	node_reset.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_node_reset(&node_reset);
	if (ret) {
		BT_LOGE("[%s] Node 0x%x reset failed! reason: 0x%x\r\n", __func__, node_reset.dst, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_beacon_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_beacon_get_t beacon_get;
	beacon_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_beacon_get(&beacon_get);
	if (ret) {
		BT_LOGE("[%s] Get beacon state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_beacon_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_beacon_set_t beacon_set;
	beacon_set.dst = str_to_int(argv[0]);
	beacon_set.state = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_beacon_set(&beacon_set);
	if (ret) {
		BT_LOGE("[%s] Set beacon state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_compo_data_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_compo_data_get_t compo_data_get;
	compo_data_get.dst = str_to_int(argv[0]);
	compo_data_get.page = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_compo_data_get(&compo_data_get);
	if (ret) {
		BT_LOGE("[%s] Get compo data state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_proxy_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_proxy_get_t proxy_get;
	proxy_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_proxy_get(&proxy_get);
	if (ret) {
		BT_LOGE("[%s] Get proxy state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_proxy_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_proxy_set_t proxy_set;
	proxy_set.dst = str_to_int(argv[0]);
	proxy_set.state = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_proxy_set(&proxy_set);
	if (ret) {
		BT_LOGE("[%s] Set proxy state failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pub_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_model_pub_get_t pub_get;
	pub_get.dst = str_to_int(argv[0]);
	pub_get.element_addr = pub_get.dst + str_to_int(argv[1]);
	pub_get.model_id = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_pub_get(&pub_get);
	if (ret) {
		BT_LOGE("[%s] Get model publication failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_pub_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret;
	rtk_bt_mesh_config_client_model_model_pub_set_t pub_set;
	pub_set.dst = str_to_int(argv[0]);
	pub_set.element_addr = pub_set.dst + str_to_int(argv[1]);
	pub_set.va_flag = false;
	pub_set.model_id = str_to_int(argv[2]);
	pub_set.pub_addr[0] = str_to_int(argv[3]) & 0xff;
	pub_set.pub_addr[1] = (str_to_int(argv[3]) & 0xff00) >> 8;
	pub_set.pub_key_info.app_key_index = str_to_int(argv[4]);//for test
	pub_set.pub_key_info.frnd_flag = str_to_int(argv[5]);//for test
	pub_set.pub_ttl = str_to_int(argv[6]);
	pub_set.pub_period.steps = str_to_int(argv[7]);
	pub_set.pub_period.resol = str_to_int(argv[8]);
	pub_set.pub_retrans_info.count = str_to_int(argv[9]);
	pub_set.pub_retrans_info.steps = str_to_int(argv[10]);
	ret = rtk_bt_mesh_config_client_model_pub_set(&pub_set);
	if (ret) {
		BT_LOGE("[%s] Set model publication failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_sub_delete(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_sub_delete_t sub_delete;
	sub_delete.dst = str_to_int(argv[0]);
	sub_delete.element_addr = sub_delete.dst + str_to_int(argv[1]);
	sub_delete.model_id = str_to_int(argv[2]);
	sub_delete.va_flag = false;
	sub_delete.group_addr[0] = str_to_int(argv[3]) & 0xff;
	sub_delete.group_addr[1] = (str_to_int(argv[3]) & 0xff00) >> 8;
	ret = rtk_bt_mesh_config_client_model_sub_delete(&sub_delete);
	if (ret) {
		BT_LOGE("[%s] Model sub delete failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_sub_delete_all(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_sub_delete_all_t sub_delete_all;
	sub_delete_all.dst = str_to_int(argv[0]);
	sub_delete_all.element_addr = sub_delete_all.dst + str_to_int(argv[1]);
	sub_delete_all.model_id = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_sub_delete_all(&sub_delete_all);
	if (ret) {
		BT_LOGE("[%s] Model sub delete all failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_sub_overwrite(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_sub_overwrite_t sub_overwrite;
	sub_overwrite.dst = str_to_int(argv[0]);
	sub_overwrite.element_addr = sub_overwrite.dst + str_to_int(argv[1]);
	sub_overwrite.va_flag = false;
	sub_overwrite.model_id = str_to_int(argv[2]);
	sub_overwrite.group_addr[0] = str_to_int(argv[3]) & 0xff;
	sub_overwrite.group_addr[1] = (str_to_int(argv[3]) & 0xff00) >> 8;
	ret = rtk_bt_mesh_config_client_model_sub_overwrite(&sub_overwrite);
	if (ret) {
		BT_LOGE("[%s] Model sub overwrite failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_sub_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_sub_get_t sub_get;
	sub_get.dst = str_to_int(argv[0]);
	sub_get.element_addr = sub_get.dst + str_to_int(argv[1]);
	sub_get.model_id = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_sub_get(&sub_get);
	if (ret) {
		BT_LOGE("[%s] Model sub get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_key_add(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_net_key_add_t net_key_add;
	net_key_add.dst = str_to_int(argv[0]);
	net_key_add.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_net_key_add(&net_key_add);
	if (ret) {
		BT_LOGE("[%s] Add netkey failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_key_update(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_net_key_update_t net_key_update;
	net_key_update.dst = str_to_int(argv[0]);
	net_key_update.net_key_index = str_to_int(argv[1]);
	uint16_t len;
	len = strlen(argv[2]);
	if (len / 2 > 16) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, 16);
		return -2;
	}
	if (hexdata_str_to_array(argv[2], net_key_update.net_key, len)) {
		ret = rtk_bt_mesh_config_client_model_net_key_update(&net_key_update);
		if (ret) {
			BT_LOGE("[%s] Update netkey failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_key_delete(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_net_key_delete_t net_key_delete;
	net_key_delete.dst = str_to_int(argv[0]);
	net_key_delete.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_net_key_delete(&net_key_delete);
	if (ret) {
		BT_LOGE("[%s] Delete netkey failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_net_key_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_net_key_get_t net_key_get;
	net_key_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_net_key_get(&net_key_get);
	if (ret) {
		BT_LOGE("[%s] Get netkey failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_app_key_update(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_app_key_update_t app_key_update;
	app_key_update.dst = str_to_int(argv[0]);
	app_key_update.net_key_index = str_to_int(argv[1]);
	app_key_update.app_key_index = str_to_int(argv[2]);
	uint16_t len;
	len = strlen(argv[3]);
	if (len / 2 > 16) {
		BT_LOGE("[%s] The len %d of input data extend max value %d\r\n", __func__, len, 16);
		return -2;
	}
	if (hexdata_str_to_array(argv[3], app_key_update.app_key, len)) {
		ret = rtk_bt_mesh_config_client_model_app_key_update(&app_key_update);
		if (ret) {
			BT_LOGE("[%s] Update appkey failed! reason: 0x%x\r\n", __func__, ret);
			return -1;
		}
		return 0;
	} else {
		BT_LOGE("[%s] Input data format is not corrent.\r\n", __func__);
		return -3;
	}
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_app_key_delete(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_app_key_delete_t app_key_delete;
	app_key_delete.dst = str_to_int(argv[0]);
	app_key_delete.net_key_index = str_to_int(argv[1]);
	app_key_delete.app_key_index = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_app_key_delete(&app_key_delete);
	if (ret) {
		BT_LOGE("[%s] Delete appkey failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_app_key_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_app_key_get_t app_key_get;
	app_key_get.dst = str_to_int(argv[0]);
	app_key_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_app_key_get(&app_key_get);
	if (ret) {
		BT_LOGE("[%s] Delete appkey failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_node_identity_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_node_identity_get_t node_identity_get;
	node_identity_get.dst = str_to_int(argv[0]);
	node_identity_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_node_identity_get(&node_identity_get);
	if (ret) {
		BT_LOGE("[%s] Get node identity failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_node_identity_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_node_identity_set_t node_identity_set;
	node_identity_set.dst = str_to_int(argv[0]);
	node_identity_set.net_key_index = str_to_int(argv[1]);
	node_identity_set.identity = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_node_identity_set(&node_identity_set);
	if (ret) {
		BT_LOGE("[%s] Set node identity failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_app_unbind(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_app_unbind_t model_app_unbind;
	model_app_unbind.dst = str_to_int(argv[0]);
	model_app_unbind.element_addr = model_app_unbind.dst + str_to_int(argv[1]);
	model_app_unbind.model_id = str_to_int(argv[2]);
	model_app_unbind.app_key_index = str_to_int(argv[3]);
	ret = rtk_bt_mesh_config_client_model_app_unbind(&model_app_unbind);
	if (ret) {
		BT_LOGE("[%s] Model unbind app key failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_model_app_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_model_app_get_t model_app_get;
	model_app_get.dst = str_to_int(argv[0]);
	model_app_get.element_addr = model_app_get.dst + str_to_int(argv[1]);
	model_app_get.model_id = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_app_get(&model_app_get);
	if (ret) {
		BT_LOGE("[%s] Model app get failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_frnd_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_frnd_get_t frnd_get;
	frnd_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_frnd_get(&frnd_get);
	if (ret) {
		BT_LOGE("[%s] Get friend failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_frnd_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_frnd_set_t frnd_set;
	frnd_set.dst = str_to_int(argv[0]);
	frnd_set.state = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_frnd_set(&frnd_set);
	if (ret) {
		BT_LOGE("[%s] Set friend failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_lpn_poll_timeout_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t lpn_poll_timeout_get;
	lpn_poll_timeout_get.dst = str_to_int(argv[0]);
	lpn_poll_timeout_get.lpn_addr = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_lpn_poll_timeout_get(&lpn_poll_timeout_get);
	if (ret) {
		BT_LOGE("[%s] Get lpn poll timeout failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_key_refresh_phase_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_key_refresh_phase_get_t key_refresh_phase_get;
	key_refresh_phase_get.dst = str_to_int(argv[0]);
	key_refresh_phase_get.net_key_index = str_to_int(argv[1]);
	ret = rtk_bt_mesh_config_client_model_key_refresh_phase_get(&key_refresh_phase_get);
	if (ret) {
		BT_LOGE("[%s] Get key refresh phase failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_key_refresh_phase_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_key_refresh_phase_set_t key_refresh_phase_set;
	key_refresh_phase_set.dst = str_to_int(argv[0]);
	key_refresh_phase_set.net_key_index = str_to_int(argv[1]);
	key_refresh_phase_set.state = str_to_int(argv[2]);
	ret = rtk_bt_mesh_config_client_model_key_refresh_phase_set(&key_refresh_phase_set);
	if (ret) {
		BT_LOGE("[%s] Set key refresh phase failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_hb_pub_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_hb_pub_get_t hb_pub_get;
	hb_pub_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_hb_pub_get(&hb_pub_get);
	if (ret) {
		BT_LOGE("[%s] Get hearbeat publication failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_hb_pub_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_hb_pub_set_t hb_pub_set;
	hb_pub_set.dst = str_to_int(argv[0]);  // Dst of config heartbeat publish set message
	hb_pub_set.net_key_index = str_to_int(argv[1]);
	hb_pub_set.dst_pub = str_to_int(argv[2]);  // Dst of hearbeat messages
	hb_pub_set.count_log = str_to_int(argv[3]);
	hb_pub_set.period_log = str_to_int(argv[4]);
	hb_pub_set.ttl = str_to_int(argv[5]);
	hb_pub_set.features.relay = str_to_int(argv[6]);
	hb_pub_set.features.proxy = str_to_int(argv[7]);
	hb_pub_set.features.frnd = str_to_int(argv[8]);
	hb_pub_set.features.lpn = str_to_int(argv[9]);
	ret = rtk_bt_mesh_config_client_model_hb_pub_set(&hb_pub_set);
	if (ret) {
		BT_LOGE("[%s] Set hearbeat publication failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_hb_sub_get(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_hb_sub_get_t hb_sub_get;
	hb_sub_get.dst = str_to_int(argv[0]);
	ret = rtk_bt_mesh_config_client_model_hb_sub_get(&hb_sub_get);
	if (ret) {
		BT_LOGE("[%s] Get hearbeat subscription failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static int atcmd_ble_mesh_hb_sub_set(int argc, char **argv)
{
	(void)argc;
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
	uint16_t ret = 0;
	rtk_bt_mesh_config_client_model_hb_sub_set_t hb_sub_set;
	hb_sub_set.dst = str_to_int(argv[0]);  // Dst of config heartbeat subscribe set message
	hb_sub_set.src = str_to_int(argv[1]);  // Src of heartbeat messages
	hb_sub_set.dst_set = str_to_int(argv[2]);  // Dst of hearbeat messages
	hb_sub_set.period_log = str_to_int(argv[3]);
	ret = rtk_bt_mesh_config_client_model_hb_sub_set(&hb_sub_set);
	if (ret) {
		BT_LOGE("[%s] Set hearbeat subscription failed! reason: 0x%x\r\n", __func__, ret);
		return -1;
	}
	return 0;
#else
	(void)argv;
	BT_LOGE("Is not provisioner role, do not support configuration client model.\r\n");
	return -1;
#endif
}

static const cmd_table_t mesh_config_cmd_table[] = {
	{"aka",    atcmd_ble_mesh_app_key_add,    4, 4},
	{"mab",    atcmd_ble_mesh_model_app_bind,    5, 5},
	{"msa",    atcmd_ble_mesh_model_sub_add,    5, 5},
	{"ttl_get",    atcmd_ble_mesh_ttl_get,    2, 2},
	{"ttl_set",    atcmd_ble_mesh_ttl_set,    3, 3},
	{"relay_get",    atcmd_ble_mesh_relay_get,    2, 2},
	{"relay_set",    atcmd_ble_mesh_relay_set,    5, 5},
	{"net_trans_get",    atcmd_ble_mesh_net_trans_get,    2, 2},
	{"net_trans_set",    atcmd_ble_mesh_net_trans_set,    4, 4},
	{"reset",    atcmd_ble_mesh_node_reset,    2, 2},
	{"beacon_get",    atcmd_ble_mesh_beacon_get,    2, 2},
	{"beacon_set",    atcmd_ble_mesh_beacon_set,    3, 3},
	{"compo_data_get",    atcmd_ble_mesh_compo_data_get,    3, 3},
	{"proxy_get",    atcmd_ble_mesh_proxy_get,    2, 2},
	{"proxy_set",    atcmd_ble_mesh_proxy_set,    3, 3},
	{"pub_get",    atcmd_ble_mesh_pub_get,    4, 4},
	{"pub_set",    atcmd_ble_mesh_pub_set,    12, 12},
	{"msd",    atcmd_ble_mesh_model_sub_delete,    5, 5},
	{"msda",    atcmd_ble_mesh_model_sub_delete_all,    4, 4},
	{"mso",    atcmd_ble_mesh_model_sub_overwrite,    5, 5},
	{"msg",    atcmd_ble_mesh_model_sub_get,    4, 4},
	{"nka",    atcmd_ble_mesh_net_key_add,    3, 3},
	{"nku",    atcmd_ble_mesh_net_key_update,    4, 4},
	{"nkd",    atcmd_ble_mesh_net_key_delete,    3, 3},
	{"nkg",    atcmd_ble_mesh_net_key_get,    2, 2},
	{"aku",    atcmd_ble_mesh_app_key_update,    5, 5},
	{"akd",    atcmd_ble_mesh_app_key_delete,    4, 4},
	{"akg",    atcmd_ble_mesh_app_key_get,    3, 3},
	{"node_identity_get",    atcmd_ble_mesh_node_identity_get,    3, 3},
	{"node_identity_set",    atcmd_ble_mesh_node_identity_set,    4, 4},
	{"maub",    atcmd_ble_mesh_model_app_unbind,    5, 5},
	{"mag",    atcmd_ble_mesh_model_app_get,    4, 4},
	{"frg",    atcmd_ble_mesh_frnd_get,    2, 2},
	{"frs",    atcmd_ble_mesh_frnd_set,    3, 3},
	{"lptg",    atcmd_ble_mesh_lpn_poll_timeout_get,    3, 3},
	{"krpg",    atcmd_ble_mesh_key_refresh_phase_get,    3, 3},
	{"krps",    atcmd_ble_mesh_key_refresh_phase_set,    4, 4},
	{"hpg",    atcmd_ble_mesh_hb_pub_get,    2, 2},
	{"hps",    atcmd_ble_mesh_hb_pub_set,    11, 11},
	{"hsg",    atcmd_ble_mesh_hb_sub_get,    2, 2},
	{"hss",    atcmd_ble_mesh_hb_sub_set,    5, 5},
	{NULL,},
};

int atcmd_bt_mesh_config(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, mesh_config_cmd_table, "[AT+BLEMESHCONFIG]");
}

#endif // end of RTK_BLE_MESH_SUPPORT
