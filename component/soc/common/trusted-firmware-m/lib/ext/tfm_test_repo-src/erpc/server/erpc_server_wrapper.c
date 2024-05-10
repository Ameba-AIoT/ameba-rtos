/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "psa/client.h"
#include "tfm_erpc_psa_client_api.h"

psa_status_t erpc_psa_call(psa_handle_t handle, int32_t t,
                           const list_binary_1_t *erpc_in_vec,
                           list_binary_1_t *erpc_out_vec)
{
    psa_status_t status;
    psa_invec in_vec[PSA_MAX_IOVEC];
    psa_outvec out_vec[PSA_MAX_IOVEC];
    size_t i;
    size_t in_len = erpc_in_vec->elementsCount;
    size_t out_len = erpc_out_vec->elementsCount;

    /* Copy RPC iovecs into PSA iovecs */
    for (i = 0; i < in_len; ++i) {
        in_vec[i] = (psa_invec){erpc_in_vec->elements[i].data,
                                erpc_in_vec->elements[i].dataLength};
    }

    for (i = 0; i < out_len; ++i) {
        out_vec[i] = (psa_outvec){erpc_out_vec->elements[i].data,
                                  erpc_out_vec->elements[i].dataLength};
    }

    status = psa_call(handle, t, in_vec, in_len, out_vec, out_len);

    /* Copy updated PSA outvec lens into RPC outvecs */
    for (i = 0; i < out_len; ++i) {
        erpc_out_vec->elements[i].dataLength = out_vec[i].len;
    }

    return status;
}
