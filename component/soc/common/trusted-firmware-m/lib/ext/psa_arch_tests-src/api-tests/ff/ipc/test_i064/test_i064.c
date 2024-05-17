/** @file
 * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i064.h"

#if STATELESS_ROT == 1

const client_test_t test_i064_client_tests_list[] = {
    NULL,
    client_test_psa_eoi_with_non_intr_signal,
    NULL,
};

int32_t client_test_psa_eoi_with_non_intr_signal(caller_security_t caller __UNUSED)
{
   driver_test_fn_id_t    driver_test_fn_id = TEST_PSA_EOI_WITH_NON_INTR_SIGNAL;

   /*
    * The interrupt related test check is captured in driver_partition.c as this is the
    * only partition in test suite that holds the interrupt source. The interrupt test check
    * is invoked by client by calling to DRIVER_TEST_SID RoT service of driver partition that
    * hold the test check.
    */

   val->print(PRINT_TEST, "[Check 1] Test psa_eoi with non-interrupt signal\n", 0);

   /* Execute driver function related to TEST_PSA_EOI_WITH_NON_INTR_SIGNAL */
   psa_invec invec = {&driver_test_fn_id, sizeof(driver_test_fn_id)};

   psa->call(DRIVER_TEST_HANDLE, PSA_IPC_CALL, &invec, 1, NULL, 0);

   /* The expectation is that driver partition get panic and control never reaches here. */
   return VAL_STATUS_SPM_FAILED;
}

#else

const client_test_t test_i064_client_tests_list[] = {
    NULL,
    client_test_psa_eoi_with_non_intr_signal,
    NULL,
};

int32_t client_test_psa_eoi_with_non_intr_signal(caller_security_t caller __UNUSED)
{
   psa_handle_t           handle;
   driver_test_fn_id_t    driver_test_fn_id = TEST_PSA_EOI_WITH_NON_INTR_SIGNAL;

   /*
    * The interrupt related test check is captured in driver_partition.c as this is the
    * only partition in test suite that holds the interrupt source. The interrupt test check
    * is invoked by client by calling to DRIVER_TEST_SID RoT service of driver partition that
    * hold the test check.
    */

   val->print(PRINT_TEST, "[Check 1] Test psa_eoi with non-interrupt signal\n", 0);


   /* Connect to DRIVER_TEST_SID */
   handle = psa->connect(DRIVER_TEST_SID, DRIVER_TEST_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       val->print(PRINT_ERROR, "\t psa_connect failed. handle=0x%x\n", handle);
       return VAL_STATUS_SPM_FAILED;
   }

   /* Execute driver function related to TEST_PSA_EOI_WITH_NON_INTR_SIGNAL */
   psa_invec invec = {&driver_test_fn_id, sizeof(driver_test_fn_id)};

   psa->call(handle, PSA_IPC_CALL, &invec, 1, NULL, 0);

   psa->close(handle);

   /* The expectation is that driver partition get panic and control never reaches here. */
   return VAL_STATUS_SPM_FAILED;
}

#endif