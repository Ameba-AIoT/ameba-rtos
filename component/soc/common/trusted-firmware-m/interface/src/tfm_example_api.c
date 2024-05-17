/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_example_api.h"
#include "psa_manifest/sid.h"

enum tfm_example_err_t
tfm_example_ioctl(tfm_example_ioctl_req_t request,
                   psa_invec *input, psa_outvec *output)
{
    tfm_example_ioctl_req_t req = request;
    struct psa_invec in_vec[2] = { {0} };
    size_t inlen, outlen;
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;

    in_vec[0].base = &req;
    in_vec[0].len = sizeof(req);
    if (input != NULL) {
        in_vec[1].base = input->base;
        in_vec[1].len = input->len;
        inlen = 2;
    } else {
        inlen = 1;
    }

    if (output != NULL) {
        outlen = 1;
    } else {
        outlen = 0;
    }

    status = psa_call(TFM_PARTITION_EXAMPLE_SERVICE_HANDLE,
                      TFM_EXAMPLE_API_ID_IOCTL,
                      in_vec, inlen,
                      output, outlen);

    if (status < PSA_SUCCESS) {
        return TFM_EXAMPLE_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_example_err_t)status;
    }
}
