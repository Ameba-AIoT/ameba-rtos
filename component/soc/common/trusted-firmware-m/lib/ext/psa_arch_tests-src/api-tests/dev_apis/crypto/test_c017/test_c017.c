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
#include "test_c017.h"
#include "test_data.h"

const client_test_t test_c017_crypto_list[] = {
    NULL,
    psa_generate_random_test,
    NULL,
};

static uint8_t changed[BUFFER_SIZE];

int32_t psa_generate_random_test(caller_security_t caller __UNUSED)
{
    int32_t     i, num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t    j, run;
    uint8_t     trail[] = "don't overwrite me";
    int32_t     status;

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

        memset(check1[i].output, 0, BUFFER_SIZE);
        memcpy(check1[i].output + check1[i].output_size, trail, sizeof(trail));

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Run several times, to ensure that every output byte will be
         * nonzero at least once with overwhelming probability
         * (2^(-8*number_of_runs)).
         */
        for (run = 0; run < 10; run++)
        {
            memset(check1[i].output, 0, check1[i].output_size);

            /* Generate random bytes */
            status = val->crypto_function(VAL_CRYPTO_GENERATE_RANDOM, check1[i].output,
                                          check1[i].output_size);
            TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(3));

            /* Check that no more than bytes have been overwritten */
            status = memcmp(check1[i].output + check1[i].output_size, trail, sizeof(trail));
            TEST_ASSERT_EQUAL(status, 0, TEST_CHECKPOINT_NUM(4));

            for (j = 0; j < check1[i].output_size; j++)
            {
                if (check1[i].output[j] != 0)
                    ++changed[j];
            }
        }

        /* Check that every byte was changed to nonzero at least once. This
         * validates that psa_generate_random is overwriting every byte of
         * the output buffer.
         */
        for (j = 0; j < check1[i].output_size; j++)
            TEST_ASSERT_NOT_EQUAL(changed[j], 0, TEST_CHECKPOINT_NUM(5));
    }

    return VAL_STATUS_SUCCESS;
}
