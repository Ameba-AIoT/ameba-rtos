/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_ps_test_service_api.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

__attribute__((section("SFN")))
psa_status_t tfm_ps_test_system_prepare(void)
{
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(TFM_PS_TEST_PREPARE_SID,
                         TFM_PS_TEST_PREPARE_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
    psa_close(handle);

    return status;
}
