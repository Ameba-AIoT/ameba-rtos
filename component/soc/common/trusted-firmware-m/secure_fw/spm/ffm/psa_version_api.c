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
#include "load/service_defs.h"
#include "spm.h"

uint32_t tfm_spm_client_psa_framework_version(void)
{
    return PSA_FRAMEWORK_VERSION;
}

uint32_t tfm_spm_client_psa_version(uint32_t sid)
{
    struct service_t *service;
    bool ns_caller = tfm_spm_is_ns_caller();

    /*
     * It should return PSA_VERSION_NONE if the RoT Service is not
     * implemented.
     */
    service = tfm_spm_get_service_by_sid(sid);
    if (!service) {
        return PSA_VERSION_NONE;
    }

    /*
     * It should return PSA_VERSION_NONE if the caller is not authorized
     * to access the RoT Service.
     */
    if (tfm_spm_check_authorization(sid, service, ns_caller) != PSA_SUCCESS) {
        return PSA_VERSION_NONE;
    }

    return service->p_ldinf->version;
}
