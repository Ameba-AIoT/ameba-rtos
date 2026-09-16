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
#include <rtk_bt_mesh_light_model.h>
#include <rtk_bt_mesh_generic_onoff_model.h>
#include <msg.h>

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
/******************OPCODE DEFINE*********************/
/*****************LIGHT LIGHTNESS MODEL PART*********************/
#define OP_LIGHT_LIGHTNESS_GET                  BT_MESH_MODEL_OP_2(0x82, 0x4B)
#define OP_LIGHT_LIGHTNESS_SET                  BT_MESH_MODEL_OP_2(0x82, 0x4C)
#define OP_LIGHT_LIGHTNESS_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x4D)
#define OP_LIGHT_LIGHTNESS_LINEAR_GET           BT_MESH_MODEL_OP_2(0x82, 0x4F)
#define OP_LIGHT_LIGHTNESS_LINEAR_SET           BT_MESH_MODEL_OP_2(0x82, 0x50)
#define OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK     BT_MESH_MODEL_OP_2(0x82, 0x51)
#define OP_LIGHT_LIGHTNESS_LAST_GET             BT_MESH_MODEL_OP_2(0x82, 0x53)
#define OP_LIGHT_LIGHTNESS_DEFAULT_GET          BT_MESH_MODEL_OP_2(0x82, 0x55)
#define OP_LIGHT_LIGHTNESS_RANGE_GET            BT_MESH_MODEL_OP_2(0x82, 0x57)

#define OP_LIGHT_LIGHTNESS_DEFAULT_SET          BT_MESH_MODEL_OP_2(0x82, 0x59)
#define OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK    BT_MESH_MODEL_OP_2(0x82, 0x5A)
#define OP_LIGHT_LIGHTNESS_RANGE_SET            BT_MESH_MODEL_OP_2(0x82, 0x5B)
#define OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK      BT_MESH_MODEL_OP_2(0x82, 0x5C)

#define OP_LIGHT_LIGHTNESS_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x4E)
#define OP_LIGHT_LIGHTNESS_LINEAR_STATUS        BT_MESH_MODEL_OP_2(0x82, 0x52)
#define OP_LIGHT_LIGHTNESS_LAST_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x54)
#define OP_LIGHT_LIGHTNESS_DEFAULT_STATUS       BT_MESH_MODEL_OP_2(0x82, 0x56)
#define OP_LIGHT_LIGHTNESS_RANGE_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x58)

/*******************OPCODE DEFINE END**********************/
/******COMMON PARAMETER SETTING******/
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
const uint32_t time_res[] = {
	100,
	MSEC_PER_SEC,
	10 * MSEC_PER_SEC,
	10 * 60 * MSEC_PER_SEC,
};
#endif

/****************LIGHT LIGHTNESS CODE & FUNCTION*********************/
/****LIGHT LIGHTNESS SERVER****/
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SERVER_MODEL
struct lightness_pa_t {
	uint16_t lightness;
	uint16_t target_lightness;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_lightness_pa;

struct lightness_linear_pa_t {
	uint16_t lightness_linear;
	uint16_t target_lightness_linear;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_lightness_linear_pa;

struct k_mutex mutex_lightness;
struct k_mutex mutex_lightness_linear;
static const struct bt_mesh_model *light_lightness_server_model = NULL;

static uint16_t get_present_lightness(const struct bt_mesh_model *model, bool linear)
{
	(void)model;
	rtk_bt_mesh_light_lightness_server_direct_get_t *lightness;
	rtk_bt_mesh_light_lightness_server_get_t lightness_val;
	uint8_t evt = RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_GET;
	if (linear) {
		evt = RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_GET;
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, evt,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_t));
	lightness = (rtk_bt_mesh_light_lightness_server_direct_get_t *)p_evt->data;
	lightness->value = &lightness_val;
	rtk_bt_evt_indicate(p_evt, NULL);
	return lightness_val.lightness;
}

static int light_lightness_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t present_lightness, bool optional,
								uint16_t target_lightness,
								rtk_bt_mesh_generic_transition_time_t remaining_time, bool linear)
{
	int ret;
	uint16_t msg_len = 0;
	if (optional) {
		msg_len = 5;
	} else {
		msg_len = 2;
	}
	if (linear) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_LINEAR_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
		if (optional) {
			net_buf_simple_add_mem(&pmsg, &present_lightness, sizeof(uint16_t));
			net_buf_simple_add_mem(&pmsg, &target_lightness, sizeof(uint16_t));
			net_buf_simple_add_mem(&pmsg, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		} else {
			net_buf_simple_add_mem(&pmsg, &present_lightness, sizeof(uint16_t));
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_STATUS, msg_len);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_STATUS);
		if (optional) {
			net_buf_simple_add_mem(&pmsg, &present_lightness, sizeof(uint16_t));
			net_buf_simple_add_mem(&pmsg, &target_lightness, sizeof(uint16_t));
			net_buf_simple_add_mem(&pmsg, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		} else {
			net_buf_simple_add_mem(&pmsg, &present_lightness, sizeof(uint16_t));
		}
		ret = bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return ret;
}

int light_lightness_publish(const struct bt_mesh_model *model, bool linear)
{
	uint16_t present_lightness = get_present_lightness(model, linear);
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_lightness_stat(model, NULL, present_lightness, FALSE, 0, remaining_time, linear);
	/* If want to publish periodicly like spec, use below part
	 * when use below part, should check publication related info is ok first
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
	// if (linear) {
	//     BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_LINEAR_STATUS, 2);
	//     bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
	//     net_buf_simple_add_mem(&buf, &present_lightness, sizeof(uint16_t));
	//     net_buf_simple_reset(model->pub->msg);
	//     net_buf_simple_add_mem(model->pub->msg, buf.data, buf.len);
	// } else {
	//     BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_STATUS, 2);
	//     bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_STATUS);
	//     net_buf_simple_add_mem(&buf, &present_lightness, sizeof(uint16_t));
	//     net_buf_simple_reset(model->pub->msg);
	//     net_buf_simple_add_mem(model->pub->msg, buf.data, buf.len);
	// }
}

int light_lightness_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	uint16_t present_lightness = get_present_lightness(model, FALSE);
	bt_mesh_model_msg_init(msg, OP_LIGHT_LIGHTNESS_STATUS);
	net_buf_simple_add_mem(msg, &present_lightness, sizeof(uint16_t));
	return 0;
}

void lightness_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_lightness, K_FOREVER);
	if (!light_lightness_pa.transition_flag) {
		light_lightness_pa.remain_time.num_steps = 0;
		light_lightness_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_lightness_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_lightness_server_set_t *lightness_set = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_lightness_server_set_t));
	lightness_set = (rtk_bt_mesh_light_lightness_server_set_t *)p_evt->data;
	lightness_set->lightness = light_lightness_pa.lightness;
	lightness_set->total_time = light_lightness_pa.total_time;
	lightness_set->remaining_time = light_lightness_pa.remain_time;
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_lightness_pa.remain_time.num_steps) {
		light_lightness_pa.remain_time.num_steps--;
		k_work_reschedule(&light_lightness_pa.work, K_MSEC(trans));
	} else {
		light_lightness_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_lightness);
	return;
}

uint16_t light_lightness_linear_to_actual(uint16_t lightness_linear)
{
	return (uint16_t)(65535 * sqrt(lightness_linear / 65535.0));
}

uint16_t light_lightness_actual_to_linear(uint16_t lightness_actual)
{
	return (uint16_t)(lightness_actual / 65535.0 * lightness_actual);
}

void lightness_linear_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_lightness_linear, K_FOREVER);
	int32_t trans = time_res[light_lightness_linear_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_lightness_server_set_t *lightness_set = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	if (!light_lightness_linear_pa.transition_flag) {
		light_lightness_linear_pa.remain_time.num_steps = 0;
		light_lightness_linear_pa.total_time.num_steps = 0;
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_lightness_server_set_t));
	lightness_set = (rtk_bt_mesh_light_lightness_server_set_t *)p_evt->data;
	lightness_set->lightness = light_lightness_linear_pa.lightness_linear;
	lightness_set->total_time = light_lightness_linear_pa.total_time;
	lightness_set->remaining_time = light_lightness_linear_pa.remain_time;
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_lightness_linear_pa.remain_time.num_steps) {
		light_lightness_linear_pa.remain_time.num_steps--;
		k_work_reschedule(&light_lightness_linear_pa.work, K_MSEC(trans));
	} else {
		light_lightness_linear_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_lightness_linear);
	return;
}

static int light_lightness_srv_get(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t lightness = 0;
		lightness = get_present_lightness(model, FALSE);
		k_mutex_lock(&mutex_lightness, K_FOREVER);
		rtk_bt_mesh_generic_transition_time_t remain_time = light_lightness_pa.remain_time;
		bool trans_flag = light_lightness_pa.transition_flag;
		uint16_t target_lightness = light_lightness_pa.target_lightness;
		if (trans_flag && remain_time.num_steps < light_lightness_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		k_mutex_unlock(&mutex_lightness);
		light_lightness_stat(model, ctx, lightness, trans_flag, target_lightness, remain_time, FALSE);
	}
	return 0;
}

int light_lightness_srv_set_unack(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	(void)model;
	uint16_t lightness = net_buf_simple_pull_le16(buf);
	/*****check lightness in the range******/
	rtk_bt_mesh_light_lightness_server_get_range_t range = {0, 0};
	rtk_bt_mesh_light_lightness_server_direct_get_range_t *range_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_range_t));
	range_get = (rtk_bt_mesh_light_lightness_server_direct_get_range_t *)p_evt->data;
	range_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);
	if ((0 != range.range_min) && (0 != range.range_max)) {
		/* need to clamp lightness */
		lightness = CLAMP(lightness, range.range_min, range.range_max);
	}

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_lightness_server_set_last_t));
	memcpy(p_evt->data, &lightness, sizeof(uint16_t));
	rtk_bt_evt_indicate(p_evt, NULL);

	uint8_t tid = net_buf_simple_pull_u8(buf);
	// int32_t trans = 0;
	int32_t delay = 0;
	k_mutex_lock(&mutex_lightness, K_FOREVER);
	if (tid == light_lightness_pa.tid && ctx->addr == light_lightness_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_lightness);
		return 0;
	}
	if (lightness == light_lightness_pa.target_lightness) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_lightness);
		BT_LOGE("unlock success when same value \r\n");
		return 0;
	}

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_lightness_pa.total_time.num_steps = trans_time & 0x3F;
		light_lightness_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_lightness_pa.remain_time = light_lightness_pa.total_time;
		light_lightness_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_lightness_pa.total_time.num_steps = 0;
		light_lightness_pa.total_time.step_resolution = 0;
		light_lightness_pa.remain_time = light_lightness_pa.total_time;
		light_lightness_pa.transition_flag = FALSE;

		light_lightness_pa.lightness = lightness;
		light_lightness_pa.target_lightness = lightness;
		light_lightness_pa.src = ctx->addr;
		light_lightness_pa.tid = tid;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_lightness_server_set_t lightness_set;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_lightness_server_set_t));
		lightness_set.lightness = lightness;
		lightness_set.total_time = light_lightness_pa.total_time;
		lightness_set.remaining_time = light_lightness_pa.remain_time;
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_lightness_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_lightness);
		return 0;
	}

	light_lightness_pa.lightness = lightness;
	light_lightness_pa.target_lightness = lightness;
	light_lightness_pa.src = ctx->addr;
	light_lightness_pa.tid = tid;
	/* Schedule the next action to happen on the delay, and keep
	 * transition time stored, so it can be applied in the timeout.
	 */
	k_work_reschedule(&light_lightness_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_lightness);
	return 0;
}

static int light_lightness_srv_set(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	light_lightness_srv_set_unack(model, ctx, buf);
	uint16_t lightness = get_present_lightness(model, FALSE);
	k_mutex_lock(&mutex_lightness, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = light_lightness_pa.remain_time;
	bool optional = !(light_lightness_pa.remain_time.num_steps == 0);
	uint16_t target_lightness = light_lightness_pa.target_lightness;
	if (light_lightness_pa.transition_flag && remain_time.num_steps < light_lightness_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_lightness);
	light_lightness_stat(model, ctx, lightness, optional, target_lightness, remain_time, FALSE);
	return 0;
}

static int light_lightness_srv_linear_get(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t lightness = 0;
		lightness = get_present_lightness(model, TRUE);
		k_mutex_lock(&mutex_lightness_linear, K_FOREVER);
		rtk_bt_mesh_generic_transition_time_t remain_time = light_lightness_linear_pa.remain_time;
		bool trans_flag = light_lightness_linear_pa.transition_flag;
		uint16_t target_lightness_linear = light_lightness_linear_pa.target_lightness_linear;
		if (trans_flag && remain_time.num_steps < light_lightness_linear_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		k_mutex_unlock(&mutex_lightness_linear);
		light_lightness_stat(model, ctx, lightness, trans_flag, target_lightness_linear, remain_time, TRUE);
	}
	return 0;
}

static int light_lightness_srv_linear_set_unack(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx,
												struct net_buf_simple *buf)
{
	(void)model;
	uint16_t lightness = net_buf_simple_pull_le16(buf);
	/*****check lightness in the range******/
	rtk_bt_evt_t *p_evt = NULL;

	rtk_bt_mesh_light_lightness_server_get_range_t range = {0, 0};
	rtk_bt_mesh_light_lightness_server_direct_get_range_t *range_get;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_range_t));
	range_get = (rtk_bt_mesh_light_lightness_server_direct_get_range_t *)p_evt->data;
	range_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);
	if ((0 != range.range_min) && (0 != range.range_max)) {
		/* need to clamp lightness */
		lightness = CLAMP(lightness, light_lightness_actual_to_linear(range.range_min),
						  light_lightness_actual_to_linear(range.range_max));
	}

	rtk_bt_mesh_light_lightness_server_set_last_t p_set_data;
	p_set_data.lightness = light_lightness_linear_to_actual(lightness);
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_lightness_server_set_last_t));
	memcpy(p_evt->data, &p_set_data, sizeof(rtk_bt_mesh_light_lightness_server_set_last_t));
	rtk_bt_evt_indicate(p_evt, NULL);

	uint8_t tid = net_buf_simple_pull_u8(buf);
	// int32_t trans = 0;
	int32_t delay = 0;

	k_mutex_lock(&mutex_lightness_linear, K_FOREVER);
	if (tid == light_lightness_linear_pa.tid && ctx->addr == light_lightness_linear_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_lightness_linear);
		return 0;
	}

	if (lightness == light_lightness_linear_pa.target_lightness_linear) {
		// BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_lightness_linear);
		return 0;
	}

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_lightness_linear_pa.total_time.num_steps = trans_time & 0x3F;
		light_lightness_linear_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_lightness_linear_pa.remain_time = light_lightness_linear_pa.total_time;
		light_lightness_linear_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_lightness_linear_pa.total_time.num_steps = 0;
		light_lightness_linear_pa.total_time.step_resolution = 0;
		light_lightness_linear_pa.remain_time = light_lightness_linear_pa.total_time;
		light_lightness_linear_pa.transition_flag = FALSE;

		light_lightness_linear_pa.lightness_linear = lightness;
		light_lightness_linear_pa.target_lightness_linear = lightness;
		light_lightness_linear_pa.src = ctx->addr;
		light_lightness_linear_pa.tid = tid;

		rtk_bt_mesh_light_lightness_server_set_t lightness_set;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_lightness_server_set_t));
		lightness_set.lightness = lightness;
		lightness_set.total_time = light_lightness_linear_pa.total_time;
		lightness_set.remaining_time = light_lightness_linear_pa.remain_time;
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_lightness_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_lightness_linear);
		return 0;
	}

	light_lightness_linear_pa.lightness_linear = lightness;
	light_lightness_linear_pa.target_lightness_linear = lightness;
	light_lightness_linear_pa.src = ctx->addr;
	light_lightness_linear_pa.tid = tid;
	/* Schedule the next action to happen on the delay, and keep
	 * transition time stored, so it can be applied in the timeout.
	 */
	k_work_reschedule(&light_lightness_linear_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_lightness_linear);

	return 0;
}

static int light_lightness_srv_linear_set(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	light_lightness_srv_linear_set_unack(model, ctx, buf);
	uint16_t lightness = get_present_lightness(model, TRUE);
	k_mutex_lock(&mutex_lightness_linear, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = light_lightness_linear_pa.remain_time;
	bool trans_flag = light_lightness_linear_pa.transition_flag;
	uint16_t target_lightness_linear = light_lightness_linear_pa.target_lightness_linear;
	if (trans_flag && remain_time.num_steps < light_lightness_linear_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_lightness_linear);
	light_lightness_stat(model, ctx, lightness, trans_flag, target_lightness_linear, remain_time, TRUE);
	return 0;
}

static int light_lightness_srv_last_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lightness_server_direct_get_t *last_get;
	rtk_bt_mesh_light_lightness_server_get_t last_value;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_t));
	last_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)p_evt->data;
	last_get->value = &last_value;
	rtk_bt_evt_indicate(p_evt, NULL);
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_LAST_STATUS, 2);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_LAST_STATUS);
	net_buf_simple_add_mem(&pmsg, &(last_value.lightness), sizeof(uint16_t));
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_lightness_srv_default_get(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lightness_server_direct_get_t *default_get;
	rtk_bt_mesh_light_lightness_server_get_t default_val = {0};
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_t));
	default_get = (rtk_bt_mesh_light_lightness_server_direct_get_t *)p_evt->data;
	default_get->value = &default_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, 2);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);
	net_buf_simple_add_le16(&pmsg, default_val.lightness);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_lightness_srv_range_get(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lightness_server_direct_get_range_t *range_get;
	rtk_bt_mesh_light_lightness_server_get_range_t range = {0, 0};
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_lightness_server_direct_get_range_t));
	range_get = (rtk_bt_mesh_light_lightness_server_direct_get_range_t *)p_evt->data;
	range_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_RANGE_STATUS, 5);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_RANGE_STATUS);
	net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
	net_buf_simple_add_le16(&pmsg, range.range_min);
	net_buf_simple_add_le16(&pmsg, range.range_max);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

