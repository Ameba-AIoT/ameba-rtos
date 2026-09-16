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
#include <rtk_bt_mesh_scheduler_model.h>
#include <msg.h>

#define OP_SCHEDULER_ACTION_GET              BT_MESH_MODEL_OP_2(0x82, 0x48)
#define OP_SCHEDULER_ACTION_STATUS           BT_MESH_MODEL_OP_1(0x5F)
#define OP_SCHEDULER_GET                     BT_MESH_MODEL_OP_2(0x82, 0x49)
#define OP_SCHEDULER_STATUS                  BT_MESH_MODEL_OP_2(0x82, 0x4A)

#define OP_SCHEDULER_ACTION_SET              BT_MESH_MODEL_OP_1(0x60)
#define OP_SCHEDULER_ACTION_SET_UNACK        BT_MESH_MODEL_OP_1(0x61)

#if ((defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL))
static int scheduler_action_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_scheduler_register_t scheduler)
{
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SCHEDULER_ACTION_STATUS, sizeof(rtk_bt_mesh_scheduler_register_t));
	bt_mesh_model_msg_init(&pmsg, OP_SCHEDULER_ACTION_STATUS);
	net_buf_simple_add_mem(&pmsg, &scheduler, sizeof(rtk_bt_mesh_scheduler_register_t));
	return bt_mesh_msg_send(model, ctx, &pmsg);
}
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SERVER_MODEL
static const struct bt_mesh_model *scheduler_server_model = NULL;

static int scheduler_srv_init(const struct bt_mesh_model *model)
{
	scheduler_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_scheduler_srv_cb = {
	.init = scheduler_srv_init,
};

static int scheduler_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, uint16_t schedulers)
{
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SCHEDULER_STATUS, 2);
	bt_mesh_model_msg_init(&pmsg, OP_SCHEDULER_STATUS);
	net_buf_simple_add_le16(&pmsg, schedulers);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

int scheduler_publish(const struct bt_mesh_model *model, uint16_t schedulers)
{
	return scheduler_status(model, NULL, schedulers);
}

int scheduler_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	uint16_t scheduler;
	rtk_bt_mesh_scheduler_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_SERVER_MODEL, RTK_BT_MESH_SCHEDULER_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_scheduler_server_get_t));
	p_get_data = (rtk_bt_mesh_scheduler_server_get_t *)p_evt->data;
	p_get_data->schedulers = &scheduler;
	rtk_bt_evt_indicate(p_evt, NULL);

	bt_mesh_model_msg_init(msg, OP_SCHEDULER_STATUS);
	net_buf_simple_add_le16(msg, scheduler);
	return 0;
}

static int scheduler_srv_get(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t scheduler;
		rtk_bt_mesh_scheduler_server_get_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_SERVER_MODEL, RTK_BT_MESH_SCHEDULER_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_scheduler_server_get_t));
		p_get_data = (rtk_bt_mesh_scheduler_server_get_t *)p_evt->data;
		p_get_data->schedulers = &scheduler;
		rtk_bt_evt_indicate(p_evt, NULL);

		return scheduler_status(model, ctx, scheduler);
	}
	return -1;
}

static int scheduler_srv_action_get(const struct bt_mesh_model *model,
									struct bt_mesh_msg_ctx *ctx,
									struct net_buf_simple *buf)
{
	if (buf->len) {
		uint8_t index = net_buf_simple_pull_u8(buf);
		rtk_bt_mesh_scheduler_register_t schedulers = {0};
		rtk_bt_mesh_scheduler_server_get_action_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_SERVER_MODEL, RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_scheduler_server_get_action_t));
		p_get_data = (rtk_bt_mesh_scheduler_server_get_action_t *)p_evt->data;
		p_get_data->index = index;
		p_get_data->scheduler = &schedulers;
		rtk_bt_evt_indicate(p_evt, NULL);

		return scheduler_action_status(model, ctx, schedulers);
	}
	return -1;
}

