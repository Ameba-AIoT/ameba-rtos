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
#include "test_c022.h"
#include "test_data.h"

const client_test_t test_c022_crypto_list[] = {
    NULL,
    psa_key_derivation_abort_test,
    NULL,
};

int32_t psa_key_derivation_abort_test(caller_security_t caller __UNUSED)
{
    int32_t                         status, i;
    psa_key_derivation_operation_t  func = psa_key_derivation_operation_init();
    psa_key_derivation_operation_t  init = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_derivation_operation_t  zero;

    memset(&zero, 0, sizeof(zero));

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    i = 1;
    val->print(PRINT_TEST, "[Check %d] ", i++);
    val->print(PRINT_TEST, "Test psa_key_derivation_abort\n", 0);

    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &func);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &init);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &zero);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    /* Read some data from a key derivation operation with no data in the operation */
    status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES, &func, expected_output,
             BUFFER_SIZE);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(6));

    return VAL_STATUS_SUCCESS;
}
