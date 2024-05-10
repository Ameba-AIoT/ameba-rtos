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
#include "utilities.h"
#include "tfm_hal_isolation.h"

size_t tfm_spm_partition_psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                                  void *buffer, size_t num_bytes)
{
    size_t bytes;
    struct connection_t *handle = NULL;
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE));
#endif

    /* There was no remaining data in this input vector */
    if (handle->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * Copy the client data to the service buffer. It is a fatal error
     * if the memory reference for buffer is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)buffer,
             num_bytes, TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    bytes = num_bytes > handle->msg.in_size[invec_idx] ?
                        handle->msg.in_size[invec_idx] : num_bytes;

    spm_memcpy(buffer, handle->invec[invec_idx].base, bytes);

    /* There maybe some remaining data */
    handle->invec[invec_idx].base =
                                (char *)handle->invec[invec_idx].base + bytes;
    handle->msg.in_size[invec_idx] -= bytes;

    return bytes;
}

size_t tfm_spm_partition_psa_skip(psa_handle_t msg_handle, uint32_t invec_idx,
                                  size_t num_bytes)
{
    struct connection_t *handle = NULL;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE));
#endif

    /* There was no remaining data in this input vector */
    if (handle->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * If num_bytes is greater than the remaining size of the input vector then
     * the remaining size of the input vector is used.
     */
    if (num_bytes > handle->msg.in_size[invec_idx]) {
        num_bytes = handle->msg.in_size[invec_idx];
    }

    /* There maybe some remaining data */
    handle->invec[invec_idx].base =
                            (char *)handle->invec[invec_idx].base + num_bytes;
    handle->msg.in_size[invec_idx] -= num_bytes;

    return num_bytes;
}

void tfm_spm_partition_psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                                 const void *buffer, size_t num_bytes)
{
    struct connection_t *handle = NULL;
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the call attempts to write data past the end of
     * the client output vector
     */
    if (num_bytes > handle->msg.out_size[outvec_idx] -
        handle->outvec[outvec_idx].len) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the output vector has already been mapped using
     * psa_map_outvec().
     */
    if (IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (outvec_idx + OUTVEC_IDX_BASE));
#endif

    /*
     * Copy the service buffer to client outvecs. It is a fatal error
     * if the memory reference for buffer is invalid or not readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)buffer,
             num_bytes, TFM_HAL_ACCESS_READABLE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    spm_memcpy((char *)handle->outvec[outvec_idx].base +
               handle->outvec[outvec_idx].len, buffer, num_bytes);

    /* Update the write number */
    handle->outvec[outvec_idx].len += num_bytes;
}
