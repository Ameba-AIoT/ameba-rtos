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

#include "test_i068.h"

#define NO_OF_BYTES 16
typedef void (*fptr_t)(void);
void test_i068_dummy_func(void);
fptr_t fptr;

char opcode[NO_OF_BYTES] = {0}; // Data memory

const client_test_t test_i068_client_tests_list[] = {
    NULL,
    client_test_instr_exec_from_writable_mem,
    NULL,
};

static void copy_mem(void *addr, void *data, size_t l)
{
   int i;
   char *src = (char *)data;
   char *dest = (char *)addr;

   for (i = 0; i < (int)l; i++)
       dest[i] = src[i];
}

/* Empty function to create opcode data set */
void test_i068_dummy_func(void)
{
}

int32_t client_test_instr_exec_from_writable_mem(caller_security_t caller __UNUSED)
{
  val->print(PRINT_TEST, "[Check 1] Test Instr execution from writable memory\n", 0);

  /*
   * Copy test_i068_dummy_func function code into data memory
   * Assuming function size to be 16 bytes max.
   * Remove thumb bit from address if exists.
   */
  copy_mem(&opcode, (void *) ((uintptr_t)&test_i068_dummy_func & ~(uintptr_t)0x1), NO_OF_BYTES);

  /* Point function pointer to data memory */
  fptr = (fptr_t) opcode;
  val->print(PRINT_DEBUG, "\t&opcode = 0x%x\n", (uint32_t) &opcode);
  val->print(PRINT_DEBUG, "\tfptr = 0x%x\n",(uint32_t) fptr);

  /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_S))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

  /* Check - Execute opcode from data memory. This should generate internal fault */
  fptr();

  val->print(PRINT_ERROR, "\tControl shouldn't have reached here\n", 0);

  /* Resetting boot.state to catch unwanted reboot */
  if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
  {
      val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
  }

  return VAL_STATUS_ERROR;
}


