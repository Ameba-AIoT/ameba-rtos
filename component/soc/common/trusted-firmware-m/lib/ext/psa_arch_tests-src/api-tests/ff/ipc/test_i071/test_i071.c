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

#include "test_i071.h"

#define BUFF_SIZE 32

const client_test_t test_i071_client_tests_list[] = {
    NULL,
    client_test_mem_manipulation_fn,
    NULL,
};

int32_t client_test_mem_manipulation_fn(caller_security_t caller __UNUSED)
{
  uint8_t    buffer[BUFF_SIZE] = {0};
  uint8_t    buffer1[BUFF_SIZE] = {0};
  int         i;

  val->print(PRINT_TEST, "[Check 1] Test memory manipulation functions\n", 0);

  /* Test memset()- Set all buffer elements  with zero */
  memset((uint8_t *)buffer, 0, BUFF_SIZE);
  for (i = 0; i < BUFF_SIZE; i++)
  {
      if (buffer[i] != 0)
      {
        val->print(PRINT_ERROR,
                  "\tmemset() failed, found buffer with non-zero value = %x\n", buffer[i]);
        return VAL_STATUS_ERROR;
      }
  }

  /* Test memset()- Set all buffer elements  with 0x3 */
  memset((uint8_t *)buffer, 0x3, BUFF_SIZE);
  for (i = 0; i < BUFF_SIZE; i++)
  {
      if (buffer[i] != 0x3)
      {
        val->print(PRINT_ERROR,
                  "\tmemset() failed, found buffer with wrong value = %x\n", buffer[i]);
        return VAL_STATUS_ERROR;
      }
  }

  /* Test memcpy(), copy buffer to buffer1 */
  memcpy((uint8_t *)buffer1, (uint8_t *)buffer, BUFF_SIZE);
  for (i = 0; i < BUFF_SIZE; i++)
  {
      if (buffer1[i] != 0x3)
      {
         val->print(PRINT_ERROR,
                  "\tmemcpy() failed, found buffer with wrong value = %x\n", buffer1[i]);
         return VAL_STATUS_ERROR;
      }
  }

  /* Test memcmp() with equal buffer */
  if (memcmp((uint8_t *)buffer1, (uint8_t *)buffer, BUFF_SIZE))
  {
      val->print(PRINT_ERROR,
                 "\tmemcmp() failed for two equal buffer\n", 0);
      return VAL_STATUS_ERROR;
  }

  buffer1[0] = 0x4;
  /* Test memcmp() with unequal buffer */
  if (!memcmp((uint8_t *)buffer1, (uint8_t *)buffer, BUFF_SIZE))
  {
      val->print(PRINT_ERROR,
                 "\tmemcmp() failed for two unequal buffer\n", 0);
      return VAL_STATUS_ERROR;
  }

  /* buffer[0-4] = 1 and buffer[5-31] = 2 */
  memset((uint8_t *)buffer, 1, BUFF_SIZE);
  memset((uint8_t *)buffer+5, 2, BUFF_SIZE-5);

  /* Test memmove(), expected result is buffer[0-9] = 1 and buffer[10-31] = 2 */
  memmove((uint8_t *)buffer+5, (uint8_t *)buffer, BUFF_SIZE-5);
  for (i = 0; i < BUFF_SIZE; i++)
  {
      if ((buffer[i] != 0x1) && (i < 10))
      {
          val->print(PRINT_ERROR, "\tmemmove() failed-1\n", 0);
          return VAL_STATUS_ERROR;
      }
      else if ((buffer[i] != 0x2) && (i >10))
      {
          val->print(PRINT_ERROR, "\tmemmove() failed-2\n", 0);
          return VAL_STATUS_ERROR;
      }
  }

  return VAL_STATUS_SUCCESS;
}


