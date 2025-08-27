/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_lc_fsm.c
* @brief    Source file for light lc state machine.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2020-04-22
* @version  v1.0
* *************************************************************************************
*/
#include "light_lc_fsm.h"
#include "mesh_api.h"
#include "model_fsm.h"

#define LIGHT_LC_DEFAULT_TIME                   4000 /* ms */
#define LIGHT_LC_MIN_TIME                       10 /* ms */

typedef struct
{
    uint32_t time_fade_on;
    uint32_t time_run_on;
    uint32_t time_fade;
    uint32_t time_prolong;
    uint32_t time_fade_standby_auto;
    uint32_t time_fade_standby_manual;
    uint32_t time_occupancy_delay;
    uint32_t time_trans;
} light_lc_time_t;

typedef struct
{
    model_fsm_t *pfsm;
    bool mode_on;
    bool om_on;
    bool light_on;
    plt_timer_t lc_timer;
    light_lc_action_cb_t lc_action;
    light_lc_time_t lc_time;
} light_lc_fsm_ctx_t;

static light_lc_fsm_ctx_t lc_fsm_ctx;

static bool light_lc_mode_on_action(uint32_t state)
{
    plt_timer_stop(lc_fsm_ctx.lc_timer, 0);
    switch (state)
    {
    case LIGHT_LC_STATE_OFF:
        if (lc_fsm_ctx.lc_action)
        {
            lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_STANDBY, 0);
        }
        break;
    default:
        break;
    }
    return TRUE;
}

static bool light_lc_mode_off_action(uint32_t state)
{
    UNUSED(state);
    plt_timer_stop(lc_fsm_ctx.lc_timer, 0);
    if (lc_fsm_ctx.lc_action)
    {
        lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_OFF, 0);
    }
    return TRUE;
}

static bool light_lc_light_on_action(uint32_t state)
{
    switch (state)
    {
    case LIGHT_LC_STATE_STANDBY:
    case LIGHT_LC_STATE_FADE:
    case LIGHT_LC_STATE_PROLONG:
    case LIGHT_LC_STATE_FADE_STANDBY_AUTO:
    case LIGHT_LC_STATE_FADE_STANDBY_MANUAL:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_trans ? lc_fsm_ctx.lc_time.time_trans : LIGHT_LC_MIN_TIME, 0);
        break;
    case LIGHT_LC_STATE_RUN:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_run_on ? lc_fsm_ctx.lc_time.time_run_on : LIGHT_LC_MIN_TIME, 0);
        break;
    default:
        break;
    }
    return TRUE;
}

static bool light_lc_light_off_action(uint32_t state)
{
    switch (state)
    {
    case LIGHT_LC_STATE_STANDBY:
        break;
    case LIGHT_LC_STATE_FADE_ON:
    case LIGHT_LC_STATE_RUN:
    case LIGHT_LC_STATE_FADE:
    case LIGHT_LC_STATE_PROLONG:
    case LIGHT_LC_STATE_FADE_STANDBY_AUTO:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_trans ? lc_fsm_ctx.lc_time.time_trans : LIGHT_LC_MIN_TIME, 0);
        break;
    default:
        break;
    }
    return TRUE;
}

static bool light_lc_occupancy_on_action(uint32_t state)
{
    switch (state)
    {
    case LIGHT_LC_STATE_STANDBY:
        if (lc_fsm_ctx.om_on)
        {
            if (lc_fsm_ctx.lc_action)
            {
                lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_FADE_ON, lc_fsm_ctx.lc_time.time_fade_on);
            }
            plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                    lc_fsm_ctx.lc_time.time_fade_on ? lc_fsm_ctx.lc_time.time_fade_on : LIGHT_LC_MIN_TIME, 0);
        }
        break;
    case LIGHT_LC_STATE_RUN:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_run_on ? lc_fsm_ctx.lc_time.time_run_on : LIGHT_LC_MIN_TIME, 0);
        break;
    case LIGHT_LC_STATE_FADE:
    case LIGHT_LC_STATE_PROLONG:
    case LIGHT_LC_STATE_FADE_STANDBY_AUTO:
        if (lc_fsm_ctx.lc_action)
        {
            lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_FADE_ON, lc_fsm_ctx.lc_time.time_fade_on);
        }
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_fade_on ? lc_fsm_ctx.lc_time.time_fade_on : LIGHT_LC_MIN_TIME, 0);
        break;
    default:
        break;
    }
    return TRUE;
}

