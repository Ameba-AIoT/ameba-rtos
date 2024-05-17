/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2021-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "aapcs_local.h"
#include "critical_section.h"
#include "compiler_ext_defs.h"
#include "config_spm.h"
#include "runtime_defs.h"
#include "ffm/stack_watermark.h"
#include "spm.h"
#include "tfm_hal_isolation.h"
#include "tfm_hal_platform.h"
#include "tfm_rpc.h"
#include "ffm/backend.h"
#include "utilities.h"
#include "cmsis_psa/memory_symbols.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/spm_load_api.h"
#include "psa/error.h"

/* Declare the global component list */
struct partition_head_t partition_listhead;

#if CONFIG_TFM_PSA_API_CROSS_CALL == 1
/* Instance for SPM_THREAD_CONTEXT */

#ifdef CONFIG_TFM_USE_TRUSTZONE
struct context_ctrl_t *p_spm_thread_context;
#else
/* If ns_agent_tz isn't used, we need to provide a stack for SPM to use */
static uint8_t spm_thread_stack[CONFIG_TFM_SPM_THREAD_STACK_SIZE] __aligned(8);
ARCH_CLAIM_CTXCTRL_INSTANCE(spm_thread_context,
                            spm_thread_stack,
                            sizeof(spm_thread_stack));

struct context_ctrl_t *p_spm_thread_context = &spm_thread_context;
#endif

#endif

/* Indicator point to the partition meta */
uintptr_t *partition_meta_indicator_pos;

extern uint32_t scheduler_lock;

/*
 * Query the state of current thread.
 */
static uint32_t query_state(struct thread_t *p_thrd, uint32_t *p_retval)
{
    struct critical_section_t cs_signal = CRITICAL_SECTION_STATIC_INIT;
    struct partition_t *p_pt = NULL;
    uint32_t state = p_thrd->state;
    psa_signal_t signal_ret = 0;

    /* Get current partition of thread. */
    p_pt = TO_CONTAINER(p_thrd->p_context_ctrl,
                        struct partition_t, ctx_ctrl);

    CRITICAL_SECTION_ENTER(cs_signal);

    signal_ret = p_pt->signals_waiting & p_pt->signals_asserted;

    if (signal_ret) {
        /*
         * If the partition is waiting some signals and any of them is asserted,
         * change thread to be THRD_STATE_RET_VAL_AVAIL and fill the retval. If
         * the waiting signal is TFM_IPC_REPLY_SIGNAL, it means the Secure
         * Partition is waiting for the services to be fulfilled, then the
         * return value comes from the backend_replying() by the server
         * Partition. For other waiting signals by psa_wait(), the return value
         * is just the signal.
         */
        if (signal_ret == TFM_IPC_REPLY_SIGNAL) {
            p_pt->signals_asserted &= ~TFM_IPC_REPLY_SIGNAL;
            *p_retval = (uint32_t)p_pt->reply_value;
        } else {
            *p_retval = signal_ret;
        }

        p_pt->signals_waiting = 0;
        state = THRD_STATE_RET_VAL_AVAIL;
    } else if (p_pt->signals_waiting != 0) {
        /*
         * If the thread is waiting some signals but none of them is asserted,
         * block the thread.
         */
        state = THRD_STATE_BLOCK;
    }

    CRITICAL_SECTION_LEAVE(cs_signal);
    return state;
}

extern struct psa_api_tbl_t psa_api_cross;
extern struct psa_api_tbl_t psa_api_svc;

