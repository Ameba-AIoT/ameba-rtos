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
#include "test_c038.h"
#include "test_data.h"

const client_test_t test_c038_crypto_list[] = {
    NULL,
    psa_cipher_abort_test,
    psa_cipher_abort_before_update_test,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_cipher_abort_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[valid_test_input_index]);
    int32_t                 i, status;
    psa_cipher_operation_t  operation;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t            key;

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

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        if (check1[i].usage_flags == PSA_KEY_USAGE_ENCRYPT)
        {
            /* Set the key for a multipart symmetric encryption operation */
            status = val->crypto_function(VAL_CRYPTO_CIPHER_ENCRYPT_SETUP, &operation, key,
                                          check1[i].alg);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));
        } else if (check1[i].usage_flags == PSA_KEY_USAGE_DECRYPT)
        {
            /* Set the key for a multipart symmetric decryption operation */
            status = val->crypto_function(VAL_CRYPTO_CIPHER_DECRYPT_SETUP, &operation, key,
                                          check1[i].alg);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));
        }

        /* Abort a cipher operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Multiple abort cipher operation should return success*/
        status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_cipher_abort_before_update_test(caller_security_t caller __UNUSED)
{
    size_t                  get_output_length;
    psa_key_id_t            key;
    psa_cipher_operation_t  operation;
    int32_t                 status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "Test psa_cipher_update after psa_cipher_abort should fail\n", 0);
    memset(&operation, 0, sizeof(operation));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Setup the attributes for the key */
    val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                         &attributes, check1[valid_test_input_index].type);
    val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                         &attributes, check1[valid_test_input_index].alg);
    val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                         &attributes, check1[valid_test_input_index].usage_flags);

    /* Import the key data into the key slot */
    status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                  &attributes, check1[valid_test_input_index].data,
                                  check1[valid_test_input_index].data_length, &key);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Set the key for a multipart symmetric encryption operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_ENCRYPT_SETUP, &operation, key,
                                  check1[valid_test_input_index].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    /* Set an IV for a symmetric encryption operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_SET_IV, &operation, iv, 16);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    /* Abort a cipher operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

    /* Encrypt or decrypt a message fragment in an active cipher operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_UPDATE, &operation, plaintext, 16,
                                  expected_output, BUFFER_SIZE, &get_output_length);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(7));

    /* Destroy the key */
    status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

    /* Reset the key attributes and check if psa_import_key fails */
    val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

    return VAL_STATUS_SUCCESS;
}
