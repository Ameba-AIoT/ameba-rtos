/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <access.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <zephyr/bluetooth/mesh/cfg_cli.h>
#include <foundation.h>
#include <bt_app_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_config_client_model.h>

uint8_t rtk_bt_mesh_config_client_model_indicate_event(uint32_t event_code, void *pdata, uint32_t len)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_CONFIG_CLIENT_MODEL, event_code, len);
	if (len) {
		if (event_code == RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT) {
			memcpy(p_evt->data, pdata, 10);
			rtk_bt_mesh_cfg_compo_data_stat_call_t *data_test = (rtk_bt_mesh_cfg_compo_data_stat_call_t *) pdata;
			memcpy((uint8_t *)(p_evt->data) + 10, data_test->data, len - 10);
		} else {
			memcpy(p_evt->data, pdata, len);
		}
	}
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static void comp_data_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t page,
							 struct net_buf_simple *buf)
{
	(void)cli;
	uint16_t msg_len = buf->len;
	rtk_bt_mesh_cfg_compo_data_stat_call_t pdata_test;
	pdata_test.page = page;
	pdata_test.msg_len = msg_len + 2; // add opcode length for rtk stack style
	if (page == 0) {
		pdata_test.flag = 1;
	} else {
		pdata_test.flag = 0;
	}
	pdata_test.src = addr;
	pdata_test.length = msg_len + 10; // add page length
	pdata_test.data = buf->data;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_STAT,
												   (uint8_t *)(&pdata_test), pdata_test.length);
}

static void beacon_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
							   uint8_t status)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_beacon_stat_t beacon_state;
	beacon_state.state = status;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_BEACON_STAT,
												   &beacon_state, sizeof(rtk_bt_mesh_cfg_beacon_stat_t));
}

static void ttl_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_default_ttl_stat_t ttl_stat;
	ttl_stat.ttl = status;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_STAT, &ttl_stat, sizeof(rtk_bt_mesh_cfg_default_ttl_stat_t));
}

static void frd_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_frnd_stat_t frnd_stat;
	frnd_stat.state = status;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_FRND_STAT,
												   &frnd_stat, sizeof(rtk_bt_mesh_cfg_frnd_stat_t));
}

static void proxy_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_proxy_stat_t prox_stat;
	prox_stat.state = status;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_PROXY_STAT,
												   &prox_stat, sizeof(rtk_bt_mesh_cfg_proxy_stat_t));
}

static void krp_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
							uint16_t net_idx, uint8_t phase)
{
	(void)cli;
	rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t krp_param;
	krp_param.stat = status;
	krp_param.net_key_index = net_idx;
	krp_param.state = phase;
	krp_param.src = addr;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_STAT,
												   &krp_param, sizeof(rtk_bt_mesh_cfg_key_refresh_phase_stat_call_t));
}

static void relay_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
							  uint8_t status, uint8_t transmit)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_relay_stat_t relay_stat;
	relay_stat.state = status;
	relay_stat.count = transmit & 0x07;
	relay_stat.steps = (transmit >> 3) & 0x1F;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_RELAY_STAT,
												   &relay_stat, sizeof(rtk_bt_mesh_cfg_relay_stat_t));
}

static void net_transmit_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
									 uint8_t status)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_net_transmit_stat_t net_t_stat;
	net_t_stat.count = status & 0x07;
	net_t_stat.steps = (status >> 3) & 0x1F;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_STAT, &net_t_stat, sizeof(rtk_bt_mesh_cfg_net_transmit_stat_t));
}

static void net_key_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
								uint8_t status, uint16_t net_idx)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_net_key_stat_t net_key_stat;
	net_key_stat.stat = status;
	net_key_stat.net_key_index = net_idx;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NET_KEY_STAT,
												   &net_key_stat, sizeof(rtk_bt_mesh_cfg_net_key_stat_t));
}

static void net_key_list_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
								struct net_buf_simple *buf)
{
	(void)cli;
	(void)addr;
	uint16_t len = buf->len;
	uint8_t remainder = len % 3;
	if (remainder == 0 || remainder == 2) {
		uint16_t key_count = len / 3 * 2 + (remainder != 0);
		uint16_t net_key_list[key_count + 4];
		net_key_list[0] = 1;
		net_key_list[1] = key_count;
		uint32_t length = sizeof(uint16_t) * (key_count + 4);
		uint32_t *len_call = (uint32_t *) & net_key_list[2];
		*len_call = length;
		uint16_t count_flag = 4;
		size_t key_len = len / 3 * 2 + (remainder != 0);
		bt_mesh_key_idx_unpack_list(buf, &net_key_list[count_flag], &key_len);
		rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST, &net_key_list, length);
	} else {
		uint16_t net_key_list[4];
		net_key_list[0] = 0;
		net_key_list[1] = len;
		uint32_t *len_call = (uint32_t *) &net_key_list[2];
		*len_call = 8;
		rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NET_KEY_LIST, &net_key_list, 8);
	}
}

