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

#ifndef _VAL_INITIAL_ATTESTATION_H_
#define _VAL_INITIAL_ATTESTATION_H_

#ifdef INITIAL_ATTESTATION

#include "val.h"
#include "qcbor.h"

#define COSE_HEADER_PARAM_ALG             1
#define COSE_HEADER_PARAM_KID             4

/**
 * The size of X and Y coordinate in 2 parameter style EC public
 * key. Format is as defined in [COSE (RFC 8152)]
 * (https://tools.ietf.org/html/rfc8152) and [SEC 1: Elliptic Curve
 * Cryptography](http://www.secg.org/sec1-v2.pdf).
 *
 * This size is well-known and documented in public standards.
 */
#define T_COSE_CRYPTO_EC_P256_COORD_SIZE  32
#define T_COSE_CRYPTO_SHA256_SIZE         32

#define MAX_ENCODED_COSE_KEY_SIZE \
   (1 + /* 1 byte to encode map */ \
    2 + /* 2 bytes to encode key type */ \
    2 + /* 2 bytes to encode curve */ \
    2 * /* the X and Y coordinates at 32 bytes each */ \
        (T_COSE_CRYPTO_EC_P256_COORD_SIZE + 1 + 2))

#define USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define MAX_CHALLENGE_SIZE      PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64

#define EAT_CBOR_ARM_RANGE_BASE                 (-75000)
#define EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION   (EAT_CBOR_ARM_RANGE_BASE - 0)
#define EAT_CBOR_ARM_LABEL_CLIENT_ID            (EAT_CBOR_ARM_RANGE_BASE - 1)
#define EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE   (EAT_CBOR_ARM_RANGE_BASE - 2)
#define EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID    (EAT_CBOR_ARM_RANGE_BASE - 3)
#define EAT_CBOR_ARM_LABEL_BOOT_SEED            (EAT_CBOR_ARM_RANGE_BASE - 4)
#define EAT_CBOR_ARM_LABEL_HW_VERSION           (EAT_CBOR_ARM_RANGE_BASE - 5)
#define EAT_CBOR_ARM_LABEL_SW_COMPONENTS        (EAT_CBOR_ARM_RANGE_BASE - 6)
#define EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS     (EAT_CBOR_ARM_RANGE_BASE - 7)
#define EAT_CBOR_ARM_LABEL_NONCE                (EAT_CBOR_ARM_RANGE_BASE - 8)
#define EAT_CBOR_ARM_LABEL_UEID                 (EAT_CBOR_ARM_RANGE_BASE - 9)
#define EAT_CBOR_ARM_LABEL_ORIGINATION          (EAT_CBOR_ARM_RANGE_BASE - 10)

#define CBOR_ARM_TOTAL_CLAIM_INSTANCE           10

#define EAT_CBOR_SW_COMPONENT_TYPE              (1u)
#define EAT_CBOR_SW_COMPONENT_MEASUREMENT       (2u)
#define EAT_CBOR_SW_COMPONENT_EPOCH             (3u)
#define EAT_CBOR_SW_COMPONENT_VERSION           (4u)
#define EAT_CBOR_SW_COMPONENT_SIGNER_ID         (5u)
#define EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC  (6u)

#define MANDATORY_CLAIM_WITH_SW_COMP           (1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NONCE)              |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_UEID)               |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID)  |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_CLIENT_ID)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE) |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_BOOT_SEED)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SW_COMPONENTS))

#define MANDATORY_CLAIM_NO_SW_COMP             (1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NONCE)              |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_UEID)               |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID)  |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_CLIENT_ID)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE) |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_BOOT_SEED)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS))

#define MANDATORY_SW_COMP                      (1 << EAT_CBOR_SW_COMPONENT_MEASUREMENT)

#define VAL_ATTEST_MIN_ERROR                    70

enum attestation_error_code {
    VAL_ATTEST_SUCCESS = 0,
    VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING = VAL_ATTEST_MIN_ERROR,
    VAL_ATTEST_TOKEN_CHALLENGE_MISMATCH,
    VAL_ATTEST_TOKEN_NOT_SUPPORTED,
    VAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS,
    VAL_ATTEST_HASH_LENGTH_MISMATCH,
    VAL_ATTEST_HASH_MISMATCH,
    VAL_ATTEST_HASH_FAIL,
    VAL_ATTEST_HASH_UNSUPPORTED,
    VAL_ATTEST_HASH_BUFFER_SIZE,
    VAL_ATTEST_ERR_PROTECTED_HEADERS,
    VAL_ATTEST_ERR_SIGN_STRUCT,
    VAL_ATTEST_ERR_KEY_FAIL,
    VAL_ATTEST_ERR_SIGNATURE_FAIL,
    VAL_ATTEST_ERR_CBOR_STRUCTURE,
    VAL_ATTEST_ERR_SMALL_BUFFER,
    VAL_ATTEST_ERROR,
};

struct items_to_get_t {
    int64_t label;
    QCBORItem item;
};

enum attestation_function_code {
    VAL_INITIAL_ATTEST_GET_TOKEN        = 0x1,
    VAL_INITIAL_ATTEST_GET_TOKEN_SIZE   = 0x2,
    VAL_INITIAL_ATTEST_VERIFY_TOKEN     = 0x3,
    VAL_INITIAL_ATTEST_COMPUTE_HASH     = 0x4,
    VAL_INITIAL_ATTEST_VERIFY_WITH_PK   = 0x5,
};

int32_t val_initial_attest_verify_token(uint8_t *challenge, size_t challenge_size,
                                        uint8_t *token, size_t token_size);
#endif /* INITIAL_ATTESTATION */

int32_t val_attestation_function(int type, ...);
#endif /* _VAL_INITIAL_ATTESTATION_H_ */
