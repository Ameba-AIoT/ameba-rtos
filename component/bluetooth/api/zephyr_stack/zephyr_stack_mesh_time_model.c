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
#include <rtk_bt_mesh_time_model.h>
#include <msg.h>

#define OP_TIME_GET                     BT_MESH_MODEL_OP_2(0x82, 0x37)
#define OP_TIME_SET                     BT_MESH_MODEL_OP_1(0x5C)
#define OP_TIME_STATUS                  BT_MESH_MODEL_OP_1(0x5D)
#define OP_TIME_ROLE_GET                BT_MESH_MODEL_OP_2(0x82, 0x38)
#define OP_TIME_ROLE_SET                BT_MESH_MODEL_OP_2(0x82, 0x39)
#define OP_TIME_ROLE_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x3A)
#define OP_TIME_ZONE_GET                BT_MESH_MODEL_OP_2(0x82, 0x3B)
#define OP_TIME_ZONE_SET                BT_MESH_MODEL_OP_2(0x82, 0x3C)
#define OP_TIME_ZONE_STATUS             BT_MESH_MODEL_OP_2(0x82, 0x3D)
#define OP_TIME_TAI_UTC_DELTA_GET       BT_MESH_MODEL_OP_2(0x82, 0x3E)
#define OP_TIME_TAI_UTC_DELTA_SET       BT_MESH_MODEL_OP_2(0x82, 0x3F)
#define OP_TIME_TAI_UTC_DELTA_STATUS    BT_MESH_MODEL_OP_2(0x82, 0x40)

#if ((defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL))
static int time_status(const struct bt_mesh_model *model,
					   struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_tai_time_t time)
{
	uint8_t zero_tai_seconds[5] = {0, 0, 0, 0, 0};
	uint16_t msg_len = 10;
	if (0 == memcmp(time.tai_seconds, zero_tai_seconds, 5)) {
		msg_len = 5;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_TIME_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_TIME_STATUS);
	net_buf_simple_add_mem(&pmsg, time.tai_seconds, 5);
	if (0 != memcmp(time.tai_seconds, zero_tai_seconds, 5)) {
		net_buf_simple_add_u8(&pmsg, time.subsecond);
		net_buf_simple_add_u8(&pmsg, time.uncertainty);
		uint16_t val = 0;
		val = time.time_authority;
		val |= (time.tai_utc_delta << 1);
		net_buf_simple_add_le16(&pmsg, val);
		net_buf_simple_add_u8(&pmsg, time.time_zone_offset);
	}
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int time_zone_status(const struct bt_mesh_model *model,
							struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_time_zone_t time_zone)
{
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_TIME_ZONE_STATUS, 7);
	bt_mesh_model_msg_init(&pmsg, OP_TIME_ZONE_STATUS);
	net_buf_simple_add_u8(&pmsg, time_zone.time_zone_offset_current);
	net_buf_simple_add_u8(&pmsg, time_zone.time_zone_offset_new);
	net_buf_simple_add_mem(&pmsg, time_zone.tai_of_zone_change, 5);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int tai_utc_delta_status(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_tai_utc_delta_t time_tai_utc_delta)
{
	uint16_t current = 0;
	uint16_t new = 0;
	current |= time_tai_utc_delta.tai_utc_delta_current;
	new |= time_tai_utc_delta.tai_utc_delta_new;
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_TIME_TAI_UTC_DELTA_STATUS, 9);
	bt_mesh_model_msg_init(&pmsg, OP_TIME_TAI_UTC_DELTA_STATUS);
	net_buf_simple_add_le16(&pmsg, current);
	net_buf_simple_add_le16(&pmsg, new);
	net_buf_simple_add_mem(&pmsg, time_tai_utc_delta.tai_of_delta_change, 5);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static rtk_bt_mesh_time_role_t get_time_role(uint8_t group)
{
	rtk_bt_mesh_time_role_t role;
	rtk_bt_mesh_time_server_get_role_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	if (group == RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL) {
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_GET,
									sizeof(rtk_bt_mesh_time_server_get_role_t));
	} else if (group == RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL) {
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_ROLE_GET,
									sizeof(rtk_bt_mesh_time_server_get_role_t));
	}
	p_get_data = (rtk_bt_mesh_time_server_get_role_t *)p_evt->data;
	p_get_data->role = &role;
	rtk_bt_evt_indicate(p_evt, NULL);
	return role;
}
#endif