static void nr_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr)
{
	(void)cli;
	rtk_bt_mesh_config_client_model_node_reset_stat_t nr_stat;
	nr_stat.src = addr;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NODE_RESET_STAT, &nr_stat,
												   sizeof(rtk_bt_mesh_config_client_model_node_reset_stat_t));
}

static void app_key_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
								uint8_t status, uint16_t net_idx, uint16_t app_idx)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_app_key_stat_t app_keys_status;
	app_keys_status.stat = status;
	NET_BUF_SIMPLE_DEFINE(buf, 3);
	key_idx_pack_pair(&buf, net_idx, app_idx);
	memcpy(app_keys_status.key_index, buf.data, 3);
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_APP_KEY_STAT, &app_keys_status,
												   sizeof(rtk_bt_mesh_cfg_app_key_stat_t));
	net_buf_simple_reset(&buf);
}

static void app_key_list_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
								uint16_t net_idx, struct net_buf_simple *buf)
{
	(void)cli;
	(void)addr;
	uint16_t len = buf->len;
	uint8_t remainder = len % 3;
	if (status == 0 && (remainder == 0 || remainder == 2)) {
		uint16_t key_count = len / 3 * 2 + (remainder != 0);
		uint16_t app_key_list[key_count + 5];
		app_key_list[0] = 1;
		app_key_list[1] = net_idx;
		uint32_t *len_call = (uint32_t *) & app_key_list[2];
		*len_call = sizeof(uint16_t) * (key_count + 5);
		app_key_list[4] = key_count;
		uint16_t count_flag = 5;
		size_t key_len = len / 3 * 2 + (remainder != 0);
		bt_mesh_key_idx_unpack_list(buf, &app_key_list[count_flag], &key_len);
		uint32_t length = sizeof(uint16_t) * (key_count + 5);
		rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST, &app_key_list, length);
	} else {
		uint16_t app_key_list[5];
		app_key_list[0] = 0;
		app_key_list[1] = status;
		uint32_t *len_call = (uint32_t *) & app_key_list[2];
		*len_call = sizeof(uint16_t) * 5;
		app_key_list[4] = len;
		rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_APP_KEY_LIST, &app_key_list, 10);
	}
}

static void model_app_status_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
									uint8_t status, uint16_t elem_addr,
									uint16_t app_idx, uint32_t mod_id)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_model_app_stat_t model_app_stat;
	model_app_stat.stat = status;
	model_app_stat.element_addr = elem_addr;
	model_app_stat.app_key_index = app_idx;
	model_app_stat.model_id = mod_id;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_STAT,
												   &model_app_stat, sizeof(rtk_bt_mesh_cfg_model_app_stat_t));
}

static void model_sub_list_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
								  uint16_t elem_addr, uint16_t mod_id, uint16_t cid,
								  struct net_buf_simple *buf)
{
	(void)cli;
	uint32_t model_id = cid + ((mod_id) << 16);
	uint32_t event_code;
	if (cid == 0xffff) {
		event_code = RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_SUB_LIST;
	} else {
		event_code = RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_SUB_LIST;
	}

	if (status == 0) {
		uint16_t addr_count = buf->len / 2;
		uint16_t model_sub_list[addr_count + 8];
		model_sub_list[0] = status;
		model_sub_list[1] = addr;
		uint32_t *len_call = (uint32_t *) & model_sub_list[2];
		*len_call = sizeof(uint16_t) * (addr_count + 8);
		uint32_t *model_id_pointer = (uint32_t *) &model_sub_list[4];
		*model_id_pointer = model_id;
		model_sub_list[6] = elem_addr - addr;
		model_sub_list[7] = addr_count;
		// uint8_t count = 0;
		for (uint8_t count = 0; count < addr_count; count++) {
			model_sub_list[count + 8] = net_buf_simple_pull_le16(buf);
		}
		uint32_t length = sizeof(uint16_t) * (addr_count + 8);
		rtk_bt_mesh_config_client_model_indicate_event(event_code, &model_sub_list, length);
	} else {
		uint16_t model_sub_list[4];
		model_sub_list[0] = status;
		model_sub_list[1] = addr;
		uint32_t *len_call = (uint32_t *) & model_sub_list[2];
		*len_call = sizeof(uint16_t) * 4;
		rtk_bt_mesh_config_client_model_indicate_event(event_code, &model_sub_list, sizeof(uint16_t) * 4);
	}

}

