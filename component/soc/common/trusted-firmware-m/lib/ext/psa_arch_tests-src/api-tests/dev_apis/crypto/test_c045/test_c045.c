/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c045.h"
#include "test_data.h"

const client_test_t test_c045_crypto_list[] = {
    NULL,
    psa_hash_clone_test,
    psa_hash_clone_with_inactive_source_operation,
    psa_hash_clone_with_active_target_operation,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_hash_clone_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    size_t                  get_source_hash_length;
    size_t                  get_target_hash_length;
    psa_hash_operation_t    source_operation;
    psa_hash_operation_t    target_operation;

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
        memset(&source_operation, 0, sizeof(source_operation));
        memset(&target_operation, 0, sizeof(target_operation));
        get_source_hash_length = 0;
        get_target_hash_length = 0;

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Start a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &source_operation, check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Clone a hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        /* Add a message fragment to a multipart for source_operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE,
                                      &source_operation,
                                      check1[i].input,
                                      check1[i].input_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Add a message fragment to a multipart for target_operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE,
                                      &target_operation,
                                      check1[i].input,
                                      check1[i].input_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Finish the calculation of the hash of a message for source operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_FINISH,
                                      &source_operation,
                                      check1[i].source_hash,
                                      check1[i].source_hash_size,
                                      &get_source_hash_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        /* Finish the calculation of the hash of a message for target operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_FINISH,
                                      &target_operation,
                                      check1[i].target_hash,
                                      check1[i].target_hash_size,
                                      &get_target_hash_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Check for expected hash length for source and target operation */
        TEST_ASSERT_EQUAL(get_source_hash_length, check1[i].expected_hash_length,
                          TEST_CHECKPOINT_NUM(9));
        TEST_ASSERT_EQUAL(get_target_hash_length, check1[i].expected_hash_length,
                          TEST_CHECKPOINT_NUM(10));
        /* Check for expected hash for source and target operation */
        TEST_ASSERT_MEMCMP(check1[i].source_hash, check1[i].expected_hash, get_source_hash_length,
                           TEST_CHECKPOINT_NUM(11));
        TEST_ASSERT_MEMCMP(check1[i].target_hash, check1[i].expected_hash, get_target_hash_length,
                           TEST_CHECKPOINT_NUM(12));

        if (valid_test_input_index < 0)
            valid_test_input_index = i;

        /* Abort the hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &source_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &target_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_clone_with_inactive_source_operation(caller_security_t caller __UNUSED)
{
    int32_t                 status;
    psa_hash_operation_t    source_operation;
    psa_hash_operation_t    target_operation;
    size_t                  get_source_hash_length = 0;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    val->print(PRINT_TEST, "[Check %d] Test psa_hash_clone - from an inactive source operation\n",
                                                                             g_test_count++);

    memset(&source_operation, 0, sizeof(source_operation));
    memset(&target_operation, 0, sizeof(target_operation));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &source_operation,
                                  check1[valid_test_input_index].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Add a message fragment to a multipart for source_operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE,
                                  &source_operation,
                                  check1[valid_test_input_index].input,
                                  check1[valid_test_input_index].input_length);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    /* Finish the calculation of the hash of a message for source operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH,
                                  &source_operation,
                                  check1[valid_test_input_index].source_hash,
                                  check1[valid_test_input_index].source_hash_size,
                                  &get_source_hash_length);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    /* Check for expected hash length for source operation */
    TEST_ASSERT_EQUAL(get_source_hash_length, check1[valid_test_input_index].expected_hash_length,
                      TEST_CHECKPOINT_NUM(6));
    /* Check for expected hash for source operation */
    TEST_ASSERT_MEMCMP(check1[valid_test_input_index].source_hash,
                       check1[valid_test_input_index].expected_hash, get_source_hash_length,
                       TEST_CHECKPOINT_NUM(7));

    /* Clone from an inactive source operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(8));

    /* Abort the hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &source_operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_clone_with_active_target_operation(caller_security_t caller __UNUSED)
{
    int32_t                 status;
    psa_hash_operation_t    source_operation;
    psa_hash_operation_t    target_operation;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    val->print(PRINT_TEST, "[Check %d] Test psa_hash_clone - on an active target operation\n",
                                                                             g_test_count++);

    memset(&source_operation, 0, sizeof(source_operation));
    memset(&target_operation, 0, sizeof(target_operation));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &source_operation,
                                  check1[valid_test_input_index].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Clone from an active source operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    /* Try clonig to an active hash operation and expect an error */
    status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(5));

    /* Abort the hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &source_operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

    status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &target_operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

    return VAL_STATUS_SUCCESS;
}
