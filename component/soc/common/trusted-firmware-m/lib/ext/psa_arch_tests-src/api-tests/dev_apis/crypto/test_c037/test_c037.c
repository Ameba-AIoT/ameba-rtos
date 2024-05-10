/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c037.h"
#include "test_data.h"

const client_test_t test_c037_crypto_list[] = {
    NULL,
    psa_cipher_finish_test,
    NULL,
};

int32_t psa_cipher_finish_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    size_t                  get_update_output_length, get_finish_output_length;
    psa_cipher_operation_t  operation, invalid_operation;
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
        val->print(PRINT_TEST, "[Check %d] ", i+1);
        val->print(PRINT_TEST, check1[i].test_desc, 0);
        memset(&operation, 0, sizeof(operation));
        memset(&invalid_operation, 0, sizeof(invalid_operation));

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

        /* Set an IV for a symmetric encryption operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_SET_IV, &operation, check1[i].iv,
                                      check1[i].iv_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Encrypt or decrypt a message fragment in an active cipher operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_UPDATE, &operation, check1[i].input,
                                      check1[i].input_length, check1[i].output,
                                      check1[i].output_size, &get_update_output_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        /* Finish encrypting or decrypting a message in a cipher operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_FINISH, &operation,
                                      check1[i].output + get_update_output_length,
                                      check1[i].output_size - get_update_output_length,
                                      &get_finish_output_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(8));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Abort a cipher operation */
            status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));
            continue;
        }

        /* Check if the output length matches the expected length */
        TEST_ASSERT_EQUAL(get_update_output_length + get_finish_output_length,
                          check1[i].expected_output_length,
                          TEST_CHECKPOINT_NUM(11));

        /* Check if the output data matches the expected data */
        TEST_ASSERT_MEMCMP(check1[i].output, check1[i].expected_output,
                           (get_update_output_length + get_finish_output_length),
                           TEST_CHECKPOINT_NUM(12));

        /* Finish encrypting or decrypting a message using an invalid operation should fail */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_FINISH, &invalid_operation,
                                      check1[i].output + get_update_output_length,
                                      check1[i].output_size - get_update_output_length,
                                      &get_finish_output_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(13));

        /* Abort a cipher operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

        /* Abort a cipher operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &invalid_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(15));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_NOT_SUPPORTED, TEST_CHECKPOINT_NUM(3));
    }

    return VAL_STATUS_SUCCESS;
}
