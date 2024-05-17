/*
 * Copyright (c) 2018-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "thread.h"
#include "tfm_arch.h"
#include "utilities.h"

/* Declaration of current thread pointer. */
struct thread_t *p_curr_thrd;

/* Force ZERO in case ZI(bss) clear is missing. */
static struct thread_t *p_thrd_head = NULL; /* Point to the first thread. */
static struct thread_t *p_rnbl_head = NULL; /* Point to the first runnable. */

/* Define Macro to fetch global to support future expansion (PERCPU e.g.) */
#define LIST_HEAD   p_thrd_head
#define RNBL_HEAD   p_rnbl_head

/* Callback function pointer for thread to query current state. */
static thrd_query_state_t query_state_cb = (thrd_query_state_t)NULL;

void thrd_set_query_callback(thrd_query_state_t fn)
{
    query_state_cb = fn;
}

struct thread_t *thrd_next(void)
{
    struct thread_t *p_thrd = RNBL_HEAD;
    uint32_t retval = 0;

    /*
     * First runnable thread has highest priority since threads are
     * sorted by priority.
     */
    while (p_thrd) {
        /* Change thread state if any signal changed */
        p_thrd->state = query_state_cb(p_thrd, &retval);

        if (p_thrd->state == THRD_STATE_RET_VAL_AVAIL) {
            tfm_arch_set_context_ret_code(p_thrd->p_context_ctrl, retval);
            p_thrd->state = THRD_STATE_RUNNABLE;
        }

        if (p_thrd->state == THRD_STATE_RUNNABLE) {
            break;
        }

        p_thrd = p_thrd->next;
    }

    return p_thrd;
}

static void insert_by_prior(struct thread_t **head, struct thread_t *node)
{
    if (*head == NULL || (node->priority <= (*head)->priority)) {
        node->next = *head;
        *head = node;
    } else {
        struct thread_t *iter = *head;

        while (iter->next && (node->priority > iter->next->priority)) {
            iter = iter->next;
        }

        node->next = iter->next;
        iter->next = node;
    }
}

void thrd_start(struct thread_t *p_thrd, thrd_fn_t fn, thrd_fn_t exit_fn, void *param)
{
    SPM_ASSERT(p_thrd != NULL);

    /* Insert a new thread with priority */
    insert_by_prior(&LIST_HEAD, p_thrd);

    tfm_arch_init_context(p_thrd->p_context_ctrl, (uintptr_t)fn, param,
                          (uintptr_t)exit_fn);

    /* Mark it as RUNNABLE after insertion */
    thrd_set_state(p_thrd, THRD_STATE_RUNNABLE);
}

void thrd_set_state(struct thread_t *p_thrd, uint32_t new_state)
{
    SPM_ASSERT(p_thrd != NULL);

    p_thrd->state = new_state;

    /*
     * Set first runnable thread as head to reduce enumerate
     * depth while searching for a first runnable thread.
     */
    if ((p_thrd->state == THRD_STATE_RUNNABLE) &&
        ((RNBL_HEAD == NULL) || (p_thrd->priority < RNBL_HEAD->priority))) {
        RNBL_HEAD = p_thrd;
    } else {
        RNBL_HEAD = LIST_HEAD;
    }
}

uint32_t thrd_start_scheduler(struct thread_t **ppth)
{
    struct thread_t *pth = thrd_next();

    tfm_arch_trigger_pendsv();

    if (ppth) {
        *ppth = pth;
    }

    return tfm_arch_refresh_hardware_context(pth->p_context_ctrl);
}
