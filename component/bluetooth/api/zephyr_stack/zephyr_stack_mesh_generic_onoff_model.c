/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <rtk_bt_common.h>
#include <bt_api_config.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <msg.h>

#include <rtk_bt_def.h>
#include <rtk_bt_mesh_generic_onoff_model.h>

#define OP_ONOFF_GET       BT_MESH_MODEL_OP_2(0x82, 0x01)
#define OP_ONOFF_SET       BT_MESH_MODEL_OP_2(0x82, 0x02)
#define OP_ONOFF_SET_UNACK BT_MESH_MODEL_OP_2(0x82, 0x03)
#define OP_ONOFF_STATUS    BT_MESH_MODEL_OP_2(0x82, 0x04)

/* OnOff messages' transition time and remaining time fields are encoded as an
 * 8 bit value with a 6 bit step field and a 2 bit resolution field.
 * The resolution field maps to:
 * 0: 100 ms
 * 1: 1 s
 * 2: 10 s
 * 3: 20 min
 */
static const uint32_t time_res[] = {
	100,
	MSEC_PER_SEC,
	10 * MSEC_PER_SEC,
	10 * 60 * MSEC_PER_SEC,
};

static inline int32_t model_time_decode(uint8_t val)
{
	uint8_t resolution = (val >> 6) & BIT_MASK(2);
	uint8_t steps = val & BIT_MASK(6);

	if (steps == 0x3f) {
		return SYS_FOREVER_MS;
	}

	return steps * time_res[resolution];
}

static inline uint8_t model_time_encode(uint32_t ms)
{
	if (ms == SYS_FOREVER_MS) {
		return 0x3f;
	}

	for (uint32_t i = 0; i < ARRAY_SIZE(time_res); i++) {
		if (ms >= BIT_MASK(6) * time_res[i]) {
			continue;
		}

		uint8_t steps = DIV_ROUND_UP(ms, time_res[i]);

		return steps | (i << 6);
	}

	return 0x3f;
}

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL
static const struct bt_mesh_model *generic_onoff_client_model = NULL;

