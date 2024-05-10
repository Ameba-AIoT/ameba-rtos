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
#include "test_s015.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 5

const client_test_t s015_storage_test_list[] = {
    NULL,
    s015_storage_test,
    NULL,
};

static psa_storage_uid_t p_uid = UID_BASE_VALUE + 5;

static int32_t psa_sst_create_api_write_once_flag_check(storage_function_code_t fCode)
{
    int32_t status;

    /* Call the get_support API and check if optional APIs are supported */
    status = STORAGE_FUNCTION(s015_data[VAL_TEST_IDX0].api[fCode]);

    if (status == s015_data[VAL_TEST_IDX0].status)
    {
        /* Call create API with write_once flag */
        val->print(PRINT_TEST, "[Check 1] Call create API with write_once flag\n", 0);
        status = STORAGE_FUNCTION(s015_data[VAL_TEST_IDX1].api[fCode], p_uid, TEST_BUFF_SIZE,
                                  PSA_STORAGE_FLAG_WRITE_ONCE);
        TEST_ASSERT_EQUAL(status, s015_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));
    }
    else
    {
        val->print(PRINT_TEST, "Test Case skipped as Optional PS APIs are not supported.\n", 0);
        return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}

int32_t s015_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_create_api_write_once_flag_check(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    return VAL_STATUS_SUCCESS;
}
