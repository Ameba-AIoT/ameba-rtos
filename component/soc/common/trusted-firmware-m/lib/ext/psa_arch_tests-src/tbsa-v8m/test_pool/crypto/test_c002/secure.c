/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_test_common.h"

/* Size in Words*/
#define   TBSA_ROTPK_RSA_SIZE  96
#define   TBSA_ROTPK_ECC_SIZE  8
#define   MAX_HASH_SIZE        16

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 2),
                  CREATE_TEST_TITLE("Check if the device embed a ROTPK information"),
                  CREATE_REF_TAG("R160/R0180/R190/R200_TBSA_KEY"),
                  entry_hook,
                  test_payload,
                  exit_hook);


void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t        status;
    uint32_t             key_valid = 0, i, instance = 0;
    uint32_t             key[TBSA_ROTPK_RSA_SIZE], data1[MAX_HASH_SIZE];
    uint32_t             data2[MAX_HASH_SIZE] = {0xDEADDEAD};
    key_desc_t           *key_desc;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->crypto_get_key_info(&key_desc, ROTPK, &instance);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    switch (key_desc->type & ASM_MSK) {

        case ECC:
            if (key_desc->size >= TBSA_ROTPK_ECC_SIZE) {
                status = val->fuse_ops(FUSE_READ, key_desc->addr, key, MIN(TBSA_ROTPK_RSA_SIZE, key_desc->size));
                if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                    return;
                }

                status = val->crypto_validate_public_key(ECC, key, MIN(TBSA_ROTPK_RSA_SIZE, key_desc->size), key_desc->addr, &key_valid);
                if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                    return;
                }

                if (key_valid)
                    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
                else
                    val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_ZERO_COUNT_ERROR);

            } else {
                val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_INCORRECT_KEY_SIZE);
            }
            break;
        case RSA:
            if (key_desc->size >= TBSA_ROTPK_RSA_SIZE) {
                status = val->fuse_ops(FUSE_READ, key_desc->addr, key, key_desc->size);
                if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                    return;
                }

                status = val->crypto_validate_public_key(RSA, key, key_desc->size, key_desc->addr, &key_valid);
                if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                    return;
                }

                if (key_valid)
                    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
                else
                    val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_ERROR);

            } else
                val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_ERROR);
            break;
        default:
            switch (key_desc->type & SYM_MSK) {
                case HASH:
                    status = val->fuse_ops(FUSE_READ, key_desc->addr, data1, MIN(MAX_HASH_SIZE, key_desc->size));
                    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                        return;
                    }

                    /* Try corrupting the hash by writing some random data.*/
                    val->fuse_ops(FUSE_WRITE, key_desc->addr, data2, MIN(MAX_HASH_SIZE, key_desc->size));

                    status = val->fuse_ops(FUSE_READ, key_desc->addr, data2, MIN(MAX_HASH_SIZE, key_desc->size));
                    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
                        return;
                    }

                    for(i=0; i< MIN(MAX_HASH_SIZE, key_desc->size); i++) {
                        if(data1[i] != data2[i]) {
                            val->err_check_set(TEST_CHECKPOINT_D, TBSA_STATUS_ERROR);
                            val->print(PRINT_ERROR, "\n\r\tHASH of ROTPK was not immutable", 0);
                            return;
                        } else {
                            val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
                        }
                    }
                    break;
                default:
                    val->print(PRINT_ERROR, "\n\r\tNo ROTPK was found in the system", 0);
                    val->err_check_set(TEST_CHECKPOINT_E, TBSA_STATUS_NOT_FOUND);
            }
    }
    return;
}

void exit_hook(tbsa_val_api_t *val)
{
}