/* Mapping of message handlers for Light Lightness Server (0x1300) */
const struct bt_mesh_model_op light_lightness_srv_op[] = {
	{ OP_LIGHT_LIGHTNESS_GET,               BT_MESH_LEN_EXACT(0),   light_lightness_srv_get },
	{ OP_LIGHT_LIGHTNESS_SET,               BT_MESH_LEN_MIN(3),     light_lightness_srv_set },
	{ OP_LIGHT_LIGHTNESS_SET_UNACK,         BT_MESH_LEN_MIN(3),     light_lightness_srv_set_unack },
	{ OP_LIGHT_LIGHTNESS_LINEAR_GET,        BT_MESH_LEN_EXACT(0),   light_lightness_srv_linear_get },
	{ OP_LIGHT_LIGHTNESS_LINEAR_SET,        BT_MESH_LEN_MIN(3),     light_lightness_srv_linear_set },
	{ OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK,  BT_MESH_LEN_MIN(3),     light_lightness_srv_linear_set_unack },
	{ OP_LIGHT_LIGHTNESS_LAST_GET,          BT_MESH_LEN_EXACT(0),   light_lightness_srv_last_get },
	{ OP_LIGHT_LIGHTNESS_DEFAULT_GET,       BT_MESH_LEN_EXACT(0),   light_lightness_srv_default_get },
	{ OP_LIGHT_LIGHTNESS_RANGE_GET,         BT_MESH_LEN_EXACT(0),   light_lightness_srv_range_get },
	BT_MESH_MODEL_OP_END,
};

static int light_lightness_srv_init(const struct bt_mesh_model *model)
{
	light_lightness_server_model = model;
	k_work_init_delayable(&light_lightness_pa.work, lightness_timeout);
	k_mutex_init(&mutex_lightness);
	k_work_init_delayable(&light_lightness_linear_pa.work, lightness_linear_timeout);
	k_mutex_init(&mutex_lightness_linear);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_lightness_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_lightness);
	k_mutex_deinit(&mutex_lightness_linear);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_lightness_srv_cb = {
	.init = light_lightness_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_lightness_srv_deinit,
#endif
};
#endif

/****LIGHT LIGHTNESS SETUP SERVER****/
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL
static const struct bt_mesh_model *light_lightness_setup_server_model = NULL;

static int light_lightness_setup_srv_init(const struct bt_mesh_model *model)
{
	light_lightness_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_lightness_setup_srv_cb = {
	.init = light_lightness_setup_srv_init,
};

static int light_lightness_setup_srv_default_set_unack(const struct bt_mesh_model *model,
													   struct bt_mesh_msg_ctx *ctx,
													   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_light_lightness_server_set_default_t set_default;
		set_default.lightness = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_lightness_server_set_default_t));
		memcpy(p_evt->data, &set_default, sizeof(rtk_bt_mesh_light_lightness_server_set_default_t));
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	return 0;
}

static int light_lightness_setup_srv_default_set(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	uint16_t lightness = LE_TO_U16(buf->data);
	light_lightness_setup_srv_default_set_unack(model, ctx, buf);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_DEFAULT_STATUS, 2);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_DEFAULT_STATUS);
	net_buf_simple_add_le16(&pmsg, lightness);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_lightness_setup_srv_range_set_unack(const struct bt_mesh_model *model,
													 struct bt_mesh_msg_ctx *ctx,
													 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_light_lightness_server_set_range_t range_default;
		range_default.range_min = net_buf_simple_pull_le16(buf);
		range_default.range_max = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_lightness_server_set_range_t));
		memcpy(p_evt->data, &range_default, sizeof(rtk_bt_mesh_light_lightness_server_set_range_t));
		rtk_bt_evt_indicate(p_evt, NULL);
	}
	return 0;
}

static int light_lightness_setup_srv_range_set(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	rtk_bt_mesh_light_lightness_server_set_range_t range_default;
	range_default.range_min = LE_TO_U16(buf->data);
	range_default.range_max = LE_TO_U16(buf->data + 2);
	light_lightness_setup_srv_range_set_unack(model, ctx, buf);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_LIGHTNESS_RANGE_STATUS, 5);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_LIGHTNESS_RANGE_STATUS);
	net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
	net_buf_simple_add_le16(&pmsg, range_default.range_min);
	net_buf_simple_add_le16(&pmsg, range_default.range_max);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}


/* Mapping of message handlers for Light Lightness Setup Server (0x1301) */
const struct bt_mesh_model_op light_lightness_setup_srv_op[] = {
	{ OP_LIGHT_LIGHTNESS_DEFAULT_SET,       BT_MESH_LEN_EXACT(2),   light_lightness_setup_srv_default_set },
	{ OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK, BT_MESH_LEN_EXACT(2),   light_lightness_setup_srv_default_set_unack },
	{ OP_LIGHT_LIGHTNESS_RANGE_SET,         BT_MESH_LEN_EXACT(4),   light_lightness_setup_srv_range_set },
	{ OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK,   BT_MESH_LEN_EXACT(4),   light_lightness_setup_srv_range_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

/*****LIGHT LIGHTNESS CLIENT******/
#if defined(BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LIGHTNESS_CLIENT_MODEL
static const struct bt_mesh_model *light_lightness_client_model = NULL;

static int light_lightness_cli_init(const struct bt_mesh_model *model)
{
	light_lightness_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_lightness_cli_cb = {
	.init = light_lightness_cli_init,
};

static int light_lightness_cli_status(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lightness_client_status_t status_data;
	status_data.src = ctx->addr;
	status_data.present_lightness = net_buf_simple_pull_le16(buf);
	status_data.optional = FALSE;
	if (buf->len) {
		status_data.optional = TRUE;
		status_data.target_lightness = net_buf_simple_pull_le16(buf);
		uint8_t remain_time = net_buf_simple_pull_u8(buf);
		memcpy(&status_data.remaining_time, &remain_time, 1);
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_lightness_client_status_t));
	memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lightness_client_status_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_lightness_cli_linear_status(const struct bt_mesh_model *model,
											 struct bt_mesh_msg_ctx *ctx,
											 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lightness_client_status_t status_data;
	status_data.src = ctx->addr;
	status_data.present_lightness = net_buf_simple_pull_le16(buf);
	status_data.optional = FALSE;
	if (buf->len) {
		status_data.optional = TRUE;
		status_data.target_lightness = net_buf_simple_pull_le16(buf);
		uint8_t remain_time = net_buf_simple_pull_u8(buf);
		memcpy(&status_data.remaining_time, &remain_time, 1);
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_lightness_client_status_t));
	memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lightness_client_status_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_lightness_cli_last_status(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lightness_client_status_last_t status_data;
	status_data.src = ctx->addr;
	status_data.lightness = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_lightness_client_status_last_t));
	memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lightness_client_status_last_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_lightness_cli_default_status(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lightness_client_status_default_t status_data;
	status_data.src = ctx->addr;
	status_data.lightness = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_lightness_client_status_default_t));
	memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lightness_client_status_default_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_lightness_cli_range_status(const struct bt_mesh_model *model,
											struct bt_mesh_msg_ctx *ctx,
											struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lightness_client_status_range_t status_data;
	status_data.src = ctx->addr;
	status_data.status = net_buf_simple_pull_u8(buf);
	status_data.range_min = net_buf_simple_pull_le16(buf);
	status_data.range_max = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_lightness_client_status_range_t));
	memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lightness_client_status_range_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}


/* Mapping of message handlers for Light Lightness Client (0x1302) */
const struct bt_mesh_model_op light_lightness_cli_op[] = {
	{ OP_LIGHT_LIGHTNESS_STATUS,            BT_MESH_LEN_MIN(2),     light_lightness_cli_status },
	{ OP_LIGHT_LIGHTNESS_LINEAR_STATUS,     BT_MESH_LEN_MIN(2),     light_lightness_cli_linear_status },
	{ OP_LIGHT_LIGHTNESS_LAST_STATUS,       BT_MESH_LEN_EXACT(2),   light_lightness_cli_last_status },
	{ OP_LIGHT_LIGHTNESS_DEFAULT_STATUS,    BT_MESH_LEN_EXACT(2),   light_lightness_cli_default_status },
	{ OP_LIGHT_LIGHTNESS_RANGE_STATUS,      BT_MESH_LEN_EXACT(5),   light_lightness_cli_range_status },
	BT_MESH_MODEL_OP_END,
};

/********SEND MSG API**********/
static int light_lightness_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_set(const struct bt_mesh_model *model, uint16_t dst,
							   uint16_t app_key_index, uint16_t lightness, uint8_t tid, bool optional,
							   rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_LIGHT_LIGHTNESS_SET : OP_LIGHT_LIGHTNESS_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_linear_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_LINEAR_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_LINEAR_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_linear_set(const struct bt_mesh_model *model, uint16_t dst,
									  uint16_t app_key_index, uint16_t lightness, uint8_t tid, bool optional,
									  rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_LIGHT_LIGHTNESS_LINEAR_SET : OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_LINEAR_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_last_get(const struct bt_mesh_model *model, uint16_t dst,
									uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_LAST_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_LAST_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_default_get(const struct bt_mesh_model *model, uint16_t dst,
									   uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_DEFAULT_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_DEFAULT_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_default_set(const struct bt_mesh_model *model, uint16_t dst,
									   uint16_t app_key_index, uint16_t lightness, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_LIGHTNESS_DEFAULT_SET : OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_DEFAULT_SET, 2);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_range_get(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_RANGE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LIGHTNESS_RANGE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lightness_range_set(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index, uint16_t range_min, uint16_t range_max, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_LIGHTNESS_RANGE_SET : OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LIGHTNESS_RANGE_SET, 4);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, range_min);
	net_buf_simple_add_le16(&buf, range_max);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_light_lightness_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	int ret = -1;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lightness_get_t *lightness_get = (rtk_bt_mesh_light_lightness_get_t *)p_cmd->param;
		ret = light_lightness_get(light_lightness_client_model, lightness_get->dst, lightness_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lightness_set_t *lightness_set = (rtk_bt_mesh_light_lightness_set_t *)p_cmd->param;
		ret = light_lightness_set(light_lightness_client_model, lightness_set->dst, lightness_set->app_key_index,
								  lightness_set->lightness, lightness_set->tid, lightness_set->optional,
								  lightness_set->trans_time, lightness_set->delay, lightness_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lightness_linear_get_t *lightness_linear_get = (rtk_bt_mesh_light_lightness_linear_get_t *)p_cmd->param;
		ret = light_lightness_linear_get(light_lightness_client_model, lightness_linear_get->dst, lightness_linear_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LINEAR_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lightness_linear_set_t *lightness_linear_set = (rtk_bt_mesh_light_lightness_linear_set_t *)p_cmd->param;
		ret = light_lightness_linear_set(light_lightness_client_model, lightness_linear_set->dst, lightness_linear_set->app_key_index,
										 lightness_linear_set->lightness, lightness_linear_set->tid, lightness_linear_set->optional,
										 lightness_linear_set->trans_time, lightness_linear_set->delay, lightness_linear_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_LAST_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lightness_last_get_t *lightness_last_get = (rtk_bt_mesh_light_lightness_last_get_t *)p_cmd->param;
		ret = light_lightness_last_get(light_lightness_client_model, lightness_last_get->dst, lightness_last_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lightness_default_get_t *lightness_default_get = (rtk_bt_mesh_light_lightness_default_get_t *)p_cmd->param;
		ret = light_lightness_default_get(light_lightness_client_model, lightness_default_get->dst, lightness_default_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lightness_default_set_t *lightness_default_set = (rtk_bt_mesh_light_lightness_default_set_t *)p_cmd->param;
		ret = light_lightness_default_set(light_lightness_client_model, lightness_default_set->dst, lightness_default_set->app_key_index,
										  lightness_default_set->lightness, lightness_default_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lightness_range_get_t *lightness_range_get = (rtk_bt_mesh_light_lightness_range_get_t *)p_cmd->param;
		ret = light_lightness_range_get(light_lightness_client_model, lightness_range_get->dst, lightness_range_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LIGHTNESS_RANGE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lightness_range_set_t *lightness_range_set = (rtk_bt_mesh_light_lightness_range_set_t *)p_cmd->param;
		ret = light_lightness_range_set(light_lightness_client_model, lightness_range_set->dst, lightness_range_set->app_key_index,
										lightness_range_set->range_min, lightness_range_set->range_max, lightness_range_set->ack);
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
/*********************END OF LIGHT LIGHTNESS CODE & FUNCTION********************/


/******************OPCODE DEFINE*********************/
/*****************LIGHT CTL MODEL PART*********************/
#define OP_LIGHT_CTL_GET                  BT_MESH_MODEL_OP_2(0x82, 0x5D)
#define OP_LIGHT_CTL_SET                  BT_MESH_MODEL_OP_2(0x82, 0x5E)
#define OP_LIGHT_CTL_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x5F)
#define OP_LIGHT_CTL_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x60)
#define OP_LIGHT_CTL_TEMP_GET             BT_MESH_MODEL_OP_2(0x82, 0x61)
#define OP_LIGHT_CTL_TEMP_RANGE_GET       BT_MESH_MODEL_OP_2(0x82, 0x62)
#define OP_LIGHT_CTL_TEMP_RANGE_STATUS    BT_MESH_MODEL_OP_2(0x82, 0x63)
#define OP_LIGHT_CTL_TEMP_SET             BT_MESH_MODEL_OP_2(0x82, 0x64)
#define OP_LIGHT_CTL_TEMP_SET_UNACK       BT_MESH_MODEL_OP_2(0x82, 0x65)
#define OP_LIGHT_CTL_TEMP_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x66)
#define OP_LIGHT_CTL_DEFAULT_GET          BT_MESH_MODEL_OP_2(0x82, 0x67)
#define OP_LIGHT_CTL_DEFAULT_STATUS       BT_MESH_MODEL_OP_2(0x82, 0x68)

#define OP_LIGHT_CTL_DEFAULT_SET          BT_MESH_MODEL_OP_2(0x82, 0x69)
#define OP_LIGHT_CTL_DEFAULT_SET_UNACK    BT_MESH_MODEL_OP_2(0x82, 0x6A)
#define OP_LIGHT_CTL_TEMP_RANGE_SET       BT_MESH_MODEL_OP_2(0x82, 0x6B)
#define OP_LIGHT_CTL_TEMP_RANGE_SET_UNACK BT_MESH_MODEL_OP_2(0x82, 0x6C)

#define LIGHT_CTL_TEMPERATURE_LOWER_LIMIT               0x0320
#define LIGHT_CTL_TEMPERATURE_UPPER_LIMIT               0x4E20
#define LIGHT_CTL_TEMPERATURE_DELTA                     (0x4E20 - 0x0320)
#define LIGHT_CTL_TEMPERATURE_UNKNOWN                   0xFFFF
#define IS_LIGHT_CTL_TEMPERATURE_VALID(VAL)  \
    (((VAL) >= LIGHT_CTL_TEMPERATURE_LOWER_LIMIT) && ((VAL) <= LIGHT_CTL_TEMPERATURE_UPPER_LIMIT))
#define IS_LIGHT_CTL_TEMPERATURE_RANGE_VALID(VAL) \
    (IS_LIGHT_CTL_TEMPERATURE_VALID(VAL) || ((VAL) == LIGHT_CTL_TEMPERATURE_UNKNOWN))

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SERVER_MODEL
struct lightness_ctl_t {
	uint16_t lightness;
	uint16_t target_lightness;
	uint16_t temperature;
	uint16_t target_temperature;
	int16_t delta_uv;
	int16_t target_delta_uv;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_ctl_pa;

struct k_mutex mutex_ctl;
static const struct bt_mesh_model *light_ctl_server_model = NULL;

void light_ctl_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_ctl, K_FOREVER);
	if (!light_ctl_pa.transition_flag) {
		light_ctl_pa.remain_time.num_steps = 0;
		light_ctl_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_ctl_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_ctl_server_set_t *lightness_set = NULL;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_ctl_server_set_t));
	lightness_set = (rtk_bt_mesh_light_ctl_server_set_t *)p_evt->data;
	lightness_set->lightness = light_ctl_pa.lightness;
	lightness_set->temperature = light_ctl_pa.temperature;
	lightness_set->delta_uv = light_ctl_pa.delta_uv;
	lightness_set->total_time = light_ctl_pa.total_time;
	lightness_set->remaining_time = light_ctl_pa.remain_time;
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_ctl_pa.remain_time.num_steps) {
		light_ctl_pa.remain_time.num_steps--;
		k_work_reschedule(&light_ctl_pa.work, K_MSEC(trans));
	} else {
		light_ctl_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_ctl);
	return;
}

static int light_ctl_srv_init(const struct bt_mesh_model *model)
{
	light_ctl_server_model = model;
	k_work_init_delayable(&light_ctl_pa.work, light_ctl_timeout);
	k_mutex_init(&mutex_ctl);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_ctl_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_ctl);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_ctl_srv_cb = {
	.init = light_ctl_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_ctl_srv_deinit,
#endif
};

static int light_ctl_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t present_lightness, uint16_t present_temperature,
						  bool optional, uint16_t target_lightness, uint16_t target_temperature, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	if (optional) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_STATUS, 9);
		bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_STATUS);
		net_buf_simple_add_le16(&buf, present_lightness);
		net_buf_simple_add_le16(&buf, present_temperature);
		net_buf_simple_add_le16(&buf, target_lightness);
		net_buf_simple_add_le16(&buf, target_temperature);
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		return bt_mesh_msg_send(model, ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_STATUS, 4);
		bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_STATUS);
		net_buf_simple_add_le16(&buf, present_lightness);
		net_buf_simple_add_le16(&buf, present_temperature);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
}

int light_ctl_publish(const struct bt_mesh_model *model, uint16_t present_lightness, uint16_t present_temperature)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_ctl_stat(model, NULL, present_lightness, present_temperature, FALSE, 0, 0, remaining_time);
	/* If want to publish periodicly like spec, use below part
	 * when use below part, should check publication related info is ok first
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
	// BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_STATUS, 4);
	// bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_STATUS);
	// net_buf_simple_add_le16(&buf, present_lightness);
	// net_buf_simple_add_le16(&buf, present_temperature);
	// net_buf_simple_reset(model->pub->msg);
	// net_buf_simple_add_mem(model->pub->msg, buf.data, buf.len);
}

int light_ctl_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_STATUS);

	rtk_bt_mesh_light_ctl_server_get_t light_ctl = {0, 0};
	rtk_bt_mesh_light_ctl_server_direct_get_t *lightness_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_t));
	lightness_get = (rtk_bt_mesh_light_ctl_server_direct_get_t *)p_evt->data;
	lightness_get->value = &light_ctl;
	rtk_bt_evt_indicate(p_evt, NULL);

	net_buf_simple_add_le16(msg, light_ctl.lightness);
	net_buf_simple_add_le16(msg, light_ctl.temperature);
	return 0;
}

static int light_ctl_srv_get(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	(void)buf;
	// if (!buf->len) {
	rtk_bt_mesh_light_ctl_server_get_t light_ctl = {0, 0};
	rtk_bt_mesh_light_ctl_server_direct_get_t *lightness_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_t));
	lightness_get = (rtk_bt_mesh_light_ctl_server_direct_get_t *)p_evt->data;
	lightness_get->value = &light_ctl;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_ctl, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_ctl_pa.remain_time;
	bool trans_flag = light_ctl_pa.transition_flag;
	uint16_t target_lightness = light_ctl_pa.target_lightness;
	uint16_t target_temperature = light_ctl_pa.target_temperature;
	if (trans_flag && remain_time.num_steps < light_ctl_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}

	k_mutex_unlock(&mutex_ctl);
	light_ctl_stat(model, ctx, light_ctl.lightness, light_ctl.temperature, trans_flag,
				   target_lightness, target_temperature, remain_time);
	// }
	return 0;
}

static int light_ctl_srv_set_unack(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	(void)model;
	/**********Get msg info************/
	uint16_t lightness = net_buf_simple_pull_le16(buf);
	uint16_t temperature = net_buf_simple_pull_le16(buf);
	int16_t delta_uv = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	/*****Get range****/
	rtk_bt_mesh_light_ctl_server_get_temperature_range_t range;
	rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *range_ctl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t));
	range_ctl_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *)p_evt->data;
	range_ctl_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	if ((0 != range.range_min) && (0 != range.range_max)) {
		temperature = CLAMP(temperature, range.range_min, range.range_max);
	}

	// int32_t trans = 0;
	int32_t delay = 0;
	k_mutex_lock(&mutex_ctl, K_FOREVER);
	if (tid == light_ctl_pa.tid && ctx->addr == light_ctl_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_ctl);
		return 0;
	}
	if (lightness == light_ctl_pa.target_lightness && temperature == light_ctl_pa.target_temperature && delta_uv == light_ctl_pa.target_delta_uv) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_ctl);
		return 0;
	}

	light_ctl_pa.lightness = lightness;
	light_ctl_pa.target_lightness = lightness;
	light_ctl_pa.temperature = temperature;
	light_ctl_pa.target_temperature = temperature;
	light_ctl_pa.delta_uv = delta_uv;
	light_ctl_pa.target_delta_uv = delta_uv;
	light_ctl_pa.src = ctx->addr;
	light_ctl_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_ctl_pa.total_time.num_steps = trans_time & 0x3F;
		light_ctl_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_ctl_pa.remain_time = light_ctl_pa.total_time;
		light_ctl_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_ctl_pa.total_time.num_steps = 0;
		light_ctl_pa.total_time.step_resolution = 0;
		light_ctl_pa.remain_time = light_ctl_pa.total_time;
		light_ctl_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_ctl_server_set_t lightness_set;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_ctl_server_set_t));
		lightness_set.lightness = lightness;
		lightness_set.temperature = temperature;
		lightness_set.delta_uv = delta_uv;
		lightness_set.total_time = light_ctl_pa.total_time;
		lightness_set.remaining_time = light_ctl_pa.remain_time;
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_ctl_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_ctl);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&light_ctl_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_ctl);
	return 0;
}

