/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ping_app.c
  * @brief    Source file for ping app.
  * @details  Data types and external functions declaration.
  * @author   bill
  * @date     2018-4-2
  * @version  v1.0
  * *************************************************************************************
  */

/* Add Includes here */
#include "trace.h"
#include "app_msg.h"
#include "ping_app.h"

extern void *evt_queue_handle; //!< Event queue handle
extern void *io_queue_handle; //!< IO queue handle
static plt_timer_t pong_timer;
static uint16_t pong_dst;
static uint8_t pong_ttl;
static uint8_t pong_key_index;
static uint8_t pong_hops_forward;
static ping_pong_type_t pong_type;
static uint16_t pong_delay;

static void ping_app_timeout_cb(void *ptimer)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = PING_APP_TIMEOUT_MSG;
    if (os_msg_send(io_queue_handle, &msg, 0) == false)
    {
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
    }
}

void ping_app_handle_timeout(void)
{
    plt_timer_delete(pong_timer, 0);
    pong_timer = 0;
    printi("pong_timeout_process: delayed %d0ms pong right now!", pong_delay);
    switch (pong_type)
    {
    case PING_PONG_TYPE_TRANSPORT:
        trans_pong(pong_dst, pong_ttl, pong_key_index, pong_hops_forward, pong_delay);
        break;
    case PING_PONG_TYPE_ACCESS:
        pong(pong_dst, pong_ttl, pong_key_index, pong_hops_forward, pong_delay);
        break;
    case PING_PONG_TYPE_ACCESS_BIG:
        big_pong(pong_dst, pong_ttl, pong_key_index, pong_hops_forward, pong_delay);
        break;
    default:
        break;
    }
}

void ping_app_ping_cb(uint16_t src, uint16_t dst, uint8_t hops_forward, ping_pong_type_t type,
                      uint8_t init_ttl, uint8_t key_index, uint16_t pong_max_delay)
{
    UNUSED(dst);
    if (pong_max_delay == 0 || pong_timer)
    {
        switch (type)
        {
        case PING_PONG_TYPE_TRANSPORT:
            trans_pong(src, init_ttl, key_index, hops_forward, 0);
            break;
        case PING_PONG_TYPE_ACCESS:
            pong(src, init_ttl, key_index, hops_forward, 0);
            break;
        case PING_PONG_TYPE_ACCESS_BIG:
            big_pong(src, init_ttl, key_index, hops_forward, 0);
            break;
        default:
            break;
        }
    }
    else
    {
        pong_dst = src;
        pong_ttl = init_ttl;
        pong_key_index = key_index;
        pong_hops_forward = hops_forward;
        pong_type = type;
        uint16_t rand_delay;
        plt_rand((uint8_t *)&rand_delay, sizeof(rand_delay));
        rand_delay = ((rand_delay * pong_max_delay) >> 16) + 1;
        pong_delay = rand_delay;
        pong_timer = plt_timer_create("pong", rand_delay * 10, false, 0, ping_app_timeout_cb);
        if (pong_timer)
        {
            plt_timer_start(pong_timer, 0);
        }
    }
}
