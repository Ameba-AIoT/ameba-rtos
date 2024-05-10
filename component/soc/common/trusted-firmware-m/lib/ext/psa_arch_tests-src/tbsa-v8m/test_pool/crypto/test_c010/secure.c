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

#define FUSE_SIZE  32

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 10),
                  CREATE_TEST_TITLE("Check blowing fuses when the device has left the manufacturing facility."),
                  CREATE_REF_TAG("R040_TBSA_FUSE"),
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
    uint32_t      crypto_lcs;
    uint32_t      i, index = 0;
    crypto_hdr_t  *crypto_hdr;
    crypto_desc_t *crypto_desc;
    fuse_desc_t   *fuse_desc;
    uint32_t      rd_data[FUSE_SIZE], wr_data[FUSE_SIZE];
    bool_t        secure_crypto_found = FALSE;

    val->memset(wr_data, 0xFF, sizeof(wr_data)/sizeof(uint32_t));

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->fuse_get_lcs(&crypto_lcs);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CRYPTO, 0, 0),
                          (uint8_t **)&crypto_hdr,
                          (uint32_t *)sizeof(crypto_hdr_t));

    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

   while (index < crypto_hdr->num) {
       status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CRYPTO, CRYPTO_CRYPTO, index),
                                       (uint8_t **)&crypto_desc,
                                       (uint32_t *)sizeof(crypto_desc_t));

        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }

       if (crypto_desc->attribute != SECURE_PROGRAMMABLE) {
            index++;
            continue;
       }

       secure_crypto_found = TRUE;

       break;
    }

    if (secure_crypto_found && (!(crypto_desc->lcs == LCS_SEC_ENABLED || crypto_lcs == LCS_SEC_ENABLED))) {
        val->print(PRINT_ERROR, "\n\r\tDevice has not left the manufacturing facility", 0);
        val->set_status(RESULT_SKIP(1));
        return;
    }

    status = val->get_fuse_info((fuse_desc_t **)&fuse_desc, FUSE_USER, 0);
    if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, rd_data, MIN(FUSE_READ, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    if (fuse_desc->def_val != 0) {
         for (i = 0; i < MIN(FUSE_READ, fuse_desc->size); i++)
            wr_data[i] = 0;
   }

    for (i = 0; i < MIN(FUSE_READ, fuse_desc->size); i++) {
        if (rd_data[i] != fuse_desc->def_val) {
            val->print(PRINT_ERROR, "\n\r\tGiven fuse is not empty", 0);
            val->err_check_set(TEST_CHECKPOINT_7, TBSA_STATUS_INVALID);
            return;
        }
    }

    status = val->fuse_ops(FUSE_WRITE, fuse_desc->addr, wr_data, MIN(FUSE_READ, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, rd_data, MIN(FUSE_READ, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
        return;
    }

    for (i = 0; i < MIN(FUSE_READ, fuse_desc->size); i++) {
        if (rd_data[i] != wr_data[i]) {
            val->print(PRINT_ERROR, "\n\r\tNot able to program the fuse", 0);
            val->err_check_set(TEST_CHECKPOINT_A, status);
            return;
        }
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