const struct bt_mesh_model_op scheduler_srv_op[] = {
	{ OP_SCHEDULER_GET,                 BT_MESH_LEN_EXACT(0),     scheduler_srv_get },
	{ OP_SCHEDULER_ACTION_GET,          BT_MESH_LEN_EXACT(1),     scheduler_srv_action_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCHEDULER_SETUP_SERVER_MODEL
#define IS_SCHEDULER_INDEX_VALID(index)     (1)

/* scheduler year */
#define SCHEDULER_EVERY_YEAR                0x64
#define IS_SCHEDULER_YEAR_VALIE(year)       ((year) <= 0x64)

/* scheduler month */
#define SCHEDULER_IN_JANUARY                (1 << 0)
#define SCHEDULER_IN_FEBRUARY               (1 << 1)
#define SCHEDULER_IN_MARCH                  (1 << 2)
#define SCHEDULER_IN_APRIL                  (1 << 3)
#define SCHEDULER_IN_MAY                    (1 << 4)
#define SCHEDULER_IN_JUNE                   (1 << 5)
#define SCHEDULER_IN_JULY                   (1 << 6)
#define SCHEDULER_IN_AUGUST                 (1 << 7)
#define SCHEDULER_IN_SETEMBER               (1 << 8)
#define SCHEDULER_IN_OCTOBER                (1 << 9)
#define SCHEDULER_IN_NOVEMBER               (1 << 10)
#define SCHEDULER_IN_DECEMBER               (1 << 11)
#define SCHEDULER_EVERY_MONTH               0x0FFF

/* scheduler day */
#define SCHEDULER_EVERY_DAY                 0x00

/* scheduler hour */
#define SCHEDULER_EVERY_HOUR                0x18
#define SCHEDULER_RANDOM_HOUR               0x19
#define IS_SCHEDULER_HOUR_VALID(hour)       ((hour) <= 0x19)

/* scheduler minute */
#define SCHEDULER_EVERY_MINUTE              0x3C
#define SCHEDULER_EVERY_15_MINUTE           0x3D
#define SCHEDULER_EVERY_20_MINUTE           0x3E
#define SCHEDULER_RADOM_MINUTE              0x3F

/* scheduler second */
#define SCHEDULER_EVERY_SECOND              0x3C
#define SCHEDULER_EVERY_15_SECOND           0x3D
#define SCHEDULER_EVERY_20_SECOND           0x3E
#define SCHEDULER_RANDOM_SECOND             0x3F

/* scheduler day of week */
#define SCHEDULER_ON_MONDAYS               (1 << 0)
#define SCHEDULER_ON_TUESDAYS              (1 << 1)
#define SCHEDULER_ON_WEDNESDAYS            (1 << 2)
#define SCHEDULER_ON_THURSDAYS             (1 << 3)
#define SCHEDULER_ON_FRIDAYS               (1 << 4)
#define SCHEDULER_ON_SATURDAYS             (1 << 5)
#define SCHEDULER_ON_SUNDAYS               (1 << 6)

/* scheduler action */
#define SCHEDULER_TURN_OFF                 0x00
#define SCHEDULER_TURN_ON                  0x01
#define SCHEDULER_SCENE_RECALL             0x02
#define SCHEDULER_NO_ACTION                0x0F

/* scheduler scene */
#define SCHEDULER_NO_SCENE                 0x00

static const struct bt_mesh_model *scheduler_setup_server_model = NULL;

static int scheduler_setup_srv_init(const struct bt_mesh_model *model)
{
	scheduler_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_scheduler_setup_srv_cb = {
	.init = scheduler_setup_srv_init,
};

static bool scheduler_check_parameter(const rtk_bt_mesh_scheduler_register_t *pscheduler)
{
	if (!IS_SCHEDULER_INDEX_VALID(pscheduler->index)) {
		return FALSE;
	}

	if (!IS_SCHEDULER_YEAR_VALIE(pscheduler->year)) {
		return FALSE;
	}

	if (!IS_SCHEDULER_HOUR_VALID(pscheduler->hour)) {
		return FALSE;
	}

	return TRUE;
}

static int scheduler_setup_srv_action_set_unack(const struct bt_mesh_model *model,
												struct bt_mesh_msg_ctx *ctx,
												struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_scheduler_register_t *p_scheduler = (rtk_bt_mesh_scheduler_register_t *)buf->data;
		net_buf_simple_pull(buf, sizeof(rtk_bt_mesh_scheduler_register_t));
		if (scheduler_check_parameter(p_scheduler)) {
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_SETUP_SERVER_MODEL, RTK_BT_MESH_SCHEDULER_ACTION_SERVER_MODEL_SET,
										sizeof(rtk_bt_mesh_scheduler_register_t));
			memcpy(p_evt->data, p_scheduler, sizeof(rtk_bt_mesh_scheduler_register_t));
			return rtk_bt_evt_indicate(p_evt, NULL);
		}
		return -1;
	}
	return -1;
}

static int scheduler_setup_srv_action_set(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	rtk_bt_mesh_scheduler_register_t *p_scheduler = (rtk_bt_mesh_scheduler_register_t *)buf->data;
	uint8_t index = p_scheduler->index;
	if (!scheduler_setup_srv_action_set_unack(model, ctx, buf)) {
		rtk_bt_mesh_scheduler_server_get_action_t *p_get_data;
		rtk_bt_mesh_scheduler_register_t schedulers;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_SETUP_SERVER_MODEL, RTK_BT_MESH_SCHEDULER_SETUP_SERVER_MODEL_GET_ACTION,
									sizeof(rtk_bt_mesh_scheduler_server_get_action_t));
		p_get_data = (rtk_bt_mesh_scheduler_server_get_action_t *)p_evt->data;
		p_get_data->index = index;
		p_get_data->scheduler = &schedulers;
		rtk_bt_evt_indicate(p_evt, NULL);

		return scheduler_action_status(model, ctx, schedulers);
	}
	return -1;
}

