/** @file
 * Copyright (c) 2018-2021, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i021.h"

#if STATELESS_ROT == 1

const client_test_t test_i021_client_tests_list[] = {
    NULL,
    client_test_irq_routing,
    NULL,
};

int32_t client_test_irq_routing(caller_security_t caller __UNUSED)
{

   driver_test_fn_id_t    driver_test_fn_id = TEST_INTR_SERVICE;

   /*
    * The interrupt related test check is captured in driver_partition.c as this is the
    * only partition in test suite that holds the interrupt source. The interrupt test check
    * is invoked by client by calling to DRIVER_TEST_SID RoT service of driver partition that
    * hold the test check.
    */

   val->print(PRINT_TEST, "[Check 1] Test irq routing\n", 0);

   /* Execute driver function related to TEST_INTR_SERVICE */
   psa_invec invec = {&driver_test_fn_id, sizeof(driver_test_fn_id)};

   if (psa->call(DRIVER_TEST_HANDLE, PSA_IPC_CALL, &invec, 1, NULL, 0) != PSA_SUCCESS)
   {
           return VAL_STATUS_SPM_FAILED;
   }

   return VAL_STATUS_SUCCESS;
}

#else

const client_test_t test_i021_client_tests_list[] = {
    NULL,
    client_test_irq_routing,
    NULL,
};

int32_t client_test_irq_routing(caller_security_t caller __UNUSED)
{
   psa_handle_t           handle;
   driver_test_fn_id_t    driver_test_fn_id = TEST_INTR_SERVICE;

   /*
    * The interrupt related test check is captured in driver_partition.c as this is the
    * only partition in test suite that holds the interrupt source. The interrupt test check
    * is invoked by client by calling to DRIVER_TEST_SID RoT service of driver partition that
    * hold the test check.
    */

   val->print(PRINT_TEST, "[Check 1] Test irq routing\n", 0);

   /* Connect to DRIVER_TEST_SID */
   handle = psa->connect(DRIVER_TEST_SID, DRIVER_TEST_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       val->print(PRINT_ERROR, "\t psa_connect failed. handle=0x%x\n", handle);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Execute driver function related to TEST_INTR_SERVICE */
   psa_invec invec = {&driver_test_fn_id, sizeof(driver_test_fn_id)};

   if (psa->call(handle, PSA_IPC_CALL, &invec, 1, NULL, 0) != PSA_SUCCESS)
   {
        psa->close(handle);
        return VAL_STATUS_SPM_FAILED;
   }

   psa->close(handle);
   return VAL_STATUS_SUCCESS;
}

#endif
