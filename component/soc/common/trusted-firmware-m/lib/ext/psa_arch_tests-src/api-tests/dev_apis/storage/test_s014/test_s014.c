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
#include "test_s014.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 16

const client_test_t s014_storage_test_list[] = {
    NULL,
    s014_storage_test,
    NULL,
};

static psa_storage_uid_t p_uid                        = UID_BASE_VALUE + 5;
static uint8_t           write_buff[TEST_BUFF_SIZE/4] = { 0x03, 0x04, 0x05, 0x07};
static uint8_t           read_buff[TEST_BUFF_SIZE/4]  = {0};

static int32_t psa_sst_optional_api_not_supported(storage_function_code_t fCode)
{
    int32_t status;
    size_t  p_data_length = 0;
    struct  psa_storage_info_t info;

    /* Try to create storage using create API */
    val->print(PRINT_TEST, "[Check 1] Call to create API should fail as API not supported\n", 0);
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX1].api[fCode], p_uid, TEST_BUFF_SIZE,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Verify that UID doesn't exist */
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX2].api[fCode], p_uid, &info);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Create a valid storage using set API */
    val->print(PRINT_TEST, "[Check 2] Create valid storage with set API\n", 0);
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX3].api[fCode], p_uid, TEST_BUFF_SIZE/4,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));

    /* Partial data write with set_extended API should fail */
    val->print(PRINT_TEST, "[Check 3] Call to set_extended API call should fail\n", 0);
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX4].api[fCode], p_uid, 0, 2, write_buff);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(4));

    /* Call the get function to match the data */
    val->print(PRINT_TEST, "[Check 4] Verify data is unchanged\n", 0);
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX5].api[fCode], p_uid, 0, TEST_BUFF_SIZE/4,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(7));

    /* Remove the storage */
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX7].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s014_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(8));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_optional_api_not_supported_check(storage_function_code_t fCode)
{
    int32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = STORAGE_FUNCTION(s014_data[VAL_TEST_IDX0].api[fCode]);

    if (status == s014_data[VAL_TEST_IDX0].status)
    {
       val->print(PRINT_INFO, "Test Case skipped as Optional PS APIs are supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }
    else
    {
        val->print(PRINT_TEST, "Optional PS APIs are not supported.\n", 0);
        test_status = psa_sst_optional_api_not_supported(fCode);
        if (test_status != VAL_STATUS_SUCCESS)
           return test_status;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t s014_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_optional_api_not_supported_check(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    return VAL_STATUS_SUCCESS;
}
