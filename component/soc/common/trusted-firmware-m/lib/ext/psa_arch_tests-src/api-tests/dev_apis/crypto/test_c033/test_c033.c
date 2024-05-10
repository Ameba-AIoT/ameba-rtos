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
#include "test_c033.h"
#include "test_data.h"

const client_test_t test_c033_crypto_list[] = {
    NULL,
    psa_cipher_decrypt_setup_test,
    psa_cipher_decrypt_setup_negative_test,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_cipher_decrypt_setup_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_cipher_operation_t  operation;
    psa_key_id_t            key;

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
        val->crypto_function(VAL_CRYPTO_CIPHER_OPERATION_INIT, &operation);

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

        /* Set the key for a multipart symmetric decryption operation */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_DECRYPT_SETUP, &operation, key,
                                      check1[i].alg);
	    /* TBD Note:- these changes are done because of the spec bug : https://github.com/ARMmbed/mbedtls/pull/4582
		 *        revisit the 1.0.x spec is out
		 */
		if (PSA_ERROR_NOT_SUPPORTED == check1[i].expected_status ||
		     PSA_ERROR_INVALID_ARGUMENT == check1[i].expected_status)
		{
			TEST_ASSERT_DUAL(status, PSA_ERROR_NOT_SUPPORTED,
                 			 PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(4));
		} else
		{
          TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));
		}

        /* Whether setup succeeded or failed, abort must succeed.
         * Abort a cipher operation
         */
        status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            status = val->crypto_function(VAL_CRYPTO_CIPHER_DECRYPT_SETUP, &operation, key,
                                          check1[i].alg);
	       /* TBD Note:- these changes are done because of the spec bug : https://github.com/ARMmbed/mbedtls/pull/4582
		    *        revisit the 1.0.x spec is out
		    */
            if (PSA_ERROR_NOT_SUPPORTED == check1[i].expected_status ||
  			     PSA_ERROR_INVALID_ARGUMENT == check1[i].expected_status)
			{
				TEST_ASSERT_DUAL(status, PSA_ERROR_NOT_SUPPORTED, \
                                 PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(6));
			} else
			{
                TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(6));
			}

            /* Abort a cipher operation */
            status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
        }

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

         /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
	    /* TBD Note:- these changes are done because of the spec bug : https://github.com/ARMmbed/mbedtls/pull/4582
		 *        revisit the 1.0.x spec is out
		 */
        TEST_ASSERT_DUAL(status, PSA_ERROR_NOT_SUPPORTED,
 		                 PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(9));

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_cipher_decrypt_setup_negative_test(caller_security_t caller __UNUSED)
{
    int32_t                 status;
    psa_cipher_operation_t  operation;
    psa_key_id_t            key = 16;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    memset(&operation, 0, sizeof(operation));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    val->print(PRINT_TEST, "[Check %d] Test psa_cipher_decrypt_setup - Invalid key handle\n",
                                                                               g_test_count++);
    /* Set the key for a multipart symmetric decryption operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_DECRYPT_SETUP, &operation, key,
                                  check1[valid_test_input_index].alg);
#ifdef TGT_DEV_APIS_TFM_AMEBADPLUS
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(3));
#else
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(3));
#endif

    val->print(PRINT_TEST, "[Check %d] Test psa_cipher_decrypt_setup - Zero as key handle\n",
                                                                               g_test_count++);

    memset(&operation, 0, sizeof(operation));

    /* Set the key for a multipart symmetric decryption operation */
    status = val->crypto_function(VAL_CRYPTO_CIPHER_DECRYPT_SETUP, &operation, 0,
                                  check1[valid_test_input_index].alg);
#ifdef TGT_DEV_APIS_TFM_AMEBADPLUS
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_ARGUMENT, TEST_CHECKPOINT_NUM(4));
#else
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));
#endif

    status = val->crypto_function(VAL_CRYPTO_CIPHER_ABORT, &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    return VAL_STATUS_SUCCESS;
}