static bool light_lc_timer_off_action(uint32_t state)
{
    switch (state)
    {
    case LIGHT_LC_STATE_FADE_ON:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_run_on ? lc_fsm_ctx.lc_time.time_run_on : LIGHT_LC_MIN_TIME, 0);
        break;
    case LIGHT_LC_STATE_RUN:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_fade ? lc_fsm_ctx.lc_time.time_fade : LIGHT_LC_MIN_TIME, 0);
        if (lc_fsm_ctx.lc_action)
        {
            lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_FADE, lc_fsm_ctx.lc_time.time_fade);
        }
        break;
    case LIGHT_LC_STATE_FADE:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_prolong ? lc_fsm_ctx.lc_time.time_prolong : LIGHT_LC_MIN_TIME, 0);
        if (lc_fsm_ctx.lc_action)
        {
            lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_PROLONG, lc_fsm_ctx.lc_time.time_prolong);
        }
        break;
    case LIGHT_LC_STATE_PROLONG:
        plt_timer_change_period(lc_fsm_ctx.lc_timer,
                                lc_fsm_ctx.lc_time.time_fade_standby_auto ? lc_fsm_ctx.lc_time.time_fade_standby_auto :
                                LIGHT_LC_MIN_TIME, 0);
        if (lc_fsm_ctx.lc_action)
        {
            lc_fsm_ctx.lc_action(LIGHT_LC_ACTION_STANDBY, lc_fsm_ctx.lc_time.time_fade_standby_auto);
        }
        break;
    case LIGHT_LC_STATE_FADE_STANDBY_AUTO:
    case LIGHT_LC_STATE_FADE_STANDBY_MANUAL:
        plt_timer_stop(lc_fsm_ctx.lc_timer, 0);
        break;
    default:
        break;
    }
    return TRUE;
}

static void light_lc_mode_off_enter(void)
{
    printi("lc enter mode off state");
}

static void light_lc_mode_off_exit(void)
{
    printi("lc exit mode off state");
}

static void light_lc_standby_enter(void)
{
    printi("lc enter standby state");
}

static void light_lc_standby_exit(void)
{
    printi("lc exit standby state");
}

static void light_lc_fade_on_enter(void)
{
    printi("lc enter fade on state");
}

static void light_lc_fade_on_exit(void)
{
    printi("lc exit fade on state");
}

static void light_lc_run_enter(void)
{
    printi("lc enter run state");
}

static void light_lc_run_exit(void)
{
    printi("sm: exit run state");
}

static void light_lc_fade_enter(void)
{
    printi("lc enter fade state");
}

static void light_lc_fade_exit(void)
{
    printi("lc exit fade state");
}

static void light_lc_prolong_enter(void)
{
    printi("lc enter prolong state");
}

static void light_lc_prolong_exit(void)
{
    printi("lc exit prolong state");
}

static void light_lc_fade_standby_auto_enter(void)
{
    printi("lc enter fade standby auto state");
}

static void light_lc_fade_standby_auto_exit(void)
{
    printi("lc exit fade standby auto state");
}

static void light_lc_fade_standby_manual_enter(void)
{
    printi("lc enter fade standby manual state");
}

static void light_lc_fade_standby_manual_exit(void)
{
    printi("lc exit fade standby manual state");
}

const model_fsm_event_group_t light_lc_off_events[] =
{
    {LIGHT_LC_EVENT_MODE_ON, light_lc_mode_on_action, LIGHT_LC_STATE_STANDBY}
};