#if defined(BT_MESH_ENABLE_TIME_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SERVER_MODEL
static const struct bt_mesh_model *time_server_model = NULL;

static int time_srv_init(const struct bt_mesh_model *model)
{
	time_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_srv_cb = {
	.init = time_srv_init,
};

int time_publish(const struct bt_mesh_model *model, rtk_bt_mesh_time_server_status_set_t status)
{
	rtk_bt_mesh_tai_time_t time;
	memcpy(time.tai_seconds, status.tai_seconds, 5);
	time.subsecond = status.subsecond;
	time.uncertainty = status.uncertainty;
	time.time_authority = status.tai_utc_delta & 0x0001;
	time.tai_utc_delta = (status.tai_utc_delta & 0xFFFE) >> 1;
	time.time_zone_offset = status.time_zone_offset;
	return time_status(model, NULL, time);
}

int time_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	rtk_bt_mesh_time_server_get_t time;
	rtk_bt_mesh_time_server_direct_get_t *time_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_GET,
								sizeof(rtk_bt_mesh_time_server_direct_get_t));
	time_get = (rtk_bt_mesh_time_server_direct_get_t *)p_evt->data;
	time_get->value = &time;
	rtk_bt_evt_indicate(p_evt, NULL);

	uint8_t zero_tai_seconds[5] = {0, 0, 0, 0, 0};
	bt_mesh_model_msg_init(msg, OP_TIME_STATUS);
	net_buf_simple_add_mem(msg, time.tai_seconds, 5);
	if (0 == memcmp(time.tai_seconds, zero_tai_seconds, 5)) {
		net_buf_simple_add_u8(msg, time.subsecond);
		net_buf_simple_add_u8(msg, time.uncertainty);
		uint16_t val = 0;
		val = time.time_authority;
		val |= (time.tai_utc_delta << 1);
		net_buf_simple_add_le16(msg, val);
		net_buf_simple_add_u8(msg, time.time_zone_offset);
	}
	return 0;
}

static int time_srv_get(const struct bt_mesh_model *model,
						struct bt_mesh_msg_ctx *ctx,
						struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_time_server_get_t time;
		rtk_bt_mesh_time_server_direct_get_t *time_get;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_t));
		time_get = (rtk_bt_mesh_time_server_direct_get_t *)p_evt->data;
		time_get->value = &time;
		rtk_bt_evt_indicate(p_evt, NULL);

		return time_status(model, ctx, time);
	}
	return -1;
}

static int time_srv_zone_get(const struct bt_mesh_model *model,
							 struct bt_mesh_msg_ctx *ctx,
							 struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_time_server_get_zone_t zone;
		rtk_bt_mesh_time_server_direct_get_zone_t *zone_get;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_ZONE_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_zone_t));
		zone_get = (rtk_bt_mesh_time_server_direct_get_zone_t *)p_evt->data;
		zone_get->value = &zone;
		rtk_bt_evt_indicate(p_evt, NULL);

		return time_zone_status(model, ctx, zone);
	}
	return -1;
}

static int time_srv_tai_utc_delta_get(const struct bt_mesh_model *model,
									  struct bt_mesh_msg_ctx *ctx,
									  struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_time_server_get_tai_utc_delta_t delta;
		rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *delta_get;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_TAI_UTC_DELTA_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t));
		delta_get = (rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *)p_evt->data;
		delta_get->value = &delta;
		rtk_bt_evt_indicate(p_evt, NULL);

		return tai_utc_delta_status(model, ctx, delta);
	}
	return -1;
}

