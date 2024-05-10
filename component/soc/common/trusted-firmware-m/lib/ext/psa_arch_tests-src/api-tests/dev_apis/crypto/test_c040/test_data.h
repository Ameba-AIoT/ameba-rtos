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

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    const uint8_t          *salt;
    size_t                  salt_length;
    const uint8_t          *input;
    size_t                  input_length;
    uint8_t                *output;
    size_t                  output_size;
    const uint8_t          *expected_output;
    size_t                  expected_output_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_RSA_1024
#ifdef ARCH_TEST_RSA_PKCS1V15_CRYPT
{
    .test_desc              = "Test psa_asymmetric_decrypt - RSA KEY_PAIR PKCS1V15\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = ciphertext_7,
    .input_length           = 128,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 22,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_RSA_OAEP
{
    .test_desc              = "Test psa_asymmetric_decrypt - RSA KEY_PAIR OAEP SHA256\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),
    .input                  = ciphertext_8,
    .input_length           = 128,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 22,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_asymmetric_decrypt - RSA KEY_PAIR OAEP SHA256 with label\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),
    .input                  = ciphertext_9,
    .input_length           = 128,
    .salt                   = salt,
    .salt_length            = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 22,
    .expected_status        = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_CRYPT
{
    .test_desc              = "Test psa_asymmetric_decrypt - Invalid key type (RSA public key)\n",
    .type                   = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data                   = rsa_128_key_data,
    .data_length            = 140,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = NULL,
    .input_length           = 0,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc              = "Test psa_asymmetric_decrypt - Small output buffer\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = ciphertext_7,
    .input_length           = 128,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = expected_output,
    .output_size            = 21,
    .expected_output        = plaintext,
    .expected_output_length = 22,
    .expected_status        = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc              = "Test psa_asymmetric_decrypt - Invalid algorithm\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_SHA_256,
    .input                  = NULL,
    .input_length           = 0,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_CRYPT
#ifdef ARCH_TEST_AES_128
{
    .test_desc              = "Test psa_asymmetric_decrypt - Invalid key type (AES Key)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = NULL,
    .input_length           = 0,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_RSA_1024
{
    .test_desc              = "Test psa_asymmetric_decrypt - Invalid usage\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = NULL,
    .input_length           = 0,
    .salt                   = NULL,
    .salt_length            = 0,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc              = "Test psa_asymmetric_decrypt - RSA PKCS1V15 - Salt\n",
    .type                   = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data                   = rsa_128_key_pair,
    .data_length            = 610,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_RSA_PKCS1V15_CRYPT,
    .input                  = NULL,
    .input_length           = 0,
    .salt                   = salt,
    .salt_length            = 16,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
};
