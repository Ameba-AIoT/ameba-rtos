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
#include "test_c010.h"
#include "test_data.h"

const client_test_t test_c010_crypto_list[] = {
    NULL,
    psa_key_attributes_set_get_test,
    NULL,
};

int32_t psa_key_attributes_set_get_test(caller_security_t caller __UNUSED)
{
    int32_t               i, status;
    psa_key_type_t        get_type;
    psa_key_id_t          get_id;
    psa_key_lifetime_t    get_lifetime;
    psa_key_usage_t       get_usage_flags;
    psa_algorithm_t       get_alg;
    size_t                get_bits;
    int32_t               num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;

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

        /* Check if all the attributes are initialized to zero */
        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &attributes, &get_type);
        TEST_ASSERT_EQUAL(get_type, 0, TEST_CHECKPOINT_NUM(3));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ID, &attributes, &get_id);
        TEST_ASSERT_EQUAL(get_id, 0, TEST_CHECKPOINT_NUM(4));

        val->crypto_function(VAL_CRYPTO_GET_KEY_LIFETIME, &attributes, &get_lifetime);
        TEST_ASSERT_EQUAL(get_lifetime, 0, TEST_CHECKPOINT_NUM(5));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &attributes, &get_usage_flags);
        TEST_ASSERT_EQUAL(get_usage_flags, 0, TEST_CHECKPOINT_NUM(6));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &attributes, &get_alg);
        TEST_ASSERT_EQUAL(get_alg, 0, TEST_CHECKPOINT_NUM(7));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &attributes, &get_bits);
        TEST_ASSERT_EQUAL(get_bits,  0, TEST_CHECKPOINT_NUM(8));

       /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS,        &attributes, check1[i].bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ID,          &attributes, check1[i].id);
        val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME,    &attributes, check1[i].lifetime);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);

        /* Check if all the attributes are as per the input */
        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &attributes, &get_type);
        TEST_ASSERT_EQUAL(get_type, check1[i].type, TEST_CHECKPOINT_NUM(9));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ID, &attributes, &get_id);
        TEST_ASSERT_EQUAL(get_id, check1[i].id, TEST_CHECKPOINT_NUM(10));

        val->crypto_function(VAL_CRYPTO_GET_KEY_LIFETIME, &attributes, &get_lifetime);
        TEST_ASSERT_EQUAL(get_lifetime, check1[i].lifetime, TEST_CHECKPOINT_NUM(11));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &attributes, &get_usage_flags);
        TEST_ASSERT_EQUAL(get_usage_flags, check1[i].usage_flags, TEST_CHECKPOINT_NUM(12));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &attributes, &get_alg);
        TEST_ASSERT_EQUAL(get_alg, check1[i].alg, TEST_CHECKPOINT_NUM(13));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &attributes, &get_bits);
        TEST_ASSERT_EQUAL(get_bits,  check1[i].bits, TEST_CHECKPOINT_NUM(14));

        /* Reset the attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        /* Check if all the attributes are erased */
        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &attributes, &get_type);
        TEST_ASSERT_EQUAL(get_type, 0, TEST_CHECKPOINT_NUM(15));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ID, &attributes, &get_id);
        TEST_ASSERT_EQUAL(get_id, 0, TEST_CHECKPOINT_NUM(16));

        val->crypto_function(VAL_CRYPTO_GET_KEY_LIFETIME, &attributes, &get_lifetime);
        TEST_ASSERT_EQUAL(get_lifetime, 0, TEST_CHECKPOINT_NUM(17));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &attributes, &get_usage_flags);
        TEST_ASSERT_EQUAL(get_usage_flags, 0, TEST_CHECKPOINT_NUM(18));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &attributes, &get_alg);
        TEST_ASSERT_EQUAL(get_alg, 0, TEST_CHECKPOINT_NUM(19));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &attributes, &get_bits);
        TEST_ASSERT_EQUAL(get_bits,  0, TEST_CHECKPOINT_NUM(20));
    }

    return VAL_STATUS_SUCCESS;
}