static int time_srv_status(const struct bt_mesh_model *model,
						   struct bt_mesh_msg_ctx *ctx,
						   struct net_buf_simple *buf)
{
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_time_server_status_set_t status = {0};
		memcpy(status.tai_seconds, buf->data, 5);
		net_buf_simple_pull(buf, 5);
		if (buf->len) {
			status.subsecond = net_buf_simple_pull_u8(buf);
			status.uncertainty = net_buf_simple_pull_u8(buf);
			status.tai_utc_delta = net_buf_simple_pull_le16(buf);
			status.time_zone_offset = net_buf_simple_pull_u8(buf);
		}
		/******Get role first******/
		rtk_bt_mesh_time_role_t role = get_time_role(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL);
		if ((RTK_BT_MESH_TIME_ROLE_RELAY == role) || (RTK_BT_MESH_TIME_ROLE_CLIENT == role)) {
			/* update time */
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SERVER_MODEL, RTK_BT_MESH_TIME_SERVER_MODEL_STATUS_SET, sizeof(rtk_bt_mesh_time_server_status_set_t));
			memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_time_server_status_set_t));
			rtk_bt_evt_indicate(p_evt, NULL);
		}

		if (RTK_BT_MESH_TIME_ROLE_RELAY == role) {
			/* publish time */
			time_publish(model, status);
		}
	}
	return -1;
}

const struct bt_mesh_model_op time_srv_op[] = {
	{ OP_TIME_GET,                      BT_MESH_LEN_EXACT(0),     time_srv_get },
	{ OP_TIME_STATUS,                   BT_MESH_LEN_MIN(5),       time_srv_status },
	{ OP_TIME_ZONE_GET,                 BT_MESH_LEN_EXACT(0),     time_srv_zone_get },
	{ OP_TIME_TAI_UTC_DELTA_GET,        BT_MESH_LEN_EXACT(0),     time_srv_tai_utc_delta_get },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_TIME_SETUP_SERVER_MODEL
static const struct bt_mesh_model *time_setup_server_model = NULL;

static int time_setup_srv_init(const struct bt_mesh_model *model)
{
	time_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_setup_srv_cb = {
	.init = time_setup_srv_init,
};

static int time_setup_srv_set(const struct bt_mesh_model *model,
							  struct bt_mesh_msg_ctx *ctx,
							  struct net_buf_simple *buf)
{
	if (buf->len) {
		rtk_bt_mesh_time_server_set_t p_set_data;
		memcpy(p_set_data.tai_seconds, buf->data, 5);
		net_buf_simple_pull(buf, 5);
		p_set_data.subsecond = net_buf_simple_pull_u8(buf);
		p_set_data.uncertainty = net_buf_simple_pull_u8(buf);
		uint16_t val = net_buf_simple_pull_le16(buf);
		p_set_data.time_authority = val & 0x0001;
		p_set_data.tai_utc_delta = (val & 0xFFFE) >> 1;
		p_set_data.time_zone_offset = net_buf_simple_pull_u8(buf);
		/**********First, set************/
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_SET, sizeof(rtk_bt_mesh_time_server_set_t));
		memcpy(p_evt->data, &p_set_data, sizeof(rtk_bt_mesh_time_server_set_t));
		rtk_bt_evt_indicate(p_evt, NULL);
		/**********Second, get************/
		rtk_bt_mesh_time_server_get_t time;
		rtk_bt_mesh_time_server_direct_get_t *time_get;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_t));
		time_get = (rtk_bt_mesh_time_server_direct_get_t *)p_evt->data;
		time_get->value = &time;
		rtk_bt_evt_indicate(p_evt, NULL);
		/**********Third, send status***********/
		return time_status(model, ctx, time);
	}
	return -1;
}

static int time_setup_srv_zone_set(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	if (buf->len) {
		rtk_bt_mesh_time_server_set_zone_t p_set_data;
		p_set_data.time_zone_offset_new = net_buf_simple_pull_u8(buf);
		memcpy(p_set_data.tai_of_zone_change, buf->data, 5);
		net_buf_simple_pull(buf, 5);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_SET,
									sizeof(rtk_bt_mesh_time_server_set_zone_t));
		memcpy(p_evt->data, &p_set_data, sizeof(rtk_bt_mesh_time_server_set_zone_t));
		rtk_bt_evt_indicate(p_evt, NULL);

		rtk_bt_mesh_time_server_get_zone_t zone;
		rtk_bt_mesh_time_server_direct_get_zone_t *zone_get;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ZONE_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_zone_t));
		zone_get = (rtk_bt_mesh_time_server_direct_get_zone_t *)p_evt->data;
		zone_get->value = &zone;
		rtk_bt_evt_indicate(p_evt, NULL);

		return time_zone_status(model, ctx, zone);
	}
	return -1;
}

