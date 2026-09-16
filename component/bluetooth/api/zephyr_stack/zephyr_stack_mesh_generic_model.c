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
#include <rtk_bt_mesh_generic_model.h>
#include <rtk_bt_mesh_generic_onoff_model.h>
#include <msg.h>

#define OP_GENERIC_LEVEL_GET              BT_MESH_MODEL_OP_2(0x82, 0x05)
#define OP_GENERIC_LEVEL_SET              BT_MESH_MODEL_OP_2(0x82, 0x06)
#define OP_GENERIC_LEVEL_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x07)
#define OP_GENERIC_LEVEL_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x08)
#define OP_GENERIC_DELTA_SET              BT_MESH_MODEL_OP_2(0x82, 0x09)
#define OP_GENERIC_DELTA_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x0A)
#define OP_GENERIC_MOVE_SET               BT_MESH_MODEL_OP_2(0x82, 0x0B)
#define OP_GENERIC_MOVE_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x0C)

#if ((defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL))
static const uint32_t time_res[] = {
	100,
	MSEC_PER_SEC,
	10 * MSEC_PER_SEC,
	10 * 60 * MSEC_PER_SEC,
};
#endif

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_SERVER_MODEL
struct generic_level_t {
	int16_t level;
	int16_t target_level;
	int32_t delta_level;
	int16_t move_level;
	uint16_t src;
	uint8_t level_tid;
	uint8_t delta_tid;
	uint8_t move_tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} generic_level_pa;

static const struct bt_mesh_model *generic_level_server_model = NULL;

struct k_mutex mutex_level;

void generic_level_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_level, K_FOREVER);
	if (!generic_level_pa.transition_flag) {
		generic_level_pa.remain_time.num_steps = 0;
		generic_level_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[generic_level_pa.remain_time.step_resolution];
	rtk_bt_mesh_generic_level_server_set_t level_set;
	level_set.level = generic_level_pa.target_level;
	level_set.total_time = generic_level_pa.total_time;
	level_set.remaining_time = generic_level_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_generic_level_server_set_t));
	memcpy(p_evt->data, &level_set, sizeof(rtk_bt_mesh_generic_level_server_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (generic_level_pa.remain_time.num_steps) {
		generic_level_pa.remain_time.num_steps--;
		k_work_reschedule(&generic_level_pa.work, K_MSEC(trans));
	} else {
		generic_level_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_level);
	return;
}

static int generic_level_srv_init(const struct bt_mesh_model *model)
{
	generic_level_server_model = model;
	k_work_init_delayable(&generic_level_pa.work, generic_level_timeout);
	k_mutex_init(&mutex_level);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int generic_level_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_level);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_generic_level_srv_cb = {
	.init = generic_level_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = generic_level_srv_deinit,
#endif
};

static int16_t get_present_level(void)
{
	int16_t get_data = 0;
	rtk_bt_mesh_generic_level_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_level_server_get_t));
	p_get_data = (rtk_bt_mesh_generic_level_server_get_t *)p_evt->data;
	p_get_data->level = &get_data;
	rtk_bt_evt_indicate(p_evt, NULL);

	return get_data;
}

static int generic_level_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, int16_t present_level,
							  bool optional, int16_t target_level, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 2;
	if (optional) {
		msg_len = 5;
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LEVEL_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LEVEL_STATUS);
	net_buf_simple_add_le16(&buf, present_level);
	if (optional) {
		net_buf_simple_add_le16(&buf, target_level);
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

int generic_level_publish(const struct bt_mesh_model *model)
{
	int16_t level = get_present_level();
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return generic_level_stat(model, NULL, level, FALSE, 0, remaining_time);
}

int generic_level_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	int16_t level = get_present_level();
	bt_mesh_model_msg_init(msg, OP_GENERIC_LEVEL_STATUS);
	net_buf_simple_add_le16(msg, level);
	return 0;
}

static int generic_level_srv_get(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	if (!buf->len) {
		int16_t level = get_present_level();
		k_mutex_lock(&mutex_level, K_FOREVER);
		rtk_bt_mesh_generic_transition_time_t remain_time = generic_level_pa.remain_time;
		bool trans_flag = generic_level_pa.transition_flag;
		int16_t target_level = generic_level_pa.target_level;
		if (trans_flag && remain_time.num_steps < generic_level_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		k_mutex_unlock(&mutex_level);
		generic_level_stat(model, ctx, level, trans_flag, target_level, remain_time);
		return 0;
	}
	return -1;
}

static int generic_level_srv_set_unack(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	(void)model;
	int16_t level = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	int32_t delay = 0;
	k_mutex_lock(&mutex_level, K_FOREVER);
	if (tid == generic_level_pa.level_tid && ctx->addr == generic_level_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_level);
		return 0;
	}
	if (level == generic_level_pa.target_level) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_level);
		return 0;
	}

	generic_level_pa.level = level;
	generic_level_pa.target_level = level;
	generic_level_pa.src = ctx->addr;
	generic_level_pa.level_tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		generic_level_pa.total_time.num_steps = trans_time & 0x3F;
		generic_level_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		generic_level_pa.remain_time = generic_level_pa.total_time;
		generic_level_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		generic_level_pa.total_time.num_steps = 0;
		generic_level_pa.total_time.step_resolution = 0;
		generic_level_pa.remain_time = generic_level_pa.total_time;
		generic_level_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_generic_level_server_set_t level_set;
		level_set.level = level;
		level_set.total_time = generic_level_pa.total_time;
		level_set.remaining_time = generic_level_pa.remain_time;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_generic_level_server_set_t));
		memcpy(p_evt->data, &level_set, sizeof(rtk_bt_mesh_generic_level_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_level);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&generic_level_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_level);
	return 0;
}

static int generic_level_srv_set(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	generic_level_srv_set_unack(model, ctx, buf);
	int16_t level = get_present_level();
	k_mutex_lock(&mutex_level, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = generic_level_pa.remain_time;
	bool trans_flag = generic_level_pa.transition_flag;
	int16_t target_level = generic_level_pa.target_level;
	if (trans_flag && remain_time.num_steps < generic_level_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_level);
	generic_level_stat(model, ctx, level, trans_flag, target_level, remain_time);
	return 0;
}

static int generic_level_srv_delta_set_unack(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	(void)model;
	int32_t delta_level = net_buf_simple_pull_le32(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	uint8_t trans_time = 0;
	int32_t delay = 0;
	k_mutex_lock(&mutex_level, K_FOREVER);
	if (tid == generic_level_pa.delta_tid && ctx->addr == generic_level_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_level);
		return 0;
	}

	int16_t level = get_present_level();
	generic_level_pa.level = level;
	generic_level_pa.target_level = level + delta_level;
	generic_level_pa.delta_level = delta_level;
	generic_level_pa.src = ctx->addr;
	generic_level_pa.delta_tid = tid;


	if (buf->len) {
		trans_time = net_buf_simple_pull_u8(buf);
		generic_level_pa.total_time.num_steps = trans_time & 0x3F;
		generic_level_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		generic_level_pa.remain_time = generic_level_pa.total_time;
		generic_level_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		generic_level_pa.total_time.num_steps = 0;
		generic_level_pa.total_time.step_resolution = 0;
		generic_level_pa.remain_time = generic_level_pa.total_time;
		generic_level_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_generic_level_server_set_t level_set;
		level_set.level = generic_level_pa.target_level;
		level_set.total_time = generic_level_pa.total_time;
		level_set.remaining_time = generic_level_pa.remain_time;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_generic_level_server_set_t));
		memcpy(p_evt->data, &level_set, sizeof(rtk_bt_mesh_generic_level_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_level);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&generic_level_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_level);
	return 0;
}

static int generic_level_srv_delta_set(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	generic_level_srv_delta_set_unack(model, ctx, buf);
	int16_t level = get_present_level();
	k_mutex_lock(&mutex_level, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = generic_level_pa.remain_time;
	bool trans_flag = generic_level_pa.transition_flag;
	int16_t target_level = generic_level_pa.target_level;
	if (trans_flag && remain_time.num_steps < generic_level_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_level);
	generic_level_stat(model, ctx, level, trans_flag, target_level, remain_time);
	return 0;
}

static int generic_level_srv_move_set_unack(const struct bt_mesh_model *model,
											struct bt_mesh_msg_ctx *ctx,
											struct net_buf_simple *buf)
{
	(void)model;
	int16_t move_level = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	uint8_t trans_time = 0;
	int32_t delay = 0;
	k_mutex_lock(&mutex_level, K_FOREVER);
	if (tid == generic_level_pa.move_tid && ctx->addr == generic_level_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_level);
		return 0;
	}
	rtk_bt_mesh_generic_transition_time_t trans_time_val = {0, 0};
	bool trans_flag = FALSE;

	if (buf->len) {
		trans_time = net_buf_simple_pull_u8(buf);
		if (trans_time != 0) {
			trans_flag = TRUE;
		}
		generic_level_pa.total_time.num_steps = trans_time & 0x3F;
		generic_level_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		generic_level_pa.remain_time = generic_level_pa.total_time;
		trans_time_val = generic_level_pa.total_time;
		generic_level_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	}

	int16_t level = get_present_level();
	generic_level_pa.level = level;
	generic_level_pa.target_level = level + trans_time_val.num_steps * move_level;
	generic_level_pa.move_level = move_level;
	generic_level_pa.src = ctx->addr;
	generic_level_pa.move_tid = tid;
	/**If no transition time in msg
	 * can get default transition time as transtime
	 * and use move speed to transtion
	 **/

	/*********Indicate app move msg info*********/
	rtk_bt_mesh_generic_level_server_set_move_t move_set;
	move_set.move_delta = move_level;
	move_set.target_level = generic_level_pa.target_level;
	move_set.total_time = trans_time_val;
	move_set.remaining_time = trans_time_val;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_LEVEL_SERVER_MODEL_SET_MOVE,
								sizeof(rtk_bt_mesh_generic_level_server_set_move_t));
	memcpy(p_evt->data, &move_set, sizeof(rtk_bt_mesh_generic_level_server_set_move_t));
	rtk_bt_evt_indicate(p_evt, NULL);

	/*****Need add timeout handler*****/
	if (trans_flag) {
		k_work_reschedule(&generic_level_pa.work, K_MSEC(delay));
	}
	k_mutex_unlock(&mutex_level);
	return 0;
}

