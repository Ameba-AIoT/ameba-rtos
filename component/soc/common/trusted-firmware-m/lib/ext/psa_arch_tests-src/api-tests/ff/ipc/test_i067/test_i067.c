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

#include "test_i067.h"

#ifdef SP_HEAP_MEM_SUPP
void *malloc(size_t size);
void free(void *ptr);
#endif

const client_test_t test_i067_client_tests_list[] = {
    NULL,
    client_test_dynamic_mem_alloc_fn,
    NULL,
};

int32_t client_test_dynamic_mem_alloc_fn(caller_security_t caller __UNUSED)
{
  /* Check heap memory support available to secure partition */
#ifdef SP_HEAP_MEM_SUPP
  uint8_t              *buffer;

  val->print(PRINT_TEST, "[Check 1] Test dynamic memory allocation\n", 0);

  /* If heap_size field is not specified in the manifest then the SPM can assume the size is 0 */
  buffer = (uint8_t *)malloc(sizeof(uint8_t) * 8);
  if (buffer != NULL)
  {
      val->print(PRINT_ERROR, "\tmalloc failed for unspecified heap size\n", 0);
      return VAL_STATUS_SPM_FAILED;
  }

#else

  val->print(PRINT_TEST, "[Check 1] Test dynamic memory allocation\n", 0);

  /* Skip the test */
  val->print(PRINT_ERROR, "\tSkipping test as heap memory not supported\n", 0);
  return RESULT_SKIP(VAL_STATUS_HEAP_NOT_AVAILABLE);

#endif
  return VAL_STATUS_SUCCESS;
}
