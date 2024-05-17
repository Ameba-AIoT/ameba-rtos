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
#include "test_c020.h"
#include "test_data.h"

const client_test_t test_c020_crypto_list[] = {
    NULL,
    psa_key_derivation_output_bytes_test,
    NULL,
};

int32_t psa_key_derivation_output_bytes_test(caller_security_t caller __UNUSED)
{
    int32_t                         status;
    int32_t                         i, inIdx;
    int32_t                         num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t            attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t  operation;
    psa_key_id_t                    key;

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

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Start the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation, check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Set the capacity for the generator */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                                      check1[i].capacity);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Loop through the available steps */
        /* Provide an input for key derivation or key agreement */
        for (inIdx = 0; inIdx < DERIVATION_INPUT_CNT; inIdx++)
        {
            if ((check1[i].derv_inputs[inIdx].step        == 0) &&
                (check1[i].derv_inputs[inIdx].data        == NULL) &&
                (check1[i].derv_inputs[inIdx].data_length == 0))
                continue;

            if (check1[i].derv_inputs[inIdx].step == PSA_KEY_DERIVATION_INPUT_SECRET)
            {
                status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
                                              check1[i].derv_inputs[inIdx].step, key);
                TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));
            } else {
                status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES, &operation,
                                              check1[i].derv_inputs[inIdx].step,
                                              check1[i].derv_inputs[inIdx].data,
                                              check1[i].derv_inputs[inIdx].data_length);
                TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
            }
        }

        /* Read some data from a key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES, &operation,
                                      check1[i].output, check1[i].output_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(8));

        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

        /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        /* Abort the derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

        /* Expect bad state when derivation is called on an aborted operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES, &operation,
                                      check1[i].output, check1[i].output_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(11));
    }

    return VAL_STATUS_SUCCESS;
}
