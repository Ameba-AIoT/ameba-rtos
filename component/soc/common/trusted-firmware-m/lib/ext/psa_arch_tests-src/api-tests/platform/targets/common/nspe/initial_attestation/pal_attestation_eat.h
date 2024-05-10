/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _PAL_ATTESTATION_EAT_H_
#define _PAL_ATTESTATION_EAT_H_

#include "qcbor.h"
#include "pal_common.h"

#define PAL_ATTEST_MIN_ERROR              30

enum attestation_error_code {
    PAL_ATTEST_SUCCESS = 0,
    PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING = PAL_ATTEST_MIN_ERROR,
    PAL_ATTEST_TOKEN_CHALLENGE_MISMATCH,
    PAL_ATTEST_TOKEN_NOT_SUPPORTED,
    PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS,
    PAL_ATTEST_HASH_LENGTH_MISMATCH,
    PAL_ATTEST_HASH_MISMATCH,
    PAL_ATTEST_HASH_FAIL,
    PAL_ATTEST_HASH_UNSUPPORTED,
    PAL_ATTEST_HASH_BUFFER_SIZE,
    PAL_ATTEST_ERR_PROTECTED_HEADERS,
    PAL_ATTEST_ERR_SIGN_STRUCT,
    PAL_ATTEST_ERR_KEY_FAIL,
    PAL_ATTEST_ERR_SIGNATURE_FAIL,
    PAL_ATTEST_ERR_CBOR_STRUCTURE,
    PAL_ATTEST_ERR_SMALL_BUFFER,
    PAL_ATTEST_ERROR,
};

#endif /* _PAL_ATTESTATION_EAT_H_ */