const model_fsm_event_group_t light_lc_standby_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_OCCUPANCY_ON, light_lc_occupancy_on_action, LIGHT_LC_STATE_FADE_ON},
};

const model_fsm_event_group_t light_lc_fade_on_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_OFF, light_lc_light_off_action, LIGHT_LC_STATE_FADE_STANDBY_MANUAL},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_RUN},
};

const model_fsm_event_group_t light_lc_run_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_OFF, light_lc_light_off_action, LIGHT_LC_STATE_FADE_STANDBY_MANUAL},
    {LIGHT_LC_EVENT_OCCUPANCY_ON, light_lc_occupancy_on_action, LIGHT_LC_STATE_RUN},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_RUN},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_FADE},
};

const model_fsm_event_group_t light_lc_fade_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_OFF, light_lc_light_off_action, LIGHT_LC_STATE_FADE_STANDBY_MANUAL},
    {LIGHT_LC_EVENT_OCCUPANCY_ON, light_lc_occupancy_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_PROLONG},
};

const model_fsm_event_group_t light_lc_prolong_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_OFF, light_lc_light_off_action, LIGHT_LC_STATE_FADE_STANDBY_MANUAL},
    {LIGHT_LC_EVENT_OCCUPANCY_ON, light_lc_occupancy_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_FADE_STANDBY_AUTO},
};

const model_fsm_event_group_t light_lc_fade_standby_auto_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_OFF, light_lc_light_off_action, LIGHT_LC_STATE_FADE_STANDBY_MANUAL},
    {LIGHT_LC_EVENT_OCCUPANCY_ON, light_lc_occupancy_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_STANDBY},
};

const model_fsm_event_group_t light_lc_fade_standby_manual_events[] =
{
    {LIGHT_LC_EVENT_MODE_OFF, light_lc_mode_off_action, LIGHT_LC_STATE_OFF},
    {LIGHT_LC_EVENT_LIGHT_ON, light_lc_light_on_action, LIGHT_LC_STATE_FADE_ON},
    {LIGHT_LC_EVENT_TIMER_OFF, light_lc_timer_off_action, LIGHT_LC_STATE_STANDBY},
};

const model_fsm_table_t lc_fsm_table[] =
{
    {LIGHT_LC_STATE_OFF, light_lc_mode_off_enter, light_lc_off_events, ARRAY_LEN(light_lc_off_events), light_lc_mode_off_exit},
    {LIGHT_LC_STATE_STANDBY, light_lc_standby_enter, light_lc_standby_events, ARRAY_LEN(light_lc_standby_events), light_lc_standby_exit},
    {LIGHT_LC_STATE_FADE_ON, light_lc_fade_on_enter, light_lc_fade_on_events, ARRAY_LEN(light_lc_fade_on_events), light_lc_fade_on_exit},
    {LIGHT_LC_STATE_RUN, light_lc_run_enter, light_lc_run_events, ARRAY_LEN(light_lc_run_events), light_lc_run_exit},
    {LIGHT_LC_STATE_FADE, light_lc_fade_enter, light_lc_fade_events, ARRAY_LEN(light_lc_fade_events), light_lc_fade_exit},
    {LIGHT_LC_STATE_PROLONG, light_lc_prolong_enter, light_lc_prolong_events, ARRAY_LEN(light_lc_prolong_events), light_lc_prolong_exit},
    {LIGHT_LC_STATE_FADE_STANDBY_AUTO, light_lc_fade_standby_auto_enter, light_lc_fade_standby_auto_events, ARRAY_LEN(light_lc_fade_standby_auto_events), light_lc_fade_standby_auto_exit},
    {LIGHT_LC_STATE_FADE_STANDBY_MANUAL, light_lc_fade_standby_manual_enter, light_lc_fade_standby_manual_events, ARRAY_LEN(light_lc_fade_standby_manual_events), light_lc_fade_standby_manual_exit},
};