static int light_ctl_srv_set(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	light_ctl_srv_set_unack(model, ctx, buf);
	light_ctl_srv_get(model, ctx, buf);
	return 0;
}

static int light_ctl_srv_default_get(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_light_ctl_server_get_default_t get_value;
		rtk_bt_mesh_light_ctl_server_direct_get_default_t *default_ctl_get;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_GET,
									sizeof(rtk_bt_mesh_light_ctl_server_direct_get_default_t));
		default_ctl_get = (rtk_bt_mesh_light_ctl_server_direct_get_default_t *)p_evt->data;
		default_ctl_get->value = &get_value;
		rtk_bt_evt_indicate(p_evt, NULL);
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_CTL_DEFAULT_STATUS, 6);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_CTL_DEFAULT_STATUS);
		net_buf_simple_add_le16(&pmsg, get_value.lightness);
		net_buf_simple_add_le16(&pmsg, get_value.temperature);
		net_buf_simple_add_le16(&pmsg, get_value.delta_uv);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static int light_ctl_srv_temp_range_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_ctl_server_get_temperature_range_t range;
	rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *range_ctl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t));
	range_ctl_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *)p_evt->data;
	range_ctl_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_CTL_TEMP_RANGE_STATUS, 5);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_CTL_TEMP_RANGE_STATUS);
	net_buf_simple_add_u8(&pmsg, 0);
	net_buf_simple_add_le16(&pmsg, range.range_min);
	net_buf_simple_add_le16(&pmsg, range.range_max);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

const struct bt_mesh_model_op light_ctl_srv_op[] = {
	{ OP_LIGHT_CTL_GET,               BT_MESH_LEN_EXACT(0),   light_ctl_srv_get },
	{ OP_LIGHT_CTL_SET,               BT_MESH_LEN_MIN(7),     light_ctl_srv_set },
	{ OP_LIGHT_CTL_SET_UNACK,         BT_MESH_LEN_MIN(7),     light_ctl_srv_set_unack },
	{ OP_LIGHT_CTL_DEFAULT_GET,       BT_MESH_LEN_EXACT(0),   light_ctl_srv_default_get },
	{ OP_LIGHT_CTL_TEMP_RANGE_GET,    BT_MESH_LEN_EXACT(0),   light_ctl_srv_temp_range_get },
	BT_MESH_MODEL_OP_END,
};

#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_TEMPERATURE_SERVER_MODEL
struct lightness_temp_ctl_t {
	uint16_t temperature;
	uint16_t target_temperature;
	int16_t delta_uv;
	int16_t target_delta_uv;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_ctl_temp_pa;

struct k_mutex mutex_ctl_temp;
static const struct bt_mesh_model *light_ctl_temp_server_model = NULL;

void light_ctl_temp_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_ctl_temp, K_FOREVER);
	if (!light_ctl_temp_pa.transition_flag) {
		light_ctl_temp_pa.remain_time.num_steps = 0;
		light_ctl_temp_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_ctl_temp_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_ctl_server_set_temperature_t lightness_set;
	rtk_bt_evt_t *p_evt = NULL;

	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_SET,
								sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_t));
	lightness_set.temperature = light_ctl_temp_pa.temperature;
	lightness_set.delta_uv = light_ctl_temp_pa.delta_uv;
	lightness_set.total_time = light_ctl_temp_pa.total_time;
	lightness_set.remaining_time = light_ctl_temp_pa.remain_time;
	memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_ctl_temp_pa.remain_time.num_steps) {
		light_ctl_temp_pa.remain_time.num_steps--;
		k_work_reschedule(&light_ctl_temp_pa.work, K_MSEC(trans));
	} else {
		light_ctl_temp_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_ctl_temp);
	return;
}

static int light_ctl_temp_srv_init(const struct bt_mesh_model *model)
{
	light_ctl_temp_server_model = model;
	k_work_init_delayable(&light_ctl_temp_pa.work, light_ctl_temp_timeout);
	k_mutex_init(&mutex_ctl_temp);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_ctl_temp_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_ctl_temp);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_ctl_temp_srv_cb = {
	.init = light_ctl_temp_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_ctl_temp_srv_deinit,
#endif
};

static int light_ctl_temp_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t present_temperature, int16_t present_delta_uv,
							   bool optional, uint16_t target_temperature, int16_t target_delta_uv, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	if (optional) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_STATUS, 9);
		bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_TEMP_STATUS);
		net_buf_simple_add_le16(&buf, present_temperature);
		net_buf_simple_add_le16(&buf, present_delta_uv);
		net_buf_simple_add_le16(&buf, target_temperature);
		net_buf_simple_add_le16(&buf, target_delta_uv);
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		return bt_mesh_msg_send(model, ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_STATUS, 4);
		bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_TEMP_STATUS);
		net_buf_simple_add_le16(&buf, present_temperature);
		net_buf_simple_add_le16(&buf, present_delta_uv);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
}

int light_ctl_temperature_publish(const struct bt_mesh_model *model, uint16_t present_temperature, int16_t present_delta_uv)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_ctl_temp_stat(model, NULL, present_temperature, present_delta_uv, FALSE, 0, 0, remaining_time);
	/* If want to publish periodicly like spec, use below part
	 * when use below part, should check publication related info is ok first
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
	// BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_STATUS, 4);
	// bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_TEMP_STATUS);
	// net_buf_simple_add_le16(&buf, present_temperature);
	// net_buf_simple_add_le16(&buf, present_delta_uv);
	// net_buf_simple_reset(model->pub->msg);
	// net_buf_simple_add_mem(model->pub->msg, buf.data, buf.len);
}

int light_ctl_temp_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;

	rtk_bt_mesh_light_ctl_server_get_temperature_t light_ctl_temp = {0, 0};
	rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *ctl_temperature_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_temperature_t));
	ctl_temperature_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *)p_evt->data;
	ctl_temperature_get->value = &light_ctl_temp;
	rtk_bt_evt_indicate(p_evt, NULL);

	bt_mesh_model_msg_init(msg, OP_LIGHT_CTL_TEMP_STATUS);
	net_buf_simple_add_le16(msg, light_ctl_temp.temperature);
	net_buf_simple_add_le16(msg, light_ctl_temp.delta_uv);
	return 0;
}

static int light_ctl_srv_temp_get(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_ctl_server_get_temperature_t light_ctl_temp = {0, 0};
	rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *ctl_temperature_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_GET,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_temperature_t));
	ctl_temperature_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_t *)p_evt->data;
	ctl_temperature_get->value = &light_ctl_temp;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_ctl_temp, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_ctl_temp_pa.remain_time;
	bool trans_flag = light_ctl_temp_pa.transition_flag;
	uint16_t target_temperature = light_ctl_temp_pa.target_temperature;
	uint16_t target_delta_uv = light_ctl_temp_pa.target_delta_uv;
	if (trans_flag && remain_time.num_steps < light_ctl_temp_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}

	k_mutex_unlock(&mutex_ctl_temp);
	light_ctl_temp_stat(model, ctx, light_ctl_temp.temperature, light_ctl_temp.delta_uv, trans_flag,
						target_temperature, target_delta_uv, remain_time);
	return 0;
}

static int light_ctl_srv_temp_set_unack(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)model;
	/**********Get msg info************/
	uint16_t temperature = net_buf_simple_pull_le16(buf);
	int16_t delta_uv = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	/*****Get range****/
	rtk_bt_mesh_light_ctl_server_get_temperature_range_t range;
	rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *range_ctl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_GET_T,
								sizeof(rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t));
	range_ctl_get = (rtk_bt_mesh_light_ctl_server_direct_get_temperature_range_t *)p_evt->data;
	range_ctl_get->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	if ((0 != range.range_min) && (0 != range.range_max)) {
		temperature = CLAMP(temperature, range.range_min, range.range_max);
	}

	// int32_t trans = 0;
	int32_t delay = 0;
	k_mutex_lock(&mutex_ctl_temp, K_FOREVER);
	if (tid == light_ctl_temp_pa.tid && ctx->addr == light_ctl_temp_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_ctl_temp);
		return 0;
	}
	if (delta_uv == light_ctl_temp_pa.target_delta_uv && temperature == light_ctl_temp_pa.target_temperature) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_ctl_temp);
		return 0;
	}

	light_ctl_temp_pa.delta_uv = delta_uv;
	light_ctl_temp_pa.target_delta_uv = delta_uv;
	light_ctl_temp_pa.temperature = temperature;
	light_ctl_temp_pa.target_temperature = temperature;
	light_ctl_temp_pa.src = ctx->addr;
	light_ctl_temp_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_ctl_temp_pa.total_time.num_steps = trans_time & 0x3F;
		light_ctl_temp_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_ctl_temp_pa.remain_time = light_ctl_temp_pa.total_time;
		light_ctl_temp_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_ctl_temp_pa.total_time.num_steps = 0;
		light_ctl_temp_pa.total_time.step_resolution = 0;
		light_ctl_temp_pa.remain_time = light_ctl_temp_pa.total_time;
		light_ctl_temp_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_ctl_server_set_temperature_t lightness_set;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_TEMPERATURE_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_SET,
									sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_t));
		lightness_set.temperature = temperature;
		lightness_set.delta_uv = delta_uv;
		lightness_set.total_time = light_ctl_temp_pa.total_time;
		lightness_set.remaining_time = light_ctl_temp_pa.remain_time;
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_ctl_temp);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&light_ctl_temp_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_ctl_temp);
	return 0;
}

static int light_ctl_srv_temp_set(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	light_ctl_srv_temp_set_unack(model, ctx, buf);
	light_ctl_srv_temp_get(model, ctx, buf);
	return 0;
}