static int gen_onoff_cli_init(const struct bt_mesh_model *model)
{
	generic_onoff_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_gen_onoff_cli_cb = {
	.init = gen_onoff_cli_init,
};

static int gen_onoff_status(const struct bt_mesh_model *model,
							struct bt_mesh_msg_ctx *ctx,
							struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	uint8_t present = net_buf_simple_pull_u8(buf);

	rtk_bt_mesh_generic_onoff_client_evt_status_t *onoff_status;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_CLIENT_MODEL, RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_EVT_STATUS,
								sizeof(rtk_bt_mesh_generic_onoff_client_evt_status_t));
	onoff_status = (rtk_bt_mesh_generic_onoff_client_evt_status_t *)p_evt->data;
	onoff_status->src = ctx->addr;
	onoff_status->present_on_off = present;

	if (buf->len) {
		uint8_t val;
		uint8_t target = net_buf_simple_pull_u8(buf);
		val = net_buf_simple_pull_u8(buf);

		onoff_status->optional = true;
		onoff_status->target_on_off = target;
		onoff_status->remaining_time.num_steps = BIT_MASK(6) & val;
		onoff_status->remaining_time.step_resolution = BIT_MASK(2) & (val >> 6);
	}

	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

const struct bt_mesh_model_op gen_onoff_cli_op[] = {
	{OP_ONOFF_STATUS, BT_MESH_LEN_MIN(1), gen_onoff_status},
	BT_MESH_MODEL_OP_END,
};

static uint16_t bt_mesh_generic_onoff_set(rtk_bt_mesh_generic_onoff_client_act_set_t *set)
{
	uint16_t ret = 1;

	BT_MESH_MODEL_BUF_DEFINE(msg, OP_ONOFF_SET, 4);
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(set->app_key_index, set->dst);

	if (set->ack) {
		bt_mesh_model_msg_init(&msg, OP_ONOFF_SET);
	} else {
		bt_mesh_model_msg_init(&msg, OP_ONOFF_SET_UNACK);
	}

	net_buf_simple_add_u8(&msg, set->on_off);
	net_buf_simple_add_u8(&msg, set->tid);

	if (set->optional) {
		uint8_t val = (BIT_MASK(6) & set->trans_time.num_steps) | ((BIT_MASK(2) & set->trans_time.step_resolution) << 6);
		net_buf_simple_add_u8(&msg, val);
		net_buf_simple_add_u8(&msg, set->delay);
	} else {
		net_buf_simple_add_u8(&msg, 0);
		net_buf_simple_add_u8(&msg, 0);
	}

	if (generic_onoff_client_model) {
		ret = (uint16_t)bt_mesh_msg_send(generic_onoff_client_model, &ctx, &msg);
	} else {
		BT_LOGE("[%s] Get model fail\r\n", __func__);
		ret = 2;
	}
	return ret;
}

static uint16_t bt_mesh_generic_onoff_get(rtk_bt_mesh_generic_onoff_client_act_get_t *get)
{
	uint16_t ret = 1;

	BT_MESH_MODEL_BUF_DEFINE(msg, OP_ONOFF_GET, 0);
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(get->app_key_index, get->dst);
	bt_mesh_model_msg_init(&msg, OP_ONOFF_GET);
	if (generic_onoff_client_model) {
		ret = (uint16_t)bt_mesh_msg_send(generic_onoff_client_model, &ctx, &msg);
	} else {
		BT_LOGE("[%s] Get model fail\r\n", __func__);
		ret = 2;
	}
	return ret;
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_generic_onoff_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("[%s] Error: BLE MESH profile is not initiated\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_SET: {
		rtk_bt_mesh_generic_onoff_client_act_set_t *onoff_set = (rtk_bt_mesh_generic_onoff_client_act_set_t *)p_cmd->param;
		ret = bt_mesh_generic_onoff_set(onoff_set);
		break;
	}
	case RTK_BT_MESH_GENERIC_ONOFF_CLIENT_MODEL_ACT_GET: {
		rtk_bt_mesh_generic_onoff_client_act_get_t *onoff_get = (rtk_bt_mesh_generic_onoff_client_act_get_t *)p_cmd->param;
		ret = bt_mesh_generic_onoff_get(onoff_get);
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
#endif  // BT_MESH_ENABLE_GENERIC_ON_OFF_CLIENT_MODEL

#if defined(BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL) && BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL

static void onoff_timeout_delay(struct k_work *work);
static void onoff_timeout_trans(struct k_work *work);

static struct {
	bool val;
	uint8_t tid;
	uint16_t src;
	uint32_t transition_time;
	struct k_work_delayable work_delay;
	struct k_work_delayable work_trans;
} onoff;

static int gen_onoff_srv_init(const struct bt_mesh_model *model)
{
	(void)model;
	k_work_init_delayable(&onoff.work_delay, onoff_timeout_delay);
	k_work_init_delayable(&onoff.work_trans, onoff_timeout_trans);
	return 0;
}

static void indicate_generic_onoff_status(rtk_bt_mesh_generic_on_off_t onoff, uint32_t total, uint32_t remain)
{
	uint8_t val;
	rtk_bt_mesh_generic_transition_time_t time = {0};
	rtk_bt_mesh_generic_onoff_server_evt_set_t *onoff_set = NULL;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_SERVER_MODEL, RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_SET,
								sizeof(rtk_bt_mesh_generic_onoff_server_evt_set_t));
	onoff_set = (rtk_bt_mesh_generic_onoff_server_evt_set_t *)p_evt->data;
	onoff_set->on_off = onoff;

	val = model_time_encode(total);
	time.num_steps = BIT_MASK(6) & val;
	time.step_resolution = BIT_MASK(2) & (val >> 6);
	onoff_set->total_time = time;

	val = model_time_encode(remain);
	time.num_steps = BIT_MASK(6) & val;
	time.step_resolution = BIT_MASK(2) & (val >> 6);
	onoff_set->remaining_time = time;

	rtk_bt_evt_indicate(p_evt, NULL);
}

// Get the newest on off value from app
static uint8_t get_generic_onoff_status_from_app(void)
{
	rtk_bt_mesh_generic_on_off_t value = RTK_BT_MESH_GENERIC_OFF;
	rtk_bt_mesh_generic_onoff_server_evt_get_t *get_onoff;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_GENERIC_ONOFF_SERVER_MODEL, RTK_BT_MESH_GENERIC_ONOFF_SERVER_MODEL_EVT_GET,
								sizeof(rtk_bt_mesh_generic_onoff_server_evt_get_t));
	get_onoff = (rtk_bt_mesh_generic_onoff_server_evt_get_t *)p_evt->data;
	get_onoff->on_off = &value;
	rtk_bt_evt_indicate(p_evt, NULL);

	return (RTK_BT_MESH_GENERIC_ON == value) ? 1 : 0;
}

const struct bt_mesh_model_cb bt_mesh_gen_onoff_srv_cb = {
	.init = gen_onoff_srv_init,
};

int generic_onoff_server_pub_update(const struct bt_mesh_model *mod)
{
	// Prepare and init the Generic OnOff Status message for publish of generic on off server model
	struct net_buf_simple *msg = mod->pub->msg;

	bt_mesh_model_msg_init(msg, OP_ONOFF_STATUS);

	// Get the newest on off value from app
	net_buf_simple_add_u8(msg, get_generic_onoff_status_from_app());

	return 0;
}

static void onoff_timeout_trans(struct k_work *work)
{
	(void)work;
	// if set to OFF and trans time not equal to 0, should notify app set to OFF when transition time (ref mesh model spec 3.1.1.1 Binary state transitions)
	if (!onoff.val) {
		indicate_generic_onoff_status(onoff.val, 0, 0);
	}
	onoff.transition_time = 0;
}

static void onoff_timeout_delay(struct k_work *work)
{
	(void)work;
	if (0 != onoff.transition_time) {
		// if set to ON and trans time not equal to 0, should notify app set to ON when start transition(ref mesh model spec 3.1.1.1 Binary state transitions)
		if (onoff.val) {
			indicate_generic_onoff_status(onoff.val, onoff.transition_time, onoff.transition_time);
		}
		k_work_reschedule(&onoff.work_trans, K_MSEC(onoff.transition_time));
	} else {
		indicate_generic_onoff_status(onoff.val, 0, 0);
	}
}

static int onoff_status_send(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx)
{
	uint32_t remaining = onoff.transition_time;

	BT_MESH_MODEL_BUF_DEFINE(buf, OP_ONOFF_STATUS, 3);
	bt_mesh_model_msg_init(&buf, OP_ONOFF_STATUS);

	/* Check using remaining time instead of "work pending" to make the
	 * onoff status send the right value on instant transitions. As the
	 * work item is executed in a lower priority than the mesh message
	 * handler, the work will be pending even on instant transitions.
	 */
	if (remaining) {
		net_buf_simple_add_u8(&buf, get_generic_onoff_status_from_app());
		net_buf_simple_add_u8(&buf, onoff.val);
		net_buf_simple_add_u8(&buf, model_time_encode(remaining));
	} else {
		net_buf_simple_add_u8(&buf, get_generic_onoff_status_from_app());
	}

	return bt_mesh_model_send(model, ctx, &buf, NULL, NULL);
}

static int gen_onoff_get(const struct bt_mesh_model *model,
						 struct bt_mesh_msg_ctx *ctx,
						 struct net_buf_simple *buf)
{
	(void)buf;
	onoff_status_send(model, ctx);
	return 0;
}

static int gen_onoff_set_unack(const struct bt_mesh_model *model,
							   struct bt_mesh_msg_ctx *ctx,
							   struct net_buf_simple *buf)
{
	(void)model;
	uint8_t val = net_buf_simple_pull_u8(buf);
	uint8_t tid = net_buf_simple_pull_u8(buf);
	int32_t trans = 0;
	int32_t delay = 0;

	if (buf->len) {
		trans = model_time_decode(net_buf_simple_pull_u8(buf));
		delay = net_buf_simple_pull_u8(buf) * 5;
	}

	/* Only perform change if the message wasn't a duplicate and the
	 * value is different.
	 */
	if (tid == onoff.tid && ctx->addr == onoff.src) {
		/* Duplicate */
		// BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
		return 0;
	}

	if (val == onoff.val) {
		// BT_LOGE("[%s] Equal to target, do not need change\r\n", __func__);
		/* No change */
		// return 0;
	}

	onoff.tid = tid;
	onoff.src = ctx->addr;
	onoff.val = val;
	onoff.transition_time = trans;

	/* Schedule the next action to happen on the delay, and keep
	 * transition time stored, so it can be applied in the timeout.
	 */
	if (0 == delay) {
		if (0 != trans) {
			// if set to ON and trans time not equal to 0, should notify app set to ON when start transition(ref mesh model spec 3.1.1.1 Binary state transitions)
			if (onoff.val) {
				indicate_generic_onoff_status(onoff.val, onoff.transition_time, onoff.transition_time);
			}
			k_work_reschedule(&onoff.work_trans, K_MSEC(onoff.transition_time));
		} else {
			indicate_generic_onoff_status(onoff.val, 0, 0);
		}
	} else {
		k_work_reschedule(&onoff.work_delay, K_MSEC(delay));
	}

	return 0;
}

static int gen_onoff_set(const struct bt_mesh_model *model,
						 struct bt_mesh_msg_ctx *ctx,
						 struct net_buf_simple *buf)
{
	(void)gen_onoff_set_unack(model, ctx, buf);
	onoff_status_send(model, ctx);

	return 0;
}

const struct bt_mesh_model_op gen_onoff_srv_op[] = {
	{ OP_ONOFF_GET,       BT_MESH_LEN_EXACT(0), gen_onoff_get },
	{ OP_ONOFF_SET,       BT_MESH_LEN_MIN(2),   gen_onoff_set },
	{ OP_ONOFF_SET_UNACK, BT_MESH_LEN_MIN(2),   gen_onoff_set_unack },
	BT_MESH_MODEL_OP_END,
};

#endif  // BT_MESH_ENABLE_GENERIC_ON_OFF_SERVER_MODEL
