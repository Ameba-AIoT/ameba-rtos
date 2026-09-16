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
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_datatrans_model.h>

#define BT_MESH_MODEL_ID_DATATRANS 0x0004

#define OP_DATATRANS_WRITE              BT_MESH_MODEL_OP_3(0xC9, COMPANY_ID)
#define OP_DATATRANS_WRITE_UNACK        BT_MESH_MODEL_OP_3(0xCA, COMPANY_ID)
#define OP_DATATRANS_STATUS             BT_MESH_MODEL_OP_3(0xCB, COMPANY_ID)
#define OP_DATATRANS_READ               BT_MESH_MODEL_OP_3(0xCC, COMPANY_ID)
#define OP_DATATRANS_DATA               BT_MESH_MODEL_OP_3(0xCD, COMPANY_ID)

#if defined(BT_MESH_ENABLE_DATATRANS_MODEL) && BT_MESH_ENABLE_DATATRANS_MODEL

static const struct bt_mesh_model *datatrans_model = NULL;

static int datatrans_init(const struct bt_mesh_model *model)
{
	datatrans_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_datatrans_cb = {
	.init = datatrans_init,
};

static uint16_t datatrans_write_len = 0;
static rtk_bt_mesh_datatrans_stat_t data_write_status = RTK_BT_MESH_DATATRANS_SUCCESS;

static int datatrans_write_unack_receive(const struct bt_mesh_model *model,
										 struct bt_mesh_msg_ctx *ctx,
										 struct net_buf_simple *buf)
{
	(void)model;
	(void)ctx;
	uint8_t p_data[DATA_TRANS_DATA_MAX_LEN] = {};
	if (buf->len) {
		datatrans_write_len = buf->len;
		rtk_bt_evt_t *p_evt = NULL;
		rtk_bt_mesh_datatrans_server_write_event_t *p_data_write;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_WRITE,
									sizeof(rtk_bt_mesh_datatrans_server_write_event_t));
		p_data_write = (rtk_bt_mesh_datatrans_server_write_event_t *)p_evt->data;
		p_data_write->status = RTK_BT_MESH_DATATRANS_SUCCESS;
		if (buf->len > DATA_TRANS_DATA_MAX_LEN) {
			BT_LOGA("[%s] The len of datatrans receive data is %d, extend max DATA_TRANS_DATA_MAX_LEN:%d\r\n", __func__, buf->len, DATA_TRANS_DATA_MAX_LEN);
			p_data_write->data_len = DATA_TRANS_DATA_MAX_LEN;
		} else {
			p_data_write->data_len = buf->len;
		}
		for (int i = 0; i < p_data_write->data_len; i++) {
			p_data[i] = net_buf_simple_pull_u8(buf);
		}
		memcpy(p_data_write->data, p_data, p_data_write->data_len);
		rtk_bt_evt_indicate(p_evt, NULL);
	}

	return 0;
}

static int datatrans_write_status_send(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx)
{

	BT_MESH_MODEL_BUF_DEFINE(buf, OP_DATATRANS_STATUS, 3);
	bt_mesh_model_msg_init(&buf, OP_DATATRANS_STATUS);
	net_buf_simple_add_u8(&buf, data_write_status);
	net_buf_simple_add_le16(&buf, datatrans_write_len);

	return bt_mesh_msg_send(model, ctx, &buf);
}

static int datatrans_write_receive(const struct bt_mesh_model *model,
								   struct bt_mesh_msg_ctx *ctx,
								   struct net_buf_simple *buf)
{
	datatrans_write_unack_receive(model, ctx, buf);
	datatrans_write_status_send(model, ctx);
	return 0;
}

static int datatrans_read_data_receive(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	uint16_t data_len = net_buf_simple_pull_le16(buf);
	uint8_t cb_ret = 0;
	if (data_len > DATA_TRANS_DATA_MAX_LEN) {
		data_len = DATA_TRANS_DATA_MAX_LEN;
	}
	uint8_t p_data[DATA_TRANS_DATA_MAX_LEN] = {};
	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_datatrans_server_read_event_t *server_read;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_EVT_SERVER_READ,
								sizeof(rtk_bt_mesh_datatrans_server_read_event_t) + data_len);
	server_read = (rtk_bt_mesh_datatrans_server_read_event_t *)p_evt->data;
	server_read->data_len = data_len;
	server_read->data = p_data;
	rtk_bt_evt_indicate(p_evt, &cb_ret);

	BT_MESH_MODEL_BUF_DEFINE(buf_rsp, OP_DATATRANS_DATA, data_len);
	bt_mesh_model_msg_init(&buf_rsp, OP_DATATRANS_DATA);
	net_buf_simple_add_mem(&buf_rsp, p_data, data_len);
	return bt_mesh_msg_send(model, ctx, &buf_rsp);
}