const struct bt_mesh_model_op scheduler_setup_srv_op[] = {
	{ OP_SCHEDULER_ACTION_SET,                BT_MESH_LEN_EXACT(10),     scheduler_setup_srv_action_set },
	{ OP_SCHEDULER_ACTION_SET_UNACK,          BT_MESH_LEN_EXACT(10),     scheduler_setup_srv_action_set_unack },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL) && BT_MESH_ENABLE_SCHEDULER_CLIENT_MODEL
static const struct bt_mesh_model *scheduler_client_model = NULL;

static int scheduler_cli_init(const struct bt_mesh_model *model)
{
	scheduler_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_scheduler_cli_cb = {
	.init = scheduler_cli_init,
};

static int scheduler_cli_action_status(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_scheduler_register_t *scheduler = (rtk_bt_mesh_scheduler_register_t *)buf->data;
		rtk_bt_mesh_scheduler_client_status_action_t status_data;
		status_data.src = ctx->addr;
		status_data.scheduler = *scheduler;

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL, RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_scheduler_client_status_action_t));
		memcpy(p_evt->data, &status_data, sizeof(rtk_bt_mesh_scheduler_client_status_action_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int scheduler_cli_status(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx,
								struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_scheduler_client_status_t status;
		status.src = ctx->addr;
		status.schedulers = net_buf_simple_pull_le16(buf);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCHEDULER_CLIENT_MODEL, RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_scheduler_client_status_t));
		memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_scheduler_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op scheduler_cli_op[] = {
	{ OP_SCHEDULER_ACTION_STATUS,       BT_MESH_LEN_EXACT(10),     scheduler_cli_action_status },
	{ OP_SCHEDULER_STATUS,              BT_MESH_LEN_EXACT(2),      scheduler_cli_status },
	BT_MESH_MODEL_OP_END,
};

static int scheduler_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCHEDULER_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_SCHEDULER_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scheduler_action_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, uint8_t index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCHEDULER_ACTION_GET, 1);
	bt_mesh_model_msg_init(&buf, OP_SCHEDULER_ACTION_GET);
	net_buf_simple_add_u8(&buf, index);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scheduler_action_set(const struct bt_mesh_model *model, uint16_t dst,
								uint16_t app_key_index, rtk_bt_mesh_scheduler_register_t scheduler,
								bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_SCHEDULER_ACTION_SET : OP_SCHEDULER_ACTION_SET_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCHEDULER_ACTION_SET, 10);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_mem(&buf, &scheduler, sizeof(rtk_bt_mesh_scheduler_register_t));
	return bt_mesh_msg_send(model, &ctx, &buf);
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_scheduler_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SCHEDULER_CLIENT_MODEL_GET: {
		rtk_bt_mesh_scheduler_get_t *scheduler = (rtk_bt_mesh_scheduler_get_t *)p_cmd->param;
		ret = scheduler_get(scheduler_client_model, scheduler->dst, scheduler->app_key_index);
		break;
	}
	case RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_GET: {
		rtk_bt_mesh_scheduler_action_get_t *scheduler_action = (rtk_bt_mesh_scheduler_action_get_t *)p_cmd->param;
		ret = scheduler_action_get(scheduler_client_model, scheduler_action->dst, scheduler_action->app_key_index,
								   scheduler_action->index);
		break;
	}
	case RTK_BT_MESH_SCHEDULER_ACTION_CLIENT_MODEL_SET: {
		rtk_bt_mesh_scheduler_action_set_t *action_set = (rtk_bt_mesh_scheduler_action_set_t *)p_cmd->param;
		ret = scheduler_action_set(scheduler_client_model, action_set->dst, action_set->app_key_index,
								   action_set->scheduler, action_set->ack);
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