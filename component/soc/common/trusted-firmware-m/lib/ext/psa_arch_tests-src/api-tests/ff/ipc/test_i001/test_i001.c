/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i001.h"

const client_test_t test_i001_client_tests_list[] = {
    NULL,
    client_test_psa_framework_version,
    client_test_psa_version,
    NULL,
};

int32_t client_test_psa_framework_version(caller_security_t caller __UNUSED)
{
   int32_t         status = VAL_STATUS_SUCCESS;

   val->print(PRINT_TEST, "[Check 1] psa_framework_version\n", 0);

   /* Retrieve the version of the PSA Framework API that is implemented.*/
   if (psa->framework_version() != PSA_FRAMEWORK_VERSION)
   {
       status = VAL_STATUS_FRAMEWORK_VERSION_FAILED;
       val->print(PRINT_ERROR,
            "\tpsa_framework_version API failed, Returned=0x%x\n", (psa->framework_version()));
   }

   return status;
}

int32_t client_test_psa_version(caller_security_t caller)
{
   int32_t         status = VAL_STATUS_SUCCESS;
   uint32_t        version;

   val->print(PRINT_TEST, "[Check 2] psa_version\n", 0);

   /*
    * Return PSA_VERSION_NONE when the RoT Service is not implemented,
    * or the caller is not permitted to access the service.
    * Return version of the implemented and allowed RoT Service
    */

   /* psa_version() check for un-implemented SID */
   if (psa->version(INVALID_SID) != PSA_VERSION_NONE)
   {
       status = VAL_STATUS_VERSION_API_FAILED;
   }

   if (val->err_check_set(TEST_CHECKPOINT_NUM(101), status))
   {
       return status;
   }

   /* psa_version() check for implemented SID but allows only secure connection */
   version = psa->version(SERVER_SECURE_CONNECT_ONLY_SID);
   if (((caller == CALLER_NONSECURE) && (version != PSA_VERSION_NONE))
       || ((caller == CALLER_SECURE) && (version != SERVER_SECURE_CONNECT_ONLY_VERSION)))
   {
       status = VAL_STATUS_VERSION_API_FAILED;
       val->print(PRINT_ERROR,
            "\tpsa_version API failed for SID which allows secure only connection\n", 0);
   }

   if (val->err_check_set(TEST_CHECKPOINT_NUM(102), status))
   {
       return status;
   }

   /* psa_version() returns version of the implemented and allowed RoT Service */
   if (psa->version(SERVER_TEST_DISPATCHER_SID) != SERVER_TEST_DISPATCHER_VERSION)
   {
       status = VAL_STATUS_VERSION_API_FAILED;
   }
   return status;
}
