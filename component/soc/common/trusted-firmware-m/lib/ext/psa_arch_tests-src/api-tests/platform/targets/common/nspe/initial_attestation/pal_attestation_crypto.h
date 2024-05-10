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

#ifndef _PAL_ATTESTATION_CRYPTO_H_
#define _PAL_ATTESTATION_CRYPTO_H_

#include "pal_attestation_eat.h"
#include "psa/crypto.h"

int32_t pal_cose_crypto_hash_start(psa_hash_operation_t *psa_hash, int32_t cose_hash_alg_id);
void pal_cose_crypto_hash_update(psa_hash_operation_t *psa_hash,
                                 struct q_useful_buf_c data_to_hash);
int32_t pal_cose_crypto_hash_finish(psa_hash_operation_t *psa_hash,
                                    struct q_useful_buf buffer_to_hold_result,
                                    struct q_useful_buf_c *hash_result);
int pal_create_sha256(struct q_useful_buf_c bytes_to_hash, struct q_useful_buf buffer_for_hash,
                      struct q_useful_buf_c *hash);
uint32_t pal_compute_hash(int32_t cose_alg_id, struct q_useful_buf buffer_for_hash,
                          struct q_useful_buf_c *hash, struct q_useful_buf_c protected_headers,
                          struct q_useful_buf_c payload);
uint32_t pal_crypto_pub_key_verify(int32_t cose_algorithm_id, struct q_useful_buf_c token_hash,
                                   struct q_useful_buf_c signature);
#endif /* _PAL_ATTESTATION_CRYPTO_H_ */
