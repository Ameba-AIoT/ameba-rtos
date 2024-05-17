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
    const unsigned char    *iv;
    size_t                  iv_length;
    const uint8_t          *input;
    size_t                  input_length;
    unsigned char          *output;
    size_t                  output_size;
    const unsigned char    *expected_output;
    size_t                  expected_output_length;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CBC_NO_PADDING
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CBC_NO_PADDING\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_1,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CBC_NO_PADDING (Short in)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 5,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_1,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_CBC_PKCS7
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CBC_PKCS7\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_PKCS7,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_1,
    .expected_output_length = 32,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CBC_PKCS7 (Short input)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_PKCS7,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 15,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_6,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CTR\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CTR,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_2,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Encrypt - AES CTR (short input)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CTR,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 15,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_2,
    .expected_output_length = 15,
    .expected_status        = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - DES CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES_8B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = plaintext,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_3,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - 2-key 3DE -CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES3_2B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = plaintext,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_4,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - 3-key 3DE -CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES3_3B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = plaintext,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = ciphertext_5,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CBC_PKCS7
{
    .test_desc              = "Test psa_cipher_finish - Encrypt - small output buffer size\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_ENCRYPT,
    .alg                    = PSA_ALG_CBC_PKCS7,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = plaintext,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = 20,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_BUFFER_TOO_SMALL
},
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
{
    .test_desc              = "Test psa_cipher_finish - Decrypt - AES CBC_NO_PADDING\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_1,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Decrypt - AES CBC_NO_PADDING (Short in)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_1,
    .input_length           = 5,
    .output                 = NULL,
    .output_size            = 0,
    .expected_output        = NULL,
    .expected_output_length = 0,
    .expected_status        = PSA_ERROR_INVALID_ARGUMENT
},

#endif

#ifdef ARCH_TEST_CBC_PKCS7
{
    .test_desc              = "Test psa_cipher_update - Decrypt - AES CBC_PKCS7\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_PKCS7,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_1,
    .input_length           = 32,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Decrypt - AES CBC_PKCS7 (Short input)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_PKCS7,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_6,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 15,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
{
    .test_desc              = "Test psa_cipher_finish - Decrypt - AES CTR\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CTR,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_2,
    .input_length           = 16,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 16,
    .expected_status        = PSA_SUCCESS
},

{
    .test_desc              = "Test psa_cipher_finish - Decrypt - AES CTR (short input)\n",
    .type                   = PSA_KEY_TYPE_AES,
    .data                   = key_data,
    .data_length            = AES_16B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CTR,
    .iv                     = iv,
    .iv_length              = 16,
    .input                  = ciphertext_2,
    .input_length           = 15,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 15,
    .expected_status        = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CBC_NO_PADDING
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc              = "Test psa_cipher_finish - Decrypt - DES CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES_8B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = ciphertext_3,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc              = "Test psa_cipher_finish - Decrypt - 2-key 3DE -CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES3_2B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = ciphertext_4,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc              = "Test psa_cipher_finish - Decrypt - 3-key 3DE -CBC (nopad)\n",
    .type                   = PSA_KEY_TYPE_DES,
    .data                   = key_data,
    .data_length            = DES3_3B_KEY_SIZE,
    .usage_flags            = PSA_KEY_USAGE_DECRYPT,
    .alg                    = PSA_ALG_CBC_NO_PADDING,
    .iv                     = iv,
    .iv_length              = 8,
    .input                  = ciphertext_5,
    .input_length           = 8,
    .output                 = expected_output,
    .output_size            = BUFFER_SIZE,
    .expected_output        = plaintext,
    .expected_output_length = 8,
    .expected_status        = PSA_SUCCESS
},
#endif
#endif
};
