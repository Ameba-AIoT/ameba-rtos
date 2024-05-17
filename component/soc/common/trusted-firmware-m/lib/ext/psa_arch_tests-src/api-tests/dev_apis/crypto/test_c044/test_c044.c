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
#include "test_c044.h"
#include "test_data.h"

const client_test_t test_c044_crypto_list[] = {
    NULL,
    psa_copy_key_test,
    NULL,
};

int32_t psa_copy_key_test(caller_security_t caller __UNUSED)
{
    size_t                get_data_length;
    psa_key_usage_t       get_usage_flags;
    psa_algorithm_t       get_alg;
    size_t                get_bits;
    psa_key_type_t        get_type;
    int32_t               i, num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t               status, export_status;
    psa_key_attributes_t  source_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t  target_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key;
    psa_key_id_t          target_key = 0;

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
        val->print(PRINT_TEST, "[Check %d] ", i+1);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &source_attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &source_attributes, check1[i].alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &source_attributes,
                             check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME,    &source_attributes,
                             check1[i].lifetime);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &source_attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Setup the attributes for the target key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &target_attributes,
                             check1[i].target_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &target_attributes,
                             check1[i].target_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &target_attributes,
                             check1[i].target_usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME,    &target_attributes,
                             check1[i].target_lifetime);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS,        &target_attributes,
                             check1[i].target_bits);

        /* Make a copy of a key */
        status = val->crypto_function(VAL_CRYPTO_COPY_KEY, key, &target_attributes,
                                      &target_key);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        /* Destroy the source key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &source_attributes);
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &target_attributes);

            continue;
        }

        /* Check if the target key attributes is as expected */
        status = val->crypto_function(VAL_CRYPTO_GET_KEY_ATTRIBUTES, target_key,
                                      &target_attributes);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &target_attributes, &get_type);
        TEST_ASSERT_EQUAL(get_type, check1[i].target_type, TEST_CHECKPOINT_NUM(7));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &target_attributes,
                             &get_usage_flags);
        TEST_ASSERT_EQUAL(get_usage_flags, check1[i].expected_usage_flags, TEST_CHECKPOINT_NUM(8));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &target_attributes, &get_alg);
        TEST_ASSERT_EQUAL(get_alg, check1[i].target_alg, TEST_CHECKPOINT_NUM(9));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &target_attributes, &get_bits);
        TEST_ASSERT_EQUAL(get_bits, check1[i].target_bits, TEST_CHECKPOINT_NUM(10));

        /* Export a key in binary format */
        export_status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY, target_key,
                                             check1[i].expected_data, check1[i].expected_data_size,
                                             &get_data_length);

        if (check1[i].expected_usage_flags & PSA_KEY_USAGE_EXPORT)
        {
            /* Check if the target key data is same as source data */
            TEST_ASSERT_EQUAL(export_status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

            TEST_ASSERT_EQUAL(get_data_length, check1[i].expected_data_length,
                              TEST_CHECKPOINT_NUM(12));

            TEST_ASSERT_MEMCMP(check1[i].data, check1[i].expected_data, get_data_length,
                               TEST_CHECKPOINT_NUM(13));
        }

        /* Destroy the target key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, target_key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

        /* Copy on a destroyed source should be an error */
        status = val->crypto_function(VAL_CRYPTO_COPY_KEY, key, &target_attributes,
                                      &target_key);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(14));

        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &source_attributes);
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &target_attributes);
    }

    return VAL_STATUS_SUCCESS;
}
