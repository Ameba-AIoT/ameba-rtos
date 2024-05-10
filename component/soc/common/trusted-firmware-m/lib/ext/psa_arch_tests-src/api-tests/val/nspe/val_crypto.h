/** @file
 * Copyright (c) 2018-2021, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_CRYPTO_H_
#define _VAL_CRYPTO_H_

#include "val.h"

#define BYTES_TO_BITS(byte)             (byte * 8)
#define BITS_TO_BYTES(bits)             (((bits) + 7) / 8)

/* Size */
#define AES_16B_KEY_SIZE                16
#define AES_24B_KEY_SIZE                24
#define AES_32B_KEY_SIZE                32
#define AES_18B_KEY_SIZE                18
#define AES_34B_KEY_SIZE                34
#define DES_8B_KEY_SIZE                 8
#define DES3_2B_KEY_SIZE                16
#define DES3_3B_KEY_SIZE                24
#define SIZE_32B                        32
#define SIZE_50B                        50
#define SIZE_128B                       128
#define SIZE_256B                       256
#define SIZE_512B                       512
#define BUFFER_SIZE                     1200
#define HASH_64B                        64
#define BUFFER_SIZE_HIGH                5130

/* Key Slot */
#define INVALID_KEY_SLOT                0xDEAD
#define ZERO_KEY_SLOT                   0
#define OCCUPIED_KEY_SLOT               1
#define MAX_KEY_SLOT                    32

#define PSA_KEY_LIFETIME_INVALID        0xFFFFFFFF
#define PSA_KEY_USAGE_INVALID           0xFFFFFFFF
#define PSA_HASH_ALG_INVALID            0x02FFFFFF
#define PSA_ALG_INVALID                 0xFFFFFFFF
#define PSA_KEY_DERIVATION_STEP_INVALID 0x0000FFFF

#define PSA_ALG_NONE ((psa_algorithm_t)0)
#define PSA_KEY_ID_NULL ((psa_key_id_t)0)
#if 0
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
        ((((aead_alg) & ~0x003f0000) == 0x05400100) ? PSA_ALG_CCM : \
        (((aead_alg) & ~0x003f0000) == 0x05400200) ? PSA_ALG_GCM : \
        (((aead_alg) & ~0x003f0000) == 0x05000500) ? PSA_ALG_CHACHA20_POLY1305 : \
        PSA_ALG_NONE)

#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
		((psa_algorithm_t) (((aead_alg) & ~0x003f0000) | (((tag_length) & 0x3f) << 16)))
#endif

