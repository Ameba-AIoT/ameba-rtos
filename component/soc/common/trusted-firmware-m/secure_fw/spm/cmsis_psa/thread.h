/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __M_THREAD_H__ /* Add an extra M as thread.h is common. */
#define __M_THREAD_H__

#include <stddef.h>
#include <stdint.h>

/* State codes */
#define THRD_STATE_CREATING       0
#define THRD_STATE_RUNNABLE       1
#define THRD_STATE_BLOCK          2
#define THRD_STATE_DETACH         3
#define THRD_STATE_INVALID        4
#define THRD_STATE_RET_VAL_AVAIL  5

/* Priorities. Lower value has higher priority */
#define THRD_PRIOR_HIGHEST        0x0
#define THRD_PRIOR_HIGH           0xF
#define THRD_PRIOR_MEDIUM         0x1F
#define THRD_PRIOR_LOW            0x7F
#define THRD_PRIOR_LOWEST         0xFF

/* Error codes */
#define THRD_SUCCESS              0
#define THRD_ERR_GENERIC          1

/* Thread entry function type */
typedef void (*thrd_fn_t)(void *);

/* An address causes exceptions (invalid address and security bit). */
#define THRD_GENERAL_EXIT  ((thrd_fn_t)(0xFFFFFFFE))

/* Thread context */
struct thread_t {
    uint8_t         priority;           /* Priority                          */
    uint8_t         state;              /* State                             */
    uint16_t        flags;              /* Flags and align, DO NOT REMOVE!   */
    void            *p_context_ctrl;    /* Context control (sp, splimit, lr) */
    struct thread_t *next;              /* Next thread in list               */
};

/* Query thread state function type */
typedef uint32_t (*thrd_query_state_t)(struct thread_t *p_thrd,
                                       uint32_t *p_retval);
/*
 * Definition for the current thread and its access helper preprocessor.
 * The definition needs to be declared in one of the sources.
 */
extern struct thread_t *p_curr_thrd;
#define CURRENT_THREAD p_curr_thrd

/*
 * Initialize the thread_t struct with the given inputs.
 *
 * Parameters:
 *  p_thrd         -    Pointer of caller provided thread_t struct to be init
 *  p_ctx_ctrl     -    Initial Context control (sp, splimit, lr)
 *  priority       -    Initial priority
 */
#define THRD_INIT(p_thrd, p_ctx_ctrl, prio) do {                         \
                        (p_thrd)->priority       = (uint8_t)(prio);      \
                        (p_thrd)->state          = THRD_STATE_CREATING;  \
                        (p_thrd)->flags          = 0;                    \
                        (p_thrd)->p_context_ctrl = p_ctx_ctrl;           \
                    } while (0)

/*
 * Set thread priority.
 *
 * Parameters :
 *  p_thrd         -     Pointer of thread_t struct
 *  priority       -     Priority value (0~255)
 *
 * Note :
 *  The new priority may not take effect immediately.
 */
#define THRD_SET_PRIORITY(p_thrd, priority) \
                                        p_thrd->priority = (uint8_t)(priority)

/*
 * Update current thread's bound context pointer.
 *
 * Parameters :
 *  x              -     Context pointer to be bound with the current thread.
 */
#define THRD_UPDATE_CUR_CTXCTRL(x)          \
                                CURRENT_THREAD->p_context_ctrl = (void *)(x)

/*
 * Check if a schedule is under expectation by measuring on a given thread.
 *
 * Return :
 *  `true` if schedule is under expectation. `false` if not.
 */
#define THRD_EXPECTING_SCHEDULE() (!(thrd_next() == CURRENT_THREAD))

/*
 * Init the global query state callback function pointer.
 *
 * Parameters :
 *  fn             -     Query state function pointer.
 */
void thrd_set_query_callback(thrd_query_state_t fn);

/*
 * Set thread state, and updates the runnable head.
 *
 * Parameters :
 *  p_thrd         -     Pointer of thread_t struct
 *  new_state      -     New state of thread
 */
void thrd_set_state(struct thread_t *p_thrd, uint32_t new_state);

/*
 * Prepare thread context with given info and insert it into schedulable list.
 *
 * Parameters :
 *  p_thrd         -     Pointer of thread_t struct
 *  fn             -     Thread entry function
 *  exit_fn        -     The function to go when 'fn' exited
 *  param          -     Parameter passed to fn
 *
 * Note :
 *  - The thread is not "started" immediately.
 */
void thrd_start(struct thread_t *p_thrd, thrd_fn_t fn, thrd_fn_t exit_fn, void *param);

/*
 * Get the next thread to run in list.
 *
 * Return :
 *  Pointer of next thread to run.
 */
struct thread_t *thrd_next(void);

/*
 * Start scheduling.
 *
 * ppth [out]       -     The first runnable thread
 *
 * Return :
 *  The EXC_RETURN payload of the first runnable thread for caller usage.
 */
uint32_t thrd_start_scheduler(struct thread_t **ppth);

#endif /* __M_THREAD_H__ */
