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


#include "pal_attestation_intf.h"

/**
    @brief    - This API will call the requested attestation function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
int32_t pal_attestation_function(int type, va_list valist)
{
    uint8_t                *challenge, *token;
    size_t                  challenge_size, *token_size, token_buffer_size;
    int32_t                 cose_algorithm_id;
    struct q_useful_buf     buffer_for_hash;
    struct q_useful_buf_c  *hash, payload, protected_headers, token_hash, signature;

    switch (type)
    {
        case PAL_INITIAL_ATTEST_GET_TOKEN:
            challenge = va_arg(valist, uint8_t*);
            challenge_size = va_arg(valist, size_t);
            token = va_arg(valist, uint8_t*);
            token_buffer_size = va_arg(valist, size_t);
            token_size = va_arg(valist, size_t*);
            return psa_initial_attest_get_token(challenge, challenge_size, token, token_buffer_size,
            token_size);
        case PAL_INITIAL_ATTEST_GET_TOKEN_SIZE:
            challenge_size = va_arg(valist, size_t);
            token_size = va_arg(valist, size_t*);
            return psa_initial_attest_get_token_size(challenge_size, token_size);
        case PAL_INITIAL_ATTEST_COMPUTE_HASH:
            cose_algorithm_id = va_arg(valist, int32_t);
            buffer_for_hash = va_arg(valist, struct q_useful_buf);
            hash = va_arg(valist, struct q_useful_buf_c*);
            protected_headers = va_arg(valist, struct q_useful_buf_c);
            payload = va_arg(valist, struct q_useful_buf_c);
            return pal_compute_hash(cose_algorithm_id, buffer_for_hash, hash,
                                    protected_headers, payload);
        case PAL_INITIAL_ATTEST_VERIFY_WITH_PK:
            cose_algorithm_id = va_arg(valist, int32_t);
            token_hash = va_arg(valist, struct q_useful_buf_c);
            signature = va_arg(valist, struct q_useful_buf_c);
            return pal_crypto_pub_key_verify(cose_algorithm_id, token_hash, signature);
        default:
            return PAL_STATUS_UNSUPPORTED_FUNC;
    }
}
