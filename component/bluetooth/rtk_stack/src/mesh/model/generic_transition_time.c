/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_transition_time.c
* @brief    Source file for generic transition time.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-11
* @version  v1.0
* *************************************************************************************
*/
#include "generic_transition_time.h"
#include "mesh_api.h"
#include "generic_types.h"

#define TRANS_TICK_PERIOD          50

typedef struct _trans_list
{
    struct _trans_list *prev;
    struct _trans_list *next;
} trans_list_t, *trans_list_p;

typedef struct _trans_time_remain
{
    mesh_model_info_p pmodel_info;
    uint32_t trans_type;
    generic_transition_time_t total_time;
    int8_t remain_num_steps;
    int16_t remain_step_ticks;
    generic_transition_step_change_cb step_change;
    trans_list_t node;
} trans_time_remain_t, *trans_time_remain_p;

static trans_list_t trans_list_head;

#if MODEL_ENABLE_MULTI_THREAD
#define TRANS_INFINITE_WAIT      0xffffffff
plt_mutex_t trans_mutex;
#endif

const int16_t tick_map[] = {100 / TRANS_TICK_PERIOD, 1000 / TRANS_TICK_PERIOD, 10000 / TRANS_TICK_PERIOD, 600000 / TRANS_TICK_PERIOD};
const uint32_t trans_time_map_max[] = {6200, 62000, 620000, 37200000};
const uint32_t trans_time_tick_map[] = {100, 1000, 10000, 600000};

static __INLINE bool trans_time_is_empty(void)
{
    return (trans_list_head.next == &trans_list_head);
}


uint32_t generic_transition_time_convert(generic_transition_time_t trans_time)
{
    uint32_t time = tick_map[trans_time.step_resolution];
    time *= TRANS_TICK_PERIOD;
    time *= trans_time.num_steps;

    return time;
}

generic_transition_time_t time_to_generic_transition_time(uint32_t time)
{
    generic_transition_time_t trans_time;
    uint8_t index = 0;
    for (; index < sizeof(trans_time_map_max) / sizeof(trans_time_map_max[0]); ++index)
    {
        if (time <= trans_time_map_max[index])
        {
            break;
        }
    }

    if (index >= sizeof(trans_time_map_max) / sizeof(trans_time_map_max[0]))
    {
        index = sizeof(trans_time_map_max) / sizeof(trans_time_map_max[0]) - 1;
    }

    uint8_t steps = time / trans_time_tick_map[index];
    if (steps > 0x3e)
    {
        steps = 0x3e;
    }

    trans_time.num_steps = steps;
    trans_time.step_resolution = index;

    return trans_time;
}

static bool trans_time_insert(const mesh_model_info_p pmodel_info,
                              uint32_t trans_type,
                              generic_transition_time_t trans_time,
                              generic_transition_step_change_cb step_change)
{
    trans_list_p pcur = trans_list_head.next;
    trans_time_remain_p ptime = NULL;
    for (; pcur != &trans_list_head; pcur = pcur->next)
    {
        ptime = CONTAINER_OF(pcur, trans_time_remain_t, node);
        if ((ptime->pmodel_info == pmodel_info) &&
            (ptime->trans_type == trans_type))
        {
            break;
        }
    }

    if (pcur != &trans_list_head)
    {
        /* reset transition time */
        ptime->total_time = trans_time;
        ptime->remain_num_steps = trans_time.num_steps;
        ptime->remain_step_ticks = tick_map[trans_time.step_resolution];
        return TRUE;
    }

    trans_time_remain_p premain_time = plt_malloc(sizeof(trans_time_remain_t), RAM_TYPE_DATA_ON);
    if (NULL == premain_time)
    {
        printe("trans_time_insert: allocate transition time memory failed");
        return FALSE;
    }
    premain_time->pmodel_info = pmodel_info;
    premain_time->trans_type = trans_type;
    premain_time->total_time = trans_time;
    premain_time->remain_num_steps = trans_time.num_steps;
    premain_time->remain_step_ticks = tick_map[trans_time.step_resolution];
    premain_time->step_change = step_change;
    premain_time->node.next = NULL;
    premain_time->node.prev = NULL;

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(trans_mutex, TRANS_INFINITE_WAIT);
#else
    plt_sched_suspend();
#endif

    /* sort insert */
    pcur = trans_list_head.next;
    for (; pcur != &trans_list_head; pcur = pcur->next)
    {
        trans_time_remain_p ptime = CONTAINER_OF(pcur, trans_time_remain_t, node);
        int32_t cur_time_tick = ptime->remain_num_steps * tick_map[ptime->total_time.step_resolution];
        int32_t insert_time_tick = premain_time->remain_num_steps *
                                   tick_map[ptime->total_time.step_resolution];
        if (cur_time_tick > insert_time_tick)
        {
            break;
        }
    }

    pcur->prev->next = &premain_time->node;
    premain_time->node.prev = pcur->prev;
    pcur->prev = &premain_time->node;
    premain_time->node.next = pcur;

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(trans_mutex);
#else
    plt_sched_resume();
#endif

    return TRUE;
}