static void model_app_list_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
								  uint16_t elem_addr, uint16_t mod_id, uint16_t cid,
								  struct net_buf_simple *buf)
{
	(void)cli;
	uint32_t event_code;
	if (cid == 0xffff) {
		event_code = RTK_BT_MESH_CONFIG_MODEL_SIG_MODEL_APP_LIST;
	} else {
		event_code = RTK_BT_MESH_CONFIG_MODEL_VENDOR_MODEL_APP_LIST;
	}
	uint16_t len = buf->len;
	uint8_t remainder = len % 3;
	if (status == 0 && (remainder == 0 || remainder == 2)) {
		uint16_t key_count = len / 3 * 2 + (remainder != 0);
		uint32_t model_id = cid + ((mod_id) << 16);
		uint16_t model_app_list[key_count + 7];
		model_app_list[0] = 1;
		model_app_list[1] = elem_addr - addr;
		uint32_t *len_call = (uint32_t *) & model_app_list[2];
		*len_call = sizeof(uint16_t) * (key_count + 7);
		uint32_t *model_id_pointer = (uint32_t *) & model_app_list[4];
		*model_id_pointer = model_id;
		model_app_list[6] = key_count;
		uint16_t count_flag = 7;
		size_t key_len = len / 3 * 2 + (remainder != 0);
		bt_mesh_key_idx_unpack_list(buf, &model_app_list[count_flag], &key_len);
		uint32_t length = sizeof(uint16_t) * (key_count + 7);
		rtk_bt_mesh_config_client_model_indicate_event(event_code, &model_app_list, length);
	} else {
		uint16_t model_app_list[5];
		model_app_list[0] = 0;
		model_app_list[1] = status;
		uint32_t *len_call = (uint32_t *) & model_app_list[2];
		*len_call = sizeof(uint16_t) * 5;
		model_app_list[4] = len;
		uint32_t length = sizeof(uint16_t) * 5;
		rtk_bt_mesh_config_client_model_indicate_event(event_code, &model_app_list, length);
	}
}

static void model_pub_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
								  uint16_t elem_addr, uint16_t mod_id, uint16_t cid,
								  struct bt_mesh_cfg_cli_mod_pub *pub)
{
	(void)cli;
	(void)addr;
	uint32_t model_id = cid + ((mod_id) << 16);
	rtk_bt_mesh_cfg_model_pub_stat_t pub_stat;
	pub_stat.stat = status;
	pub_stat.element_addr = elem_addr;
	pub_stat.pub_addr = pub->addr;
	pub_stat.pub_key_info.app_key_index = pub->app_idx;
	pub_stat.pub_key_info.frnd_flag = pub->cred_flag;
	pub_stat.pub_ttl = pub->ttl;
	memcpy(&pub_stat.pub_period, &(pub->period), 1);
	memcpy(&pub_stat.pub_retrans_info, &(pub->transmit), 1);
	pub_stat.model_id = model_id;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_STAT,
												   &pub_stat, sizeof(rtk_bt_mesh_cfg_model_pub_stat_t));
}

static void model_sub_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
								  uint8_t status, uint16_t elem_addr,
								  uint16_t sub_addr, uint32_t mod_id)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_model_sub_stat_t model_sub_stat;
	model_sub_stat.stat = status;
	model_sub_stat.element_addr = elem_addr;
	model_sub_stat.addr = sub_addr;
	model_sub_stat.model_id = mod_id;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_STAT,
												   &model_sub_stat, sizeof(rtk_bt_mesh_cfg_model_sub_stat_t));
}

static void hb_sub_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
							   struct bt_mesh_cfg_cli_hb_sub *sub)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_hb_sub_stat_t sub_stat;
	sub_stat.stat = status;
	sub_stat.src = sub->src;
	sub_stat.dst = sub->dst;
	sub_stat.period_log = sub->period;
	sub_stat.count_log = sub->count;
	sub_stat.min_hops = sub->min;
	sub_stat.max_hops = sub->max;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_HB_SUB_STAT,
												   &sub_stat, sizeof(rtk_bt_mesh_cfg_hb_sub_stat_t));
}

static void hb_pub_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr, uint8_t status,
							   struct bt_mesh_cfg_cli_hb_pub *pub)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_hb_pub_stat_t hp_pub_stat;
	hp_pub_stat.stat = status;
	hp_pub_stat.dst = pub->dst;
	hp_pub_stat.count_log = pub->count;
	hp_pub_stat.period_log = pub->period;
	hp_pub_stat.ttl = pub->ttl;
	memcpy(&(hp_pub_stat.features), &(pub->feat), sizeof(uint16_t));
	hp_pub_stat.net_key_index = pub->net_idx;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_HB_PUB_STAT,
												   &hp_pub_stat, sizeof(rtk_bt_mesh_cfg_hb_pub_stat_t));
}