static int time_setup_srv_tai_utc_delta_set(const struct bt_mesh_model *model,
											struct bt_mesh_msg_ctx *ctx,
											struct net_buf_simple *buf)
{
	if (buf->len) {
		rtk_bt_mesh_time_server_set_tai_utc_delta_t p_set_data;
		p_set_data.tai_utc_delta_new = net_buf_simple_pull_le16(buf) & 0x7FFF;
		memcpy(p_set_data.tai_of_delta_change, buf->data, 5);
		net_buf_simple_pull(buf, 5);
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_SET,
									sizeof(rtk_bt_mesh_time_server_set_tai_utc_delta_t));
		memcpy(p_evt->data, &p_set_data, sizeof(rtk_bt_mesh_time_server_set_tai_utc_delta_t));
		rtk_bt_evt_indicate(p_evt, NULL);

		rtk_bt_mesh_time_server_get_tai_utc_delta_t delta;
		rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *delta_get;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_TAI_UTC_DELTA_GET,
									sizeof(rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t));
		delta_get = (rtk_bt_mesh_time_server_direct_get_tai_utc_delta_t *)p_evt->data;
		delta_get->value = &delta;
		rtk_bt_evt_indicate(p_evt, NULL);

		return tai_utc_delta_status(model, ctx, delta);
	}
	return -1;
}

static int time_role_status(const struct bt_mesh_model *model,
							struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_time_role_t role)
{
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_TIME_ROLE_STATUS, 1);
	bt_mesh_model_msg_init(&pmsg, OP_TIME_ROLE_STATUS);
	net_buf_simple_add_u8(&pmsg, role);
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

static int time_setup_srv_role_get(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	if (!buf->len) {
		rtk_bt_mesh_time_role_t role = get_time_role(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL);
		return time_role_status(model, ctx, role);
	}
	return -1;
}

static int time_setup_srv_role_set(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	if (buf->len) {
		rtk_bt_mesh_time_role_t role = net_buf_simple_pull_u8(buf);
		if (role > 0x03) {
			return -1;
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL, RTK_BT_MESH_TIME_SETUP_SERVER_MODEL_ROLE_SET, sizeof(rtk_bt_mesh_time_role_t));
		memcpy(p_evt->data, &role, sizeof(rtk_bt_mesh_time_role_t));
		rtk_bt_evt_indicate(p_evt, NULL);

		rtk_bt_mesh_time_role_t current_role = get_time_role(RTK_BT_LE_GP_MESH_TIME_SETUP_SERVER_MODEL);
		return time_role_status(model, ctx, current_role);
	}
	return -1;
}

const struct bt_mesh_model_op time_setup_srv_op[] = {
	{ OP_TIME_SET,                      BT_MESH_LEN_EXACT(10),    time_setup_srv_set },
	{ OP_TIME_ZONE_SET,                 BT_MESH_LEN_EXACT(6),     time_setup_srv_zone_set },
	{ OP_TIME_TAI_UTC_DELTA_SET,        BT_MESH_LEN_EXACT(7),     time_setup_srv_tai_utc_delta_set },
	{ OP_TIME_ROLE_GET,                 BT_MESH_LEN_EXACT(0),     time_setup_srv_role_get },
	{ OP_TIME_ROLE_SET,                 BT_MESH_LEN_EXACT(1),     time_setup_srv_role_set },
	BT_MESH_MODEL_OP_END,
};
#endif

#if defined(BT_MESH_ENABLE_TIME_CLIENT_MODEL) && BT_MESH_ENABLE_TIME_CLIENT_MODEL
static const struct bt_mesh_model *time_client_model = NULL;

