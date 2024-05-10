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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i069.h"

const client_test_t test_i069_client_tests_list[] = {
    NULL,
    client_test_write_to_code_space,
    NULL,
};

int32_t client_test_write_to_code_space(caller_security_t caller __UNUSED)
{
  int32_t *p;

  val->print(PRINT_TEST, "[Check 1] Test write to code space\n", 0);

  /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_S))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

  p = (int32_t *)&client_test_write_to_code_space;

  /* Check - Write to code memory. This should generate internal fault */
  *p = 0x0;

  if (*p == (int32_t)client_test_write_to_code_space)
  {
      /* This means, write ignored */
      return VAL_STATUS_SUCCESS;
  }
  val->print(PRINT_ERROR, "\tWrite to code memory check failed\n", 0);

  /* Resetting boot.state to catch unwanted reboot */
  if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
  {
      val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
  }

  return VAL_STATUS_ERROR;
}


