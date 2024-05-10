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
#include "test_c018.h"
#include "test_data.h"

const client_test_t test_c018_crypto_list[] = {
    NULL,
    psa_key_derivation_input_key_test,
    psa_key_derivation_input_key_negative_test,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_key_derivation_input_key_test(caller_security_t caller __UNUSED)
{
    int32_t                        i, status;
    int32_t                        num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_derivation_operation_t operation  = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_attributes_t           attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t                   key;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Set the key data buffer to the input base on algorithm */
    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS,        &attributes, check1[i].bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                 check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        if (check1[i].expected_status == PSA_SUCCESS)
        {
            /* Provide an input for key derivation or key agreement without setup should be
             * failure.
             */
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
                     check1[i].step, key);
            TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(4));
        }

        /* Set up a key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                 check1[i].setup_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Provide an input for key derivation or key agreement */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
                 check1[i].step, key);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(6));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Abort the key derivation operation */
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

            continue;
        }

        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Provide an input for key derivation or key agreement */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
                 check1[i].step, key);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(9));

        /* Abort the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                 check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_NOT_SUPPORTED, TEST_CHECKPOINT_NUM(11));

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_key_derivation_input_key_negative_test(caller_security_t caller __UNUSED)
{
    int32_t                        status;
    psa_key_derivation_operation_t operation;
    psa_key_id_t                   key = 11;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    val->print(PRINT_TEST, "[Check %d] Test psa_cipher_decrypt_setup - Invalid key handle\n",
                                                                               g_test_count++);

    operation = psa_key_derivation_operation_init();

    /* Set up a key derivation operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
             check1[valid_test_input_index].setup_alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Provide an input for key derivation or key agreement */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
             check1[valid_test_input_index].step, key);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));

    /* Abort the key derivation operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    val->print(PRINT_TEST, "[Check %d] Test psa_cipher_decrypt_setup - Zero as key handle\n",
                                                                               g_test_count++);

    operation = psa_key_derivation_operation_init();

    /* Set up a key derivation operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
             check1[valid_test_input_index].setup_alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

    /* Provide an input for key derivation or key agreement */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
             check1[valid_test_input_index].step, 0);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(7));

    /* Abort the key derivation operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

    return VAL_STATUS_SUCCESS;
}

