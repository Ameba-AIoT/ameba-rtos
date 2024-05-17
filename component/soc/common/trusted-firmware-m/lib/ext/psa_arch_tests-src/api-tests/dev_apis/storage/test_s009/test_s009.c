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
#include "test_s009.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 20

const client_test_t s009_storage_test_list[] = {
    NULL,
    s009_storage_test,
    NULL,
};

static psa_storage_uid_t uid                        = UID_BASE_VALUE + 5;
static uint8_t           write_buff[TEST_BUFF_SIZE] = {0x99, 0x01, 0x02, 0x03, 0x04,
                                                       0x23, 0xF6, 0x07, 0x08, 0x0D,
                                                       0x70, 0xA1, 0xFF, 0xFF, 0x14,
                                                       0x73, 0x46, 0x97, 0xE8, 0xDD};

static int32_t psa_sst_zero_length_check(storage_function_code_t fCode)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* Set data for UID with length 0 and NULL pointer */
    val->print(PRINT_TEST, "[Check 1] Call set API with NULL pointer and data length 0\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX1].api[fCode], uid, 0, NULL,
                              PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Call the get_info function to verify UID created */
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX2].api[fCode], uid, &info);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* Call get API with NULL read buffer */
    val->print(PRINT_TEST, "[Check 2] Call get API with NULL read buffer and data length 0\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX3].api[fCode], uid, 0, 0, NULL, &p_data_length);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(p_data_length, 0, TEST_CHECKPOINT_NUM(4));

    /* Remove the UID */
    val->print(PRINT_TEST, "[Check 3] Remove the UID\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX4].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(5));

    /* Call the get_info function to verify UID is removed */
    val->print(PRINT_TEST, "[Check 4] Call get_info API to verify UID removed\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX5].api[fCode], uid, &info);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(6));

    /* Create UID with length 0 and valid write buffer */
    val->print(PRINT_TEST, "[Check 5] Create UID with zero data_len and valid write buffer\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX6].api[fCode], uid, 0, write_buff,
                          PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(7));

    /* Call the get_info function and match the attributes */
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX7].api[fCode], uid, &info);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(8));
    TEST_ASSERT_EQUAL(info.size, 0, TEST_CHECKPOINT_NUM(9));

    /* Call get API with NULL read buffer and valid UID */
    val->print(PRINT_TEST, "[Check 8] Call get API with NULL read buffer and data length 0\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX9].api[fCode], uid, 0, 0, NULL, &p_data_length);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX9].status, TEST_CHECKPOINT_NUM(10));
    TEST_ASSERT_EQUAL(p_data_length, 0, TEST_CHECKPOINT_NUM(11));

    /* Change the length to test_buff_size */
    val->print(PRINT_TEST, "[Check 9] Increase the length\n", 0);
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX10].api[fCode], uid, TEST_BUFF_SIZE, write_buff,
                          PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(12));

    /* Remove the UID */
    status = STORAGE_FUNCTION(s009_data[VAL_TEST_IDX11].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s009_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(13));

    return VAL_STATUS_SUCCESS;
}

int32_t s009_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_zero_length_check(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_zero_length_check(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
