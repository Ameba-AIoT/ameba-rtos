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

#include "test_crypto_common.h"

typedef struct {
    char                    test_desc[75];
    uint8_t                *output;
    size_t                  output_size;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
{
    .test_desc       = "Test psa_generate_random to get 0 Byte data\n",
    .output          = expected_output,
    .output_size     = 0,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 16 Byte data\n",
    .output          = expected_output,
    .output_size     = 16,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 24 Byte data\n",
    .output          = expected_output,
    .output_size     = 24,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 32 Byte data\n",
    .output          = expected_output,
    .output_size     = 32,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 64 Byte data\n",
    .output          = expected_output,
    .output_size     = 64,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 128 Byte data\n",
    .output          = expected_output,
    .output_size     = 128,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 256 Byte data\n",
    .output          = expected_output,
    .output_size     = 256,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 512 Byte data\n",
    .output          = expected_output,
    .output_size     = 512,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_generate_random to get 1000 Byte data\n",
    .output          = expected_output,
    .output_size     = 1000,
    .expected_status = PSA_SUCCESS
},
};
