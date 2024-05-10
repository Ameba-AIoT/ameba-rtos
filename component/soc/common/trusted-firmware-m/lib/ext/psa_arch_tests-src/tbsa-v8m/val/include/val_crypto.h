/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#include "val_common.h"
#include "val_target.h"
#include "val_infra.h"

#define ENCRYPT  1
#define DECRYPT  0

typedef enum {
  AES_ECB=1,
  AES_CBC,
  AES_CFB,
  AES_CTR,
  AES_OFB
} aes_cipher_t;

typedef struct {
  uint32_t num;
  uint32_t type;
  uint32_t addr;
  uint32_t size;
  uint32_t index;
} key_info_desc_t;

void          val_crypto_init(addr_t crypto_base_addr);
tbsa_status_t val_crypto_key_generate(uint8_t *key, crypt_t enc_type, uint32_t size);
tbsa_status_t val_crypto_validate_public_key(crypt_t type, uint32_t *key, uint32_t size,
                                             addr_t addr, uint32_t *valid);
tbsa_status_t val_crypto_get_key_info(key_desc_t **key_info_desc, key_type_t key_type,
                                      uint32_t *instance);
tbsa_status_t val_crypto_compute_hash(unsigned char *input, uint32_t ilen, unsigned char *output,
                                      int algo);
tbsa_status_t val_crypto_set_base_addr(dev_attr_t attribute);
tbsa_status_t val_crypto_revoke_key(uint32_t index, addr_t addr, uint32_t size);
tbsa_status_t val_crypto_validate_certificate    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
tbsa_status_t val_crypto_get_uniqueID_from_certificate    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
tbsa_status_t val_crypto_get_dpm_from_key    (addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);
tbsa_status_t val_crypto_get_dpm_from_certificate    (addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);
#endif /* _VAL_CRYPTO_H_ */
