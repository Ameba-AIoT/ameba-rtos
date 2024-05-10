/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_property_client.c
* @brief    Source file for generic property client model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2019-07-03
* @version  v1.0
* *************************************************************************************
*/
#include "generic_property.h"

static mesh_msg_send_cause_t generic_property_client_send(const mesh_model_info_p pmodel_info,
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

mesh_msg_send_cause_t generic_user_properties_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                  uint16_t app_key_index)
{
    generic_user_properties_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_USER_PROPERTIES_GET);
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_user_property_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint16_t property_id)
{
    generic_user_property_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_USER_PROPERTY_GET);
    msg.property_id = property_id;
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_user_property_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                uint16_t app_key_index, uint16_t property_id,
                                                const uint8_t *pvalue, uint8_t value_len, bool ack)
{
    generic_user_property_set_t *pmsg;
    uint16_t msg_len = sizeof(generic_user_property_set_t) + value_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    if (ack)
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_GENERIC_USER_PROPERTY_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_GENERIC_USER_PROPERTY_SET_UNACK);
    }
    pmsg->property_id = property_id;
    memcpy(pmsg->property_value, pvalue, value_len);
    mesh_msg_send_cause_t ret = generic_property_client_send(pmodel_info, dst, app_key_index,
                                                             (uint8_t *)pmsg, msg_len);

    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t generic_admin_properties_get(const mesh_model_info_p pmodel_info,
                                                   uint16_t dst,
                                                   uint16_t app_key_index)
{
    generic_admin_properties_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ADMIN_PROPERTIES_GET);
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_admin_property_get(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, uint16_t property_id)
{
    generic_admin_property_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_ADMIN_PROPERTY_GET);
    msg.property_id = property_id;
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_admin_property_set(const mesh_model_info_p pmodel_info, uint16_t dst,
                                                 uint16_t app_key_index, uint16_t property_id, uint8_t property_access,
                                                 const uint8_t *pvalue, uint8_t value_len, bool ack)
{
    generic_admin_property_set_t *pmsg;
    uint16_t msg_len = sizeof(generic_admin_property_set_t) + value_len;
    pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    if (ack)
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_GENERIC_ADMIN_PROPERTY_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_GENERIC_ADMIN_PROPERTY_SET_UNACK);
    }

    pmsg->property_id = property_id;
    pmsg->property_access = property_access;
    memcpy(pmsg->property_value, pvalue, value_len);
    mesh_msg_send_cause_t ret = generic_property_client_send(pmodel_info, dst, app_key_index,
                                                             (uint8_t *)pmsg, msg_len);

    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t generic_manufacturer_properties_get(const mesh_model_info_p pmodel_info,
                                                          uint16_t dst,
                                                          uint16_t app_key_index)
{
    generic_manufacturer_properties_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MANUFACTURER_PROPERTIES_GET);
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_manufacturer_property_get(const mesh_model_info_p pmodel_info,
                                                        uint16_t dst,
                                                        uint16_t app_key_index, uint16_t property_id)
{
    generic_manufacturer_property_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MANUFACTURER_PROPERTY_GET);
    msg.property_id = property_id;
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t generic_manufacturer_property_set(const mesh_model_info_p pmodel_info,
                                                        uint16_t dst,
                                                        uint16_t app_key_index, uint16_t property_id, uint8_t property_access,
                                                        bool ack)
{
    generic_manufacturer_property_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MANUFACTURER_PROPERTY_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_MANUFACTURER_PROPERTY_SET_UNACK);
    }

    msg.property_id = property_id;
    msg.property_access = property_access;
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg,
                                        sizeof(generic_manufacturer_property_set_t));
}

