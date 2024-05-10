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
#include "test_s007.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 30

const client_test_t s007_storage_test_list[] = {
    NULL,
    s007_storage_test,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x23, 0xF6, 0x07, 0x08, 0x0D, 0x0A, 0x1B, 0x0C, 0x5D, 0x0E,\
  0x70, 0xA1, 0xFF, 0xFF, 0x14, 0x73, 0x46, 0x97, 0xE8, 0xDD, 0xCA, 0x0B, 0x3C, 0x0D, 0x2E};
static uint8_t read_buff[TEST_BUFF_SIZE];

static int32_t psa_sst_get_incorrect_size(storage_function_code_t fCode)
{
    psa_storage_uid_t uid           = UID_BASE_VALUE + 5;
    size_t            p_data_length = 0;
    int32_t           status        = VAL_STATUS_SUCCESS;

    /* Set the UID with the data_len and data_buff */
    val->print(PRINT_TEST, "Create a valid Storage - TEST_BUFF_SIZE/2\n", 0);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX1].api[fCode], uid, TEST_BUFF_SIZE/2,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Call set for same UID and increase the length */
    val->print(PRINT_TEST, "Increase the length of storage - TEST_BUFF_SIZE\n", 0);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX2].api[fCode], uid, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Access data using get API and old length */
    val->print(PRINT_TEST, "[Check 1] Call get API with old length - TEST_BUFF_SIZE/2\n", 0);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX3].api[fCode], uid, 0, TEST_BUFF_SIZE/2,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(4));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(5));

    /* Access data using get API and valid length */
    val->print(PRINT_TEST, "[Check 2] Call get API with old length - TEST_BUFF_SIZE/4\n", 0);
    memset(read_buff, 0x0, TEST_BUFF_SIZE);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX4].api[fCode], uid, 0, TEST_BUFF_SIZE/4,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(7));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(8));

    /* Decrease the length again */
    val->print(PRINT_TEST, "Decrease the length of storage - TEST_BUFF_SIZE/4\n", 0);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX6].api[fCode], uid, TEST_BUFF_SIZE/4,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(9));

    /* Access data using get API and old length */
    val->print(PRINT_TEST, "[Check 3] Call get API with old length - TEST_BUFF_SIZE/2\n", 0);
    memset(read_buff, 0x0, TEST_BUFF_SIZE);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX7].api[fCode], uid, 0, TEST_BUFF_SIZE/2,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(10));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(11));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(12));

    /* Access data using get API and old length */
    val->print(PRINT_TEST, "[Check 4] Call get API with old length - TEST_BUFF_SIZE\n", 0);
    memset(read_buff, 0x0, TEST_BUFF_SIZE);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX8].api[fCode], uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(13));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(14));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(15));

    /* Access data using correct length */
    val->print(PRINT_TEST, "[Check 5] Call get API with valid length - TEST_BUFF_SIZE/4\n", 0);
    memset(read_buff, 0x0, TEST_BUFF_SIZE);
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX9].api[fCode], uid, 0, TEST_BUFF_SIZE/4,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX9].status, TEST_CHECKPOINT_NUM(16));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(17));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(18));

    /* Remove the UID  */
    status = STORAGE_FUNCTION(s007_data[VAL_TEST_IDX10].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s007_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(19));

    return status;
}

int32_t s007_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_get_incorrect_size(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_get_incorrect_size(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
