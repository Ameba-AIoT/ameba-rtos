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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_s012.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 16

const client_test_t s012_storage_test_list[] = {
    NULL,
    s012_storage_test,
    NULL,
};

static psa_storage_uid_t p_uid                        = UID_BASE_VALUE + 6;
static uint8_t           write_buff[TEST_BUFF_SIZE]   = {0x00, 0x01, 0x02, 0x03,
                                                         0x04, 0x05, 0x06, 0x07,
                                                         0x08, 0x09, 0x0A, 0x0B,
                                                         0x0C, 0x0D, 0x0E, 0x0F};
static uint8_t           read_buff[TEST_BUFF_SIZE]    = {0};
static uint8_t           write_buff_2[TEST_BUFF_SIZE] = {0xFF, 0xC1, 0xA2, 0xE3,
                                                         0x04, 0x05, 0x06, 0x07,
                                                         0x03, 0x09, 0x0A, 0x1B,
                                                         0x0C, 0x0D, 0x0E, 0x0F};

static int32_t psa_sst_offset_invalid(storage_function_code_t fCode)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* Create valid storage using create API */
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX1].api[fCode], p_uid, TEST_BUFF_SIZE,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Set some data in the storage created */
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX2].api[fCode], p_uid, 0, TEST_BUFF_SIZE/2,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Try to set data at invalid location with incorrect data len + offset  */
    val->print(PRINT_TEST, "[Check 1] Call set_extended API with invalid offset + length\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX3].api[fCode], p_uid, TEST_BUFF_SIZE, 2,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));

    /* Try to set data at invalid location with incorrect offset */
    val->print(PRINT_TEST, "[Check 2] Call set_extended API with invalid offset\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX4].api[fCode], p_uid, TEST_BUFF_SIZE + 2, 0,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(4));

    /* Try to set data at correct offset, but zero data len */
    val->print(PRINT_TEST, "[Check 3] Call set_extended API with offset equals length\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX5].api[fCode], p_uid, TEST_BUFF_SIZE/2, 0,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(5));

    /* Try to set data at invalid location with incorrect data len + offset */
    val->print(PRINT_TEST, "[Check 4] Call set_extended API with invalid offset + length\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX6].api[fCode], p_uid, 1, TEST_BUFF_SIZE,
                              write_buff);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(6));

    /* Set data using set API */
    val->print(PRINT_TEST, "[Check 5] Overwrite the whole data with set API\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX7].api[fCode], p_uid, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(7));

    /* Call the get function to check data is correctly overwritten */
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX8].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(8));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(9));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(10));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_bad_pointer(storage_function_code_t fCode)
{
    int32_t            status;
    psa_storage_uid_t  uid = 0;

    /* Call create API with UID value 0 */
    val->print(PRINT_TEST, "[Check 6] Call create API with UID 0\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX10].api[fCode], uid, TEST_BUFF_SIZE,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(11));

    /* Call set extended API with UID value 0 */
    val->print(PRINT_TEST, "[Check 7] Call set_extended API with UID 0\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX11].api[fCode], uid, 0, TEST_BUFF_SIZE,
                              write_buff_2);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(12));

    /* Call remove API with UID value 0 */
    val->print(PRINT_TEST, "[Check 8] Call remove API with UID 0\n", 0);
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX12].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX12].status, TEST_CHECKPOINT_NUM(13));

    /* Remove the UID */
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX13].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s012_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(14));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_optional_api_offset_invalid(storage_function_code_t fCode)
{
    int32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = STORAGE_FUNCTION(s012_data[VAL_TEST_IDX0].api[fCode]);

    if (status == s012_data[VAL_TEST_IDX0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       test_status = psa_sst_offset_invalid(fCode);
       if (test_status != VAL_STATUS_SUCCESS)
          return test_status;

       test_status = psa_sst_bad_pointer(fCode);
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

int32_t s012_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_optional_api_offset_invalid(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    return VAL_STATUS_SUCCESS;
}
