/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "ffm/psa_api.h"
#include "spm.h"
#include "load/service_defs.h"
#include "utilities.h"
#include "tfm_hal_isolation.h"

const void *tfm_spm_partition_psa_map_invec(psa_handle_t msg_handle,
                                            uint32_t invec_idx)
{
    struct connection_t *handle;
    struct partition_t *partition = NULL;
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    partition = handle->service->partition;

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* It is a fatal error if the input vector has length zero. */
    if (handle->msg.in_size[invec_idx] == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been accessed
     * using psa_read() or psa_skip().
     */
    if (IOVEC_IS_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the memory reference for the wrap input vector is
     * invalid or not readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             partition->boundary, (uintptr_t)handle->invec[invec_idx].base,
             handle->invec[invec_idx].len, TFM_HAL_ACCESS_READABLE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    SET_IOVEC_MAPPED(handle, (invec_idx + INVEC_IDX_BASE));

    return handle->invec[invec_idx].base;
}

void tfm_spm_partition_psa_unmap_invec(psa_handle_t msg_handle,
                                       uint32_t invec_idx)
{
    struct connection_t *handle;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if The input vector has not been mapped by a call to
     * psa_map_invec().
     */
    if (!IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been unmapped by a
     * call to psa_unmap_invec().
     */
    if (IOVEC_IS_UNMAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_UNMAPPED(handle, (invec_idx + INVEC_IDX_BASE));
}

void *tfm_spm_partition_psa_map_outvec(psa_handle_t msg_handle,
                                       uint32_t outvec_idx)
{
    struct connection_t *handle;
    struct partition_t *partition = NULL;
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    partition = handle->service->partition;

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* It is a fatal error if the output vector has length zero. */
    if (handle->msg.out_size[outvec_idx] == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been mapped using
     * psa_map_outvec().
     */
    if (IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been accessed
     * using psa_write().
     */
    if (IOVEC_IS_ACCESSED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             partition->boundary, (uintptr_t)handle->outvec[outvec_idx].base,
             handle->outvec[outvec_idx].len, TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }
    SET_IOVEC_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE));

    return handle->outvec[outvec_idx].base;
}

void tfm_spm_partition_psa_unmap_outvec(psa_handle_t msg_handle,
                                        uint32_t outvec_idx, size_t len)
{
    struct connection_t *handle;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if len is greater than the output vector size.
     */
    if (len > handle->msg.out_size[outvec_idx]) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if The output vector has not been mapped by a call to
     * psa_map_outvec().
     */
    if (!IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been unmapped by a
     * call to psa_unmap_outvec().
     */
    if (IOVEC_IS_UNMAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_UNMAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE));

    /* Update the write number */
    handle->outvec[outvec_idx].len = len;
}
