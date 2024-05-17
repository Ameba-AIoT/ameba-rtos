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

#define KEY_SIZE  32 /* 32*32 = 1024 bits = 1K */

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 5),
                  CREATE_TEST_TITLE("A fuse should only transit to programmed state"),
                  CREATE_REF_TAG("R020_TBSA_FUSE"),
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
    fuse_desc_t     *fuse_desc;
    uint32_t        i;
    uint32_t        rd_data1[KEY_SIZE], rd_data2[KEY_SIZE];
    uint32_t        wr_data1[KEY_SIZE], wr_data2[KEY_SIZE];

    val->memset(rd_data1, 0x0, sizeof(rd_data1)/sizeof(uint32_t));
    val->memset(rd_data2, 0x0, sizeof(rd_data2)/sizeof(uint32_t));
    val->memset(wr_data1, 0x0, sizeof(wr_data1)/sizeof(uint32_t));
    val->memset(wr_data2, 0xF0, sizeof(wr_data2)/sizeof(uint32_t));

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->get_fuse_info((fuse_desc_t **)&fuse_desc, FUSE_OPEN, 0);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    if (fuse_desc->def_val == 0) {
        for (i = 0; i < MIN(KEY_SIZE, fuse_desc->size); i++)
            wr_data1[i] = 0xFFFFFFFF;
    }

    status = val->fuse_ops(FUSE_WRITE, fuse_desc->addr, wr_data1, MIN(KEY_SIZE, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, rd_data1, MIN(KEY_SIZE, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    for (i = 0; i < MIN(KEY_SIZE, fuse_desc->size); i++) {
        if (rd_data1[i] != wr_data1[i]) {
            val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_WRITE_ERROR);
            return;
        }
    }

    status = val->fuse_ops(FUSE_WRITE, fuse_desc->addr, wr_data2, MIN(KEY_SIZE, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, rd_data2, MIN(KEY_SIZE, fuse_desc->size));
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }

    for (i = 0; i < MIN(KEY_SIZE, fuse_desc->size); i++) {
        if (rd_data2[i] != rd_data1[i])
        {
            val->print(PRINT_ERROR, "\n\r\tAble to modify programmed bits", 0);
            val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
            return;
        }
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
