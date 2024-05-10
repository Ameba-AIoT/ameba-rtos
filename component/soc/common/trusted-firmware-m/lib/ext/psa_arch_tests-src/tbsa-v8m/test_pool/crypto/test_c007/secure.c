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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 7),
                  CREATE_TEST_TITLE("A confidential hardware IP fuse must not be readable by any software"),
                  CREATE_REF_TAG("R080/R090_TBSA_FUSE"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;
memory_desc_t *memory_desc;

void HF_Handler(void)
{
    /* Issue system warm reset */
    g_val->system_reset(WARM_RESET);

    /* Shouldn't come here */
    while(1);
}

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    tbsa_status_t status;

    g_val  = val;
    status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
    if(val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      i;
    uint32_t      key[KEY_SIZE] = {0};
    fuse_desc_t   *fuse_desc;
    boot_t        boot;
    uint32_t      shcsr = 0UL;

    val->memset(key, 0x0, sizeof(key)/sizeof(uint32_t));

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    if (boot.wb != WARM_BOOT_REQUESTED) {
        status = val->get_fuse_info((fuse_desc_t **)&fuse_desc, FUSE_HW_IP, 0);
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }

        status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }

        boot.wb = WARM_BOOT_REQUESTED;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
        status = val->mem_reg_read(SHCSR, &shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            return;
        }

        status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
        if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
            return;
        }

        status = val->fuse_ops(FUSE_READ, fuse_desc->addr, key, MIN(KEY_SIZE, fuse_desc->size));
        if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
            return;
        }

        for (i = 0; i < MIN(KEY_SIZE, fuse_desc->size); i++) {
            if (key[i] != fuse_desc->def_val) {
                val->print(PRINT_ERROR, "\n\r\tAble to read the confidential fuse", 0);
                val->err_check_set(TEST_CHECKPOINT_C, TBSA_STATUS_ERROR);
                return;
            }
        }
    }

    boot.wb = BOOT_UNKNOWN;
    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
    if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }
}

