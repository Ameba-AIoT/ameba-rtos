/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     datatrans_model_client.c
* @brief    Source file for data transmission client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-10-29
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "datatrans_model.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif

#if defined(CONFIG_BT_MESH_TEST) && CONFIG_BT_MESH_TEST
#include "bt_mesh_device_test.h"
#include "bt_mesh_receive_response.h"
#endif

#if 0
typedef struct {
	uint8_t tid;
#if MODEL_ENABLE_DELAY_MSG_RSP
	uint32_t delay_pub_time;
#endif
} datatrans_info_t;
#endif

static mesh_msg_send_cause_t datatrans_server_send(const mesh_model_info_p pmodel_info,
		uint16_t dst, uint16_t app_key_index,
		uint8_t *pmsg, uint16_t msg_len, uint32_t delay_time)
{
	mesh_msg_t mesh_msg;
	mesh_msg.pmodel_info = pmodel_info;
	access_cfg(&mesh_msg);
	mesh_msg.pbuffer = pmsg;
	mesh_msg.msg_len = msg_len;
	if (0 != dst) {
		mesh_msg.dst = dst;
		mesh_msg.app_key_index = app_key_index;
	}
	mesh_msg.delay_time = delay_time;
	return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t datatrans_send_data(const mesh_model_info_p pmodel_info,
		uint16_t dst, uint16_t app_key_index,
		uint16_t data_len, uint8_t *data, uint32_t delay_time)
{
	mesh_msg_send_cause_t ret;
	datatrans_data_t *pmsg;
	uint16_t msg_len = sizeof(datatrans_data_t);
	msg_len += data_len;
	pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
	if (NULL == pmsg) {
		return MESH_MSG_SEND_CAUSE_NO_MEMORY;
	}

	ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_DATATRANS_DATA);
	memcpy(pmsg->data, data, data_len);

	ret = datatrans_server_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len, delay_time);
	plt_free(pmsg, RAM_TYPE_DATA_ON);

	return ret;
}

mesh_msg_send_cause_t datatrans_publish(const mesh_model_info_p pmodel_info,
										uint16_t data_len, uint8_t *data)
{
	mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
	if (mesh_model_pub_check(pmodel_info)) {
		uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
		delay_rsp_time = delay_msg_get_rsp_delay(0);
#endif
		ret = datatrans_send_data(pmodel_info, 0, 0, data_len, data, delay_rsp_time);
	}

	return ret;
}

static mesh_msg_send_cause_t datatrans_client_send(const mesh_model_info_p pmodel_info,
		uint16_t dst, uint16_t app_key_index,
		uint8_t *pmsg, uint16_t msg_len)
{
	mesh_msg_t mesh_msg;
	mesh_msg.pmodel_info = pmodel_info;
	access_cfg(&mesh_msg);
	mesh_msg.pbuffer = pmsg;
	mesh_msg.msg_len = msg_len;
	mesh_msg.dst = dst;
	mesh_msg.app_key_index = app_key_index;
	return access_send(&mesh_msg);
}

mesh_msg_send_cause_t datatrans_write(const mesh_model_info_p pmodel_info, uint16_t dst,
									  uint16_t app_key_index, uint16_t data_len, uint8_t *data,
									  bool ack)
{
	datatrans_write_t *pmsg;
	mesh_msg_send_cause_t ret;
	uint16_t msg_len = sizeof(datatrans_write_t);
	msg_len += data_len;
	pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
	if (NULL == pmsg) {
		return MESH_MSG_SEND_CAUSE_NO_MEMORY;
	}

	if (ack) {
		ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_DATATRANS_WRITE);
	} else {
		ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_DATATRANS_WRITE_UNACK);
	}

	memcpy(pmsg->data, data, data_len);
	ret = datatrans_client_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
	plt_free(pmsg, RAM_TYPE_DATA_ON);

	return ret;
}

mesh_msg_send_cause_t datatrans_read(const mesh_model_info_p pmodel_info, uint16_t dst,
									 uint16_t app_key_index, uint16_t read_len)
{
	datatrans_read_t msg;
	ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DATATRANS_READ);
	msg.read_len = read_len;
	return datatrans_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
								 sizeof(datatrans_read_t));
}