const struct bt_mesh_model_op light_ctl_temp_srv_op[] = {
	{ OP_LIGHT_CTL_TEMP_GET,               BT_MESH_LEN_EXACT(0),   light_ctl_srv_temp_get },
	{ OP_LIGHT_CTL_TEMP_SET,               BT_MESH_LEN_MIN(5),     light_ctl_srv_temp_set },
	{ OP_LIGHT_CTL_TEMP_SET_UNACK,         BT_MESH_LEN_MIN(5),     light_ctl_srv_temp_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_SETUP_SERVER_MODEL
static const struct bt_mesh_model *light_ctl_setup_server_model = NULL;

static int light_ctl_setup_srv_init(const struct bt_mesh_model *model)
{
	light_ctl_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_ctl_setup_srv_cb = {
	.init = light_ctl_setup_srv_init,
};

static int light_ctl_setup_srv_default_set_unack(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	rtk_bt_mesh_light_ctl_server_set_default_t light_set;
	light_set.lightness = net_buf_simple_pull_le16(buf);
	light_set.temperature = net_buf_simple_pull_le16(buf);
	if (IS_LIGHT_CTL_TEMPERATURE_VALID(light_set.temperature)) {
		light_set.delta_uv = net_buf_simple_pull_le16(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_DEFAULT_SET,
									sizeof(rtk_bt_mesh_light_ctl_server_set_default_t));
		memcpy(p_evt->data, &light_set, sizeof(rtk_bt_mesh_light_ctl_server_set_default_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		return 0;
	}
	return -1;
}

static int light_ctl_setup_srv_default_set(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	struct net_buf_simple_state state;
	net_buf_simple_save(buf, &state);
	if (0 == light_ctl_setup_srv_default_set_unack(model, ctx, buf)) {
		net_buf_simple_restore(buf, &state);
		rtk_bt_mesh_light_ctl_server_set_default_t light_set;
		light_set.lightness = net_buf_simple_pull_le16(buf);
		light_set.temperature = net_buf_simple_pull_le16(buf);
		light_set.delta_uv = net_buf_simple_pull_le16(buf);
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_DEFAULT_STATUS, 6);
		bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_DEFAULT_STATUS);
		net_buf_simple_add_le16(&buf, light_set.lightness);
		net_buf_simple_add_le16(&buf, light_set.temperature);
		net_buf_simple_add_le16(&buf, light_set.delta_uv);
		return bt_mesh_msg_send(model, ctx, &buf);
	}
	return -1;
}

static int light_ctl_setup_srv_range_set_unack(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	rtk_bt_mesh_light_ctl_server_set_temperature_range_t range;
	range.range_min = net_buf_simple_pull_le16(buf);
	range.range_max = net_buf_simple_pull_le16(buf);
	if ((range.range_min < range.range_max) &&
		IS_LIGHT_CTL_TEMPERATURE_RANGE_VALID(range.range_max) &&
		IS_LIGHT_CTL_TEMPERATURE_RANGE_VALID(range.range_max)) {
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_CTL_SERVER_MODEL_TEMPERATURE_RANGE_SET,
									sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_range_t));
		memcpy(p_evt->data, &range, sizeof(rtk_bt_mesh_light_ctl_server_set_temperature_range_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		return 0;
	}
	return -1;
}

static int light_ctl_setup_srv_range_set(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	rtk_bt_mesh_light_ctl_server_set_temperature_range_t range_default;
	range_default.range_min = LE_TO_U16(buf->data);
	range_default.range_max = LE_TO_U16(buf->data + 2);
	if (0 == light_ctl_setup_srv_range_set_unack(model, ctx, buf)) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_CTL_TEMP_RANGE_STATUS, 5);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_CTL_TEMP_RANGE_STATUS);
		net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
		net_buf_simple_add_le16(&pmsg, range_default.range_min);
		net_buf_simple_add_le16(&pmsg, range_default.range_max);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

const struct bt_mesh_model_op light_ctl_setup_srv_op[] = {
	{ OP_LIGHT_CTL_DEFAULT_SET,               BT_MESH_LEN_EXACT(6),   light_ctl_setup_srv_default_set },
	{ OP_LIGHT_CTL_DEFAULT_SET_UNACK,         BT_MESH_LEN_EXACT(6),   light_ctl_setup_srv_default_set_unack },
	{ OP_LIGHT_CTL_TEMP_RANGE_SET,            BT_MESH_LEN_EXACT(4),   light_ctl_setup_srv_range_set },
	{ OP_LIGHT_CTL_TEMP_RANGE_SET_UNACK,      BT_MESH_LEN_EXACT(4),   light_ctl_setup_srv_range_set_unack },
	BT_MESH_MODEL_OP_END,
};

#endif

#if defined(BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_CTL_CLIENT_MODEL
static const struct bt_mesh_model *light_ctl_client_model = NULL;

static int light_ctl_cli_init(const struct bt_mesh_model *model)
{
	light_ctl_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_ctl_cli_cb = {
	.init = light_ctl_cli_init,
};

static int light_ctl_cli_status(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx,
								struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_ctl_client_status_t ctx_stat = {0};
	ctx_stat.optional = FALSE;
	ctx_stat.src = ctx->addr;
	ctx_stat.present_lightness = net_buf_simple_pull_le16(buf);
	ctx_stat.present_temperature = net_buf_simple_pull_le16(buf);
	if (buf->len) {
		ctx_stat.optional = TRUE;
		ctx_stat.target_lightness = net_buf_simple_pull_le16(buf);
		ctx_stat.target_temperature = net_buf_simple_pull_le16(buf);
		ctx_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_ctl_client_status_t));
	memcpy(p_evt->data, &ctx_stat, sizeof(rtk_bt_mesh_light_ctl_client_status_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_ctl_cli_temp_range_status(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_ctl_client_status_temperature_range_t temp_range_stat = {0};
	temp_range_stat.src = ctx->addr;
	temp_range_stat.status = *(rtk_bt_mesh_generic_stat_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_stat_t)));
	temp_range_stat.range_min = net_buf_simple_pull_le16(buf);
	temp_range_stat.range_max = net_buf_simple_pull_le16(buf);

	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_STATUS,
								sizeof(rtk_bt_mesh_light_ctl_client_status_temperature_range_t));
	memcpy(p_evt->data, &temp_range_stat, sizeof(rtk_bt_mesh_light_ctl_client_status_temperature_range_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_ctl_cli_temp_status(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_ctl_client_status_temperature_t temp_stat;
	temp_stat.optional = FALSE;
	temp_stat.src = ctx->addr;
	temp_stat.present_temperature = net_buf_simple_pull_le16(buf);
	temp_stat.present_delta_uv = net_buf_simple_pull_le16(buf);
	if (buf->len) {
		temp_stat.optional = TRUE;
		temp_stat.target_temperature = net_buf_simple_pull_le16(buf);
		temp_stat.target_delta_uv = net_buf_simple_pull_le16(buf);
		temp_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_STATUS,
								sizeof(rtk_bt_mesh_light_ctl_client_status_temperature_t));
	memcpy(p_evt->data, &temp_stat, sizeof(rtk_bt_mesh_light_ctl_client_status_temperature_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int light_ctl_cli_default_status(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_ctl_client_status_default_t default_stat;
	default_stat.src = ctx->addr;
	default_stat.lightness = net_buf_simple_pull_le16(buf);
	default_stat.temperature = net_buf_simple_pull_le16(buf);
	default_stat.delta_uv = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_CTL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_STATUS,
								sizeof(rtk_bt_mesh_light_ctl_client_status_default_t));
	memcpy(p_evt->data, &default_stat, sizeof(rtk_bt_mesh_light_ctl_client_status_default_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

const struct bt_mesh_model_op light_ctl_cli_op[] = {
	{ OP_LIGHT_CTL_STATUS,               BT_MESH_LEN_MIN(4),     light_ctl_cli_status },
	{ OP_LIGHT_CTL_TEMP_RANGE_STATUS,    BT_MESH_LEN_EXACT(5),   light_ctl_cli_temp_range_status },
	{ OP_LIGHT_CTL_TEMP_STATUS,          BT_MESH_LEN_MIN(4),     light_ctl_cli_temp_status },
	{ OP_LIGHT_CTL_DEFAULT_STATUS,       BT_MESH_LEN_EXACT(6),   light_ctl_cli_default_status },
	BT_MESH_MODEL_OP_END,
};

static int light_ctl_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_set(const struct bt_mesh_model *model, uint16_t dst,
						 uint16_t app_key_index, uint16_t lightness, uint16_t temperature, int16_t delta_uv, uint8_t tid,
						 bool optional, rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 7;
	if (optional) {
		msg_len = 9;
	}
	uint32_t op = ack ? OP_LIGHT_CTL_SET : OP_LIGHT_CTL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, temperature);
	net_buf_simple_add_le16(&buf, delta_uv);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_temperature_get(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_TEMP_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_temperature_set(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index, uint16_t temperature, int16_t delta_uv, uint8_t tid, bool optional,
									 rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 5;
	if (optional) {
		msg_len = 7;
	}
	uint32_t op = ack ? OP_LIGHT_CTL_TEMP_SET : OP_LIGHT_CTL_TEMP_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, temperature);
	net_buf_simple_add_le16(&buf, delta_uv);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_temperature_range_get(const struct bt_mesh_model *model,
										   uint16_t dst,
										   uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_RANGE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_TEMP_RANGE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_temperature_range_set(const struct bt_mesh_model *model,
										   uint16_t dst, uint16_t app_key_index, uint16_t range_min, uint16_t range_max, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_CTL_TEMP_RANGE_SET : OP_LIGHT_CTL_TEMP_RANGE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_TEMP_RANGE_SET, 4);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, range_min);
	net_buf_simple_add_le16(&buf, range_max);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_default_get(const struct bt_mesh_model *model, uint16_t dst,
								 uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_DEFAULT_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_CTL_DEFAULT_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_ctl_default_set(const struct bt_mesh_model *model, uint16_t dst,
								 uint16_t app_key_index, uint16_t lightness, uint16_t temperature, int16_t delta_uv, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_CTL_DEFAULT_SET : OP_LIGHT_CTL_DEFAULT_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_CTL_DEFAULT_SET, 6);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, temperature);
	net_buf_simple_add_le16(&buf, delta_uv);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_light_ctl_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_ctl_get_t *ctl_get = (rtk_bt_mesh_light_ctl_get_t *)p_cmd->param;
		ret = light_ctl_get(light_ctl_client_model, ctl_get->dst, ctl_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_ctl_set_t *ctl_set = (rtk_bt_mesh_light_ctl_set_t *)p_cmd->param;
		ret = light_ctl_set(light_ctl_client_model, ctl_set->dst, ctl_set->app_key_index, ctl_set->lightness,
							ctl_set->temperature, ctl_set->delta_uv, ctl_set->tid, ctl_set->optional,
							ctl_set->trans_time, ctl_set->delay, ctl_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_GET: {
		rtk_bt_mesh_light_ctl_temperature_get_t *ctl_temperature_get = (rtk_bt_mesh_light_ctl_temperature_get_t *)p_cmd->param;
		ret = light_ctl_temperature_get(light_ctl_client_model, ctl_temperature_get->dst, ctl_temperature_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_SET: {
		rtk_bt_mesh_light_ctl_temperature_set_t *ctl_temperature_set = (rtk_bt_mesh_light_ctl_temperature_set_t *)p_cmd->param;
		ret = light_ctl_temperature_set(light_ctl_client_model, ctl_temperature_set->dst, ctl_temperature_set->app_key_index,
										ctl_temperature_set->temperature, ctl_temperature_set->delta_uv, ctl_temperature_set->tid,
										ctl_temperature_set->optional, ctl_temperature_set->trans_time, ctl_temperature_set->delay, ctl_temperature_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_GET: {
		rtk_bt_mesh_light_ctl_temperature_range_get_t *ctl_temperature_range_get = (rtk_bt_mesh_light_ctl_temperature_range_get_t *)p_cmd->param;
		ret = light_ctl_temperature_range_get(light_ctl_client_model, ctl_temperature_range_get->dst, ctl_temperature_range_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_TEMPERATURE_RANGE_SET: {
		rtk_bt_mesh_light_ctl_temperature_range_set_t *ctl_temperature_range_set = (rtk_bt_mesh_light_ctl_temperature_range_set_t *)p_cmd->param;
		ret = light_ctl_temperature_range_set(light_ctl_client_model, ctl_temperature_range_set->dst, ctl_temperature_range_set->app_key_index,
											  ctl_temperature_range_set->range_min, ctl_temperature_range_set->range_max, ctl_temperature_range_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_GET: {
		rtk_bt_mesh_light_ctl_default_get_t *ctl_default_get = (rtk_bt_mesh_light_ctl_default_get_t *)p_cmd->param;
		ret = light_ctl_default_get(light_ctl_client_model, ctl_default_get->dst, ctl_default_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_CTL_CLIENT_MODEL_DEFAULT_SET: {
		rtk_bt_mesh_light_ctl_default_set_t *ctl_default_set = (rtk_bt_mesh_light_ctl_default_set_t *)p_cmd->param;
		ret = light_ctl_default_set(light_ctl_client_model, ctl_default_set->dst, ctl_default_set->app_key_index, ctl_default_set->lightness,
									ctl_default_set->temperature, ctl_default_set->delta_uv, ctl_default_set->ack);
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

#define OP_LIGHT_HSL_GET                   BT_MESH_MODEL_OP_2(0x82, 0x6D)
#define OP_LIGHT_HSL_HUE_GET               BT_MESH_MODEL_OP_2(0x82, 0x6E)
#define OP_LIGHT_HSL_HUE_SET               BT_MESH_MODEL_OP_2(0x82, 0x6F)
#define OP_LIGHT_HSL_HUE_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x70)
#define OP_LIGHT_HSL_HUE_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x71)
#define OP_LIGHT_HSL_SATURATION_GET        BT_MESH_MODEL_OP_2(0x82, 0x72)
#define OP_LIGHT_HSL_SATURATION_SET        BT_MESH_MODEL_OP_2(0x82, 0x73)
#define OP_LIGHT_HSL_SATURATION_SET_UNACK  BT_MESH_MODEL_OP_2(0x82, 0x74)
#define OP_LIGHT_HSL_SATURATION_STATUS     BT_MESH_MODEL_OP_2(0x82, 0x75)
#define OP_LIGHT_HSL_SET                   BT_MESH_MODEL_OP_2(0x82, 0x76)
#define OP_LIGHT_HSL_SET_UNACK             BT_MESH_MODEL_OP_2(0x82, 0x77)
#define OP_LIGHT_HSL_STATUS                BT_MESH_MODEL_OP_2(0x82, 0x78)
#define OP_LIGHT_HSL_TARGET_GET            BT_MESH_MODEL_OP_2(0x82, 0x79)
#define OP_LIGHT_HSL_TARGET_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x7A)
#define OP_LIGHT_HSL_DEFAULT_GET           BT_MESH_MODEL_OP_2(0x82, 0x7B)
#define OP_LIGHT_HSL_DEFAULT_STATUS        BT_MESH_MODEL_OP_2(0x82, 0x7C)
#define OP_LIGHT_HSL_RANGE_GET             BT_MESH_MODEL_OP_2(0x82, 0x7D)
#define OP_LIGHT_HSL_RANGE_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x7E)

#define OP_LIGHT_HSL_DEFAULT_SET           BT_MESH_MODEL_OP_2(0x82, 0x7F)
#define OP_LIGHT_HSL_DEFAULT_SET_UNACK     BT_MESH_MODEL_OP_2(0x82, 0x80)
#define OP_LIGHT_HSL_RANGE_SET             BT_MESH_MODEL_OP_2(0x82, 0x81)
#define OP_LIGHT_HSL_RANGE_SET_UNACK       BT_MESH_MODEL_OP_2(0x82, 0x82)

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SERVER_MODEL
struct lightness_hsl_t {
	uint16_t lightness;
	uint16_t target_lightness;
	uint16_t hue;
	uint16_t target_hue;
	uint16_t saturation;
	uint16_t target_saturation;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_hsl_pa;

struct k_mutex mutex_hsl;
static const struct bt_mesh_model *light_hsl_server_model = NULL;

void light_hsl_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_hsl, K_FOREVER);
	if (!light_hsl_pa.transition_flag) {
		light_hsl_pa.remain_time.num_steps = 0;
		light_hsl_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_hsl_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_hsl_server_set_t lightness_set;
	lightness_set.lightness = light_hsl_pa.lightness;
	lightness_set.hue = light_hsl_pa.hue;
	lightness_set.saturation = light_hsl_pa.saturation;
	lightness_set.total_time = light_hsl_pa.total_time;
	lightness_set.remaining_time = light_hsl_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_hsl_server_set_t));
	memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_hsl_pa.remain_time.num_steps) {
		light_hsl_pa.remain_time.num_steps--;
		k_work_reschedule(&light_hsl_pa.work, K_MSEC(trans));
	} else {
		light_hsl_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_hsl);
	return;
}

static int light_hsl_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t lightness, uint16_t hue, uint16_t saturation,
						  bool optional, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 6;
	if (optional) {
		msg_len = 7;
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_STATUS);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, hue);
	net_buf_simple_add_le16(&buf, saturation);
	if (optional) {
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

int light_hsl_publish(const struct bt_mesh_model *model, uint16_t present_lightness, uint16_t present_hue, uint16_t present_saturation)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_hsl_stat(model, NULL, present_lightness, present_hue, present_saturation, FALSE, remaining_time);
	/* If want to publish periodicly like spec,
	 * use light_hsl_srv_update_publish(),
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
}

int light_hsl_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, OP_LIGHT_HSL_STATUS);

	rtk_bt_mesh_light_hsl_server_get_t hsl_data;
	rtk_bt_mesh_light_hsl_server_direct_get_t *hsl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_t));
	hsl_get = (rtk_bt_mesh_light_hsl_server_direct_get_t *)p_evt->data;
	hsl_get->value = &hsl_data;
	rtk_bt_evt_indicate(p_evt, NULL);

	net_buf_simple_add_le16(msg, hsl_data.lightness);
	net_buf_simple_add_le16(msg, hsl_data.hue);
	net_buf_simple_add_le16(msg, hsl_data.saturation);
	return 0;
}

static int light_hsl_srv_get(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_hsl_server_get_t hsl_data;
	rtk_bt_mesh_light_hsl_server_direct_get_t *hsl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_t));
	hsl_get = (rtk_bt_mesh_light_hsl_server_direct_get_t *)p_evt->data;
	hsl_get->value = &hsl_data;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_hsl, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_hsl_pa.remain_time;
	bool trans_flag = light_hsl_pa.transition_flag;
	if (trans_flag && remain_time.num_steps < light_hsl_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_hsl);
	light_hsl_stat(model, ctx, hsl_data.lightness, hsl_data.hue, hsl_data.saturation,
				   trans_flag, remain_time);
	return 0;
}

static int light_hsl_srv_set_unack(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	(void)model;
	uint16_t lightness = net_buf_simple_pull_le16(buf);
	uint16_t hue = net_buf_simple_pull_le16(buf);
	uint16_t saturation = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	/*****Get range*****/
	rtk_bt_mesh_light_hsl_server_get_range_t range;
	rtk_bt_mesh_light_hsl_server_direct_get_range_t *hsl_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_range_t));
	hsl_range = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)p_evt->data;
	hsl_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	if ((0 != range.hue_range_min) && (0 != range.hue_range_max)) {
		hue = CLAMP(hue, range.hue_range_min, range.hue_range_max);
	}

	if ((0 != range.saturation_range_min) && (0 != range.saturation_range_max)) {
		saturation = CLAMP(saturation, range.saturation_range_min, range.saturation_range_max);
	}

	int32_t delay = 0;
	k_mutex_lock(&mutex_hsl, K_FOREVER);
	if (tid == light_hsl_pa.tid && ctx->addr == light_hsl_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_hsl);
		return 0;
	}
	if (lightness == light_hsl_pa.target_lightness && hue == light_hsl_pa.target_hue && saturation == light_hsl_pa.target_saturation) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_hsl);
		return 0;
	}

	light_hsl_pa.lightness = lightness;
	light_hsl_pa.target_lightness = lightness;
	light_hsl_pa.hue = hue;
	light_hsl_pa.target_hue = hue;
	light_hsl_pa.saturation = saturation;
	light_hsl_pa.target_saturation = saturation;
	light_hsl_pa.src = ctx->addr;
	light_hsl_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_hsl_pa.total_time.num_steps = trans_time & 0x3F;
		light_hsl_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_hsl_pa.remain_time = light_hsl_pa.total_time;
		light_hsl_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_hsl_pa.total_time.num_steps = 0;
		light_hsl_pa.total_time.step_resolution = 0;
		light_hsl_pa.remain_time = light_hsl_pa.total_time;
		light_hsl_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_hsl_server_set_t lightness_set;
		lightness_set.lightness = lightness;
		lightness_set.hue = hue;
		lightness_set.saturation = saturation;
		lightness_set.total_time = light_hsl_pa.total_time;
		lightness_set.remaining_time = light_hsl_pa.remain_time;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_hsl_server_set_t));
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_hsl);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&light_hsl_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_hsl);
	return 0;

}

static int light_hsl_srv_set(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	light_hsl_srv_set_unack(model, ctx, buf);
	light_hsl_srv_get(model, ctx, buf);
	return 0;
}

static int light_hsl_srv_target_get(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)buf;
	k_mutex_lock(&mutex_hsl, K_FOREVER);
	uint16_t msg_len = 6;
	if (light_hsl_pa.transition_flag) {
		msg_len = 7;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_TARGET_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_TARGET_STATUS);
	net_buf_simple_add_le16(&pmsg, light_hsl_pa.target_lightness);
	net_buf_simple_add_le16(&pmsg, light_hsl_pa.target_hue);
	net_buf_simple_add_le16(&pmsg, light_hsl_pa.target_saturation);
	if (light_hsl_pa.transition_flag) {
		rtk_bt_mesh_generic_transition_time_t remain_time = light_hsl_pa.remain_time;
		if (remain_time.num_steps < light_hsl_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		net_buf_simple_add_mem(&pmsg, &remain_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	k_mutex_unlock(&mutex_hsl);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_hsl_srv_default_get(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_hsl_server_get_default_t get_default;
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_light_hsl_server_direct_get_default_t *hsl_default;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_default_t));
	hsl_default = (rtk_bt_mesh_light_hsl_server_direct_get_default_t *)p_evt->data;
	hsl_default->value = &get_default;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_DEFAULT_STATUS, 6);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_DEFAULT_STATUS);
	net_buf_simple_add_le16(&pmsg, get_default.lightness);
	net_buf_simple_add_le16(&pmsg, get_default.hue);
	net_buf_simple_add_le16(&pmsg, get_default.saturation);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_hsl_srv_range_get(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_hsl_server_get_range_t range;
	rtk_bt_mesh_light_hsl_server_direct_get_range_t *hsl_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_range_t));
	hsl_range = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)p_evt->data;
	hsl_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_RANGE_STATUS, 9);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_RANGE_STATUS);
	net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
	net_buf_simple_add_le16(&pmsg, range.hue_range_min);
	net_buf_simple_add_le16(&pmsg, range.hue_range_max);
	net_buf_simple_add_le16(&pmsg, range.saturation_range_min);
	net_buf_simple_add_le16(&pmsg, range.saturation_range_max);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_hsl_srv_init(const struct bt_mesh_model *model)
{
	light_hsl_server_model = model;
	k_work_init_delayable(&light_hsl_pa.work, light_hsl_timeout);
	k_mutex_init(&mutex_hsl);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_hsl_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_hsl);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_hsl_srv_cb = {
	.init = light_hsl_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_hsl_srv_deinit,
#endif
};

const struct bt_mesh_model_op light_hsl_srv_op[] = {
	{ OP_LIGHT_HSL_GET,               BT_MESH_LEN_EXACT(0),   light_hsl_srv_get },
	{ OP_LIGHT_HSL_SET,               BT_MESH_LEN_MIN(7),     light_hsl_srv_set },
	{ OP_LIGHT_HSL_SET_UNACK,         BT_MESH_LEN_MIN(7),     light_hsl_srv_set_unack },
	{ OP_LIGHT_HSL_TARGET_GET,        BT_MESH_LEN_EXACT(0),   light_hsl_srv_target_get },
	{ OP_LIGHT_HSL_DEFAULT_GET,       BT_MESH_LEN_EXACT(0),   light_hsl_srv_default_get },
	{ OP_LIGHT_HSL_RANGE_GET,         BT_MESH_LEN_EXACT(0),   light_hsl_srv_range_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_HUE_SERVER_MODEL
struct lightness_hsl_hue_t {
	uint16_t hue;
	uint16_t target_hue;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_hsl_hue_pa;

struct k_mutex mutex_hsl_hue;
static const struct bt_mesh_model *light_hsl_hue_server_model = NULL;

void light_hsl_hue_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_hsl_hue, K_FOREVER);
	if (!light_hsl_hue_pa.transition_flag) {
		light_hsl_hue_pa.remain_time.num_steps = 0;
		light_hsl_hue_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_hsl_hue_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_hsl_server_set_hue_t lightness_set;
	lightness_set.hue = light_hsl_hue_pa.hue;
	lightness_set.total_time = light_hsl_hue_pa.total_time;
	lightness_set.remaining_time = light_hsl_hue_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_hsl_server_set_hue_t));
	memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_hue_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_hsl_hue_pa.remain_time.num_steps) {
		light_hsl_hue_pa.remain_time.num_steps--;
		k_work_reschedule(&light_hsl_hue_pa.work, K_MSEC(trans));
	} else {
		light_hsl_hue_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_hsl_hue);
	return;
}

static int light_hsl_hue_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t present_hue,
							  bool optional, uint16_t target_hue, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 2;
	if (optional) {
		msg_len = 5;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_HUE_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_HUE_STATUS);
	net_buf_simple_add_le16(&pmsg, present_hue);
	if (optional) {
		net_buf_simple_add_le16(&pmsg, target_hue);
		net_buf_simple_add_mem(&pmsg, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

int light_hsl_hue_publish(const struct bt_mesh_model *model, uint16_t present_hue)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_hsl_hue_stat(model, NULL, present_hue, FALSE, 0, remaining_time);
	/* If want to publish periodicly like spec,
	 * use light_hsl_hue_stat(),
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
}

int light_hsl_hue_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, OP_LIGHT_HSL_HUE_STATUS);

	rtk_bt_mesh_light_hsl_server_get_hue_t hue_val;
	rtk_bt_mesh_light_hsl_server_direct_get_hue_t *hue_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_hue_t));
	hue_get = (rtk_bt_mesh_light_hsl_server_direct_get_hue_t *)p_evt->data;
	hue_get->value = &hue_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	net_buf_simple_add_le16(msg, hue_val.hue);
	return 0;
}

static int light_hsl_hue_srv_get(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_hsl_server_get_hue_t hue_val;
	rtk_bt_mesh_light_hsl_server_direct_get_hue_t *hue_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_hue_t));
	hue_get = (rtk_bt_mesh_light_hsl_server_direct_get_hue_t *)p_evt->data;
	hue_get->value = &hue_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_hsl_hue, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_hsl_hue_pa.remain_time;
	bool trans_flag = light_hsl_hue_pa.transition_flag;
	uint16_t target_hue = light_hsl_hue_pa.target_hue;
	if (trans_flag && remain_time.num_steps < light_hsl_hue_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_hsl_hue);
	light_hsl_hue_stat(model, ctx, hue_val.hue, trans_flag, target_hue, remain_time);
	return 0;
}

static int light_hsl_hue_srv_set_unack(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	(void)model;
	uint16_t hue = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	/*************Get range*****************/
	rtk_bt_mesh_light_hsl_server_get_range_t range;
	rtk_bt_mesh_light_hsl_server_direct_get_range_t *hue_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_H,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_range_t));
	hue_range = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)p_evt->data;
	hue_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);
	if ((0 != range.hue_range_min) && (0 != range.hue_range_max)) {
		hue = CLAMP(hue, range.hue_range_min, range.hue_range_max);
	}

	int32_t delay = 0;
	k_mutex_lock(&mutex_hsl_hue, K_FOREVER);
	if (tid == light_hsl_hue_pa.tid && ctx->addr == light_hsl_hue_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_hsl_hue);
		return 0;
	}
	if (hue == light_hsl_hue_pa.target_hue) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_hsl_hue);
		return 0;
	}


	light_hsl_hue_pa.hue = hue;
	light_hsl_hue_pa.target_hue = hue;

	light_hsl_hue_pa.src = ctx->addr;
	light_hsl_hue_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_hsl_hue_pa.total_time.num_steps = trans_time & 0x3F;
		light_hsl_hue_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_hsl_hue_pa.remain_time = light_hsl_hue_pa.total_time;
		light_hsl_hue_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_hsl_hue_pa.total_time.num_steps = 0;
		light_hsl_hue_pa.total_time.step_resolution = 0;
		light_hsl_hue_pa.remain_time = light_hsl_hue_pa.total_time;
		light_hsl_hue_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_hsl_server_set_hue_t lightness_set;
		lightness_set.hue = light_hsl_hue_pa.hue;
		lightness_set.total_time = light_hsl_hue_pa.total_time;
		lightness_set.remaining_time = light_hsl_hue_pa.remain_time;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_HUE_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_hsl_server_set_hue_t));
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_hue_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_hsl_hue);
		return 0;
	}

	k_work_reschedule(&light_hsl_hue_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_hsl_hue);
	return 0;

}