void light_lc_fsm_init(light_lc_state_t init_state, light_lc_action_cb_t action_cb,
                       light_lc_timeout_cb_t timeout_cb)
{
    lc_fsm_ctx.lc_timer = plt_timer_create("lc_tmr", 1000, false, 0, timeout_cb);
    lc_fsm_ctx.lc_action = action_cb;
    lc_fsm_ctx.lc_time.time_fade_on = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_run_on = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_fade = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_prolong = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_fade_standby_auto = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_fade_standby_manual = LIGHT_LC_DEFAULT_TIME;
    lc_fsm_ctx.lc_time.time_occupancy_delay = LIGHT_LC_DEFAULT_TIME;

    lc_fsm_ctx.pfsm = model_fsm_reg(lc_fsm_table, sizeof(lc_fsm_table) / sizeof(model_fsm_table_t),
                                    init_state);

    model_fsm_run(lc_fsm_ctx.pfsm);
}

void light_lc_fsm_om_set(uint8_t om)
{
    lc_fsm_ctx.om_on = om;
}

void light_lc_time_set(light_lc_time_type_t type, uint32_t value)
{
    switch (type)
    {
    case LIGHT_LC_TIME_TYPE_FADE_ON:
        lc_fsm_ctx.lc_time.time_fade_on = value;
        break;
    case LIGHT_LC_TIME_TYPE_RUN_ON:
        lc_fsm_ctx.lc_time.time_run_on = value;
        break;
    case LIGHT_LC_TIME_TYPE_FADE:
        lc_fsm_ctx.lc_time.time_fade = value;
        break;
    case LIGHT_LC_TIME_TYPE_PROLONG:
        lc_fsm_ctx.lc_time.time_prolong = value;
        break;
    case LIGHT_LC_TIME_TYPE_FADE_STANDBY_AUTO:
        lc_fsm_ctx.lc_time.time_fade_standby_auto = value;
        break;
    case LIGHT_LC_TIME_TYPE_FADE_STANDBY_MANUAL:
        lc_fsm_ctx.lc_time.time_fade_standby_manual = value;
        break;
    case LIGHT_LC_TIME_TYPE_OCCUPANCY_DELAY:
        lc_fsm_ctx.lc_time.time_occupancy_delay = value;
        break;
    case LIGHT_LC_TIME_TYPE_TRANS:
        lc_fsm_ctx.lc_time.time_trans = value;
        break;
    default:
        break;
    }
}

uint32_t light_lc_time_get(light_lc_time_type_t type)
{
    uint32_t value = 0;
    switch (type)
    {
    case LIGHT_LC_TIME_TYPE_FADE_ON:
        value = lc_fsm_ctx.lc_time.time_fade_on;
        break;
    case LIGHT_LC_TIME_TYPE_RUN_ON:
        value = lc_fsm_ctx.lc_time.time_run_on;
        break;
    case LIGHT_LC_TIME_TYPE_FADE:
        value = lc_fsm_ctx.lc_time.time_fade;
        break;
    case LIGHT_LC_TIME_TYPE_PROLONG:
        value = lc_fsm_ctx.lc_time.time_prolong;
        break;
    case LIGHT_LC_TIME_TYPE_FADE_STANDBY_AUTO:
        value = lc_fsm_ctx.lc_time.time_fade_standby_auto;
        break;
    case LIGHT_LC_TIME_TYPE_FADE_STANDBY_MANUAL:
        value = lc_fsm_ctx.lc_time.time_fade_standby_manual;
        break;
    case LIGHT_LC_TIME_TYPE_OCCUPANCY_DELAY:
        value = lc_fsm_ctx.lc_time.time_occupancy_delay;
        break;
    default:
        break;
    }

    return value;
}

void light_lc_fsm_handle_timeout(void *ptimer)
{
    UNUSED(ptimer);
    light_lc_fsm_handle_event(LIGHT_LC_EVENT_TIMER_OFF);
}

void light_lc_fsm_handle_event(light_lc_event_t event)
{
    model_fsm_handle_event(lc_fsm_ctx.pfsm, event);
}

