/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_test_common.h"

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 2),
                  CREATE_TEST_TITLE("Check Arm v8M security extensions and  MPU implemented"),
                  CREATE_REF_TAG("R001/R002/R030_TBSA_BASE"),
                  entry_hook,
                  test_payload,
                  exit_hook);

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t read_val;
    cpuid_t  cpuid;
    bool_t   mpu_preset = FALSE;

    val->mem_reg_read(CPUID, &read_val);
    cpuid.id = read_val;

    switch(cpuid.id_s.arc)
    {
        case ARMV8M_MAINLINE:
            val->print(PRINT_DEBUG, "ARMv8-M33\n", 0);
            break;
        case ARMV8M_BASELINE:
            val->print(PRINT_DEBUG, "ARMv8-M23\n", 0);
            break;
        default:
            val->print(PRINT_ERROR, "Unknown architecture\n", 0);
            val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
            return;
    }

    /* Check for Security Extensions */
    if (is_sec_ext_enabled()) {
        val->print(PRINT_DEBUG, "Security Extension implemented\n", 0);
    } else {
        val->print(PRINT_ERROR, "Security Extension not implemented\n", 0);
        val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
        return;
    }

    /* Checking for Secure MPU ? */
    val->mem_reg_read(MPU_TYPE, &read_val);
    if (read_val == 0) {
        val->print(PRINT_DEBUG, "\n\tSecure side of MPU is not implemented", 0);
    } else {
        mpu_preset = TRUE;
        val->print(PRINT_DEBUG, "\n\tSecure side of MPU is implemented", 0);
    }

    /* Checking for Non-secure MPU ? */
    val->mem_reg_read(MPU_TYPE_NS, &read_val);
    if (read_val == 0) {
        val->print(PRINT_DEBUG, "\n\tNon-secure side of MPU is not implemented", 0);
    } else {
        mpu_preset = TRUE;
        val->print(PRINT_DEBUG, "\n\tNon-secure side of MPU is implemented", 0);
    }

    if (mpu_preset) {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        val->set_status(RESULT_PASS(TBSA_STATUS_NOT_FOUND));
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
