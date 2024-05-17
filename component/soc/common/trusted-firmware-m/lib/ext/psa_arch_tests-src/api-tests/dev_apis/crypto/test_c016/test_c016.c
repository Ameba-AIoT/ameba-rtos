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
#include "test_c016.h"
#include "test_data.h"

const client_test_t test_c016_crypto_list[] = {
    NULL,
    psa_generate_key_test,
    NULL,
};

int32_t psa_generate_key_test(caller_security_t caller __UNUSED)
{
    int32_t               i, status;
    size_t                expected_data_length;
    psa_key_type_t        get_type;
    psa_key_usage_t       get_usage_flags;
    psa_algorithm_t       get_alg;
    size_t                get_bits;
    int32_t               num_checks     = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes     = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t  get_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key;

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
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS,        &attributes, check1[i].bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);

        /* Generate the key */
        status = val->crypto_function(VAL_CRYPTO_GENERATE_KEY, &attributes, &key);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(3));

        if (check1[i].expected_status != PSA_SUCCESS)
            continue;

        /* Get the attributes of the imported key and check if it matches the given value */
        status = val->crypto_function(VAL_CRYPTO_GET_KEY_ATTRIBUTES, key,
                 &get_attributes);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &get_attributes, &get_type);
        TEST_ASSERT_EQUAL(get_type, check1[i].type, TEST_CHECKPOINT_NUM(5));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &get_attributes, &get_bits);
        TEST_ASSERT_EQUAL(get_bits, check1[i].bits, TEST_CHECKPOINT_NUM(6));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &get_attributes, &get_usage_flags);
        TEST_ASSERT_EQUAL(get_usage_flags, check1[i].usage_flags, TEST_CHECKPOINT_NUM(7));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &get_attributes, &get_alg);
        TEST_ASSERT_EQUAL(get_alg, check1[i].alg, TEST_CHECKPOINT_NUM(8));

        /* Export a key in binary format */
        status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY, key, expected_output,
                                      BUFFER_SIZE, &expected_data_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

        /* Check the attributes of the exported key */
        TEST_ASSERT_RANGE(expected_data_length, check1[i].expected_range[0],
                          check1[i].expected_range[1], TEST_CHECKPOINT_NUM(10));

        /* Reset the attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &get_attributes);

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));
    }

    return VAL_STATUS_SUCCESS;
}
