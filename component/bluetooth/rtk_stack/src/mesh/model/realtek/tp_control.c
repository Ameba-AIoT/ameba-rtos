/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     tp_control.c
* @brief    Source file for throughput/transparent control model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-3-7
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include <string.h>
#include "trace.h"
#include "tp.h"

mesh_model_info_t tp_control;

struct tp_tx_ctx_t
{
    uint32_t tid;
    uint32_t count;
    uint16_t dst;
    uint8_t ttl;
    uint16_t app_key_index;
    uint32_t begin_time;
    uint32_t end_time;
} tp_tx_ctx;

static mesh_msg_send_cause_t tp_control_send(uint16_t dst, uint8_t ttl, uint8_t *pmsg,
                                             uint16_t msg_len, uint16_t app_key_index)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = &tp_control;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.ttl = ttl;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t tp_msg(uint16_t dst, uint8_t ttl, uint16_t app_key_index, uint8_t *pdata,
                             uint16_t data_len)
{
    uint16_t len = MEMBER_OFFSET(tp_msg_t, padding) + data_len;
    tp_msg_t *pmsg = (tp_msg_t *)plt_malloc(len, RAM_TYPE_DATA_OFF);
    if (pmsg == NULL)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    pmsg->tid = tp_tx_ctx.tid;
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_TP_MSG);
    if (pdata)
    {
        memcpy(pmsg->padding, pdata, data_len);
    }
    mesh_msg_send_cause_t ret = tp_control_send(dst, ttl, (uint8_t *)pmsg, len,
                                                app_key_index);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

mesh_msg_send_cause_t tp_start(uint16_t dst, uint8_t ttl, uint16_t app_key_index,
                               uint32_t count)
{
    if (MESH_IS_UNASSIGNED_ADDR(dst) || count == 0)
    {
        /* quit to send segmented message */
        tp_tx_ctx.count = tp_tx_ctx.tid;
        return MESH_MSG_SEND_CAUSE_SUCCESS;
    }
    tp_tx_ctx.tid = 0;
    tp_tx_ctx.count = count;
    tp_tx_ctx.dst = dst;
    tp_tx_ctx.ttl = ttl;
    tp_tx_ctx.app_key_index = app_key_index;
    tp_tx_ctx.begin_time = plt_time_read_ms();
    return tp_msg(tp_tx_ctx.dst, tp_tx_ctx.ttl, tp_tx_ctx.app_key_index, NULL,
                  sizeof(tp_msg_t) - MEMBER_OFFSET(tp_msg_t, padding));
}

bool tp_control_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_TP_MSG:
        if (pmesh_msg->msg_len >= MEMBER_OFFSET(tp_msg_t, padding))
        {
            if (tp_control.model_data_cb)
            {
                tp_control.model_data_cb(&tp_control, 0, pmesh_msg);
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }

    return ret;
}

void tp_send_cb(mesh_model_info_p pmodel_info, mesh_msg_send_stat_t stat, uint32_t access_opcode)
{
    UNUSED(pmodel_info);
    UNUSED(access_opcode);
    if (stat == MESH_MSG_SEND_STAT_SENT || stat == MESH_MSG_SEND_STAT_ACKED ||
        stat == MESH_MSG_SEND_STAT_ACKED_OBO)
    {
        tp_tx_ctx.tid++;
    }
    else if (MESH_IS_GROUP_ADDR(tp_tx_ctx.dst) && stat == MESH_MSG_SEND_STAT_TIMEOUT)
    {
        tp_tx_ctx.tid++;
    }
    else
    {
        data_uart_debug("tp tx fail reason=%d\r\n", stat);
    }
    /* throughput mode */
    if (tp_tx_ctx.count)
    {
        if (tp_tx_ctx.tid < tp_tx_ctx.count)
        {
            tp_msg(tp_tx_ctx.dst, tp_tx_ctx.ttl, tp_tx_ctx.app_key_index, NULL,
                   sizeof(tp_msg_t) - MEMBER_OFFSET(tp_msg_t, padding));
        }
        else
        {
            tp_tx_ctx.end_time = plt_time_read_ms();
            data_uart_debug("tp tx elapsed time=%dms tp=%dbyte/s\r\n",
                            tp_tx_ctx.end_time - tp_tx_ctx.begin_time,
                            ACCESS_PAYLOAD_MAX_SIZE * tp_tx_ctx.tid * 1000 / (tp_tx_ctx.end_time - tp_tx_ctx.begin_time));
            tp_tx_ctx.count = 0;
        }
    }
}

void tp_control_reg(model_data_cb_pf data_cb)
{
    tp_control.model_id = MESH_MODEL_TP_CONTROL;
    tp_control.model_receive = tp_control_receive;
    tp_control.model_send_cb = tp_send_cb;
    tp_control.model_data_cb = data_cb;
    mesh_model_reg(0, &tp_control);
}

