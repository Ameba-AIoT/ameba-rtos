/** @file
 * Copyright (c) 2018-2022, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c014.h"
#include "test_data.h"

const client_test_t test_c014_crypto_list[] = {
    NULL,
    psa_hash_finish_test,
    psa_hash_finish_inactive_operation_handle,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_hash_finish_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_hash_operation_t    operation;
    size_t                  expected_hash_length;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);
        memset(&operation, 0, sizeof(operation));

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Start a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation, check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Add a message fragment to a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                      check1[i].input, check1[i].input_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Finish the calculation of the hash of a message */
        status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &operation, check1[i].expected_hash,
                                      check1[i].hash_size, &expected_hash_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status == PSA_SUCCESS)
        {
            TEST_ASSERT_EQUAL(expected_hash_length, check1[i].hash_size, TEST_CHECKPOINT_NUM(6));
            TEST_ASSERT_MEMCMP(check1[i].expected_hash, check1[i].verify_hash,
                               expected_hash_length, TEST_CHECKPOINT_NUM(7));
        } else {
            /*Abort the hash operation */
            status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));
        }

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_finish_inactive_operation_handle(caller_security_t caller __UNUSED)
{
    psa_hash_operation_t    operation    = PSA_HASH_OPERATION_INIT;
    size_t                  expected_hash_length;
    int32_t                 status;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "test psa_hash_finish with inactive operation handle\n", 0);

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation,
                                  check1[valid_test_input_index].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Add a message fragment to a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE,
                                  &operation,
                                  check1[valid_test_input_index].input,
                                  check1[valid_test_input_index].input_length);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    /* Finish the calculation of the hash of a message */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH,
                                  &operation,
                                  check1[valid_test_input_index].expected_hash,
                                  check1[valid_test_input_index].hash_size,
                                  &expected_hash_length);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    /* Retry the operation with completed operation handle */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH,
                                  &operation,
                                  check1[valid_test_input_index].expected_hash,
                                  check1[valid_test_input_index].hash_size,
                                  &expected_hash_length);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(6));

    /*Abort the hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

    return VAL_STATUS_SUCCESS;
}