static bool trans_time_remove_head(void)
{
    trans_list_p pfirst = trans_list_head.next;
    if (pfirst == &trans_list_head)
    {
        return FALSE;
    }

    trans_time_remain_p premain_time = CONTAINER_OF(pfirst, trans_time_remain_t, node);
    if (premain_time->remain_num_steps <= 0)
    {
        /* remove first */
        pfirst->prev->next = pfirst->next;
        pfirst->next->prev = pfirst->prev;
        plt_free(premain_time, RAM_TYPE_DATA_ON);
        return TRUE;
    }

    return FALSE;
}

static void trans_time_timeout_handle(void)
{
#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(trans_mutex, TRANS_INFINITE_WAIT);
#else
    MESH_SW_TIMER_ENTER_CRITICAL();
#endif

    trans_list_p pcur = trans_list_head.next;
    trans_time_remain_p ptime = NULL;
    for (; pcur != &trans_list_head; pcur = pcur->next)
    {
        ptime = CONTAINER_OF(pcur, trans_time_remain_t, node);
        ptime->remain_step_ticks --;
        if (ptime->remain_step_ticks <= 0)
        {
            if (ptime->remain_num_steps > 0)
            {
                ptime->remain_num_steps --;
            }

            /* reload step tick */
            ptime->remain_step_ticks = tick_map[ptime->total_time.step_resolution];
            if (1 == ptime->remain_num_steps)
            {
                /* make up for for timer lost(TRANS_TICK_PERIOD) */
                ptime->remain_step_ticks ++;
            }
            if (NULL != ptime->step_change)
            {
                int32_t ret = MODEL_SUCCESS;
                /* notify remaining step change */
                generic_transition_time_t remaining_time = {ptime->remain_num_steps, ptime->total_time.step_resolution};
                ret = ptime->step_change(ptime->pmodel_info, ptime->trans_type, ptime->total_time, remaining_time);
                if ((MODEL_STOP_TRANSITION == ret) &&
                    (ptime->remain_num_steps > 0))
                {
                    ptime->remain_num_steps = 0;
                    remaining_time.num_steps = 0;
#if MODEL_ENABLE_USER_STOP_TRANSITION_NOTIFICATION
                    /* notify transition end */
                    ptime->step_change(ptime->pmodel_info, ptime->trans_type, ptime->total_time, remaining_time);
#endif
                }
            }
        }
    }

    /* remove expired transition */
    while (trans_time_remove_head());

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(trans_mutex);
#else
    MESH_SW_TIMER_EXIT_CRITICAL();
#endif

    /* check empty */
    if (trans_time_is_empty())
    {
        printi("stop transition timer");
        mesh_tick_timer_stop();
    }
}

bool generic_transition_timer_start(const mesh_model_info_p pmodel,
                                    uint32_t trans_type,
                                    generic_transition_time_t trans_time,
                                    generic_transition_step_change_cb step_change)
{
    bool ret = FALSE;
    if (!mesh_tick_timer_is_running())
    {
        if (trans_time_insert(pmodel, trans_type, trans_time, step_change))
        {
            mesh_tick_timer_start(TRANS_TICK_PERIOD, trans_time_timeout_handle);
            ret = TRUE;
        }
    }
    else
    {
        ret = trans_time_insert(pmodel, trans_type, trans_time, step_change);
    }

    return ret;
}


void generic_transition_timer_stop(const mesh_model_info_p pmodel_info,
                                   uint32_t trans_type)
{
    /* remove specified model timer */
#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(trans_mutex, TRANS_INFINITE_WAIT);
#else
    plt_sched_suspend();
#endif

    trans_list_p pcur = trans_list_head.next;
    for (; pcur != &trans_list_head; pcur = pcur->next)
    {
        trans_time_remain_p ptime = CONTAINER_OF(pcur, trans_time_remain_t, node);
        if ((ptime->pmodel_info == pmodel_info) &&
            (ptime->trans_type == trans_type))
        {
            ptime->remain_num_steps = 0;
            break;
        }
    }

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(trans_mutex);
#else
    plt_sched_resume();
#endif
}


generic_transition_time_t generic_transition_time_get(const mesh_model_info_p pmodel_info,
                                                      uint32_t trans_type)
{
#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_take(trans_mutex, TRANS_INFINITE_WAIT);
#else
    plt_sched_suspend();
#endif
    trans_list_p pcur = trans_list_head.next;
    trans_time_remain_p ptime = NULL;
    for (; pcur != &trans_list_head; pcur = pcur->next)
    {
        ptime = CONTAINER_OF(pcur, trans_time_remain_t, node);
        if ((ptime->pmodel_info == pmodel_info) &&
            (ptime->trans_type == trans_type))
        {
            break;
        }
    }

    generic_transition_time_t remaining_time = {0, 0};
    if (pcur != &trans_list_head)
    {
        remaining_time.num_steps = ptime->remain_num_steps;
        remaining_time.step_resolution = ptime->total_time.step_resolution;
    }

#if MODEL_ENABLE_MULTI_THREAD
    plt_mutex_give(trans_mutex);
#else
    plt_sched_resume();
#endif

    return remaining_time;
}


bool generic_transition_time_init(void)
{
#if MODEL_ENABLE_MULTI_THREAD
    if (NULL == trans_mutex)
    {
        trans_mutex = plt_mutex_create();
        if (NULL == trans_mutex)
        {
            return FALSE;
        }
    }
#endif

    if ((NULL == trans_list_head.prev) &&
        (NULL == trans_list_head.next))
    {
        trans_list_head.prev = &trans_list_head;
        trans_list_head.next = &trans_list_head;
    }

    return TRUE;
}
