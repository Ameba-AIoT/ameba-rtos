/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     ali_model.c
* @brief    Source file for ali model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2019-3-25
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "ali_model.h"

typedef struct
{
    uint8_t tid_rx;
    uint8_t tid_tx;
} ali_model_ctx_t;

mesh_msg_send_cause_t ali_attr_get(mesh_model_info_t *pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint8_t tid, ali_attr_type_t attr_type[], uint8_t type_num)
{
    mesh_msg_send_cause_t ret;
    ali_attr_get_t *pmsg;
    uint16_t msg_len = MEMBER_OFFSET(ali_attr_get_t, attr_type) + type_num * sizeof(ali_attr_type_t);
    pmsg = (ali_attr_get_t *)plt_malloc(msg_len, RAM_TYPE_DATA_OFF);
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_ALI_ATTR_GET);
    pmsg->tid = tid;
    memcpy(pmsg->attr_type, attr_type, type_num * sizeof(ali_attr_type_t));

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    ret = access_send(&mesh_msg);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

mesh_msg_send_cause_t ali_attr_conf(mesh_model_info_t *pmodel_info, const mesh_msg_t *pmesh_msg,
                                    uint8_t tid)
{
    ali_attr_conf_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_ALI_ATTR_CONF);
    msg.tid = tid;

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = sizeof(msg);
    mesh_msg.dst = pmesh_msg->dst;
    mesh_msg.app_key_index = pmesh_msg->app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t ali_attr_msg_raw(mesh_model_info_t *pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, uint32_t ali_opcode, uint8_t tid, uint8_t raw_data[], uint16_t data_len)
{
    mesh_msg_send_cause_t ret;
    ali_attr_set_t *pmsg;
    uint16_t msg_len = MEMBER_OFFSET(ali_attr_set_t, attr_type) + data_len;
    pmsg = (ali_attr_set_t *)plt_malloc(msg_len, RAM_TYPE_DATA_OFF);
    ACCESS_OPCODE_BYTE(pmsg->opcode, ali_opcode);
    pmsg->tid = tid;
    memcpy(&pmsg->attr_type, raw_data, data_len);

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    ret = access_send(&mesh_msg);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

static uint16_t ali_attr_len(ali_attr_t attr[1], uint16_t attr_num)
{
    uint16_t len = 0;
    for (uint16_t loop = 0; loop < attr_num; loop++)
    {
        len += sizeof(ali_attr_type_t) + attr[loop].param_len;
    }
    return len;
}

mesh_msg_send_cause_t ali_attr_msg(mesh_model_info_t *pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint32_t ali_opcode, uint8_t tid, ali_attr_t attr[1], uint16_t attr_num)
{
    mesh_msg_send_cause_t ret;
    ali_attr_set_t *pmsg;
    uint16_t msg_len = MEMBER_OFFSET(ali_attr_set_t, attr_type) + ali_attr_len(attr, attr_num);
    pmsg = (ali_attr_set_t *)plt_malloc(msg_len, RAM_TYPE_DATA_OFF);
    ACCESS_OPCODE_BYTE(pmsg->opcode, ali_opcode);
    pmsg->tid = tid;
    uint8_t *pdata = (uint8_t *)&pmsg->attr_type;
    for (uint16_t loop = 0; loop < attr_num; loop++)
    {
        LE_WORD2EXTRN(pdata, attr[loop].attr_type);
        pdata += sizeof(ali_attr_type_t);
        memcpy(pdata, attr[loop].attr_param, attr[loop].param_len);
        pdata += attr[loop].param_len;
    }

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    ret = access_send(&mesh_msg);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

mesh_msg_send_cause_t ali_transparent_msg(mesh_model_info_t *pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint8_t transparent_msg[1], uint16_t msg_len)
{
    mesh_msg_send_cause_t ret;
    ali_transparent_msg_t *pmsg;
    pmsg = (ali_transparent_msg_t *)plt_malloc(msg_len + MEMBER_OFFSET(ali_transparent_msg_t,
                                                                       transparent_msg), RAM_TYPE_DATA_OFF);
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_ALI_TRANSPARENT_MSG);
    memcpy(pmsg->transparent_msg, transparent_msg, msg_len);
    msg_len += MEMBER_OFFSET(ali_transparent_msg_t, transparent_msg);

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    ret = access_send(&mesh_msg);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

bool ali_model_reg(uint8_t element_index, mesh_model_info_p pmodel_info, bool server)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }
    /* ali model server & client can't exist at the same element due to the transparent msg */
    if (NULL != mesh_model_info_get_by_model_id(element_index,
                                                server ? MESH_MODEL_ALI_VENDOR_CLIENT : MESH_MODEL_ALI_VENDOR_SERVER))
    {
        return FALSE;
    }
    pmodel_info->model_id = server ? MESH_MODEL_ALI_VENDOR_SERVER : MESH_MODEL_ALI_VENDOR_CLIENT;
    return mesh_model_reg(element_index, pmodel_info);
}