enum crypto_function_code {
    VAL_CRYPTO_AEAD_ABORT                        = 0x1,
    VAL_CRYPTO_AEAD_DECRYPT,
    VAL_CRYPTO_AEAD_DECRYPT_SETUP,
    VAL_CRYPTO_AEAD_ENCRYPT,
    VAL_CRYPTO_AEAD_ENCRYPT_SETUP,
    VAL_CRYPTO_AEAD_FINISH,
    VAL_CRYPTO_AEAD_GENERATE_NONCE,
    VAL_CRYPTO_AEAD_OPERATION_INIT,
    VAL_CRYPTO_AEAD_SET_LENGTHS,
    VAL_CRYPTO_AEAD_SET_NONCE,
    VAL_CRYPTO_AEAD_UPDATE,
    VAL_CRYPTO_AEAD_UPDATE_AD,
    VAL_CRYPTO_AEAD_VERIFY,
    VAL_CRYPTO_ASYMMETRIC_DECRYPT,
    VAL_CRYPTO_ASYMMETRIC_ENCRYPT,
    VAL_CRYPTO_CIPHER_ABORT,
    VAL_CRYPTO_CIPHER_DECRYPT,
    VAL_CRYPTO_CIPHER_DECRYPT_SETUP,
    VAL_CRYPTO_CIPHER_ENCRYPT,
    VAL_CRYPTO_CIPHER_ENCRYPT_SETUP,
    VAL_CRYPTO_CIPHER_FINISH,
    VAL_CRYPTO_CIPHER_GENERATE_IV,
    VAL_CRYPTO_CIPHER_OPERATION_INIT,
    VAL_CRYPTO_CIPHER_SET_IV,
    VAL_CRYPTO_CIPHER_UPDATE,
    VAL_CRYPTO_COPY_KEY,
    VAL_CRYPTO_INIT,
    VAL_CRYPTO_DESTROY_KEY,
    VAL_CRYPTO_EXPORT_KEY,
    VAL_CRYPTO_EXPORT_PUBLIC_KEY,
    VAL_CRYPTO_GENERATE_KEY,
    VAL_CRYPTO_GENERATE_RANDOM,
    VAL_CRYPTO_GET_KEY_ALGORITHM,
    VAL_CRYPTO_GET_KEY_ATTRIBUTES,
    VAL_CRYPTO_GET_KEY_BITS,
    VAL_CRYPTO_GET_KEY_ID,
    VAL_CRYPTO_GET_KEY_LIFETIME,
    VAL_CRYPTO_GET_KEY_TYPE,
    VAL_CRYPTO_GET_KEY_USAGE_FLAGS,
    VAL_CRYPTO_HASH_ABORT,
    VAL_CRYPTO_HASH_CLONE,
    VAL_CRYPTO_HASH_COMPARE,
    VAL_CRYPTO_HASH_COMPUTE,
    VAL_CRYPTO_HASH_FINISH,
    VAL_CRYPTO_HASH_OPERATION_INIT,
    VAL_CRYPTO_HASH_RESUME,
    VAL_CRYPTO_HASH_SETUP,
    VAL_CRYPTO_HASH_SUSPEND,
    VAL_CRYPTO_HASH_UPDATE,
    VAL_CRYPTO_HASH_VERIFY,
    VAL_CRYPTO_IMPORT_KEY,
    VAL_CRYPTO_KEY_ATTRIBUTES_INIT,
    VAL_CRYPTO_KEY_DERIVATION_ABORT,
    VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY,
    VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES,
    VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
    VAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT,
    VAL_CRYPTO_KEY_DERIVATION_OPERATION_INIT,
    VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
    VAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY,
    VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY,
    VAL_CRYPTO_KEY_DERIVATION_SETUP,
    VAL_CRYPTO_MAC_ABORT,
    VAL_CRYPTO_MAC_COMPUTE,
    VAL_CRYPTO_MAC_OPERATION_INIT,
    VAL_CRYPTO_MAC_SIGN_FINISH,
    VAL_CRYPTO_MAC_SIGN_SETUP,
    VAL_CRYPTO_MAC_UPDATE,
    VAL_CRYPTO_MAC_VERIFY,
    VAL_CRYPTO_MAC_VERIFY_FINISH,
    VAL_CRYPTO_MAC_VERIFY_SETUP,
    VAL_CRYPTO_PURGE_KEY,
    VAL_CRYPTO_RAW_KEY_AGREEMENT,
    VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
    VAL_CRYPTO_SET_KEY_ALGORITHM,
    VAL_CRYPTO_SET_KEY_BITS,
    VAL_CRYPTO_SET_KEY_ID,
    VAL_CRYPTO_SET_KEY_LIFETIME,
    VAL_CRYPTO_SET_KEY_TYPE,
    VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
    VAL_CRYPTO_SIGN_HASH,
    VAL_CRYPTO_SIGN_MESSAGE,
    VAL_CRYPTO_VERIFY_HASH,
    VAL_CRYPTO_VERIFY_MESSAGE,
    VAL_CRYPTO_RESET                            = 0xF0,
    VAL_CRYPTO_FREE                             = 0xFE,
};

int32_t val_crypto_function(int type, ...);
#endif /* _VAL_CRYPTO_H_ */
