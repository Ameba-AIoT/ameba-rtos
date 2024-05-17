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
#include "test_s001.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 16

static struct psa_storage_info_t info;
static uint8_t                   write_buff[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03,
                                                               0x04, 0x05, 0x06, 0x07,
                                                               0x08, 0x09, 0x0A, 0x0B,
                                                               0x0C, 0x0D, 0x0E, 0x0F};
static uint8_t                   read_buff[TEST_BUFF_SIZE]  = {0};
const client_test_t              s001_storage_test_list[] = {
    NULL,
    s001_storage_test,
    NULL,
};

static int32_t sst_calls_without_set_call(storage_function_code_t fCode, psa_storage_uid_t p_uid)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* get() without using set() before */
    val->print(PRINT_TEST, "[Check 1] Call get API for UID %d which is not set\n", (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX1].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /*  get_info() without using set() before */
    val->print(PRINT_TEST, "[Check 2] Call get_info API for UID %d which is not set\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX2].api[fCode], p_uid, &info);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));

    /* remove() without using set() before */
    val->print(PRINT_TEST, "[Check 3] Call remove API for UID %d which is not set\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX3].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX3].status, TEST_CHECKPOINT_NUM(3));

    return VAL_STATUS_SUCCESS;
}

static int32_t sst_set_and_remove(storage_function_code_t fCode, psa_storage_uid_t p_uid)
{
    int32_t status;

    /* set() a UID1 */
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX4].api[fCode], p_uid, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(4));

    /* Also set() with a different UID */
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX5].api[fCode], p_uid + 1, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(5));

    /* remove() UID1 */
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX6].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(6));

    return VAL_STATUS_SUCCESS;
}

static int32_t sst_calls_after_uid_remove(storage_function_code_t fCode, psa_storage_uid_t p_uid)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* get() for UID which is removed */
    val->print(PRINT_TEST, "[Check 4] Call get API for UID %d which is removed\n", (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX7].api[fCode], p_uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(7));

    /* get_info() for UID which is removed */
    val->print(PRINT_TEST, "[Check 5] Call get_info API for UID %d which is removed\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX8].api[fCode], p_uid, &info);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(8));

    /* remove() for UID which is removed */
    val->print(PRINT_TEST, "[Check 6] Call remove API for UID %d which is removed\n",
                            (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX9].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX9].status, TEST_CHECKPOINT_NUM(9));

    return VAL_STATUS_SUCCESS;
}

static int32_t sst_calls_with_different_uid(storage_function_code_t fCode, psa_storage_uid_t p_uid)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* set() a UID */
    val->print(PRINT_TEST, "Set storage for UID %d\n", (int32_t)p_uid);
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX10].api[fCode], p_uid, TEST_BUFF_SIZE,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(10));

    /* get() for different UID then set UID */
    val->print(PRINT_TEST, "[Check 7] Call get API for different UID %d\n", (int32_t)(p_uid-1));
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX11].api[fCode], p_uid-1, 0, TEST_BUFF_SIZE - 1,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(11));

    /* get_info() for different UID then set UID */
    val->print(PRINT_TEST, "[Check 8] Call get_info API for different UID %d\n",
                            (int32_t)(p_uid-1));
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX12].api[fCode], p_uid-1, &info);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX12].status, TEST_CHECKPOINT_NUM(12));

    /* remove() for different UID then set UID */
    val->print(PRINT_TEST, "[Check 9] Call remove API for different UID %d\n", (int32_t)(p_uid-1));
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX13].api[fCode], p_uid-1);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(13));

    /* remove() the set UID */
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX14].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX14].status, TEST_CHECKPOINT_NUM(14));

    return VAL_STATUS_SUCCESS;
}

static int32_t sst_remove_stray_uid(storage_function_code_t fCode, psa_storage_uid_t p_uid)
{
    int32_t status;

    /* Remove UID + 1 */
    status = STORAGE_FUNCTION(s001_data[VAL_TEST_IDX15].api[fCode], p_uid);
    TEST_ASSERT_EQUAL(status, s001_data[VAL_TEST_IDX15].status, TEST_CHECKPOINT_NUM(15));
    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_uid_not_found(storage_function_code_t fCode)
{
    int32_t test_status;
    psa_storage_uid_t uid = UID_BASE_VALUE + 6;

    test_status = sst_calls_without_set_call(fCode, uid);
    if (test_status != VAL_STATUS_SUCCESS)
        return test_status;

    test_status = sst_set_and_remove(fCode, uid);
    if (test_status != VAL_STATUS_SUCCESS)
        return test_status;

    test_status = sst_calls_after_uid_remove(fCode, uid);
    if (test_status != VAL_STATUS_SUCCESS)
        return test_status;

    test_status = sst_calls_with_different_uid(fCode, uid);
    if (test_status != VAL_STATUS_SUCCESS)
        return test_status;

    test_status = sst_remove_stray_uid(fCode, (uid + 1));
    if (test_status != VAL_STATUS_SUCCESS)
        return test_status;

    return VAL_STATUS_SUCCESS;
}

int32_t s001_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_uid_not_found(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_uid_not_found(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