static void node_identity_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
									  uint8_t status, uint16_t net_idx, uint8_t identity)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_node_identity_stat_t node_identity_stat;
	node_identity_stat.stat = status;
	node_identity_stat.net_key_index = net_idx;
	node_identity_stat.identity = identity;
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_STAT,
												   &node_identity_stat, sizeof(rtk_bt_mesh_cfg_node_identity_stat_t));
}

static void lpn_to_stat_handle(struct bt_mesh_cfg_cli *cli, uint16_t addr,
							   uint16_t elem_addr, uint32_t timeout)
{
	(void)cli;
	(void)addr;
	rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t lpn_poll_stat;
	lpn_poll_stat.lpn_addr = elem_addr;
	memcpy(lpn_poll_stat.poll_to, &timeout, 3);
	rtk_bt_mesh_config_client_model_indicate_event(RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TO_STAT,
												   &lpn_poll_stat, sizeof(rtk_bt_mesh_cfg_lpn_poll_timeout_stat_t));
}

const struct bt_mesh_cfg_cli_cb cfg_cb = {
	.comp_data = comp_data_handle,
	.beacon_status = beacon_stat_handle,
	.ttl_status = ttl_stat_handle,
	.friend_status = frd_stat_handle,
	.gatt_proxy_status = proxy_stat_handle,
	.krp_status = krp_stat_handle,
	.relay_status = relay_stat_handle,
	.network_transmit_status = net_transmit_stat_handle,
	.net_key_status = net_key_stat_handle,
	.net_key_list = net_key_list_handle,
	.node_reset_status = nr_stat_handle,
	.app_key_status = app_key_stat_handle,
	.app_key_list = app_key_list_handle,
	.mod_app_status = model_app_status_handle,
	.mod_sub_list = model_sub_list_handle,
	.mod_app_list = model_app_list_handle,
	.mod_pub_status = model_pub_stat_handle,
	.mod_sub_status = model_sub_stat_handle,
	.hb_sub_status = hb_sub_stat_handle,
	.hb_pub_status = hb_pub_stat_handle,
	.node_identity_status = node_identity_stat_handle,
	.lpn_timeout_status = lpn_to_stat_handle,
};

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t composition_net_idx = 0;
uint16_t bt_mesh_config_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_CONFIG_MODEL_ADD_APP_KEY: {
		rtk_bt_mesh_config_client_add_app_key_t *add_app_key = (rtk_bt_mesh_config_client_add_app_key_t *)p_cmd->param;
		struct bt_mesh_cdb_app_key *app_key = NULL;
		app_key = bt_mesh_cdb_app_key_get(add_app_key->app_key_index);
		uint8_t app_key_add_keys[16] = {0};
		memcpy(app_key_add_keys, app_key->keys[0].app_key.key, 16);
		ret = bt_mesh_cfg_cli_app_key_add(composition_net_idx, add_app_key->unicast_addr, add_app_key->net_key_index,
										  add_app_key->app_key_index, app_key_add_keys, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_BIND: {
		rtk_bt_mesh_config_client_model_app_bind_t *model_app_bind = (rtk_bt_mesh_config_client_model_app_bind_t *)p_cmd->param;
		if (((model_app_bind->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (model_app_bind->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_bind(composition_net_idx, model_app_bind->dst, model_app_bind->element_addr,
											   model_app_bind->app_key_index, id_model, NULL);
		} else {
			uint16_t id_company = (model_app_bind->model_id) & 0xffff;
			uint16_t id_model = (model_app_bind->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_bind_vnd(composition_net_idx, model_app_bind->dst, model_app_bind->element_addr,
												   model_app_bind->app_key_index, id_model, id_company, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_ADD: {
		rtk_bt_mesh_config_client_model_sub_add_t *model_sub_add = (rtk_bt_mesh_config_client_model_sub_add_t *)p_cmd->param;
		if (!model_sub_add->va_flag) {
			uint16_t group_addr;
			memcpy(&group_addr, model_sub_add->group_addr, 2);
			if (((model_sub_add->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (model_sub_add->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_add(composition_net_idx, model_sub_add->dst, model_sub_add->element_addr,
												  group_addr, id_model, NULL);
			} else {
				uint16_t id_company = (model_sub_add->model_id) & 0xffff;
				uint16_t id_model = (model_sub_add->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_add_vnd(composition_net_idx, model_sub_add->dst, model_sub_add->element_addr,
													  group_addr, id_model, id_company, NULL);
			}
		} else {
			if (((model_sub_add->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (model_sub_add->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_add(composition_net_idx, model_sub_add->dst, model_sub_add->element_addr,
													 model_sub_add->group_addr, id_model, NULL, NULL);
			} else {
				uint16_t id_company = (model_sub_add->model_id) & 0xffff;
				uint16_t id_model = (model_sub_add->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_add_vnd(composition_net_idx, model_sub_add->dst, model_sub_add->element_addr,
														 model_sub_add->group_addr, id_model, id_company, NULL, NULL);
			}
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_GET: {
		rtk_bt_mesh_config_client_model_default_ttl_get_t *ttl_get = (rtk_bt_mesh_config_client_model_default_ttl_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_ttl_get(composition_net_idx, ttl_get->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_DEFAULT_TTL_SET: {
		rtk_bt_mesh_config_client_model_default_ttl_set_t *ttl_set = (rtk_bt_mesh_config_client_model_default_ttl_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_ttl_set(composition_net_idx, ttl_set->dst, ttl_set->ttl, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_RELAY_GET: {
		rtk_bt_mesh_config_client_model_relay_get_t *relay_get = (rtk_bt_mesh_config_client_model_relay_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_relay_get(composition_net_idx, relay_get->dst, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_RELAY_SET: {
		rtk_bt_mesh_config_client_model_relay_set_t *relay_set = (rtk_bt_mesh_config_client_model_relay_set_t *)p_cmd->param;
		uint8_t new_transmit = 0;
		// memcpy(&new_transmit, &relay_set->count, 1);
		new_transmit = relay_set->count + (relay_set->steps << 3);
		ret = bt_mesh_cfg_cli_relay_set(composition_net_idx, relay_set->dst, relay_set->state, new_transmit, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_GET: {
		rtk_bt_mesh_config_client_model_net_transmit_get_t *net_transmit_get;
		net_transmit_get = (rtk_bt_mesh_config_client_model_net_transmit_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_net_transmit_get(composition_net_idx, net_transmit_get->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_TRANSMIT_SET: {
		rtk_bt_mesh_config_client_model_net_transmit_set_t *net_transmit_set;
		net_transmit_set = (rtk_bt_mesh_config_client_model_net_transmit_set_t *)p_cmd->param;
		uint8_t new_transmit = 0;
		// memcpy(&new_transmit, &relay_set->count, 1);
		new_transmit = net_transmit_set->count + (net_transmit_set->steps << 3);
		ret = bt_mesh_cfg_cli_net_transmit_set(composition_net_idx, net_transmit_set->dst, new_transmit, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_RESET: {
		rtk_bt_mesh_config_client_model_node_reset_t *node_reset;
		node_reset = (rtk_bt_mesh_config_client_model_node_reset_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_node_reset(composition_net_idx, node_reset->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_COMPO_DATA_GET: {
		rtk_bt_mesh_config_client_model_compo_data_get_t *compo_data_get;
		compo_data_get = (rtk_bt_mesh_config_client_model_compo_data_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_comp_data_get(composition_net_idx, compo_data_get->dst, compo_data_get->page, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_BEACON_GET: {
		rtk_bt_mesh_config_client_model_beacon_get_t *beacon_get;
		beacon_get = (rtk_bt_mesh_config_client_model_beacon_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_beacon_get(composition_net_idx, beacon_get->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_BEACON_SET: {
		rtk_bt_mesh_config_client_model_beacon_set_t *beacon_set;
		beacon_set = (rtk_bt_mesh_config_client_model_beacon_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_beacon_set(composition_net_idx, beacon_set->dst, beacon_set->state, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_PROXY_GET: {
		rtk_bt_mesh_config_client_model_proxy_get_t *proxy_get;
		proxy_get = (rtk_bt_mesh_config_client_model_proxy_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_gatt_proxy_get(composition_net_idx, proxy_get->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_PROXY_SET: {
		rtk_bt_mesh_config_client_model_proxy_set_t *proxy_set;
		proxy_set = (rtk_bt_mesh_config_client_model_proxy_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_gatt_proxy_set(composition_net_idx, proxy_set->dst, proxy_set->state, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_GET: {
		rtk_bt_mesh_config_client_model_model_pub_get_t *pub_get;
		pub_get = (rtk_bt_mesh_config_client_model_model_pub_get_t *)p_cmd->param;
		if (((pub_get->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (pub_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_pub_get(composition_net_idx, pub_get->dst, pub_get->element_addr,
											  id_model, NULL, NULL);

		} else {
			uint16_t id_company = (pub_get->model_id) & 0xffff;
			uint16_t id_model = (pub_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_pub_get_vnd(composition_net_idx, pub_get->dst, pub_get->element_addr,
												  id_model, id_company, NULL, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_PUB_SET: {
		rtk_bt_mesh_config_client_model_model_pub_set_t *pub_set;
		pub_set = (rtk_bt_mesh_config_client_model_model_pub_set_t *)p_cmd->param;
		uint16_t pub_addr;
		memcpy(&pub_addr, pub_set->pub_addr, 2);
		struct bt_mesh_cfg_cli_mod_pub pub_para;
		pub_para.addr = pub_addr;
		pub_para.app_idx = pub_set->pub_key_info.app_key_index;
		if (pub_set->va_flag) {
			pub_para.uuid = pub_set->pub_addr;
		} else {
			pub_para.uuid = NULL;
		}
		pub_para.cred_flag = pub_set->pub_key_info.frnd_flag;
		pub_para.ttl = pub_set->pub_ttl;
		// pub_para.period = (uint8_t) pub_set->pub_period;
		// pub_para.transmit = (uint8_t) pub_set->pub_retrans_info;
		memcpy(&pub_para.period, (uint8_t *) & (pub_set->pub_period), 1);
		memcpy(&pub_para.transmit, (uint8_t *) & (pub_set->pub_retrans_info), 1);
		if (((pub_set->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (pub_set->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_pub_set(composition_net_idx, pub_set->dst, pub_set->element_addr,
											  id_model, &pub_para, NULL);

		} else {
			uint16_t id_company = (pub_set->model_id) & 0xffff;
			uint16_t id_model = (pub_set->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_pub_set_vnd(composition_net_idx, pub_set->dst, pub_set->element_addr,
												  id_model, id_company, &pub_para, NULL);
		}

		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE: {
		rtk_bt_mesh_config_client_model_model_sub_delete_t *sub_delete;
		sub_delete = (rtk_bt_mesh_config_client_model_model_sub_delete_t *)p_cmd->param;
		if (!sub_delete->va_flag) {
			uint16_t group_addr;
			memcpy(&group_addr, sub_delete->group_addr, 2);
			if (((sub_delete->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (sub_delete->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_del(composition_net_idx, sub_delete->dst, sub_delete->element_addr,
												  group_addr, id_model, NULL);
			} else {
				uint16_t id_company = (sub_delete->model_id) & 0xffff;
				uint16_t id_model = (sub_delete->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_del_vnd(composition_net_idx, sub_delete->dst, sub_delete->element_addr,
													  group_addr, id_model, id_company, NULL);
			}
		} else {
			if (((sub_delete->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (sub_delete->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_del(composition_net_idx, sub_delete->dst, sub_delete->element_addr,
													 sub_delete->group_addr, id_model, NULL, NULL);
			} else {
				uint16_t id_company = (sub_delete->model_id) & 0xffff;
				uint16_t id_model = (sub_delete->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_del_vnd(composition_net_idx, sub_delete->dst, sub_delete->element_addr,
														 sub_delete->group_addr, id_model, id_company, NULL, NULL);
			}
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_DELETE_ALL: {
		rtk_bt_mesh_config_client_model_model_sub_delete_all_t *sub_delete_all;
		sub_delete_all = (rtk_bt_mesh_config_client_model_model_sub_delete_all_t *)p_cmd->param;
		if (((sub_delete_all->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (sub_delete_all->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_sub_del_all(composition_net_idx, sub_delete_all->dst, sub_delete_all->element_addr,
												  id_model, NULL);
		} else {
			uint16_t id_company = (sub_delete_all->model_id) & 0xffff;
			uint16_t id_model = (sub_delete_all->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_sub_del_all_vnd(composition_net_idx, sub_delete_all->dst, sub_delete_all->element_addr,
													  id_model, id_company, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_OVERWRITE: {
		rtk_bt_mesh_config_client_model_model_sub_overwrite_t *sub_overwrite;
		sub_overwrite = (rtk_bt_mesh_config_client_model_model_sub_overwrite_t *)p_cmd->param;
		if (!sub_overwrite->va_flag) {
			uint16_t group_addr;
			memcpy(&group_addr, sub_overwrite->group_addr, 2);
			if (((sub_overwrite->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (sub_overwrite->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_overwrite(composition_net_idx, sub_overwrite->dst, sub_overwrite->element_addr,
														group_addr, id_model, NULL);
			} else {
				uint16_t id_company = (sub_overwrite->model_id) & 0xffff;
				uint16_t id_model = (sub_overwrite->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_overwrite_vnd(composition_net_idx, sub_overwrite->dst, sub_overwrite->element_addr,
															group_addr, id_model, id_company, NULL);
			}
		} else {
			if (((sub_overwrite->model_id) & 0xffff) == 0xffff) {
				uint16_t id_model = (sub_overwrite->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_overwrite(composition_net_idx, sub_overwrite->dst, sub_overwrite->element_addr,
														   sub_overwrite->group_addr, id_model, NULL, NULL);
			} else {
				uint16_t id_company = (sub_overwrite->model_id) & 0xffff;
				uint16_t id_model = (sub_overwrite->model_id) >> 16;
				ret = bt_mesh_cfg_cli_mod_sub_va_overwrite_vnd(composition_net_idx, sub_overwrite->dst, sub_overwrite->element_addr,
															   sub_overwrite->group_addr, id_model, id_company, NULL, NULL);
			}
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_SUB_GET: {
		rtk_bt_mesh_config_client_model_model_sub_get_t *sub_get;
		sub_get = (rtk_bt_mesh_config_client_model_model_sub_get_t *)p_cmd->param;
		if (((sub_get->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (sub_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_sub_get(composition_net_idx, sub_get->dst, sub_get->element_addr,
											  id_model, NULL, NULL, NULL);
		} else {
			uint16_t id_company = (sub_get->model_id) & 0xffff;
			uint16_t id_model = (sub_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_sub_get_vnd(composition_net_idx, sub_get->dst, sub_get->element_addr,
												  id_model, id_company, NULL, NULL, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_ADD: {
		rtk_bt_mesh_config_client_model_net_key_add_t *net_key_add;
		net_key_add = (rtk_bt_mesh_config_client_model_net_key_add_t *)p_cmd->param;
		struct bt_mesh_cdb_subnet *get_key = NULL;
		get_key = bt_mesh_cdb_subnet_get(net_key_add->net_key_index);
		uint8_t net_key_add_arr[16] = {0};
		memcpy(net_key_add_arr, get_key->keys[0].net_key.key, 16);
		ret = bt_mesh_cfg_cli_net_key_add(composition_net_idx, net_key_add->dst, net_key_add->net_key_index,
										  net_key_add_arr, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_UPDATE: {
		rtk_bt_mesh_config_client_model_net_key_update_t *net_key_update;
		net_key_update = (rtk_bt_mesh_config_client_model_net_key_update_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_net_key_update(composition_net_idx, net_key_update->dst, net_key_update->net_key_index,
											 net_key_update->net_key, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_DELETE: {
		rtk_bt_mesh_config_client_model_net_key_delete_t *net_key_delete;
		net_key_delete = (rtk_bt_mesh_config_client_model_net_key_delete_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_net_key_del(composition_net_idx, net_key_delete->dst, net_key_delete->net_key_index, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NET_KEY_GET: {
		rtk_bt_mesh_config_client_model_net_key_get_t *net_key_get;
		net_key_get = (rtk_bt_mesh_config_client_model_net_key_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_net_key_get(composition_net_idx, net_key_get->dst, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_UPDATE: {
		rtk_bt_mesh_config_client_model_app_key_update_t *app_key_update;
		app_key_update = (rtk_bt_mesh_config_client_model_app_key_update_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_app_key_update(composition_net_idx, app_key_update->dst, app_key_update->net_key_index,
											 app_key_update->app_key_index, app_key_update->app_key, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_DELETE: {
		rtk_bt_mesh_config_client_model_app_key_delete_t *app_key_delete;
		app_key_delete = (rtk_bt_mesh_config_client_model_app_key_delete_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_app_key_del(composition_net_idx, app_key_delete->dst,
										  app_key_delete->net_key_index, app_key_delete->app_key_index, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_APP_KEY_GET: {
		rtk_bt_mesh_config_client_model_app_key_get_t *app_key_get;
		app_key_get = (rtk_bt_mesh_config_client_model_app_key_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_app_key_get(composition_net_idx, app_key_get->dst, app_key_get->net_key_index, NULL, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_GET: {
		rtk_bt_mesh_config_client_model_node_identity_get_t *node_identity_get;
		node_identity_get = (rtk_bt_mesh_config_client_model_node_identity_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_node_identity_get(composition_net_idx, node_identity_get->dst, node_identity_get->net_key_index, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_NODE_IDENTITY_SET: {
		rtk_bt_mesh_config_client_model_node_identity_set_t *node_identity_set;
		node_identity_set = (rtk_bt_mesh_config_client_model_node_identity_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_node_identity_set(composition_net_idx, node_identity_set->dst, node_identity_set->net_key_index, node_identity_set->identity, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_UNBIND: {
		rtk_bt_mesh_config_client_model_model_app_unbind_t *model_app_unbind;
		model_app_unbind = (rtk_bt_mesh_config_client_model_model_app_unbind_t *)p_cmd->param;
		if (((model_app_unbind->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (model_app_unbind->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_unbind(composition_net_idx, model_app_unbind->dst, model_app_unbind->element_addr,
												 model_app_unbind->app_key_index, id_model, NULL);
		} else {
			uint16_t id_company = (model_app_unbind->model_id) & 0xffff;
			uint16_t id_model = (model_app_unbind->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_unbind_vnd(composition_net_idx, model_app_unbind->dst, model_app_unbind->element_addr,
													 model_app_unbind->app_key_index, id_model, id_company, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_MODEL_APP_GET: {
		rtk_bt_mesh_config_client_model_model_app_get_t *model_app_get;
		model_app_get = (rtk_bt_mesh_config_client_model_model_app_get_t *)p_cmd->param;
		if (((model_app_get->model_id) & 0xffff) == 0xffff) {
			uint16_t id_model = (model_app_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_get(composition_net_idx, model_app_get->dst, model_app_get->element_addr,
											  id_model, NULL, NULL, NULL);
		} else {
			uint16_t id_company = (model_app_get->model_id) & 0xffff;
			uint16_t id_model = (model_app_get->model_id) >> 16;
			ret = bt_mesh_cfg_cli_mod_app_get_vnd(composition_net_idx, model_app_get->dst, model_app_get->element_addr,
												  id_model, id_company, NULL, NULL, NULL);
		}
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_FRND_GET: {
		rtk_bt_mesh_config_client_model_frnd_get_t *frnd_get;
		frnd_get = (rtk_bt_mesh_config_client_model_frnd_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_friend_get(composition_net_idx, frnd_get->dst, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_FRND_SET: {
		rtk_bt_mesh_config_client_model_frnd_set_t *frnd_set;
		frnd_set = (rtk_bt_mesh_config_client_model_frnd_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_friend_set(composition_net_idx, frnd_set->dst, frnd_set->state, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_LPN_POLL_TIMEOUT_GET: {
		rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t *lpn_poll_timeout_get;
		lpn_poll_timeout_get = (rtk_bt_mesh_config_client_model_lpn_poll_timeout_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_lpn_timeout_get(composition_net_idx, lpn_poll_timeout_get->dst, lpn_poll_timeout_get->lpn_addr, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_GET: {
		rtk_bt_mesh_config_client_model_key_refresh_phase_get_t *key_refresh_phase_get;
		key_refresh_phase_get = (rtk_bt_mesh_config_client_model_key_refresh_phase_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_krp_get(composition_net_idx, key_refresh_phase_get->dst, key_refresh_phase_get->net_key_index, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_KEY_REFRESH_PHASE_SET: {
		rtk_bt_mesh_config_client_model_key_refresh_phase_set_t *key_refresh_phase_set;
		key_refresh_phase_set = (rtk_bt_mesh_config_client_model_key_refresh_phase_set_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_krp_set(composition_net_idx, key_refresh_phase_set->dst, key_refresh_phase_set->net_key_index,
									  key_refresh_phase_set->state, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_PUB_GET: {
		rtk_bt_mesh_config_client_model_hb_pub_get_t *hb_pub_get;
		hb_pub_get = (rtk_bt_mesh_config_client_model_hb_pub_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_hb_pub_get(composition_net_idx, hb_pub_get->dst, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_PUB_SET: {
		rtk_bt_mesh_config_client_model_hb_pub_set_t *hb_pub_set;
		hb_pub_set = (rtk_bt_mesh_config_client_model_hb_pub_set_t *)p_cmd->param;
		struct bt_mesh_cfg_cli_hb_pub pub_param;
		pub_param.dst = hb_pub_set->dst_pub;
		pub_param.count = hb_pub_set->count_log;
		pub_param.period = hb_pub_set->period_log;
		pub_param.ttl = hb_pub_set->ttl;
		memcpy(&pub_param.feat, &(hb_pub_set->features), sizeof(uint16_t));
		pub_param.net_idx = hb_pub_set->net_key_index;
		ret = bt_mesh_cfg_cli_hb_pub_set(composition_net_idx, hb_pub_set->dst, &pub_param, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_SUB_GET: {
		rtk_bt_mesh_config_client_model_hb_sub_get_t *hb_sub_get;
		hb_sub_get = (rtk_bt_mesh_config_client_model_hb_sub_get_t *)p_cmd->param;
		ret = bt_mesh_cfg_cli_hb_sub_get(composition_net_idx, hb_sub_get->dst, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_CONFIG_MODEL_HB_SUB_SET: {
		rtk_bt_mesh_config_client_model_hb_sub_set_t *hb_sub_set;
		hb_sub_set = (rtk_bt_mesh_config_client_model_hb_sub_set_t *)p_cmd->param;
		struct bt_mesh_cfg_cli_hb_sub sub_param;
		sub_param.src = hb_sub_set->src;
		sub_param.dst = hb_sub_set->dst_set;
		sub_param.period = hb_sub_set->period_log;
		ret = bt_mesh_cfg_cli_hb_sub_set(composition_net_idx, hb_sub_set->dst, &sub_param, NULL);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}

#endif
#endif