/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     test_cmd.h
  * @brief    Head file for test cmd.
  * @details  User command interfaces.
  * @author   bill
  * @date     2017-12-18
  * @version  v1.0
  * *************************************************************************************
  */

/* Add Includes here */
#include "test_cmd.h"
#include "mesh_api.h"

user_cmd_parse_result_t user_cmd_node_state_set(user_cmd_parse_value_t *pparse_value)
{
    mesh_node_state_t node_state = pparse_value->dw_parameter[0] == 0 ? UNPROV_DEVICE : PROV_NODE;
    mesh_node.node_state = node_state;
    data_uart_debug("Node State:\t%d\r\n", mesh_node.node_state);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_node_addr_set(user_cmd_parse_value_t *pparse_value)
{
    if (pparse_value->para_count != 1)
    {
        return USER_CMD_RESULT_WRONG_NUM_OF_PARAMETERS;
    }
    uint16_t addr = pparse_value->dw_parameter[0];
    mesh_node.unicast_addr = addr & 0x7fff;
    data_uart_debug("NodeAddr:\t0x%04x\r\n", mesh_node.unicast_addr);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_net_key_set(user_cmd_parse_value_t *pparse_value)
{
    uint16_t net_key_index = pparse_value->dw_parameter[0];
    uint16_t net_key_index_g = pparse_value->dw_parameter[1];
    uint8_t net_key[16];
    plt_hex_to_bin(net_key, (uint8_t *)pparse_value->pparameter[2], sizeof(net_key));
    net_key_update(net_key_index, net_key_index_g, net_key);
    data_uart_debug("NetKey:\t\t");
    data_uart_dump(mesh_node.net_key_list[net_key_index].pnet_key[key_state_to_tx_loop(
                                                                      mesh_node.net_key_list[net_key_index].key_state)]->net_key, 16);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_net_key_refresh(user_cmd_parse_value_t *pparse_value)
{
    uint16_t net_key_index = pparse_value->dw_parameter[0];
    uint8_t phase_old, phase_new;
    phase_old = key_state_to_key_refresh_phase(mesh_node.net_key_list[net_key_index].key_state);
    net_key_refresh(net_key_index);
    phase_new = key_state_to_key_refresh_phase(mesh_node.net_key_list[net_key_index].key_state);
    data_uart_debug("NetKey %d: phase %d -> %d\r\n", net_key_index, phase_old, phase_new);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_app_key_set(user_cmd_parse_value_t *pparse_value)
{
    uint16_t app_key_index = pparse_value->dw_parameter[0];
    uint16_t net_key_index = pparse_value->dw_parameter[1];
    uint16_t app_key_index_g = pparse_value->dw_parameter[2];
    uint8_t app_key[16];
    plt_hex_to_bin(app_key, (uint8_t *)pparse_value->pparameter[3], sizeof(app_key));
    app_key_update(app_key_index, net_key_index, app_key_index_g, app_key);
    data_uart_debug("AppKey:\t\t");
    data_uart_dump(mesh_node.app_key_list[app_key_index].papp_key[key_state_to_tx_loop(
                                                                      mesh_node.app_key_list[app_key_index].key_state)]->app_key, 16);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_dev_key_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t index = pparse_value->dw_parameter[0];
    uint16_t unicast_addr = pparse_value->dw_parameter[1];
    uint8_t element_num = pparse_value->dw_parameter[2];
    uint8_t dev_key[16];
    plt_hex_to_bin(dev_key, (uint8_t *)pparse_value->pparameter[3], sizeof(dev_key));
    dev_key_set(index, unicast_addr, element_num, dev_key);
    data_uart_debug("DevKey:\t\t%d-0x%04x-%d-", index, mesh_node.dev_key_list[index].unicast_addr,
                    mesh_node.dev_key_list[index].element_num);
    data_uart_dump(mesh_node.dev_key_list[index].dev_key, 16);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_seq_set(user_cmd_parse_value_t *pparse_value)
{
    uint32_t seq = pparse_value->dw_parameter[0];
    mesh_seq_set(seq);
    data_uart_debug("Seq:\t\t0x%06x\r\n", mesh_node.seq);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_iv_index_set(user_cmd_parse_value_t *pparse_value)
{
    uint32_t iv_index = pparse_value->dw_parameter[0];
    iv_index_set(iv_index);
    data_uart_debug("IVindex:\t0x%x\r\n", mesh_node.iv_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_iv_index_transit(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("IVindex:\t%d-0x%x\r\n", mesh_node.iv_update_flag, mesh_node.iv_index);
    if (pparse_value->dw_parameter[0])
    {
        iv_index_transit_to_iv_update();
    }
    else
    {
        iv_index_transit_to_normal();
    }
    data_uart_debug("IVindex:\t%d-0x%x\r\n", mesh_node.iv_update_flag, mesh_node.iv_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_iv_index_mode(user_cmd_parse_value_t *pparse_value)
{
    iv_index_test_mode_set(pparse_value->dw_parameter[0]);
    data_uart_debug("IVindex mode: %s\r\n", pparse_value->dw_parameter[0] ? "Test" : "Normal");
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_nmc_clear(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    nmc_init();
    tsmc_init();
    data_uart_debug("Net Msg Cache are cleared.\r\n");
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_rpl_clear(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    rpl_clear();
    rpl_clear();
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_subscribe(user_cmd_parse_value_t *pparse_value)
{
    uint8_t element_index = pparse_value->dw_parameter[0];
    uint8_t model_index = pparse_value->dw_parameter[1];
    uint16_t addr = pparse_value->dw_parameter[2];
    mesh_model_p pmodel = mesh_model_get(element_index, model_index);
    if (pmodel == NULL || MESH_NOT_SUBSCRIBE_ADDR(addr))
    {
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    else
    {
        mesh_model_sub(pmodel, addr);
        return USER_CMD_RESULT_OK;
    }
}

user_cmd_parse_result_t user_cmd_test_send(user_cmd_parse_value_t *pparse_value)
{
    uint16_t addr = pparse_value->dw_parameter[0];
    uint8_t ttl = pparse_value->dw_parameter[1];
    uint8_t len = pparse_value->dw_parameter[2];
    uint8_t app_msg[20];
    for (uint8_t loop = 0; loop < len && loop < sizeof(app_msg); loop++)
    {
        app_msg[loop] = loop;
    }
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = NULL;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = app_msg;
    mesh_msg.msg_len = len;
    mesh_msg.akf = pparse_value->dw_parameter[3] != 0xff;
    mesh_msg.app_key_index = pparse_value->dw_parameter[3];
    mesh_msg.dst = addr;
    mesh_msg.ttl = ttl;
    return access_send(&mesh_msg) == MESH_MSG_SEND_CAUSE_SUCCESS ? USER_CMD_RESULT_OK :
           USER_CMD_RESULT_ERROR;
}

user_cmd_parse_result_t user_cmd_test_data(user_cmd_parse_value_t *pparse_value)
{
    uint8_t test_case = pparse_value->dw_parameter[0];
    //uint8_t app_msg[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0, 0, 0, 0};
    uint8_t app_msg[] = {0x01, 0x02, 0, 0, 0, 0};
    uint8_t app_msg1[] = {0x80, 0x11, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0};
    uint8_t app_msg2[] = {0x80, 0x11, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0, 0, 0, 0};
    uint8_t app_msg3[] = {0x01, 0x02, 0, 0, 0, 0, 0, 0, 0, 0};
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = NULL;
    mesh_msg.msg_offset = 0;

    if (test_case == 3 || test_case == 4)
    {
        mesh_msg.pbuffer = app_msg1;
        mesh_msg.msg_len = sizeof(app_msg1);
        iv_index_set(0x01020304);
        mesh_seq_set(0x08090a);
    }
    else if (test_case == 5)
    {
        mesh_msg.pbuffer = app_msg2;
        mesh_msg.msg_len = sizeof(app_msg2);
        iv_index_set(0x01020304);
        mesh_seq_set(0x08090a);
    }
    else if (test_case == 6)
    {
        mesh_msg.pbuffer = app_msg3;
        mesh_msg.msg_len = sizeof(app_msg3);
        iv_index_set(0x01020304);
        mesh_seq_set(0x08090a);
    }
    else
    {
        mesh_msg.pbuffer = app_msg;
        mesh_msg.msg_len = sizeof(app_msg);
        iv_index_set(2);
        mesh_seq_set(0x010203);
    }

    access_cfg(&mesh_msg);
    if (test_case == 1)
    {
        mesh_msg.akf = 0;
    }

    if (test_case == 2)
    {
        mesh_msg.ttl = 6;
    }
    else if (test_case == 3 || test_case == 4)
    {
        mesh_msg.ttl = 0x1a;
    }
    else if (test_case == 5 || test_case == 6)
    {
        mesh_msg.ttl = 0x12;
    }
    else
    {
        mesh_msg.ttl = 7;
    }
    if (test_case == 2 || test_case == 3 || test_case == 4)
    {
        //mesh_msg.frnd = 1;
    }
    if (test_case == 3 || test_case == 4 || test_case == 5 || test_case == 6)
    {
        mesh_msg.src = 0x0b0c;
        mesh_msg.dst = 0x0d0e;
        mesh_msg.app_key_index = 1;
    }
    else
    {
        mesh_msg.src = 0x0405;
        mesh_msg.dst = 0x0607;
        mesh_msg.app_key_index = 0;
    }

    if (test_case == 6)
    {
        mesh_msg.szmic = 1;
        mesh_msg.ctl = 1;
    }
    access_send(&mesh_msg);
    return USER_CMD_RESULT_OK;
}

