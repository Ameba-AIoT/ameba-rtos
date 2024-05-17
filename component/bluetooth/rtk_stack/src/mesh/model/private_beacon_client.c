/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     private_beacon_client.c
* @brief    Source file for private beacon model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2020-08-04
* @version  v1.0
* *************************************************************************************
*/
#include "mesh_config.h"

#if MESH_PRB
#include "private_beacon_model.h"

mesh_model_info_t private_beacon_client_model;


static mesh_msg_send_cause_t private_beacon_client_send(const mesh_model_info_p pmodel_info,
                                                        uint16_t dst, uint16_t net_key_index, uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.akf = 0;
    mesh_msg.net_key_index = net_key_index;
    mesh_msg.dst = dst;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t private_beacon_get(uint16_t dst, uint16_t net_key_index)
{
    private_beacon_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_BEACON_GET);

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      sizeof(msg));
}

mesh_msg_send_cause_t private_beacon_set(uint16_t dst, uint16_t net_key_index,
                                         uint8_t private_beacon, bool has_random_update, uint8_t random_update_interval_steps)
{
    private_beacon_set_t msg;
    uint8_t msg_len = sizeof(private_beacon_set_t) - 1;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_BEACON_SET);
    msg.private_beacon = private_beacon;
    if (has_random_update)
    {
        msg.random_update_interval_steps = random_update_interval_steps;
        msg_len += 1;
    }

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      msg_len);
}

mesh_msg_send_cause_t private_gatt_proxy_get(uint16_t dst, uint16_t net_key_index)
{
    private_gatt_proxy_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_GATT_PROXY_GET);

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      sizeof(msg));
}

mesh_msg_send_cause_t private_gatt_proxy_set(uint16_t dst, uint16_t net_key_index,
                                             uint8_t private_gatt_proxy)
{
    private_gatt_proxy_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_GATT_PROXY_SET);
    msg.private_gatt_proxy = private_gatt_proxy;

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      sizeof(msg));
}

mesh_msg_send_cause_t private_node_identity_get(uint16_t dst, uint16_t net_key_index,
                                                uint16_t sub_net_key_index)
{
    private_node_identity_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_NODE_IDENTITY_GET);
    msg.net_key_index = sub_net_key_index;

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      sizeof(msg));
}

mesh_msg_send_cause_t private_node_identity_set(uint16_t dst, uint16_t net_key_index,
                                                uint16_t sub_net_key_index, uint8_t private_identity)
{
    private_node_identity_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PRIVATE_NODE_IDENTITY_SET);
    msg.net_key_index = sub_net_key_index;
    msg.private_identity = private_identity;

    return private_beacon_client_send(&private_beacon_client_model, dst, net_key_index, (uint8_t *)&msg,
                                      sizeof(msg));
}

static bool private_beacon_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    //mesh_model_info_t *pmodel_info = pmesh_msg->pmodel_info;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_PRIVATE_BEACON_STATUS:
        if (pmesh_msg->msg_len == sizeof(private_beacon_status_t))
        {
            private_beacon_status_t *pmsg = (private_beacon_status_t *)pbuffer;
            data_uart_debug("private_beacon_client_receive: private beacon %d, random update interval steps %d\r\n",
                            pmsg->private_beacon, pmsg->random_update_interval_steps);
        }
        break;
    case MESH_MSG_PRIVATE_GATT_PROXY_STATUS:
        if (pmesh_msg->msg_len == sizeof(private_gatt_proxy_status_t))
        {
            private_gatt_proxy_status_t *pmsg = (private_gatt_proxy_status_t *)pbuffer;
            data_uart_debug("private_beacon_client_receive: private gatt proxy %d\r\n",
                            pmsg->private_gatt_proxy);
        }
        break;
    case MESH_MSG_PRIVATE_NODE_IDENTITY_STATUS:
        if (pmesh_msg->msg_len == sizeof(private_node_identity_status_t))
        {
            private_node_identity_status_t *pmsg = (private_node_identity_status_t *)pbuffer;
            data_uart_debug("private_beacon_client_receive: status %d, net_key_index 0x%04x, private identity %d\r\n",
                            pmsg->status, pmsg->net_key_index, pmsg->private_identity);
        }
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

bool private_beacon_client_reg(uint8_t element_index)
{
    private_beacon_client_model.model_id = MESH_MODEL_PRIVATE_BEACON_CLIENT;
    if (NULL == private_beacon_client_model.model_receive)
    {
        private_beacon_client_model.model_receive = private_beacon_client_receive;
    }
    return mesh_model_reg(element_index, &private_beacon_client_model);
}
#endif