static int datatrans_write_status_receive(const struct bt_mesh_model *model,
										  struct bt_mesh_msg_ctx *ctx,
										  struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_datatrans_client_write_event_t p_data;
	p_data.src = ctx->addr;
	p_data.status = net_buf_simple_pull_u8(buf);
	p_data.written_len = net_buf_simple_pull_le16(buf);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_WRITE_STATUS,
								sizeof(rtk_bt_mesh_datatrans_client_write_event_t));
	memcpy(p_evt->data, &p_data, sizeof(rtk_bt_mesh_datatrans_client_write_event_t));

	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static int datatrans_read_data_rsp_receive(const struct bt_mesh_model *model,
										   struct bt_mesh_msg_ctx *ctx,
										   struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_datatrans_client_read_event_t *p_data;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DATATRANS_MODEL, RTK_BT_MESH_DATATRANS_MODEL_EVT_CLIENT_READ_RESULT,
								sizeof(rtk_bt_mesh_datatrans_client_read_event_t));
	p_data = (rtk_bt_mesh_datatrans_client_read_event_t *)p_evt->data;
	p_data->src = ctx->addr;
	p_data->data_len = buf->len;
	for (int i = 0; i < p_data->data_len; i++) {
		p_data->data[i] = net_buf_simple_pull_u8(buf);
	}
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

const struct bt_mesh_model_op datatrans_op[] = {
	{ OP_DATATRANS_WRITE_UNACK,     BT_MESH_LEN_MIN(0),     datatrans_write_unack_receive },
	{ OP_DATATRANS_WRITE,           BT_MESH_LEN_MIN(0),     datatrans_write_receive },
	{ OP_DATATRANS_READ,            BT_MESH_LEN_EXACT(2),   datatrans_read_data_receive },
	{ OP_DATATRANS_STATUS,          BT_MESH_LEN_EXACT(3),   datatrans_write_status_receive },
	{ OP_DATATRANS_DATA,            BT_MESH_LEN_MIN(0),     datatrans_read_data_rsp_receive },
	BT_MESH_MODEL_OP_END,
};

static int datatrans_write(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, uint16_t data_len, uint8_t *data, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	if (ack) {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_DATATRANS_WRITE, data_len);
		bt_mesh_model_msg_init(&buf, OP_DATATRANS_WRITE);
		net_buf_simple_add_mem(&buf, data, data_len);
		return bt_mesh_msg_send(model, &ctx, &buf);
	} else {
		BT_MESH_MODEL_BUF_DEFINE(buf, OP_DATATRANS_WRITE_UNACK, data_len);
		bt_mesh_model_msg_init(&buf, OP_DATATRANS_WRITE_UNACK);
		net_buf_simple_add_mem(&buf, data, data_len);
		return bt_mesh_msg_send(model, &ctx, &buf);
	}
	// net_buf_simple_add_mem(&buf, data, data_len);
	// return bt_mesh_msg_send(model, &ctx, &buf);
}

static int datatrans_read(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index, uint16_t read_len)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_DATATRANS_READ, 2);
	bt_mesh_model_msg_init(&buf, OP_DATATRANS_READ);
	net_buf_simple_add_le16(&buf, read_len);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_datatrans_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	if (!p_cmd) {
		BT_LOGE("[%s] param is NULL!\r\n", __func__);
		return RTK_BT_FAIL;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_DATATRANS_MODEL_ACT_WRITE: {
		rtk_bt_mesh_datatrans_client_write_api_t *data_write = (rtk_bt_mesh_datatrans_client_write_api_t *)p_cmd->param;
		ret = datatrans_write(datatrans_model, data_write->dst, data_write->app_key_index, data_write->data_len, data_write->data, data_write->ack);
	}
	break;
	case RTK_BT_MESH_DATATRANS_MODEL_ACT_READ: {
		rtk_bt_mesh_datatrans_client_read_api_t *data_read = (rtk_bt_mesh_datatrans_client_read_api_t *)p_cmd->param;
		ret = datatrans_read(datatrans_model, data_read->dst, data_read->app_key_index, data_read->read_len);
	}
	break;
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