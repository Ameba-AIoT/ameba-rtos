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

#include "test_crypto_common.h"

#define DERIVATION_INPUT_CNT    3

typedef struct {
    psa_key_derivation_step_t  step;
    const uint8_t             *data;
    size_t                     data_length;
} key_derivation_input_t;

typedef struct {
    char                        test_desc[75];
    psa_key_type_t              type;
    psa_key_usage_t             usage_flags;
    psa_algorithm_t             alg;
    size_t                      capacity;
    const uint8_t              *data;
    size_t                      data_length;
    key_derivation_input_t      derv_inputs[DERIVATION_INPUT_CNT];
    uint8_t                    *output;
    size_t                      output_length;
    psa_status_t                expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_HKDF
{
    .test_desc       = "Test psa_key_derivation_output_bytes - HKDF\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                         .data        = input_salt,
                         .data_length = INPUT_SALT_LEN
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_INFO,
                         .data        = input_info,
                         .data_length = INPUT_INFO_LEN
                        }
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - optional salt\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_INFO,
                         .data        = input_info,
                         .data_length = INPUT_INFO_LEN
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - capacity < output_length\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                         .data        = input_salt,
                         .data_length = INPUT_SALT_LEN
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_INFO,
                         .data        = input_info,
                         .data_length = INPUT_INFO_LEN
                        }
                       },
    .output          = expected_output,
    .output_length   = 43,
    .expected_status = PSA_ERROR_INSUFFICIENT_DATA
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - missing info\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                         .data        = input_salt,
                         .data_length = INPUT_SALT_LEN
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_ERROR_BAD_STATE
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - missing salt/secret/info\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_ERROR_BAD_STATE
},
#endif /* ARCH_TEST_HKDF */

#ifdef ARCH_TEST_TLS12_PRF
{
    .test_desc       = "Test psa_key_derivation_output_bytes - TLS12_PRF\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SEED,
                         .data        = input_seed,
                         .data_length = INPUT_SEED_LEN
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_LABEL,
                         .data        = input_label,
                         .data_length = INPUT_LABEL_LEN
                        },
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - capacity < output_length\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SEED,
                         .data        = input_seed,
                         .data_length = INPUT_SEED_LEN
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = PSA_KEY_DERIVATION_INPUT_LABEL,
                         .data        = input_label,
                         .data_length = INPUT_LABEL_LEN
                        },
                       },
    .output          = expected_output,
    .output_length   = 43,
    .expected_status = PSA_ERROR_INSUFFICIENT_DATA
},

{
    .test_desc       = "Test psa_key_derivation_output_bytes - missing seed/secret/label\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256)),
    .capacity        = 42,
    .data            = input_bytes_data,
    .data_length     = INPUT_BYTES_DATA_LEN,
    .derv_inputs     = {
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                        {
                         .step        = 0,
                         .data        = NULL,
                         .data_length = 0
                        },
                       },
    .output          = expected_output,
    .output_length   = 42,
    .expected_status = PSA_ERROR_BAD_STATE
},
#endif /* ARCH_TEST_TLS12_PRF */
};
