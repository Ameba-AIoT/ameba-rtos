/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RUNTIME_DEFS_H__
#define __RUNTIME_DEFS_H__

#include <stdint.h>

#include "config_impl.h"
#include "psa/client.h"
#include "psa/error.h"
#include "psa/service.h"

/* SFN defs */
typedef psa_status_t (*service_fn_t)(psa_msg_t *msg);
typedef psa_status_t (*sfn_init_fn_t)(void *param);

/* PSA API dispatcher for IPC model. */
#if CONFIG_TFM_SPM_BACKEND_IPC == 1
struct psa_api_tbl_t {
    psa_status_t     (*psa_call)(psa_handle_t handle, uint32_t ctrl_param, const psa_invec *in_vec,
                                 psa_outvec *out_vec);
    uint32_t         (*psa_version)(uint32_t sid);
    uint32_t         (*psa_framework_version)(void);
    psa_signal_t     (*psa_wait)(psa_signal_t signal_mask, uint32_t timeout);
    psa_status_t     (*psa_get)(psa_signal_t signal, psa_msg_t *msg);
    size_t           (*psa_read)(psa_handle_t msg_handle, uint32_t invec_idx, void *buffer,
                                 size_t num_bytes);
    size_t           (*psa_skip)(psa_handle_t msg_handle, uint32_t invec_idx, size_t num_bytes);
    void             (*psa_write)(psa_handle_t msg_handle, uint32_t outvec_idx, const void *buffer,
                                  size_t num_bytes);
    void             (*psa_reply)(psa_handle_t msg_handle, psa_status_t retval);
    void             (*psa_panic)(void);
    uint32_t         (*psa_rot_lifecycle_state)(void);
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
    psa_handle_t     (*psa_connect)(uint32_t sid, uint32_t version);
    void             (*psa_close)(psa_handle_t handle);
    void             (*psa_set_rhandle)(psa_handle_t msg_handle, void *rhandle);
#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1 */
#if CONFIG_TFM_DOORBELL_API == 1
    void             (*psa_notify)(int32_t partition_id);
    void             (*psa_clear)(void);
#endif /* CONFIG_TFM_DOORBELL_API == 1 */
#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
    void             (*psa_irq_enable)(psa_signal_t irq_signal);
    psa_irq_status_t (*psa_irq_disable)(psa_signal_t irq_signal);
#if CONFIG_TFM_FLIH_API == 1
    void             (*psa_reset_signal)(psa_signal_t irq_signal);
#endif /* CONFIG_TFM_FLIH_API == 1 */
#if CONFIG_TFM_SLIH_API == 1
    void             (*psa_eoi)(psa_signal_t irq_signal);
#endif /* CONFIG_TFM_SLIH_API == 1 */
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */
#if PSA_FRAMEWORK_HAS_MM_IOVEC
    void *           (*psa_map_invec)(psa_handle_t msg_handle, uint32_t invec_idx);
    void             (*psa_unmap_invec)(psa_handle_t msg_handle, uint32_t invec_idx);
    void *           (*psa_map_outvec)(psa_handle_t msg_handle, uint32_t outvec_idx);
    void             (*psa_unmap_outvec)(psa_handle_t msg_handle, uint32_t outvec_idx, size_t len);
#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */
};

struct runtime_metadata_t {
    uintptr_t            entry;      /* Entry function */
    struct psa_api_tbl_t *psa_fns;   /* PSA API entry table */
    uint32_t             n_sfn;      /* Number of Secure FuNctions */
    service_fn_t         sfn_table[];/* Secure FuNctions Table */
};
#endif /* CONFIG_TFM_SPM_BACKEND_IPC == 1 */

#endif /* __RUNTIME_DEFS_H__ */
