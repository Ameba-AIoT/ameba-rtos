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

#include "test_i080.h"

#define DATA_VALUE1 0x1234

#if STATELESS_ROT == 1

const client_test_t test_i080_client_tests_list[] = {
    NULL,
    client_test_app_rot_read_psa_rot_variable,
    client_test_app_rot_write_psa_rot_variable,
    NULL,
};

static int32_t get_secure_partition_address(__attribute__((unused)) psa_handle_t *handle,
                                            addr_t *addr,
                                            driver_test_fn_id_t test_fn_id)
{
   /* Execute driver function related to TEST_ISOLATION_PSA_ROT_* */
      psa_invec invec[1] = { {&test_fn_id, sizeof(test_fn_id)} };
      psa_outvec outvec[1] = { {addr, sizeof(addr_t)} };
      if (psa->call(DRIVER_TEST_HANDLE, PSA_IPC_CALL, invec, 1, outvec, 1) != PSA_SUCCESS)
      {
          val->print(PRINT_ERROR, "\tmsg request failed\n", 0);
          return VAL_STATUS_CALL_FAILED;
      }

      return VAL_STATUS_SUCCESS;
}

static int32_t get_driver_status(psa_handle_t *handle)
{
   if (psa->call(*handle, PSA_IPC_CALL, NULL, 0, NULL, 0) != PSA_SUCCESS)
   {
       return VAL_STATUS_CALL_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}

int32_t client_test_app_rot_read_psa_rot_variable(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint32_t     data = DATA_VALUE1;
   psa_handle_t handle = 0;

   val->print(PRINT_TEST, "[Check 1] Test APP-RoT reading PSA-RoT data\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_DATA_RD)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read PSA RoT global variable address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint32_t *)psa_rot_addr;

   /* Did read ignore? */
   if (data == DATA_VALUE1)
        return VAL_STATUS_SUCCESS;

   val->print(PRINT_ERROR, "\tExpected read to fault but it didn't\n", 0);

   /* Resetting boot.state to catch unwanted reboot */
   if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
       return VAL_STATUS_ERROR;
   }

   return VAL_STATUS_SPM_FAILED;
}

int32_t client_test_app_rot_write_psa_rot_variable(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint32_t     data = DATA_VALUE1;
   psa_handle_t handle = 0;

   val->print(PRINT_TEST, "[Check 2] Test APP-RoT writing PSA-RoT data\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_DATA_WR)))
       return VAL_STATUS_ERROR;

   /* Write PSA RoT global variable address.
    * This should generate internal fault or ignore the write.
    */
   *(uint32_t *)psa_rot_addr = (uint32_t)data;

   /* Handshake with driver to decide write status */
   if (VAL_ERROR(get_driver_status(&handle)))
   {
       return VAL_STATUS_DRIVER_FN_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}

#else

const client_test_t test_i080_client_tests_list[] = {
    NULL,
    client_test_app_rot_read_psa_rot_variable,
    client_test_app_rot_write_psa_rot_variable,
    NULL,
};

static int32_t get_secure_partition_address(psa_handle_t *handle,
                                            addr_t *addr,
                                            driver_test_fn_id_t test_fn_id)
{
   *handle = psa->connect(DRIVER_TEST_SID, DRIVER_TEST_VERSION);
   if (!PSA_HANDLE_IS_VALID(*handle))
   {
       val->print(PRINT_ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* Execute driver function related to TEST_ISOLATION_PSA_ROT_* */
   psa_invec invec[1] = {{&test_fn_id, sizeof(test_fn_id)}};
   psa_outvec outvec[1] = {{addr, sizeof(addr_t)}};
   if (psa->call(*handle, PSA_IPC_CALL, invec, 1, outvec, 1) != PSA_SUCCESS)
   {
       val->print(PRINT_ERROR, "\tmsg request failed\n", 0);
       return VAL_STATUS_CALL_FAILED;
   }

   return VAL_STATUS_SUCCESS;
}

static int32_t get_driver_status(psa_handle_t *handle)
{
   if (psa->call(*handle, PSA_IPC_CALL, NULL, 0, NULL, 0) != PSA_SUCCESS)
   {
       return VAL_STATUS_CALL_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}

static void close_driver_fn(psa_handle_t *handle)
{
   psa->close(*handle);
}

int32_t client_test_app_rot_read_psa_rot_variable(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint32_t     data = DATA_VALUE1;
   psa_handle_t handle = 0;

   val->print(PRINT_TEST, "[Check 1] Test APP-RoT reading PSA-RoT data\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_DATA_RD)))
       return VAL_STATUS_ERROR;

   close_driver_fn(&handle);

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read PSA RoT global variable address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint32_t *)psa_rot_addr;

   /* Did read ignore? */
   if (data == DATA_VALUE1)
        return VAL_STATUS_SUCCESS;

   val->print(PRINT_ERROR, "\tExpected read to fault but it didn't\n", 0);

   /* Resetting boot.state to catch unwanted reboot */
   if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
       return VAL_STATUS_ERROR;
   }

   return VAL_STATUS_SPM_FAILED;
}

int32_t client_test_app_rot_write_psa_rot_variable(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint32_t     data = DATA_VALUE1;
   psa_handle_t handle = 0;

   val->print(PRINT_TEST, "[Check 2] Test APP-RoT writing PSA-RoT data\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_DATA_WR)))
       return VAL_STATUS_ERROR;

   /* Write PSA RoT global variable address.
    * This should generate internal fault or ignore the write.
    */
   *(uint32_t *)psa_rot_addr = (uint32_t)data;

   /* Handshake with driver to decide write status */
   if (VAL_ERROR(get_driver_status(&handle)))
   {
       close_driver_fn(&handle);
       return VAL_STATUS_DRIVER_FN_FAILED;
   }
   close_driver_fn(&handle);
   return VAL_STATUS_SUCCESS;
}

#endif