static int time_cli_init(const struct bt_mesh_model *model)
{
	time_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_time_cli_cb = {
	.init = time_cli_init,
};

static int time_cli_status(const struct bt_mesh_model *model,
						   struct bt_mesh_msg_ctx *ctx,
						   struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_time_client_status_t status = {0};
		status.src = ctx->addr;
		memcpy(status.tai_time.tai_seconds, buf->data, 5);
		net_buf_simple_pull(buf, 5);
		if (buf->len) {
			status.tai_time.subsecond = net_buf_simple_pull_u8(buf);
			status.tai_time.uncertainty = net_buf_simple_pull_u8(buf);
			uint16_t tai_utc_delta = net_buf_simple_pull_le16(buf);
			status.tai_time.time_authority = tai_utc_delta & 0x0001;
			status.tai_time.tai_utc_delta = (tai_utc_delta & 0xFFFE) >> 1;
			status.tai_time.time_zone_offset = net_buf_simple_pull_u8(buf);
		}
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_STATUS,
									sizeof(rtk_bt_mesh_time_client_status_t));
		memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_time_client_status_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int time_cli_zone_status(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx,
								struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_time_client_status_zone_t zone;
		zone.src = ctx->addr;
		zone.time_zone_offset_current = net_buf_simple_pull_u8(buf);
		zone.time_zone_offset_new = net_buf_simple_pull_u8(buf);
		memcpy(zone.tai_of_zone_change, buf->data, 5);
		net_buf_simple_pull(buf, 5);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_STATUS,
									sizeof(rtk_bt_mesh_time_client_status_zone_t));
		memcpy(p_evt->data, &zone, sizeof(rtk_bt_mesh_time_client_status_zone_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int time_cli_tai_utc_delta_status(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	(void)model;
	if (buf->len) {
		rtk_bt_mesh_time_client_status_tai_utc_delta_t delta;
		delta.src = ctx->addr;
		delta.tai_utc_delta_current = net_buf_simple_pull_le16(buf) & 0x7FFF;
		delta.padding1 = 0;
		delta.tai_utc_delta_new = net_buf_simple_pull_le16(buf) & 0x7FFF;
		delta.padding2 = 0;
		memcpy(delta.tai_of_delta_change, buf->data, 5);
		net_buf_simple_pull(buf, 5);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_STATUS,
									sizeof(rtk_bt_mesh_time_client_status_tai_utc_delta_t));
		memcpy(p_evt->data, &delta, sizeof(rtk_bt_mesh_time_client_status_tai_utc_delta_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

static int time_cli_role_status(const struct bt_mesh_model *model,
								struct bt_mesh_msg_ctx *ctx,
								struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	if (buf->len) {
		rtk_bt_mesh_time_client_status_role_t role;
		role.role = net_buf_simple_pull_u8(buf);

		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_TIME_CLIENT_MODEL, RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_STATUS,
									sizeof(rtk_bt_mesh_time_client_status_role_t));
		memcpy(p_evt->data, &role, sizeof(rtk_bt_mesh_time_client_status_role_t));
		return rtk_bt_evt_indicate(p_evt, NULL);
	}
	return -1;
}

const struct bt_mesh_model_op time_cli_op[] = {
	{ OP_TIME_STATUS,                      BT_MESH_LEN_MIN(5),       time_cli_status },
	{ OP_TIME_ZONE_STATUS,                 BT_MESH_LEN_EXACT(7),     time_cli_zone_status },
	{ OP_TIME_TAI_UTC_DELTA_STATUS,        BT_MESH_LEN_EXACT(9),     time_cli_tai_utc_delta_status },
	{ OP_TIME_ROLE_STATUS,                 BT_MESH_LEN_EXACT(1),     time_cli_role_status },
	BT_MESH_MODEL_OP_END,
};

static int time_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_TIME_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_get_zone(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_ZONE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_TIME_ZONE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_get_tai_utc_delta(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_TAI_UTC_DELTA_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_TIME_TAI_UTC_DELTA_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_get_role(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_ROLE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_TIME_ROLE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_set(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, rtk_bt_mesh_tai_time_t time)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_SET, 10);
	bt_mesh_model_msg_init(&buf, OP_TIME_SET);
	net_buf_simple_add_mem(&buf, &time, sizeof(rtk_bt_mesh_tai_time_t));
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_set_zone(const struct bt_mesh_model *model, uint16_t dst,
						 uint16_t app_key_index, uint8_t time_zone_offset_new,
						 uint8_t tai_of_zone_change[5])
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_ZONE_SET, 6);
	bt_mesh_model_msg_init(&buf, OP_TIME_ZONE_SET);
	net_buf_simple_add_u8(&buf, time_zone_offset_new);
	net_buf_simple_add_mem(&buf, tai_of_zone_change, 5);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_set_tai_utc_delta(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index,
								  uint16_t tai_utc_delta_new, uint8_t tai_of_delta_change[5])
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_TAI_UTC_DELTA_SET, 7);
	bt_mesh_model_msg_init(&buf, OP_TIME_TAI_UTC_DELTA_SET);
	tai_utc_delta_new &= 0x7FFF;
	net_buf_simple_add_le16(&buf, tai_utc_delta_new);
	net_buf_simple_add_mem(&buf, tai_of_delta_change, 5);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int time_set_role(const struct bt_mesh_model *model, uint16_t dst,
						 uint16_t app_key_index, rtk_bt_mesh_time_role_t role)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_TIME_ROLE_SET, 1);
	bt_mesh_model_msg_init(&buf, OP_TIME_ROLE_SET);
	net_buf_simple_add_u8(&buf, role);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_time_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_TIME_CLIENT_MODEL_GET: {
		rtk_bt_mesh_time_get_t *time_client_get = (rtk_bt_mesh_time_get_t *)p_cmd->param;
		ret = time_get(time_client_model, time_client_get->dst, time_client_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_GET: {
		rtk_bt_mesh_time_zone_get_t *time_client_zone_get = (rtk_bt_mesh_time_zone_get_t *)p_cmd->param;
		ret = time_get_zone(time_client_model, time_client_zone_get->dst, time_client_zone_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_GET: {
		rtk_bt_mesh_time_tai_utc_delta_get_t *time_client_delta_get = (rtk_bt_mesh_time_tai_utc_delta_get_t *)p_cmd->param;
		ret = time_get_tai_utc_delta(time_client_model, time_client_delta_get->dst, time_client_delta_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_GET: {
		rtk_bt_mesh_time_role_get_t *time_client_role_get = (rtk_bt_mesh_time_role_get_t *)p_cmd->param;
		ret = time_get_role(time_client_model, time_client_role_get->dst, time_client_role_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_SET: {
		rtk_bt_mesh_time_set_t *time_client_set = (rtk_bt_mesh_time_set_t *)p_cmd->param;
		ret = time_set(time_client_model, time_client_set->dst, time_client_set->app_key_index, time_client_set->time);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ZONE_SET: {
		rtk_bt_mesh_time_zone_set_t *time_client_zone_set = (rtk_bt_mesh_time_zone_set_t *)p_cmd->param;
		ret = time_set_zone(time_client_model, time_client_zone_set->dst, time_client_zone_set->app_key_index,
							time_client_zone_set->time_zone_offset_new, time_client_zone_set->tai_of_zone_change);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_TAI_UTC_DELTA_SET: {
		rtk_bt_mesh_time_tai_utc_delta_set_t *time_client_delta_set = (rtk_bt_mesh_time_tai_utc_delta_set_t *)p_cmd->param;
		ret = time_set_tai_utc_delta(time_client_model, time_client_delta_set->dst, time_client_delta_set->app_key_index,
									 time_client_delta_set->tai_utc_delta_new, time_client_delta_set->tai_of_delta_change);
		break;
	}
	case RTK_BT_MESH_TIME_CLIENT_MODEL_ROLE_SET: {
		rtk_bt_mesh_time_role_set_t *time_client_role_set = (rtk_bt_mesh_time_role_set_t *)p_cmd->param;
		ret = time_set_role(time_client_model, time_client_role_set->dst, time_client_role_set->app_key_index, time_client_role_set->role);
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