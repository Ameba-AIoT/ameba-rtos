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
#include "test_s008.h"
#include "test_data.h"

#define TEST_BUFF_SIZE 20
#define TEST_MAX_UINT32 0xFFFFFFFF

const client_test_t s008_storage_test_list[] = {
    NULL,
    s008_storage_test,
    NULL,
};

static psa_storage_uid_t uid = UID_BASE_VALUE + 5;
static uint8_t           read_buff[TEST_BUFF_SIZE];
static uint8_t           write_buff[TEST_BUFF_SIZE] = {0x99, 0x01, 0x02, 0x03, 0x04,
                                                       0x23, 0xF6, 0x07, 0x08, 0x0D,
                                                       0x70, 0xA1, 0xFF, 0xFF, 0x14,
                                                      0x73, 0x46, 0x97, 0xE8, 0xDD};

static int32_t psa_sst_invalid_offset_failure(storage_function_code_t fCode)
{
    uint32_t j;
    size_t   p_data_length = 0;
    int32_t  status;

    /* Case where offset = data_size +1 , data_len 0. Also check nothing is returned in read buff */
    val->print(PRINT_TEST, "[Check 2] Try to access data with varying invalid offset\n", 0);
    memset(read_buff, 0, TEST_BUFF_SIZE);
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX6].api[fCode], uid, TEST_BUFF_SIZE+1, 0,
                              read_buff, &p_data_length);
    TEST_ASSERT_NOT_EQUAL(status, s008_data[VAL_TEST_IDX6].status, TEST_CHECKPOINT_NUM(8));
    TEST_ASSERT_EQUAL(p_data_length, 0, TEST_CHECKPOINT_NUM(9));
    for (j = 0; j < TEST_BUFF_SIZE; j++)
    {
        TEST_ASSERT_EQUAL(read_buff[j], 0x00, TEST_CHECKPOINT_NUM(10));
    }

    /* Case where offset = data_size  , data_len= 1  Also check nothing is returned in read buff */
    memset(read_buff, 0xCD, TEST_BUFF_SIZE);
    p_data_length = 0xFF;
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX8].api[fCode], uid, TEST_BUFF_SIZE, 1,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(11));
    TEST_ASSERT_EQUAL(p_data_length, 0, TEST_CHECKPOINT_NUM(12));

    /* Case where offset = 0, data_len > data_size, check if just data_size is returned */
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX10].api[fCode], uid, 0, TEST_BUFF_SIZE+1,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX10].status, TEST_CHECKPOINT_NUM(13));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(14));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(15));

    /* Try to access data with offset as MAX_UINT32 and length less than buffer size */
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX12].api[fCode], uid, TEST_MAX_UINT32,
                          TEST_BUFF_SIZE/2, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX12].status, TEST_CHECKPOINT_NUM(16));

    /* Remove the UID */
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX13].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(17));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_valid_offset_success(storage_function_code_t fCode)
{
    int32_t  status;
    uint32_t data_len, offset = TEST_BUFF_SIZE;
    size_t   p_data_length = 0;

    /* Set data for UID */
    status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX1].api[fCode], uid, TEST_BUFF_SIZE, write_buff,
                          PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));

    /* Case where offset + datalen =  data_size */
    val->print(PRINT_TEST, "[Check 1] Try to access data with varying valid offset\n", 0);
    while (offset > 0)
    {
         data_len = TEST_BUFF_SIZE - offset;
         memset(read_buff, 0, TEST_BUFF_SIZE);
         status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX2].api[fCode], uid, offset, data_len,
                               read_buff, &p_data_length);
         TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));
         TEST_ASSERT_MEMCMP(read_buff, write_buff + offset, data_len, TEST_CHECKPOINT_NUM(3));
         TEST_ASSERT_EQUAL(p_data_length, data_len, TEST_CHECKPOINT_NUM(4));
         offset >>= 1;
     }

    offset = TEST_BUFF_SIZE - 2;
    data_len = 1;
    /* Case where offset + datalen <  data_size */
    while (offset > 0)
    {
         status = STORAGE_FUNCTION(s008_data[VAL_TEST_IDX4].api[fCode], uid, offset, data_len,
                               read_buff, &p_data_length);
         TEST_ASSERT_EQUAL(status, s008_data[VAL_TEST_IDX4].status, TEST_CHECKPOINT_NUM(5));
         TEST_ASSERT_MEMCMP(read_buff, write_buff + offset, data_len, TEST_CHECKPOINT_NUM(6));
         TEST_ASSERT_EQUAL(p_data_length, data_len, TEST_CHECKPOINT_NUM(7));
         offset >>= 1;
         data_len <<= 1;
     }

    return VAL_STATUS_SUCCESS;
}

int32_t s008_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_valid_offset_success(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    status = psa_sst_invalid_offset_failure(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_valid_offset_success(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    status = psa_sst_invalid_offset_failure(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