static int light_hsl_hue_srv_set(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	light_hsl_hue_srv_set_unack(model, ctx, buf);
	light_hsl_hue_srv_get(model, ctx, buf);
	return 0;
}

static int light_hsl_hue_srv_init(const struct bt_mesh_model *model)
{
	light_hsl_hue_server_model = model;
	k_work_init_delayable(&light_hsl_hue_pa.work, light_hsl_hue_timeout);
	k_mutex_init(&mutex_hsl_hue);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_hsl_hue_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_hsl_hue);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_hsl_hue_srv_cb = {
	.init = light_hsl_hue_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_hsl_hue_srv_deinit,
#endif
};

const struct bt_mesh_model_op light_hsl_hue_srv_op[] = {
	{ OP_LIGHT_HSL_HUE_GET,             BT_MESH_LEN_EXACT(0),   light_hsl_hue_srv_get },
	{ OP_LIGHT_HSL_HUE_SET,             BT_MESH_LEN_MIN(3),     light_hsl_hue_srv_set },
	{ OP_LIGHT_HSL_HUE_SET_UNACK,       BT_MESH_LEN_MIN(3),     light_hsl_hue_srv_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SATURATION_SERVER_MODEL
struct lightness_hsl_saturation_t {
	uint16_t saturation;
	uint16_t target_saturation;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_hsl_saturation_pa;

struct k_mutex mutex_hsl_saturation;
static const struct bt_mesh_model *light_hsl_saturation_server_model = NULL;

void light_hsl_saturation_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_hsl_saturation, K_FOREVER);
	if (!light_hsl_saturation_pa.transition_flag) {
		light_hsl_saturation_pa.remain_time.num_steps = 0;
		light_hsl_saturation_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_hsl_saturation_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_hsl_server_set_saturation_t lightness_set;
	lightness_set.saturation = light_hsl_saturation_pa.saturation;
	lightness_set.total_time = light_hsl_saturation_pa.total_time;
	lightness_set.remaining_time = light_hsl_saturation_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_hsl_server_set_saturation_t));
	memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_saturation_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_hsl_saturation_pa.remain_time.num_steps) {
		light_hsl_saturation_pa.remain_time.num_steps--;
		k_work_reschedule(&light_hsl_saturation_pa.work, K_MSEC(trans));
	} else {
		light_hsl_saturation_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_hsl_saturation);
	return;
}

static int light_hsl_saturation_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t present_saturation,
									 bool optional, uint16_t target_saturation, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 2;
	if (optional) {
		msg_len = 5;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_SATURATION_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_SATURATION_STATUS);
	net_buf_simple_add_le16(&pmsg, present_saturation);
	if (optional) {
		net_buf_simple_add_le16(&pmsg, target_saturation);
		net_buf_simple_add_mem(&pmsg, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

int light_hsl_saturation_publish(const struct bt_mesh_model *model, uint16_t present_saturation)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_hsl_saturation_stat(model, NULL, present_saturation, FALSE, 0, remaining_time);
	/* If want to publish periodicly like spec,
	 * use light_hsl_saturation_stat(),
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
}

int light_hsl_saturation_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, OP_LIGHT_HSL_SATURATION_STATUS);

	rtk_bt_mesh_light_hsl_server_get_saturation_t saturation_val;
	rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *saturation_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_saturation_t));
	saturation_get = (rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *)p_evt->data;
	saturation_get->value = &saturation_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	net_buf_simple_add_le16(msg, saturation_val.saturation);
	return 0;
}

static int light_hsl_saturation_srv_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_hsl_server_get_saturation_t saturation_val;
	rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *saturation_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_saturation_t));
	saturation_get = (rtk_bt_mesh_light_hsl_server_direct_get_saturation_t *)p_evt->data;
	saturation_get->value = &saturation_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_hsl_saturation, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_hsl_saturation_pa.remain_time;
	bool trans_flag = light_hsl_saturation_pa.transition_flag;
	uint16_t target_saturation = light_hsl_saturation_pa.target_saturation;
	if (trans_flag && remain_time.num_steps < light_hsl_saturation_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_hsl_saturation);
	light_hsl_saturation_stat(model, ctx, saturation_val.saturation, trans_flag, target_saturation,
							  remain_time);
	return 0;
}

static int light_hsl_saturation_srv_set_unack(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)model;
	uint16_t saturation = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	/*************Get range*****************/
	rtk_bt_mesh_light_hsl_server_get_range_t range;
	rtk_bt_mesh_light_hsl_server_direct_get_range_t *saturation_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_GET_S,
								sizeof(rtk_bt_mesh_light_hsl_server_direct_get_range_t));
	saturation_range = (rtk_bt_mesh_light_hsl_server_direct_get_range_t *)p_evt->data;
	saturation_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);
	if ((0 != range.saturation_range_min) && (0 != range.saturation_range_max)) {
		saturation = CLAMP(saturation, range.saturation_range_min, range.saturation_range_max);
	}

	int32_t delay = 0;
	k_mutex_lock(&mutex_hsl_saturation, K_FOREVER);
	if (tid == light_hsl_saturation_pa.tid && ctx->addr == light_hsl_saturation_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_hsl_saturation);
		return 0;
	}
	if (saturation == light_hsl_saturation_pa.target_saturation) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_hsl_saturation);
		return 0;
	}


	light_hsl_saturation_pa.saturation = saturation;
	light_hsl_saturation_pa.target_saturation = saturation;

	light_hsl_saturation_pa.src = ctx->addr;
	light_hsl_saturation_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_hsl_saturation_pa.total_time.num_steps = trans_time & 0x3F;
		light_hsl_saturation_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_hsl_saturation_pa.remain_time = light_hsl_saturation_pa.total_time;
		light_hsl_saturation_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_hsl_saturation_pa.total_time.num_steps = 0;
		light_hsl_saturation_pa.total_time.step_resolution = 0;
		light_hsl_saturation_pa.remain_time = light_hsl_saturation_pa.total_time;
		light_hsl_saturation_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_hsl_server_set_saturation_t lightness_set;
		lightness_set.saturation = light_hsl_saturation_pa.saturation;
		lightness_set.total_time = light_hsl_saturation_pa.total_time;
		lightness_set.remaining_time = light_hsl_saturation_pa.remain_time;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_hsl_server_set_saturation_t));
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_hsl_server_set_saturation_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_hsl_saturation);
		return 0;
	}

	k_work_reschedule(&light_hsl_saturation_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_hsl_saturation);
	return 0;

}

static int light_hsl_saturation_srv_set(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	light_hsl_saturation_srv_set_unack(model, ctx, buf);
	light_hsl_saturation_srv_get(model, ctx, buf);
	return 0;
}

static int light_hsl_saturation_srv_init(const struct bt_mesh_model *model)
{
	light_hsl_saturation_server_model = model;
	k_work_init_delayable(&light_hsl_saturation_pa.work, light_hsl_saturation_timeout);
	k_mutex_init(&mutex_hsl_saturation);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_hsl_saturation_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_hsl_saturation);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_hsl_saturation_srv_cb = {
	.init = light_hsl_saturation_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_hsl_saturation_srv_deinit,
#endif
};

