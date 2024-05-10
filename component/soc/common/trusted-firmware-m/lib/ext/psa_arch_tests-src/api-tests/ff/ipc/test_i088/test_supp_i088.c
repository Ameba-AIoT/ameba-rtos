/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_client_defs.h"
#include "val_service_defs.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

int32_t server_test_psa_rot_lifecycle_state(void);

const server_test_t test_i088_server_tests_list[] = {
    NULL,
    server_test_psa_rot_lifecycle_state,
    NULL,
};

int32_t server_test_psa_rot_lifecycle_state(void)
{
    uint32_t state;
    int32_t  status = VAL_STATUS_UNSUPPORTED;

    val->print(PRINT_TEST, "[Check 1] Test calling psa_rot_lifecycle_state API\n", 0);

    state = psa->rot_lifecycle_state();
    switch (state & PSA_LIFECYCLE_PSA_STATE_MASK)
    {
        case PSA_LIFECYCLE_UNKNOWN:
        case PSA_LIFECYCLE_ASSEMBLY_AND_TEST:
        case PSA_LIFECYCLE_PSA_ROT_PROVISIONING:
        case PSA_LIFECYCLE_SECURED:
        case PSA_LIFECYCLE_NON_PSA_ROT_DEBUG:
        case PSA_LIFECYCLE_RECOVERABLE_PSA_ROT_DEBUG:
        case PSA_LIFECYCLE_DECOMMISSIONED:
            status = VAL_STATUS_SUCCESS;
            break;
        default:
            val->print(PRINT_ERROR, "\tpsa_rot_lifecycle_state API returned"
                                    "unsupported value\n", 0);
    }

    return status;
}