static void prv_process_metadata(struct partition_t *p_pt)
{
    const struct partition_load_info_t *p_pt_ldi;
    const struct service_load_info_t *p_srv_ldi;
    struct context_ctrl_t *ctx_ctrl;
    struct runtime_metadata_t *p_rt_meta;
    service_fn_t *p_sfn_table;
    uint32_t allocate_size;

    p_pt_ldi = p_pt->p_ldinf;
    p_srv_ldi = LOAD_INFO_SERVICE(p_pt_ldi);
    ctx_ctrl = &p_pt->ctx_ctrl;

    /* common runtime metadata */
    allocate_size = sizeof(*p_rt_meta);

    if (!IS_IPC_MODEL(p_pt_ldi)) {
        /* SFN specific metadata - SFN function table */
        allocate_size += sizeof(service_fn_t) * p_pt_ldi->nservices;
    }

    ARCH_CTXCTRL_ALLOCATE_STACK(ctx_ctrl, allocate_size);
    p_rt_meta = (struct runtime_metadata_t *)
                                    ARCH_CTXCTRL_ALLOCATED_PTR(ctx_ctrl);

    p_rt_meta->entry = p_pt_ldi->entry;
#if TFM_LVL == 1
    p_rt_meta->psa_fns = &psa_api_cross;
#else
    /* TODO: ABI for PRoT partitions needs to be updated based on implementations. */
    p_rt_meta->psa_fns = &psa_api_svc;
#endif
    p_rt_meta->n_sfn = 0;
    p_sfn_table = p_rt_meta->sfn_table;

    if (!IS_IPC_MODEL(p_pt_ldi)) {
        /* SFN table. The signal bit of the service is the same index of SFN. */
        for (int i = 0; i < p_pt_ldi->nservices; i++) {
            p_sfn_table[i] = (service_fn_t)p_srv_ldi[i].sfn;
        }

        p_rt_meta->n_sfn = p_pt_ldi->nservices;
    }

    p_pt->p_metadata = (void *)p_rt_meta;
}

/*
 * Send message and wake up the SP who is waiting on message queue, block the
 * current thread and trigger scheduler.
 */
psa_status_t backend_messaging(struct service_t *service,
                               struct connection_t *handle)
{
    struct partition_t *p_owner = NULL;
    psa_signal_t signal = 0;

    if (!handle || !service || !service->p_ldinf || !service->partition) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    p_owner = service->partition;
    signal = service->p_ldinf->signal;

    UNI_LIST_INSERT_AFTER(p_owner, handle, p_handles);

    /* Messages put. Update signals */
    backend_assert_signal(p_owner, signal);

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */

    if (!is_tfm_rpc_msg(handle)) {
        backend_wait_signals(handle->p_client, TFM_IPC_REPLY_SIGNAL);
    }

    handle->status = TFM_HANDLE_STATUS_ACTIVE;

    return PSA_SUCCESS;
}

psa_status_t backend_replying(struct connection_t *handle, int32_t status)
{
    if (is_tfm_rpc_msg(handle)) {
        tfm_rpc_client_call_reply(handle, status);
    } else {
        handle->p_client->reply_value = (uintptr_t)status;
        backend_assert_signal(handle->p_client, TFM_IPC_REPLY_SIGNAL);
    }

    /*
     * 'psa_reply' exists in IPC model only and returns 'void'. Return
     * 'PSA_SUCCESS' here always since SPM does not forward the status
     * to the caller.
     */
    return PSA_SUCCESS;
}

extern void common_sfn_thread(void *param);

/* Parameters are treated as assuredly */
void backend_init_comp_assuredly(struct partition_t *p_pt,
                                 uint32_t service_setting)
{
    const struct partition_load_info_t *p_pldi = p_pt->p_ldinf;
    thrd_fn_t thrd_entry;
    void *param = NULL;

#if CONFIG_TFM_DOORBELL_API == 1
    p_pt->signals_allowed |= PSA_DOORBELL;
#endif /* CONFIG_TFM_DOORBELL_API == 1 */

    p_pt->signals_allowed |= service_setting;

    UNI_LISI_INIT_NODE(p_pt, p_handles);

    ARCH_CTXCTRL_INIT(&p_pt->ctx_ctrl,
                      LOAD_ALLOCED_STACK_ADDR(p_pldi),
                      p_pldi->stack_size);

    watermark_stack(p_pt);

    prv_process_metadata(p_pt);

    THRD_INIT(&p_pt->thrd, &p_pt->ctx_ctrl,
              TO_THREAD_PRIORITY(PARTITION_PRIORITY(p_pldi->flags)));

#if (CONFIG_TFM_PSA_API_CROSS_CALL == 1)
    if (IS_NS_AGENT_TZ(p_pldi)) {
        /* Get the context from ns_agent_tz */
        SPM_THREAD_CONTEXT = &p_pt->ctx_ctrl;
    }
#endif

    if (IS_IPC_MODEL(p_pldi)) {
        /* IPC Partition */
        thrd_entry = POSITION_TO_ENTRY(p_pldi->entry, thrd_fn_t);
    } else {
        /* SFN Partition */
        thrd_entry = POSITION_TO_ENTRY(common_sfn_thread, thrd_fn_t);
    }

    if (IS_NS_AGENT_TZ(p_pldi)) {
        /* NS agent TZ expects NSPE entry point as the parameter */
        param = (void *)tfm_hal_get_ns_entry_point();
    }

    thrd_start(&p_pt->thrd,
               thrd_entry,
               THRD_GENERAL_EXIT,
               param);
}

