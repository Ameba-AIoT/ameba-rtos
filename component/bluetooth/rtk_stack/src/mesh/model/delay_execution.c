/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     delay_execution.c
* @brief    Source file for delay execution.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-11-29
* @version  v1.0
* *************************************************************************************
*/
#include "delay_execution.h"


#define DELAY_EXECUTION_TIMER_ID      100

typedef struct _delay_list
{
    struct _delay_list *next;
} delay_list_t;

typedef struct _delay_execution_t
{
    plt_timer_t delay_timer;
    const mesh_model_info_t *pmodel_info;
    uint32_t delay_type;
    delay_execution_cb delay_execution;
    delay_list_t node;
} delay_execution_t;

static delay_list_t delay_list_head;


#if MODEL_ENABLE_MULTI_THREAD
#define DELAY_INFINITE_WAIT      0xffffffff
plt_mutex_t delay_mutex;
#endif

void delay_execution_handler(void *pargs)
{
    delay_execution_t *pexecute = pargs;
    /* free expired delay and notify model */
    if (NULL != pexecute->delay_execution)
    {
        pexecute->delay_execution((mesh_model_info_t *)(pexecute->pmodel_info), pexecute->delay_type);
    }
    plt_timer_delete(pexecute->delay_timer, 0);
    plt_free(pexecute, RAM_TYPE_DATA_ON);
}

static void delay_execution_timeout_handle(void *ptimer)
{
#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(delay_mutex, DELAY_INFINITE_WAIT);
#else
    MESH_SW_TIMER_ENTER_CRITICAL();
#endif

    delay_list_t *pcur = delay_list_head.next;
    delay_list_t *pprev = &delay_list_head;
    delay_execution_t *pexecute = NULL;
    for (; pcur != &delay_list_head; pprev = pcur, pcur = pcur->next)
    {
        pexecute = CONTAINER_OF(pcur, delay_execution_t, node);
        if (pexecute->delay_timer == ptimer)
        {
            pprev->next = pcur->next;
            break;
        }
    }

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(delay_mutex);
#else
    MESH_SW_TIMER_EXIT_CRITICAL();
#endif

    if (pcur != &delay_list_head)
    {
        mesh_inner_msg_t msg;
        msg.type = MESH_MODEL_DELAY_EXECUTION_TIMEOUT;
        msg.pbuf = pexecute;
        if (!mesh_inner_msg_send(&msg))
        {
            printe("delay_execution_timeout_handle: send message failed!");
        }
    }
}

bool delay_execution_timer_start(const mesh_model_info_t *pmodel_info,
                                 uint32_t delay_type, uint32_t delay_time,
                                 delay_execution_cb delay_execution)
{
    delay_list_t *pcur = delay_list_head.next;
    delay_execution_t *pexecute = NULL;
    for (; pcur != &delay_list_head; pcur = pcur->next)
    {
        pexecute = CONTAINER_OF(pcur, delay_execution_t, node);
        if ((pexecute->pmodel_info == pmodel_info) && (pexecute->delay_type == delay_type))
        {
            /* find same delay execution, update it */
            pexecute->delay_execution = delay_execution;
            plt_timer_change_period(pexecute->delay_timer, delay_time, 0);
            printi("delay_execution_timer_start: update timer");
            return TRUE;
        }
    }

    /* new delay execution */
    pexecute = plt_malloc(sizeof(delay_execution_t), RAM_TYPE_DATA_ON);
    if (NULL == pexecute)
    {
        printe("delay_execution_timer_start: allocate delay execution structure failed!");
        return FALSE;
    }
    pexecute->delay_timer = plt_timer_create("delay", delay_time, FALSE, DELAY_EXECUTION_TIMER_ID,
                                             delay_execution_timeout_handle);
    if (NULL == pexecute->delay_timer)
    {
        printe("delay_execution_timer_start: allocate delay execution timer failed!");
        plt_free(pexecute, RAM_TYPE_DATA_ON);
        return FALSE;
    }
    pexecute->pmodel_info = pmodel_info;
    pexecute->delay_type = delay_type;
    pexecute->delay_execution = delay_execution;
    pexecute->node.next = NULL;

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(delay_mutex, DELAY_INFINITE_WAIT);
#else
    plt_sched_suspend();
#endif

    /* prepend node */
    pcur = delay_list_head.next;
    pexecute->node.next = pcur;
    delay_list_head.next = &pexecute->node;

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(delay_mutex);
#else
    plt_sched_resume();
#endif

    /* start delay execution */
    plt_timer_start(pexecute->delay_timer, 0);

    return TRUE;
}

void delay_execution_timer_stop(const mesh_model_info_t *pmodel_info,
                                uint32_t delay_type)
{
    /* remove specified model delay */
#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(delay_mutex, DELAY_INFINITE_WAIT);
#else
    plt_sched_suspend();
#endif

    delay_list_t *pcur = delay_list_head.next;
    delay_list_t *pprev = &delay_list_head;
    delay_execution_t *pexecute = NULL;
    for (; pcur != &delay_list_head; pprev = pcur, pcur = pcur->next)
    {
        pexecute = CONTAINER_OF(pcur, delay_execution_t, node);
        if ((pexecute->pmodel_info == pmodel_info) &&
            (pexecute->delay_type == delay_type))
        {
            pprev->next = pcur->next;
            break;
        }
    }

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(delay_mutex);
#else
    plt_sched_resume();
#endif

    if (pcur != &delay_list_head)
    {
        /* free expired delay and notify model */
#if MODEL_ENABLE_USER_STOP_DELAY_NOTIFICATION
        if (NULL != pexecute->delay_execution)
        {
            pexecute->delay_execution((mesh_model_info_t *)(pexecute->pmodel_info), pexecute->delay_type);
        }
#endif
        plt_timer_delete(pexecute->delay_timer, 0);
        plt_free(pexecute, RAM_TYPE_DATA_ON);
    }
}

bool delay_execution_init(void)
{
#if MODEL_ENABLE_MULTI_THREAD
    if (NULL == delay_mutex)
    {
        delay_mutex = plt_mutex_create();
        if (NULL == delay_mutex)
        {
            return FALSE;
        }
    }
#endif

    if (NULL == delay_list_head.next)
    {
        delay_list_head.next = &delay_list_head;
    }

    mesh_model_delay_execution_init(delay_execution_handler);
    return TRUE;
}
