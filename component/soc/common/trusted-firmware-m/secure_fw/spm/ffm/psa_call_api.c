/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "config_impl.h"
#include "critical_section.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "tfm_hal_isolation.h"
#include "tfm_psa_call_pack.h"
#include "utilities.h"

extern struct service_t *stateless_services_ref_tbl[];

psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                     uint32_t ctrl_param,
                                     const psa_invec *inptr,
                                     psa_outvec *outptr)
{
    psa_invec invecs[PSA_MAX_IOVEC];
    psa_outvec outvecs[PSA_MAX_IOVEC];
    struct connection_t *p_connection;
    struct service_t *service;
    int i, j;
    int32_t client_id;
    uint32_t sid, version, index;
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    bool ns_caller = tfm_spm_is_ns_caller();
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    int32_t type = (int32_t)(int16_t)((ctrl_param & TYPE_MASK) >> TYPE_OFFSET);
    size_t in_num = (size_t)((ctrl_param & IN_LEN_MASK) >> IN_LEN_OFFSET);
    size_t out_num = (size_t)((ctrl_param & OUT_LEN_MASK) >> OUT_LEN_OFFSET);
    fih_int fih_rc = FIH_FAILURE;

    /* The request type must be zero or positive. */
    if (type < 0) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* It is a PROGRAMMER ERROR if in_len + out_len > PSA_MAX_IOVEC. */
    if ((in_num > SIZE_MAX - out_num) ||
        (in_num + out_num > PSA_MAX_IOVEC)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* It is a PROGRAMMER ERROR if the handle is a null handle. */
    if (handle == PSA_NULL_HANDLE) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    client_id = tfm_spm_get_client_id(ns_caller);

    /* Allocate space from handle pool for static handle. */
    if (IS_STATIC_HANDLE(handle)) {
        index = GET_INDEX_FROM_STATIC_HANDLE(handle);

        service = stateless_services_ref_tbl[index];
        if (!service) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        sid = service->p_ldinf->sid;

        /*
         * It is a PROGRAMMER ERROR if the caller is not authorized to access
         * the RoT Service.
         */
        if (tfm_spm_check_authorization(sid, service, ns_caller)
            != PSA_SUCCESS) {
            return PSA_ERROR_CONNECTION_REFUSED;
        }

        version = GET_VERSION_FROM_STATIC_HANDLE(handle);

        if (tfm_spm_check_client_version(service, version) != PSA_SUCCESS) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        CRITICAL_SECTION_ENTER(cs_assert);
        p_connection = spm_allocate_connection();
        CRITICAL_SECTION_LEAVE(cs_assert);

        if (!p_connection) {
            return PSA_ERROR_CONNECTION_BUSY;
        }

#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
        p_connection->rhandle = NULL;
#endif
        handle = connection_to_handle(p_connection);
    } else {
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
        /* It is a PROGRAMMER ERROR if an invalid handle was passed. */
        p_connection = spm_get_client_connection(handle, client_id);
        if (!p_connection) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        /*
         * It is a PROGRAMMER ERROR if the connection is currently
         * handling a request.
         */
        if (p_connection->status != TFM_HANDLE_STATUS_IDLE) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        service = p_connection->service;

        if (!service) {
            /* FixMe: Need to implement a mechanism to resolve this failure. */
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
#else
        return PSA_ERROR_PROGRAMMER_ERROR;
#endif
    }

    /*
     * Read client invecs from the wrap input vector. It is a PROGRAMMER ERROR
     * if the memory reference for the wrap input vector is invalid or not
     * readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)inptr,
             in_num * sizeof(psa_invec), TFM_HAL_ACCESS_READABLE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /*
     * Read client outvecs from the wrap output vector and will update the
     * actual length later. It is a PROGRAMMER ERROR if the memory reference for
     * the wrap output vector is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)outptr,
             out_num * sizeof(psa_outvec), TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    spm_memset(invecs, 0, sizeof(invecs));
    spm_memset(outvecs, 0, sizeof(outvecs));

    /* Copy the address out to avoid TOCTOU attacks. */
    spm_memcpy(invecs, inptr, in_num * sizeof(psa_invec));
    spm_memcpy(outvecs, outptr, out_num * sizeof(psa_outvec));

    /*
     * For client input vector, it is a PROGRAMMER ERROR if the provided payload
     * memory reference was invalid or not readable.
     */
    for (i = 0; i < in_num; i++) {
        FIH_CALL(tfm_hal_memory_check, fih_rc,
                 curr_partition->boundary, (uintptr_t)invecs[i].base,
                 invecs[i].len, TFM_HAL_ACCESS_READABLE);
        if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
    }

    /*
     * Clients must never overlap input parameters because of the risk of a
     * double-fetch inconsistency.
     * Overflow is checked in tfm_hal_memory_check functions.
     */
    for (i = 0; i + 1 < in_num; i++) {
        for (j = i+1; j < in_num; j++) {
            if (!((char *) invecs[j].base + invecs[j].len <=
                  (char *) invecs[i].base ||
                  (char *) invecs[j].base >=
                  (char *) invecs[i].base + invecs[i].len)) {
                return PSA_ERROR_PROGRAMMER_ERROR;
            }
        }
    }

    /*
     * For client output vector, it is a PROGRAMMER ERROR if the provided
     * payload memory reference was invalid or not read-write.
     */
    for (i = 0; i < out_num; i++) {
        FIH_CALL(tfm_hal_memory_check, fih_rc,
                 curr_partition->boundary, (uintptr_t)outvecs[i].base,
                 outvecs[i].len, TFM_HAL_ACCESS_READWRITE);
        if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
    }

    spm_fill_message(p_connection, service, handle, type, client_id);
    for (i = 0; i < in_num; i++) {
        p_connection->msg.in_size[i] = invecs[i].len;
        p_connection->invec[i].base = invecs[i].base;
    }

    for (i = 0; i < out_num; i++) {
        p_connection->msg.out_size[i] = outvecs[i].len;
        p_connection->outvec[i].base = outvecs[i].base;
        /* Out len is used to record the written number, set 0 here again */
        p_connection->outvec[i].len = 0;
    }
    p_connection->caller_outvec = outptr;

    return backend_messaging(service, p_connection);
}
