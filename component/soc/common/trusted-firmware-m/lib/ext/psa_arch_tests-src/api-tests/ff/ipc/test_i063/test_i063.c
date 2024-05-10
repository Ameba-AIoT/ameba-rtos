/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i063.h"

#if STATELESS_ROT == 1

const client_test_t test_i063_client_tests_list[] = {
    NULL,
    client_test_psa_wait_signal_mask,
    NULL,
};

int32_t client_test_psa_wait_signal_mask(caller_security_t caller __UNUSED)
{
   val->print(PRINT_TEST, "[Check 1] Test psa_wait signal mask\n", 0);

   psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);

   psa->call(SERVER_RELAX_VERSION_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);

   return VAL_STATUS_SUCCESS;
}

#else

const client_test_t test_i063_client_tests_list[] = {
    NULL,
    client_test_psa_wait_signal_mask,
    NULL,
};

int32_t client_test_psa_wait_signal_mask(caller_security_t caller __UNUSED)
{

   psa_handle_t     handle = 0;

   val->print(PRINT_TEST, "[Check 1] Test psa_wait signal mask\n", 0);

   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);

   if (handle != PSA_ERROR_CONNECTION_REFUSED)
   {
       val->print(PRINT_ERROR, "psa_connect failed -1\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   handle = psa->connect(SERVER_RELAX_VERSION_SID, SERVER_RELAX_VERSION_VERSION);

   if (handle != PSA_ERROR_CONNECTION_REFUSED)
   {
       val->print(PRINT_ERROR, "psa_connect failed -2\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }
   return VAL_STATUS_SUCCESS;
}

#endif