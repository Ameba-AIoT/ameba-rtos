/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     model_fsm.c
* @brief    Source file for model state machine
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2020-4-20
* @version  v1.0
* *************************************************************************************
*/
#include "model_fsm.h"
#include "mesh_api.h"

typedef enum
{
    MODEL_FSM_STATE_IDLE,
    MODEL_FSM_STATE_RUNNING,
    MODEL_FSM_STATE_PAUSE,
} model_fsm_state_t;

struct _model_fsm_t
{
    model_fsm_state_t fsm_state;
    const model_fsm_table_t *ptable;
    uint32_t table_len;
    uint32_t init_state;
    uint32_t cur_state;
};


model_fsm_t *model_fsm_reg(const model_fsm_table_t *ptable, uint32_t table_len, uint32_t init_state)
{
    model_fsm_t *pfsm = plt_malloc(sizeof(struct _model_fsm_t), RAM_TYPE_DATA_ON);
    if (NULL == pfsm)
    {
        printe("model_fsm_reg: register fsm failed, out of memory");
        return NULL;
    }
    pfsm->fsm_state = MODEL_FSM_STATE_IDLE;
    pfsm->ptable = ptable;
    pfsm->table_len = table_len;
    pfsm->cur_state = init_state;
    pfsm->init_state = init_state;

    return pfsm;
}

void model_fsm_unreg(model_fsm_t *pfsm)
{
    plt_free(pfsm, RAM_TYPE_DATA_ON);
}

void model_fsm_run(model_fsm_t *pfsm)
{
    if (MODEL_FSM_STATE_RUNNING == pfsm->fsm_state)
    {
        printi("model_fsm_run: state machine is already running");
        return ;
    }

    printi("model_fsm_run: fsm state %d->%d", pfsm->fsm_state, MODEL_FSM_STATE_RUNNING);
    uint32_t prev_state = pfsm->fsm_state;
    pfsm->fsm_state = MODEL_FSM_STATE_RUNNING;
    if (MODEL_FSM_STATE_IDLE == prev_state)
    {
        /* execute state enter */
        for (uint32_t i = 0; i < pfsm->table_len; ++i)
        {
            if (pfsm->cur_state == pfsm->ptable[i].state)
            {
                if (NULL != pfsm->ptable[i].state_enter)
                {
                    pfsm->ptable[i].state_enter();
                }
            }
        }
    }
}

void model_fsm_pause(model_fsm_t *pfsm)
{
    if (MODEL_FSM_STATE_RUNNING == pfsm->fsm_state)
    {
        printi("model_fsm_pause: fsm state %d->%d", pfsm->fsm_state, MODEL_FSM_STATE_PAUSE);
        pfsm->fsm_state = MODEL_FSM_STATE_PAUSE;
    }
}

void model_fsm_stop(model_fsm_t *pfsm)
{
    if (MODEL_FSM_STATE_IDLE == pfsm->fsm_state)
    {
        printi("model_fsm_stop: state machine is not running");
        return ;
    }

    printi("model_fsm_pause: fsm stop, state %d->%d", pfsm->cur_state, pfsm->init_state);
    pfsm->fsm_state = MODEL_FSM_STATE_IDLE;
    uint32_t prev_state = pfsm->cur_state;
    pfsm->cur_state = pfsm->init_state;
    /* execute state exit */
    for (uint32_t i = 0; i < pfsm->table_len; ++i)
    {
        if (prev_state == pfsm->ptable[i].state)
        {
            if (NULL != pfsm->ptable[i].state_exit)
            {
                pfsm->ptable[i].state_exit();
            }
        }
    }
}

void model_fsm_handle_event(model_fsm_t *pfsm, uint32_t event)
{
    if (MODEL_FSM_STATE_RUNNING != pfsm->fsm_state)
    {
        printw("model_fsm_handle_event: state machine is not running(%d)", pfsm->fsm_state);
        return ;
    }

    printi("model_fsm_handle_event: state %d, event %d", pfsm->cur_state, event);
    bool state_transfer = false;
    for (uint32_t index_table = 0; index_table < pfsm->table_len; ++index_table)
    {
        if (pfsm->cur_state == pfsm->ptable[index_table].state)
        {
            for (uint32_t index_event = 0; index_event < pfsm->ptable[index_table].event_group_len;
                 ++index_event)
            {
                if (pfsm->ptable[index_table].pevent_group[index_event].event == event)
                {
                    /* state and event matched, start exection action */
                    if (NULL != pfsm->ptable[index_table].pevent_group[index_event].event_action)
                    {
                        state_transfer = pfsm->ptable[index_table].pevent_group[index_event].event_action(
                                             pfsm->ptable[index_table].state);
                    }

                    printi("model_fsm_handle_event: event match, transfer %d", state_transfer);
                    if (state_transfer)
                    {
                        /* exit state */
                        if (NULL != pfsm->ptable[index_table].state_exit)
                        {
                            pfsm->ptable[index_table].state_exit();
                        }

                        pfsm->cur_state = pfsm->ptable[index_table].pevent_group[index_event].next_state;
                    }
                    goto TRANSFER;
                }
            }
        }
    }

TRANSFER:
    if (state_transfer)
    {
        for (uint32_t index_table = 0; index_table < pfsm->table_len; ++index_table)
        {
            if (pfsm->cur_state == pfsm->ptable[index_table].state)
            {
                /* enter state */
                if (NULL != pfsm->ptable[index_table].state_enter)
                {
                    pfsm->ptable[index_table].state_enter();
                }
                break;
            }
        }
    }
}

