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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_s003.h"
#include "test_data.h"

#define NUM_ITERATIONS 2
#define TEST_BASE_UID_VALUE UID_BASE_VALUE + 5

const client_test_t s003_storage_test_list[] = {
    NULL,
    s003_storage_test,
    NULL,
};

static uint8_t      write_buff[ARCH_TEST_STORAGE_UID_MAX_SIZE];
static char         test_desc[2][80] = {
                                "Overload storage space\n",
                                "Overload storage again to verify all previous UID removed\n"};

int32_t psa_sst_insufficient_space(storage_function_code_t fCode)
{
    int32_t           status                  = PSA_SUCCESS;
    psa_storage_uid_t uid;
    uint32_t          count                   = 0;
    uint32_t          results[NUM_ITERATIONS] = {0};
    int               i                       = 0;

    /* Saturate the storage for NUM_ITERATION times, and remove them after */
    for (i = 0 ; i < NUM_ITERATIONS; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", i + 1);
        val->print(PRINT_TEST, &test_desc[i][0], 0);
        for (uid = TEST_BASE_UID_VALUE; status == PSA_SUCCESS; uid++)
        {
            val->print(PRINT_INFO, "Setting 0x%x bytes for ", ARCH_TEST_STORAGE_UID_MAX_SIZE);
            val->print(PRINT_INFO, "UID %d\n", (int32_t)uid);
            status = STORAGE_FUNCTION(s003_data[VAL_TEST_IDX1].api[fCode], uid,
                                      ARCH_TEST_STORAGE_UID_MAX_SIZE, write_buff,
                                      PSA_STORAGE_FLAG_NONE);
            if (status != PSA_SUCCESS)
            {
                val->print(PRINT_TEST, "UID %d set failed due to insufficient space\n",
                                       (int32_t)uid);
                break;
            }
        }
        TEST_ASSERT_EQUAL(status, s003_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

        /* Store number of set()s it took to saturate the storage */
        count = (uint32_t)uid - (TEST_BASE_UID_VALUE);
        results[i] = count;

        if (count) {
           val->print(PRINT_TEST, "Remove all registered UIDs\n", 0);
        }
        for (uid = TEST_BASE_UID_VALUE; uid < (count + TEST_BASE_UID_VALUE); uid++)
        {
            val->print(PRINT_INFO, "Removing UID %d\n", (int32_t)uid);
            status = STORAGE_FUNCTION(s003_data[VAL_TEST_IDX2].api[fCode], uid);
            if (status != PSA_SUCCESS)
                break;
        }
        if (count) {
           TEST_ASSERT_EQUAL(status, s003_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));
        }
    }

    /* Check that it takes equal number of UIDs to fill up the storage each time */
    for (i = 0; i < (NUM_ITERATIONS -1); i++)
    {
        if (results[i] != results[i+1])
        {
            val->print(PRINT_ERROR, "\tERROR : Mismatch between number of UIDs required to\n", 0);
            val->print(PRINT_ERROR, "\t fill up the storage between iteration %d", i);
            val->print(PRINT_ERROR, " and iteration %d\n", i+1);
            return VAL_STATUS_ERROR;
        }
    }
    return VAL_STATUS_SUCCESS;
}

int32_t s003_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_insufficient_space(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_insufficient_space(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
