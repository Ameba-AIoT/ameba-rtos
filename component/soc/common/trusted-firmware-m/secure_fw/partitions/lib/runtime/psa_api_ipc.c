/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "psa/client.h"
#include "config_impl.h"
#include "tfm_psa_call_pack.h"
#include "sprt_partition_metadata_indicator.h"
#include "runtime_defs.h"

uint32_t psa_framework_version(void)
{
    return PART_METADATA()->psa_fns->psa_framework_version();
}

uint32_t psa_version(uint32_t sid)
{
    return PART_METADATA()->psa_fns->psa_version(sid);
}

psa_status_t tfm_psa_call_pack(psa_handle_t handle,
                               uint32_t ctrl_param,
                               const psa_invec *in_vec,
                               psa_outvec *out_vec)
{
    return PART_METADATA()->psa_fns->psa_call(handle, ctrl_param,
                                              in_vec, out_vec);
}

psa_signal_t psa_wait(psa_signal_t signal_mask, uint32_t timeout)
{
    return PART_METADATA()->psa_fns->psa_wait(signal_mask, timeout);
}

psa_status_t psa_get(psa_signal_t signal, psa_msg_t *msg)
{
    return PART_METADATA()->psa_fns->psa_get(signal, msg);
}

size_t psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                void *buffer, size_t num_bytes)
{
    return PART_METADATA()->psa_fns->psa_read(msg_handle, invec_idx, buffer, num_bytes);
}

size_t psa_skip(psa_handle_t msg_handle, uint32_t invec_idx, size_t num_bytes)
{
    return PART_METADATA()->psa_fns->psa_skip(msg_handle, invec_idx, num_bytes);
}

void psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
               const void *buffer, size_t num_bytes)
{
    PART_METADATA()->psa_fns->psa_write(msg_handle, outvec_idx, buffer, num_bytes);
}

void psa_reply(psa_handle_t msg_handle, psa_status_t retval)
{
    PART_METADATA()->psa_fns->psa_reply(msg_handle, retval);
}

void psa_panic(void)
{
    PART_METADATA()->psa_fns->psa_panic();
}

uint32_t psa_rot_lifecycle_state(void)
{
    return PART_METADATA()->psa_fns->psa_rot_lifecycle_state();
}

#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
psa_handle_t psa_connect(uint32_t sid, uint32_t version)
{
    return PART_METADATA()->psa_fns->psa_connect(sid, version);
}

void psa_close(psa_handle_t handle)
{
   PART_METADATA()->psa_fns->psa_close(handle);
}

void psa_set_rhandle(psa_handle_t msg_handle, void *rhandle)
{
    PART_METADATA()->psa_fns->psa_set_rhandle(msg_handle, rhandle);
}
#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1 */

#if CONFIG_TFM_DOORBELL_API == 1
void psa_notify(int32_t partition_id)
{
    PART_METADATA()->psa_fns->psa_notify(partition_id);
}

void psa_clear(void)
{
    PART_METADATA()->psa_fns->psa_clear();
}
#endif /* CONFIG_TFM_DOORBELL_API == 1 */

#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
void psa_irq_enable(psa_signal_t irq_signal)
{
    PART_METADATA()->psa_fns->psa_irq_enable(irq_signal);
}

psa_irq_status_t psa_irq_disable(psa_signal_t irq_signal)
{
    return PART_METADATA()->psa_fns->psa_irq_disable(irq_signal);
}

#if CONFIG_TFM_FLIH_API == 1
void psa_reset_signal(psa_signal_t irq_signal)
{
   PART_METADATA()->psa_fns->psa_reset_signal(irq_signal);
}
#endif /* CONFIG_TFM_FLIH_API == 1 */

#if CONFIG_TFM_SLIH_API == 1
void psa_eoi(psa_signal_t irq_signal)
{
    PART_METADATA()->psa_fns->psa_eoi(irq_signal);
}
#endif /* CONFIG_TFM_SLIH_API == 1 */
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */

#if PSA_FRAMEWORK_HAS_MM_IOVEC
const void *psa_map_invec(psa_handle_t msg_handle, uint32_t invec_idx)
{
    return PART_METADATA()->psa_fns->psa_map_invec(msg_handle, invec_idx);
}

void psa_unmap_invec(psa_handle_t msg_handle, uint32_t invec_idx)
{
    PART_METADATA()->psa_fns->psa_unmap_invec(msg_handle, invec_idx);
}

void *psa_map_outvec(psa_handle_t msg_handle, uint32_t outvec_idx)
{
    PART_METADATA()->psa_fns->psa_map_outvec(msg_handle, outvec_idx);
}

void psa_unmap_outvec(psa_handle_t msg_handle, uint32_t outvec_idx, size_t len)
{
    PART_METADATA()->psa_fns->psa_unmap_outvec(msg_handle, outvec_idx, len);
}
#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */
