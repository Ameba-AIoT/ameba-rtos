/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "psa/client.h"
#include "tfm_erpc_psa_client_api.h"

psa_status_t psa_call(psa_handle_t handle, int32_t type,
                      const psa_invec *in_vec, size_t in_len,
                      psa_outvec *out_vec, size_t out_len)
{
    psa_status_t status;
    size_t i;
    binary_t in_elements[PSA_MAX_IOVEC];
    binary_t out_elements[PSA_MAX_IOVEC];
    list_binary_1_t erpc_in_vec = {in_elements, in_len};
    list_binary_1_t erpc_out_vec = {out_elements, out_len};

    if (in_len + out_len > PSA_MAX_IOVEC) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Copy PSA iovecs into RPC binary lists */
    for (i = 0; i < in_len; ++i) {
        in_elements[i] = (binary_t){(void *)in_vec[i].base, in_vec[i].len};
    }
    for (i = 0; i < out_len; ++i) {
        out_elements[i] = (binary_t){out_vec[i].base, out_vec[i].len};
    }

    status = erpc_psa_call(handle, type, &erpc_in_vec, &erpc_out_vec);

    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Copy updated out length into PSA outvec */
    for (i = 0; i < out_len; ++i) {
        out_vec[i].len = out_elements[i].dataLength;
    }

    return status;
}
