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
#include "test_s005.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 30

const client_test_t s005_storage_test_list[] = {
    NULL,
    s005_storage_test,
    NULL,
};

static uint8_t read_buff[TEST_BUFF_SIZE];
static uint8_t write_buff[TEST_BUFF_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x23, 0xF6, 0x07, 0x08, 0x0D, 0x0A, 0x1B, 0x0C, 0x5D, 0x0E,\
  0x70, 0xA1, 0xFF, 0xFF, 0x14, 0x73, 0x46, 0x97, 0xE8, 0xDD, 0xCA, 0x0B, 0x3C, 0x0D, 0x2E};

static int32_t psa_sst_apis_check(storage_function_code_t    fCode,
                                  psa_storage_uid_t          uid,
                                  uint32_t                   data_len,
                                  uint8_t                    *data_buff,
                                  psa_storage_create_flags_t create_flag)
{
    int32_t status;
    size_t  p_data_length = 0;

    /* Set the UID with the data_len and data_buff */
    status = STORAGE_FUNCTION(s005_data[VAL_TEST_IDX1].api[fCode], uid, data_len, data_buff,
                          create_flag);
    TEST_ASSERT_EQUAL(status, s005_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Call the get function to get the data buffer and match the buffer */
    status = STORAGE_FUNCTION(s005_data[VAL_TEST_IDX2].api[fCode], uid, 0, data_len, read_buff,
                          &p_data_length);
    TEST_ASSERT_EQUAL(status, s005_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));
    TEST_ASSERT_MEMCMP(read_buff, data_buff, data_len, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(p_data_length, data_len, TEST_CHECKPOINT_NUM(4));

    /* Call the get_info function and match the attributes */
    status = STORAGE_FUNCTION(s005_data[VAL_TEST_IDX4].api[fCode], uid, &info);
    TEST_ASSERT_EQUAL(status, s005_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_EQUAL(info.size, data_len, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_EQUAL(info.flags, create_flag, TEST_CHECKPOINT_NUM(7));

    /* Remove the UID  */
    status = STORAGE_FUNCTION(s005_data[VAL_TEST_IDX7].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s005_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(8));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_apis_check_success_case(storage_function_code_t fCode)
{
   psa_storage_uid_t uid      = UID_BASE_VALUE + 4;
   uint32_t          data_len = 0;
   int32_t           status   = VAL_STATUS_SUCCESS;

    /* Calling set function with data_len 1 and valid data pointer */
    val->print(PRINT_TEST, "[Check 1] Set UID with data length zero and call storage APIs\n", 0);
    if (psa_sst_apis_check(fCode, uid, data_len, write_buff, PSA_STORAGE_FLAG_NONE))
    {
        val->print(PRINT_ERROR, "Data Len = %d\n", data_len);
        return VAL_STATUS_ERROR;
    }

    data_len = TEST_BUFF_SIZE/2;
    val->print(PRINT_TEST, "[Check 2] Resetting the length check\n", 0);
    if (psa_sst_apis_check(fCode, uid, data_len, write_buff, PSA_STORAGE_FLAG_NONE))
    {
       val->print(PRINT_ERROR, "Data Len = %d\n", data_len);
       return VAL_STATUS_ERROR;
    }

    return status;
}

int32_t s005_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_apis_check_success_case(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_apis_check_success_case(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
