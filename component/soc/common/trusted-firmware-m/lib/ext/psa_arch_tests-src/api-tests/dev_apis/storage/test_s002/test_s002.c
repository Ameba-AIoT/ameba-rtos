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
#include "test_s002.h"
#include "test_data.h"

#define UID_WRITE_ONCE_1 UID_BASE_VALUE + 1
#define UID_WRITE_ONCE_2 UID_BASE_VALUE + 2
#define TEST_BUFF_SIZE 16

const client_test_t s002_storage_test_list[] = {
    NULL,
    s002_storage_test,
    NULL,
};

static int32_t psa_sst_update_write_once_flag_after_create(storage_function_code_t fCode)
{
    int32_t           status;
    size_t            p_data_length                    = 0;
    psa_storage_uid_t uid                              = UID_WRITE_ONCE_1;
    uint8_t           write_buff[TEST_BUFF_SIZE/2]     = {0xDE, 0xAD, 0xBE, 0xEF,
                                                          0xCA, 0xFE, 0xBA, 0xBE};
    uint8_t           read_buff[TEST_BUFF_SIZE/2]      = {0};
    uint8_t           write_buff_new[TEST_BUFF_SIZE/4] = {0xFF, 0xFF, 0xFF, 0xFF};

    /* set() data without a WRITE_ONCE flag */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX1].api[fCode], uid, TEST_BUFF_SIZE/2,
                              write_buff, PSA_STORAGE_FLAG_NONE);
    if (status == PSA_ERROR_NOT_PERMITTED)
    {
        val->print(PRINT_TEST, "[Info] UID %d was created with PSA_STORAGE_FLAG_WRITE_ONCE "
                               "previously\n", (int32_t)uid);
        val->print(PRINT_TEST, "[Check 1] Retrieve metadata for UID %d and check content\n",
                               (int32_t)uid);
        goto check_write_once_uid_metadata;
    } else
    {
        TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX1].status, TEST_CHECKPOINT_NUM(1));
    }

    /* Check that get_info() returns correct attributes; also store for reference for later */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX2].api[fCode], uid, &orig_info);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX2].status, TEST_CHECKPOINT_NUM(2));
    TEST_ASSERT_EQUAL(orig_info.size, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(orig_info.flags, PSA_STORAGE_FLAG_NONE, TEST_CHECKPOINT_NUM(4));

    /* Check for data consistency using get() */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX5].api[fCode], uid, 0, TEST_BUFF_SIZE/2,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX5].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_MEMCMP(write_buff, read_buff, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(7));

    /* set() with WRITE_ONCE_FLAG */
    val->print(PRINT_TEST, "[Check 1] Update the flag of UID %d with WRITE_ONCE flag\n",
                            (int32_t)uid);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX7].api[fCode], uid, TEST_BUFF_SIZE/4,
                              write_buff_new, PSA_STORAGE_FLAG_WRITE_ONCE);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX7].status, TEST_CHECKPOINT_NUM(8));

check_write_once_uid_metadata:
    /* Check that info is updated, after new set */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX8].api[fCode], uid, &new_info);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX8].status, TEST_CHECKPOINT_NUM(9));
    TEST_ASSERT_EQUAL(new_info.size, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(10));
    TEST_ASSERT_EQUAL(new_info.flags, PSA_STORAGE_FLAG_WRITE_ONCE, TEST_CHECKPOINT_NUM(11));

    /* Check that data contents are preserved which were written with WRITE_ONCE_FLAG originally */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX11].api[fCode], uid, 0, TEST_BUFF_SIZE/4,
                          read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX11].status, TEST_CHECKPOINT_NUM(12));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(13));
    TEST_ASSERT_MEMCMP(write_buff_new, read_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(14));

    /* remove() the UID */
    val->print(PRINT_TEST, "[Check 2] Try to remove the UID %d having WRITE_ONCE flag\n",
                            (int32_t)uid);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX13].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX13].status, TEST_CHECKPOINT_NUM(15));

    return VAL_STATUS_SUCCESS;
}


