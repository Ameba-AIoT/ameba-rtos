/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     subnet_bridge_client.c
* @brief    Source file for subnet bridge model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2021-01-12
* @version  v1.0
* *************************************************************************************
*/
#include "mesh_config.h"
#include "subnet_bridge_model.h"
#include "app_mesh_flags.h"

#if F_BT_MESH_1_1_SBR_SUPPORT

mesh_model_info_t subnet_bridge_client_model;


static mesh_msg_send_cause_t subnet_bridge_client_send(uint16_t dst, uint16_t net_key_index,
                                                       uint8_t *pmsg, uint16_t msg_len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = &subnet_bridge_client_model;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.akf = 0;
    mesh_msg.net_key_index = net_key_index;
    mesh_msg.dst = dst;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t subnet_bridge_get(uint16_t dst, uint16_t net_key_index)
{
    subnet_bridge_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SUBNET_BRIDGE_GET);

    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t subnet_bridge_set(uint16_t dst, uint16_t net_key_index,
                                        subnet_bridge_state_t state)
{
    subnet_bridge_set_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_SUBNET_BRIDGE_SET);
    msg.state = state;

    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t bridging_table_add(uint16_t dst, uint16_t net_key_index, uint8_t directions,
                                         uint16_t net_key_index1, uint16_t net_key_index2, uint16_t addr1, uint16_t addr2)
{
    bridging_table_add_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BRIDGING_TABLE_ADD);
    msg.directions = directions;
    msg.net_key_index1 = net_key_index1;
    msg.net_key_index2 = net_key_index2;
    msg.addr1 = addr1;
    msg.addr2 = addr2;

    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t bridging_table_remove(uint16_t dst, uint16_t net_key_index,
                                            uint16_t net_key_index1, uint16_t net_key_index2, uint16_t addr1, uint16_t addr2)
{
    bridging_table_remove_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BRIDGING_TABLE_REMOVE);
    msg.net_key_index[0] = net_key_index1;
    msg.net_key_index[1] = (net_key_index1 >> 8) & 0x0f;
    msg.net_key_index[1] += ((net_key_index2 & 0x0f) << 4);
    msg.net_key_index[2] = (net_key_index2 >> 4) & 0xff;
    msg.addr1 = addr1;
    msg.addr2 = addr2;

    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t bridged_subnets_get(uint16_t dst, uint16_t net_key_index,
                                          subnets_filter_t filter, uint16_t filter_net_key_index, uint8_t start_index)
{
    bridged_subnets_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BRIDGED_SUBNETS_GET);
    msg.filter = filter;
    msg.prohibited = 0;
    msg.net_key_index = filter_net_key_index;
    msg.start_index = start_index;
    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t bridging_table_get(uint16_t dst, uint16_t net_key_index,
                                         uint16_t net_key_index1, uint16_t net_key_index2, uint16_t start_index)
{
    bridging_table_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BRIDGING_TABLE_GET);
    msg.net_key_index[0] = net_key_index1;
    msg.net_key_index[1] = (net_key_index1 >> 8) & 0x0f;
    msg.net_key_index[1] += ((net_key_index2 & 0x0f) << 4);
    msg.net_key_index[2] = (net_key_index2 >> 4) & 0xff;
    msg.start_index = start_index;
    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t bridging_table_size_get(uint16_t dst, uint16_t net_key_index)
{
    bridging_table_size_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BRIDGING_TABLE_SIZE_GET);
    return subnet_bridge_client_send(dst, net_key_index, (uint8_t *)&msg, sizeof(msg));
}

static bool subnet_bridge_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    //mesh_model_info_t *pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SUBNET_BRIDGE_STATUS:
        if (pmesh_msg->msg_len == sizeof(subnet_bridge_status_t))
        {
            subnet_bridge_status_t *pmsg = (subnet_bridge_status_t *)pbuffer;
            data_uart_debug("subnet_bridge_status: state %d\r\n", pmsg->state);
        }
        break;
    case MESH_MSG_BRIDGING_TABLE_STATUS:
        if (pmesh_msg->msg_len == sizeof(bridging_table_status_t))
        {
            bridging_table_status_t *pmsg = (bridging_table_status_t *)pbuffer;
            data_uart_debug("bridging_table_status: status%d, directions %d, net_key_index1 0x%04x, net_key_index2 0x%04x, addr1 0x%04x, addr2 0x%04x\r\n",
                            pmsg->status, pmsg->directions, pmsg->net_key_index1, pmsg->net_key_index2, pmsg->addr1,
                            pmsg->addr2);
        }
        break;
    case MESH_MSG_BRIDGED_SUBNETS_LIST:
        {
            bridged_subnets_list_t *pmsg = (bridged_subnets_list_t *)pbuffer;
            data_uart_debug("bridged_subnets_list: filter %d, net_key_index 0x%04x, start_index %d\r\n",
                            pmsg->filter, pmsg->net_key_index, pmsg->start_index);
            uint16_t subnets_len = pmesh_msg->msg_len - sizeof(bridged_subnets_list_t);
            uint16_t net_key_index1, net_key_index2;
            uint8_t *psubnets = pmsg->bridged_subnets_list;
            for (uint16_t i = 0; i < subnets_len;)
            {
                net_key_index1 = LE_EXTRN2WORD(psubnets + i) & 0x0fff;
                net_key_index2 = (LE_EXTRN2WORD(psubnets + i + 1) >> 4);
                data_uart_debug("net_key_index1 %d, net_key_index2 %d\r\n", net_key_index1, net_key_index2);
                i += 3;
            }
        }
        break;
    case MESH_MSG_BRIDGING_TABLE_LIST:
        {
            bridging_table_list_t *pmsg = (bridging_table_list_t *)pbuffer;
            data_uart_debug("bridging_table_list: status %d, net_key_index1 0x%04x, net_key_index2 0x%04x, start_index %d, bridged_addrs_list \r\n",
                            pmsg->status, pmsg->net_key_index1, pmsg->net_key_index2, pmsg->start_index);
            uint16_t addrs_len = pmesh_msg->msg_len - sizeof(bridging_table_list_t);
            bridged_address_entry_t *pentry = (bridged_address_entry_t *)pmsg->bridged_addrs_list;
            for (uint16_t i = 0; i < addrs_len;)
            {
                data_uart_debug("addr1 0x%04x, addr2 0x%04x, directions %d\r\n", pentry->addr1, pentry->addr2,
                                pentry->directions);
                i += sizeof(bridged_address_entry_t);
                pentry += 1;
            }
        }
        break;
    case MESH_MSG_BRIDGING_TABLE_SIZE_STATUS:
        if (pmesh_msg->msg_len == sizeof(bridging_table_size_status_t))
        {
            bridging_table_size_status_t *pmsg = (bridging_table_size_status_t *)pbuffer;
            data_uart_debug("bridging_table_size_status: table size %d\r\n", pmsg->bridging_table_size);
        }
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

bool subnet_bridge_client_reg(uint8_t element_index)
{
    subnet_bridge_client_model.model_id = MESH_MODEL_SUBNET_BRIDGE_CLIENT;
    if (NULL == subnet_bridge_client_model.model_receive)
    {
        subnet_bridge_client_model.model_receive = subnet_bridge_client_receive;
    }
    return mesh_model_reg(element_index, &subnet_bridge_client_model);
}
#endif
