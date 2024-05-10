/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     datatrans_model_server.c
* @brief    Source file for data transmission server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-10-29
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "datatrans_model.h"


static mesh_msg_send_cause_t datatrans_server_send(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst, uint16_t app_key_index,
                                                   uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t datatrans_send_data(const mesh_model_info_p pmodel_info,
                                                 uint16_t dst, uint16_t app_key_index,
                                                 uint16_t data_len, uint8_t *data)
{
    mesh_msg_send_cause_t ret;
    datatrans_data_t *pmsg;
    uint16_t msg_len = sizeof(datatrans_data_t);
    msg_len += data_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_DATATRANS_DATA);
    memcpy(pmsg->data, data, data_len);

    ret = datatrans_server_send(pmodel_info, dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t datatrans_publish(const mesh_model_info_p pmodel_info,
                                        uint16_t data_len, uint8_t *data)
{
    mesh_msg_send_cause_t ret = MESH_MSG_SEND_CAUSE_INVALID_DST;
    if (mesh_model_pub_check(pmodel_info))
    {
        ret = datatrans_send_data(pmodel_info, 0, 0, data_len, data);
    }

    return ret;
}

static bool datatrans_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_DATATRANS_READ:
        if (pmesh_msg->msg_len == sizeof(datatrans_read_t))
        {
            datatrans_read_t *pmsg = (datatrans_read_t *)pbuffer;
            datatrans_server_read_t read_data = {0, NULL};
            if (NULL != pmodel_info->model_data_cb)
            {
                read_data.data_len = pmsg->read_len;
                pmodel_info->model_data_cb(pmodel_info, DATATRANS_SERVER_READ, &read_data);
            }
            datatrans_send_data(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                read_data.data_len, read_data.data);
        }
        break;
    case MESH_MSG_DATATRANS_WRITE:
    case MESH_MSG_DATATRANS_WRITE_UNACK:
        {
            datatrans_write_t *pmsg = (datatrans_write_t *)pbuffer;
            uint16_t data_len = pmesh_msg->msg_len - sizeof(datatrans_write_t);
            datatrans_server_write_t write_data = {data_len, NULL, DATATRANS_SUCCESS, data_len};
            if (NULL != pmodel_info->model_data_cb)
            {
                write_data.data = pmsg->data;
                pmodel_info->model_data_cb(pmodel_info, DATATRANS_SERVER_WRITE, &write_data);
            }

            if (pmesh_msg->access_opcode == MESH_MSG_DATATRANS_WRITE)
            {
                datatrans_status_t msg;
                ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_DATATRANS_STATUS);
                msg.status = write_data.status;
                msg.written_len = write_data.written_len;
                datatrans_server_send(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index, (uint8_t *)&msg,
                                      sizeof(datatrans_status_t));
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool datatrans_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_DATATRANS_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = datatrans_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("datatrans server reg: missing model data process callback!");
        }
    }

    return mesh_model_reg(element_index, pmodel_info);
}

