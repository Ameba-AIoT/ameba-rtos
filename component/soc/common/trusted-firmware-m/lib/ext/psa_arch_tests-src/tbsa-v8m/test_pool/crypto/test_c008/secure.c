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

#define KEY_SIZE  32

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 8),
                  CREATE_TEST_TITLE("An expired key must be revoked to prevent a hack from revealing it"),
                  CREATE_REF_TAG("R030_TBSA_KEY"),
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
    tbsa_status_t   status;
    uint32_t        key[KEY_SIZE], rev_key[KEY_SIZE];
    uint32_t        data, i, instance = 0;
    key_desc_t      *key_info_rev;
    bool_t          key_present = FALSE;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    do {
        status = val->crypto_get_key_info((key_desc_t **)&key_info_rev, REVOKE, &instance);
        if (status != TBSA_STATUS_SUCCESS && key_present == FALSE) {
            val->err_check_set(TEST_CHECKPOINT_2, status);
            return;
        }

        if (status != TBSA_STATUS_SUCCESS)
            return;

        key_present = TRUE;
        status = val->fuse_ops(FUSE_READ, key_info_rev->addr, key, MIN(KEY_SIZE, key_info_rev->size));
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        if (key_info_rev->def_val == 0) {
            data = 0;
            /* Check that if Key is non-zero*/
            for(i = 0; i < MIN(KEY_SIZE, key_info_rev->size); i++)
                data += key[i];

            if (!data) {
                val->print(PRINT_ERROR, "\n\r\tIncorrect key - all bits are zero", 0);
                val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_INCORRECT_VALUE);
                return;
            }
        } else {
            for (i = 0; i < MIN(KEY_SIZE, key_info_rev->size); i++) {
                if (key[i] == key_info_rev->def_val) {
                    val->print(PRINT_ERROR, "\n\r\tIncorrect key - insufficient entropy/unprogrammed word", 0);
                    val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_INCORRECT_VALUE);
                    return;
                }
            }
        }

        status = val->crypto_revoke_key(key_info_rev->index, key_info_rev->addr, key_info_rev->size);
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        status = val->fuse_ops(FUSE_READ, key_info_rev->addr, rev_key, MIN(KEY_SIZE, key_info_rev->size));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        /* Check that if Key is not same after it is revoked*/
        for(i = 0; i < MIN(KEY_SIZE, key_info_rev->size); i++) {
            if(key[i] == rev_key[i]) {
                val->print(PRINT_ERROR, "\n\r\tKey is not revoked", 0);
                val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
                return;
            }
        }

        instance++;
    } while (instance < GET_NUM_INSTANCE(key_info_rev));

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
