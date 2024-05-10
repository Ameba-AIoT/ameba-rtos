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

#include "test_i070.h"

const client_test_t test_i070_client_tests_list[] = {
    NULL,
    client_test_write_to_const_data,
    NULL,
};

int32_t client_test_write_to_const_data(caller_security_t caller __UNUSED)
{
  const char *string = "This text should be in RO space";
  char       *p;

  val->print(PRINT_TEST, "[Check 1] Test write to const data\n", 0);

  /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_S))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

  p = (char *) string;
  val->print(PRINT_DEBUG, "\tstring[0] = 0x%x\n", (uint32_t) string);
  val->print(PRINT_DEBUG, "\tp[0] = 0x%x\n", (uint32_t) p);

  /*
   * Check - Write to const data string[0].
   * This should generate internal fault or write ignored
   */
  p[0] = 'a';

  if (p[0] == 'T')
  {
      /* This means, write ignored */
      return VAL_STATUS_SUCCESS;
  }

  val->print(PRINT_ERROR, "\tWrite to constant data check failed\n", 0);

  /* Resetting boot.state to catch unwanted reboot */
  if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
  {
      val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
  }

  return VAL_STATUS_ERROR;
}


