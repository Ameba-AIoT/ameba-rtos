/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <zephyr/net/buf.h>
#include <access.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <bt_app_config.h>
#include <math.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_generic_default_transition_time.h>
#include <msg.h>

#define OP_GENERIC_DEFAULT_TRANSITION_TIME_GET              BT_MESH_MODEL_OP_2(0x82, 0x0D)
#define OP_GENERIC_DEFAULT_TRANSITION_TIME_SET              BT_MESH_MODEL_OP_2(0x82, 0x0E)
#define OP_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x0F)
#define OP_GENERIC_DEFAULT_TRANSITION_TIME_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x10)

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSTION_TIME_SERVER_MODEL
static const struct bt_mesh_model *generic_default_transition_time_server_model = NULL;

static int generic_default_transition_time_srv_init(const struct bt_mesh_model *model)
{
	generic_default_transition_time_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_default_transition_time_srv_cb = {
	.init = generic_default_transition_time_srv_init,
};

static rtk_bt_mesh_generic_transition_time_t get_present_transition_time(void)
{
	rtk_bt_mesh_generic_transition_time_t get_data = {0, 0};
	rtk_bt_mesh_generic_default_transition_time_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL, RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_default_transition_time_server_get_t));
	p_get_data = (rtk_bt_mesh_generic_default_transition_time_server_get_t *)p_evt->data;
	p_get_data->trans_time = &get_data;
	rtk_bt_evt_indicate(p_evt, NULL);

	return get_data;
}

static int generic_default_transition_time_stat(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_generic_transition_time_t trans_time)
{
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_GENERIC_DEFAULT_TRANSITION_TIME_STATUS, 1);
	bt_mesh_model_msg_init(&pmsg, OP_GENERIC_DEFAULT_TRANSITION_TIME_STATUS);
	net_buf_simple_add_mem(&pmsg, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

int generic_default_transition_time_publish(const struct bt_mesh_model *model, rtk_bt_mesh_generic_transition_time_t trans_time)
{
	return generic_default_transition_time_stat(model, NULL, trans_time);
}

int generic_default_transition_time_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	rtk_bt_mesh_generic_transition_time_t time = get_present_transition_time();

	bt_mesh_model_msg_init(msg, OP_GENERIC_DEFAULT_TRANSITION_TIME_STATUS);
	net_buf_simple_add_mem(msg, &time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	return 0;
}

static int generic_default_transition_time_srv_get(const struct bt_mesh_model *model,
												   struct bt_mesh_msg_ctx *ctx,
												   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_transition_time_t time = get_present_transition_time();
		return generic_default_transition_time_stat(model, ctx, time);
	}
	return -1;
}

static int generic_default_transition_time_srv_set_unack(const struct bt_mesh_model *model,
														 struct bt_mesh_msg_ctx *ctx,
														 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_generic_transition_time_t *time = (rtk_bt_mesh_generic_transition_time_t *)buf->data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL, RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_generic_transition_time_t));
		memcpy(p_evt->data, time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		return 0;
	}
	return -1;
}

static int generic_default_transition_time_srv_set(const struct bt_mesh_model *model,
												   struct bt_mesh_msg_ctx *ctx,
												   struct net_buf_simple *buf)
{
	if (!generic_default_transition_time_srv_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_transition_time_t time = get_present_transition_time();
		return generic_default_transition_time_stat(model, ctx, time);
	}
	return -1;
}

const struct bt_mesh_model_op generic_default_transition_time_srv_op[] = {
	{ OP_GENERIC_DEFAULT_TRANSITION_TIME_GET,               BT_MESH_LEN_EXACT(0),     generic_default_transition_time_srv_get },
	{ OP_GENERIC_DEFAULT_TRANSITION_TIME_SET,               BT_MESH_LEN_EXACT(1),     generic_default_transition_time_srv_set },
	{ OP_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK,         BT_MESH_LEN_EXACT(1),     generic_default_transition_time_srv_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL
static const struct bt_mesh_model *generic_default_transition_time_client_model = NULL;

static int generic_default_transition_time_cli_init(const struct bt_mesh_model *model)
{
	generic_default_transition_time_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_default_transition_time_cli_cb = {
	.init = generic_default_transition_time_cli_init,
};

static int generic_default_transition_time_cli_status(const struct bt_mesh_model *model,
													  struct bt_mesh_msg_ctx *ctx,
													  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_transition_time_t *time = (rtk_bt_mesh_generic_transition_time_t *)buf->data;
		rtk_bt_mesh_generic_transition_time_client_status_t status;
		status.src = ctx->addr;
		status.trans_time = *time;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL, RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_generic_transition_time_client_status_t));
		memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_generic_transition_time_client_status_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		return 0;
	}
	return -1;
}

const struct bt_mesh_model_op generic_default_transition_time_cli_op[] = {
	{ OP_GENERIC_DEFAULT_TRANSITION_TIME_STATUS,            BT_MESH_LEN_EXACT(1),     generic_default_transition_time_cli_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_default_transition_time_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_default_transition_time_set(const struct bt_mesh_model *model,
											   uint16_t dst, uint16_t app_key_index,
											   rtk_bt_mesh_generic_transition_time_t trans_time, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	if (ack) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_SET, 1);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_SET);
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		return bt_mesh_msg_send(model, &ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK, 1);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_DEFAULT_TRANSITION_TIME_SET_UNACK);
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		return bt_mesh_msg_send(model, &ctx, &buf);
	}
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_generic_default_transition_time_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_GET: {
		rtk_bt_mesh_generic_default_transition_time_get_t *time_client_get = (rtk_bt_mesh_generic_default_transition_time_get_t *)p_cmd->param;
		ret = generic_default_transition_time_get(generic_default_transition_time_client_model, time_client_get->dst, time_client_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_MODEL_SET: {
		rtk_bt_mesh_generic_default_transition_time_set_t *time_client_set = (rtk_bt_mesh_generic_default_transition_time_set_t *)p_cmd->param;
		ret = generic_default_transition_time_set(generic_default_transition_time_client_model, time_client_set->dst, time_client_set->app_key_index,
												  time_client_set->trans_time, time_client_set->ack);
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