static bool datatrans_receive(mesh_msg_p pmesh_msg)
{
	bool ret = TRUE;
	uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
	mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

	switch (pmesh_msg->access_opcode) {
	case MESH_MSG_DATATRANS_STATUS:
		if (pmesh_msg->msg_len == sizeof(datatrans_status_t)) {
			datatrans_status_t *pmsg = (datatrans_status_t *)pbuffer;

			if (NULL != pmesh_msg->pmodel_info->model_data_cb) {
				datatrans_client_status_t status_data;
				status_data.src = pmesh_msg->src;
				status_data.status = pmsg->status;
				status_data.written_len = pmsg->written_len;
				pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, DATATRANS_CLIENT_STATUS,
													  &status_data);
			}
		}
		break;
	case MESH_MSG_DATATRANS_DATA: {
		datatrans_data_t *pmsg = (datatrans_data_t *)pbuffer;
		if (NULL != pmesh_msg->pmodel_info->model_data_cb) {
			datatrans_client_data_t read_data;
			read_data.src = pmesh_msg->src;
			read_data.data_len = pmesh_msg->msg_len - sizeof(datatrans_data_t);
			read_data.data = pmsg->data;
			pmesh_msg->pmodel_info->model_data_cb(pmesh_msg->pmodel_info, DATATRANS_CLIENT_DATA,
												  &read_data);
		}
	}
	break;
	case MESH_MSG_DATATRANS_READ:
		if (pmesh_msg->msg_len == sizeof(datatrans_read_t)) {
			datatrans_read_t *pmsg = (datatrans_read_t *)pbuffer;
			datatrans_server_read_t read_data = {0, NULL};
			if (NULL != pmodel_info->model_data_cb) {
				read_data.data_len = pmsg->read_len;
				pmodel_info->model_data_cb(pmodel_info, DATATRANS_SERVER_READ, &read_data);
			}
			uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
			delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
			datatrans_send_data(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
								read_data.data_len, read_data.data, delay_rsp_time);
		}
		break;
	case MESH_MSG_DATATRANS_WRITE:
	case MESH_MSG_DATATRANS_WRITE_UNACK: {
		datatrans_write_t *pmsg = (datatrans_write_t *)pbuffer;
		uint16_t data_len = pmesh_msg->msg_len - sizeof(datatrans_write_t);

#if defined(CONFIG_BT_MESH_TEST) && CONFIG_BT_MESH_TEST
		datatrans_server_write_t write_data = {data_len, NULL, pmesh_msg->src, DATATRANS_SUCCESS, data_len};
#else
		datatrans_server_write_t write_data = {data_len, NULL, DATATRANS_SUCCESS, data_len};
#endif

		if (NULL != pmodel_info->model_data_cb) {
			write_data.data = pmsg->data;
			pmodel_info->model_data_cb(pmodel_info, DATATRANS_SERVER_WRITE, &write_data);
		}

		if (pmesh_msg->access_opcode == MESH_MSG_DATATRANS_WRITE) {
			datatrans_status_t msg;
			ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DATATRANS_STATUS);
			msg.status = write_data.status;
			msg.written_len = write_data.written_len;
			uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
			delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
			datatrans_server_send(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, (uint8_t *)&msg,
								  sizeof(datatrans_status_t), delay_rsp_time);
		}
	}
	break;
	default:
		ret = FALSE;
		break;
	}
	return ret;
}

#if MESH_MODEL_ENABLE_DEINIT
static void datatrans_deinit(mesh_model_info_t *pmodel_info)
{
	if (pmodel_info->model_receive == datatrans_receive) {
#if 0
		/* now we can remove */
		if (NULL != pmodel_info->pargs) {
			plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
			pmodel_info->pargs = NULL;
		}
#endif
		pmodel_info->model_receive = NULL;
	}
}
#endif

bool datatrans_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
	if (NULL == pmodel_info) {
		return FALSE;
	}

	pmodel_info->model_id = MESH_MODEL_DATATRANS;
	if (NULL == pmodel_info->model_receive) {
#if 0
		pmodel_info->pargs = plt_malloc(sizeof(datatrans_info_t), RAM_TYPE_DATA_ON);
		if (NULL == pmodel_info->pargs) {
			printe("datatrans_reg: fail to allocate memory for the new model extension data!");
			return FALSE;
		}
		memset(pmodel_info->pargs, 0, sizeof(datatrans_info_t));
#endif
		pmodel_info->model_receive = datatrans_receive;
		if (NULL == pmodel_info->model_data_cb) {
			printw("datatrans_reg: missing model data process callback!");
		}
#if MESH_MODEL_ENABLE_DEINIT
		pmodel_info->model_deinit = datatrans_deinit;
#endif
	}
	return mesh_model_reg(element_index, pmodel_info);
}

