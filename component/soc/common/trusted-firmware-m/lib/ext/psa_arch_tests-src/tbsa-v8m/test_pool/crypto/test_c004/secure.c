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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 4),
                  CREATE_TEST_TITLE("Static key must be in an immutable structure"),
                  CREATE_REF_TAG("R070_TBSA_KEY"),
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
    tbsa_status_t status;
    uint32_t      expected_fuse_type, i, instance = 0;
    uint32_t      key[KEY_SIZE];
    uint32_t      rd_data[KEY_SIZE], wr_data[KEY_SIZE];
    uint32_t      key_mask, result;
    key_desc_t    *key_info_static;
    bool_t        key_present = FALSE;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    val->memset(wr_data, 0xFF, sizeof(wr_data)/sizeof(uint32_t));

    do {
        status = val->crypto_get_key_info(&key_info_static, STATIC, &instance);
        if (status != TBSA_STATUS_SUCCESS && key_present == FALSE) {
            val->print(PRINT_ALWAYS, "\n\r\tStatic key is not available to check", 0);
            val->set_status(RESULT_SKIP(status));
            return;
        }

        if (status != TBSA_STATUS_SUCCESS) {
            return;
        }

        key_present = TRUE;
        expected_fuse_type = FUSE_BULK | FUSE_LOCKABLE;
        if ((key_info_static->type & expected_fuse_type) != expected_fuse_type) {
            val->print(PRINT_ERROR, "\n\r\tFuse type in which static key stored is non-compliant", 0);
            val->print(PRINT_ERROR, "\n\r\tFuse type %x", key_info_static->type);
            val->print(PRINT_ERROR, "\n\r\tExpected Fuse type %x", expected_fuse_type);
            val->err_check_set(TEST_CHECKPOINT_2, TBSA_STATUS_ERROR);
            return;
        }

        if ((key_info_static->state & FUSE_OPEN) == FUSE_OPEN) {
            status = val->fuse_ops(FUSE_READ, key_info_static->addr, key, MIN(KEY_SIZE, key_info_static->size));
            if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                return;
            }

            result   = 0;
            key_mask = 0;
            /* check whether key holds the default value, if so warn ! */
            for (i = 0; i < MIN(KEY_SIZE, key_info_static->size); i++) {
                if (key[i] == key_info_static->def_val) {
                    result |= (1U << i);
                }
                key_mask |= (1U << i);
            }

            if (result == key_mask) {
                val->print(PRINT_ALWAYS, "\n\r\tStatic key having the default value : %X", key_info_static->def_val);
            }

            status = val->fuse_ops(FUSE_WRITE, key_info_static->addr, wr_data, MIN(KEY_SIZE, key_info_static->size));
            if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }

            status = val->fuse_ops(FUSE_READ, key_info_static->addr, rd_data, MIN(KEY_SIZE, key_info_static->size));
            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                return;
            }

            for (i = 0; i < MIN(KEY_SIZE, key_info_static->size); i++) {
                if (key[i] != rd_data[i]) {
                    val->print(PRINT_ERROR, "\n\r\tAble to modify static key", 0);
                    val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_ERROR);
                    return;
                }
            }
        } else {
           val->set_status(RESULT_SKIP(status));
           val->print(PRINT_ALWAYS, "\n\r\tStatic key %d is not open", instance);
        }

        instance++;
    } while (instance < GET_NUM_INSTANCE(key_info_static));

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
