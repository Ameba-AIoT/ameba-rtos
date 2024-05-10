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
#include "test_c043.h"
#include "test_data.h"

const client_test_t test_c043_crypto_list[] = {
    NULL,
    psa_raw_key_agreement_test,
    psa_raw_key_agreement_negative_test,
    NULL,
};

extern  uint32_t g_test_count;

static uint8_t     output[SIZE_50B];

int32_t psa_raw_key_agreement_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    size_t                  output_length;
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

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                 check1[i].key_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_RAW_KEY_AGREEMENT, check1[i].key_alg,
                    key, check1[i].peer_key, check1[i].peer_key_length,
                    output, check1[i].output_size, &output_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Destroy a key and restore the slot to its default state */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));
            continue;
        }

        TEST_ASSERT_EQUAL(output_length, check1[i].expected_output_length, TEST_CHECKPOINT_NUM(6));
        TEST_ASSERT_MEMCMP(output, check1[i].expected_output, output_length,
        TEST_CHECKPOINT_NUM(7));

        /* Destroy a key and restore the slot to its default state */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_raw_key_agreement_negative_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check2)/sizeof(check2[0]);
    int32_t                 i, status;
    size_t                  output_length;
    psa_key_id_t            key = 8;

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));


        val->print(PRINT_TEST, "[Check %d] Test psa_raw_key_agreement - Invalid key handle\n",
                                                                                 g_test_count++);
        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_RAW_KEY_AGREEMENT, check2[i].key_alg,
                    key, check2[i].peer_key, check2[i].peer_key_length,
                    output, check2[i].output_size, &output_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(3));

        val->print(PRINT_TEST, "[Check %d] Test psa_raw_key_agreement - Zero as key handle\n",
                                                                                 g_test_count++);

        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_RAW_KEY_AGREEMENT, check2[i].key_alg,
                    0, check2[i].peer_key, check2[i].peer_key_length,
                    output, check2[i].output_size, &output_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));
    }

    return VAL_STATUS_SUCCESS;
}