static int generic_level_srv_move_set(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	generic_level_srv_move_set_unack(model, ctx, buf);
	int16_t level = get_present_level();
	k_mutex_lock(&mutex_level, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = generic_level_pa.remain_time;
	bool trans_flag = generic_level_pa.transition_flag;
	int16_t target_level = generic_level_pa.target_level;
	if (trans_flag && remain_time.num_steps < generic_level_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_level);
	generic_level_stat(model, ctx, level, trans_flag, target_level, remain_time);
	return 0;
}

const struct bt_mesh_model_op generic_level_srv_op[] = {
	{ OP_GENERIC_LEVEL_GET,               BT_MESH_LEN_EXACT(0),   generic_level_srv_get },
	{ OP_GENERIC_LEVEL_SET,               BT_MESH_LEN_MIN(3),     generic_level_srv_set },
	{ OP_GENERIC_LEVEL_SET_UNACK,         BT_MESH_LEN_MIN(3),     generic_level_srv_set_unack },
	{ OP_GENERIC_DELTA_SET,               BT_MESH_LEN_MIN(5),     generic_level_srv_delta_set },
	{ OP_GENERIC_DELTA_SET_UNACK,         BT_MESH_LEN_MIN(5),     generic_level_srv_delta_set_unack },
	{ OP_GENERIC_MOVE_SET,                BT_MESH_LEN_MIN(3),     generic_level_srv_move_set },
	{ OP_GENERIC_MOVE_SET_UNACK,          BT_MESH_LEN_MIN(3),     generic_level_srv_move_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LEVEL_CLIENT_MODEL
static const struct bt_mesh_model *generic_level_client_model = NULL;

static int generic_level_cli_init(const struct bt_mesh_model *model)
{
	generic_level_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_level_cli_cb = {
	.init = generic_level_cli_init,
};

static int generic_level_cli_status(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_level_client_status_t level_status;
		level_status.src = ctx->addr;
		level_status.present_level = net_buf_simple_pull_le16(buf);
		level_status.optional = FALSE;
		if (buf->len) {
			level_status.optional = TRUE;
			level_status.target_level = net_buf_simple_pull_le16(buf);
			level_status.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_generic_level_client_status_t));
		memcpy(p_evt->data, &level_status, sizeof(rtk_bt_mesh_generic_level_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op generic_level_cli_op[] = {
	{ OP_GENERIC_LEVEL_STATUS,            BT_MESH_LEN_MIN(2),     generic_level_cli_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_level_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LEVEL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LEVEL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_level_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
							 int16_t level, uint8_t tid, bool optional,
							 rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_GENERIC_LEVEL_SET : OP_GENERIC_LEVEL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LEVEL_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, level);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_delta_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
							 int32_t delta_level, uint8_t tid, bool optional,
							 rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 5;
	if (optional) {
		msg_len = 7;
	}
	uint32_t op = ack ? OP_GENERIC_DELTA_SET : OP_GENERIC_DELTA_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_DELTA_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le32(&buf, delta_level);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_move_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
							int16_t delta_level, uint8_t tid, bool optional,
							rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_GENERIC_MOVE_SET : OP_GENERIC_MOVE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MOVE_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, delta_level);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_level_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_GET: {
		rtk_bt_mesh_generic_level_get_t *level_get = (rtk_bt_mesh_generic_level_get_t *)p_cmd->param;
		ret = generic_level_get(generic_level_client_model, level_get->dst, level_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_SET: {
		rtk_bt_mesh_generic_level_set_t *level_set = (rtk_bt_mesh_generic_level_set_t *)p_cmd->param;
		ret = generic_level_set(generic_level_client_model, level_set->dst, level_set->app_key_index,
								level_set->level, level_set->tid, level_set->optional, level_set->trans_time, level_set->delay, level_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_DELTA_SET: {
		rtk_bt_mesh_generic_level_delta_set_t *delta_set = (rtk_bt_mesh_generic_level_delta_set_t *)p_cmd->param;
		ret = generic_delta_set(generic_level_client_model, delta_set->dst, delta_set->app_key_index,
								delta_set->delta_level, delta_set->tid, delta_set->optional,
								delta_set->trans_time, delta_set->delay, delta_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_LEVEL_CLIENT_MODEL_MOVE_SET: {
		rtk_bt_mesh_generic_level_move_set_t *move_set = (rtk_bt_mesh_generic_level_move_set_t *)p_cmd->param;
		ret = generic_move_set(generic_level_client_model, move_set->dst, move_set->app_key_index,
							   move_set->delta_level, move_set->tid, move_set->optional,
							   move_set->trans_time, move_set->delay, move_set->ack);
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

#define OP_GENERIC_ON_POWER_UP_GET              BT_MESH_MODEL_OP_2(0x82, 0x11)
#define OP_GENERIC_ON_POWER_UP_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x12)

#define OP_GENERIC_ON_POWER_UP_SET              BT_MESH_MODEL_OP_2(0x82, 0x13)
#define OP_GENERIC_ON_POWER_UP_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x14)

#if ((defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL))
static int generic_on_power_up_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_generic_on_power_up_t on_power_up)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ON_POWER_UP_STATUS, 1);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_ON_POWER_UP_STATUS);
	net_buf_simple_add_u8(&buf, on_power_up);
	return bt_mesh_msg_send(model, ctx, &buf);
}

#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SERVER_MODEL
static const struct bt_mesh_model *generic_power_on_off_server_model = NULL;

static int generic_power_on_off_srv_init(const struct bt_mesh_model *model)
{
	generic_power_on_off_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_srv_cb = {
	.init = generic_power_on_off_srv_init,
};

static rtk_bt_mesh_generic_on_power_up_t get_present_power_on_off(void)
{
	rtk_bt_mesh_generic_on_power_up_t get_data = {RTK_BT_MESH_GENERIC_ON_POWER_UP_OFF};
	rtk_bt_mesh_generic_power_on_off_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_power_on_off_server_get_t));
	p_get_data = (rtk_bt_mesh_generic_power_on_off_server_get_t *)p_evt->data;
	p_get_data->on_power_up = &get_data;
	rtk_bt_evt_indicate(p_evt, NULL);
	return get_data;
}

int generic_power_on_off_publish(const struct bt_mesh_model *model)
{
	rtk_bt_mesh_generic_on_power_up_t on_power_up = get_present_power_on_off();
	return generic_on_power_up_stat(model, NULL, on_power_up);
}

int generic_power_on_off_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	rtk_bt_mesh_generic_on_power_up_t on_power_up = get_present_power_on_off();
	bt_mesh_model_msg_init(msg, OP_GENERIC_ON_POWER_UP_STATUS);
	net_buf_simple_add_u8(msg, on_power_up);
	return 0;
}

static int generic_power_onoff_srv_get(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_on_power_up_t on_power_up = get_present_power_on_off();
		return generic_on_power_up_stat(model, ctx, on_power_up);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_onoff_srv_op[] = {
	{ OP_GENERIC_ON_POWER_UP_GET,         BT_MESH_LEN_EXACT(0),   generic_power_onoff_srv_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL
static const struct bt_mesh_model *generic_power_on_off_setup_server_model = NULL;

static int generic_power_on_off_setup_srv_init(const struct bt_mesh_model *model)
{
	generic_power_on_off_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_setup_srv_cb = {
	.init = generic_power_on_off_setup_srv_init,
};

static rtk_bt_mesh_generic_power_on_off_server_set_t on_power_up;
static int generic_power_onoff_srv_set_unack(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		on_power_up.on_power_up = net_buf_simple_pull_u8(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_generic_power_on_off_server_set_t));
		memcpy(p_evt->data, &on_power_up, sizeof(rtk_bt_mesh_generic_power_on_off_server_set_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_onoff_srv_set(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	if (!generic_power_onoff_srv_set_unack(model, ctx, buf)) {
		return generic_on_power_up_stat(model, ctx, on_power_up.on_power_up);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_onoff_setup_srv_op[] = {
	{ OP_GENERIC_ON_POWER_UP_SET,         BT_MESH_LEN_EXACT(1),   generic_power_onoff_srv_set },
	{ OP_GENERIC_ON_POWER_UP_SET_UNACK,   BT_MESH_LEN_EXACT(1),   generic_power_onoff_srv_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_ONOFF_CLIENT_MODEL
static const struct bt_mesh_model *generic_power_on_off_client_model = NULL;

static int generic_power_on_off_cli_init(const struct bt_mesh_model *model)
{
	generic_power_on_off_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_power_on_off_cli_cb = {
	.init = generic_power_on_off_cli_init,
};

static int generic_power_onoff_cli_status(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_power_on_off_client_status_t status;
		status.src = ctx->addr;
		status.on_power_up = net_buf_simple_pull_u8(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_generic_power_on_off_client_status_t));
		memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_generic_power_on_off_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_onoff_cli_op[] = {
	{ OP_GENERIC_ON_POWER_UP_STATUS,      BT_MESH_LEN_EXACT(1),   generic_power_onoff_cli_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_on_power_up_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ON_POWER_UP_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_ON_POWER_UP_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_on_power_up_set(const struct bt_mesh_model *model,
								   uint16_t dst, uint16_t app_key_index,
								   rtk_bt_mesh_generic_on_power_up_t on_power_up, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_GENERIC_ON_POWER_UP_SET : OP_GENERIC_ON_POWER_UP_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ON_POWER_UP_SET, 1);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_u8(&buf, on_power_up);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_power_on_off_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_GET: {
		rtk_bt_mesh_generic_on_power_up_get_t *power_get = (rtk_bt_mesh_generic_on_power_up_get_t *)p_cmd->param;
		ret = generic_on_power_up_get(generic_power_on_off_client_model, power_get->dst, power_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_ON_POWER_UP_CLIENT_MODEL_SET: {
		rtk_bt_mesh_generic_on_power_up_set_t *power_set = (rtk_bt_mesh_generic_on_power_up_set_t *)p_cmd->param;
		ret = generic_on_power_up_set(generic_power_on_off_client_model, power_set->dst, power_set->app_key_index,
									  power_set->on_power_up, power_set->ack);
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

#define OP_GENERIC_POWER_LEVEL_GET              BT_MESH_MODEL_OP_2(0x82, 0x15)
#define OP_GENERIC_POWER_LEVEL_SET              BT_MESH_MODEL_OP_2(0x82, 0x16)
#define OP_GENERIC_POWER_LEVEL_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x17)
#define OP_GENERIC_POWER_LEVEL_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x18)
#define OP_GENERIC_POWER_LAST_GET               BT_MESH_MODEL_OP_2(0x82, 0x19)
#define OP_GENERIC_POWER_LAST_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x1A)
#define OP_GENERIC_POWER_DEFAULT_GET            BT_MESH_MODEL_OP_2(0x82, 0x1B)
#define OP_GENERIC_POWER_DEFAULT_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x1C)
#define OP_GENERIC_POWER_RANGE_GET              BT_MESH_MODEL_OP_2(0x82, 0x1D)
#define OP_GENERIC_POWER_RANGE_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x1E)

#define OP_GENERIC_POWER_DEFAULT_SET            BT_MESH_MODEL_OP_2(0x82, 0x1F)
#define OP_GENERIC_POWER_DEFAULT_SET_UNACK      BT_MESH_MODEL_OP_2(0x82, 0x20)
#define OP_GENERIC_POWER_RANGE_SET              BT_MESH_MODEL_OP_2(0x82, 0x21)
#define OP_GENERIC_POWER_RANGE_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x22)

#if ((defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL))

static int generic_power_default_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t power)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_DEFAULT_STATUS, 2);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_DEFAULT_STATUS);
	net_buf_simple_add_le16(&buf, power);
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_power_range_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
									rtk_bt_mesh_generic_stat_t stat, uint16_t range_min, uint16_t range_max)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_RANGE_STATUS, 5);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_RANGE_STATUS);
	net_buf_simple_add_u8(&buf, stat);
	net_buf_simple_add_le16(&buf, range_min);
	net_buf_simple_add_le16(&buf, range_max);
	return bt_mesh_msg_send(model, ctx, &buf);
}

#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SERVER_MODEL
struct generic_power_t {
	uint16_t power;
	uint16_t target_power;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} generic_power_pa;

static const struct bt_mesh_model *generic_power_level_server_model = NULL;

struct k_mutex mutex_power;

void generic_power_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_power, K_FOREVER);
	if (!generic_power_pa.transition_flag) {
		generic_power_pa.remain_time.num_steps = 0;
		generic_power_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[generic_power_pa.remain_time.step_resolution];
	rtk_bt_mesh_generic_power_level_server_set_t level_set;
	level_set.power = generic_power_pa.target_power;
	level_set.total_time = generic_power_pa.total_time;
	level_set.remaining_time = generic_power_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_generic_power_level_server_set_t));
	memcpy(p_evt->data, &level_set, sizeof(rtk_bt_mesh_generic_power_level_server_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (generic_power_pa.remain_time.num_steps) {
		generic_power_pa.remain_time.num_steps--;
		k_work_reschedule(&generic_power_pa.work, K_MSEC(trans));
	} else {
		generic_power_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_power);
	return;
}

static int generic_power_level_srv_init(const struct bt_mesh_model *model)
{
	generic_power_level_server_model = model;
	k_work_init_delayable(&generic_power_pa.work, generic_power_timeout);
	k_mutex_init(&mutex_power);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int generic_power_level_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_power);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_generic_power_level_srv_cb = {
	.init = generic_power_level_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = generic_power_level_srv_deinit,
#endif
};

static uint16_t get_present_power_level(void)
{
	uint16_t power = 0;
	rtk_bt_mesh_generic_power_level_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_power_level_server_get_t));
	p_get_data = (rtk_bt_mesh_generic_power_level_server_get_t *)p_evt->data;
	p_get_data->power = &power;
	rtk_bt_evt_indicate(p_evt, NULL);
	return power;
}

static int generic_power_level_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
									uint16_t present_power, bool optional, uint16_t target_power,
									rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 2;
	if (optional) {
		msg_len = 5;
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_LEVEL_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_LEVEL_STATUS);
	net_buf_simple_add_le16(&buf, present_power);
	if (optional) {
		net_buf_simple_add_le16(&buf, target_power);
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

int generic_power_level_publish(const struct bt_mesh_model *model)
{
	uint16_t level = get_present_power_level();
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return generic_power_level_stat(model, NULL, level, FALSE, 0, remaining_time);
}

int generic_power_level_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	uint16_t level = get_present_power_level();
	bt_mesh_model_msg_init(msg, OP_GENERIC_POWER_LEVEL_STATUS);
	net_buf_simple_add_le16(msg, level);
	return 0;
}

static int generic_power_level_srv_get(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t power = get_present_power_level();
		k_mutex_lock(&mutex_power, K_FOREVER);
		rtk_bt_mesh_generic_transition_time_t remain_time = generic_power_pa.remain_time;
		bool trans_flag = generic_power_pa.transition_flag;
		int16_t target_power = generic_power_pa.target_power;
		if (trans_flag && remain_time.num_steps < generic_power_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		k_mutex_unlock(&mutex_power);
		generic_power_level_stat(model, ctx, power, trans_flag,
								 target_power, remain_time);
		return 0;
	}
	return -1;
}

static int generic_power_level_srv_set_unack(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	(void)model;
	uint16_t power = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	rtk_bt_mesh_generic_power_level_server_get_range_t range = {0, 0};
	rtk_bt_mesh_generic_power_level_server_direct_get_range_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_RANGE_GET,
								sizeof(rtk_bt_mesh_generic_power_level_server_direct_get_range_t));
	p_get_data = (rtk_bt_mesh_generic_power_level_server_direct_get_range_t *)p_evt->data;
	p_get_data->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	if ((0 != range.range_min) && (0 != range.range_max)) {
		power = CLAMP(power, range.range_min, range.range_max);
	}

	int32_t delay = 0;
	k_mutex_lock(&mutex_power, K_FOREVER);
	if (tid == generic_power_pa.tid && ctx->addr == generic_power_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_power);
		return 0;
	}
	if (power == generic_power_pa.target_power) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_power);
		return 0;
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_SET,
								sizeof(rtk_bt_mesh_generic_power_level_server_set_last_t));
	memcpy(p_evt->data, &power, sizeof(rtk_bt_mesh_generic_power_level_server_set_last_t));
	rtk_bt_evt_indicate(p_evt, NULL);

	generic_power_pa.power = power;
	generic_power_pa.target_power = power;
	generic_power_pa.src = ctx->addr;
	generic_power_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		generic_power_pa.total_time.num_steps = trans_time & 0x3F;
		generic_power_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		generic_power_pa.remain_time = generic_power_pa.total_time;
		generic_power_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		generic_power_pa.total_time.num_steps = 0;
		generic_power_pa.total_time.step_resolution = 0;
		generic_power_pa.remain_time = generic_power_pa.total_time;
		generic_power_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_generic_power_level_server_set_t power_set;
		power_set.power = power;
		power_set.total_time = generic_power_pa.total_time;
		power_set.remaining_time = generic_power_pa.remain_time;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_generic_power_level_server_set_t));
		memcpy(p_evt->data, &power_set, sizeof(rtk_bt_mesh_generic_power_level_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_power);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&generic_power_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_power);
	return 0;
}

static int generic_power_level_srv_set(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	generic_power_level_srv_set_unack(model, ctx, buf);
	uint16_t power = get_present_power_level();
	k_mutex_lock(&mutex_power, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = generic_power_pa.remain_time;
	bool trans_flag = generic_power_pa.transition_flag;
	int16_t target_power = generic_power_pa.target_power;
	if (trans_flag && remain_time.num_steps < generic_power_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_power);
	generic_power_level_stat(model, ctx, power, trans_flag,
							 target_power, remain_time);
	return 0;
}

static int generic_power_level_srv_last_get(const struct bt_mesh_model *model,
											struct bt_mesh_msg_ctx *ctx,
											struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t last_power;
		rtk_bt_mesh_generic_power_level_server_get_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_LAST_GET,
									sizeof(rtk_bt_mesh_generic_power_level_server_get_t));
		p_get_data = (rtk_bt_mesh_generic_power_level_server_get_t *)p_evt->data;
		p_get_data->power = &last_power;
		rtk_bt_evt_indicate(p_evt, NULL);

		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_GENERIC_POWER_LAST_STATUS, 2);
		bt_mesh_model_msg_init(&pmsg, OP_GENERIC_POWER_LAST_STATUS);
		net_buf_simple_add_le16(&pmsg, last_power);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static int generic_power_level_srv_default_get(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_power_level_server_get_t *p_get_data;
		uint16_t power;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_DEFAULT_GET,
									sizeof(rtk_bt_mesh_generic_power_level_server_get_t));
		p_get_data = (rtk_bt_mesh_generic_power_level_server_get_t *)p_evt->data;
		p_get_data->power = &power;
		rtk_bt_evt_indicate(p_evt, NULL);

		return generic_power_default_stat(model, ctx, power);
	}
	return -1;
}

static int generic_power_level_srv_range_get(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_power_level_server_direct_get_range_t *p_get_data;
		rtk_bt_mesh_generic_power_level_server_get_range_t range;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_RANGE_GET,
									sizeof(rtk_bt_mesh_generic_power_level_server_direct_get_range_t));
		p_get_data = (rtk_bt_mesh_generic_power_level_server_direct_get_range_t *)p_evt->data;
		p_get_data->value = &range;
		rtk_bt_evt_indicate(p_evt, NULL);

		return generic_power_range_stat(model, ctx, MESH_GENERIC_STAT_SUCCESS, range.range_min, range.range_max);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_level_srv_op[] = {
	{ OP_GENERIC_POWER_LEVEL_GET,               BT_MESH_LEN_EXACT(0),     generic_power_level_srv_get },
	{ OP_GENERIC_POWER_LEVEL_SET,               BT_MESH_LEN_MIN(3),       generic_power_level_srv_set },
	{ OP_GENERIC_POWER_LEVEL_SET_UNACK,         BT_MESH_LEN_MIN(3),       generic_power_level_srv_set_unack },
	{ OP_GENERIC_POWER_LAST_GET,                BT_MESH_LEN_EXACT(0),     generic_power_level_srv_last_get },
	{ OP_GENERIC_POWER_DEFAULT_GET,             BT_MESH_LEN_EXACT(0),     generic_power_level_srv_default_get },
	{ OP_GENERIC_POWER_RANGE_GET,               BT_MESH_LEN_EXACT(0),     generic_power_level_srv_range_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL
static const struct bt_mesh_model *generic_power_level_setup_server_model = NULL;

static int generic_power_level_setup_srv_init(const struct bt_mesh_model *model)
{
	generic_power_level_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_power_level_setup_srv_cb = {
	.init = generic_power_level_setup_srv_init,
};

static rtk_bt_mesh_generic_power_level_server_set_default_t default_power;
static rtk_bt_mesh_generic_power_level_server_set_range_t range_power;

static int generic_power_level_setup_srv_default_set_unack(const struct bt_mesh_model *model,
														   struct bt_mesh_msg_ctx *ctx,
														   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		default_power.power = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_DEFAULT_SET,
									sizeof(rtk_bt_mesh_generic_power_level_server_set_default_t));
		memcpy(p_evt->data, &default_power, sizeof(rtk_bt_mesh_generic_power_level_server_set_default_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_level_setup_srv_default_set(const struct bt_mesh_model *model,
													 struct bt_mesh_msg_ctx *ctx,
													 struct net_buf_simple *buf)
{
	if (!generic_power_level_setup_srv_default_set_unack(model, ctx, buf)) {
		return generic_power_default_stat(model, ctx, default_power.power);
	}
	return -1;
}

static int generic_power_level_setup_srv_range_set_unack(const struct bt_mesh_model *model,
														 struct bt_mesh_msg_ctx *ctx,
														 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		range_power.range_min = net_buf_simple_pull_le16(buf);
		range_power.range_max = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_RANGE_SET,
									sizeof(rtk_bt_mesh_generic_power_level_server_set_range_t));
		memcpy(p_evt->data, &range_power, sizeof(rtk_bt_mesh_generic_power_level_server_set_range_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_level_setup_srv_range_set(const struct bt_mesh_model *model,
												   struct bt_mesh_msg_ctx *ctx,
												   struct net_buf_simple *buf)
{
	if (!generic_power_level_setup_srv_range_set_unack(model, ctx, buf)) {
		return generic_power_range_stat(model, ctx, MESH_GENERIC_STAT_SUCCESS, range_power.range_min, range_power.range_max);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_level_setup_srv_op[] = {
	{ OP_GENERIC_POWER_DEFAULT_SET,               BT_MESH_LEN_EXACT(2),     generic_power_level_setup_srv_default_set },
	{ OP_GENERIC_POWER_DEFAULT_SET_UNACK,         BT_MESH_LEN_EXACT(2),     generic_power_level_setup_srv_default_set_unack },
	{ OP_GENERIC_POWER_RANGE_SET,                 BT_MESH_LEN_EXACT(4),     generic_power_level_setup_srv_range_set },
	{ OP_GENERIC_POWER_RANGE_SET_UNACK,           BT_MESH_LEN_EXACT(4),     generic_power_level_setup_srv_range_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_POWER_LEVEL_CLIENT_MODEL
static const struct bt_mesh_model *generic_power_level_client_model = NULL;

static int generic_power_level_cli_init(const struct bt_mesh_model *model)
{
	generic_power_level_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_power_level_cli_cb = {
	.init = generic_power_level_cli_init,
};

static int generic_power_level_cli_status(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_power_level_client_status_t power_status;
		power_status.src = ctx->addr;
		power_status.present_power = net_buf_simple_pull_le16(buf);
		power_status.optional = FALSE;
		if (buf->len) {
			power_status.optional = TRUE;
			power_status.target_power = net_buf_simple_pull_le16(buf);
			power_status.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
		}

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_generic_power_level_client_status_t));
		memcpy(p_evt->data, &power_status, sizeof(rtk_bt_mesh_generic_power_level_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_level_cli_last_status(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_power_level_client_status_simple_t last_status;
		last_status.src = ctx->addr;
		last_status.power = net_buf_simple_pull_le16(buf);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_STATUS,
									sizeof(rtk_bt_mesh_generic_power_level_client_status_simple_t));
		memcpy(p_evt->data, &last_status, sizeof(rtk_bt_mesh_generic_power_level_client_status_simple_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_level_cli_default_status(const struct bt_mesh_model *model,
												  struct bt_mesh_msg_ctx *ctx,
												  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_power_level_client_status_simple_t default_status;
		default_status.src = ctx->addr;
		default_status.power = net_buf_simple_pull_le16(buf);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_STATUS,
									sizeof(rtk_bt_mesh_generic_power_level_client_status_simple_t));
		memcpy(p_evt->data, &default_status, sizeof(rtk_bt_mesh_generic_power_level_client_status_simple_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_power_level_cli_range_status(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx,
												struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_power_level_client_status_range_t range_status;
		range_status.src = ctx->addr;
		range_status.stat = net_buf_simple_pull_u8(buf);
		range_status.range_min = net_buf_simple_pull_le16(buf);
		range_status.range_max = net_buf_simple_pull_le16(buf);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL, RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_STATUS,
									sizeof(rtk_bt_mesh_generic_power_level_client_status_range_t));
		memcpy(p_evt->data, &range_status, sizeof(rtk_bt_mesh_generic_power_level_client_status_range_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op generic_power_level_cli_op[] = {
	{ OP_GENERIC_POWER_LEVEL_STATUS,              BT_MESH_LEN_MIN(2),       generic_power_level_cli_status },
	{ OP_GENERIC_POWER_LAST_STATUS,               BT_MESH_LEN_EXACT(2),     generic_power_level_cli_last_status },
	{ OP_GENERIC_POWER_DEFAULT_STATUS,            BT_MESH_LEN_EXACT(2),     generic_power_level_cli_default_status },
	{ OP_GENERIC_POWER_RANGE_STATUS,              BT_MESH_LEN_EXACT(5),     generic_power_level_cli_range_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_power_level_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_LEVEL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_LEVEL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_level_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
								   uint16_t power, uint8_t tid, bool optional,
								   rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_GENERIC_POWER_LEVEL_SET : OP_GENERIC_POWER_LEVEL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_LEVEL_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, power);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_last_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_LAST_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_LAST_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_default_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_DEFAULT_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_DEFAULT_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_default_set(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index, uint16_t power, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_GENERIC_POWER_DEFAULT_SET : OP_GENERIC_POWER_DEFAULT_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_DEFAULT_SET, 2);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, power);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_range_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_RANGE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_POWER_RANGE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_power_range_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
								   uint16_t range_min, uint16_t range_max, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_GENERIC_POWER_RANGE_SET : OP_GENERIC_POWER_RANGE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_POWER_RANGE_SET, 4);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, range_min);
	net_buf_simple_add_le16(&buf, range_max);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_power_level_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_GET: {
		rtk_bt_mesh_generic_power_level_get_t *level_get = (rtk_bt_mesh_generic_power_level_get_t *)p_cmd->param;
		ret = generic_power_level_get(generic_power_level_client_model, level_get->dst, level_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_SET: {
		rtk_bt_mesh_generic_power_level_set_t *level_set = (rtk_bt_mesh_generic_power_level_set_t *)p_cmd->param;
		ret = generic_power_level_set(generic_power_level_client_model, level_set->dst, level_set->app_key_index,
									  level_set->power, level_set->tid, level_set->optional, level_set->trans_time, level_set->delay, level_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_LAST_GET: {
		rtk_bt_mesh_generic_power_level_last_get_t *last_get = (rtk_bt_mesh_generic_power_level_last_get_t *)p_cmd->param;
		ret = generic_power_last_get(generic_power_level_client_model, last_get->dst, last_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_GET: {
		rtk_bt_mesh_generic_power_level_default_get_t *default_get = (rtk_bt_mesh_generic_power_level_default_get_t *)p_cmd->param;
		ret = generic_power_default_get(generic_power_level_client_model, default_get->dst, default_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_DEFAULT_SET: {
		rtk_bt_mesh_generic_power_level_default_set_t *default_set = (rtk_bt_mesh_generic_power_level_default_set_t *)p_cmd->param;
		ret = generic_power_default_set(generic_power_level_client_model, default_set->dst, default_set->app_key_index, default_set->power, default_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_GET: {
		rtk_bt_mesh_generic_power_level_range_get_t *range_get = (rtk_bt_mesh_generic_power_level_range_get_t *)p_cmd->param;
		ret = generic_power_range_get(generic_power_level_client_model, range_get->dst, range_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_RANGE_SET: {
		rtk_bt_mesh_generic_power_level_range_set_t *range_set = (rtk_bt_mesh_generic_power_level_range_set_t *)p_cmd->param;
		ret = generic_power_range_set(generic_power_level_client_model, range_set->dst, range_set->app_key_index,
									  range_set->range_min, range_set->range_max, range_set->ack);
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

#define OP_GENERIC_BATTERY_GET              BT_MESH_MODEL_OP_2(0x82, 0x23)
#define OP_GENERIC_BATTERY_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x24)

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_SERVER_MODEL
static const struct bt_mesh_model *generic_battery_server_model = NULL;

static int generic_battery_srv_init(const struct bt_mesh_model *model)
{
	generic_battery_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_battery_srv_cb = {
	.init = generic_battery_srv_init,
};

static __inline void time_to_array(uint8_t *buf, uint32_t time)
{
	for (int i = 2; i >= 0; --i) {
		buf[i] = (uint8_t)(time & 0xff);
		time >>= 8;
	}
}

static int generic_battery_stat(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx, uint8_t battery_level,
								uint32_t time_to_discharge, uint32_t time_to_charge,
								rtk_bt_mesh_generic_battery_flags_t flags)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_BATTERY_STATUS, 8);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_BATTERY_STATUS);
	uint8_t discharge[4], charge[4];
	net_buf_simple_add_u8(&buf, battery_level);
	time_to_array(discharge, time_to_discharge);
	time_to_array(charge, time_to_charge);
	net_buf_simple_add_mem(&buf, discharge, 3);
	net_buf_simple_add_mem(&buf, charge, 3);
	net_buf_simple_add_mem(&buf, &flags, sizeof(rtk_bt_mesh_generic_battery_flags_t));
	return bt_mesh_msg_send(model, ctx, &buf);
}

int generic_battery_publish(const struct bt_mesh_model *model)
{
	rtk_bt_mesh_generic_battery_server_get_t battery;
	rtk_bt_mesh_generic_battery_server_direct_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_SERVER_MODEL, RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_battery_server_direct_get_t));
	p_get_data = (rtk_bt_mesh_generic_battery_server_direct_get_t *)p_evt->data;
	p_get_data->value = &battery;
	rtk_bt_evt_indicate(p_evt, NULL);

	return generic_battery_stat(model, NULL, battery.battery_level, battery.time_to_discharge,
								battery.time_to_charge, battery.flags);
}

int generic_battery_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	rtk_bt_mesh_generic_battery_server_get_t battery;
	rtk_bt_mesh_generic_battery_server_direct_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_SERVER_MODEL, RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_generic_battery_server_direct_get_t));
	p_get_data = (rtk_bt_mesh_generic_battery_server_direct_get_t *)p_evt->data;
	p_get_data->value = &battery;
	rtk_bt_evt_indicate(p_evt, NULL);

	bt_mesh_model_msg_init(msg, OP_GENERIC_BATTERY_STATUS);
	uint8_t discharge[4], charge[4];
	net_buf_simple_add_u8(msg, battery.battery_level);
	time_to_array(discharge, battery.time_to_discharge);
	time_to_array(charge, battery.time_to_charge);
	net_buf_simple_add_mem(msg, discharge, 3);
	net_buf_simple_add_mem(msg, charge, 3);
	net_buf_simple_add_mem(msg, &battery.flags, sizeof(rtk_bt_mesh_generic_battery_flags_t));
	return 0;
}

static int generic_battery_srv_get(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_battery_server_get_t battery;
		rtk_bt_mesh_generic_battery_server_direct_get_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_SERVER_MODEL, RTK_BT_MESH_GENERIC_BATTERY_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_generic_battery_server_direct_get_t));
		p_get_data = (rtk_bt_mesh_generic_battery_server_direct_get_t *)p_evt->data;
		p_get_data->value = &battery;
		rtk_bt_evt_indicate(p_evt, NULL);

		return generic_battery_stat(model, ctx, battery.battery_level, battery.time_to_discharge,
									battery.time_to_charge, battery.flags);
	}
	return -1;
}

const struct bt_mesh_model_op generic_battery_srv_op[] = {
	{ OP_GENERIC_BATTERY_GET,               BT_MESH_LEN_EXACT(0),     generic_battery_srv_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_BATTERY_CLIENT_MODEL
static const struct bt_mesh_model *generic_battery_client_model = NULL;

static int generic_battery_cli_init(const struct bt_mesh_model *model)
{
	generic_battery_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_battery_cli_cb = {
	.init = generic_battery_cli_init,
};

static __inline uint32_t array_to_time(uint8_t *buf)
{
	// diag_assert(NULL != buf);
	uint32_t time = 0;
	for (int i = 2; i >= 0; --i) {
		time <<= 8;
		time |= buf[i];
	}

	return time;
}

static int generic_battery_cli_status(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_battery_client_status_t battery_status;
		battery_status.src = ctx->addr;
		battery_status.battery_level = net_buf_simple_pull_u8(buf);
		battery_status.time_to_discharge = array_to_time(net_buf_simple_pull_mem(buf, 3));
		battery_status.time_to_charge = array_to_time(net_buf_simple_pull_mem(buf, 3));
		battery_status.flags = *(rtk_bt_mesh_generic_battery_flags_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_battery_flags_t)));

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_BATTERY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_generic_battery_client_status_t));
		memcpy(p_evt->data, &battery_status, sizeof(rtk_bt_mesh_generic_battery_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op generic_battery_cli_op[] = {
	{ OP_GENERIC_BATTERY_STATUS,            BT_MESH_LEN_EXACT(8),     generic_battery_cli_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_battery_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_BATTERY_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_BATTERY_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_battery_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_BATTERY_CLIENT_MODEL_GET: {
		rtk_bt_mesh_generic_battery_get_t *battery_get = (rtk_bt_mesh_generic_battery_get_t *)p_cmd->param;
		ret = generic_battery_get(generic_battery_client_model, battery_get->dst, battery_get->app_key_index);
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

#define OP_GENERIC_LOCATION_GLOBAL_GET              BT_MESH_MODEL_OP_2(0x82, 0x25)
#define OP_GENERIC_LOCATION_GLOBAL_STATUS           BT_MESH_MODEL_OP_1(0x40)
#define OP_GENERIC_LOCATION_LOCAL_GET               BT_MESH_MODEL_OP_2(0x82, 0x26)
#define OP_GENERIC_LOCATION_LOCAL_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x27)

#define OP_GENERIC_LOCATION_GLOBAL_SET              BT_MESH_MODEL_OP_1(0x41)
#define OP_GENERIC_LOCATION_GLOBAL_SET_UNACK        BT_MESH_MODEL_OP_1(0x42)
#define OP_GENERIC_LOCATION_LOCAL_SET               BT_MESH_MODEL_OP_2(0x82, 0x28)
#define OP_GENERIC_LOCATION_LOCAL_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x29)

#if ((defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL))
static int generic_location_global_status(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  rtk_bt_mesh_generic_location_global_t global)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_GLOBAL_STATUS, 10);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LOCATION_GLOBAL_STATUS);
	net_buf_simple_add_le32(&buf, global.global_latitude);
	net_buf_simple_add_le32(&buf, global.global_longitude);
	net_buf_simple_add_le16(&buf, global.global_altitude);
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_location_local_status(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 rtk_bt_mesh_generic_location_local_t local)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_LOCAL_STATUS, 9);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LOCATION_LOCAL_STATUS);
	net_buf_simple_add_le16(&buf, local.local_north);
	net_buf_simple_add_le16(&buf, local.local_east);
	net_buf_simple_add_le16(&buf, local.local_altitude);
	net_buf_simple_add_u8(&buf, local.floor_num);
	net_buf_simple_add_mem(&buf, &local.uncertainty, sizeof(rtk_bt_mesh_generic_location_uncertainty_t));
	return bt_mesh_msg_send(model, ctx, &buf);
}

rtk_bt_mesh_generic_location_global_t get_present_global(const struct bt_mesh_model *model)
{
	uint8_t group = RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SERVER_MODEL;
	uint8_t evt = RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_GLOBAL_GET;
	if (model->id == 0x100f) {
		group = RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL;
		evt = RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_GET;
	}
	rtk_bt_mesh_generic_location_global_t global;
	rtk_bt_mesh_generic_location_server_direct_get_global_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(group, evt, sizeof(rtk_bt_mesh_generic_location_server_direct_get_global_t));
	p_get_data = (rtk_bt_mesh_generic_location_server_direct_get_global_t *)p_evt->data;
	p_get_data->value = &global;
	rtk_bt_evt_indicate(p_evt, NULL);
	return global;
}

rtk_bt_mesh_generic_location_local_t get_present_local(const struct bt_mesh_model *model)
{
	uint8_t group = RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SERVER_MODEL;
	uint8_t evt = RTK_BT_MESH_GENERIC_LOCATION_SERVER_MODEL_LOCAL_GET;
	if (model->id == 0x100f) {
		group = RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL;
		evt = RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_GET;
	}
	rtk_bt_mesh_generic_location_local_t local;
	rtk_bt_mesh_generic_location_server_direct_get_local_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(group, evt,
								sizeof(rtk_bt_mesh_generic_location_server_direct_get_local_t));
	p_get_data = (rtk_bt_mesh_generic_location_server_direct_get_local_t *)p_evt->data;
	p_get_data->value = &local;
	rtk_bt_evt_indicate(p_evt, NULL);
	return local;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SERVER_MODEL
static const struct bt_mesh_model *generic_location_server_model = NULL;
static bool location_pub_flag = FALSE;

static int generic_location_srv_init(const struct bt_mesh_model *model)
{
	generic_location_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_location_srv_cb = {
	.init = generic_location_srv_init,
};

int generic_location_publish(const struct bt_mesh_model *model, bool flag)
{
	if (flag) {
		rtk_bt_mesh_generic_location_global_t global = get_present_global(model);
		return generic_location_global_status(model, NULL, global);
	} else {
		rtk_bt_mesh_generic_location_local_t local = get_present_local(model);
		return generic_location_local_status(model, NULL, local);
	}
}

int generic_location_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	if (location_pub_flag) {
		bt_mesh_model_msg_init(msg, OP_GENERIC_LOCATION_GLOBAL_STATUS);
		rtk_bt_mesh_generic_location_global_t global = get_present_global(model);
		net_buf_simple_add_mem(msg, &global, sizeof(rtk_bt_mesh_generic_location_global_t));
	} else {
		bt_mesh_model_msg_init(msg, OP_GENERIC_LOCATION_LOCAL_STATUS);
		rtk_bt_mesh_generic_location_local_t local = get_present_local(model);
		net_buf_simple_add_mem(msg, &local, sizeof(rtk_bt_mesh_generic_location_local_t));
	}
	return 0;
}

static int generic_location_srv_global_get(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_location_global_t global = get_present_global(model);
		return generic_location_global_status(model, ctx, global);
	}
	return -1;
}

static int generic_location_srv_local_get(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_generic_location_local_t local = get_present_local(model);
		return generic_location_local_status(model, ctx, local);
	}
	return -1;
}

const struct bt_mesh_model_op generic_location_srv_op[] = {
	{ OP_GENERIC_LOCATION_GLOBAL_GET,               BT_MESH_LEN_EXACT(0),     generic_location_srv_global_get },
	{ OP_GENERIC_LOCATION_LOCAL_GET,                BT_MESH_LEN_EXACT(0),     generic_location_srv_local_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_SETUP_SERVER_MODEL
static const struct bt_mesh_model *generic_location_setup_server_model = NULL;

static int generic_location_setup_srv_init(const struct bt_mesh_model *model)
{
	generic_location_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_location_setup_srv_cb = {
	.init = generic_location_setup_srv_init,
};

static int generic_location_setup_srv_global_set_unack(const struct bt_mesh_model *model,
													   struct bt_mesh_msg_ctx *ctx,
													   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_generic_location_server_set_global_t global;
		global.global_latitude = net_buf_simple_pull_le32(buf);
		global.global_longitude = net_buf_simple_pull_le32(buf);
		global.global_altitude = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL, RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_GLOBAL_SET,
									sizeof(rtk_bt_mesh_generic_location_server_set_global_t));
		memcpy(p_evt->data, &global, sizeof(rtk_bt_mesh_generic_location_server_set_global_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_location_setup_srv_global_set(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	if (!generic_location_setup_srv_global_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_location_global_t global = get_present_global(model);
		return generic_location_global_status(model, ctx, global);
	}
	return -1;
}

static int generic_location_setup_srv_local_set_unack(const struct bt_mesh_model *model,
													  struct bt_mesh_msg_ctx *ctx,
													  struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_generic_location_server_set_local_t local;
		local.local_north = net_buf_simple_pull_le16(buf);
		local.local_east = net_buf_simple_pull_le16(buf);
		local.local_altitude = net_buf_simple_pull_le16(buf);
		local.floor_num = net_buf_simple_pull_u8(buf);
		local.uncertainty = *((rtk_bt_mesh_generic_location_uncertainty_t *) net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_location_uncertainty_t)));
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL, RTK_BT_MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_LOCAL_SET,
									sizeof(rtk_bt_mesh_generic_location_server_set_local_t));
		memcpy(p_evt->data, &local, sizeof(rtk_bt_mesh_generic_location_server_set_local_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_location_setup_srv_local_set(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx,
												struct net_buf_simple *buf)
{
	if (!generic_location_setup_srv_local_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_location_local_t local = get_present_local(model);
		return generic_location_local_status(model, ctx, local);
	}
	return -1;
}

const struct bt_mesh_model_op generic_location_setup_srv_op[] = {
	{ OP_GENERIC_LOCATION_GLOBAL_SET,               BT_MESH_LEN_EXACT(10),     generic_location_setup_srv_global_set },
	{ OP_GENERIC_LOCATION_GLOBAL_SET_UNACK,         BT_MESH_LEN_EXACT(10),     generic_location_setup_srv_global_set_unack },
	{ OP_GENERIC_LOCATION_LOCAL_SET,                BT_MESH_LEN_EXACT(9),      generic_location_setup_srv_local_set },
	{ OP_GENERIC_LOCATION_LOCAL_SET_UNACK,          BT_MESH_LEN_EXACT(9),      generic_location_setup_srv_local_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_LOCATION_CLIENT_MODEL
static const struct bt_mesh_model *generic_location_client_model = NULL;

static int generic_location_cli_init(const struct bt_mesh_model *model)
{
	generic_location_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_location_cli_cb = {
	.init = generic_location_cli_init,
};

static int generic_location_cli_global_status(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_location_client_status_global_t global_status;
		global_status.src = ctx->addr;
		global_status.global.global_latitude = net_buf_simple_pull_le32(buf);
		global_status.global.global_longitude = net_buf_simple_pull_le32(buf);
		global_status.global.global_altitude = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_STATUS,
									sizeof(rtk_bt_mesh_generic_location_client_status_global_t));
		memcpy(p_evt->data, &global_status, sizeof(rtk_bt_mesh_generic_location_client_status_global_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_location_cli_local_status(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_generic_location_client_status_local_t local_status;
		local_status.src = ctx->addr;
		local_status.local.local_north = net_buf_simple_pull_le16(buf);
		local_status.local.local_east = net_buf_simple_pull_le16(buf);
		local_status.local.local_altitude = net_buf_simple_pull_le16(buf);
		local_status.local.floor_num = net_buf_simple_pull_u8(buf);
		local_status.local.uncertainty = *((rtk_bt_mesh_generic_location_uncertainty_t *) net_buf_simple_pull_mem(buf,
																												  sizeof(rtk_bt_mesh_generic_location_uncertainty_t)));
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_LOCATION_CLIENT_MODEL, RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_STATUS,
									sizeof(rtk_bt_mesh_generic_location_client_status_local_t));
		memcpy(p_evt->data, &local_status, sizeof(rtk_bt_mesh_generic_location_client_status_local_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op generic_location_cli_op[] = {
	{ OP_GENERIC_LOCATION_GLOBAL_STATUS,               BT_MESH_LEN_EXACT(10),     generic_location_cli_global_status },
	{ OP_GENERIC_LOCATION_LOCAL_STATUS,                BT_MESH_LEN_EXACT(9),      generic_location_cli_local_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_location_global_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_GLOBAL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LOCATION_GLOBAL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_location_global_set(const struct bt_mesh_model *model, uint16_t dst,
									   uint16_t app_key_index, rtk_bt_mesh_generic_location_global_t global, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_GENERIC_LOCATION_GLOBAL_SET : OP_GENERIC_LOCATION_GLOBAL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_GLOBAL_SET, 10);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_mem(&buf, &global, sizeof(rtk_bt_mesh_generic_location_global_t));
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_location_local_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_LOCAL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_LOCATION_LOCAL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_location_local_set(const struct bt_mesh_model *model, uint16_t dst,
									  uint16_t app_key_index, rtk_bt_mesh_generic_location_local_t local, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_GENERIC_LOCATION_LOCAL_SET : OP_GENERIC_LOCATION_LOCAL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_LOCATION_LOCAL_SET, 9);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_mem(&buf, &local, sizeof(rtk_bt_mesh_generic_location_local_t));
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_location_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_GET: {
		rtk_bt_mesh_generic_location_global_get_t *global_get = (rtk_bt_mesh_generic_location_global_get_t *)p_cmd->param;
		ret = generic_location_global_get(generic_location_client_model, global_get->dst, global_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_GLOBAL_SET: {
		rtk_bt_mesh_generic_location_global_set_t *global_set = (rtk_bt_mesh_generic_location_global_set_t *)p_cmd->param;
		ret = generic_location_global_set(generic_location_client_model, global_set->dst, global_set->app_key_index, global_set->global, global_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_GET: {
		rtk_bt_mesh_generic_location_local_get_t *local_get = (rtk_bt_mesh_generic_location_local_get_t *)p_cmd->param;
		ret = generic_location_local_get(generic_location_client_model, local_get->dst, local_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_LOCATION_CLIENT_MODEL_LOCAL_SET: {
		rtk_bt_mesh_generic_location_local_set_t *local_set = (rtk_bt_mesh_generic_location_local_set_t *)p_cmd->param;
		ret = generic_location_local_set(generic_location_client_model, local_set->dst, local_set->app_key_index, local_set->local, local_set->ack);
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

#define OP_GENERIC_MANUFACTURER_PROPERTIES_GET              BT_MESH_MODEL_OP_2(0x82, 0x2A)
#define OP_GENERIC_MANUFACTURER_PROPERTIES_STATUS           BT_MESH_MODEL_OP_1(0x43)
#define OP_GENERIC_MANUFACTURER_PROPERTY_GET                BT_MESH_MODEL_OP_2(0x82, 0x2B)
#define OP_GENERIC_MANUFACTURER_PROPERTY_SET                BT_MESH_MODEL_OP_1(0x44)
#define OP_GENERIC_MANUFACTURER_PROPERTY_SET_UNACK          BT_MESH_MODEL_OP_1(0x45)
#define OP_GENERIC_MANUFACTURER_PROPERTY_STATUS             BT_MESH_MODEL_OP_1(0x46)

#define OP_GENERIC_ADMIN_PROPERTIES_GET                     BT_MESH_MODEL_OP_2(0x82, 0x2C)
#define OP_GENERIC_ADMIN_PROPERTIES_STATUS                  BT_MESH_MODEL_OP_1(0x47)
#define OP_GENERIC_ADMIN_PROPERTY_GET                       BT_MESH_MODEL_OP_2(0x82, 0x2D)
#define OP_GENERIC_ADMIN_PROPERTY_SET                       BT_MESH_MODEL_OP_1(0x48)
#define OP_GENERIC_ADMIN_PROPERTY_SET_UNACK                 BT_MESH_MODEL_OP_1(0x49)
#define OP_GENERIC_ADMIN_PROPERTY_STATUS                    BT_MESH_MODEL_OP_1(0x4A)

#define OP_GENERIC_USER_PROPERTIES_GET                      BT_MESH_MODEL_OP_2(0x82, 0x2E)
#define OP_GENERIC_USER_PROPERTIES_STATUS                   BT_MESH_MODEL_OP_1(0x4B)
#define OP_GENERIC_USER_PROPERTY_GET                        BT_MESH_MODEL_OP_2(0x82, 0x2F)
#define OP_GENERIC_USER_PROPERTY_SET                        BT_MESH_MODEL_OP_1(0x4C)
#define OP_GENERIC_USER_PROPERTY_SET_UNACK                  BT_MESH_MODEL_OP_1(0x4D)
#define OP_GENERIC_USER_PROPERTY_STATUS                     BT_MESH_MODEL_OP_1(0x4E)

#define OP_GENERIC_CLIENT_PROPERTIES_GET                    BT_MESH_MODEL_OP_1(0x4F)
#define OP_GENERIC_CLIENT_PROPERTIES_STATUS                 BT_MESH_MODEL_OP_1(0x50)

#if ((defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL))
typedef struct {
	rtk_bt_mesh_generic_property_db_t *pproperties;
	uint16_t num_properties;
} generic_property_info_t;

#define GENERIC_PROPERTY_ID_PROHIBITED                      0x00

#define GENERIC_PROPERTY_ACCESS_NOT_TO_USER                 0
#define GENERIC_PROPERTY_ACCESS_READ                        1
#define GENERIC_PROPERTY_ACCESS_WRITE                       2
#define GENERIC_PROPERTY_ACCESS_READ_AND_WRITE              3

void generic_property_server_set_db(const struct bt_mesh_model *model,
									rtk_bt_mesh_generic_property_db_t *pproperties, uint16_t num_properties)
{
	generic_property_info_t *pinfo = model->rt->user_data;
	pinfo->pproperties = pproperties;
	pinfo->num_properties = num_properties;
}

rtk_bt_mesh_generic_property_db_t *generic_property_find(const struct bt_mesh_model *model,
														 uint16_t property_id)
{
	generic_property_info_t *pinfo = model->rt->user_data;
	rtk_bt_mesh_generic_property_db_t *pdb = NULL;
	for (uint16_t i = 0; i < pinfo->num_properties; ++i) {
		if (pinfo->pproperties[i].property_id == property_id) {
			pdb = &pinfo->pproperties[i];
		}
	}

	return pdb;
}
#endif

#if defined(BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ADMIN_PROPERTY_SERVER_MODEL
static const struct bt_mesh_model *generic_admin_property_server_model = NULL;

static int generic_admin_property_srv_init(const struct bt_mesh_model *model)
{
	generic_admin_property_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_admin_property_srv_cb = {
	.init = generic_admin_property_srv_init,
};

uint8_t admin_property_store_value[3] = {0x1, 0x1, 0x2};
uint8_t admin_property_store_value2[2] = {0x3, 0x5};

rtk_bt_mesh_generic_property_db_t admin_property_db[] = {
	{1, 3, admin_property_store_value, 3},
	{2, 3, admin_property_store_value2, 2}
};

generic_property_info_t admin_property_info = {admin_property_db, 2};

static int generic_admin_properties_status(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   const rtk_bt_mesh_generic_property_db_t *pproperties,
										   uint16_t num_properties)
{
	uint16_t msg_len = 2 * num_properties;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTIES_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_ADMIN_PROPERTIES_STATUS);
	for (int i = 0; i < num_properties; i++) {
		net_buf_simple_add_le16(&buf, pproperties[i].property_id);
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_admin_property_status(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 uint16_t property_id,
										 const rtk_bt_mesh_generic_property_db_t *pdb)
{
	if (NULL != pdb) {
		uint16_t msg_len = 3 + pdb->value_len;
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTY_STATUS, msg_len);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_ADMIN_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		net_buf_simple_add_u8(&buf, pdb->property_access);
		net_buf_simple_add_mem(&buf, pdb->pproperty_value, pdb->value_len);
		return bt_mesh_msg_send(model, ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTY_STATUS, 2);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_ADMIN_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
}

static int generic_admin_property_srv_props_get(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx,
												struct net_buf_simple *buf)
{
	if (!buf->len) {
		generic_property_info_t *pinfo = model->rt->user_data;
		return generic_admin_properties_status(model, ctx, pinfo->pproperties, pinfo->num_properties);
	}
	return -1;
}

/**
 * This part is to verify if realated database parameter can be get from app layer
*/
#if 0
static int generic_admin_property_srv_get_para_from_app(uint16_t property_id)
{
	rtk_bt_mesh_generic_property_server_para_t para_get;
	rtk_bt_mesh_generic_property_server_para_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PARAMETER_GET,
								sizeof(rtk_bt_mesh_generic_property_server_para_get_t));
	p_get_data = (rtk_bt_mesh_generic_property_server_para_get_t *)p_evt->data;
	p_get_data->property_id = property_id;
	p_get_data->pvalue = &para_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, property access %d, value len %d \r\n", para_get.pdb, para_get.property_access, para_get.value_len);

	uint8_t admin_pvalue_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	rtk_bt_mesh_generic_property_server_value_get_t *val_get;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_VALUE_GET,
								sizeof(rtk_bt_mesh_generic_property_server_value_get_t) + para_get.value_len);
	val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)p_evt->data;
	val_get->property_id = property_id;
	val_get->value_len = para_get.value_len;
	if (para_get.value_len > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The len of generic property data is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, para_get.value_len,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		val_get->value_len = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	val_get->pproperty_value = admin_pvalue_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMPA("", admin_pvalue_store, val_get->value_len);

	rtk_bt_mesh_generic_property_server_num_ids_t num_get;
	rtk_bt_mesh_generic_property_server_num_ids_get_t *p_get_id_num;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_NUM_IDS_GET,
								sizeof(rtk_bt_mesh_generic_property_server_num_ids_get_t));
	p_get_id_num = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)p_evt->data;
	p_get_id_num->pvalue = &num_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, num ids %d \r\n", num_get.pdb, num_get.num_ids);

	rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_get;
	uint16_t admin_ids_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,
								num_get.num_ids * 2 + sizeof(rtk_bt_mesh_generic_property_server_num_ids_value_get_t));
	ids_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)p_evt->data;
	ids_get->num_ids = num_get.num_ids;
	if (num_get.num_ids > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The num of generic property ids is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, num_get.num_ids,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		ids_get->num_ids = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	ids_get->property_ids = admin_ids_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMP16A("", admin_ids_store, ids_get->num_ids);
	return 0;
}
#endif

static int generic_admin_property_srv_prop_get(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		if (property_id != GENERIC_PROPERTY_ID_PROHIBITED) {
			rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
			return generic_admin_property_status(model, ctx, property_id, pdb);
		}
	}
	return -1;
}

static uint16_t admin_property_id;
static int generic_admin_property_srv_prop_set_unack(const struct bt_mesh_model *model,
													 struct bt_mesh_msg_ctx *ctx,
													 struct net_buf_simple *buf)
{
	(void)ctx;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		admin_property_id = property_id;
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
		if (NULL != pdb) {
			uint8_t property_access = net_buf_simple_pull_u8(buf);
			if (buf->len == pdb->value_len && property_access <= GENERIC_PROPERTY_ACCESS_READ_AND_WRITE) {
				uint16_t value_len = buf->len;
				pdb->property_access = property_access;
				memcpy(pdb->pproperty_value, buf->data, value_len);
				net_buf_simple_pull(buf, value_len);
				pdb->value_len = value_len;

				rtk_bt_evt_t *p_evt = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_PROPERTY_SET,
											5 + value_len);
				memcpy(p_evt->data, &property_id, 2);
				memcpy((uint8_t *)(p_evt->data) + 2, &property_access, 1);
				memcpy((uint8_t *)(p_evt->data) + 3, &value_len, 2);
				memcpy((uint8_t *)(p_evt->data) + 5, pdb->pproperty_value, value_len);
				return rtk_bt_evt_indicate(p_evt, NULL);
			}
		}
		return 0;
	}
	return -1;
}

static int generic_admin_property_srv_prop_set(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	if (!generic_admin_property_srv_prop_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, admin_property_id);
		return generic_admin_property_status(model, ctx, admin_property_id, pdb);
	}
	return -1;
}

const struct bt_mesh_model_op generic_admin_property_srv_op[] = {
	{ OP_GENERIC_ADMIN_PROPERTIES_GET,             BT_MESH_LEN_EXACT(0),      generic_admin_property_srv_props_get },
	{ OP_GENERIC_ADMIN_PROPERTY_GET,               BT_MESH_LEN_EXACT(2),      generic_admin_property_srv_prop_get },
	{ OP_GENERIC_ADMIN_PROPERTY_SET,               BT_MESH_LEN_MIN(3),        generic_admin_property_srv_prop_set },
	{ OP_GENERIC_ADMIN_PROPERTY_SET_UNACK,         BT_MESH_LEN_MIN(3),        generic_admin_property_srv_prop_set_unack },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_generic_admin_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_generic_property_server_db_set_t *db_get = (rtk_bt_mesh_generic_property_server_db_set_t *)p_cmd->param;
		generic_property_server_set_db(generic_admin_property_server_model, db_get->pdata_db, db_get->num_properties);
		ret = 0;
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

#if defined(BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_MANUFACTURER_PROPERTY_SERVER_MODEL
static const struct bt_mesh_model *generic_manu_property_server_model = NULL;

static int generic_manufacturer_property_srv_init(const struct bt_mesh_model *model)
{
	generic_manu_property_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_manufacturer_property_srv_cb = {
	.init = generic_manufacturer_property_srv_init,
};

uint8_t manu_property_store_value[3] = {0x1, 0x1, 0x2};
uint8_t manu_property_store_value2[2] = {0x3, 0x5};

rtk_bt_mesh_generic_property_db_t manu_property_db[] = {
	{1, 3, manu_property_store_value, 3},
	{2, 3, manu_property_store_value2, 2}
};

generic_property_info_t manu_property_info = {manu_property_db, 2};

static int generic_manu_properties_status(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  const rtk_bt_mesh_generic_property_db_t *pproperties,
										  uint16_t num_properties)
{
	uint16_t msg_len = 2 * num_properties;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTIES_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_MANUFACTURER_PROPERTIES_STATUS);
	for (int i = 0; i < num_properties; i++) {
		net_buf_simple_add_le16(&buf, pproperties[i].property_id);
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_manu_property_status(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										uint16_t property_id,
										const rtk_bt_mesh_generic_property_db_t *pdb)
{
	if (NULL != pdb) {
		uint16_t msg_len = 3 + pdb->value_len;
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTY_STATUS, msg_len);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_MANUFACTURER_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		net_buf_simple_add_u8(&buf, pdb->property_access);
		net_buf_simple_add_mem(&buf, pdb->pproperty_value, pdb->value_len);
		return bt_mesh_msg_send(model, ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTY_STATUS, 2);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_MANUFACTURER_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
}

/**
 * This part is to verify if realated database parameter can be get from app layer
*/
#if 0
static int generic_manu_property_srv_get_para_from_app(uint16_t property_id)
{
	rtk_bt_mesh_generic_property_server_para_t para_get;
	rtk_bt_mesh_generic_property_server_para_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PARAMETER_GET,
								sizeof(rtk_bt_mesh_generic_property_server_para_get_t));
	p_get_data = (rtk_bt_mesh_generic_property_server_para_get_t *)p_evt->data;
	p_get_data->property_id = property_id;
	p_get_data->pvalue = &para_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, property access %d, value len %d \r\n", para_get.pdb, para_get.property_access, para_get.value_len);

	uint8_t manu_pvalue_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	rtk_bt_mesh_generic_property_server_value_get_t *val_get;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_VALUE_GET,
								sizeof(rtk_bt_mesh_generic_property_server_value_get_t) + para_get.value_len);
	val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)p_evt->data;
	val_get->property_id = property_id;
	val_get->value_len = para_get.value_len;
	if (para_get.value_len > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The len of generic property data is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, para_get.value_len,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		val_get->value_len = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	val_get->pproperty_value = manu_pvalue_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMPA("", manu_pvalue_store, val_get->value_len);

	rtk_bt_mesh_generic_property_server_num_ids_t num_get;
	rtk_bt_mesh_generic_property_server_num_ids_get_t *p_get_id_num;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_NUM_IDS_GET,
								sizeof(rtk_bt_mesh_generic_property_server_num_ids_get_t));
	p_get_id_num = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)p_evt->data;
	p_get_id_num->pvalue = &num_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, num ids %d \r\n", num_get.pdb, num_get.num_ids);

	rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_get;
	uint16_t manu_ids_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,
								num_get.num_ids * 2 + sizeof(rtk_bt_mesh_generic_property_server_num_ids_value_get_t));
	ids_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)p_evt->data;
	ids_get->num_ids = num_get.num_ids;
	if (num_get.num_ids > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The num of generic property ids is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, num_get.num_ids,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		ids_get->num_ids = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	ids_get->property_ids = manu_ids_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMP16A("", manu_ids_store, ids_get->num_ids);
	return 0;
}
#endif

static int generic_manu_property_srv_props_get(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	if (!buf->len) {
		generic_property_info_t *pinfo = model->rt->user_data;
		return generic_manu_properties_status(model, ctx, pinfo->pproperties, pinfo->num_properties);
	}
	return -1;
}

static int generic_manu_property_srv_prop_get(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		if (property_id != GENERIC_PROPERTY_ID_PROHIBITED) {
			rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
			return generic_manu_property_status(model, ctx, property_id, pdb);
		}
	}
	return -1;
}

static uint16_t manu_property_id;
static int generic_manu_property_srv_prop_set_unack(const struct bt_mesh_model *model,
													struct bt_mesh_msg_ctx *ctx,
													struct net_buf_simple *buf)
{
	(void)ctx;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		manu_property_id = property_id;
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
		if (NULL != pdb) {
			uint8_t property_access = net_buf_simple_pull_u8(buf);
			if (property_access <= GENERIC_PROPERTY_ACCESS_READ_AND_WRITE) {
				pdb->property_access = property_access;
				rtk_bt_mesh_generic_manu_property_server_set_t settings;
				settings.property_id = property_id;
				settings.property_access = property_access;
				rtk_bt_evt_t *p_evt = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_PROPERTY_SET,
											sizeof(rtk_bt_mesh_generic_manu_property_server_set_t));
				memcpy(p_evt->data, &settings, sizeof(rtk_bt_mesh_generic_manu_property_server_set_t));
				return rtk_bt_evt_indicate(p_evt, NULL);
			}
		}
		return 0;
	}
	return -1;
}

static int generic_manu_property_srv_prop_set(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	if (!generic_manu_property_srv_prop_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, manu_property_id);
		return generic_manu_property_status(model, ctx, manu_property_id, pdb);
	}
	return -1;
}

const struct bt_mesh_model_op generic_manu_property_srv_op[] = {
	{ OP_GENERIC_MANUFACTURER_PROPERTIES_GET,        BT_MESH_LEN_EXACT(0),      generic_manu_property_srv_props_get },
	{ OP_GENERIC_MANUFACTURER_PROPERTY_GET,          BT_MESH_LEN_EXACT(2),      generic_manu_property_srv_prop_get },
	{ OP_GENERIC_MANUFACTURER_PROPERTY_SET,          BT_MESH_LEN_EXACT(3),      generic_manu_property_srv_prop_set },
	{ OP_GENERIC_MANUFACTURER_PROPERTY_SET_UNACK,    BT_MESH_LEN_EXACT(3),      generic_manu_property_srv_prop_set_unack },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_generic_manu_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_MANU_PROPERTY_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_generic_property_server_db_set_t *db_get = (rtk_bt_mesh_generic_property_server_db_set_t *)p_cmd->param;
		generic_property_server_set_db(generic_manu_property_server_model, db_get->pdata_db, db_get->num_properties);
		ret = 0;
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

#if defined(BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_USER_PROPERTY_SERVER_MODEL
static const struct bt_mesh_model *generic_user_property_server_model = NULL;

static int generic_user_property_srv_init(const struct bt_mesh_model *model)
{
	generic_user_property_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_user_property_srv_cb = {
	.init = generic_user_property_srv_init,
};

uint8_t user_property_store_value[3] = {0x1, 0x1, 0x2};
uint8_t user_property_store_value2[2] = {0x3, 0x5};

rtk_bt_mesh_generic_property_db_t user_property_db[] = {
	{1, 3, user_property_store_value, 3},
	{2, 3, user_property_store_value2, 2}
};

generic_property_info_t user_property_info = {user_property_db, 2};

static int generic_user_properties_status(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  const rtk_bt_mesh_generic_property_db_t *pproperties,
										  uint16_t num_properties)
{
	uint16_t msg_len = 2 * num_properties;
	for (uint16_t i = 0; i < num_properties; ++i) {
		if (GENERIC_PROPERTY_ACCESS_NOT_TO_USER == pproperties[i].property_access) {
			msg_len -= 2;
		}
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTIES_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_USER_PROPERTIES_STATUS);
	for (uint16_t i = 0; i < num_properties; ++i) {
		if (GENERIC_PROPERTY_ACCESS_NOT_TO_USER != pproperties[i].property_access) {
			net_buf_simple_add_le16(&buf, pproperties[i].property_id);
		}
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_user_property_status(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										uint16_t property_id,
										const rtk_bt_mesh_generic_property_db_t *pdb,
										bool send_value)
{
	if (NULL != pdb) {
		uint16_t msg_len = 3;
		if (send_value) {
			msg_len += pdb->value_len;
		}
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTY_STATUS, msg_len);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_USER_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		net_buf_simple_add_u8(&buf, pdb->property_access);
		if (send_value) {
			net_buf_simple_add_mem(&buf, pdb->pproperty_value, pdb->value_len);
		}
		return bt_mesh_msg_send(model, ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTY_STATUS, 2);
		bt_mesh_model_msg_init(&buf, OP_GENERIC_USER_PROPERTY_STATUS);
		net_buf_simple_add_le16(&buf, property_id);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
}

#if 0
static int generic_user_property_srv_get_para_from_app(uint16_t property_id)
{
	rtk_bt_mesh_generic_property_server_para_t para_get;
	rtk_bt_mesh_generic_property_server_para_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PARAMETER_GET,
								sizeof(rtk_bt_mesh_generic_property_server_para_get_t));
	p_get_data = (rtk_bt_mesh_generic_property_server_para_get_t *)p_evt->data;
	p_get_data->property_id = property_id;
	p_get_data->pvalue = &para_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, property access %d, value len %d \r\n", para_get.pdb, para_get.property_access, para_get.value_len);

	uint8_t user_pvalue_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	rtk_bt_mesh_generic_property_server_value_get_t *val_get;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_VALUE_GET,
								sizeof(rtk_bt_mesh_generic_property_server_value_get_t) + para_get.value_len);
	val_get = (rtk_bt_mesh_generic_property_server_value_get_t *)p_evt->data;
	val_get->property_id = property_id;
	val_get->value_len = para_get.value_len;
	if (para_get.value_len > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The len of generic property data is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, para_get.value_len,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		val_get->value_len = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	val_get->pproperty_value = user_pvalue_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMPA("", user_pvalue_store, val_get->value_len);

	rtk_bt_mesh_generic_property_server_num_ids_t num_get;
	rtk_bt_mesh_generic_property_server_num_ids_get_t *p_get_id_num;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_NUM_IDS_GET,
								sizeof(rtk_bt_mesh_generic_property_server_num_ids_get_t));
	p_get_id_num = (rtk_bt_mesh_generic_property_server_num_ids_get_t *)p_evt->data;
	p_get_id_num->pvalue = &num_get;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_LOGA("pdb is NULL?%d, num ids %d \r\n", num_get.pdb, num_get.num_ids);

	rtk_bt_mesh_generic_property_server_num_ids_value_get_t *ids_get;
	uint16_t user_ids_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_IDS_VALUE_GET,
								num_get.num_ids * 2 + sizeof(rtk_bt_mesh_generic_property_server_num_ids_value_get_t));
	ids_get = (rtk_bt_mesh_generic_property_server_num_ids_value_get_t *)p_evt->data;
	ids_get->num_ids = num_get.num_ids;
	if (num_get.num_ids > GENERIC_PROPERTY_DATA_MAX_LEN) {
		BT_LOGA("[%s] The num of generic property ids is %d, extend max GENERIC_PROPERTY_DATA_MAX_LEN:%d\r\n", __func__, num_get.num_ids,
				GENERIC_PROPERTY_DATA_MAX_LEN);
		ids_get->num_ids = GENERIC_PROPERTY_DATA_MAX_LEN;
	}
	ids_get->property_ids = user_ids_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_DUMP16A("", user_ids_store, ids_get->num_ids);
	return 0;
}
#endif

static int generic_user_property_srv_props_get(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	if (!buf->len) {
		generic_property_info_t *pinfo = model->rt->user_data;
		return generic_user_properties_status(model, ctx, pinfo->pproperties, pinfo->num_properties);
	}
	return -1;
}

static int generic_user_property_srv_prop_get(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		if (property_id != GENERIC_PROPERTY_ID_PROHIBITED) {
			rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
			bool send_value = FALSE;
			if (pdb != NULL && GENERIC_PROPERTY_ACCESS_WRITE != pdb->property_access) {
				send_value = TRUE;
			}
			return generic_user_property_status(model, ctx, property_id, pdb, send_value);
		}
	}
	return -1;
}

static bool user_send_value = FALSE;
static uint16_t user_property_id;
static int generic_user_property_srv_prop_set_unack(const struct bt_mesh_model *model,
													struct bt_mesh_msg_ctx *ctx,
													struct net_buf_simple *buf)
{
	(void)ctx;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, property_id);
		user_send_value = FALSE;
		if (NULL != pdb) {
			uint16_t value_len = buf->len;
			if (pdb->value_len == value_len && pdb->property_access != GENERIC_PROPERTY_ACCESS_READ) {
				memcpy(pdb->pproperty_value, buf->data, value_len);

				rtk_bt_evt_t *p_evt = NULL;
				p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_PROPERTY_SET,
											sizeof(rtk_bt_mesh_generic_user_property_server_set_t) + value_len);
				memcpy(p_evt->data, &property_id, 2);
				memcpy((uint8_t *)(p_evt->data) + 2, &value_len, 2);
				memcpy((uint8_t *)(p_evt->data) + 4, pdb->pproperty_value, value_len);
				rtk_bt_evt_indicate(p_evt, NULL);
				user_send_value = TRUE;
				user_property_id = property_id;
			}
		}
		return 0;
	}
	return -1;
}

static int generic_user_property_srv_prop_set(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	if (!generic_user_property_srv_prop_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_generic_property_db_t *pdb = generic_property_find(model, user_property_id);
		return generic_user_property_status(model, ctx, user_property_id, pdb, user_send_value);
	}
	return -1;
}

const struct bt_mesh_model_op generic_user_property_srv_op[] = {
	{ OP_GENERIC_USER_PROPERTIES_GET,             BT_MESH_LEN_EXACT(0),      generic_user_property_srv_props_get },
	{ OP_GENERIC_USER_PROPERTY_GET,               BT_MESH_LEN_EXACT(2),      generic_user_property_srv_prop_get },
	{ OP_GENERIC_USER_PROPERTY_SET,               BT_MESH_LEN_MIN(2),        generic_user_property_srv_prop_set },
	{ OP_GENERIC_USER_PROPERTY_SET_UNACK,         BT_MESH_LEN_MIN(2),        generic_user_property_srv_prop_set_unack },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_generic_user_property_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_generic_property_server_db_set_t *db_get = (rtk_bt_mesh_generic_property_server_db_set_t *)p_cmd->param;
		generic_property_server_set_db(generic_user_property_server_model, db_get->pdata_db, db_get->num_properties);
		ret = 0;
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

#if defined(BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_CLIENT_PROPERTY_SERVER_MODEL
static const struct bt_mesh_model *generic_client_property_server_model = NULL;

static int generic_client_property_srv_init(const struct bt_mesh_model *model)
{
	generic_client_property_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_client_property_srv_cb = {
	.init = generic_client_property_srv_init,
};

static int generic_client_properties_status(const struct bt_mesh_model *model,
											struct bt_mesh_msg_ctx *ctx,
											const uint16_t *pproperty_ids,
											uint16_t num_ids)
{
	uint16_t msg_len = 2 * num_ids;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_CLIENT_PROPERTIES_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_CLIENT_PROPERTIES_STATUS);
	for (uint16_t i = 0; i < num_ids; ++i) {
		net_buf_simple_add_le16(&buf, pproperty_ids[i]);
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int generic_property_cli_manu_props_status(const struct bt_mesh_model *model,
												  struct bt_mesh_msg_ctx *ctx,
												  struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		uint8_t client_ids_stores[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
		uint16_t client_ids_value_store[GENERIC_PROPERTY_DATA_MAX_LEN] = {0};
		rtk_bt_mesh_generic_client_property_server_direct_get_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL, RTK_BT_MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_PROPERTY_GET,
									2 * GENERIC_PROPERTY_DATA_MAX_LEN + 3);
		p_get_data = (rtk_bt_mesh_generic_client_property_server_direct_get_t *)p_evt->data;
		p_get_data->property_id_start = property_id;
		p_get_data->value = client_ids_stores;
		rtk_bt_evt_indicate(p_evt, NULL);
		uint8_t num_ids = client_ids_stores[0];
		memcpy(client_ids_value_store, &client_ids_stores[1], 2 * num_ids);
		return generic_client_properties_status(model, ctx, client_ids_value_store, num_ids);
	}
	return -1;
}

const struct bt_mesh_model_op generic_client_property_srv_op[] = {
	{ OP_GENERIC_CLIENT_PROPERTIES_GET,        BT_MESH_LEN_EXACT(2),      generic_property_cli_manu_props_status },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_PROPERTY_CLIENT_MODEL
static const struct bt_mesh_model *generic_property_client_model = NULL;

static int generic_property_cli_init(const struct bt_mesh_model *model)
{
	generic_property_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_generic_property_cli_cb = {
	.init = generic_property_cli_init,
};

static int generic_property_cli_manu_props_status(const struct bt_mesh_model *model,
												  struct bt_mesh_msg_ctx *ctx,
												  struct net_buf_simple *buf)
{
	(void)model;
	uint16_t value_len = buf->len;
	uint8_t num_ids = 0;
	if (value_len > 0) {
		num_ids = value_len / 2;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_STATUS,
								sizeof(rtk_bt_mesh_generic_properties_client_status_t) + num_ids * sizeof(uint16_t));
	memcpy(p_evt->data, &(ctx->addr), 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &num_ids, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, buf->data, 2 * num_ids);
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int generic_property_cli_manu_prop_status(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		uint8_t property_access = 0;
		uint16_t value_len = 0;
		if (buf->len) {
			property_access = net_buf_simple_pull_u8(buf);
			value_len = buf->len;
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_STATUS,
									sizeof(rtk_bt_mesh_generic_property_client_status_t) + value_len);
		memcpy(p_evt->data, &(ctx->addr), 2);
		memcpy((uint8_t *)(p_evt->data) + 2, &property_id, 2);
		memcpy((uint8_t *)(p_evt->data) + 4, &property_access, 1);
		memcpy((uint8_t *)(p_evt->data) + 5, &value_len, 2);
		memcpy((uint8_t *)(p_evt->data) + 7, buf->data, value_len);
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_property_cli_admin_props_status(const struct bt_mesh_model *model,
												   struct bt_mesh_msg_ctx *ctx,
												   struct net_buf_simple *buf)
{
	(void)model;
	uint16_t value_len = buf->len;
	uint8_t num_ids = 0;
	if (value_len > 0) {
		num_ids = value_len / 2;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_STATUS,
								sizeof(rtk_bt_mesh_generic_properties_client_status_t) + num_ids * sizeof(uint16_t));
	memcpy(p_evt->data, &(ctx->addr), 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &num_ids, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, buf->data, 2 * num_ids);
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int generic_property_cli_admin_prop_status(const struct bt_mesh_model *model,
												  struct bt_mesh_msg_ctx *ctx,
												  struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		uint8_t property_access = 0;
		uint16_t value_len = 0;
		if (buf->len) {
			property_access = net_buf_simple_pull_u8(buf);
			value_len = buf->len;
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_STATUS,
									sizeof(rtk_bt_mesh_generic_property_client_status_t) + value_len);
		memcpy(p_evt->data, &(ctx->addr), 2);
		memcpy((uint8_t *)(p_evt->data) + 2, &property_id, 2);
		memcpy((uint8_t *)(p_evt->data) + 4, &property_access, 1);
		memcpy((uint8_t *)(p_evt->data) + 5, &value_len, 2);
		memcpy((uint8_t *)(p_evt->data) + 7, buf->data, value_len);
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_property_cli_user_props_status(const struct bt_mesh_model *model,
												  struct bt_mesh_msg_ctx *ctx,
												  struct net_buf_simple *buf)
{
	(void)model;
	uint16_t value_len = buf->len;
	uint8_t num_ids = 0;
	if (value_len > 0) {
		num_ids = value_len / 2;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_STATUS,
								sizeof(rtk_bt_mesh_generic_properties_client_status_t) + num_ids * sizeof(uint16_t));
	memcpy(p_evt->data, &(ctx->addr), 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &num_ids, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, buf->data, 2 * num_ids);
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int generic_property_cli_user_prop_status(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		uint8_t property_access = 0;
		uint16_t value_len = 0;
		if (buf->len) {
			property_access = net_buf_simple_pull_u8(buf);
			value_len = buf->len;
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_STATUS,
									sizeof(rtk_bt_mesh_generic_property_client_status_t) + value_len);
		memcpy(p_evt->data, &(ctx->addr), 2);
		memcpy((uint8_t *)(p_evt->data) + 2, &property_id, 2);
		memcpy((uint8_t *)(p_evt->data) + 4, &property_access, 1);
		memcpy((uint8_t *)(p_evt->data) + 5, &value_len, 2);
		memcpy((uint8_t *)(p_evt->data) + 7, buf->data, value_len);
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int generic_property_cli_client_props_status(const struct bt_mesh_model *model,
													struct bt_mesh_msg_ctx *ctx,
													struct net_buf_simple *buf)
{
	(void)model;
	uint16_t value_len = buf->len;
	uint8_t num_ids = 0;
	if (value_len > 0) {
		num_ids = value_len / 2;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_PROPERTY_CLIENT_MODEL, RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_STATUS,
								sizeof(rtk_bt_mesh_generic_properties_client_status_t) + num_ids * sizeof(uint16_t));
	memcpy(p_evt->data, &(ctx->addr), 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &num_ids, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, buf->data, 2 * num_ids);
	return rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_model_op generic_property_cli_op[] = {
	{ OP_GENERIC_MANUFACTURER_PROPERTIES_STATUS,        BT_MESH_LEN_MIN(0),      generic_property_cli_manu_props_status },
	{ OP_GENERIC_MANUFACTURER_PROPERTY_STATUS,          BT_MESH_LEN_MIN(2),      generic_property_cli_manu_prop_status },
	{ OP_GENERIC_ADMIN_PROPERTIES_STATUS,               BT_MESH_LEN_MIN(0),      generic_property_cli_admin_props_status },
	{ OP_GENERIC_ADMIN_PROPERTY_STATUS,                 BT_MESH_LEN_MIN(2),      generic_property_cli_admin_prop_status },
	{ OP_GENERIC_USER_PROPERTIES_STATUS,                BT_MESH_LEN_MIN(0),      generic_property_cli_user_props_status },
	{ OP_GENERIC_USER_PROPERTY_STATUS,                  BT_MESH_LEN_MIN(2),      generic_property_cli_user_prop_status },
	{ OP_GENERIC_CLIENT_PROPERTIES_STATUS,              BT_MESH_LEN_MIN(0),      generic_property_cli_client_props_status },
	BT_MESH_MODEL_OP_END,
};

static int generic_user_properties_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTIES_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_USER_PROPERTIES_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_user_property_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTY_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_USER_PROPERTY_GET);
	net_buf_simple_add_le16(&buf, property_id);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_user_property_set(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index, uint16_t property_id,
									 const uint8_t *pvalue, uint8_t value_len, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 2 + value_len;
	uint32_t op = ack ? OP_GENERIC_USER_PROPERTY_SET : OP_GENERIC_USER_PROPERTY_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_USER_PROPERTY_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, property_id);
	net_buf_simple_add_mem(&buf, pvalue, value_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_admin_properties_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTIES_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_ADMIN_PROPERTIES_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_admin_property_get(const struct bt_mesh_model *model, uint16_t dst,
									  uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTY_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_ADMIN_PROPERTY_GET);
	net_buf_simple_add_le16(&buf, property_id);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_admin_property_set(const struct bt_mesh_model *model, uint16_t dst,
									  uint16_t app_key_index, uint16_t property_id, uint8_t property_access,
									  const uint8_t *pvalue, uint8_t value_len, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 2 + 1 + value_len;
	uint32_t op = ack ? OP_GENERIC_ADMIN_PROPERTY_SET : OP_GENERIC_ADMIN_PROPERTY_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_ADMIN_PROPERTY_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, property_id);
	net_buf_simple_add_u8(&buf, property_access);
	net_buf_simple_add_mem(&buf, pvalue, value_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_manufacturer_properties_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTIES_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_MANUFACTURER_PROPERTIES_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_manufacturer_property_get(const struct bt_mesh_model *model,
											 uint16_t dst,
											 uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTY_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_MANUFACTURER_PROPERTY_GET);
	net_buf_simple_add_le16(&buf, property_id);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_manufacturer_property_set(const struct bt_mesh_model *model, uint16_t dst,
											 uint16_t app_key_index, uint16_t property_id, uint8_t property_access,
											 bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 2 + 1;
	uint32_t op = ack ? OP_GENERIC_MANUFACTURER_PROPERTY_SET : OP_GENERIC_MANUFACTURER_PROPERTY_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_MANUFACTURER_PROPERTY_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, property_id);
	net_buf_simple_add_u8(&buf, property_access);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int generic_client_properties_get(const struct bt_mesh_model *model,
										 uint16_t dst,
										 uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_GENERIC_CLIENT_PROPERTIES_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_GENERIC_CLIENT_PROPERTIES_GET);
	net_buf_simple_add_le16(&buf, property_id);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_generic_property_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTIES_GET: {
		rtk_bt_mesh_generic_user_properties_get_t *user_get = (rtk_bt_mesh_generic_user_properties_get_t *)p_cmd->param;
		ret = generic_user_properties_get(generic_property_client_model, user_get->dst, user_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_GET: {
		rtk_bt_mesh_generic_user_property_get_t *user_property_get = (rtk_bt_mesh_generic_user_property_get_t *)p_cmd->param;
		ret = generic_user_property_get(generic_property_client_model, user_property_get->dst, user_property_get->app_key_index, user_property_get->property_id);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_USER_PROPERTY_SET: {
		rtk_bt_mesh_generic_user_property_set_t *user_property_set = (rtk_bt_mesh_generic_user_property_set_t *)p_cmd->param;
		ret = generic_user_property_set(generic_property_client_model, user_property_set->dst, user_property_set->app_key_index, user_property_set->property_id,
										user_property_set->pvalue, user_property_set->value_len, user_property_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTIES_GET: {
		rtk_bt_mesh_generic_admin_properties_get_t *admin_get = (rtk_bt_mesh_generic_admin_properties_get_t *)p_cmd->param;
		ret = generic_admin_properties_get(generic_property_client_model, admin_get->dst, admin_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_GET: {
		rtk_bt_mesh_generic_admin_property_get_t *admin_property_get = (rtk_bt_mesh_generic_admin_property_get_t *)p_cmd->param;
		ret = generic_admin_property_get(generic_property_client_model, admin_property_get->dst, admin_property_get->app_key_index, admin_property_get->property_id);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_ADMIN_PROPERTY_SET: {
		rtk_bt_mesh_generic_admin_property_set_t *admin_property_set = (rtk_bt_mesh_generic_admin_property_set_t *)p_cmd->param;
		ret = generic_admin_property_set(generic_property_client_model, admin_property_set->dst, admin_property_set->app_key_index, admin_property_set->property_id,
										 admin_property_set->property_access, admin_property_set->pvalue, admin_property_set->value_len, admin_property_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTIES_GET: {
		rtk_bt_mesh_generic_manu_properties_get_t *manu_get = (rtk_bt_mesh_generic_manu_properties_get_t *)p_cmd->param;
		ret = generic_manufacturer_properties_get(generic_property_client_model, manu_get->dst, manu_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_GET: {
		rtk_bt_mesh_generic_manu_property_get_t *manu_property_get = (rtk_bt_mesh_generic_manu_property_get_t *)p_cmd->param;
		ret = generic_manufacturer_property_get(generic_property_client_model, manu_property_get->dst, manu_property_get->app_key_index,
												manu_property_get->property_id);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_MANU_PROPERTY_SET: {
		rtk_bt_mesh_generic_manu_property_set_t *manu_property_set = (rtk_bt_mesh_generic_manu_property_set_t *)p_cmd->param;
		ret = generic_manufacturer_property_set(generic_property_client_model, manu_property_set->dst, manu_property_set->app_key_index,
												manu_property_set->property_id,
												manu_property_set->property_access, manu_property_set->ack);
		break;
	}
	case RTK_BT_MESH_GENERIC_PROPERTY_CLIENT_MODEL_CLIENT_PROPERTY_GET: {
		rtk_bt_mesh_generic_client_properties_get_t *client_get = (rtk_bt_mesh_generic_client_properties_get_t *)p_cmd->param;
		ret = generic_client_properties_get(generic_property_client_model, client_get->dst, client_get->app_key_index, client_get->property_id);
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