mesh_msg_send_cause_t generic_client_properties_get(const mesh_model_info_p pmodel_info,
                                                    uint16_t dst,
                                                    uint16_t app_key_index, uint16_t property_id)
{
    generic_client_properties_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_GENERIC_CLIENT_PROPERTIES_GET);
    msg.property_id = property_id;
    return generic_property_client_send(pmodel_info, dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool generic_property_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_GENERIC_USER_PROPERTIES_STATUS:
        {
            uint16_t value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_user_properties_status_t,
                                                                    property_ids);
            generic_user_properties_status_t *pmsg = (generic_user_properties_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_properties_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.pproperty_ids = NULL;
                status_data.num_ids = 0;
			if (value_len > 0) {
				// status_data.pproperty_ids = pmsg->property_ids;
				void *pointer = (uint8_t *)pmsg + 1;
				status_data.pproperty_ids = (uint16_t *)pointer;
                    status_data.num_ids = value_len / 2;
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_USER_PROPERIES_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_USER_PROPERTY_STATUS:
        {
            generic_user_property_status_t *pmsg = (generic_user_property_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_property_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.property_id = pmsg->property_id;
                status_data.property_access = 0;
                status_data.pproperty_value = NULL;
                status_data.value_len = 0;
                if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_user_property_status_t, property_access))
                {
                    status_data.property_access = pmsg->property_access;
                    if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_user_property_status_t, property_value))
                    {
                        status_data.pproperty_value = pmsg->property_value;
                        status_data.value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_user_property_status_t,
                                                                                   property_value);
                    }
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_USER_PROPERTY_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_ADMIN_PROPERTIES_STATUS:
        {
            uint16_t value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_admin_properties_status_t,
                                                                    property_ids);
            generic_admin_properties_status_t *pmsg = (generic_admin_properties_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_properties_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.pproperty_ids = NULL;
                status_data.num_ids = 0;
			if (value_len > 0) {
				// status_data.pproperty_ids = pmsg->property_ids;
				void *pointer = (uint8_t *)pmsg + 1;
				status_data.pproperty_ids = (uint16_t *)pointer;
                    status_data.num_ids = value_len / 2;
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_ADMIN_PROPERTIES_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_ADMIN_PROPERTY_STATUS:
        {
            generic_admin_property_status_t *pmsg = (generic_admin_property_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_property_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.property_id = pmsg->property_id;
                status_data.property_access = 0;
                status_data.pproperty_value = NULL;
                status_data.value_len = 0;
                if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_admin_property_status_t, property_access))
                {
                    status_data.property_access = pmsg->property_access;
                    if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_admin_property_status_t, property_value))
                    {
                        status_data.pproperty_value = pmsg->property_value;
                        status_data.value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_admin_property_status_t,
                                                                                   property_value);
                    }
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_ADMIN_PROPERTY_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_MANUFACTURER_PROPERTIES_STATUS:
        {
            uint16_t value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_manufacturer_properties_status_t,
                                                                    property_ids);
            generic_manufacturer_properties_status_t *pmsg = (generic_manufacturer_properties_status_t *)
                                                             pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_properties_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.pproperty_ids = NULL;
                status_data.num_ids = 0;
			if (value_len > 0) {
				// status_data.pproperty_ids = pmsg->property_ids;
				void *pointer = (uint8_t *)pmsg + 1;
				status_data.pproperty_ids = (uint16_t *)pointer;
                    status_data.num_ids = value_len / 2;
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_MANUFACTURER_PROPERTIES_CLIENT_STATUS,
                                           &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_MANUFACTURER_PROPERTY_STATUS:
        {
            generic_manufacturer_property_status_t *pmsg = (generic_manufacturer_property_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_property_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.property_id = pmsg->property_id;
                status_data.property_access = 0;
                status_data.pproperty_value = NULL;
                status_data.value_len = 0;
                if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_manufacturer_property_status_t, property_access))
                {
                    status_data.property_access = pmsg->property_access;
                    if (pmesh_msg->msg_len > MEMBER_OFFSET(generic_manufacturer_property_status_t, property_value))
                    {
                        status_data.pproperty_value = pmsg->property_value;
                        status_data.value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_manufacturer_property_status_t,
                                                                                   property_value);
                    }
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_MANUFACTURER_PROPERTY_CLIENT_STATUS, &status_data);
            }
        }
        break;
    case MESH_MSG_GENERIC_CLIENT_PROPERTIES_STATUS:
        {
            uint16_t value_len = pmesh_msg->msg_len - MEMBER_OFFSET(generic_client_properties_status_t,
                                                                    property_ids);
            generic_client_properties_status_t *pmsg = (generic_client_properties_status_t *)pbuffer;
            if (NULL != pmodel_info->model_data_cb)
            {
                generic_properties_client_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.pproperty_ids = NULL;
                status_data.num_ids = 0;
			if (value_len > 0) {
				// status_data.pproperty_ids = pmsg->property_ids;
				void *pointer = (uint8_t *)pmsg + 1;
				status_data.pproperty_ids = (uint16_t *)pointer;
                    status_data.num_ids = value_len / 2;
                }
                pmodel_info->model_data_cb(pmodel_info, GENERIC_CLIENT_PROPERTIES_CLIENT_STATUS, &status_data);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }

    return ret;
}

bool generic_property_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_GENERIC_PROPERTY_CLIENT;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->model_receive = generic_property_client_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("generic_location_client_reg: missing data process callback!");
        }
    }
    return mesh_model_reg(element_index, pmodel_info);
}