const struct bt_mesh_model_op light_hsl_saturation_srv_op[] = {
	{ OP_LIGHT_HSL_SATURATION_GET,             BT_MESH_LEN_EXACT(0),   light_hsl_saturation_srv_get },
	{ OP_LIGHT_HSL_SATURATION_SET,             BT_MESH_LEN_MIN(3),     light_hsl_saturation_srv_set },
	{ OP_LIGHT_HSL_SATURATION_SET_UNACK,       BT_MESH_LEN_MIN(3),     light_hsl_saturation_srv_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_SETUP_SERVER_MODEL
static const struct bt_mesh_model *light_hsl_setup_server_model;

static rtk_bt_mesh_light_hsl_server_set_default_t hsl_default;
static int light_hsl_setup_srv_default_set_unack(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	hsl_default.lightness = net_buf_simple_pull_le16(buf);
	hsl_default.hue = net_buf_simple_pull_le16(buf);
	hsl_default.saturation = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_DEFAULT_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_hsl_server_set_default_t));
	memcpy(p_evt->data, &hsl_default, sizeof(rtk_bt_mesh_light_hsl_server_set_default_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_setup_srv_default_set(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	if (!light_hsl_setup_srv_default_set_unack(model, ctx, buf)) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_DEFAULT_STATUS, 6);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_DEFAULT_STATUS);
		net_buf_simple_add_le16(&pmsg, hsl_default.lightness);
		net_buf_simple_add_le16(&pmsg, hsl_default.hue);
		net_buf_simple_add_le16(&pmsg, hsl_default.saturation);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static rtk_bt_mesh_light_hsl_server_set_range_t hsl_range;
static int light_hsl_setup_srv_range_set_unack(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	hsl_range.hue_range_min = net_buf_simple_pull_le16(buf);
	hsl_range.hue_range_max = net_buf_simple_pull_le16(buf);
	hsl_range.saturation_range_min = net_buf_simple_pull_le16(buf);
	hsl_range.saturation_range_max = net_buf_simple_pull_le16(buf);
	if ((hsl_range.hue_range_min <= hsl_range.hue_range_max) &&
		(hsl_range.saturation_range_min <= hsl_range.saturation_range_max)) {
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_hsl_server_set_range_t));
		memcpy(p_evt->data, &hsl_range, sizeof(rtk_bt_mesh_light_hsl_server_set_range_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int light_hsl_setup_srv_range_set(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	if (!light_hsl_setup_srv_range_set_unack(model, ctx, buf)) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_HSL_RANGE_STATUS, 9);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_HSL_RANGE_STATUS);
		net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
		net_buf_simple_add_le16(&pmsg, hsl_range.hue_range_min);
		net_buf_simple_add_le16(&pmsg, hsl_range.hue_range_max);
		net_buf_simple_add_le16(&pmsg, hsl_range.saturation_range_min);
		net_buf_simple_add_le16(&pmsg, hsl_range.saturation_range_max);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static int light_hsl_setup_srv_init(const struct bt_mesh_model *model)
{
	light_hsl_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_hsl_setup_srv_cb = {
	.init = light_hsl_setup_srv_init,
};

const struct bt_mesh_model_op light_hsl_setup_srv_op[] = {
	{ OP_LIGHT_HSL_DEFAULT_SET,           BT_MESH_LEN_EXACT(6),    light_hsl_setup_srv_default_set },
	{ OP_LIGHT_HSL_DEFAULT_SET_UNACK,     BT_MESH_LEN_EXACT(6),    light_hsl_setup_srv_default_set_unack },
	{ OP_LIGHT_HSL_RANGE_SET,             BT_MESH_LEN_EXACT(8),    light_hsl_setup_srv_range_set },
	{ OP_LIGHT_HSL_RANGE_SET_UNACK,       BT_MESH_LEN_EXACT(8),    light_hsl_setup_srv_range_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_HSL_CLIENT_MODEL
static const struct bt_mesh_model *light_hsl_client_model = NULL;

static int light_hsl_cli_stat(const struct bt_mesh_model *model,
							  struct bt_mesh_msg_ctx *ctx,
							  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_t hsl_stat;
	hsl_stat.src = ctx->addr;
	hsl_stat.lightness = net_buf_simple_pull_le16(buf);
	hsl_stat.hue = net_buf_simple_pull_le16(buf);
	hsl_stat.saturation = net_buf_simple_pull_le16(buf);
	hsl_stat.optional = FALSE;
	if (buf->len) {
		hsl_stat.optional = TRUE;
		hsl_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_t));
	memcpy(p_evt->data, &hsl_stat, sizeof(rtk_bt_mesh_light_hsl_client_status_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_cli_target_stat(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_t hsl_stat;
	hsl_stat.src = ctx->addr;
	hsl_stat.lightness = net_buf_simple_pull_le16(buf);
	hsl_stat.hue = net_buf_simple_pull_le16(buf);
	hsl_stat.saturation = net_buf_simple_pull_le16(buf);
	hsl_stat.optional = FALSE;
	if (buf->len) {
		hsl_stat.optional = TRUE;
		hsl_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_t));
	memcpy(p_evt->data, &hsl_stat, sizeof(rtk_bt_mesh_light_hsl_client_status_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_cli_default_stat(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_default_t default_hsl;
	default_hsl.src = ctx->addr;
	default_hsl.lightness = net_buf_simple_pull_le16(buf);
	default_hsl.hue = net_buf_simple_pull_le16(buf);
	default_hsl.saturation = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_default_t));
	memcpy(p_evt->data, &default_hsl, sizeof(rtk_bt_mesh_light_hsl_client_status_default_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_cli_range_stat(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_range_t hsl_range;
	hsl_range.src = ctx->addr;
	hsl_range.status = net_buf_simple_pull_u8(buf);
	hsl_range.hue_range_min = net_buf_simple_pull_le16(buf);
	hsl_range.hue_range_max = net_buf_simple_pull_le16(buf);
	hsl_range.saturation_range_min = net_buf_simple_pull_le16(buf);
	hsl_range.saturation_range_max = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_range_t));
	memcpy(p_evt->data, &hsl_range, sizeof(rtk_bt_mesh_light_hsl_client_status_range_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_cli_hue_stat(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_hue_t hue_stat;
	hue_stat.src = ctx->addr;
	hue_stat.present_hue = net_buf_simple_pull_le16(buf);
	hue_stat.optional = FALSE;
	if (buf->len) {
		hue_stat.optional = TRUE;
		hue_stat.target_hue = net_buf_simple_pull_le16(buf);
		hue_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_hue_t));
	memcpy(p_evt->data, &hue_stat, sizeof(rtk_bt_mesh_light_hsl_client_status_hue_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_hsl_cli_saturation_stat(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_hsl_client_status_saturation_t saturation_stat;
	saturation_stat.src = ctx->addr;
	saturation_stat.present_saturation = net_buf_simple_pull_le16(buf);
	saturation_stat.optional = FALSE;
	if (buf->len) {
		saturation_stat.optional = TRUE;
		saturation_stat.target_saturation = net_buf_simple_pull_le16(buf);
		saturation_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_HSL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_hsl_client_status_saturation_t));
	memcpy(p_evt->data, &saturation_stat, sizeof(rtk_bt_mesh_light_hsl_client_status_saturation_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_model_op light_hsl_cli_op[] = {
	{ OP_LIGHT_HSL_STATUS,             BT_MESH_LEN_MIN(6),    light_hsl_cli_stat },
	{ OP_LIGHT_HSL_TARGET_STATUS,      BT_MESH_LEN_MIN(6),    light_hsl_cli_target_stat },
	{ OP_LIGHT_HSL_DEFAULT_STATUS,     BT_MESH_LEN_EXACT(6),  light_hsl_cli_default_stat },
	{ OP_LIGHT_HSL_RANGE_STATUS,       BT_MESH_LEN_EXACT(9),  light_hsl_cli_range_stat },
	{ OP_LIGHT_HSL_HUE_STATUS,         BT_MESH_LEN_MIN(2),    light_hsl_cli_hue_stat },
	{ OP_LIGHT_HSL_SATURATION_STATUS,  BT_MESH_LEN_MIN(2),    light_hsl_cli_saturation_stat },
	BT_MESH_MODEL_OP_END,
};

static int light_hsl_cli_init(const struct bt_mesh_model *model)
{
	light_hsl_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_hsl_cli_cb = {
	.init = light_hsl_cli_init,
};

static int light_hsl_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, uint16_t lightness, uint16_t hue, uint16_t saturation,
						 uint8_t tid,
						 bool optional, rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 7;
	if (optional) {
		msg_len = 9;
	}
	uint32_t op = ack ? OP_LIGHT_HSL_SET : OP_LIGHT_HSL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, hue);
	net_buf_simple_add_le16(&buf, saturation);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_target_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_TARGET_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_TARGET_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_hue_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_HUE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_HUE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_hue_set(const struct bt_mesh_model *model, uint16_t dst,
							 uint16_t app_key_index, uint16_t hue, uint8_t tid, bool optional,
							 rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_LIGHT_HSL_HUE_SET : OP_LIGHT_HSL_HUE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_HUE_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, hue);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_saturation_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_SATURATION_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_SATURATION_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_saturation_set(const struct bt_mesh_model *model, uint16_t dst,
									uint16_t app_key_index, uint16_t saturation, uint8_t tid, bool optional,
									rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_LIGHT_HSL_SATURATION_SET : OP_LIGHT_HSL_SATURATION_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_SATURATION_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, saturation);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_default_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_DEFAULT_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_DEFAULT_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_default_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
								 uint16_t lightness, uint16_t hue, uint16_t saturation, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_HSL_DEFAULT_SET : OP_LIGHT_HSL_DEFAULT_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_DEFAULT_SET, 6);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, hue);
	net_buf_simple_add_le16(&buf, saturation);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_range_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_RANGE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_HSL_RANGE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_hsl_range_set(const struct bt_mesh_model *model, uint16_t dst,
							   uint16_t app_key_index, uint16_t hue_range_min, uint16_t hue_range_max,
							   uint16_t saturation_range_min, uint16_t saturation_range_max, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_HSL_RANGE_SET : OP_LIGHT_HSL_RANGE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_HSL_RANGE_SET, 8);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, hue_range_min);
	net_buf_simple_add_le16(&buf, hue_range_max);
	net_buf_simple_add_le16(&buf, saturation_range_min);
	net_buf_simple_add_le16(&buf, saturation_range_max);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_light_hsl_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_get_t *hsl_get = (rtk_bt_mesh_light_hsl_get_t *)p_cmd->param;
		ret = light_hsl_get(light_hsl_client_model, hsl_get->dst, hsl_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_hsl_set_t *hsl_set = (rtk_bt_mesh_light_hsl_set_t *)p_cmd->param;
		ret = light_hsl_set(light_hsl_client_model, hsl_set->dst, hsl_set->app_key_index, hsl_set->lightness,
							hsl_set->hue, hsl_set->saturation, hsl_set->tid, hsl_set->optional,
							hsl_set->trans_time, hsl_set->delay, hsl_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_TARGET_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_target_get_t *hsl_target_get = (rtk_bt_mesh_light_hsl_target_get_t *)p_cmd->param;
		ret = light_hsl_target_get(light_hsl_client_model, hsl_target_get->dst, hsl_target_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_hue_get_t *hsl_hue_get = (rtk_bt_mesh_light_hsl_hue_get_t *)p_cmd->param;
		ret = light_hsl_hue_get(light_hsl_client_model, hsl_hue_get->dst, hsl_hue_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_HUE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_hsl_hue_set_t *hsl_hue_set = (rtk_bt_mesh_light_hsl_hue_set_t *)p_cmd->param;
		ret = light_hsl_hue_set(light_hsl_client_model, hsl_hue_set->dst, hsl_hue_set->app_key_index,
								hsl_hue_set->hue, hsl_hue_set->tid, hsl_hue_set->optional,
								hsl_hue_set->trans_time, hsl_hue_set->delay, hsl_hue_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_saturation_get_t *hsl_saturation_get = (rtk_bt_mesh_light_hsl_saturation_get_t *)p_cmd->param;
		ret = light_hsl_saturation_get(light_hsl_client_model, hsl_saturation_get->dst, hsl_saturation_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_SATURATION_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_hsl_saturation_set_t *hsl_saturation_set = (rtk_bt_mesh_light_hsl_saturation_set_t *)p_cmd->param;
		ret = light_hsl_saturation_set(light_hsl_client_model, hsl_saturation_set->dst, hsl_saturation_set->app_key_index,
									   hsl_saturation_set->saturation, hsl_saturation_set->tid, hsl_saturation_set->optional,
									   hsl_saturation_set->trans_time, hsl_saturation_set->delay, hsl_saturation_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_default_get_t *hsl_default_get = (rtk_bt_mesh_light_hsl_default_get_t *)p_cmd->param;
		ret = light_hsl_default_get(light_hsl_client_model, hsl_default_get->dst, hsl_default_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_DEFAULT_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_hsl_default_set_t *hsl_default_set = (rtk_bt_mesh_light_hsl_default_set_t *)p_cmd->param;
		ret = light_hsl_default_set(light_hsl_client_model, hsl_default_set->dst, hsl_default_set->app_key_index,
									hsl_default_set->lightness, hsl_default_set->hue, hsl_default_set->saturation, hsl_default_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_hsl_range_get_t *hsl_range_get = (rtk_bt_mesh_light_hsl_range_get_t *)p_cmd->param;
		ret = light_hsl_range_get(light_hsl_client_model, hsl_range_get->dst, hsl_range_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_HSL_RANGE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_hsl_range_set_t *hsl_range_set = (rtk_bt_mesh_light_hsl_range_set_t *)p_cmd->param;
		ret = light_hsl_range_set(light_hsl_client_model, hsl_range_set->dst, hsl_range_set->app_key_index, hsl_range_set->hue_range_min,
								  hsl_range_set->hue_range_max, hsl_range_set->saturation_range_min, hsl_range_set->saturation_range_max,
								  hsl_range_set->ack);
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

#define OP_LIGHT_XYL_GET                   BT_MESH_MODEL_OP_2(0x82, 0x83)
#define OP_LIGHT_XYL_SET                   BT_MESH_MODEL_OP_2(0x82, 0x84)
#define OP_LIGHT_XYL_SET_UNACK             BT_MESH_MODEL_OP_2(0x82, 0x85)
#define OP_LIGHT_XYL_STATUS                BT_MESH_MODEL_OP_2(0x82, 0x86)
#define OP_LIGHT_XYL_TARGET_GET            BT_MESH_MODEL_OP_2(0x82, 0x87)
#define OP_LIGHT_XYL_TARGET_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x88)
#define OP_LIGHT_XYL_DEFAULT_GET           BT_MESH_MODEL_OP_2(0x82, 0x89)
#define OP_LIGHT_XYL_DEFAULT_STATUS        BT_MESH_MODEL_OP_2(0x82, 0x8A)
#define OP_LIGHT_XYL_RANGE_GET             BT_MESH_MODEL_OP_2(0x82, 0x8B)
#define OP_LIGHT_XYL_RANGE_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x8C)

#define OP_LIGHT_XYL_DEFAULT_SET           BT_MESH_MODEL_OP_2(0x82, 0x8D)
#define OP_LIGHT_XYL_DEFAULT_SET_UNACK     BT_MESH_MODEL_OP_2(0x82, 0x8E)
#define OP_LIGHT_XYL_RANGE_SET             BT_MESH_MODEL_OP_2(0x82, 0x8F)
#define OP_LIGHT_XYL_RANGE_SET_UNACK       BT_MESH_MODEL_OP_2(0x82, 0x90)

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SERVER_MODEL
struct lightness_xyl_t {
	uint16_t lightness;
	uint16_t target_lightness;
	uint16_t xyl_x;
	uint16_t target_xyl_x;
	uint16_t xyl_y;
	uint16_t target_xyl_y;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_xyl_pa;

struct k_mutex mutex_xyl;
static const struct bt_mesh_model *light_xyl_server_model = NULL;

void light_xyl_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_xyl, K_FOREVER);
	if (!light_xyl_pa.transition_flag) {
		light_xyl_pa.remain_time.num_steps = 0;
		light_xyl_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_xyl_pa.remain_time.step_resolution];
	rtk_bt_mesh_light_xyl_server_set_t lightness_set;
	lightness_set.xyl.xyl_lightness = light_xyl_pa.lightness;
	lightness_set.xyl.xyl_x = light_xyl_pa.xyl_x;
	lightness_set.xyl.xyl_y = light_xyl_pa.xyl_y;
	lightness_set.total_time = light_xyl_pa.total_time;
	lightness_set.remaining_time = light_xyl_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_xyl_server_set_t));
	memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_xyl_server_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
	if (light_xyl_pa.remain_time.num_steps) {
		light_xyl_pa.remain_time.num_steps--;
		k_work_reschedule(&light_xyl_pa.work, K_MSEC(trans));
	} else {
		light_xyl_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_xyl);
	return;
}

static int light_xyl_stat(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t lightness, uint16_t xyl_x, uint16_t xyl_y,
						  bool optional, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 6;
	if (optional) {
		msg_len = 7;
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_XYL_STATUS);
	net_buf_simple_add_le16(&buf, lightness);
	net_buf_simple_add_le16(&buf, xyl_x);
	net_buf_simple_add_le16(&buf, xyl_y);
	if (optional) {
		net_buf_simple_add_mem(&buf, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

int light_xyl_publish(const struct bt_mesh_model *model, uint16_t lightness, uint16_t xyl_x, uint16_t xyl_y)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return light_xyl_stat(model, NULL, lightness, xyl_x, xyl_y, FALSE, remaining_time);
	/* If want to publish periodicly like spec,
	 * use light_xyl_srv_update_publish(),
	 * Or if want to use above, when need retransmit 3 times, should set 2
	 */
}

int light_xyl_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	bt_mesh_model_msg_init(msg, OP_LIGHT_XYL_STATUS);

	rtk_bt_mesh_light_xyl_server_get_t xyl_store;
	rtk_bt_mesh_light_xyl_server_direct_get_t *xyl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_xyl_server_direct_get_t));
	xyl_get = (rtk_bt_mesh_light_xyl_server_direct_get_t *)p_evt->data;
	xyl_get->value = &xyl_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	net_buf_simple_add_le16(msg, xyl_store.xyl_lightness);
	net_buf_simple_add_le16(msg, xyl_store.xyl_x);
	net_buf_simple_add_le16(msg, xyl_store.xyl_y);
	return 0;
}

static int light_xyl_srv_get(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_xyl_server_get_t xyl_store;
	rtk_bt_mesh_light_xyl_server_direct_get_t *xyl_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_xyl_server_direct_get_t));
	xyl_get = (rtk_bt_mesh_light_xyl_server_direct_get_t *)p_evt->data;
	xyl_get->value = &xyl_store;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_xyl, K_FOREVER);

	/****This part is to handle transition time*****/
	rtk_bt_mesh_generic_transition_time_t remain_time = light_xyl_pa.remain_time;
	bool trans_flag = light_xyl_pa.transition_flag;
	if (trans_flag && remain_time.num_steps < light_xyl_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	k_mutex_unlock(&mutex_xyl);
	light_xyl_stat(model, ctx, xyl_store.xyl_lightness, xyl_store.xyl_x, xyl_store.xyl_y,
				   trans_flag, remain_time);
	return 0;
}

static int light_xyl_srv_set_unack(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	(void)model;
	uint16_t lightness = net_buf_simple_pull_le16(buf);
	uint16_t xyl_x = net_buf_simple_pull_le16(buf);
	uint16_t xyl_y = net_buf_simple_pull_le16(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	/*****Get range*****/
	rtk_bt_mesh_light_xyl_server_get_range_t range;
	rtk_bt_mesh_light_xyl_server_direct_get_range_t *xyl_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_xyl_server_direct_get_range_t));
	xyl_range = (rtk_bt_mesh_light_xyl_server_direct_get_range_t *)p_evt->data;
	xyl_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	if ((0 != range.xyl_x_range_min) && (0 != range.xyl_x_range_max)) {
		xyl_x = CLAMP(xyl_x, range.xyl_x_range_min, range.xyl_x_range_max);
	}

	if ((0 != range.xyl_y_range_min) && (0 != range.xyl_y_range_max)) {
		xyl_y = CLAMP(xyl_y, range.xyl_y_range_min, range.xyl_y_range_max);
	}

	int32_t delay = 0;
	k_mutex_lock(&mutex_xyl, K_FOREVER);
	if (tid == light_xyl_pa.tid && ctx->addr == light_xyl_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_xyl);
		return 0;
	}
	if (lightness == light_xyl_pa.target_lightness && xyl_x == light_xyl_pa.target_xyl_x && xyl_y == light_xyl_pa.target_xyl_y) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_xyl);
		return 0;
	}

	light_xyl_pa.lightness = lightness;
	light_xyl_pa.target_lightness = lightness;
	light_xyl_pa.xyl_x = xyl_x;
	light_xyl_pa.target_xyl_x = xyl_x;
	light_xyl_pa.xyl_y = xyl_y;
	light_xyl_pa.target_xyl_y = xyl_y;
	light_xyl_pa.src = ctx->addr;
	light_xyl_pa.tid = tid;

	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_xyl_pa.total_time.num_steps = trans_time & 0x3F;
		light_xyl_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_xyl_pa.remain_time = light_xyl_pa.total_time;
		light_xyl_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_xyl_pa.total_time.num_steps = 0;
		light_xyl_pa.total_time.step_resolution = 0;
		light_xyl_pa.remain_time = light_xyl_pa.total_time;
		light_xyl_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		rtk_bt_mesh_light_xyl_server_set_t lightness_set;
		lightness_set.xyl.xyl_lightness = lightness;
		lightness_set.xyl.xyl_x = xyl_x;
		lightness_set.xyl.xyl_y = xyl_y;
		lightness_set.total_time = light_xyl_pa.total_time;
		lightness_set.remaining_time = light_xyl_pa.remain_time;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_xyl_server_set_t));
		memcpy(p_evt->data, &lightness_set, sizeof(rtk_bt_mesh_light_xyl_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		k_mutex_unlock(&mutex_xyl);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&light_xyl_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_xyl);
	return 0;

}

static int light_xyl_srv_set(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	light_xyl_srv_set_unack(model, ctx, buf);
	light_xyl_srv_get(model, ctx, buf);
	return 0;
}

static int light_xyl_srv_target_get(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)buf;
	k_mutex_lock(&mutex_xyl, K_FOREVER);
	uint16_t msg_len = 6;
	if (light_xyl_pa.transition_flag) {
		msg_len = 7;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_XYL_TARGET_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_XYL_TARGET_STATUS);
	net_buf_simple_add_le16(&pmsg, light_xyl_pa.target_lightness);
	net_buf_simple_add_le16(&pmsg, light_xyl_pa.target_xyl_x);
	net_buf_simple_add_le16(&pmsg, light_xyl_pa.target_xyl_y);
	if (light_xyl_pa.transition_flag) {
		rtk_bt_mesh_generic_transition_time_t remain_time = light_xyl_pa.remain_time;
		if (remain_time.num_steps < light_xyl_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		net_buf_simple_add_mem(&pmsg, &remain_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	k_mutex_unlock(&mutex_xyl);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_xyl_srv_default_get(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_xyl_server_get_default_t get_default;
	rtk_bt_mesh_light_xyl_server_direct_get_default_t *xyl_default;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_xyl_server_direct_get_default_t));
	xyl_default = (rtk_bt_mesh_light_xyl_server_direct_get_default_t *)p_evt->data;
	xyl_default->value = &get_default;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_XYL_DEFAULT_STATUS, 6);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_XYL_DEFAULT_STATUS);
	net_buf_simple_add_le16(&pmsg, get_default.xyl_lightness);
	net_buf_simple_add_le16(&pmsg, get_default.xyl_x);
	net_buf_simple_add_le16(&pmsg, get_default.xyl_y);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_xyl_srv_range_get(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_xyl_server_get_range_t range;
	rtk_bt_mesh_light_xyl_server_direct_get_range_t *xyl_range;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_light_xyl_server_direct_get_range_t));
	xyl_range = (rtk_bt_mesh_light_xyl_server_direct_get_range_t *)p_evt->data;
	xyl_range->value = &range;
	rtk_bt_evt_indicate(p_evt, NULL);

	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_XYL_RANGE_STATUS, 9);
	bt_mesh_model_msg_init(&pmsg, OP_LIGHT_XYL_RANGE_STATUS);
	net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
	net_buf_simple_add_le16(&pmsg, range.xyl_x_range_min);
	net_buf_simple_add_le16(&pmsg, range.xyl_x_range_max);
	net_buf_simple_add_le16(&pmsg, range.xyl_y_range_min);
	net_buf_simple_add_le16(&pmsg, range.xyl_y_range_max);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int light_xyl_srv_init(const struct bt_mesh_model *model)
{
	light_xyl_server_model = model;
	k_work_init_delayable(&light_xyl_pa.work, light_xyl_timeout);
	k_mutex_init(&mutex_xyl);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_xyl_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_xyl);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_xyl_srv_cb = {
	.init = light_xyl_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_xyl_srv_deinit,
#endif
};

const struct bt_mesh_model_op light_xyl_srv_op[] = {
	{ OP_LIGHT_XYL_GET,               BT_MESH_LEN_EXACT(0),   light_xyl_srv_get },
	{ OP_LIGHT_XYL_SET,               BT_MESH_LEN_MIN(7),     light_xyl_srv_set },
	{ OP_LIGHT_XYL_SET_UNACK,         BT_MESH_LEN_MIN(7),     light_xyl_srv_set_unack },
	{ OP_LIGHT_XYL_TARGET_GET,        BT_MESH_LEN_EXACT(0),   light_xyl_srv_target_get },
	{ OP_LIGHT_XYL_DEFAULT_GET,       BT_MESH_LEN_EXACT(0),   light_xyl_srv_default_get },
	{ OP_LIGHT_XYL_RANGE_GET,         BT_MESH_LEN_EXACT(0),   light_xyl_srv_range_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_SETUP_SERVER_MODEL
static const struct bt_mesh_model *light_xyl_setup_server_model;

static rtk_bt_mesh_light_xyl_server_set_default_t xyl_default;
static int light_xyl_setup_srv_default_set_unack(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	xyl_default.xyl_lightness = net_buf_simple_pull_le16(buf);
	xyl_default.xyl_x = net_buf_simple_pull_le16(buf);
	xyl_default.xyl_y = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_DEFAULT_SERVER_MODEL_SET,
								sizeof(rtk_bt_mesh_light_xyl_server_set_default_t));
	memcpy(p_evt->data, &xyl_default, sizeof(rtk_bt_mesh_light_xyl_server_set_default_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_xyl_setup_srv_default_set(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	if (!light_xyl_setup_srv_default_set_unack(model, ctx, buf)) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_XYL_DEFAULT_STATUS, 6);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_XYL_DEFAULT_STATUS);
		net_buf_simple_add_le16(&pmsg, xyl_default.xyl_lightness);
		net_buf_simple_add_le16(&pmsg, xyl_default.xyl_x);
		net_buf_simple_add_le16(&pmsg, xyl_default.xyl_y);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static rtk_bt_mesh_light_xyl_server_set_range_t xyl_range;
static int light_xyl_setup_srv_range_set_unack(const struct bt_mesh_model *model,
											   struct bt_mesh_msg_ctx *ctx,
											   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	xyl_range.xyl_x_range_min = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_x_range_max = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_y_range_min = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_y_range_max = net_buf_simple_pull_le16(buf);
	if ((xyl_range.xyl_x_range_min <= xyl_range.xyl_x_range_max) &&
		(xyl_range.xyl_y_range_min <= xyl_range.xyl_y_range_max)) {
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_SERVER_MODEL_SET,
									sizeof(rtk_bt_mesh_light_xyl_server_set_range_t));
		memcpy(p_evt->data, &xyl_range, sizeof(rtk_bt_mesh_light_xyl_server_set_range_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int light_xyl_setup_srv_range_set(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	if (!light_xyl_setup_srv_range_set_unack(model, ctx, buf)) {
		BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_LIGHT_XYL_RANGE_STATUS, 9);
		bt_mesh_model_msg_init(&pmsg, OP_LIGHT_XYL_RANGE_STATUS);
		net_buf_simple_add_u8(&pmsg, MESH_GENERIC_STAT_SUCCESS);
		net_buf_simple_add_le16(&pmsg, xyl_range.xyl_x_range_min);
		net_buf_simple_add_le16(&pmsg, xyl_range.xyl_x_range_max);
		net_buf_simple_add_le16(&pmsg, xyl_range.xyl_y_range_min);
		net_buf_simple_add_le16(&pmsg, xyl_range.xyl_y_range_max);
		return bt_mesh_msg_send(model, ctx, &pmsg);
	}
	return -1;
}

static int light_xyl_setup_srv_init(const struct bt_mesh_model *model)
{
	light_xyl_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_xyl_setup_srv_cb = {
	.init = light_xyl_setup_srv_init,
};

const struct bt_mesh_model_op light_xyl_setup_srv_op[] = {
	{ OP_LIGHT_XYL_DEFAULT_SET,           BT_MESH_LEN_EXACT(6),    light_xyl_setup_srv_default_set },
	{ OP_LIGHT_XYL_DEFAULT_SET_UNACK,     BT_MESH_LEN_EXACT(6),    light_xyl_setup_srv_default_set_unack },
	{ OP_LIGHT_XYL_RANGE_SET,             BT_MESH_LEN_EXACT(8),    light_xyl_setup_srv_range_set },
	{ OP_LIGHT_XYL_RANGE_SET_UNACK,       BT_MESH_LEN_EXACT(8),    light_xyl_setup_srv_range_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_XYL_CLIENT_MODEL
static const struct bt_mesh_model *light_xyl_client_model = NULL;

static int light_xyl_cli_stat(const struct bt_mesh_model *model,
							  struct bt_mesh_msg_ctx *ctx,
							  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_xyl_client_status_t xyl_stat;
	xyl_stat.src = ctx->addr;
	xyl_stat.xyl.xyl_lightness = net_buf_simple_pull_le16(buf);
	xyl_stat.xyl.xyl_x = net_buf_simple_pull_le16(buf);
	xyl_stat.xyl.xyl_y = net_buf_simple_pull_le16(buf);
	xyl_stat.optional = FALSE;
	if (buf->len) {
		xyl_stat.optional = TRUE;
		xyl_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_xyl_client_status_t));
	memcpy(p_evt->data, &xyl_stat, sizeof(rtk_bt_mesh_light_xyl_client_status_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_xyl_cli_target_stat(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_xyl_client_status_t xyl_stat;
	xyl_stat.src = ctx->addr;
	xyl_stat.xyl.xyl_lightness = net_buf_simple_pull_le16(buf);
	xyl_stat.xyl.xyl_x = net_buf_simple_pull_le16(buf);
	xyl_stat.xyl.xyl_y = net_buf_simple_pull_le16(buf);
	xyl_stat.optional = FALSE;
	if (buf->len) {
		xyl_stat.optional = TRUE;
		xyl_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_xyl_client_status_t));
	memcpy(p_evt->data, &xyl_stat, sizeof(rtk_bt_mesh_light_xyl_client_status_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_xyl_cli_default_stat(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_xyl_client_status_default_t default_xyl;
	default_xyl.src = ctx->addr;
	default_xyl.xyl.xyl_lightness = net_buf_simple_pull_le16(buf);
	default_xyl.xyl.xyl_x = net_buf_simple_pull_le16(buf);
	default_xyl.xyl.xyl_y = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_xyl_client_status_default_t));
	memcpy(p_evt->data, &default_xyl, sizeof(rtk_bt_mesh_light_xyl_client_status_default_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int light_xyl_cli_range_stat(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_xyl_client_status_range_t xyl_range;
	xyl_range.src = ctx->addr;
	xyl_range.status = net_buf_simple_pull_u8(buf);
	xyl_range.xyl_x_range_min = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_x_range_max = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_y_range_min = net_buf_simple_pull_le16(buf);
	xyl_range.xyl_y_range_max = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_XYL_CLIENT_MODEL, RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_light_xyl_client_status_range_t));
	memcpy(p_evt->data, &xyl_range, sizeof(rtk_bt_mesh_light_xyl_client_status_range_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_model_op light_xyl_cli_op[] = {
	{ OP_LIGHT_XYL_STATUS,             BT_MESH_LEN_MIN(6),    light_xyl_cli_stat },
	{ OP_LIGHT_XYL_TARGET_STATUS,      BT_MESH_LEN_MIN(6),    light_xyl_cli_target_stat },
	{ OP_LIGHT_XYL_DEFAULT_STATUS,     BT_MESH_LEN_EXACT(6),  light_xyl_cli_default_stat },
	{ OP_LIGHT_XYL_RANGE_STATUS,       BT_MESH_LEN_EXACT(9),  light_xyl_cli_range_stat },
	BT_MESH_MODEL_OP_END,
};

static int light_xyl_cli_init(const struct bt_mesh_model *model)
{
	light_xyl_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_xyl_cli_cb = {
	.init = light_xyl_cli_init,
};

static int light_xyl_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_XYL_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, rtk_bt_mesh_light_xyl_t xyl, uint8_t tid,
						 bool optional, rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 7;
	if (optional) {
		msg_len = 9;
	}
	uint32_t op = ack ? OP_LIGHT_XYL_SET : OP_LIGHT_XYL_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, xyl.xyl_lightness);
	net_buf_simple_add_le16(&buf, xyl.xyl_x);
	net_buf_simple_add_le16(&buf, xyl.xyl_y);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_target_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_TARGET_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_XYL_TARGET_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_default_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_DEFAULT_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_XYL_DEFAULT_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_default_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
								 rtk_bt_mesh_light_xyl_t xyl, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_XYL_DEFAULT_SET : OP_LIGHT_XYL_DEFAULT_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_DEFAULT_SET, 6);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, xyl.xyl_lightness);
	net_buf_simple_add_le16(&buf, xyl.xyl_x);
	net_buf_simple_add_le16(&buf, xyl.xyl_y);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_range_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_RANGE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_XYL_RANGE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_xyl_range_set(const struct bt_mesh_model *model, uint16_t dst,
							   uint16_t app_key_index, uint16_t xyl_x_range_min, uint16_t xyl_x_range_max,
							   uint16_t xyl_y_range_min, uint16_t xyl_y_range_max, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_XYL_RANGE_SET : OP_LIGHT_XYL_RANGE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_XYL_RANGE_SET, 8);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, xyl_x_range_min);
	net_buf_simple_add_le16(&buf, xyl_x_range_max);
	net_buf_simple_add_le16(&buf, xyl_y_range_min);
	net_buf_simple_add_le16(&buf, xyl_y_range_max);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_light_xyl_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_xyl_get_t *xyl_get = (rtk_bt_mesh_light_xyl_get_t *)p_cmd->param;
		ret = light_xyl_get(light_xyl_client_model, xyl_get->dst, xyl_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_xyl_set_t *xyl_set = (rtk_bt_mesh_light_xyl_set_t *)p_cmd->param;
		ret = light_xyl_set(light_xyl_client_model, xyl_set->dst, xyl_set->app_key_index, xyl_set->xyl,
							xyl_set->tid, xyl_set->optional, xyl_set->trans_time, xyl_set->delay, xyl_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_TARGET_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_xyl_target_get_t *xyl_target_set = (rtk_bt_mesh_light_xyl_target_get_t *)p_cmd->param;
		ret = light_xyl_target_get(light_xyl_client_model, xyl_target_set->dst, xyl_target_set->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_xyl_default_get_t *xyl_default_get = (rtk_bt_mesh_light_xyl_default_get_t *)p_cmd->param;
		ret = light_xyl_default_get(light_xyl_client_model, xyl_default_get->dst, xyl_default_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_DEFAULT_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_xyl_default_set_t *xyl_default_set = (rtk_bt_mesh_light_xyl_default_set_t *)p_cmd->param;
		ret = light_xyl_default_set(light_xyl_client_model, xyl_default_set->dst, xyl_default_set->app_key_index,
									xyl_default_set->xyl, xyl_default_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_xyl_range_get_t *xyl_range_get = (rtk_bt_mesh_light_xyl_range_get_t *)p_cmd->param;
		ret = light_xyl_range_get(light_xyl_client_model, xyl_range_get->dst, xyl_range_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_XYL_RANGE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_xyl_range_set_t *xyl_range_set = (rtk_bt_mesh_light_xyl_range_set_t *)p_cmd->param;
		ret = light_xyl_range_set(light_xyl_client_model, xyl_range_set->dst, xyl_range_set->app_key_index,
								  xyl_range_set->xyl_x_range_min, xyl_range_set->xyl_x_range_max,
								  xyl_range_set->xyl_y_range_min, xyl_range_set->xyl_y_range_max, xyl_range_set->ack);
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

#define OP_LIGHT_LC_MODE_GET                BT_MESH_MODEL_OP_2(0x82, 0x91)
#define OP_LIGHT_LC_MODE_SET                BT_MESH_MODEL_OP_2(0x82, 0x92)
#define OP_LIGHT_LC_MODE_SET_UNACK          BT_MESH_MODEL_OP_2(0x82, 0x93)
#define OP_LIGHT_LC_MODE_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x94)
#define OP_LIGHT_LC_OM_GET                  BT_MESH_MODEL_OP_2(0x82, 0x95)
#define OP_LIGHT_LC_OM_SET                  BT_MESH_MODEL_OP_2(0x82, 0x96)
#define OP_LIGHT_LC_OM_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x97)
#define OP_LIGHT_LC_OM_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x98)
#define OP_LIGHT_LC_LIGHT_ONOFF_GET         BT_MESH_MODEL_OP_2(0x82, 0x99)
#define OP_LIGHT_LC_LIGHT_ONOFF_SET         BT_MESH_MODEL_OP_2(0x82, 0x9A)
#define OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK   BT_MESH_MODEL_OP_2(0x82, 0x9B)
#define OP_LIGHT_LC_LIGHT_ONOFF_STATUS      BT_MESH_MODEL_OP_2(0x82, 0x9C)

#define OP_LIGHT_LC_PROPERTY_GET            BT_MESH_MODEL_OP_2(0x82, 0x9D)
#define OP_LIGHT_LC_PROPERTY_SET            BT_MESH_MODEL_OP_1(0x62)
#define OP_LIGHT_LC_PROPERTY_SET_UNACK      BT_MESH_MODEL_OP_1(0x63)
#define OP_LIGHT_LC_PROPERTY_STATUS         BT_MESH_MODEL_OP_1(0x64)

#if defined(BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SERVER_MODEL
struct lightness_lc_t {
	rtk_bt_mesh_generic_on_off_t on_off;
	rtk_bt_mesh_generic_on_off_t target_on_off;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} light_lc_pa;

struct k_mutex mutex_lc;
static const struct bt_mesh_model *light_lc_server_model = NULL;

static int indicate_light_onoff_to_app(void)
{
	rtk_bt_mesh_light_lc_server_set_light_on_off_t lc_onoff;
	lc_onoff.light_on_off = light_lc_pa.target_on_off;
	lc_onoff.total_time = light_lc_pa.total_time;
	lc_onoff.remaining_time = light_lc_pa.remain_time;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_SET,
								sizeof(rtk_bt_mesh_light_lc_server_set_light_on_off_t));
	memcpy(p_evt->data, &lc_onoff, sizeof(rtk_bt_mesh_light_lc_server_set_light_on_off_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

void light_lc_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_lc, K_FOREVER);
	if (!light_lc_pa.transition_flag) {
		light_lc_pa.remain_time.num_steps = 0;
		light_lc_pa.total_time.num_steps = 0;
	}

	int32_t trans = time_res[light_lc_pa.remain_time.step_resolution];
	if (light_lc_pa.transition_flag) {
		if (light_lc_pa.target_on_off != light_lc_pa.on_off &&
			light_lc_pa.target_on_off == RTK_BT_MESH_GENERIC_ON &&
			light_lc_pa.remain_time.num_steps == light_lc_pa.total_time.num_steps) {
			light_lc_pa.on_off = light_lc_pa.target_on_off;
			indicate_light_onoff_to_app();
		}
	}

	if (light_lc_pa.remain_time.num_steps) {
		light_lc_pa.remain_time.num_steps--;
		k_work_reschedule(&light_lc_pa.work, K_MSEC(trans));
	} else {
		if (light_lc_pa.target_on_off != light_lc_pa.on_off &&
			light_lc_pa.target_on_off == RTK_BT_MESH_GENERIC_OFF) {
			light_lc_pa.on_off = light_lc_pa.target_on_off;
			indicate_light_onoff_to_app();
		}
		light_lc_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_lc);
	return;
}

static int light_lc_srv_init(const struct bt_mesh_model *model)
{
	light_lc_server_model = model;
	k_work_init_delayable(&light_lc_pa.work, light_lc_timeout);
	k_mutex_init(&mutex_lc);
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int light_lc_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_lc);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_light_lc_srv_cb = {
	.init = light_lc_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = light_lc_srv_deinit,
#endif
};

static int light_lc_mode_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint8_t mode)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_MODE_STATUS, 1);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_MODE_STATUS);
	net_buf_simple_add_u8(&buf, mode);
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int light_lc_om_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint8_t mode)
{
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_OM_STATUS, 1);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_OM_STATUS);
	net_buf_simple_add_u8(&buf, mode);
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int light_lc_light_on_off_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_generic_on_off_t present, bool optional,
										rtk_bt_mesh_generic_on_off_t target, rtk_bt_mesh_generic_transition_time_t trans_time)
{
	uint16_t msg_len = 1;
	if (optional) {
		msg_len = 3;
	}
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_LIGHT_ONOFF_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
	net_buf_simple_add_u8(&buf, present);
	if (optional) {
		net_buf_simple_add_u8(&buf, target);
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int light_lc_srv_mode_get(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lc_server_get_mode_t mode_store;
	rtk_bt_mesh_light_lc_server_direct_get_mode_t *mode_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_GET,
								sizeof(rtk_bt_mesh_light_lc_server_direct_get_mode_t));
	mode_get = (rtk_bt_mesh_light_lc_server_direct_get_mode_t *)p_evt->data;
	mode_get->value = &mode_store;
	rtk_bt_evt_indicate(p_evt, NULL);
	return light_lc_mode_status(model, ctx, mode_store.mode);
}

static int light_lc_srv_mode_set_unack(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		uint8_t mode = net_buf_simple_pull_u8(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_MODE_SET,
									sizeof(rtk_bt_mesh_light_lc_server_set_mode_t));
		memcpy(p_evt->data, &mode, sizeof(rtk_bt_mesh_light_lc_server_set_mode_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		if (mode == 0) {
			k_mutex_lock(&mutex_lc, K_FOREVER);
			light_lc_pa.transition_flag = FALSE;
			/*****set light on off*****/
			light_lc_pa.total_time.num_steps = 0;
			light_lc_pa.total_time.step_resolution = 0;
			light_lc_pa.remain_time = light_lc_pa.total_time;
			indicate_light_onoff_to_app();
			k_mutex_unlock(&mutex_lc);
		}
		return 0;
	}
	return -1;
}

static int light_lc_srv_mode_set(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	if (!light_lc_srv_mode_set_unack(model, ctx, buf)) {
		return light_lc_srv_mode_get(model, ctx, buf);
	}
	return -1;
}

static int light_lc_srv_om_get(const struct bt_mesh_model *model,
							   struct bt_mesh_msg_ctx *ctx,
							   struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lc_server_get_om_t om_val;
	rtk_bt_mesh_light_lc_server_direct_get_om_t *om_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_GET,
								sizeof(rtk_bt_mesh_light_lc_server_direct_get_om_t));
	om_get = (rtk_bt_mesh_light_lc_server_direct_get_om_t *)p_evt->data;
	om_get->value = &om_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	return light_lc_om_status(model, ctx, om_val.mode);
}

static int light_lc_srv_om_set_unack(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		uint8_t mode = net_buf_simple_pull_u8(buf);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_OM_SET,
									sizeof(rtk_bt_mesh_light_lc_server_set_om_t));
		memcpy(p_evt->data, &mode, sizeof(rtk_bt_mesh_light_lc_server_set_om_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		return 0;
	}
	return -1;
}

static int light_lc_srv_om_set(const struct bt_mesh_model *model,
							   struct bt_mesh_msg_ctx *ctx,
							   struct net_buf_simple *buf)
{
	if (!light_lc_srv_om_set_unack(model, ctx, buf)) {
		return light_lc_srv_om_get(model, ctx, buf);
	}
	return -1;
}

static int light_lc_srv_light_onoff_get(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)buf;
	rtk_bt_mesh_light_lc_server_get_light_on_off_t onoff_val;
	rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t *onoff_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_LIGHT_ON_OFF_GET,
								sizeof(rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t));
	onoff_get = (rtk_bt_mesh_light_lc_server_direct_get_light_on_off_t *)p_evt->data;
	onoff_get->value = &onoff_val;
	rtk_bt_evt_indicate(p_evt, NULL);

	k_mutex_lock(&mutex_lc, K_FOREVER);
	bool trans_flag = light_lc_pa.transition_flag;
	rtk_bt_mesh_generic_on_off_t target_on_off = light_lc_pa.target_on_off;
	rtk_bt_mesh_generic_transition_time_t remain_time = light_lc_pa.remain_time;
	k_mutex_unlock(&mutex_lc);
	light_lc_light_on_off_status(model, ctx, onoff_val.on_off, trans_flag, target_on_off, remain_time);
	return 0;
}

static int light_lc_srv_light_onoff_set_unack(const struct bt_mesh_model *model,
											  struct bt_mesh_msg_ctx *ctx,
											  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_generic_on_off_t onoff_val = net_buf_simple_pull_u8(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);

	k_mutex_lock(&mutex_lc, K_FOREVER);
	if (tid == light_lc_pa.tid && ctx->addr == light_lc_pa.src) {
		/* Duplicate */
		BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		k_mutex_unlock(&mutex_lc);
		return 0;
	}
	if (onoff_val == light_lc_pa.target_on_off) {
		BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		k_mutex_unlock(&mutex_lc);
		return 0;
	}

	// light_lc_pa.on_off = onoff_val;
	light_lc_pa.target_on_off = onoff_val;
	light_lc_pa.src = ctx->addr;
	light_lc_pa.tid = tid;
	int32_t delay = 0;
	if (buf->len) {
		uint8_t trans_time = net_buf_simple_pull_u8(buf);
		light_lc_pa.total_time.num_steps = trans_time & 0x3F;
		light_lc_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
		light_lc_pa.remain_time = light_lc_pa.total_time;
		light_lc_pa.transition_flag = TRUE;
		delay = net_buf_simple_pull_u8(buf) * 5;
	} else {
		light_lc_pa.on_off = onoff_val;
		light_lc_pa.total_time.num_steps = 0;
		light_lc_pa.total_time.step_resolution = 0;
		light_lc_pa.remain_time = light_lc_pa.total_time;
		light_lc_pa.transition_flag = FALSE;

		/*****if no transition, call set here directly, then return******/
		indicate_light_onoff_to_app();
		k_mutex_unlock(&mutex_lc);
		return 0;
	}

	/*****Need add timeout handler*****/
	k_work_reschedule(&light_lc_pa.work, K_MSEC(delay));
	k_mutex_unlock(&mutex_lc);
	return 0;
}

static int light_lc_srv_light_onoff_set(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	if (!light_lc_srv_light_onoff_set_unack(model, ctx, buf)) {
		return light_lc_srv_light_onoff_get(model, ctx, buf);
	}
	return -1;
}


const struct bt_mesh_model_op light_lc_srv_op[] = {
	{ OP_LIGHT_LC_MODE_GET,               BT_MESH_LEN_EXACT(0),     light_lc_srv_mode_get },
	{ OP_LIGHT_LC_MODE_SET,               BT_MESH_LEN_EXACT(1),     light_lc_srv_mode_set },
	{ OP_LIGHT_LC_MODE_SET_UNACK,         BT_MESH_LEN_EXACT(1),     light_lc_srv_mode_set_unack },
	{ OP_LIGHT_LC_OM_GET,                 BT_MESH_LEN_EXACT(0),     light_lc_srv_om_get },
	{ OP_LIGHT_LC_OM_SET,                 BT_MESH_LEN_EXACT(1),     light_lc_srv_om_set },
	{ OP_LIGHT_LC_OM_SET_UNACK,           BT_MESH_LEN_EXACT(1),     light_lc_srv_om_set_unack },
	{ OP_LIGHT_LC_LIGHT_ONOFF_GET,        BT_MESH_LEN_EXACT(0),     light_lc_srv_light_onoff_get },
	{ OP_LIGHT_LC_LIGHT_ONOFF_SET,        BT_MESH_LEN_MIN(2),       light_lc_srv_light_onoff_set },
	{ OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK,  BT_MESH_LEN_MIN(2),       light_lc_srv_light_onoff_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_LIGHT_LC_SETUP_SERVER_MODEL
#define MODEL_PROPERTY_INVALID                                    0x00

#define MODEL_PROPERTY_LIGHT_CONTORL_AMBIENT_LUXLEVEL_ON          0x002B
#define MODEL_PROPERTY_LIGHT_CONTORL_AMBIENT_LUXLEVEL_PROLONG     0x002C
#define MODEL_PROPERTY_LIGHT_CONTORL_AMBIENT_LUXLEVEL_STANDBY     0x002D
#define MODEL_PROPERTY_LIGHT_CONTORL_LIGHTNESS_ON                 0x002E
#define MODEL_PROPERTY_LIGHT_CONTORL_LIGHTNESS_PROLONG            0x002F
#define MODEL_PROPERTY_LIGHT_CONTORL_LIGHTNESS_STANDBY            0x0030
#define MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_ACCURACY           0x0031
#define MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KID                0x0032
#define MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KIU                0x0033
#define MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KPD                0x0034
#define MODEL_PROPERTY_LIGHT_CONTORL_REGULATOR_KPU                0x0035
#define MODEL_PROPERTY_LIGHT_CONTORL_TIME_FADE                    0x0036
#define MODEL_PROPERTY_LIGHT_CONTROL_TIME_FADE_ON                 0x0037
#define MODEL_PROPERTY_LIGHT_CONTORL_TIME_FADE_STANDBY_AUDIO      0x0038
#define MODEL_PROPERTY_LIGHT_CONTORL_TIME_FADE_STANDBY_MANUAL     0x0039
#define MODEL_PROPERTY_LIGHT_CONTROL_TIME_OCCUPANCY_DELAY         0x003A
#define MODEL_PROPERTY_LIGHT_CONTORL_TIME_PROLONG                 0x003B
#define MODEL_PROPERTY_LIGHT_CONTROL_TIME_RUN_ON                  0x003C
#define MODEL_PROPERTY_MOTION_SENSED                              0x0042
#define MODEL_PROPERTY_PEOPLE_COUNT                               0x004C
#define MODEL_PROPERTY_PRESENCE_DETECTED                          0x004D
#define MODEL_PROPERTY_PRESENT_AMBIENT_LIGHT_LEVEL                0x004E
#define MODEL_PROPERTY_TIME_SINCE_MOTION_SENSED                   0x0068

static const struct bt_mesh_model *light_lc_setup_server_model = NULL;

static int light_lc_setup_srv_init(const struct bt_mesh_model *model)
{
	light_lc_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_lc_setup_srv_cb = {
	.init = light_lc_setup_srv_init,
};

bool light_lc_property_value_check(uint16_t property_id, const uint8_t *value, uint16_t value_len)
{
	(void)value;
	if (MODEL_PROPERTY_INVALID == property_id) {
		return FALSE;
	}

	uint16_t wanted_len = 0;
	if ((property_id >= 0x2E) && (property_id <= 0x30)) {
		wanted_len = 2;
	} else if ((property_id >= 0x36) && (property_id <= 0x3c)) {
		wanted_len = 3;
	}
	if (value_len != wanted_len) {
		return FALSE;
	}

	return TRUE;
}

static int light_lc_property_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t property_id, const uint8_t *pproperty_value,
									uint16_t len)
{
	uint16_t msg_len = len + 2;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_PROPERTY_STATUS, msg_len);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_PROPERTY_STATUS);
	net_buf_simple_add_le16(&buf, property_id);
	net_buf_simple_add_mem(&buf, pproperty_value, len);
	return bt_mesh_msg_send(model, ctx, &buf);
}

static int light_lc_setup_srv_property_get(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		if (MODEL_PROPERTY_INVALID != property_id) {
			rtk_bt_mesh_light_lc_server_direct_get_property_para_t property_val;
			rtk_bt_mesh_light_lc_server_direct_get_property_t *property_get;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_GET,
										sizeof(rtk_bt_mesh_light_lc_server_direct_get_property_t));
			property_get = (rtk_bt_mesh_light_lc_server_direct_get_property_t *)p_evt->data;
			property_get->value = &property_val;
			property_get->property_id = property_id;
			rtk_bt_evt_indicate(p_evt, NULL);

			return light_lc_property_status(model, ctx, property_id, (uint8_t *)&property_val.property_value, property_val.value_len);
		}
	}
	return -1;
}

static int light_lc_setup_srv_property_set_unack(const struct bt_mesh_model *model,
												 struct bt_mesh_msg_ctx *ctx,
												 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		if (light_lc_property_value_check(property_id, buf->data, buf->len)) {
			rtk_bt_mesh_light_lc_server_set_property_t set_data;
			set_data.property_id = property_id;
			set_data.property_value = 0;
			for (uint8_t i = buf->len; i > 0; --i) {
				set_data.property_value <<= 8;
				set_data.property_value |= buf->data[i - 1];
			}
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_SET,
										sizeof(rtk_bt_mesh_light_lc_server_set_property_t));
			memcpy(p_evt->data, &set_data, sizeof(rtk_bt_mesh_light_lc_server_set_property_t));
			return rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	return -1;
}

static int light_lc_setup_srv_property_set(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t property_id = LE_TO_U16(buf->data);
		if (!light_lc_setup_srv_property_set_unack(model, ctx, buf)) {
			rtk_bt_mesh_light_lc_server_direct_get_property_para_t property_val;
			rtk_bt_mesh_light_lc_server_direct_get_property_t *property_get;
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_SETUP_SERVER_MODEL, RTK_BT_MESH_LIGHT_LC_SERVER_MODEL_PROPERTY_GET,
										sizeof(rtk_bt_mesh_light_lc_server_direct_get_property_t));
			property_get = (rtk_bt_mesh_light_lc_server_direct_get_property_t *)p_evt->data;
			property_get->value = &property_val;
			property_get->property_id = property_id;
			rtk_bt_evt_indicate(p_evt, NULL);

			return light_lc_property_status(model, ctx, property_id, (uint8_t *)&property_val.property_value, property_val.value_len);
		}
	}
	return -1;
}

const struct bt_mesh_model_op light_lc_setup_srv_op[] = {
	{ OP_LIGHT_LC_PROPERTY_GET,               BT_MESH_LEN_EXACT(2),     light_lc_setup_srv_property_get },
	{ OP_LIGHT_LC_PROPERTY_SET,               BT_MESH_LEN_MIN(2),       light_lc_setup_srv_property_set },
	{ OP_LIGHT_LC_PROPERTY_SET_UNACK,         BT_MESH_LEN_MIN(2),       light_lc_setup_srv_property_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL) && BT_MESH_ENABLE_LIGHT_LC_CLIENT_MODEL
static const struct bt_mesh_model *light_lc_client_model = NULL;

static int light_lc_cli_init(const struct bt_mesh_model *model)
{
	light_lc_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_light_lc_cli_cb = {
	.init = light_lc_cli_init,
};

static int light_lc_cli_mode_status(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lc_client_mode_status_t mode_status;
	if (buf->len) {
		mode_status.mode = net_buf_simple_pull_u8(buf);
		mode_status.src = ctx->addr;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_light_lc_client_mode_status_t));
		memcpy(p_evt->data, &mode_status, sizeof(rtk_bt_mesh_light_lc_client_mode_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int light_lc_cli_om_status(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lc_client_om_status_t mode_status;
	if (buf->len) {
		mode_status.mode = net_buf_simple_pull_u8(buf);
		mode_status.src = ctx->addr;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_light_lc_client_om_status_t));
		memcpy(p_evt->data, &mode_status, sizeof(rtk_bt_mesh_light_lc_client_om_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int light_lc_cli_light_onoff_status(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_light_lc_client_light_on_off_status_t light_onoff_stat;
	if (buf->len) {
		light_onoff_stat.src = ctx->addr;
		light_onoff_stat.present_on_off = net_buf_simple_pull_u8(buf);
		light_onoff_stat.optional = FALSE;
		if (buf->len) {
			light_onoff_stat.optional = TRUE;
			light_onoff_stat.target_on_off = net_buf_simple_pull_u8(buf);
			light_onoff_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_light_lc_client_light_on_off_status_t));
		memcpy(p_evt->data, &light_onoff_stat, sizeof(rtk_bt_mesh_light_lc_client_light_on_off_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int light_lc_cli_property_status(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		uint16_t property_id = net_buf_simple_pull_le16(buf);
		rtk_bt_mesh_light_lc_client_property_status_t status_data;
		status_data.src = ctx->addr;
		status_data.property_id = property_id;
		status_data.property_value = 0;
		for (uint8_t i = buf->len; i > 0; --i) {
			status_data.property_value <<= 8;
			status_data.property_value |= buf->data[i - 1];
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_LIGHT_LC_CLIENT_MODEL, RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_light_lc_client_property_status_t));
		memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_light_lc_client_property_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op light_lc_cli_op[] = {
	{ OP_LIGHT_LC_MODE_STATUS,               BT_MESH_LEN_EXACT(1),     light_lc_cli_mode_status },
	{ OP_LIGHT_LC_OM_STATUS,                 BT_MESH_LEN_EXACT(1),     light_lc_cli_om_status },
	{ OP_LIGHT_LC_LIGHT_ONOFF_STATUS,        BT_MESH_LEN_MIN(1),       light_lc_cli_light_onoff_status },
	{ OP_LIGHT_LC_PROPERTY_STATUS,           BT_MESH_LEN_MIN(2),       light_lc_cli_property_status },
	BT_MESH_MODEL_OP_END,
};

static int light_lc_mode_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_MODE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_MODE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_mode_set(const struct bt_mesh_model *model, uint16_t dst,
							 uint16_t app_key_index, uint8_t mode, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_LC_MODE_SET : OP_LIGHT_LC_MODE_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_MODE_SET, 1);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_u8(&buf, mode);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_om_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_OM_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_OM_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_om_set(const struct bt_mesh_model *model, uint16_t dst,
						   uint16_t app_key_index, uint8_t mode, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_LC_OM_SET : OP_LIGHT_LC_OM_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_OM_SET, 1);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_u8(&buf, mode);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_light_on_off_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_LIGHT_ONOFF_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_LIGHT_ONOFF_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_light_on_off_set(const struct bt_mesh_model *model, uint16_t dst,
									 uint16_t app_key_index, uint8_t light_on_off, uint8_t tid, bool optional,
									 rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 2;
	if (optional) {
		msg_len = 4;
	}
	uint32_t op = ack ? OP_LIGHT_LC_LIGHT_ONOFF_SET : OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_LIGHT_ONOFF_SET, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_u8(&buf, light_on_off);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_property_get(const struct bt_mesh_model *model, uint16_t dst,
								 uint16_t app_key_index, uint16_t property_id)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_PROPERTY_GET, 2);
	bt_mesh_model_msg_init(&buf, OP_LIGHT_LC_PROPERTY_GET);
	net_buf_simple_add_le16(&buf, property_id);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int light_lc_property_set(const struct bt_mesh_model *model, uint16_t dst,
								 uint16_t app_key_index, uint16_t property_id,
								 uint8_t *pvalue, uint16_t value_len, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_LIGHT_LC_PROPERTY_SET : OP_LIGHT_LC_PROPERTY_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_LIGHT_LC_PROPERTY_SET, 2 + value_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, property_id);
	net_buf_simple_add_mem(&buf, pvalue, value_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_light_lc_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lc_mode_get_t *lc_mode_get = (rtk_bt_mesh_light_lc_mode_get_t *)p_cmd->param;
		ret = light_lc_mode_get(light_lc_client_model, lc_mode_get->dst, lc_mode_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_MODE_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lc_mode_set_t *lc_mode_set = (rtk_bt_mesh_light_lc_mode_set_t *)p_cmd->param;
		ret = light_lc_mode_set(light_lc_client_model, lc_mode_set->dst, lc_mode_set->app_key_index, lc_mode_set->mode, lc_mode_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lc_om_get_t *lc_om_get = (rtk_bt_mesh_light_lc_om_get_t *)p_cmd->param;
		ret = light_lc_om_get(light_lc_client_model, lc_om_get->dst, lc_om_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_OM_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lc_om_set_t *lc_om_set = (rtk_bt_mesh_light_lc_om_set_t *)p_cmd->param;
		ret = light_lc_om_set(light_lc_client_model, lc_om_set->dst, lc_om_set->app_key_index, lc_om_set->mode, lc_om_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lc_lightonoff_get_t *lc_lightonoff_get = (rtk_bt_mesh_light_lc_lightonoff_get_t *)p_cmd->param;
		ret = light_lc_light_on_off_get(light_lc_client_model, lc_lightonoff_get->dst, lc_lightonoff_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_LIGHTONOFF_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lc_lightonoff_set_t *lc_lightonoff_set = (rtk_bt_mesh_light_lc_lightonoff_set_t *)p_cmd->param;
		ret = light_lc_light_on_off_set(light_lc_client_model, lc_lightonoff_set->dst, lc_lightonoff_set->app_key_index,
										lc_lightonoff_set->light_on_off, lc_lightonoff_set->tid, lc_lightonoff_set->optional,
										lc_lightonoff_set->trans_time, lc_lightonoff_set->delay, lc_lightonoff_set->ack);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_GET: {
		rtk_bt_mesh_light_lc_property_get_t *lc_property_get = (rtk_bt_mesh_light_lc_property_get_t *)p_cmd->param;
		ret = light_lc_property_get(light_lc_client_model, lc_property_get->dst, lc_property_get->app_key_index, lc_property_get->property_id);
		break;
	}
	case RTK_BT_MESH_LIGHT_LC_PROPERTY_CLIENT_MODEL_SET: {
		rtk_bt_mesh_light_lc_property_set_t *lc_property_set = (rtk_bt_mesh_light_lc_property_set_t *)p_cmd->param;
		ret = light_lc_property_set(light_lc_client_model, lc_property_set->dst, lc_property_set->app_key_index, lc_property_set->property_id,
									(uint8_t *) & (lc_property_set->pvalue), lc_property_set->value_len, lc_property_set->ack);
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