uint32_t backend_system_run(void)
{
    uint32_t control;
    struct partition_t *p_cur_pt;
    fih_int fih_rc = FIH_FAILURE;

#if CONFIG_TFM_PSA_API_CROSS_CALL == 1
    SPM_ASSERT(SPM_THREAD_CONTEXT);
#endif

    /* Init thread callback function. */
    thrd_set_query_callback(query_state);

    partition_meta_indicator_pos = (uintptr_t *)PART_LOCAL_STORAGE_PTR_POS;
    control = thrd_start_scheduler(&CURRENT_THREAD);

    p_cur_pt = TO_CONTAINER(CURRENT_THREAD->p_context_ctrl,
                            struct partition_t, ctx_ctrl);

    FIH_CALL(tfm_hal_activate_boundary, fih_rc, p_cur_pt->p_ldinf, p_cur_pt->boundary);
    if (fih_not_eq(fih_rc, fih_int_encode(TFM_HAL_SUCCESS))) {
        tfm_core_panic();
    }

    return control;
}

psa_signal_t backend_wait_signals(struct partition_t *p_pt, psa_signal_t signals)
{
    struct critical_section_t cs_signal = CRITICAL_SECTION_STATIC_INIT;
    psa_signal_t ret_signal;

    if (!p_pt) {
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_signal);

    ret_signal = p_pt->signals_asserted & signals;
    if (ret_signal == 0) {
        p_pt->signals_waiting = signals;
    }

    CRITICAL_SECTION_LEAVE(cs_signal);

    return ret_signal;
}

uint32_t backend_assert_signal(struct partition_t *p_pt, psa_signal_t signal)
{
    struct critical_section_t cs_signal = CRITICAL_SECTION_STATIC_INIT;

    if (!p_pt) {
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_signal);
    p_pt->signals_asserted |= signal;
    CRITICAL_SECTION_LEAVE(cs_signal);

    return PSA_SUCCESS;
}

uint64_t ipc_schedule(void)
{
    fih_int fih_rc = FIH_FAILURE;
    AAPCS_DUAL_U32_T ctx_ctrls;
    struct partition_t *p_part_curr, *p_part_next;
    struct context_ctrl_t *p_curr_ctx;
    struct thread_t *pth_next = thrd_next();
    struct critical_section_t cs = CRITICAL_SECTION_STATIC_INIT;

    p_curr_ctx = (struct context_ctrl_t *)(CURRENT_THREAD->p_context_ctrl);

    AAPCS_DUAL_U32_SET(ctx_ctrls, (uint32_t)p_curr_ctx, (uint32_t)p_curr_ctx);

    p_part_curr = GET_CURRENT_COMPONENT();
    p_part_next = GET_THRD_OWNER(pth_next);

    if (scheduler_lock != SCHEDULER_LOCKED && pth_next != NULL &&
        p_part_curr != p_part_next) {
        /* Check if there is enough room on stack to save more context */
        if ((p_curr_ctx->sp_limit +
                sizeof(struct tfm_additional_context_t)) > __get_PSP()) {
            tfm_core_panic();
        }

        CRITICAL_SECTION_ENTER(cs);
        /*
         * If required, let the platform update boundary based on its
         * implementation. Change privilege, MPU or other configurations.
         */
        if (tfm_hal_boundary_need_switch(p_part_curr->boundary,
                                         p_part_next->boundary)) {
            FIH_CALL(tfm_hal_activate_boundary, fih_rc,
                     p_part_next->p_ldinf, p_part_next->boundary);
            if (fih_not_eq(fih_rc, fih_int_encode(TFM_HAL_SUCCESS))) {
                tfm_core_panic();
            }
        }
        ARCH_FLUSH_FP_CONTEXT();

        AAPCS_DUAL_U32_SET_A1(ctx_ctrls, (uint32_t)pth_next->p_context_ctrl);

        CURRENT_THREAD = pth_next;
        CRITICAL_SECTION_LEAVE(cs);
    }

    /* Update meta indicator */
    if (partition_meta_indicator_pos && (p_part_next->p_metadata)) {
        *partition_meta_indicator_pos = (uintptr_t)(p_part_next->p_metadata);
    }
    return AAPCS_DUAL_U32_AS_U64(ctx_ctrls);
}
