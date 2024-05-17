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
#include "test_c023.h"
#include "test_data.h"

const client_test_t test_c023_crypto_list[] = {
    NULL,
    psa_key_derivation_set_get_capacity_test,
    NULL,
};

int32_t psa_key_derivation_set_get_capacity_test(caller_security_t caller __UNUSED)
{
    int32_t                        num_checks   = sizeof(check1)/sizeof(check1[0]);
    int32_t                        i, status;
    size_t                         get_capacity = 0;
    psa_key_derivation_operation_t operation;

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
        val->print(PRINT_TEST, "[Check %d] ", i+1);
        val->print(PRINT_TEST, check1[i].test_desc, 0);
        memset(&operation, 0, sizeof(operation));

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Start the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                                      check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Set the capacity for the generator */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                                      check1[i].capacity);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));
            continue;
        }

        /* Get the capacity for the generator */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY, &operation,
                                      &get_capacity);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Check if the capacity as per the expected value */
        TEST_ASSERT_EQUAL(get_capacity, check1[i].capacity, TEST_CHECKPOINT_NUM(7));

        /* Abort the operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Setting the capacity on an aborted operation should be an error */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                                      check1[i].capacity);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(9));

        /* Getting the capacity on an aborted operation should be an error */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY, &operation,
                                      &get_capacity);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(10));

    }

    /* When the capacity is larger than the operation's current capacity,
       the operation object remains valid and its capacity remains unchanged */
    get_capacity = 0;
    memset(&operation, 0, sizeof(operation));
    val->print(PRINT_TEST, "[Check %d] ", i+1);
    val->print(PRINT_TEST, "Test psa_key_derivation_set_get_capacity - unchanged capacity\n", 0);
    /* Start the key derivation operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation, check1[0].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

    /* Set the capacity for the generator */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                                  check1[0].capacity);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

    /* Try to set capacity larger than the operation's capacity */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                                  check1[2].capacity);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(13));

    /* Get the capacity for the generator */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY, &operation,
                                  &get_capacity);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

    /* Check if the previous capacity remains unchanged */
    TEST_ASSERT_EQUAL(get_capacity, check1[0].capacity, TEST_CHECKPOINT_NUM(15));

    /* Abort the operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

    return VAL_STATUS_SUCCESS;
}
