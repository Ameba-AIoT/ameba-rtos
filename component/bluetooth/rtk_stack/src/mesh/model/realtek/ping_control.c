/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     ping_control.c
* @brief    Source file for ping control model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2016-3-24
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include <string.h>
#include "trace.h"
#include "ping.h"

mesh_model_info_t ping_control;

static pf_ping_cb_t pf_ping_app_ping_cb;
static pf_pong_cb_t pf_ping_control_pong_cb;

static mesh_msg_send_cause_t ping_control_send(uint16_t dst, uint8_t ttl, uint8_t *pmsg,
                                               uint16_t msg_len, uint16_t app_key_index)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = &ping_control;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.ttl = ttl;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t ping(uint16_t dst, uint8_t ttl, uint16_t app_key_index,
                           uint16_t pong_max_delay)
{
    ping_t msg;
    msg.init_ttl = ttl;
    msg.pong_max_delay = pong_max_delay;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PING);
    return ping_control_send(dst, ttl, (uint8_t *)&msg, sizeof(ping_t), app_key_index);
}

mesh_msg_send_cause_t big_ping(uint16_t dst, uint8_t ttl, uint16_t app_key_index,
                               uint16_t pong_max_delay)
{
    big_ping_p pmsg = (big_ping_p)plt_malloc(sizeof(big_ping_t), RAM_TYPE_DATA_OFF);
    if (pmsg == NULL)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    pmsg->init_ttl = ttl;
    pmsg->pong_max_delay = pong_max_delay;
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_BIG_PING);
    memset(pmsg->padding, 0, sizeof(pmsg->padding));
    mesh_msg_send_cause_t ret = ping_control_send(dst, ttl, (uint8_t *)pmsg, sizeof(big_ping_t),
                                                  app_key_index);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

mesh_msg_send_cause_t pong(uint16_t dst, uint8_t ttl, uint16_t app_key_index, uint8_t hops_forward,
                           uint16_t pong_delay)
{
    pong_t msg;
    msg.init_ttl = ttl;
    msg.hops_forward = hops_forward;
    msg.pong_delay = pong_delay;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_PONG);
    return ping_control_send(dst, ttl, (uint8_t *)&msg, sizeof(pong_t), app_key_index);
}

mesh_msg_send_cause_t big_pong(uint16_t dst, uint8_t ttl, uint16_t app_key_index,
                               uint8_t hops_forward, uint16_t pong_delay)
{
    big_pong_p pmsg = (big_pong_p)plt_malloc(sizeof(big_pong_t), RAM_TYPE_DATA_OFF);
    if (pmsg == NULL)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    pmsg->init_ttl = ttl;
    pmsg->hops_forward = hops_forward;
    pmsg->pong_delay = pong_delay;
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_BIG_PONG);
    memset(pmsg->padding, 0, sizeof(pmsg->padding));
    mesh_msg_send_cause_t ret = ping_control_send(dst, ttl, (uint8_t *)pmsg, sizeof(big_pong_t),
                                                  app_key_index);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

bool ping_control_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_PING:
        if (pmesh_msg->msg_len == sizeof(ping_t))
        {
            ping_t *pmsg = (ping_t *)(pbuffer);
            if (pmsg->init_ttl >= pmesh_msg->ttl)
            {
                uint8_t hops_forward = pmsg->init_ttl - pmesh_msg->ttl + 1;
                printi("ping_control_receive: [0x%04x -> 0x%04x]: %d hops!", pmesh_msg->src, pmesh_msg->dst,
                       hops_forward);
                if (pf_ping_app_ping_cb)
                {
                    pf_ping_app_ping_cb(pmesh_msg->src, pmesh_msg->dst, hops_forward, PING_PONG_TYPE_ACCESS,
                                        pmsg->init_ttl, pmesh_msg->app_key_index, pmsg->pong_max_delay);
                }
                else
                {
                    pong(pmesh_msg->src, pmsg->init_ttl, pmesh_msg->app_key_index, hops_forward, 0);
                }
            }
        }
        break;
    case MESH_MSG_BIG_PING:
        if (pmesh_msg->msg_len == sizeof(big_ping_t))
        {
            big_ping_p pmsg = (big_ping_t *)(pbuffer);
            if (pmsg->init_ttl >= pmesh_msg->ttl)
            {
                uint8_t hops_forward = pmsg->init_ttl - pmesh_msg->ttl + 1;
                printi("ping_control_receive: Big [0x%04x -> 0x%04x]: %d hops!", pmesh_msg->src, pmesh_msg->dst,
                       hops_forward);
                if (pf_ping_app_ping_cb)
                {
                    pf_ping_app_ping_cb(pmesh_msg->src, pmesh_msg->dst, hops_forward, PING_PONG_TYPE_ACCESS_BIG,
                                        pmsg->init_ttl, pmesh_msg->app_key_index, pmsg->pong_max_delay);
                }
                else
                {
                    big_pong(pmesh_msg->src, pmsg->init_ttl, pmesh_msg->app_key_index, hops_forward, 0);
                }
            }
        }
        break;
    case MESH_MSG_PONG:
        if (pmesh_msg->msg_len == sizeof(pong_t))
        {
            pong_t *pmsg = (pong_t *)(pbuffer);
            if (pmsg->init_ttl >= pmesh_msg->ttl)
            {
                uint8_t hops_reverse = pmsg->init_ttl - pmesh_msg->ttl + 1;
                printi("ping_control_receive: [0x%04x -> 0x%04x]: %d-%d hops!", pmesh_msg->dst,
                       pmesh_msg->src, pmsg->hops_forward, hops_reverse);
                if (pf_ping_control_pong_cb)
                {
                    pf_ping_control_pong_cb(pmesh_msg->src, pmesh_msg->dst, pmsg->hops_forward, PING_PONG_TYPE_ACCESS,
                                            hops_reverse, pmsg->pong_delay);
                }
            }
        }
        break;
    case MESH_MSG_BIG_PONG:
        if (pmesh_msg->msg_len == sizeof(big_pong_t))
        {
            big_pong_p pmsg = (big_pong_t *)(pbuffer);
            if (pmsg->init_ttl >= pmesh_msg->ttl)
            {
                uint8_t hops_reverse = pmsg->init_ttl - pmesh_msg->ttl + 1;
                printi("ping_control_receive: Big [0x%04x -> 0x%04x]: %d-%d hops!", pmesh_msg->dst,
                       pmesh_msg->src, pmsg->hops_forward, hops_reverse);
                if (pf_ping_control_pong_cb != NULL)
                {
                    pf_ping_control_pong_cb(pmesh_msg->src, pmesh_msg->dst, pmsg->hops_forward,
                                            PING_PONG_TYPE_ACCESS_BIG, hops_reverse, pmsg->pong_delay);
                }
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }

    return ret;
}

void ping_control_reg(pf_ping_cb_t pf_ping_cb, pf_pong_cb_t pf_pong_cb)
{
    ping_control.model_id = MESH_MODEL_PING_CONTROL;
    ping_control.model_receive = ping_control_receive;
    mesh_model_reg(0, &ping_control);
    pf_ping_app_ping_cb = pf_ping_cb;
    pf_ping_control_pong_cb = pf_pong_cb;
}

