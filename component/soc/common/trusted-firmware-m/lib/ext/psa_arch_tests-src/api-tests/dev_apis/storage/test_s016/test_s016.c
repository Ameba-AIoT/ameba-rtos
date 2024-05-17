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
#include "test_s016.h"
#include "test_data.h"

#define TEST_BUFF_SIZE_1 10
#define TEST_BUFF_SIZE_2 14
#define TEST_BUFF_SIZE_3 8

const client_test_t s016_storage_test_list[] = {
    NULL,
    s016_storage_test,
    NULL,
};

static psa_storage_uid_t p_uid                        = UID_BASE_VALUE + 5;
static uint8_t           write_buff[TEST_BUFF_SIZE_2] = {0x03, 0x04, 0x05, 0x07, 0xFF, 0x00, 0x32,
                                                         0x67, 0x99, 0x78, 0x43, 0x20, 0xFF, 0x91};

static int32_t psa_sst_optional_api_sst_capacity(storage_function_code_t fCode)
{
    int32_t status;
    struct psa_storage_info_t info;

    /* Try to create storage using create API */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX1].api[fCode], p_uid, TEST_BUFF_SIZE_1,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Call to create API for existing UID should fail */
    val->print(PRINT_TEST, "[Check 1] Create API call for existing UID\n", 0);
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX2].api[fCode], p_uid, TEST_BUFF_SIZE_2,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Check the attributes of storage are unchanged */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX3].api[fCode], p_uid, &info);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(info.capacity, TEST_BUFF_SIZE_1, TEST_CHECKPOINT_NUM(4));
    TEST_ASSERT_EQUAL(info.size, 0, TEST_CHECKPOINT_NUM(5));

    /* Override storage using set API */
    val->print(PRINT_TEST, "[Check 2] Call to set API call for existing UID\n", 0);
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX6].api[fCode], p_uid, TEST_BUFF_SIZE_3,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(6));

    /* Check the attributes of storage are changed */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX7].api[fCode], p_uid, &info);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(7));
    TEST_ASSERT_EQUAL(info.capacity, TEST_BUFF_SIZE_3, TEST_CHECKPOINT_NUM(8));
    TEST_ASSERT_EQUAL(info.size, TEST_BUFF_SIZE_3, TEST_CHECKPOINT_NUM(9));

    /* Accessing old capacity with set_extended API should fail */
    val->print(PRINT_TEST, "[Check 3] Access old capacity using set_extended API\n", 0);
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX10].api[fCode], p_uid, 0, TEST_BUFF_SIZE_1,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(10));

    /* Remove the storage */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX11].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(11));

    /* Create new storage assest using set  API */
    val->print(PRINT_TEST, "[Check 4] Creation of new storage assest\n", 0);
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX12].api[fCode], p_uid, TEST_BUFF_SIZE_2,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX12].status, TEST_CHECKPOINT_NUM(12));

    /* Call to create API for existing UID should fail */
    val->print(PRINT_TEST, "[Check 5] Create API call for existing UID\n", 0);
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX13].api[fCode], p_uid, TEST_BUFF_SIZE_2,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(13));

    /* Remove the storage */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX14].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s016_data[VAL_TEST_IDX14].status, TEST_CHECKPOINT_NUM(14));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_optional_api_sst_capacity_check(storage_function_code_t fCode)
{
    int32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = STORAGE_FUNCTION(s016_data[VAL_TEST_IDX0].api[fCode]);

    if (status != s016_data[VAL_TEST_IDX0].status)
    {
       val->print(PRINT_TEST, "Test Case skipped as Optional PS APIs not are supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    } else {
        val->print(PRINT_TEST, "Optional PS APIs are supported.\n", 0);
        test_status = psa_sst_optional_api_sst_capacity(fCode);
        if (test_status != VAL_STATUS_SUCCESS)
        {
            return test_status;
        }
    }

    return VAL_STATUS_SUCCESS;
}

int32_t s016_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_optional_api_sst_capacity_check(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    return VAL_STATUS_SUCCESS;
}