static int32_t psa_sst_create_with_write_once_flag(storage_function_code_t fCode)
{
    int32_t           status;
    size_t            p_data_length                      = 0;
    psa_storage_uid_t uid                                = UID_WRITE_ONCE_2;
    uint8_t           write_buff[TEST_BUFF_SIZE]         = {0x00, 0x01, 0x02, 0x03,
                                                            0x04, 0x05, 0x06, 0x07,
                                                            0x08, 0x09, 0x0A, 0x0B,
                                                            0x0C, 0x0D, 0x0E, 0x0F};
    uint8_t           read_buff[TEST_BUFF_SIZE]          = {0};
    uint8_t           write_buff_new[TEST_BUFF_SIZE + 1] = {0xFF, 0xFF, 0xFF, 0xFF,
                                                            0xFF, 0xFF, 0xFF, 0xFF,
                                                            0xFF, 0xFF, 0xFF, 0xFF,
                                                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    /* Set data for a UID using WRITE_ONCE flag */
    val->print(PRINT_TEST, "[Check 3] Create a new UID %d with WRITE_ONCE flag\n", (int32_t)uid);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX14].api[fCode], uid, TEST_BUFF_SIZE, write_buff,
                          PSA_STORAGE_FLAG_WRITE_ONCE);
    if (status == PSA_ERROR_NOT_PERMITTED)
    {
        val->print(PRINT_TEST, "[Info ] UID %d was created with PSA_STORAGE_FLAG_WRITE_ONCE "
                               "previously\n", (int32_t)uid);
    } else
    {
        TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX14].status, TEST_CHECKPOINT_NUM(16));
    }

    /* Check that remove() fails with PSA_SST_ERROR_WRITE_ONCE */
    val->print(PRINT_TEST, "[Check 4] Try to remove the UID %d having WRITE_ONCE flag\n",
                            (int32_t)uid);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX15].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX15].status, TEST_CHECKPOINT_NUM(17));

    /* Check data consistency using get()*/
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX16].api[fCode], uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX16].status, TEST_CHECKPOINT_NUM(18));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(19));
    TEST_ASSERT_MEMCMP(write_buff, read_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(20));

    /* Check that info values is as expected */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX18].api[fCode], uid, &orig_info);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX18].status, TEST_CHECKPOINT_NUM(21));
    TEST_ASSERT_EQUAL(orig_info.size, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(22));
    TEST_ASSERT_EQUAL(orig_info.flags, PSA_STORAGE_FLAG_WRITE_ONCE, TEST_CHECKPOINT_NUM(23));

    /* Try to overwrite using set() with same UID as used before with WRITE_ONCE_FLAG */
    val->print(PRINT_TEST, "[Check 5] Try to change the length of write_once UID %d\n",
                            (int32_t)uid);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX21].api[fCode], uid, (TEST_BUFF_SIZE + 1),
                          write_buff_new, PSA_STORAGE_FLAG_WRITE_ONCE);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX21].status, TEST_CHECKPOINT_NUM(24));

    /* Check that remove() still fails */
    val->print(PRINT_TEST, "[Check 6] Check UID removal still fails\n", 0);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX22].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX22].status, TEST_CHECKPOINT_NUM(25));

    /* Check that info is preserved */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX23].api[fCode], uid, &new_info);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX23].status, TEST_CHECKPOINT_NUM(26));
    TEST_ASSERT_EQUAL(new_info.size, orig_info.size, TEST_CHECKPOINT_NUM(27));
    TEST_ASSERT_EQUAL(new_info.flags, orig_info.flags, TEST_CHECKPOINT_NUM(28));

    /* Check that data contents are preserved which were written with WRITE_ONCE_FLAG originally */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX26].api[fCode], uid, 0, TEST_BUFF_SIZE,
                              read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX26].status, TEST_CHECKPOINT_NUM(29));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(30));
    TEST_ASSERT_MEMCMP(write_buff, read_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(31));

    /* Try to overwrite using set() with same UID as used before without WRITE_ONCE_FLAG */
    val->print(PRINT_TEST, "[Check 7] Try to change the WRITE_ONCE flag to None for UID %d\n",
                            (int32_t)uid);
    new_info.size  = 0;
    new_info.flags = 0;
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX28].api[fCode], uid, (TEST_BUFF_SIZE - 1),
                          write_buff_new, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX28].status, TEST_CHECKPOINT_NUM(32));

    /* Check that remove() still fails with */
    val->print(PRINT_TEST, "[Check 8] Check UID removal still fails\n", 0);
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX29].api[fCode], uid);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX29].status, TEST_CHECKPOINT_NUM(33));

    /* Check that info is preserved */
    status = STORAGE_FUNCTION(s002_data[VAL_TEST_IDX30].api[fCode], uid, &new_info);
    TEST_ASSERT_EQUAL(status, s002_data[VAL_TEST_IDX30].status, TEST_CHECKPOINT_NUM(34));
    TEST_ASSERT_EQUAL(new_info.size, orig_info.size, TEST_CHECKPOINT_NUM(35));
    TEST_ASSERT_EQUAL(new_info.flags, orig_info.flags, TEST_CHECKPOINT_NUM(36));

    return VAL_STATUS_SUCCESS;
}

int32_t s002_storage_test(caller_security_t caller __UNUSED)
{
    int32_t status;

#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
    val->print(PRINT_TEST, ITS_TEST_MESSAGE, 0);
    status = psa_sst_update_write_once_flag_after_create(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    status = psa_sst_create_with_write_once_flag(VAL_ITS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

#if defined(STORAGE) || defined(PROTECTED_STORAGE)
    val->print(PRINT_TEST, PS_TEST_MESSAGE, 0);
    status = psa_sst_update_write_once_flag_after_create(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }

    status = psa_sst_create_with_write_once_flag(VAL_PS_FUNCTION);
    if (status != VAL_STATUS_SUCCESS) {
        return status;
    }
#endif

    return VAL_STATUS_SUCCESS;
}
