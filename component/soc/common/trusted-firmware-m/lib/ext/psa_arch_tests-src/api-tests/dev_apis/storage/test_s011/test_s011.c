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
#include "test_s011.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 16

const client_test_t s011_storage_test_list[] = {
    NULL,
    s011_storage_test,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
static uint8_t read_buff[TEST_BUFF_SIZE]  = {0};

static int32_t psa_sst_uid_not_found(storage_function_code_t fCode)
{
    int32_t                   status;
    uint32_t                  j;
    size_t                    p_data_length = 0;
    psa_storage_uid_t         p_uid         = UID_BASE_VALUE + 5;
    struct psa_storage_info_t orig_info;

    /* Call the set_extended API with UID which is not created */
    val->print(PRINT_TEST, "[Check 1] Call set_extended API for UID %d which is not set\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX1].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Create a valid storage with set API */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX2].api[fCode], p_uid, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Try to change data length for same UID using create API */
    val->print(PRINT_TEST, "[Check 2] Call create API with length different than original\n", 0);
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX3].api[fCode], p_uid, TEST_BUFF_SIZE/2,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));

    /* Try to change flag value associated with the UID */
    val->print(PRINT_TEST, "[Check 3] Call create API with flag value different than original\n",
                           0);
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX4].api[fCode], p_uid, TEST_BUFF_SIZE,
                              PSA_STORAGE_FLAG_WRITE_ONCE);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(4));

    /* Check the flag value should be same as original*/
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX5].api[fCode], p_uid, &orig_info);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_EQUAL(orig_info.size, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_EQUAL(orig_info.flags, PSA_STORAGE_FLAG_NONE, TEST_CHECKPOINT_NUM(7));

    /* Remove the UID */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX8].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(8));

    /* Create a valid storage */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX9].api[fCode], p_uid, TEST_BUFF_SIZE/2,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX9].status, TEST_CHECKPOINT_NUM(9));

    /* Try to change length using create API */
    val->print(PRINT_TEST, "[Check 4] Call create API with parameters different than original\n",
                           0);
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX10].api[fCode], p_uid, TEST_BUFF_SIZE,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(10));

    /* Check the storage should be empty */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX11].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(11));
    for (j = 0; j < TEST_BUFF_SIZE; j++)
    {
        TEST_ASSERT_EQUAL(read_buff[j], 0, TEST_CHECKPOINT_NUM(12));
    }
    TEST_ASSERT_EQUAL(p_data_length, 0, TEST_CHECKPOINT_NUM(13));

    /* Remove the UID */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX13].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(14));

    /* Call the set_extended API with UID which is removed */
    val->print(PRINT_TEST, "[Check 5] Call set_extended API for UID %d which is removed\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX14].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s011_data[VAL_TEST_IDX14].status, TEST_CHECKPOINT_NUM(15));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_optional_api_uid_not_found(storage_function_code_t fCode)
{
    int32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = STORAGE_FUNCTION(s011_data[VAL_TEST_IDX0].api[fCode]);

    if (status == s011_data[VAL_TEST_IDX0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       test_status = psa_sst_uid_not_found(fCode);
       if (test_status != VAL_STATUS_SUCCESS)
          return test_status;
    }
    else
    {
       val->print(PRINT_TEST, "Test Case skipped as Optional PS APIs are not supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}

int32_t s011_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_optional_api_uid_not_found(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    return VAL_STATUS_SUCCESS;
}
